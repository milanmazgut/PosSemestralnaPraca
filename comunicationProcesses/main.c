#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"
#include "client.h"
#include "syn_game.h"

char* add_suffix(const char* name, const char* suffix) {
    int name_len = strlen(name);
    int suffix_len = strlen(suffix);
    char* result = malloc(name_len + suffix_len + 2 * sizeof(char));
    strcpy(result, name);
    result[name_len] = '-';
    strcpy(result + name_len + 1, suffix);
    return result;
}

void clear_names(shared_names* names) {
    free(names->shm_name_);
    free(names->mut_pc_);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage:\n");
        printf("  %s server\n", argv[0]);
        printf("  %s client <Name>\n", argv[0]);
        return 1;
    }
    shared_names names;
    names.shm_name_ = add_suffix("SHM", "farma");
    names.mut_pc_ = add_suffix("MUT-PC", "farma");
        if (strcmp(argv[1], "server") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Missing number of required clients.\n");
            return 1;
        }
        return server_main(atoi(argv[2]), names);
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
    clear_names(&names);
}
