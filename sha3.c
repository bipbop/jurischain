#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stddef.h>
#include <stdint.h>

#ifndef KECCAKF_ROUNDS
#define KECCAKF_ROUNDS 24
#endif

#ifndef ROTL64
#define ROTL64(x, y) (((x) << (y)) | ((x) >> (64 - (y))))
#endif

#define memcpy __builtin_memcpy

// state context
typedef struct {
    union {                                 // state:
        uint8_t b[200];                     // 8-bit bytes
        uint64_t q[25];                     // 64-bit words
    } st;
    int pt, rsiz, mdlen;                    // these don't overflow
} sha3_ctx_t;

// global rand num
uint8_t rand_hash[32];

// Compression function
void sha3_keccakf(uint64_t st[25]);

// OpenSSL - like interfece
int sha3_init(sha3_ctx_t *c, int mdlen);    // mdlen = hash output in bytes
int sha3_update(sha3_ctx_t *c, const void *data, size_t len);
int sha3_final(void *md, sha3_ctx_t *c);    // digest goes to md

// compute a sha3 hash (md) of given byte length from "in"
void *sha3(const void *in, size_t inlen, void *md, int mdlen);

// SHAKE128 and SHAKE256 extensible-output functions
#define shake128_init(c) sha3_init(c, 16)
#define shake256_init(c) sha3_init(c, 32)
#define shake_update sha3_update

void shake_xof(sha3_ctx_t *c);
void shake_out(sha3_ctx_t *c, void *out, size_t len);

void sha3_keccakf(uint64_t st[25])
{
    // constants
    const uint64_t keccakf_rndc[24] = {
        0x0000000000000001, 0x0000000000008082, 0x800000000000808a,
        0x8000000080008000, 0x000000000000808b, 0x0000000080000001,
        0x8000000080008081, 0x8000000000008009, 0x000000000000008a,
        0x0000000000000088, 0x0000000080008009, 0x000000008000000a,
        0x000000008000808b, 0x800000000000008b, 0x8000000000008089,
        0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
        0x000000000000800a, 0x800000008000000a, 0x8000000080008081,
        0x8000000000008080, 0x0000000080000001, 0x8000000080008008
    };
    const int keccakf_rotc[24] = {
        1,  3,  6,  10, 15, 21, 28, 36, 45, 55, 2,  14,
        27, 41, 56, 8,  25, 43, 62, 18, 39, 61, 20, 44
    };
    const int keccakf_piln[24] = {
        10, 7,  11, 17, 18, 3, 5,  16, 8,  21, 24, 4,
        15, 23, 19, 13, 12, 2, 20, 14, 22, 9,  6,  1
    };

    // variables
    int i, j, r;
    uint64_t t, bc[5];

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
    uint8_t *v;

    // endianess conversion. this is redundant on little-endian targets
    for (i = 0; i < 25; i++) {
        v = (uint8_t *) &st[i];
        st[i] = ((uint64_t) v[0])     | (((uint64_t) v[1]) << 8) |
            (((uint64_t) v[2]) << 16) | (((uint64_t) v[3]) << 24) |
            (((uint64_t) v[4]) << 32) | (((uint64_t) v[5]) << 40) |
            (((uint64_t) v[6]) << 48) | (((uint64_t) v[7]) << 56);
    }
#endif

    // actual iteration
    for (r = 0; r < KECCAKF_ROUNDS; r++) {

        // Theta
        for (i = 0; i < 5; i++)
            bc[i] = st[i] ^ st[i + 5] ^ st[i + 10] ^ st[i + 15] ^ st[i + 20];

        for (i = 0; i < 5; i++) {
            t = bc[(i + 4) % 5] ^ ROTL64(bc[(i + 1) % 5], 1);
            for (j = 0; j < 25; j += 5)
                st[j + i] ^= t;
        }

        // Rho Pi
        t = st[1];
        for (i = 0; i < 24; i++) {
            j = keccakf_piln[i];
            bc[0] = st[j];
            st[j] = ROTL64(t, keccakf_rotc[i]);
            t = bc[0];
        }

        //  Chi
        for (j = 0; j < 25; j += 5) {
            for (i = 0; i < 5; i++)
                bc[i] = st[j + i];
            for (i = 0; i < 5; i++)
                st[j + i] ^= (~bc[(i + 1) % 5]) & bc[(i + 2) % 5];
        }

        //  Iota
        st[0] ^= keccakf_rndc[r];
    }

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#error WebAssembly should be little endian
    // endianess conversion. this is redundant on little-endian targets
    for (i = 0; i < 25; i++) {
        v = (uint8_t *) &st[i];
        t = st[i];
        v[0] = t & 0xFF;
        v[1] = (t >> 8) & 0xFF;
        v[2] = (t >> 16) & 0xFF;
        v[3] = (t >> 24) & 0xFF;
        v[4] = (t >> 32) & 0xFF;
        v[5] = (t >> 40) & 0xFF;
        v[6] = (t >> 48) & 0xFF;
        v[7] = (t >> 56) & 0xFF;
    }
#endif
}

