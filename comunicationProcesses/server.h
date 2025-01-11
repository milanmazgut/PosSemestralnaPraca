#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>
#include "game.h"
#include "syn_game.h"

#define SERVER_PIPE "server_pipe"
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10


typedef struct {
    bool active;
    char name[BUFFER_SIZE];
    char pipe_path[BUFFER_SIZE];
    int  fd;
    int performedAction;
    player player_;
} Client;

typedef struct {
    Client clients[MAX_CLIENTS];
    int clientCount;
    int activeIndex;
    pthread_mutex_t mut;
    shared_names names;
    synchronized_game syn_game;
} ServerData;


int server_main(int requiredNumberOfPlayers);
int find_client(ServerData *sd, const char* name);
int add_client(ServerData *sd, const char* name);
void remove_client(ServerData *sd, int idx);
void broadcast_msg(ServerData *sd, const char* msg);
void send_to_index(ServerData *sd, int idx, const char* msg);
void next_turn(ServerData *sd);
const char* get_active_name(ServerData *sd);
int get_animal_type(const char *animalName);
void perform_exchange(ServerData *sd, const char *animalIn, const char *animalOut, char * output);
int check_action_count(ServerData *sd, int index);
player* get_active_player(ServerData *sd);
int* inventory_look(ServerData *sd, int playerIndex);
int* syn_inventories_look(ServerData* sd, int playerIndex);
#endif // SERVER_H
