#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"
#include "client.h"
#include "syn_game.h"



int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage:\n");
        printf("  %s server\n", argv[0]);
        printf("  %s client <Name>\n", argv[0]);
        return 1;
    }
    
        if (strcmp(argv[1], "server") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Missing number of required clients.\n");
            return 1;
        }
        return server_main(atoi(argv[2]));
    }
    else if (strcmp(argv[1], "client") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Missing client name.\n");
            return 1;
        }
        return client_main(argv[2]);
    }
    else {
        printf("Unknown mode '%s'. Use 'server' or 'client'.\n", argv[1]);
        return 1;
    }
    
}
