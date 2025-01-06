#include <stdio.h>
#include "game.h"

int main(int argc, char* argv[]) {
    int n = 5;
    if (argc > 1) {
        n = atoi(argv[1]);
    }
    start(n);
    return 0;
}