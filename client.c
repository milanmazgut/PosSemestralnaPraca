#include "client.h"
#include "pipe.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE 256
#define SERVER_PIPE "server_pipe"

#define RESET   "\033[0m"
#define GREEN   "\033[32m"   // Zelená farba pre trávu
#define BROWN   "\033[38;5;94m"
#define RED     "\033[31m"

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
            
            if (strncmp(buf, "shutdown",8) == 0) {
                atomic_store(&cd->running, 0);
                printf("Game has ended press enter to exit.\n");
                fflush(stdout);
                break;
            }
            
            printf("%s", buf);
            if (buf[n - 1] != '\n') {
                printf("\n");
            }
            

            fflush(stdout);
        } 
        else if (n == 0) {
            atomic_store(&cd->running, 0);
        } 
        else {
            perror("[CLIENT read]");
            atomic_store(&cd->running, 0);
        }
        printf("> ");
        fflush(stdout);
    }
    return NULL;
}


int client_main(const char *clientName) {
    ClientData cd;
    memset(&cd, 0, sizeof(cd));

    atomic_init(&cd.running, 1);
    strncpy(cd.name, clientName, BUFFER_SIZE - 1);
    cd.name[BUFFER_SIZE - 1] = '\0';

    snprintf(cd.pipePath, sizeof(cd.pipePath), "client_%s", cd.name);

    printf("\033[32m");

    printf("  ____                           __                        __       \n");
    printf(" / ___| _   _ _ __   ___ _ __   / _| __ _ _ __ _ __ ___   /_/_ _ __ \n");
    printf(" \\___ \\| | | | '_ \\ / _ \\ '__| | |_ / _` | '__| '_ ` _ \\ / _` | '__|\n");
    printf("  ___) | |_| | |_) |  __/ |    |  _| (_| | |  | | | | | | (_| | |   \n");
    printf(" |____/ \\__,_| .__/ \\___|_|    |_|  \\__,_|_|  |_| |_| |_|\\__,_|_|   \n");
    printf("             |_|                                                    \n");
    
    printf("\033[0m");

        printf(RED "                               _.-^-._    .--.\033[0m\n");
    printf(RED "                            .-'   \033[0m");
    printf("_");
    printf(RED"   '-. |__|\033[0m\n");
    printf(RED "                           /     \033[0m");
    printf("|_|");
    printf(RED"     \\|  |\033[0m\n");
    printf(RED "                          /               \\  |\033[0m\n");
    printf(RED "                         /|     _____     |\\ |\033[0m\n");
    printf(RED "                          |    |==|==|    |  |\033[0m\n");
    printf(BROWN "      |---|---|---|---|---\033[0m");
    printf(RED "|    |--|--|    |  |\033[0m\n");
    printf(BROWN "      |---|---|---|---|---\033[0m");
    printf(RED "|    |==|==|    |  |\033[0m\n");
    printf(GREEN "     ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\033[0m\n");
    
 


    printf("Welcome to game Super Farmar, your goal is to have atleast one of each animal from rabbit, sheep, cow and horse.\n");
    printf("To start of you have one rabbit, to get more animals you can roll dice, trade animals in shop\n"
                    "or for each pair of the same animal you will get one more of that type at the end of the turn\n");
    printf("DICE: If you roll same animal on both of the dice, it will join your farm, however bevare the fox and wolf,\n");
    printf("which will eat all your rabbits for fox and sheep, pigs and cows for wolf if thrown on the dice\n");
    printf("DOGS: To protect yourself you can protect your farm by acquiring small dog (fox) or big dog (wolf),\n");
    printf("you need to have them before you are rolling and you can have only one of each dog which you can trade\n");
    printf("in the shop 1 sheep for 1 small dog and 1 cow for 1 big dog. (You do not need dogs to win the game)\n");
    pipe_init(cd.pipePath);

    cd.serverFd = pipe_open_write(SERVER_PIPE);

    char joinmsg[BUFFER_SIZE];
    snprintf(joinmsg, sizeof(joinmsg), "%s join", cd.name);
    write(cd.serverFd, joinmsg, strlen(joinmsg));
    
    printf("[CLIENT %s] Opening '%s' \n", cd.name, cd.pipePath);
    cd.pipeFd = pipe_open_read(cd.pipePath);
    
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
        printf("> ");
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
            atomic_store(&cd.running, 0);
        }
        if (atomic_load(&cd.running)) {
            char msg[BUFFER_SIZE];
            snprintf(msg, sizeof(msg), "%s %s", cd.name, line);
            write(cd.serverFd, msg, strlen(msg));
        }    
    }
    pthread_join(tid, NULL);

    pipe_close(cd.pipeFd);
    pipe_close(cd.serverFd);
    pipe_destroy(cd.pipePath);

    
    return 0;
}
