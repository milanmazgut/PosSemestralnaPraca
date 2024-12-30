#ifndef SHOP_H
#define SHOP_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "enums.h"
#include "player.h"

typedef struct {
    int prices[ANIMAL_COUNT_SHOP];
    int allAnimals[ANIMAL_COUNT_SHOP];
} shop;

void shop_init(shop* this, int* prices, int* animalCounts);
void exchange_shop(shop* this, player* player, animalTypesShop in, animalTypesShop out);
void shop_destroy(shop* this);


#endif