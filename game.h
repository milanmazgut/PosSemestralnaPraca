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