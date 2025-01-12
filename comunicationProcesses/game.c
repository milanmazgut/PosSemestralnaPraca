#include "game.h"
#include "server.h"
#include "dice.h"
#include "enums.h"
#include "player.h"
#include "shop.h"
#include <stdio.h>

void game_init(game *this, int playerCount) {
    int CAPACITY = 12;
    int probabilities_1[] = {6, 3, 1, 1, 0, 0, 0, 1, 0};
    int probabilities_2[] = {6, 2, 2, 0, 1, 0, 0, 0, 1};

    // RABBITS | SHEEP | PIG | COW | HORSE | SMALLDOG | BIG DOG
    int prices[] = {6, 2, 3, 2, 1, 1};
    int animalCounts[] = {10*playerCount, 5*playerCount, 4*playerCount, 2*playerCount, playerCount, playerCount - 1, playerCount/2};
    this->playerCount = playerCount;
    dice_init(&this->dice_1, CAPACITY, probabilities_1);
    dice_init(&this->dice_2, CAPACITY, probabilities_2);
    
    shop_init(&this->shop, prices, animalCounts);
}

void game_destroy(game *this) {
  
    shop_destroy(&this->shop);
    dice_destroy(&this->dice_1);
    dice_destroy(&this->dice_2);
}

void player_roll_dice(game *this, player* currentPlayer, char* outputPlayer, char* outputOthers) {
    int dice_1;
    int dice_2;
    roll_dice(&this->dice_1, &dice_1);
    roll_dice(&this->dice_2, &dice_2);
    snprintf(outputPlayer, BUFFER_SIZE, "You dropped %s and %s \n> Nothing happens, continue playing or end your turn", animalNames[dice_1], animalNames[dice_2]);
    snprintf(outputOthers, BUFFER_SIZE, "Player %s dropped %s and %s \n>", currentPlayer->playerName, animalNames[dice_1], animalNames[dice_2]);

    if ( dice_1 == dice_2) {
        change_animal_ownership(&this->shop, currentPlayer, dice_1, 1);
        snprintf(outputPlayer, BUFFER_SIZE, "You dropped %s and %s, you get %s", animalNames[dice_1], animalNames[dice_2], animalNames[dice_1]);
        snprintf(outputOthers, BUFFER_SIZE, "Player %s dropped %s and %s, and got %s", currentPlayer->playerName, animalNames[dice_1], animalNames[dice_2], animalNames[dice_1]);

    } else {

        if(dice_1 == FOX || dice_2 == FOX) {
            if (currentPlayer->playerAnimals[SMALL_DOG] >= 1) {
                snprintf(outputPlayer, BUFFER_SIZE, "You dropped %s and %s \n> Your small dog sacrificed himself to save your rabbits from fox\n", animalNames[dice_1], animalNames[dice_2]);
                snprintf(outputOthers, BUFFER_SIZE, "Player %s dropped %s and %s \n> His/her small dog sacrificed himself to save his/her rabbits from fox\n", currentPlayer->playerName, animalNames[dice_1], animalNames[dice_2]);
                change_animal_ownership(&this->shop, currentPlayer, SMALL_DOG, -1);
            } else {
                snprintf(outputPlayer, BUFFER_SIZE, "You dropped %s and %s \n> Fox has eaten all your rabbits\n", animalNames[dice_1], animalNames[dice_2]);
                snprintf(outputOthers, BUFFER_SIZE, "Player %s dropped %s and %s \n> Fox has eaten all of his/her rabbits\n", currentPlayer->playerName, animalNames[dice_1], animalNames[dice_2]);
                change_animal_ownership(&this->shop, currentPlayer, RABBIT, -currentPlayer->playerAnimals[RABBIT]);
            }
        }
        if(dice_1 == WOLF || dice_2 == WOLF) {
            if (currentPlayer->playerAnimals[BIG_DOG] >= 1) {
                snprintf(outputPlayer, BUFFER_SIZE, "You dropped %s and %s \n> Your big dog sacrificed himself to save your sheeps, pigs and cows from wolf\n", animalNames[dice_1], animalNames[dice_2]);
                snprintf(outputOthers, BUFFER_SIZE, "Player %s dropped %s and %s \n> His/her big dog sacrificed himself to save his/her sheeps, pigs and cows from wolf\n", currentPlayer->playerName, animalNames[dice_1], animalNames[dice_2]);
                change_animal_ownership(&this->shop, currentPlayer, BIG_DOG, -1);
            } else {
                snprintf(outputPlayer, BUFFER_SIZE, "You dropped %s and %s \n> Wolf has eaten all your sheeps, pigs and cows\n", animalNames[dice_1], animalNames[dice_2]);
                snprintf(outputOthers, BUFFER_SIZE, "Player %s dropped %s and %s \n> Wolf has eaten all of his/her sheeps, pigs and cows\n", currentPlayer->playerName, animalNames[dice_1], animalNames[dice_2]);
                change_animal_ownership(&this->shop, currentPlayer, SHEEP, -currentPlayer->playerAnimals[SHEEP]);
                change_animal_ownership(&this->shop, currentPlayer, PIG, -currentPlayer->playerAnimals[PIG]);
                change_animal_ownership(&this->shop, currentPlayer, COW, -currentPlayer->playerAnimals[COW]);
            }
        }
    }
}

_Bool exchange_animal(game *this, player* currentPlayer, animalTypes in, animalTypes out) {
    if (currentPlayer->playerAnimals[in] >= this->shop.prices[in] && this->shop.allAnimals[out] > 0) {
        exchange_shop(&this->shop, currentPlayer, in, out);
        return true;
    } else {
        return false;
    }
}



void end_of_turn_animal_multiplication(game *this, player* currentPlayer) {
    for (int i = 0; i < ANIMAL_COUNT; i++) {
        change_animal_ownership(&this->shop, currentPlayer, i, currentPlayer->playerAnimals[i]/2);
    }
}
