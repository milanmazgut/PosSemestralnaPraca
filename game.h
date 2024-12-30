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
    player[] players;
    int playerCount;
    shop shop;
} game;

typedef struct syn_game {
    int number_of_players;
    int current_index;
    pthread_mutex_t mut;
    pthread_cond_t[] players_cond;
    game* game;
} syn_game;

void game_init(game* gamePtr, player* players, int playerCount, shop* shop);
void player_roll_dice(game *gamePtr, player* currentPlayer);
void syn_game_init(syn_game* this, int number_of_players);
void syn_game_destroy(syn_game* this);

#endif