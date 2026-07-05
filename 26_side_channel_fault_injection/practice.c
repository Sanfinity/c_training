/*
 * Topic 26: Side-Channel & Fault-Injection Attacks -- PRACTICE TEMPLATE
 * Fill in each function body marked TODO. Answers are in solution.c.
 * Compile: gcc -std=c11 -Wall -O2 practice.c -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

/* Glitch-hardened boolean encodings (P4): far-apart words so a few flipped bits
 * cannot turn one valid value into the other. Anything else is treated as a
 * fault. */
#define HARDENED_TRUE  0x3C3C3C3Cu
#define HARDENED_FALSE 0xC3C3C3C3u

/* Number of security stages the flow counter (P6) must pass through. */
#define FLOW_STAGES 4

/* Three-way outcome for the redundant verifier (P5). */
typedef enum { RV_DENY = 0, RV_GRANT = 1, RV_FAULT = 2 } access_result;

static const char *rv_name(access_result r) {
    switch (r) {
        case RV_GRANT: return "GRANT";
        case RV_DENY:  return "DENY";
        default:       return "FAULT";
    }
}

/* P1: constant-time byte compare. NO early return: OR the per-byte XOR of every
 * byte so the running time depends only on n, never on where (or whether) the
 * first mismatch is. Use this for tags, PINs, HMACs and secure-boot hashes. */
int constant_time_eq(const uint8_t *a, const uint8_t *b, size_t n) {
    /* TODO: implement. HINT: accumulate diff |= a[i]^b[i] over all n bytes, then return diff==0. */
    return 0; /* placeholder */
}

/* P2: branchless select -> returns (cond ? a : b) with no data-dependent branch.
 * cond is first reduced to 0/1, then mask = -(cond) is all-ones (0xFFFFFFFF) for
 * true and all-zeros for false. */
int constant_time_select(int cond, int a, int b) {
    /* TODO: implement. HINT: mask = -(cond != 0); return (a & mask) | (b & ~mask). */
    return 0; /* placeholder */
}

/* P3: constant-time conditional copy. Copies src into dst iff cond, byte by byte
 * through a mask, touching every byte either way -> no secret-dependent branch
 * and no secret-dependent memory-access pattern. */
void ct_conditional_copy(int cond, uint8_t *dst, const uint8_t *src, size_t n) {
    /* TODO: implement. HINT: make an 8-bit mask 0x00/0xFF from cond, then dst=src&mask | dst&~mask per byte. */
}

/* P4: glitch-hardened boolean check. A single fault flips a 1-bit flag (0->1 =
 * access granted); a far-apart magic pair does not. Decodes true/false and
 * reports a fault for any value that is neither magic word. Returns 1 if the
 * value was a valid hardened boolean, 0 if it was corrupted. */
int hardened_verify(uint32_t flag, int *decoded) {
    /* TODO: implement. HINT: match HARDENED_TRUE/HARDENED_FALSE (set *decoded, return 1); anything else is a fault (return 0). */
    return 0; /* placeholder */
}

/* P5: redundant double-check. A single 'if' is one instruction a glitch can
 * skip, so we evaluate the security condition twice and only act when both
 * results agree. 'glitch_skips_second' is a volatile flag that simulates a fault
 * removing the second check (leaving a stuck accept value); the disagreement is
 * then caught as a fault instead of silently granting access. */
access_result redundant_verify(int condition, volatile int glitch_skips_second) {
    /* TODO: implement. HINT: compute the check twice (2nd forced to 1 when glitched); disagree -> RV_FAULT, else grant/deny. */
    return RV_DENY; /* placeholder */
}

/* P6: control-flow integrity via a step counter. Each security stage bumps the
 * counter; at the end we require it to equal the number of stages. A glitch that
 * skips a stage leaves the total short, so the missing step is detected. Returns
 * 1 if the flow was intact, 0 if a stage was skipped. */
int flow_counter_check(int skip_stage) {
    /* TODO: implement. HINT: increment a local at each stage (skip one when skip_stage), then compare the total to FLOW_STAGES. */
    return 0; /* placeholder */
}

/* P7: fault-tolerant read. A transient during a single sample of a fuse/OTP bit
 * can flip it, so read the (volatile) location twice and require agreement.
 * 'glitch' simulates a transient that corrupts the second sample. Returns 1 if
 * the two reads agreed, 0 if a fault was detected. */
int double_read_fuse(const volatile uint32_t *fuse, int glitch, uint32_t *value) {
    /* TODO: implement. HINT: read *fuse twice (XOR a bit into the 2nd when glitch); if they differ report a fault, else store and return 1. */
    return 0; /* placeholder */
}

