#include "server.h"
#include "enums.h"
#include "names.h"
#include "shm.h"
#include "syn_game.h"
#include "pipe.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

char* add_suffix(const char* name, const char* suffix) {
    int name_len = strlen(name);
    int suffix_len = strlen(suffix);
    char* result = malloc((name_len + suffix_len + 2) * sizeof(char));
    strcpy(result, name);
    result[name_len] = '-';
    strcpy(result + name_len + 1, suffix);
    return result;
}

void clear_names(shared_names* names) {
    free(names->shm_name_);
    free(names->mut_pc_);
}

int find_client(ServerData *sd, const char* name) {
    for (int i = 0; i < sd->clientCount; i++) {
        if (sd->clients[i].active && strcmp(sd->clients[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int add_client(ServerData *sd, const char* name) {
    int idx = find_client(sd, name);
    if (idx >= 0) {
        return idx;
    }
    if (sd->clientCount >= MAX_CLIENTS) {
        return -1;
    }

    Client *client = &sd->clients[sd->clientCount];
    client->active = true;
    strncpy(client->name, name, BUFFER_SIZE - 1);
    client->name[BUFFER_SIZE - 1] = '\0';

    player_init(&client->player_, sd->clientCount, name);

    snprintf(client->pipe_path, sizeof(client->pipe_path), "client_%s", name);

    
    int cfd = pipe_open_write(client->pipe_path);
    client->fd = cfd;

    int newIndex = sd->clientCount;
    sd->clientCount++;

    if (sd->activeIndex < 0) {
        sd->activeIndex = newIndex;
    }
    sd->clients[sd->activeIndex].performedAction = 0;
    printf("[SERVER] New client '%s' (index=%d) joined.\n", name, newIndex);
    fflush(stdout);

    return newIndex;
}


void remove_client(ServerData *sd, int idx) {
    if (idx < 0 || idx >= sd->clientCount) {
        return;
    }
    pipe_close(sd->clients[idx].fd);
    //pipe_destroy(sd->clients[idx].pipe_path);

    sd->clients[idx].active = false;
    
    sd->clientCount--;
    if (idx != sd->clientCount) {
        sd->clients[idx] = sd->clients[sd->clientCount];
    }

    if (sd->activeIndex == sd->clientCount) {
        sd->activeIndex = idx;
    } 
    else if (idx < sd->activeIndex) {
        sd->activeIndex--;
    }

    if (sd->clientCount <= 0) {
        sd->activeIndex = -1;
        return;
    }

    if (idx == sd->activeIndex) {
        sd->activeIndex = (sd->activeIndex + 1) % sd->clientCount;
    }
}

void broadcast_msg(ServerData *sd, const char* msg) {
    for (int i = 0; i < sd->clientCount; /* i++ v cykle */ ) {
        if (!sd->clients[i].active) {
            i++;
            continue;
        }
        if (get_active_player(sd) != &sd->clients[i].player_) {
            ssize_t w = write(sd->clients[i].fd, msg, strlen(msg));
            if (w < 0) {
                if (errno == EPIPE) {
                    printf("[SERVER] Detected client '%s' left (EPIPE on broadcast).\n", sd->clients[i].name);
                    fflush(stdout);

                    char bc[BUFFER_SIZE*2];
                    snprintf(bc, sizeof(bc), "[BCAST] Player '%s' disconnected.\n", sd->clients[i].name);
                    remove_client(sd, i);
                    broadcast_msg(sd, bc);
                    // remove_client() presunie i na koniec ==> nerobime i++
                    continue;
                } else {
                    perror("[SERVER] write(broadcast).\n");
                }
            }
        }
        i++;
    }
}


void send_to_index(ServerData *sd, int idx, const char* msg) {
    if (idx < 0 || idx >= sd->clientCount) return;
    if (!sd->clients[idx].active) return;

    ssize_t w = write(sd->clients[idx].fd, msg, strlen(msg));
    if (w < 0) {
        if (errno == EPIPE) {
            printf("[SERVER] Detected client '%s' left (EPIPE on send).\n", sd->clients[idx].name);
            fflush(stdout);

            char bc[BUFFER_SIZE*2];
            snprintf(bc, sizeof(bc), "[BCAST] Player '%s' disconnected.\n", sd->clients[idx].name);

            remove_client(sd, idx);
            broadcast_msg(sd, bc);
        } else {
            perror("[SERVER] write(send_to_index).\n");
        }
    }
}

void next_turn(ServerData *sd) {
    if (sd->clientCount <= 0) {
        sd->activeIndex = -1;
        return;
    }
    syn_shm_game_end_of_turn_animal_multiplication(sd->syn_game_->game_, get_active_player(sd));
    sd->activeIndex = (sd->activeIndex + 1) % sd->clientCount;
}

const char* get_active_name(ServerData *sd) {
    if (sd->activeIndex < 0 || sd->activeIndex >= sd->clientCount) {
        return "none";
    }
    return sd->clients[sd->activeIndex].name;
}

int get_animal_type(const char *animalName) {
    for (int i = 0; i < ANIMAL_COUNT; i++) {
        if (strcmp(animalNames[i], animalName) == 0) {
            return i;
        }
    }
    return -1;
}

void perform_exchange(ServerData *sd, const char *animalIn, const char *animalOut, char * output) {
    int inType = get_animal_type(animalIn);
    int outType = get_animal_type(animalOut);

    if (inType == -1 || outType == -1) {
        snprintf(output, BUFFER_SIZE, "Invalid animal names provided.\n");
        return;
    }
    if ((inType - outType == 1 || inType - outType == -1) || (inType == 1 && outType == 5 || inType == 3 && outType == 6)) {

        if (inType == 1 && outType == 5 && get_active_player(sd)->playerAnimals[SMALL_DOG] != 0) {
            snprintf(output, BUFFER_SIZE, "Cannot have more than one small dog.\n");
            return;
        } else if (inType == 3 && outType == 6 && get_active_player(sd)->playerAnimals[BIG_DOG] != 0) {
            snprintf(output, BUFFER_SIZE, "Cannot have more than one big dog.\n");
            return;
        }

        _Bool success = syn_shm_game_exchange_animal(&sd->syn_game, get_active_player(sd), inType, outType);
        if (success) {
            snprintf(output, BUFFER_SIZE, "Animals have been succesfuly changed\n");
        }
        else {
            snprintf(output, BUFFER_SIZE, "There was an error while exchanging animals.\n");
        }
    } else {
        snprintf(output, BUFFER_SIZE, "Invalid exchange.\n");
    }
}

void print_player_inventory(ServerData* sd, int playerIndex, char* output) {
    int* inventory = syn_inventory_look(sd, playerIndex);
    int offset = 0; // Keeps track of the current position in the output buffer
    if (get_active_player(sd)->playerIndex == playerIndex) {
        offset += snprintf(output + offset, BUFFER_SIZE*2 - offset, "You own these animals:\n");
    } else {
        offset += snprintf(output + offset, BUFFER_SIZE*2 - offset, "Player %s owns these animals:\n", sd->clients[playerIndex].name);
    }

    // Append animal counts
    for (int i = 0; i < FOX; i++) {
        offset += snprintf(output + offset, BUFFER_SIZE*2 - offset, "%s: %d\n", animalNames[i], inventory[i]);
    }
}

void print_shop_prices(game* game, char* output) {
    int* prices = view_shop(game)[1];
    int offset = 0;
    offset += snprintf(output + offset, BUFFER_SIZE*2 - offset, "Shop prices for animal exchange (Both ways):\n");
    
    for (int i = 0; i < 4; i++) {
        offset += snprintf(output + offset, BUFFER_SIZE*2 - offset, "%d %s for %d %s", prices[i], animalNames[i], 1, animalNames[i]);
    }
    offset += snprintf(output + offset, BUFFER_SIZE*2 - offset, "%d %s for %d %s", prices[4], animalNames[SHEEP], 1, animalNames[SMALL_DOG]);
    offset += snprintf(output + offset, BUFFER_SIZE*2 - offset, "%d %s for %d %s", prices[5], animalNames[COW], 1, animalNames[BIG_DOG]);

}

void print_shop_inventory(game* game, char* output) {
    int* inventory = view_shop(game)[0];
    int offset = 0;
    offset += snprintf(output + offset, BUFFER_SIZE*2 - offset, "Animals available in shop:\n");

    for (int i = 0; i < FOX; i++) {
        offset += snprintf(output + offset, BUFFER_SIZE*2 - offset, "%s: %d\n", animalNames[i], inventory[i]);
    }
}

int check_action_count(ServerData *sd, int index) { //returns 1 if player can use roll command
    if(sd->clients[sd->activeIndex].performedAction == 1) {
        char noActionMsg[BUFFER_SIZE*2];
        snprintf(noActionMsg, sizeof(noActionMsg), "[SERVER] You already used roll this turn.\n");
        send_to_index(sd, index, noActionMsg);
        return 0;
        }
    sd->clients[sd->activeIndex].performedAction = 1;
    return 1;
}

bool check_victory(ServerData *sd) {
    for (int i = 0; i < SMALL_DOG; i++) {
        if (get_active_player(sd)->playerAnimals[i] == 0) {
            return false;
        }
    }
    return true;
}

player* get_active_player(ServerData *sd) {
    return &sd->clients[sd->activeIndex].player_;
}

int server_main(int requiredNumberOfPlayers) 
{
    shared_names names;
    names.shm_name_ = add_suffix("SHM", "farma");
    names.mut_pc_ = add_suffix("MUT-PC", "farma");
    int requiredCount = requiredNumberOfPlayers; 
    ServerData sd;
    sd.clientCount = 0;
    sd.activeIndex = -1;
    sd.names = names;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        sd.clients[i].active = false;
    }
    shm_init(&sd.names, requiredCount);
    printf("[SERVER] Creating PIPE '%s'\n", SERVER_PIPE);
    pipe_init(SERVER_PIPE);

    printf("[SERVER] Opening '%s' for reading...\n", SERVER_PIPE);
    int server_fd = pipe_open_read(SERVER_PIPE);

    printf("[SERVER] Running. Commands: roll, exchange <animal1> <animal2>, end, shutdown, quit.\n");
    fflush(stdout);

    bool running = true;
    bool initialized = false;

    synchronized_game g;
   
    sd.syn_game = g;
    

    while (running) {
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer));

        ssize_t n = read(server_fd, buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0';

            char cname[BUFFER_SIZE] = {0};
            char cmd[BUFFER_SIZE]   = {0};
            char parama[BUFFER_SIZE]   = {0};
            char paramb[BUFFER_SIZE]   = {0};

            int count = sscanf(buffer, "%s %s", cname, cmd);

            if (count < 2) {
                printf("[SERVER] Error with command parse.\n");
                fflush(stdout);
                continue;
            }
        
            int idx = add_client(&sd, cname); //if cleint already exists returns his index
            if (idx < 0) {
                printf("[SERVER] Could not add client '%s'.\n", cname);
                fflush(stdout);
                continue;
            }
        
            if (strcmp(cmd, "join") == 0) {
                char msg[BUFFER_SIZE*2];
                snprintf(msg, sizeof(msg), "You have successfully joined the game. Players joined: %d/%d.\n", sd.clientCount, requiredCount);
                send_to_index(&sd,idx,msg);
                if (sd.clientCount < requiredCount) {
                    char bc[BUFFER_SIZE*2];
                    snprintf(bc, sizeof(bc), "Player %s joined. Waiting for all players %d/%d.\n",cname, sd.clientCount, requiredCount);
                    broadcast_msg(&sd, bc);
                } else {
                    char bc[BUFFER_SIZE*2];
                    send_to_index(&sd, idx, "Game is starting.\n");
                    snprintf(bc, sizeof(bc), "Player %s joined. Game is starting for %d players.\n", cname, sd.clientCount);
                    broadcast_msg(&sd, bc); //TODO exclude 
                    
                    if (!initialized) {
                        syn_shm_game_init(&sd.syn_game, requiredCount ,&sd.names);
                        initialized = true;
                    }
                } 
                continue;
            }

            if (strcmp(cmd, "quit") == 0) {
                send_to_index(&sd, idx, "shutdown");
                remove_client(&sd, idx);
                char bc[BUFFER_SIZE*2];
                snprintf(bc, sizeof(bc), "[BCAST] Player '%s' left the game.\n", cname);
                broadcast_msg(&sd, bc);
                continue;
            }

            if (strcmp(cmd, "shutdown") == 0) {
                    send_to_index(&sd, idx, "[SERVER] Shutting down...\n");
                    broadcast_msg(&sd, "[BCAST] *** SERVER shutting down ***\n");
                    broadcast_msg(&sd, "shutdown");
                    running = false;
            }
            
            if (idx != sd.activeIndex) {
                char waitMsg[BUFFER_SIZE*2];
                snprintf(waitMsg, sizeof(waitMsg), "[SERVER] Wait your turn. Current player: %s\n", get_active_name(&sd));
                send_to_index(&sd, idx, waitMsg);
                continue;
            }
            
            if (initialized) {
                if (strcmp(cmd, "roll") == 0 && check_action_count(&sd, idx)) {
                    char msg[BUFFER_SIZE*2];
                    char msgOthers[BUFFER_SIZE*2];
                    syn_shm_game_player_roll_dice(&sd.syn_game, get_active_player(&sd), msg, msgOthers);
                    send_to_index(&sd, idx, msg);
                    char bc[BUFFER_SIZE*2];
                    //obsahuje player_roll_dice ktory tu bol pred tym
                    snprintf(bc, sizeof(bc), "Player %s performed a roll.\n", cname);
                    broadcast_msg(&sd, bc);
                    continue;
                }

                else if (strcmp(cmd, "exchange") == 0) {
                    sscanf(buffer, "%*s %*s %s %s", parama, paramb);
                    char msg[BUFFER_SIZE];
                    perform_exchange(&sd, parama, paramb, msg);
                    send_to_index(&sd, idx, msg);
                    // char bc[BUFFER_SIZE*2];
                    // broadcast_msg(&sd, bc);
                    continue;
                
                }

                else if (strcmp(cmd, "end") == 0) {
                    if (check_victory(&sd)) {
                        char bc[BUFFER_SIZE*2];
                        snprintf(bc, sizeof(bc), "[BCAST] Player '%s' won the game!\n", cname);
                        broadcast_msg(&sd, bc);
                        send_to_index(&sd, idx, "You have won the game!!!");
                        broadcast_msg(&sd, "shutdown");
                        running = false;
                        continue;
                    }
                    char bc[BUFFER_SIZE*2];
                    snprintf(bc, sizeof(bc), "[BCAST] Player '%s' ended turn.\n", cname);
                    broadcast_msg(&sd, bc);
                    next_turn(&sd);
                    send_to_index(&sd, idx, "Now it is your turn "); //TODO vsetky mozne komandy
                    snprintf(bc, sizeof(bc), "[BCAST] Now it is '%s' turn.\n", get_active_name(&sd));
                    broadcast_msg(&sd, bc);
                    sd.clients[sd.activeIndex].performedAction = 0;
                    continue;
                }
                
                else if (strcmp(cmd, "inventory") == 0) {
                    //TODO show player inventory functionality
                    continue;
                }
                else {
                    char resp[BUFFER_SIZE*2];
                    snprintf(resp, sizeof(resp), "[SERVER] Unknown command: %s\n", cmd);
                    send_to_index(&sd, idx, resp);
                    continue;
                }
            }
        }
        else if (n == 0) {
            pipe_close(server_fd);
            server_fd = pipe_open_read(SERVER_PIPE);
        }
        else {
            perror("[SERVER] read error");
            break;
        }
    }

    pipe_close(server_fd);
    pipe_destroy(SERVER_PIPE);

    
    syn_shm_game_destroy(&sd.names, &sd.syn_game);
    shm_destroy(&sd.names);
    clear_names(&names);
    printf("[SERVER] Shutdown.\n");
    fflush(stdout);
    return 0;
}

int* inventory_look(ServerData* sd, int playerIndex) {
    int* inv_cpy = malloc(sizeof(int) * FOX);
    if (!inv_cpy) {
        return NULL;
    }
    for (int i = 0; i < FOX; ++i) {
        inv_cpy[i] = sd->clients[playerIndex].player_.playerAnimals[i];
    }
    return inv_cpy;
}

//vysledok zo syn_inventory_look treba ukladat do premennej a potom na konci pouzivania treba dat free(premenna)
int* syn_inventory_look(ServerData* sd, int playerIndex) {
    pthread_mutex_lock(&sd->mut);
    int* ar = inventory_look(sd, playerIndex);
    pthread_mutex_unlock(&sd->mut);
    return ar;
}




