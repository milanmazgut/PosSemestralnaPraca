#include "player.h"
void player_init(player* this, int playerIndex, const char* playerName) {
    for(int i = 0; i < FOX; i++) {
        this->playerAnimals[i] = 3;
    }
    this->playerAnimals[0] = 1;
    this->playerIndex = playerIndex;
    this->playerName = playerName;
}

void player_destroy(player* this) {
}
