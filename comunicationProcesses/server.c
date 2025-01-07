#include "server.h"
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

    snprintf(client->pipe_path, sizeof(client->pipe_path), "client_%s", name);

    
    int cfd = pipe_open_write(client->pipe_path);
    client->fd = cfd;

    int newIndex = sd->clientCount;
    sd->clientCount++;

    if (sd->activeIndex < 0) {
        sd->activeIndex = newIndex;
    }

    printf("[SERVER] New client '%s' (index=%d) joined.\n", name, newIndex);
    fflush(stdout);

    return newIndex;
}


void remove_client(ServerData *sd, int idx) {
    if (idx < 0 || idx >= sd->clientCount) {
        return;
    }
    pipe_close(sd->clients[idx].fd);
    pipe_destroy(sd->clients[idx].pipe_path);

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

                char bc[BUFFER_SIZE];
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

            char bc[BUFFER_SIZE];
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


int server_main(void) 
{
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

    printf("[SERVER] Running. Commands: roll, trade, end, shutdown, quit.\n");
    fflush(stdout);

    bool running = true;
    bool initialized = false;

    game g;

    while (running) {
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer));

        ssize_t n = read(server_fd, buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0';

            char cname[BUFFER_SIZE] = {0};
            char cmd[BUFFER_SIZE]   = {0};
            sscanf(buffer, "%s %s", cname, cmd);

            int idx = add_client(&sd, cname);
            if (idx < 0) {
                printf("[SERVER] Could not add client '%s'.\n", cname);
                fflush(stdout);
                continue;
            }

            if (strcmp(cmd, "quit") == 0) {
                char bc[BUFFER_SIZE];
                snprintf(bc, sizeof(bc), "[BCAST] Player '%s' left the game.\n", cname);
                remove_client(&sd, idx);
                broadcast_msg(&sd, bc);
                continue;
            }
            
            if (strcmp(cmd, "init") == 0) {
                game_init(&g ,sd.clientCount);
                initialized = true;
                continue;
            }
            

            if (idx != sd.activeIndex) {
                char waitMsg[BUFFER_SIZE];
                snprintf(waitMsg, sizeof(waitMsg), "[SERVER] Wait your turn. Current: %s\n", get_active_name(&sd));
                send_to_index(&sd, idx, waitMsg);
                continue;
            }
            if (initialized) {
                if (strcmp(cmd, "roll") == 0) {
                    char bc[BUFFER_SIZE];
                    snprintf(bc, sizeof(bc), "[BCAST] Player '%s' performed ROLL.\n", cname);
                    broadcast_msg(&sd, bc);
                    player_roll_dice(&g, &sd.clients[sd.activeIndex].player_, bc);
                    broadcast_msg(&sd, bc);

                }
                else if (strcmp(cmd, "end") == 0) {
                    send_to_index(&sd, idx, "[SERVER] Your turn ended.\n");
                    char bc[BUFFER_SIZE];
                    snprintf(bc, sizeof(bc), "[BCAST] Player '%s' ended turn.\n", cname);
                    broadcast_msg(&sd, bc);
                    next_turn(&sd);
                    snprintf(bc, sizeof(bc), "[BCAST] Now it is '%s' turn.\n", get_active_name(&sd));
                    broadcast_msg(&sd, bc);
                }
                else if (strcmp(cmd, "shutdown") == 0) {
                    send_to_index(&sd, idx, "[SERVER] Shutting down...\n");
                    broadcast_msg(&sd, "[BCAST] *** SERVER shutting down ***\n");
                    running = false;
                }
                else {
                    char resp[BUFFER_SIZE];
                    snprintf(resp, sizeof(resp), "[SERVER] Unknown command: %s\n", cmd);
                    send_to_index(&sd, idx, resp);
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

    for (int i = 0; i < sd.clientCount; i++) {
        if (sd.clients[i].active) {
            pipe_close(sd.clients[i].fd);
            pipe_destroy(sd.clients[i].pipe_path);
        }
    }
    pipe_close(server_fd);
    pipe_destroy(SERVER_PIPE);

    printf("[SERVER] Shutdown.\n");
    fflush(stdout);
    return 0;
}
