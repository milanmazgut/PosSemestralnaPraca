#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#include "dice.h"
#include "shop.h"
#include "player.h"

typedef struct Syn_players {
    int number_of_players;
    int current_index;
    pthread_mutex_t mut;
    pthread_cond_t[] players_cond;
    Player[] players;
} Syn_players;

void syn_players_init(Syn_players* this, int number_of_players);
void syn_players_destroy(Syn_players* this);