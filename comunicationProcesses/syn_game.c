#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "syn_game.h"
#include "shm.h"

void syn_shm_game_init(synchronized_game* this, int player_count ,shared_names *names) {
  printf("Creating semaphore with name: %s\n", names->mut_pc_);
  fflush(stdout);
  
  this->mut_pc_ = sem_open(names->mut_pc_, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
  if (this->mut_pc_ == SEM_FAILED) {
    perror("Failed to open mutex PC");
    exit(EXIT_FAILURE);
  }
  game_init(this->game_, player_count);
}

void syn_shm_game_destroy(shared_names *names) {
  if (sem_unlink(names->mut_pc_) == -1) {
    perror("Failed to unlink mit PC");
    exit(EXIT_FAILURE);
  }
}

void syn_shm_game_open(synchronized_game *this, shared_names *names) {
  shm_game_open(names, &this->game_, &this->game_fd_);
  this->mut_pc_ = sem_open(names->mut_pc_, O_RDWR);
  if (this->mut_pc_ == SEM_FAILED) {
    perror("Failed to open mut PC");
    exit(EXIT_FAILURE);
  }
}

void syn_shm_game_close(synchronized_game *this) {
    shm_game_close(this->game_fd_, this->game_);
    if (sem_close(this->mut_pc_) == -1) {
        perror("Failed to close mut PC");
        exit(EXIT_FAILURE);
    }
}

void syn_shm_game_player_roll_dice(synchronized_game *this, player* currentPlayer, char* output) {
    /*
    printf("Milan PC guru");
    fflush(stdout);
    printf("Semaphore address: %p\n", this->mut_pc_);
    fflush(stdout);
    sem_wait(this->mut_pc_);
    printf("locked");
    fflush(stdout);
    player_roll_dice(this->game_, currentPlayer, output);
    sem_post(this->mut_pc_);
    printf("unlocked");
    fflush(stdout);
    */
    printf("Tu som nigga\n");
    fflush(stdout);
    sem_wait(this->mut_pc_);  // Lock the semaphore
    printf("Tu som nigga2\n");
    fflush(stdout);
    player_roll_dice(this->game_, currentPlayer, output);
    printf("Tu som nigga3\n");
    fflush(stdout);
    sem_post(this->mut_pc_);
    printf("Tu som nigga4\n");
    fflush(stdout);
}

_Bool syn_shm_game_exchange_animal(synchronized_game *this, player* currentPlayer, animalTypes in, animalTypes out) {
    sem_wait(this->mut_pc_);
    exchange_animal(this->game_, currentPlayer, in, out);
    sem_post(this->mut_pc_);
}

void syn_shm_game_end_of_turn_animal_multiplication(synchronized_game *this, player* currentPlayer) {
    sem_wait(this->mut_pc_);
    end_of_turn_animal_multiplication(this->game_, currentPlayer);
    sem_post(this->mut_pc_);
}

int** syn_shm_game_view_shop(synchronized_game *this) {
    sem_wait(this->mut_pc_);
    int** n = view_shop(this->game_);
    sem_post(this->mut_pc_);
    return n;
}

