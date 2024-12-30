#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "dice.h"
#include "enums.h"
#include "player.h"
#include "dice.h"
#include "shop.h"

typedef struct {
    dice dice_1;
    dice dice_2;
    player* players;
    int playerCount;
    shop shop;
} game;

void init_game(game* gamePtr, player* players, int playerCount, shop* shop);

void player_roll_dice(game *gamePtr, player* currentPlayer);

#endif
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#include "dice.h"
#include "shop.h"
#include "player.h"

typedef struct Syn_players {
    int number_of_players;
    int current_index;
    pthread_mutex_t mut;
    pthread_cond_t[] players_cond;
    Player[] players;
} Syn_players;

void syn_players_init(Syn_players* this, int number_of_players);
void syn_players_destroy(Syn_players* this);
