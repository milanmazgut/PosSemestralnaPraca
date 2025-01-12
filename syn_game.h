#ifndef SYN_GAME 
#define SYN_GAME 

#include <semaphore.h>
#include "shm.h"
#include "game.h"

typedef struct synchronized_game {
    game *game_;
    int game_fd_;
    sem_t *mut_pc_;
} synchronized_game;

void syn_shm_game_init(synchronized_game* gamePtr, int player_count,shared_names *names);
void syn_shm_game_destroy(shared_names *names, synchronized_game* syn_game);
void syn_shm_game_open(synchronized_game *gamePtr, shared_names *names);
void syn_shm_game_close(synchronized_game *gamePtr);
void syn_shm_game_player_roll_dice(synchronized_game *gamePtr, player* currentPlayer, const char* playerName, char* outputPlayer, char* outputOthers);
_Bool syn_shm_game_exchange_animal(synchronized_game *gamePtr, player* currentPlayer, animalTypes in, animalTypes out);
void syn_shm_game_end_of_turn_animal_multiplication(synchronized_game *gamePtr, player* currentPlayer);
int** syn_shm_game_view_shop(synchronized_game *gamePtr);

#endif