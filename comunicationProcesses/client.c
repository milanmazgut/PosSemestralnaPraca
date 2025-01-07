#include "client.h"
#include "pipe.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define BUFFER_SIZE 256
#define SERVER_PIPE "server_pipe"


typedef struct {
    char name[BUFFER_SIZE];
    char pipePath[BUFFER_SIZE];
    int  pipeFd;
    int  serverFd;
} ClientData;


static void* reader_thread(void* arg) {
    ClientData *cd = (ClientData*) arg;

    while (1) {
        char buf[BUFFER_SIZE];
        memset(buf, 0, sizeof(buf));
        size_t n = read(cd->pipeFd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("%s", buf);
            if (buf[n - 1] != '\n') {
                printf("\n");
            }
            fflush(stdout);
        } 
        else if (n == 0) {
            printf("[CLIENT %s] Server closed my pipe => end.\n", cd->name);
            fflush(stdout);
            break;
        } 
        else {
            perror("[CLIENT read]");
            break;
        }
    }
    return NULL;
}


int client_main(const char *clientName) {
    ClientData cd;
    memset(&cd, 0, sizeof(cd));

    strncpy(cd.name, clientName, BUFFER_SIZE - 1);
    cd.name[BUFFER_SIZE - 1] = '\0';

    snprintf(cd.pipePath, sizeof(cd.pipePath), "client_%s", cd.name);

    printf("[CLIENT] Creating pipe '%s'\n", cd.pipePath);
    pipe_init(cd.pipePath);

    printf("[CLIENT %s] Opening server PIPE '%s' for writing...\n", cd.name, SERVER_PIPE);
    cd.serverFd = pipe_open_write(SERVER_PIPE);

    printf("[CLIENT %s] Opening '%s' \n", cd.name, cd.pipePath);
    cd.pipeFd = pipe_open_read_write(cd.pipePath);
    if (cd.pipeFd < 0) {
        perror("[CLIENT] open");
        return 2;
    }

    pthread_t tid;
    if (pthread_create(&tid, NULL, reader_thread, &cd) != 0) {
        perror("[CLIENT] pthread_create");
        return 3;
    }

    printf("[CLIENT %s] Commands: roll, trade, end, shutdown, quit\n", cd.name);
    fflush(stdout);

    while (1) {
        printf("[CLIENT %s] > ", cd.name);
        fflush(stdout);

        char line[BUFFER_SIZE];
        if (!fgets(line, sizeof(line), stdin)) {
            printf("[CLIENT %s] Stdin closed => exit.\n", cd.name);
            break;
        }
        line[strcspn(line, "\n")] = '\0';

        if (strcmp(line, "quit") == 0) {
            char msg[BUFFER_SIZE];
            snprintf(msg, sizeof(msg), "%s quit", cd.name);
            write(cd.serverFd, msg, strlen(msg));

            printf("[CLIENT %s] Quitting.\n", cd.name);
            break;
        }

        char msg[BUFFER_SIZE];
        snprintf(msg, sizeof(msg), "%s %s", cd.name, line);
        write(cd.serverFd, msg, strlen(msg));
    }

    pipe_close(cd.pipeFd);
    pipe_close(cd.serverFd);

    pthread_join(tid, NULL);

    pipe_destroy(cd.pipePath);

    printf("[CLIENT %s] Exited.\n", cd.name);
    return 0;
}
