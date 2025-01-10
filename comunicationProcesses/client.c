#include "client.h"
#include "pipe.h"

#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE 256
#define SERVER_PIPE "server_pipe"


typedef struct {
    char name[BUFFER_SIZE];
    char pipePath[BUFFER_SIZE];
    int  pipeFd;
    int  serverFd;
    atomic_bool running;
} ClientData;


static void* reader_thread(void* arg) {
    ClientData *cd = (ClientData*) arg;

    while (atomic_load(&cd->running)) {
        char buf[BUFFER_SIZE];
        memset(buf, 0, sizeof(buf));
        ssize_t n = read(cd->pipeFd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            
            printf("%s", buf);
            if (buf[n - 1] != '\n') {
                printf("\n");
            }
            fflush(stdout);
            
            if (strcmp(buf, "shutdown") == 0) {
            printf("[CLIENT %s] Shutdown received, exiting.\n", cd->name);
            atomic_store(&cd->running, 0);
            }
        } 
        else if (n == 0) {
            printf("[CLIENT %s] Server closed my pipe => end.\n", cd->name);
            fflush(stdout);
            atomic_store(&cd->running, 0);
        } 
        else {
            perror("[CLIENT read]");
            atomic_store(&cd->running, 0);
        }
    }
    return NULL;
}


int client_main(const char *clientName) {
    ClientData cd;
    memset(&cd, 0, sizeof(cd));

    cd.running = 1;
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

    while (atomic_load(&cd.running)) {
        printf("[CLIENT %s] > ", cd.name);
        fflush(stdout);

        char line[BUFFER_SIZE];
        if (!fgets(line, sizeof(line), stdin)) {
            printf("[CLIENT %s] Stdin closed => exit.\n", cd.name);
            atomic_store(&cd.running, 0);
        }
        line[strcspn(line, "\n")] = '\0';

        if (strcmp(line, "quit") == 0) {
            char msg[BUFFER_SIZE];
            snprintf(msg, sizeof(msg), "%s quit", cd.name);
            write(cd.serverFd, msg, strlen(msg));
            printf("[CLIENT %s] Quitting.\n", cd.name);
            atomic_store(&cd.running, 0);
        }
        if (strcmp(line, "shutdown") == 0) {
            char msg[BUFFER_SIZE];
            snprintf(msg, sizeof(msg), "%s shutdown", cd.name);
            write(cd.serverFd, msg, strlen(msg));
        }
        if (atomic_load(&cd.running)) {
            char msg[BUFFER_SIZE];
            snprintf(msg, sizeof(msg), "%s %s", cd.name, line);
            write(cd.serverFd, msg, strlen(msg));
        }    
    }

    printf("End of this player");
    fflush(stdout);
    pthread_join(tid, NULL);

    printf("End of player read thread");
    fflush(stdout);

    pipe_close(cd.pipeFd);
    pipe_close(cd.serverFd);

    pipe_destroy(cd.pipePath);

    printf("[CLIENT %s] Exited.\n", cd.name);
    return 0;
}
