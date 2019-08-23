#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stddef.h>
#include <stdint.h>
#include "sha3pow.h"

// main
int main(int argc, char **argv) {

    size_t len = 32;
    long long int tentativa = 0;

    uint8_t hash[32], answer[32];
    uint8_t challenge[33];

    if (argc < 3) {
        printf("usage: %s <message> <seed> <difficulty>", argv[0]);
        return 0;
    }

    sha3(argv[1], strlen(argv[1]), hash, len);
    pow_initrand(argv[2]);

    pow_gen(atoi(argv[3]), challenge);
    printf("[CHALLENGE] SHA3-%lu:\n", len);
    for(int i=0; i < len; i++) printf("%02X", challenge[i]);
    printf("\nDificuldade: %d\n", challenge[32]);

    while (!pow_try(challenge, answer)) {
        tentativa++;
    }
    printf("Tentativas: %lli\n", tentativa);


    uint8_t ans[64];
    memcpy(ans, challenge, 32);
    memcpy(&ans[32], answer, 32);
    sha3(ans, 64, answer, 32);

    printf("\n[ANSWER]: ");
    for(int i=0; i < len; i++) printf("%02X", answer[i]);
    printf("\n");    

    return 0;
}