#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "enums.h"

typedef struct player{
    int playerIndex;
    int playerAnimals[ANIMAL_COUNT];
} player;

void player_init(player* playerPtr, int playerIndex);
void player_destroy(player* playerPtr);

#endif
