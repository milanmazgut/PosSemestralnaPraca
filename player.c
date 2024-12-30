#include "player.h"
void init_player(Player* this) {
    for(int i = 0; i < ANIMAL_COUNT_SHOP; i++) {
        this->playerAnimals[i] = 0;
    }
}

void destroy_player(Player* this) {
    free(this->playerAnimals);
}