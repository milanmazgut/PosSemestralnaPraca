#include "player.h"
void player_init(Player* this) {
    for(int i = 0; i < ANIMAL_COUNT_SHOP; i++) {
        this->playerAnimals[i] = 0;
    }
}

void player_destroy(Player* this) {
    free(this->playerAnimals);
}