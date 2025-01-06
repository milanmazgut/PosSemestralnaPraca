#include <stdio.h>

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

int main() {

    printf("Hello, World!\n");
    return 0;
}