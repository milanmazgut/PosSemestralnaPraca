#include "player.h"
void player_init(player* this, int playerIndex) {
    this->playerAnimals = malloc(ANIMAL_COUNT * sizeof(int));
    for(int i = 0; i < ANIMAL_COUNT; i++) {
        this->playerAnimals[i] = 0;
    }
    this->playerIndex = playerIndex;
}

void player_destroy(player* this) {
    free(this->playerAnimals);
}
