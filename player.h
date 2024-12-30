#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "enums.h"

typedef struct Player{
    int playerAnimals[ANIMAL_COUNT_SHOP];
} Player;

void init_player(Player* this);
void destroy_player(Player* this);

#endif