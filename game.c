#include "game.h"

void syn_players_init(Syn_players* this, int number_of_players) {
    this->number_of_players = number_of_players;
    this->current_index = 0;
    pthread_mutex_init(&this->mut, NULL);
    players_cond = malloc(sizeof(pthread_cond_t) * this->number_of_players);
    players = malloc(sizeof(Player) * this->number_of_players);
    for(int i = 0; i < this->number_of_players; ++i) {
        pthread_cond_init(&this->players_cond[i], NULL);
        init_player(&players[i]);
    }

}

void syn_players_destroy(Syn_players* this) {
    pthread_mutex_destroy(&this->mut);
    for(int i = 0; i < this->number_of_players; ++i) {
        pthread_cond_destroy(&this->players_cond[i]);
        destroy_player(&players[i]);
    }
    free(players_cond);
    free(players);
}