// Initialize the context for SHA3
int sha3_init(sha3_ctx_t *c, int mdlen)
{
    int i;

    for (i = 0; i < 25; i++)
        c->st.q[i] = 0;
    c->mdlen = mdlen;
    c->rsiz = 200 - 2 * mdlen;
    c->pt = 0;

    return 1;
}

// update state with more data
int sha3_update(sha3_ctx_t *c, const void *data, size_t len)
{
    size_t i;
    int j;

    j = c->pt;
    for (i = 0; i < len; i++) {
        c->st.b[j++] ^= ((const uint8_t *) data)[i];
        if (j >= c->rsiz) {
            sha3_keccakf(c->st.q);
            j = 0;
        }
    }
    c->pt = j;

    return 1;
}

// finalize and output a hash
int sha3_final(void *md, sha3_ctx_t *c)
{
    int i;

    c->st.b[c->pt] ^= 0x06;
    c->st.b[c->rsiz - 1] ^= 0x80;
    sha3_keccakf(c->st.q);

    for (i = 0; i < c->mdlen; i++) {
        ((uint8_t *) md)[i] = c->st.b[i];
    }

    return 1;
}

// compute a SHA-3 hash (md) of given byte length from "in"
void *sha3(const void *in, size_t inlen, void *md, int mdlen)
{
    sha3_ctx_t sha3;

    sha3_init(&sha3, mdlen);
    sha3_update(&sha3, in, inlen);
    sha3_final(md, &sha3);

    return md;
}

// pow_genrand
uint8_t *pow_genrand() {
    sha3(rand_hash, 32, rand_hash, 32);
    return rand_hash;
}

// pow_initrand
uint8_t *pow_initrand(const char* seed) {
    sha3(seed, strlen(seed), rand_hash, 32);
    return rand_hash;
}

// pow_gen
uint8_t *pow_gen(uint8_t d, uint8_t *challenge) {
    pow_genrand();
    
    // for(int i=0; i < 32; i++)  challenge[i] = rand_hash[i]; // substituir por memcpy
    memcpy(challenge, rand_hash, sizeof(rand_hash));
    challenge[32] = d;

    return challenge;    
}

// pow_verify
int pow_verify(uint8_t challenge[static 33], uint8_t answer[static 32]) {
    uint8_t hash[32], d, hash_concat[64], response[32];

    // for(int i=0; i < 32; i++) hash[i] = hash_concat[i] = challenge[i];
    memcpy(hash, challenge, 32);
    memcpy(hash_concat, challenge, 32);

    // for(int i=32; i < 64; i++) hash_concat[i] = answer[i-32];
    memcpy(&hash_concat[32], answer, 32);
    d = challenge[32];
    
    sha3(hash_concat, 64, response, 32);
    
    // checar se os primeiros bits estão corretos
    uint64_t mask = 0xFFFFFFFFFFFFFFFF >> (64 - (d % 64));
    uint64_t* res64 = (uint64_t *) response;  // webassembly assumes little endian
    int valid = 1;

    for (int i = 0; i < (d/64); i++) valid = (res64[i] == 0) ? valid : 0;
    if ((d % 64) != 0) valid = ((res64[d/64] & mask) == 0) ? valid : 0;

    return valid;
}

// pow_try
int pow_try(uint8_t *challenge, uint8_t *answer) {
    pow_genrand();

    
    if(pow_verify(challenge, rand_hash)) {
       // for(int i=0; i < 32; i++)  answer[i] = rand_hash[i];
        memcpy(answer, rand_hash, sizeof(rand_hash));
        return 1;
    } else {
        answer = NULL;
        return 0;
    }
}

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

    // printf("\n[ANSWER]: ");
    // for(int i=0; i < len; i++) printf("%02X", answer[i]);
    // printf("\n");    

    return 0;
}
