/*
 * Topic 28: Secure Boot & Chain of Trust
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * A toy, SELF-CONTAINED simulation of a secure-boot flow using ONLY the C
 * standard library. The "hash" (FNV-1a) and the "signature" (XOR with a secret
 * root key) are NOT cryptographically secure -- they stand in for SHA-256 and
 * an asymmetric signature so the CONCEPTS a hardware-security interview probes
 * are concrete. Each problem is a standalone function; main() drives a clean
 * boot, a tampered stage, an anti-rollback attempt and a key revocation.
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

/* P1: FNV-1a 64-bit -- a fast NON-cryptographic hash used here as the image
 * "measurement". A real RoT uses a collision-resistant hash (SHA-256/384);
 * FNV is only here to keep the demo dependency-free. */
uint64_t fnv1a_64(const void *data, size_t n) {
    const uint8_t *p = (const uint8_t *)data;
    uint64_t h = 0xcbf29ce484222325ULL;          /* FNV offset basis */
    for (size_t i = 0; i < n; i++) {
        h ^= p[i];
        h *= 0x100000001b3ULL;                    /* FNV prime */
    }
    return h;
}

/* An image = a named, versioned blob of code that a boot stage loads & runs. */
struct Image {
    const char    *name;
    uint32_t       version;
    const uint8_t *data;
    size_t         len;
};

/* P2: measure an image = hash its code bytes. NOTE we deliberately hash only
 * the code, not the version, so a correctly-signed OLD image still verifies --
 * which is exactly why anti-rollback (P7) must be a separate check. */
uint64_t compute_hash(const struct Image *img) {
    return fnv1a_64(img->data, img->len);
}

#define ROOT_KEY 0xA5A5F00DDEADBEEFULL            /* toy "private key" at vendor */

/* P3: toy signature. A REAL system signs the hash with an asymmetric PRIVATE
 * key and the immutable ROM verifies with the matching PUBLIC key held in
 * ROM/OTP. Here "signing" is just XOR with a secret constant so the demo needs
 * no big-number math -- do not mistake this for real authenticity. */
uint64_t sign_hash(uint64_t hash) {
    return hash ^ ROOT_KEY;
}

/* P4: constant-time 64-bit equality. Fold every differing bit down into bit 0
 * with no data-dependent branch, so comparing a signature never leaks WHERE it
 * first differs via timing. Returns 1 iff a == b. */
int ct_eq_u64(uint64_t a, uint64_t b) {
    uint64_t d = a ^ b;
    d |= d >> 32;
    d |= d >> 16;
    d |= d >> 8;
    d |= d >> 4;
    d |= d >> 2;
    d |= d >> 1;
    return (int)(~d & 1u);                         /* 1 only if no bits set */
}

/* P5: verify an image's signature -- recompute the hash, re-sign it, and
 * constant-time compare against the signature shipped with the image. Any edit
 * to the code changes the hash -> changes the signature -> verification fails. */
int verify_signature(const struct Image *img, uint64_t expected_sig) {
    uint64_t recomputed = sign_hash(compute_hash(img));
    return ct_eq_u64(recomputed, expected_sig);
}

/* P6: the gate one stage applies to the NEXT image before handing control to it
 * (hash + signature). In a production ROM this also checks version (P7) and key
 * revocation (P8); kept to hash+signature here. Returns 1 = safe to execute. */
int verify_stage(const struct Image *next, uint64_t expected_sig) {
    return verify_signature(next, expected_sig);
}

/* P7: anti-rollback. Reject firmware whose version is below the monotonic
 * minimum stored in a fuse/counter. A rolled-back image can be perfectly SIGNED
 * (authentic) yet still forbidden -- that is why this is separate from the
 * signature check. On accept, burn the counter forward. 1 = accept, 0 = reject. */
int anti_rollback(uint32_t version, uint32_t *fuse_min_version) {
    if (version < *fuse_min_version) return 0;     /* rollback attempt -> reject */
    *fuse_min_version = version;                    /* raise the monotonic fuse  */
    return 1;
}

