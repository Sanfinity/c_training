/*
 * Topic 21: Buffer Overflows & Memory Safety -- PRACTICE TEMPLATE
 * Fill in each function body marked TODO. Answers are in solution.c.
 * Compile: gcc -std=c11 -Wall -O2 practice.c -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* P4/P11: safe bounded copy that ALWAYS NUL-terminates.
 * VULNERABLE:   strcpy(dst, src);          // no bound -> overflow
 * PARTLY-SAFE:  strncpy(dst, src, n);      // no NUL if truncated
 * SAFE: */
size_t safe_copy(char *dst, size_t dst_size, const char *src) {
    /* TODO: implement. HINT: copy at most dst_size-1 bytes, then write '\0'; return the count copied. */
    return 0; /* placeholder */
}

/* P5: overflow-checked allocation (like calloc's internal check).
 * VULNERABLE: malloc(count * size) can wrap to a tiny buffer. */
void *checked_alloc(size_t count, size_t size) {
    /* TODO: implement. HINT: refuse (return NULL) if count*size would exceed SIZE_MAX, else malloc the product. */
    return NULL; /* placeholder */
}

/* P6/P12: bounds-checked array accessor. */
bool bounded_get(const int *a, size_t n, size_t idx, int *out) {
    /* TODO: implement. HINT: reject when idx>=n; otherwise store a[idx] into *out and report success. */
    return false; /* placeholder */
}

/* P7: format-string safety.
 * VULNERABLE: printf(user);        // user controls the format specifiers
 * SAFE: */
void safe_print(const char *user) {
    /* TODO: implement. HINT: print the user string as data, never as the format string itself. */
}

/* P13: constant-time comparison for secrets (no early-out timing leak). */
int constant_time_equal(const uint8_t *a, const uint8_t *b, size_t n) {
    /* TODO: implement. HINT: OR together the per-byte XOR of a and b over all n bytes; equal iff it stays zero. */
    return 0; /* placeholder */
}

/* P3: off-by-one demonstration - the CORRECT bound.
 * Fill n-1 chars and terminate; a '<=' here would write one past the end. */
void fill_safe(char *buf, size_t size, char c) {
    /* TODO: implement. HINT: guard size==0, write c to indices 0..size-2, then place '\0' at size-1. */
}

/* P1: a CONTROLLED buffer-overflow VIOLATION so you can SEE the corruption.
 * The two arrays sit next to each other in memory (small at the lower address,
 * neighbor right after it). We write 12 bytes into the 8-byte 'small' buffer,
 * so 4 bytes spill into 'neighbor' even though we never name neighbor.
 *
 * The write count is taken through a 'volatile' so the optimizer cannot prove
 * the overflow at compile time (keeps -Wall -Wextra clean) and cannot cache it.
 * The whole write stays inside the struct, so this is observable but safe to run.
 *
 * On a real STACK buffer the same bug keeps climbing toward the saved return
 * address; overwriting it is how classic stack-smashing hijacks control flow.
 * -fstack-protector places a canary before the return address and aborts with
 * "*** stack smashing detected ***" if it is clobbered. */
struct adjacent { char small[8]; char neighbor[8]; };

void overflow_violation(struct adjacent *s) {
    /* TODO: implement. HINT: give neighbor a known "SAFE" value, then write more bytes into small[] than it holds
     *       (use a 'volatile int count' so the optimizer keeps -Wall -Wextra clean). */
}

int main(void) {
    struct adjacent s = {0};
    overflow_violation(&s);
    printf("P1  overflow violation   -> neighbor was \"SAFE\", now \"%s\" (corrupted)\n",
           s.neighbor);

    char dst[8] = {0};
    size_t n = safe_copy(dst, sizeof dst, "this string is far too long");
    printf("P4  safe_copy            -> \"%s\" (%zu chars, no overflow)\n", dst, n);

    void *p = checked_alloc(SIZE_MAX, 2);
    printf("P5  checked_alloc(huge)  = %s (overflow refused)\n", p ? "ptr" : "NULL");
    free(p);
    p = checked_alloc(4, sizeof(int));
    printf("P5  checked_alloc(4,int) = %s\n", p ? "ok" : "NULL");
    free(p);

    int arr[] = {10, 20, 30};
    int v = 0;
    bool got1 = bounded_get(arr, 3, 1, &v);   /* in range: v becomes 20 */
    printf("P6  bounded_get(idx=1)   = %s v=%d\n", got1 ? "ok" : "reject", v);
    bool got9 = bounded_get(arr, 3, 9, &v);   /* out of range: rejected */
    printf("P6  bounded_get(idx=9)   = %s (out-of-range rejected)\n", got9 ? "ok" : "reject");

    printf("P7  safe_print(\"%%s%%s\")   -> ");
    safe_print("%s%s (treated as literal text)\n");

    uint8_t secret[4] = {0xDE,0xAD,0xBE,0xEF};
    uint8_t guess1[4] = {0xDE,0xAD,0xBE,0xEF};
    uint8_t guess2[4] = {0xDE,0xAD,0xBE,0x00};
    printf("P13 constant_time_equal  -> match=%d mismatch=%d\n",
           constant_time_equal(secret, guess1, 4),
           constant_time_equal(secret, guess2, 4));

    char buf[6] = {0};
    fill_safe(buf, sizeof buf, '*');
    printf("P3  fill_safe            -> \"%s\" (len=%zu, terminated)\n", buf, strlen(buf));

    printf("P8  mitigations: -fstack-protector (canary), ASLR, NX/DEP, RELRO\n");
    return 0;
}
