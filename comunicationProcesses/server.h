#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>
#include "game.h"

#define SERVER_PIPE "server_pipe"
#define BUFFER_SIZE 256
#define MAX_CLIENTS 10


typedef struct {
    bool active;
    char name[BUFFER_SIZE];
    char pipe_path[BUFFER_SIZE];
    int  fd;
    player player_;
} Client;


typedef struct {
    Client clients[MAX_CLIENTS];
    int clientCount;
    int activeIndex;
} ServerData;


int  server_main(void); 
int  find_client(ServerData *sd, const char* name);
int  add_client(ServerData *sd, const char* name);
void remove_client(ServerData *sd, int idx);
void broadcast_msg(ServerData *sd, const char* msg);
void send_to_index(ServerData *sd, int idx, const char* msg);
void next_turn(ServerData *sd);
const char* get_active_name(ServerData *sd);

#endif 