#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include "dice.h"
#include "enums.h"
#include "player.h"
#include "dice.h"
#include "shop.h"

typedef struct game{
    dice dice_1;
    dice dice_2;
    player* players;
    int playerCount;
    shop shop;
} game;

typedef struct syn_game {
    int number_of_players;
    int current_index;
    pthread_mutex_t mut;
    pthread_cond_t* players_cond;
    game* game;
} syn_game;

void game_init(game* gamePtr, int playerCount);
void player_roll_dice(game *gamePtr, player* currentPlayer, char* output);
_Bool exchange_animal(game *this, player* currentPlayer, animalTypes in, animalTypes out);
void end_of_turn_animal_multiplication(game *this, player* currentPlayer);
void syn_game_init(syn_game* gamePtr, int number_of_players);
void syn_game_destroy(syn_game* gamePtr);
//void syn_turn(syn_game* syn_gamePtr, int playerIndex);
void start(int number_of_players);

#endif
