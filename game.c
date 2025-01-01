#include "game.h"
#include "dice.h"
#include "enums.h"
#include "player.h"
#include "shop.h"

void init_game(game *this, player* players, int playerCount, shop* shop) {
    int CAPACITY = 12;
    int probabilities_1[] = {6, 3, 1, 1, 0, 1, 0};
    int probabilities_2[] = {6, 2, 2, 0, 1, 0, 1};

    // RABBITS | SHEEP | PIG | COW | HORSE | SMALLDOG | BIG DOG
    int prices[] = {6, 2, 3, 2, 1, 1};
    
    int animalCounts[] = {10*playerCount, 5*playerCount, 4*playerCount, 2*playerCount, playerCount, playerCount - 1, playerCount/2};

    dice_init(&this->dice_1, CAPACITY, probabilities_1);
    dice_init(&this->dice_2, CAPACITY, probabilities_2);
    this->players = players;
    shop_init(&this->shop, prices, animalCounts);
}

void changeAnimalOwnership(game* this, player* currentPlayer, int type, int count) {
    this->shop.allAnimals[type] -= count;
    currentPlayer->playerAnimals[type] += count;
}

void player_roll_dice(game *this, player* currentPlayer) {
    animalTypesDice dice_1;
    animalTypesDice dice_2;
    roll_dice(&this->dice_1, &dice_1);
    roll_dice(&this->dice_2, &dice_2);

    if (dice_1 == dice_2) {
        changeAnimalOwnership(this, currentPlayer, dice_1, 1);
    } else {
        if(dice_1 == FOX || dice_2 == FOX) {
            if (currentPlayer->playerAnimals[SMALL_DOG] == 1) {
                changeAnimalOwnership(this, currentPlayer, SMALL_DOG, -1);
            } else {
                changeAnimalOwnership(this, currentPlayer, RABBIT, -currentPlayer->playerAnimals[RABBIT]);
            }
        }
        if(dice_1 == WOLF || dice_2 == WOLF) {
            if (currentPlayer->playerAnimals[BIG_DOG] == 1) {
                changeAnimalOwnership(this, currentPlayer, BIG_DOG, -1);
            } else {
                changeAnimalOwnership(this, currentPlayer, SHEEP,currentPlayer->playerAnimals[SHEEP]);
                changeAnimalOwnership(this, currentPlayer, PIG,currentPlayer->playerAnimals[PIG]);
                changeAnimalOwnership(this, currentPlayer, COW,currentPlayer->playerAnimals[COW]);
            }
        }
    }
}

_Bool exchangeAnimal(game *this, player* currentPlayer, animalTypesShop in, animalTypesShop out) {
    if (currentPlayer->playerAnimals[in] >= this->shop.prices[0]) {
        exchange_shop(&this->shop, currentPlayer, in, out);
        return true;
    } else {
        return false;
    }
}