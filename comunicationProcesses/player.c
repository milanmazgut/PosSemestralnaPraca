#include "player.h"
void player_init(player* this, int playerIndex) {
    for(int i = 0; i < FOX; i++) {
        this->playerAnimals[i] = 5;
    }
    this->playerAnimals[0] = 1;
    this->playerIndex = playerIndex;
}

void player_destroy(player* this) {
}
