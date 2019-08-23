#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>

#ifndef KECCAKF_ROUNDS
#define KECCAKF_ROUNDS 24
#endif

#ifndef ROTL64
#define ROTL64(x, y) (((x) << (y)) | ((x) >> (64 - (y))))
#endif

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

// SHAKE128 and SHAKE256 extensible-output functionality

void shake_xof(sha3_ctx_t *c)
{
    c->st.b[c->pt] ^= 0x1F;
    c->st.b[c->rsiz - 1] ^= 0x80;
    sha3_keccakf(c->st.q);
    c->pt = 0;
}

void shake_out(sha3_ctx_t *c, void *out, size_t len)
{
    size_t i;
    int j;

    j = c->pt;
    for (i = 0; i < len; i++) {
        if (j >= c->rsiz) {
            sha3_keccakf(c->st.q);
            j = 0;
        }
        ((uint8_t *) out)[i] = c->st.b[j++];
    }
    c->pt = j;
}

/* API */
void make_sha3(const char* msg, char* buffer, size_t hash_len) {
    sha3(msg, strlen(msg), buffer, hash_len);
}

static int check_sha3(uint8_t* msg, uint8_t* hash) {
    return 0;
} 

/* TESTS */
// read a hex string, return byte length or -1 on error.

static int test_hexdigit(char ch)
{
    if (ch >= '0' && ch <= '9')
        return  ch - '0';
    if (ch >= 'A' && ch <= 'F')
        return  ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return  ch - 'a' + 10;
    return -1;
}

static int test_readhex(uint8_t *buf, const char *str, int maxbytes)
{
    int i, h, l;

    for (i = 0; i < maxbytes; i++) {
        h = test_hexdigit(str[2 * i]);
        if (h < 0)
            return i;
        l = test_hexdigit(str[2 * i + 1]);
        if (l < 0)
            return i;
        buf[i] = (h << 4) + l;
    }

    return i;
}

// returns zero on success, nonzero + stderr messages on failure

int test_sha3()
{
    // message / digest pairs, lifted from ShortMsgKAT_SHA3-xxx.txt files
    // in the official package: https://github.com/gvanas/KeccakCodePackage

    const char *testvec[][2] = {
        {   // SHA3-224, corner case with 0-length message
            "",
            "6B4E03423667DBB73B6E15454F0EB1ABD4597F9A1B078E3F5B5A6BC7"
        },
        {   // SHA3-256, short message
            "9F2FCC7C90DE090D6B87CD7E9718C1EA6CB21118FC2D5DE9F97E5DB6AC1E9C10",
            "2F1A5F7159E34EA19CDDC70EBF9B81F1A66DB40615D7EAD3CC1F1B954D82A3AF"
        },
        {   // SHA3-384, exact block size
            "E35780EB9799AD4C77535D4DDB683CF33EF367715327CF4C4A58ED9CBDCDD486"
            "F669F80189D549A9364FA82A51A52654EC721BB3AAB95DCEB4A86A6AFA93826D"
            "B923517E928F33E3FBA850D45660EF83B9876ACCAFA2A9987A254B137C6E140A"
            "21691E1069413848",
            "D1C0FA85C8D183BEFF99AD9D752B263E286B477F79F0710B0103170173978133"
            "44B99DAF3BB7B1BC5E8D722BAC85943A"
        },
        {   // SHA3-512, multiblock message
            "3A3A819C48EFDE2AD914FBF00E18AB6BC4F14513AB27D0C178A188B61431E7F5"
            "623CB66B23346775D386B50E982C493ADBBFC54B9A3CD383382336A1A0B2150A"
            "15358F336D03AE18F666C7573D55C4FD181C29E6CCFDE63EA35F0ADF5885CFC0"
            "A3D84A2B2E4DD24496DB789E663170CEF74798AA1BBCD4574EA0BBA40489D764"
            "B2F83AADC66B148B4A0CD95246C127D5871C4F11418690A5DDF01246A0C80A43"
            "C70088B6183639DCFDA4125BD113A8F49EE23ED306FAAC576C3FB0C1E256671D"
            "817FC2534A52F5B439F72E424DE376F4C565CCA82307DD9EF76DA5B7C4EB7E08"
            "5172E328807C02D011FFBF33785378D79DC266F6A5BE6BB0E4A92ECEEBAEB1",
            "6E8B8BD195BDD560689AF2348BDC74AB7CD05ED8B9A57711E9BE71E9726FDA45"
            "91FEE12205EDACAF82FFBBAF16DFF9E702A708862080166C2FF6BA379BC7FFC2"
        }
    };

    int i, fails, msg_len, sha_len;
    uint8_t sha[64], buf[64], msg[512];

    fails = 0;
    for (i = 0; i < 4; i++) {

        memset(sha, 0, sizeof(sha));
        memset(buf, 0, sizeof(buf));
        memset(msg, 0, sizeof(msg));

        msg_len = test_readhex(msg, testvec[i][0], sizeof(msg));
        sha_len = test_readhex(sha, testvec[i][1], sizeof(sha));

        sha3(msg, msg_len, buf, sha_len);

        if (memcmp(sha, buf, sha_len) != 0) {
            fprintf(stderr, "[%d] SHA3-%d, len %d test FAILED.\n",
                i, sha_len * 8, msg_len);
            fails++;
        }
    }

    return fails;
}

