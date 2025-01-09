#include "server.h"
#include "game.h"
#include "pipe.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


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

    player_init(&client->player_, sd->clientCount);

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
                perror("[SERVER] write(broadcast)");
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
            perror("[SERVER] write(send_to_index)");
        }
    }
}

void next_turn(ServerData *sd) {
    if (sd->clientCount <= 0) {
        sd->activeIndex = -1;
        return;
    }
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

void perform_exchange(game *game, ServerData *sd, const char *animalIn, const char *animalOut, int count, char * output) {
    int inType = get_animal_type(animalIn);
    int outType = get_animal_type(animalOut);

    if (inType == -1 || outType == -1) {
        snprintf(output, BUFFER_SIZE, "Invalid animal names provided.\n");
        return;
    }
    
    if (inType - outType == 1 || inType - outType == -1) {
            for (int i = 0; i < count; i++) {
        _Bool success = exchange_animal(game, get_active_player(sd), inType, outType);
        if (success) {
            snprintf(output, BUFFER_SIZE, "Animals have been succesfuly changed\n");
        }
        else {
            snprintf(output, BUFFER_SIZE, "There was an error while exchanging animals.\n");
        }
   }
    } else {
        snprintf(output, BUFFER_SIZE, "Invalid exchange.\n");
    }
}

int check_action_count(ServerData *sd, int index) { //returns 1 if player can use roll command
    if(sd->clients[sd->activeIndex].performedAction == 1) {
        char noActionMsg[BUFFER_SIZE*2];
        snprintf(noActionMsg, sizeof(noActionMsg), "[SERVER] You already used roll this turn.");
        send_to_index(sd, index, noActionMsg);
        return 0;
        }
    sd->clients[sd->activeIndex].performedAction = 1;
    return 1;
}

player* get_active_player(ServerData *sd) {
    return &sd->clients[sd->activeIndex].player_;
}

int server_main(int requiredNumberOfPlayers) 
{
    int requiredCount = requiredNumberOfPlayers; 
    ServerData sd;
    sd.clientCount = 0;
    sd.activeIndex = -1;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        sd.clients[i].active = false;
    }

    printf("[SERVER] Creating PIPE '%s'\n", SERVER_PIPE);
    pipe_init(SERVER_PIPE);

    printf("[SERVER] Opening '%s' for reading...\n", SERVER_PIPE);
    int server_fd = pipe_open_read(SERVER_PIPE);

    printf("[SERVER] Running. Commands: roll, exchange <animal1> <animal2>, end, shutdown, quit.\n");
    fflush(stdout);

    bool running = true;
    bool initialized = false;

    game* g;
    g = malloc(sizeof(game));

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
        

            if (strcmp(cmd, "quit") == 0) {
                send_to_index(&sd, idx, "shutdown");

                remove_client(&sd, idx);
                
                char bc[BUFFER_SIZE*2];
                snprintf(bc, sizeof(bc), "[BCAST] Player '%s' left the game.\n", cname);
                broadcast_msg(&sd, bc);
                continue;
            }
            
            if (strcmp(cmd, "init") == 0 && !initialized) {
                if (sd.clientCount == requiredCount) {
                    game_init(g ,sd.clientCount);
                    char bc[BUFFER_SIZE*2];
                    snprintf(bc, sizeof(bc), "[BCAST] Game was successfuly initialzied for players %d\n", sd.clientCount);
                    broadcast_msg(&sd, bc);
                    initialized = true;
                } else {
                    char bc[BUFFER_SIZE*2];
                    snprintf(bc, sizeof(bc), "You have successfully joined the game. Waiting for all players %d/%d", sd.clientCount, requiredCount);
                    send_to_index(&sd,idx,bc);
                }
                continue;
            }
            
            if (idx != sd.activeIndex) {
                char waitMsg[BUFFER_SIZE*2];
                snprintf(waitMsg, sizeof(waitMsg), "[SERVER] Wait your turn. Current player: %s\n", get_active_name(&sd));
                send_to_index(&sd, idx, waitMsg);
                continue;
            }
            
            if (initialized) {
                if (strcmp(cmd, "roll") == 0 && check_action_count(&sd, idx)) {
                    char bc[BUFFER_SIZE*2];
                    player_roll_dice(g, get_active_player(&sd), bc);
                    broadcast_msg(&sd, bc);
                    continue;
                }

                else if (strcmp(cmd, "exchange") == 0) {
                    sscanf(buffer, "%*s %*s %s %s", parama, paramb);
                    char msg[BUFFER_SIZE];
                    perform_exchange(g, &sd, parama, paramb, 1, msg);
                    send_to_index(&sd, idx, msg);
                    char bc[BUFFER_SIZE*2];
                    broadcast_msg(&sd, bc);
                    continue;
                
                }

                else if (strcmp(cmd, "end") == 0) {
                    char bc[BUFFER_SIZE*2];
                    snprintf(bc, sizeof(bc), "[BCAST] Player '%s' ended turn.\n", cname);
                    broadcast_msg(&sd, bc);
                    next_turn(&sd);
                    snprintf(bc, sizeof(bc), "[BCAST] Now it is '%s' turn.\n", get_active_name(&sd));
                    broadcast_msg(&sd, bc);
                    sd.clients[sd.activeIndex].performedAction = 0;
                    continue;
                }
                else if (strcmp(cmd, "shutdown") == 0) {
                    send_to_index(&sd, idx, "[SERVER] Shutting down...\n");
                    broadcast_msg(&sd, "[BCAST] *** SERVER shutting down ***\n");
                    broadcast_msg(&sd, "shutdown");
                    running = false;
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

    game_destroy(g);
    free(g);
    
    printf("[SERVER] Shutdown.\n");
    fflush(stdout);
    return 0;
}
