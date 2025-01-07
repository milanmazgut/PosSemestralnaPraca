#include "game.h"
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
    this->players = malloc(sizeof(player) * playerCount);
    for(int i = 0; i < this->playerCount; ++i) {
        player_init(&this->players[i], i);
    }
    shop_init(&this->shop, prices, animalCounts);
}

void game_destroy(game *this) {
    for(int i = 0; i < this->playerCount; ++i) {
        player_destroy(&this->players[i]);
    }
    shop_destroy(&this->shop);
    dice_destroy(&this->dice_1);
    dice_destroy(&this->dice_2);
}



void player_roll_dice(game *this, player* currentPlayer) {
    animalTypesDice dice_1;
    animalTypesDice dice_2;
    roll_dice(&this->dice_1, &dice_1);
    roll_dice(&this->dice_2, &dice_2);

    if ( dice_1 == dice_2) {
        change_animal_ownership(&this->shop, currentPlayer, dice_1, 1);
    } else {
        if(dice_1 == FOX || dice_2 == FOX) {
            if (currentPlayer->playerAnimals[SMALL_DOG] == 1) {
                change_animal_ownership(&this->shop, currentPlayer, SMALL_DOG, -1);
            } else {
                change_animal_ownership(&this->shop, currentPlayer, RABBIT, -currentPlayer->playerAnimals[RABBIT]);
            }
        }
        if(dice_1 == WOLF || dice_2 == WOLF) {
            if (currentPlayer->playerAnimals[BIG_DOG] == 1) {
                change_animal_ownership(&this->shop, currentPlayer, BIG_DOG, -1);
            } else {
                change_animal_ownership(&this->shop, currentPlayer, SHEEP,currentPlayer->playerAnimals[SHEEP]);
                change_animal_ownership(&this->shop, currentPlayer, PIG,currentPlayer->playerAnimals[PIG]);
                change_animal_ownership(&this->shop, currentPlayer, COW,currentPlayer->playerAnimals[COW]);
            }
        }
    }
}

_Bool exchange_animal(game *this, player* currentPlayer, animalTypesShop in, animalTypesShop out) {
    if (currentPlayer->playerAnimals[in] >= this->shop.prices[out]) {
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

//Synchronization part
void syn_game_init(syn_game* this, int number_of_players) {
    this->number_of_players = number_of_players;
    this->current_index = 0;
    pthread_mutex_init(&this->mut, NULL);
    this->players_cond = malloc(sizeof(pthread_cond_t) * this->number_of_players);
    
    for(int i = 0; i < this->number_of_players; ++i) {
        pthread_cond_init(&this->players_cond[i], NULL);
    }
    this->game = malloc(sizeof(game));
    if(this->game == NULL) {
        perror("Failed to allocate memory for game");
        exit(EXIT_FAILURE);
    }
    game_init(this->game, this->number_of_players);

}

void syn_game_destroy(syn_game* this) {
    pthread_mutex_destroy(&this->mut);
    for(int i = 0; i < this->number_of_players; ++i) {
        pthread_cond_destroy(&this->players_cond[i]);
    }
    free(this->players_cond);
    game_destroy(this->game);
    free(this->game);
}

void syn_turn(syn_game* this, int playerIndex) {
    pthread_mutex_lock(&this->mut);
    while(playerIndex != this->current_index) {
        pthread_cond_wait(&this->players_cond[playerIndex], &this->mut);
    }
    printf("PLAYER%d\n", playerIndex);
    printf("Pick your action: \n1. Roll the dice \n2. Exchange animals in the shop\n");
    int choice;
    _Bool valid = false;
    do {
        printf("Enter your choice (1 or 2): ");
        if (scanf("%d", &choice) == 1) {
            if (choice == 1) {
                printf("You chose to roll the dice.\n");
                player_roll_dice(this->game, &this->game->players[playerIndex]);
                valid = true;
            } else if (choice == 2) {
                printf("You chose to exchange animals in the shop.\n");
                //eto bleh
                valid = true;
            } else {
                printf("Invalid choice. Please enter 1 or 2.\n");
            }
        } else {
            printf("Invalid input. Please enter a number.\n");
        }
    } while(!valid);
    
    this->current_index++;
    this->current_index = this->current_index % this->number_of_players;
    pthread_cond_signal(&this->players_cond[(this->current_index)]);
    pthread_mutex_unlock(&this->mut);
}

typedef struct data {
    syn_game* syn_game_;
    int index_;
} data;

void* play(void* arg) {
    data* this = arg;
    syn_turn(this->syn_game_, this->index_);
    free(this);
    return NULL;
}

void start(int number_of_players) {
    int n = number_of_players;
    syn_game program;
    syn_game_init(&program, n);
    pthread_t players[n];
    for(int i = 0; i < n; ++i) {
        data* thread_data = malloc(sizeof(data));
        if (!thread_data) {
            perror("Failed to allocate memory for thread data");
            exit(EXIT_FAILURE);
        }
        thread_data->syn_game_ = &program;
        thread_data->index_ = i;
        pthread_create(&players[i], NULL, play, thread_data);
    }

    for(int i = 0; i < n; ++i) {
        pthread_join(players[i], NULL);
    }
    syn_game_destroy(&program);
}
