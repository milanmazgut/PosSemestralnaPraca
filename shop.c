#include "shop.h"

void shop_init(shop* this , int *prices, int *animalCounts) {
    for (int i = 0; i < ANIMAL_COUNT; i++) {
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
        this->allAnimals[type] = 0;
    }
}

void exchange_shop(shop *this, player* player, animalTypes in, animalTypes out) {
    if (in < out) {
        change_animal_ownership(this, player, in, -this->prices[in]);
        change_animal_ownership(this, player, out, 1);
        printf("You have exchanged %s {%d} for %s {%d}.\n", animalNames[in], this->prices[in], animalNames[out], 1);
    } else {
        change_animal_ownership(this, player, in, -1);
        change_animal_ownership(this, player, out, +this->prices[out]); 
        printf("You have exchanged %s {%d} for %s {%d}.\n", animalNames[in], 1, animalNames[out], this->prices[in]);
    }
}

void shop_destroy(shop* this) {
    free(this->prices);
    free(this->allAnimals);
}