#include "shop.h"
#include <stdio.h>

void shop_init(shop* this , int *prices, int *animalCounts) {
    this->prices = malloc(ANIMAL_COUNT * sizeof(int));
    this->allAnimals = malloc(ANIMAL_COUNT * sizeof(int));
    for (int i = 0; i < FOX; i++) {
        this->prices[i] = prices[i];
        this->allAnimals[i] = animalCounts[i];
    }
}
// -count for selling to shop, +count for buying from shop
void change_animal_ownership(shop* this, player* currentPlayer, int type, int count) {
    if (this->allAnimals[type] > 0) {
        this->allAnimals[type] -= count;
        currentPlayer->playerAnimals[type] += count;
    } else {
        currentPlayer->playerAnimals[type] += this->allAnimals[type];
        printf("There aren't enough %s. Only %d were added.\n", animalNames[type], this->allAnimals[type]);
        fflush(stdout);
        this->allAnimals[type] = 0;
    }
}

void exchange_shop(shop *this, player* player, animalTypes in, animalTypes out, _Bool dog) {
    int price = in;
    if (dog) {
        price = out - 1;
    }
    if (in < out) {
        change_animal_ownership(this, player, in, -this->prices[price]);
        change_animal_ownership(this, player, out, 1);
        printf("Player have exchanged %s {%d} for %s {%d}.\n", animalNames[in], this->prices[price], animalNames[out], 1);
    } else if (out < in && !dog) {
        price = out;
        change_animal_ownership(this, player, in, -1);
        change_animal_ownership(this, player, out, this->prices[out]); 
        printf("Player have exchanged %s {%d} for %s {%d}.\n", animalNames[in], 1, animalNames[out], this->prices[price]);
    }
    fflush(stdout);
}
void shop_destroy(shop* this) {
    free(this->prices);
    free(this->allAnimals);
}
