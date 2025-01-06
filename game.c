#include "game.h"
#include "dice.h"
#include "enums.h"
#include "player.h"
#include "shop.h"
#include <stdio.h>

void init_game(game *this, player* players, int playerCount, shop* shop) {
    int CAPACITY = 12;
    int probabilities_1[] = {6, 3, 1, 1, 0, 0, 0, 1, 0};
    int probabilities_2[] = {6, 2, 2, 0, 1, 0, 0, 0, 1};

    // RABBITS | SHEEP | PIG | COW | HORSE | SMALLDOG | BIG DOG
    int prices[] = {6, 2, 3, 2, 1, 1};
    
    int animalCounts[] = {10*playerCount, 5*playerCount, 4*playerCount, 2*playerCount, playerCount, playerCount - 1, playerCount/2};

    dice_init(&this->dice_1, CAPACITY, probabilities_1);
    dice_init(&this->dice_2, CAPACITY, probabilities_2);
    this->players = players;
    shop_init(&this->shop, prices, animalCounts);
}

// void changeAnimalOwnership(game* this, player* currentPlayer, int type, int count) {
//     if (this->shop.allAnimals[type] > 0) {
//         this->shop.allAnimals[type] -= count;
//         currentPlayer->playerAnimals[type] += count;
//     } else {
//         printf("There aren't enough animals of this type.\n");
//     }
// }

void player_roll_dice(syn_game *this, player* currentPlayer) {
    animalTypes dice_1;
    animalTypes dice_2;
    roll_dice(&this->game->dice_1, &dice_1);
    roll_dice(&this->game->dice_2, &dice_2);

    if ( dice_1 == dice_2) {
        change_animal_ownership(&this->game->shop, currentPlayer, dice_1, 1);
    } else {
        if(dice_1 == FOX || dice_2 == FOX) {
            if (currentPlayer->playerAnimals[SMALL_DOG] == 1) {
                change_animal_ownership(&this->game->shop, currentPlayer, SMALL_DOG, -1);
            } else {
                change_animal_ownership(&this->game->shop, currentPlayer, RABBIT, -currentPlayer->playerAnimals[RABBIT]);
            }
        }
        if(dice_1 == WOLF || dice_2 == WOLF) {
            if (currentPlayer->playerAnimals[BIG_DOG] == 1) {
                change_animal_ownership(&this->game->shop, currentPlayer, BIG_DOG, -1);
            } else {
                change_animal_ownership(&this->game->shop, currentPlayer, SHEEP,currentPlayer->playerAnimals[SHEEP]);
                change_animal_ownership(&this->game->shop, currentPlayer, PIG,currentPlayer->playerAnimals[PIG]);
                change_animal_ownership(&this->game->shop, currentPlayer, COW,currentPlayer->playerAnimals[COW]);
            }
        }
    }
}

_Bool exchange_animal(syn_game *this, player* currentPlayer, animalTypes in, animalTypes out) {
    if (currentPlayer->playerAnimals[in] >= this->game->shop.prices[out]) {
        exchange_shop(&this->game->shop, currentPlayer, in, out);
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

//Synchronization part
void syn_game_init(syn_game* this, int number_of_players) {
    this->number_of_players = number_of_players;
    this->current_index = 0;
    pthread_mutex_init(&this->mut, NULL);
    players_cond = malloc(sizeof(pthread_cond_t) * this->number_of_players);
    
    for(int i = 0; i < this->number_of_players; ++i) {
        pthread_cond_init(&this->players_cond[i], NULL);
    }
    
    game_init(this->game, this->number_of_players);
}

void syn_game_destroy(syn_game* this) {
    pthread_mutex_destroy(&this->mut);
    for(int i = 0; i < this->number_of_players; ++i) {
        pthread_cond_destroy(&this->players_cond[i]);
        destroy_player(&players[i]);
    }
    free(players_cond);
    free(players);
}