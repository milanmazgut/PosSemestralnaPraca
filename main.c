#include <stdio.h>
#include "game.h"

int get_animal_type(const char *animalName) {
    for (int i = 0; i < ANIMAL_COUNT; i++) {
        if (strcmp(animalNames[i], animalName) == 0) {
            return i;
        }
    }
    return -1;
}
void perform_exchange(shop *shop, player *player, const char *animalIn, const char *animalOut, int count) {
    int inType = get_animal_type(animalIn);
    int outType = get_animal_type(animalOut);

    if (inType == -1 || outType == -1) {
        printf("Invalid animal name provided.\n");
        return;
    }
    
    if (inType - outType == 1 || inType - outType == -1) {
            for (int i = 0; i < count; i++) {
        exchange_shop(shop, player, inType, outType);
    }
    } else {
        printf("Invalid exchange.\n");
    
    }
}


int main(int argc, char* argv[]) {
    int n = 5;
    if (argc > 1) {
        n = atoi(argv[1]);
    }
    start(n);
    return 0;
}