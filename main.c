#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "sha3pow.h"
#include "sha3tests.h"

int main(int argc, char **argv) {

    size_t len = 32;
    long long int tentativa = 0;

    uint8_t answer[32];
    uint8_t challenge[33];
    uint8_t rand[32];
    
    memset(challenge, 0, 33);
    memset(rand, 0, 32);
    memset(answer, 0, 32);

    if (argc < 3) {
        printf("usage: %s <seed> <difficulty> [--test]", argv[0]);
        return -1;
    } else if (argc == 4) {
        if (!strcmp(argv[3], "--test")) {
            run_tests();
        } else {
            printf("usage: %s <seed> <difficulty> [--test]", argv[0]);
            return -1;
        }
    } else {
        memcpy(rand, argv[1], strlen(argv[1]));
        sha3(rand, 32, challenge, 32);
        printf("[HASH] SHA3-256:\n");
        for(int i=0; i < len; i++) printf("%02X", challenge[i]);
        printf("\n");
 
        pow_gen(atoi(argv[2]), challenge, rand, sizeof(rand));
        printf("[CHALLENGE] SHA3-%lu:\n", len * 8);
        for(int i=0; i < len; i++) printf("%02X", challenge[i]);
        printf("\nDificuldade: %d\n", challenge[32]);

        while (!pow_try(challenge, answer, rand)) {
    //        for(int i=0; i < len; i++) printf("%02X", rand[i]);
    //        printf("\n");
            tentativa++;
        }
        printf("Tentativas: %lli\n", tentativa);

        uint8_t ans[64];
        memcpy(ans, challenge, 32);
        memcpy(&ans[32], answer, 32);
        sha3(ans, 64, answer, 32);

        printf("[ANSWER]:\n");
        for(int i=0; i < len; i++) printf("%02X", answer[i]);
        printf("\n");    

        return 0;
    }
}
