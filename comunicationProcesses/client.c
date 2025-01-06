#include <stdio.h>
#include "pipe.h"
#include <unistd.h>
#include <string.h>

int main(void) {
    int fd = pipe_open_write("server_fifo");
    char line[128];
    while(1) {
        printf("Type something: ");
        fflush(stdout);
        if(!fgets(line, sizeof(line), stdin)) {
            break;
        }
        // odosleme
        write(fd, line, strlen(line));
    }
    return 0;
}
