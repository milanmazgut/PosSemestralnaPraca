#ifndef SHM 
#define SHM 
#include "names.h"
#include "shop.h"

void shm_init(sharedn_names *names);
void shm_destroy(shared_names *names);
void shm_game_open(shared_names *names, game **out_game, int *out_fd_shm);
void shm_game_close(int fd_shm, game *g);
#endif