/* P8: boolean masking / blinding. Instead of touching the raw secret, operate on
 * secret ^ mask with a fresh random mask, so the values that appear on the bus
 * and in registers (what DPA measures) are randomized. Masking both operands the
 * same way preserves the comparison result. Returns 1 iff secret == guess. */
int masked_compare(uint8_t secret, uint8_t guess, uint8_t mask) {
    /* TODO: implement. HINT: XOR the same mask into secret and guess, then compare the masked values. */
    return 0; /* placeholder */
}

int main(void) {
    /* P1: constant-time compare on equal and unequal inputs. */
    uint8_t mac[4]  = {0xDE, 0xAD, 0xBE, 0xEF};
    uint8_t good[4] = {0xDE, 0xAD, 0xBE, 0xEF};
    uint8_t bad[4]  = {0xDE, 0xAD, 0xBE, 0x00};
    int p1_match    = constant_time_eq(mac, good, 4);
    int p1_mismatch = constant_time_eq(mac, bad, 4);
    printf("P1  constant_time_eq      match=%d mismatch=%d\n", p1_match, p1_mismatch);

    /* P2: branchless select returns a for cond!=0, b for cond==0. */
    int p2_true  = constant_time_select(1, 111, 222);
    int p2_false = constant_time_select(0, 111, 222);
    printf("P2  constant_time_select  cond1=%d cond0=%d\n", p2_true, p2_false);

    /* P3: conditional copy leaves dst on cond=0, overwrites it on cond=1. */
    uint8_t p3_dst[4] = {7, 7, 7, 7};
    uint8_t p3_src[4] = {1, 2, 3, 4};
    ct_conditional_copy(0, p3_dst, p3_src, 4);   /* cond=0 -> keep dst */
    int p3_keeps = p3_dst[0];
    ct_conditional_copy(1, p3_dst, p3_src, 4);   /* cond=1 -> copy src */
    int p3_copies = p3_dst[0];
    printf("P3  ct_conditional_copy   cond0_keeps=%d cond1_copies=%d\n", p3_keeps, p3_copies);

    /* P4: hardened boolean accepts a valid magic word, rejects a corrupted one. */
    int p4_decoded = -1, p4_junk = -1;
    int p4_valid   = hardened_verify(HARDENED_TRUE, &p4_decoded);
    int p4_corrupt = hardened_verify(0x00000001u, &p4_junk);   /* a bit-flipped '1' */
    printf("P4  hardened_verify       valid_ok=%d decoded=%d corrupt_ok=%d\n",
           p4_valid, p4_decoded, p4_corrupt);

    /* P5: redundant verify grants/denies legitimately, flags a skipped check. */
    access_result p5_grant = redundant_verify(1, 0);   /* legit access  */
    access_result p5_deny  = redundant_verify(0, 0);   /* legit denial  */
    access_result p5_glit  = redundant_verify(0, 1);   /* deny bypassed by a glitch */
    printf("P5  redundant_verify      legit=%s deny=%s glitched=%s\n",
           rv_name(p5_grant), rv_name(p5_deny), rv_name(p5_glit));

    /* P6: flow counter passes when intact, fails when a stage is skipped. */
    int p6_intact  = flow_counter_check(0);
    int p6_skipped = flow_counter_check(1);
    printf("P6  flow_counter_check    intact=%d skipped=%d\n", p6_intact, p6_skipped);

    /* P7: double-read agrees on a stable fuse, detects a transient on read 2. */
    volatile uint32_t fuse = 0xA5A5A5A5u;
    uint32_t p7_v_ok = 0, p7_v_flt = 0;
    int p7_stable = double_read_fuse(&fuse, 0, &p7_v_ok);
    int p7_glitch = double_read_fuse(&fuse, 1, &p7_v_flt);
    printf("P7  double_read_fuse      stable_agree=%d val=0x%08X glitch_agree=%d\n",
           p7_stable, (unsigned)p7_v_ok, p7_glitch);

    /* P8: masking blinds the on-bus value but preserves the compare result. */
    uint8_t secret = 0x42, mask = 0x5A;
    int p8_match    = masked_compare(secret, 0x42, mask);
    int p8_mismatch = masked_compare(secret, 0x43, mask);
    printf("P8  masked_compare        masked=0x%02X match=%d mismatch=%d\n",
           (unsigned)(uint8_t)(secret ^ mask), p8_match, p8_mismatch);

    return 0;
}