// test for SHAKE128 and SHAKE256

int test_shake()
{
    // Test vectors have bytes 480..511 of XOF output for given inputs.

    const char *testhex[4] = {
        // SHAKE128, message of length 0
        "43E41B45A653F2A5C4492C1ADD544512DDA2529833462B71A41A45BE97290B6F",
        // SHAKE256, message of length 0
        "AB0BAE316339894304E35877B0C28A9B1FD166C796B9CC258A064A8F57E27F2A",
        // SHAKE128, 1600-bit test pattern
        "44C9FB359FD56AC0A9A75A743CFF6862F17D7259AB075216C0699511643B6439",
        // SHAKE256, 1600-bit test pattern
        "6A1A9D7846436E4DCA5728B6F760EEF0CA92BF0BE5615E96959D767197A0BEEB"
    };

    int i, j, fails;
    sha3_ctx_t sha3;
    uint8_t buf[32], ref[32];

    fails = 0;

    for (i = 0; i < 4; i++) {

        if ((i & 1) == 0) {             // test each twice
            shake128_init(&sha3);
        } else {
            shake256_init(&sha3);
        }

        if (i >= 2) {                   // 1600-bit test pattern
            memset(buf, 0xA3, 20);
            for (j = 0; j < 200; j += 20)
                shake_update(&sha3, buf, 20);
        }

        shake_xof(&sha3);               // switch to extensible output

        for (j = 0; j < 512; j += 32)   // output. discard bytes 0..479
            shake_out(&sha3, buf, 32);

        // compare to reference
        test_readhex(ref, testhex[i], sizeof(ref));
        if (memcmp(buf, ref, 32) != 0) {
            fprintf(stderr, "[%d] SHAKE%d, len %d test FAILED.\n",
                i, i & 1 ? 256 : 128, i >= 2 ? 1600 : 0);
            fails++;
        }
    }

    return fails;
}

// test speed of the comp

void test_speed()
{
    int i;
    uint64_t st[25], x, n;
    clock_t bg, us;

    for (i = 0; i < 25; i++)
        st[i] = i;

    bg = clock();
    n = 0;
    do {
        for (i = 0; i < 100000; i++)
            sha3_keccakf(st);
        n += i;
        us = clock() - bg;
    } while (us < 3 * CLOCKS_PER_SEC);

    x = 0;
    for (i = 0; i < 25; i++)
        x += st[i];

    printf("(%016lX) %.3f Keccak-p[1600,24] / Second.\n",
        (unsigned long) x, (CLOCKS_PER_SEC * ((double) n)) / ((double) us));


}

// pow_genrand
uint8_t *pow_genrand() {
    make_sha3(rand_hash, rand_hash, 32);
    return rand_hash;
}

// pow_initrand
uint8_t *pow_initrand(const char* seed) {
    make_sha3(seed, rand_hash, 32);
    return rand_hash;
}

// pow_gen
uint8_t *pow_gen(uint8_t d, uint8_t *challenge) {
    pow_genrand();
    
    for(int i=0; i < 32; i++)  challenge[i] = rand_hash[i]; // substituir por memcpy
    challenge[32] = d;

    return challenge;    
}

// pow_verify
int pow_verify(uint8_t *challenge, uint8_t *answer) {
    uint8_t hash[32], d, hash_concat[64], response[32];

    for(int i=0; i < 32; i++) hash[i] = hash_concat[i] = challenge[i];
    for(int i=32; i < 64; i++) hash_concat[i] = answer[i-32];
    d = challenge[32];
    
    make_sha3(hash_concat, response, 32);
    
    // checar se os primeiros bits estão corretos
    

}

// pow_try
int pow_try(uint8_t *challenge, uint8_t *answer) {
    pow_genrand();

    
    if(pow_verify(challenge, rand_hash)) {
        for(int i=0; i < 32; i++)  answer[i] = rand_hash[i];
        return 1;
    } else {
        answer = NULL;
        return 0;
    }
}




// main
int main(int argc, char **argv) {
    /*if (test_sha3() == 0 && test_shake() == 0)
        printf("FIPS 202 / SHA3, SHAKE128, SHAKE256 Self-Tests OK!\n");
    test_speed();*/

    //size_t len = (atoi(argv[2]) >= 32) ? 32 : atoi(argv[2]);
    size_t len = 32;

    uint8_t hash[32];
    uint8_t challenge[33];


    make_sha3(argv[1], hash, len);

    pow_initrand(argv[2]);
    printf("SHA3-%lu:\n", len);
    for(int i=0; i < len; i++) printf("%02X", rand_hash[i]);
    printf("\n"); 
    pow_genrand();   

    printf("SHA3-%lu:\n", len);
    for(int i=0; i < len; i++) printf("%02X", rand_hash[i]);
    printf("\n");    
    pow_genrand();

    make_sha3(argv[1], hash, len);
    printf("SHA3-%lu:\n", len);
    for(int i=0; i < len; i++) printf("%02X", hash[i]);
    printf("\n"); 

    pow_gen(100, challenge);
    printf("[CHALLENGE] SHA3-%lu:\n", len);
    for(int i=0; i < len; i++) printf("%02X", challenge[i]);
    printf("\nDificuldade: %d", challenge[32]);
    printf("\n");

    return 0;
}
