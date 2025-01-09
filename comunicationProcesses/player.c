#include "player.h"
void player_init(player* this, int playerIndex) {
    for(int i = 0; i < ANIMAL_COUNT; i++) {
        this->playerAnimals[i] = 6;
    }
    this->playerIndex = playerIndex;
}

void player_destroy(player* this) {
}
