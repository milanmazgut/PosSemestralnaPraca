#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "enums.h"

typedef struct player{
    int playerAnimals[ANIMAL_COUNT_SHOP];
} player;

void player_init(player* this);
void player_destroy(player* this);

#endif