#include "player.h"
void player_init(player* this, int playerIndex) {
    for(int i = 0; i < ANIMAL_COUNT_SHOP; i++) {
        this->playerAnimals[i] = 0;
    }
    this->playerIndex = playerIndex;
}

void player_destroy(player* this) {
    free(this->playerAnimals);
}