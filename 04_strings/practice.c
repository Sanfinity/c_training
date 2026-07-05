/*
 * Topic 04: Strings -- PRACTICE TEMPLATE
 * Fill in each function body marked TODO. Answers are in solution.c.
 * Compile: gcc -std=c11 -Wall -O2 practice.c -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
 */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <stddef.h>

/* P1: strlen. */
size_t my_strlen(const char *s) {
    /* TODO: implement. HINT: walk a cursor to the terminating '\0' and return how far it moved. */
    return 0; /* placeholder */
}

/* P2: strcpy (assumes dst is large enough). */
char *my_strcpy(char *dst, const char *src) {
    /* TODO: implement. HINT: copy each character (including the terminator) from src to dst, then return dst. */
    return NULL; /* placeholder */
}

/* P3: strcmp. */
int my_strcmp(const char *a, const char *b) {
    /* TODO: implement. HINT: advance while characters match; return the difference of the first mismatch as unsigned char. */
    return 0; /* placeholder */
}

/* P4: strcat (assumes dst is large enough). */
char *my_strcat(char *dst, const char *src) {
    /* TODO: implement. HINT: find dst's terminator first, then copy src there; return dst. */
    return NULL; /* placeholder */
}

/* P5: Reverse a string in place. */
void reverse_str(char *s) {
    /* TODO: implement. HINT: get the length, then swap ends moving inward toward the middle. */
}

/* P6: Palindrome check. */
bool is_palindrome(const char *s) {
    /* TODO: implement. HINT: compare characters from both ends moving inward; mismatch means not a palindrome. */
    return false; /* placeholder */
}

/* P7: Anagram check (ASCII histogram). */
bool are_anagrams(const char *a, const char *b) {
    /* TODO: implement. HINT: tally character counts up for one string and down for the other; all zero means anagram. */
    return false; /* placeholder */
}

/* P8: Count words (whitespace-separated). */
int count_words(const char *s) {
    /* TODO: implement. HINT: track whether you are inside a word; count each transition from whitespace into a word. */
    return 0; /* placeholder */
}

/* P10: First non-repeating character (returns 0 if none). */
char first_unique(const char *s) {
    /* TODO: implement. HINT: first build a frequency table, then return the first character whose count is exactly 1. */
    return 0; /* placeholder */
}

/* P12: atoi with sign and clamping. */
int my_atoi(const char *s) {
    /* TODO: implement. HINT: skip spaces, read an optional sign, accumulate digits in a wider type, and clamp to INT_MIN/INT_MAX. */
    return 0; /* placeholder */
}

/* P14: Balanced parentheses () [] {}. */
bool balanced(const char *s) {
    /* TODO: implement. HINT: push opening brackets onto a stack and, on a closer, verify it matches the top you pop. */
    return false; /* placeholder */
}

int main(void) {
    printf("P1  my_strlen(\"hello\")   = %zu\n", my_strlen("hello"));
    char buf[32] = {0};
    my_strcpy(buf, "copy me");
    printf("P2  my_strcpy            -> %s\n", buf);
    printf("P3  my_strcmp(ab,ac)     = %d\n", my_strcmp("ab", "ac"));
    my_strcpy(buf, "foo");
    my_strcat(buf, "bar");
    printf("P4  my_strcat            -> %s\n", buf);
    my_strcpy(buf, "reverse");
    reverse_str(buf);
    printf("P5  reverse_str          -> %s\n", buf);
    printf("P6  is_palindrome(level) = %d\n", is_palindrome("level"));
    printf("P7  are_anagrams         = %d\n", are_anagrams("listen", "silent"));
    printf("P8  count_words          = %d\n", count_words("the sky is blue"));
    printf("P10 first_unique(swiss)  = %c\n", first_unique("swiss"));
    printf("P12 my_atoi(\"-2147483648\") = %d\n", my_atoi("-2147483648"));
    printf("P12 my_atoi(\"  42abc\")   = %d\n", my_atoi("  42abc"));
    printf("P14 balanced({[()]})     = %d\n", balanced("{[()]}"));
    printf("P14 balanced(([)])       = %d\n", balanced("([)]"));
    return 0;
}
