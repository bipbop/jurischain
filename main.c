#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#include "sha3pow.h"

static char *defaultSeed = "W235XX";

int main(int argc, char *argv[]) {
    pow_ctx_t challenge;

    int i = 0;
    size_t trys = 0; 
    uint8_t difficulty = argc > 1 ? MAX(atoi(argv[1]), 1) : 1;
    char * initialSeed = argc > 2 && strlen(argv[2]) ? argv[2] : defaultSeed;

    printf("Difficulty: %u Challenge: %s\r\n", difficulty, initialSeed);

    pow_gen(&challenge, difficulty, initialSeed, strlen(initialSeed) * sizeof(char));
    while (!pow_try(&challenge)) trys++;

    printf("[RESPONSE] ");
    for (i=0; i < HASH_LEN; i++) printf("%02X", challenge.seed[i]);
    printf("\r\nTrys: %lu\r\n", trys);

    return !!pow_verify(&challenge);
}
