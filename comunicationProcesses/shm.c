#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "shm.h"

void shm_init(shared_names *names, int number_of_players) {
    const int fd_shm = shm_open(names->shm_name_, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd_shm == -1) {
      if(errno == EEXIST) {
        shm_unlink(names->shm_name_);
        shm_init(names, number_of_players);
      } else {
        perror("Failed to create shared memory");
        exit(EXIT_FAILURE);
      }
    }
    if (ftruncate(fd_shm, sizeof(game)) == -1) {
        perror("Failed to truncate shared memory");
        exit(EXIT_FAILURE);
    }
    game *g =mmap(NULL, sizeof(game), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    game_init(g, number_of_players);
    shm_game_close(fd_shm, g);

}

void shm_destroy(shared_names *names) {
  if (shm_unlink(names->shm_name_) == -1) {
    perror("Failed to unlink shared memory");
    exit(EXIT_FAILURE);
  }
}

void shm_game_open(shared_names *names, game **out_game, int *out_fd_shm) {
  const int fd_shm = shm_open(names->shm_name_, O_RDWR, 0);
  if (fd_shm == -1) {
    perror("Failed to open shared memory");
    exit(EXIT_FAILURE);
  }
  game *g = mmap(NULL, sizeof(game), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
  if (g == MAP_FAILED) {
    perror("Failed to map shared memory");
    exit(EXIT_FAILURE);
  }
  *out_fd_shm = fd_shm;
  *out_game = g;
}

void shm_game_close(int fd_shm, game *g) {
  if (munmap(g, sizeof(game)) == -1) {
    perror("Failed to unmap shared memory");
    exit(EXIT_FAILURE);
  }
  if (close(fd_shm) == -1) {
    perror("Failed to close shared memory");
    exit(EXIT_FAILURE);
  }
}