/* P8: public-key revocation. The ROM trusts several signing-key slots; a blown
 * OTP fuse bit permanently marks a compromised slot as revoked. Returns 1 if
 * the given key slot has been revoked (its signature must then be rejected). */
int key_revoked(unsigned key_slot, uint32_t revoke_fuses) {
    return (int)((revoke_fuses >> key_slot) & 1u);
}

/* P9: measured boot. A PCR is never written directly, only "extended":
 * PCR_new = H(PCR_old || measurement). The final value is a tamper-evident,
 * order-sensitive fingerprint of everything that ran, later used for remote
 * attestation. Unlike verified boot it records rather than halts. */
uint64_t pcr_extend(uint64_t pcr, uint64_t measurement) {
    uint8_t buf[16];
    for (int i = 0; i < 8; i++) buf[i]     = (uint8_t)(pcr >> (8 * i));
    for (int i = 0; i < 8; i++) buf[8 + i] = (uint8_t)(measurement >> (8 * i));
    return fnv1a_64(buf, sizeof buf);
}

/* P10: run the chain of trust. BootROM verifies the SBL, the SBL verifies the
 * bootloader, the bootloader verifies the kernel -- each stage is verified
 * BEFORE it is allowed to execute, and the first failure halts the boot. Every
 * stage that passes is measured into the PCR. results[i] <- 1 pass / 0 fail
 * (untouched entries were never reached). Returns the number of stages booted. */
size_t boot_chain(const struct Image *stages, const uint64_t *golden_sig,
                  size_t n, uint64_t *pcr, int *results) {
    size_t booted = 0;
    for (size_t i = 0; i < n; i++) {
        int ok = verify_stage(&stages[i], golden_sig[i]);
        results[i] = ok;
        if (!ok) break;                             /* refuse to run bad code */
        *pcr = pcr_extend(*pcr, compute_hash(&stages[i]));  /* measure, then run */
        booted++;
    }
    return booted;
}

/* P11: tamper demo helper -- flip one bit in a mutable copy of an image to
 * simulate an attacker patching code AFTER it was signed; verifying that image
 * against its (now stale) signature will then fail. */
void tamper_byte(uint8_t *buf, size_t idx) {
    buf[idx] ^= 0x01u;
}

