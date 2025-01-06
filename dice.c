#include "dice.h"

void dice_init(dice* this,int capacity,int* probabilities) {
    srand(time(NULL));
    for (int i = 0; i < ANIMAL_COUNT; ++i) {
        this->probabilities[i] = probabilities[i];
        this->capacity += probabilities[i];
    }
}

void roll_dice(dice* this, animalTypesDice* type) {
    int random = rand() % this->capacity; 
    int sum = 0;

    
    for (int i = 0; i < ANIMAL_COUNT; i++) {
        sum += this->probabilities[i];
        if (random < sum) {
            *type = i;
            return;
        }
    }
}

void dice_destroy(dice* this) {
    free(this->probabilities);
}
