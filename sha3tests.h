#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "sha3pow.h"

const char* g_seed = "123456789";
int difficulty = 5;

void test_hash() {
    uint8_t expected[32] = {135, 205, 8, 77, 25, 14, 67, 111, 20, 115, 34, 185, 14, 115, 132, 246, 168, 224, 103, 108, 153, 210, 30, 245, 25, 234, 113, 142, 81, 212, 95, 156};
    uint8_t hash[32];

    printf("Running hash test...");
    sha3(g_seed, strlen(g_seed), hash, 32);

    assert(!memcmp(hash, expected, 32));
    printf("success!\n");
}

void test_challenge() {
    uint8_t expected[33] = {57, 209, 218, 31, 79, 159, 218, 117, 172, 44, 11, 41, 183, 108, 33, 73, 254, 87, 37, 110, 50, 64, 206, 53, 225, 231, 77, 107, 109, 137, 130, 34, 5}; 
    uint8_t challenge[33], seed[32];

    printf("Running challenge test...");
    memcpy(seed, g_seed, sizeof(seed));
    sha3(g_seed, strlen(g_seed), challenge, 32);
    pow_gen(difficulty, challenge, seed);

    assert(!memcmp(challenge, expected, 33));
    printf("success!\n");
}

void test_attempts() {
    int expected_tries = 37;
    int tries = 0;
    uint8_t challenge[33], seed[32], answer[32];

    printf("Running attempts test...");
    sha3(g_seed, strlen(g_seed), challenge, 32);
    memcpy(seed, g_seed, sizeof(seed));
    pow_gen(difficulty, challenge, seed);
    while (!pow_try(challenge, answer, seed)) { tries++; }

    assert(tries == expected_tries);
    printf("success!\n");
}

void test_answer() {
    uint8_t expected[32] = {11, 2, 131, 249, 119, 53, 202, 246, 84, 91, 141, 238, 205, 230, 54, 176, 169, 122, 57, 79, 103, 151, 239, 110, 76, 191, 97, 218, 185, 99, 246, 186};
    int tries = 0;
    uint8_t challenge[33], seed[32], answer[32], result[64];

    printf("Running answer test...");
    sha3(g_seed, strlen(g_seed), challenge, 32);
    memcpy(seed, g_seed, sizeof(seed));
    pow_gen(difficulty, challenge, seed);
    while (!pow_try(challenge, answer, seed)) { tries++; }
    memcpy(result, challenge, 32);
    memcpy(&result[32], answer, 32);
    sha3(result, 64, answer, 32);

    assert(!memcmp(answer, expected, 32));
    printf("success!\n");
}

int run_tests() {
    test_hash();
    test_challenge();
    test_attempts();
    test_answer();
    return 0;
}
