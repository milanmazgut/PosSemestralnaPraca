#include "shop.h"

void init_shop(shop* this , int *prices, int *animalCounts) {
    for (int i = 0; i < ANIMAL_COUNT_SHOP; i++) {
        this->prices[i] = prices[i];
        this->allAnimals[i] = animalCounts[i];
    }
}