/*
 * Topic 27: Applied Cryptography -- PRACTICE TEMPLATE
 * Fill in each function body marked TODO. Answers are in solution.c.
 * Compile: gcc -std=c11 -Wall -O2 practice.c -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

/* P1: XOR "stream" cipher -- keystream = repeating key; XOR is its own inverse,
 * so the same call encrypts and decrypts. Repeating-key XOR is trivially broken
 * (crib-dragging); only a one-time pad is secure. Educational only. */
void xor_cipher(uint8_t *buf, size_t n, const uint8_t *key, size_t klen) {
    /* TODO: implement. HINT: buf[i] ^= key[i % klen] for i in 0..n-1; running it twice restores the plaintext. */
}

/* P2a: FNV-1a 32-bit -- fast NON-cryptographic hash (hash tables, checksums). */
uint32_t fnv1a_32(const void *data, size_t n) {
    /* TODO: implement. HINT: h=0x811c9dc5; for each byte: h ^= byte; h *= 0x01000193 (FNV prime). */
    return 0; /* placeholder */
}

/* P2b: djb2 -- Dan Bernstein's classic string hash (h*33 + c). Non-crypto. */
uint32_t djb2(const char *s) {
    /* TODO: implement. HINT: h=5381; for each char c: h = h*33 + c. */
    return 0; /* placeholder */
}

/* P3: CRC-32 (IEEE/ISO-HDLC, reflected poly 0xEDB88320). Detects ACCIDENTAL
 * corruption only -- linear and forgeable, NOT an integrity guarantee vs an
 * attacker. Table is built once at runtime (function-local static). */
uint32_t crc32(const void *data, size_t n) {
    /* TODO: implement. HINT: crc=0xFFFFFFFF; per byte fold 8 bits with poly 0xEDB88320
     *       (or build a 256-entry table once); return crc ^ 0xFFFFFFFF. */
    return 0; /* placeholder */
}

/* P4: toy HMAC over FNV-1a -- HMAC(K,m) = H((K^opad) || H((K^ipad) || m)).
 * Simplified to a 4-byte block: the key is folded into 4 bytes; ipad=0x36,
 * opad=0x5c per byte. Demonstrates the nested keyed construction, not a secure
 * MAC (FNV is not a cryptographic hash). */
uint32_t hmac(const uint8_t *key, size_t klen, const uint8_t *msg, size_t mlen) {
    /* TODO: implement. HINT: fold key into k[4]; inner = fnv1a((k^0x36) || msg);
     *       return fnv1a((k^0x5c) || inner_bytes). */
    return 0; /* placeholder */
}

/* P5a: Linear Congruential Generator -- fast PRNG, fully predictable, NOT for
 * keys. Constants from Numerical Recipes. State advances in place. */
uint32_t lcg_next(uint32_t *state) {
    /* TODO: implement. HINT: *state = *state * 1664525u + 1013904223u; return *state. */
    return 0; /* placeholder */
}

/* P5b: xorshift32 -- another cheap non-cryptographic PRNG (Marsaglia). */
uint32_t xorshift32(uint32_t *state) {
    /* TODO: implement. HINT: x=*state; x^=x<<13; x^=x>>17; x^=x<<5; store back and return x. */
    return 0; /* placeholder */
}

/* P6a: greatest common divisor (Euclid). Basis of key math. */
int gcd(int a, int b) {
    /* TODO: implement. HINT: while (b) { t = a % b; a = b; b = t; } return |a|. */
    return 0; /* placeholder */
}

/* P6b: modular inverse via the extended Euclidean algorithm.
 * Returns a^-1 mod m (RSA needs d = e^-1 mod phi). Assumes gcd(a,m)=1. */
int modinv(int a, int m) {
    /* TODO: implement. HINT: extended Euclid tracking x0,x1; return x1 mod m (add m if negative). */
    return 0; /* placeholder */
}

/* P7: modular exponentiation by square-and-multiply. uint64_t products avoid
 * overflow for our small moduli. This is the core op of RSA and DH. */
uint64_t mod_exp(uint64_t base, uint64_t exp, uint64_t mod) {
    /* TODO: implement. HINT: result=1; base%=mod; while(exp){ if(exp&1) result=result*base%mod;
     *       exp>>=1; base=base*base%mod; } return result. */
    return 0; /* placeholder */
}

/* P8: textbook RSA primitive -- both encrypt (e) and decrypt (d) are just a
 * modular exponentiation: c = m^e mod n, m = c^d mod n. NO padding (OAEP/PSS),
 * so insecure by design; here only to show the round-trip. */
