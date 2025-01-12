#ifndef SHOP_H
#define SHOP_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "enums.h"
#include "player.h"

typedef struct {
    int* prices;
    int* allAnimals;
} shop;

void shop_init(shop* shopPtr, int* prices, int* animalCounts);
void exchange_shop(shop* shopPtr, player* player, animalTypes in, animalTypes out);
void change_animal_ownership(shop* shopPtr, player* currentPlayer, int type, int count);
void shop_destroy(shop* shopPtr);


#endif
