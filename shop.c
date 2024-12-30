#include "shop.h"

void init_shop(shop* this , int *prices, int *animalCounts) {
    for (int i = 0; i < ANIMAL_COUNT_SHOP; i++) {
        this->prices[i] = prices[i];
        this->allAnimals[i] = animalCounts[i];
    }
}

void exchange_shop(shop *this, player* player, animalTypesShop in, animalTypesShop out) {
    if (in < out) {
        player->playerAnimals[in] -= this->prices[in];
        player->playerAnimals[out] += 1;
    } else {
        player->playerAnimals[in] -= 1;
        player->playerAnimals[out] += this->prices[out];
    }
}