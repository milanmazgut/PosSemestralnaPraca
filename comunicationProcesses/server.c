#include <stdio.h>
#include "pipe.h"
#include <unistd.h>
#include <string.h>

int main(void) {
    pipe_init("server_fifo");
    int fd = pipe_open_read("server_fifo");
    char buf[128];
    while (1) {
        ssize_t r = read(fd, buf, sizeof(buf)-1);
        if (r > 0) {
            buf[r] = '\0';
            printf("Server got: '%s'\n", buf);
        } else if (r == 0) {
            pipe_close(fd);
            fd = pipe_open_read("server_fifo");
        }
    }
    return 0;
}
