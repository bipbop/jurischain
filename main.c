#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#include "juschain.h"

static char *defaultSeed = "W235XX";

int main(int argc, char *argv[]) {
  pow_ctx_t challenge;

  size_t tries = 0;
  int i = 0;
  uint8_t difficulty = argc > 1 ? MAX(atoi(argv[1]), 1) : 1;
  char *initialSeed = argc > 2 ? argv[2] : defaultSeed;

  printf("Difficulty: %u\nChallenge: %s\r\n", difficulty, initialSeed);

  pow_gen(&challenge, difficulty, initialSeed,
          strlen(initialSeed) * sizeof(char));
  while (!pow_try(&challenge))
    tries++;

  printf("[RESPONSE] ");
  for (i = 0; i < HASH_LEN; i++)
    printf("%02X", challenge.seed[i]);
  printf("\r\nTries: %lu\r\n", tries);
  return !pow_verify(&challenge);
}
