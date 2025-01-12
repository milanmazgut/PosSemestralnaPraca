#ifndef PIPE_PC
#define PIPE_PC

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void pipe_init(const char *path);
void pipe_destroy(const char *path);
int pipe_open_write(const char *path);
int pipe_open_read(const char *path);
int pipe_open_read_write(const char *path);
void pipe_close(int fd);

#endif // !DEBUG