int main(void) {
    /* ---- clean images, each "signed at the vendor" ---- */
    static const uint8_t sbl_code[] = { 0x53,0x42,0x4C,0x00, 0xDE,0xAD,0xC0,0xDE };
    static const uint8_t ldr_code[] = { 0x4C,0x44,0x52,0x00, 0x11,0x22,0x33,0x44 };
    static const uint8_t knl_code[] = { 0x4B,0x4E,0x4C,0x00, 0xCA,0xFE,0xBA,0xBE, 0x01,0x02 };

    struct Image sbl = { "SBL",        2, sbl_code, sizeof sbl_code };
    struct Image ldr = { "Bootloader", 4, ldr_code, sizeof ldr_code };
    struct Image knl = { "Kernel",     7, knl_code, sizeof knl_code };

    /* golden signatures embedded in each image at build time */
    uint64_t golden_sbl = sign_hash(compute_hash(&sbl));
    uint64_t golden_ldr = sign_hash(compute_hash(&ldr));
    uint64_t golden_knl = sign_hash(compute_hash(&knl));

    /* ---- P1..P5: the boot primitives ---- */
    uint64_t hk    = compute_hash(&knl);
    uint64_t sig_k = sign_hash(hk);
    puts("== Boot primitives ==");
    printf("P1  %-30s = 0x%016" PRIx64 "\n", "fnv1a_64(\"boot\")", fnv1a_64("boot", 4));
    printf("P2  %-30s = 0x%016" PRIx64 "\n", "compute_hash(Kernel)", hk);
    printf("P3  %-30s = 0x%016" PRIx64 "\n", "sign_hash(Kernel)", sig_k);
    printf("P4  %-30s = %d / %d\n", "ct_eq_u64 equal / differ",
           ct_eq_u64(sig_k, sig_k), ct_eq_u64(sig_k, sig_k ^ 1u));
    printf("P5  %-30s = %s\n", "verify_signature(Kernel)",
           verify_signature(&knl, golden_knl) ? "PASS" : "FAIL");

    /* ---- P6 / P9 / P10: a clean boot up the chain of trust ---- */
    const char  *verifier[3] = { "BootROM", "SBL", "Bootloader" };
    struct Image chain[3]    = { sbl, ldr, knl };
    uint64_t     golden[3]   = { golden_sbl, golden_ldr, golden_knl };

    uint64_t pcr = 0;
    int      res[3] = { -1, -1, -1 };
    size_t   booted = boot_chain(chain, golden, 3, &pcr, res);
    char     lbl[48];
    puts("\n== Clean boot: chain of trust (verify-then-execute) ==");
    for (size_t i = 0; i < 3u; i++)
        printf("P6  %-10s -> %-16s = %s\n", verifier[i], chain[i].name,
               res[i] == 1 ? "PASS" : res[i] == 0 ? "FAIL" : "----");
    printf("P9  %-30s = 0x%016" PRIx64 "\n", "measured-boot PCR", pcr);
    snprintf(lbl, sizeof lbl, "boot_chain booted %lu/3", (unsigned long)booted);
    printf("P10 %-30s = %s\n", lbl, booted == 3u ? "PASS" : "HALT");

    /* ---- P11: tamper a stage AFTER it was signed -> the chain halts ---- */
    uint8_t tampered[sizeof ldr_code];
    memcpy(tampered, ldr_code, sizeof ldr_code);
    tamper_byte(tampered, 3);                        /* flip 1 bit of the code */
    struct Image ldr_bad   = { "Bootloader", 4, tampered, sizeof tampered };
    struct Image chain2[3] = { sbl, ldr_bad, knl };
    uint64_t pcr2 = 0;
    int      res2[3] = { -1, -1, -1 };
    size_t   booted2 = boot_chain(chain2, golden, 3, &pcr2, res2);
    puts("\n== Tampered Bootloader (1 bit flipped after signing) ==");
    for (size_t i = 0; i < 3u; i++)
        printf("P6  %-10s -> %-16s = %s\n", verifier[i], chain2[i].name,
               res2[i] == 1 ? "PASS" : res2[i] == 0 ? "FAIL" : "----");
    snprintf(lbl, sizeof lbl, "boot_chain booted %lu/3", (unsigned long)booted2);
    printf("P10 %-30s = %s\n", lbl, booted2 == 3u ? "PASS" : "HALT");

    /* ---- P7: anti-rollback on the monotonic version fuse ---- */
    puts("\n== Anti-rollback (monotonic version fuse) ==");
    uint32_t fuse_min = 5;
    int      ok_v7   = anti_rollback(7, &fuse_min);  /* 7 >= 5 -> accept, raise */
    uint32_t after_v7 = fuse_min;                    /* capture before next call */
    int      ok_v3   = anti_rollback(3, &fuse_min);  /* 3 <  7 -> reject rollback */
    snprintf(lbl, sizeof lbl, "accept Kernel v7 (fuse 5->%u)", after_v7);
    printf("P7  %-30s = %s\n", lbl, ok_v7 ? "PASS" : "FAIL");
    snprintf(lbl, sizeof lbl, "accept Kernel v3 (fuse %u)", fuse_min);
    printf("P7  %-30s = %s\n", lbl, ok_v3 ? "PASS (BUG!)" : "FAIL (blocked)");

    /* ---- P8: public-key revocation via OTP fuses ---- */
    puts("\n== Public-key revocation (OTP fuse bitmap) ==");
    uint32_t revoke_fuses = 0x4u;                    /* bit 2 blown: slot #2 dead */
    printf("P8  %-30s = %s\n", "key slot #0 status",
           key_revoked(0, revoke_fuses) ? "REVOKED" : "trusted");
    printf("P8  %-30s = %s\n", "key slot #2 status",
           key_revoked(2, revoke_fuses) ? "REVOKED" : "trusted");

    return 0;
}
