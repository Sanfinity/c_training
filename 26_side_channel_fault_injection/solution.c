/*
 * Topic 26: Side-Channel & Fault-Injection Attacks
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * Each problem is a standalone function; main() demonstrates all of them.
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
    uint8_t diff = 0;
    for (size_t i = 0; i < n; i++)
        diff |= (uint8_t)(a[i] ^ b[i]);
    return diff == 0;                 /* 1 iff every byte matched */
}

/* P2: branchless select -> returns (cond ? a : b) with no data-dependent branch.
 * cond is first reduced to 0/1, then mask = -(cond) is all-ones (0xFFFFFFFF) for
 * true and all-zeros for false. */
int constant_time_select(int cond, int a, int b) {
    int mask = -(cond != 0);          /* 0 or -1 (all ones) */
    return (a & mask) | (b & ~mask);
}

/* P3: constant-time conditional copy. Copies src into dst iff cond, byte by byte
 * through a mask, touching every byte either way -> no secret-dependent branch
 * and no secret-dependent memory-access pattern. */
void ct_conditional_copy(int cond, uint8_t *dst, const uint8_t *src, size_t n) {
    uint8_t mask  = (uint8_t)-(cond != 0);   /* 0x00 or 0xFF */
    uint8_t nmask = (uint8_t)~mask;          /* 0xFF or 0x00 */
    for (size_t i = 0; i < n; i++)
        dst[i] = (uint8_t)((src[i] & mask) | (dst[i] & nmask));
}

/* P4: glitch-hardened boolean check. A single fault flips a 1-bit flag (0->1 =
 * access granted); a far-apart magic pair does not. Decodes true/false and
 * reports a fault for any value that is neither magic word. Returns 1 if the
 * value was a valid hardened boolean, 0 if it was corrupted. */
int hardened_verify(uint32_t flag, int *decoded) {
    if (flag == HARDENED_TRUE)  { *decoded = 1; return 1; }
    if (flag == HARDENED_FALSE) { *decoded = 0; return 1; }
    *decoded = 0;
    return 0;                         /* neither magic value -> fault */
}

/* P5: redundant double-check. A single 'if' is one instruction a glitch can
 * skip, so we evaluate the security condition twice and only act when both
 * results agree. 'glitch_skips_second' is a volatile flag that simulates a fault
 * removing the second check (leaving a stuck accept value); the disagreement is
 * then caught as a fault instead of silently granting access. */
access_result redundant_verify(int condition, volatile int glitch_skips_second) {
    int check1 = (condition != 0);
    int check2 = glitch_skips_second ? 1 : (condition != 0);
    if (check1 != check2) return RV_FAULT;   /* the two must agree */
    return check1 ? RV_GRANT : RV_DENY;
}

/* P6: control-flow integrity via a step counter. Each security stage bumps the
 * counter; at the end we require it to equal the number of stages. A glitch that
 * skips a stage leaves the total short, so the missing step is detected. Returns
 * 1 if the flow was intact, 0 if a stage was skipped. */
int flow_counter_check(int skip_stage) {
    int steps = 0;
    steps++;                          /* stage 1: init            */
    steps++;                          /* stage 2: authenticate    */
    if (!skip_stage) steps++;         /* stage 3: verify signature (glitch may skip) */
    steps++;                          /* stage 4: commit          */
    return steps == FLOW_STAGES;      /* 1 = intact, 0 = a stage was skipped */
}

/* P7: fault-tolerant read. A transient during a single sample of a fuse/OTP bit
 * can flip it, so read the (volatile) location twice and require agreement.
 * 'glitch' simulates a transient that corrupts the second sample. Returns 1 if
 * the two reads agreed, 0 if a fault was detected. */
int double_read_fuse(const volatile uint32_t *fuse, int glitch, uint32_t *value) {
    uint32_t r1 = *fuse;
    uint32_t r2 = *fuse ^ (glitch ? 0x00000001u : 0u);  /* fault flips a bit on 2nd read */
    if (r1 != r2) { *value = r1; return 0; }            /* reads disagree -> fault */
    *value = r1;
    return 1;
}

/* P8: boolean masking / blinding. Instead of touching the raw secret, operate on
 * secret ^ mask with a fresh random mask, so the values that appear on the bus
 * and in registers (what DPA measures) are randomized. Masking both operands the
 * same way preserves the comparison result. Returns 1 iff secret == guess. */
int masked_compare(uint8_t secret, uint8_t guess, uint8_t mask) {
    uint8_t ms = secret ^ mask;       /* masked secret (what the hardware sees) */
    uint8_t mg = guess  ^ mask;       /* guess masked identically */
    return ms == mg;                  /* equal iff secret == guess */
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