uint64_t rsa_crypt(uint64_t m, uint64_t exp, uint64_t n) {
    /* TODO: implement. HINT: RSA is just modular exponentiation -- return mod_exp(m, exp, n). */
    return 0; /* placeholder */
}

/* P9: constant-time equality -- no data-dependent early return, so timing does
 * not reveal where a secret (MAC tag / password) first differs. */
int constant_time_eq(const uint8_t *a, const uint8_t *b, size_t n) {
    /* TODO: implement. HINT: OR together the per-byte XOR of a and b over all n bytes; equal iff it stays 0. */
    return 0; /* placeholder */
}

int main(void) {
    /* P1: XOR stream round-trip (encrypt, fingerprint ciphertext, decrypt) */
    uint8_t msg[] = "Qualcomm";
    size_t mlen = sizeof msg - 1;                     /* drop the NUL for the demo */
    uint8_t buf[sizeof msg];
    memcpy(buf, msg, sizeof msg);
    const uint8_t key[] = {0x5A, 0xA5, 0x3C};
    xor_cipher(buf, mlen, key, sizeof key);           /* encrypt */
    uint32_t cipher_fp = fnv1a_32(buf, mlen);         /* fingerprint of ciphertext */
    xor_cipher(buf, mlen, key, sizeof key);           /* decrypt (same operation) */
    const char *xor_ok = (memcmp(buf, msg, mlen) == 0) ? "OK" : "FAIL";
    printf("P1  xor_cipher   round-trip=%s  cipher_fnv=%08" PRIx32 "\n", xor_ok, cipher_fp);

    /* P2: two non-cryptographic hashes */
    uint32_t h_fnv = fnv1a_32("abc", 3);
    uint32_t h_djb = djb2("abc");
    printf("P2  hashes       fnv1a(\"abc\")=%08" PRIx32 "  djb2(\"abc\")=%08" PRIx32 "\n",
           h_fnv, h_djb);

    /* P3: CRC-32 with the standard test vector */
    uint32_t crc = crc32("123456789", 9);
    printf("P3  crc32        \"123456789\"=%08" PRIx32 "  (std check=CBF43926)\n", crc);

    /* P4: toy HMAC */
    uint32_t tag = hmac((const uint8_t *)"key", 3, (const uint8_t *)"msg", 3);
    printf("P4  hmac_fnv     HMAC(\"key\",\"msg\")=%08" PRIx32 "\n", tag);

    /* P5: two weak PRNGs (compute into temporaries -> defined order) */
    uint32_t lcg_state = 12345u, xs_state = 12345u;
    uint32_t lcg1 = lcg_next(&lcg_state);
    uint32_t xs1  = xorshift32(&xs_state);
    printf("P5  prng         lcg_next=%" PRIu32 "  xorshift32=%" PRIu32 "\n", lcg1, xs1);

    /* P6: number-theory building blocks */
    int g   = gcd(1071, 462);          /* -> 21  */
    int inv = modinv(17, 3120);        /* -> 2753 (this is RSA d for e=17) */
    printf("P6  numtheory    gcd(1071,462)=%d  modinv(17,3120)=%d\n", g, inv);

    /* P7: modular exponentiation (classic 4^13 mod 497 = 445) */
    uint64_t me = mod_exp(4, 13, 497);
    printf("P7  mod_exp      4^13 mod 497=%" PRIu64 "  (expect 445)\n", me);

    /* P8: RSA round-trip with textbook tiny keys n=3233, e=17, d=2753 */
    uint64_t rsa_n = 3233, rsa_e = 17, rsa_d = 2753, pt = 65;
    uint64_t ct = rsa_crypt(pt, rsa_e, rsa_n);
    uint64_t rt = rsa_crypt(ct, rsa_d, rsa_n);
    const char *rsa_ok = (rt == pt) ? "OK" : "FAIL";
    printf("P8  RSA enc(65)=%" PRIu64 " dec=%" PRIu64 " round-trip=%s\n", ct, rt, rsa_ok);

    /* P9: constant-time comparison */
    uint8_t tagA[4] = {0xDE, 0xAD, 0xBE, 0xEF};
    uint8_t tagB[4] = {0xDE, 0xAD, 0xBE, 0xEF};
    uint8_t tagC[4] = {0xDE, 0xAD, 0xBE, 0x00};
    int eq_match = constant_time_eq(tagA, tagB, 4);
    int eq_diff  = constant_time_eq(tagA, tagC, 4);
    printf("P9  ct_eq        match=%d  diff=%d\n", eq_match, eq_diff);

    return 0;
}
