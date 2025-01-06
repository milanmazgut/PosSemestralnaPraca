#include "player.h"
void player_init(player* this) {
    for(int i = 0; i < ANIMAL_COUNT; i++) {
        this->playerAnimals[i] = 0;
    }
}

void player_destroy(player* this) {
    free(this->playerAnimals);
}