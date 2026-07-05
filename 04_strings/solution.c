/*
 * Topic 04: Strings
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * Each problem is a standalone function; main() demonstrates all of them.
 */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <stddef.h>

/* P1: strlen. */
size_t my_strlen(const char *s) {
    const char *p = s;
    while (*p) p++;
    return (size_t)(p - s);
}

/* P2: strcpy (assumes dst is large enough). */
char *my_strcpy(char *dst, const char *src) {
    char *d = dst;
    while ((*d++ = *src++)) { }
    return dst;
}

/* P3: strcmp. */
int my_strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}

/* P4: strcat (assumes dst is large enough). */
char *my_strcat(char *dst, const char *src) {
    char *d = dst;
    while (*d) d++;
    while ((*d++ = *src++)) { }
    return dst;
}

/* P5: Reverse a string in place. */
void reverse_str(char *s) {
    size_t n = my_strlen(s);
    for (size_t i = 0, j = n ? n - 1 : 0; i < j; i++, j--) {
        char t = s[i]; s[i] = s[j]; s[j] = t;
    }
}

/* P6: Palindrome check. */
bool is_palindrome(const char *s) {
    size_t i = 0, j = my_strlen(s);
    if (j == 0) return true;
    j--;
    while (i < j) { if (s[i++] != s[j--]) return false; }
    return true;
}

/* P7: Anagram check (ASCII histogram). */
bool are_anagrams(const char *a, const char *b) {
    int freq[256] = {0};
    for (; *a; a++) freq[(unsigned char)*a]++;
    for (; *b; b++) freq[(unsigned char)*b]--;
    for (int i = 0; i < 256; i++) if (freq[i]) return false;
    return true;
}

/* P8: Count words (whitespace-separated). */
int count_words(const char *s) {
    int count = 0;
    bool in_word = false;
    for (; *s; s++) {
        if (*s == ' ' || *s == '\t' || *s == '\n') in_word = false;
        else if (!in_word) { in_word = true; count++; }
    }
    return count;
}

/* P10: First non-repeating character (returns 0 if none). */
char first_unique(const char *s) {
    int freq[256] = {0};
    for (const char *p = s; *p; p++) freq[(unsigned char)*p]++;
    for (const char *p = s; *p; p++) if (freq[(unsigned char)*p] == 1) return *p;
    return 0;
}

/* P12: atoi with sign and clamping. */
int my_atoi(const char *s) {
    while (*s == ' ') s++;
    int sign = 1;
    if (*s == '+' || *s == '-') { if (*s == '-') sign = -1; s++; }
    long result = 0;
    while (*s >= '0' && *s <= '9') {
        result = result * 10 + (*s - '0');
        if (sign == 1 && result > INT_MAX) return INT_MAX;
        if (sign == -1 && -result < INT_MIN) return INT_MIN;
        s++;
    }
    return (int)(sign * result);
}

/* P14: Balanced parentheses () [] {}. */
bool balanced(const char *s) {
    char stack[256];
    int top = 0;
    for (; *s; s++) {
        char c = *s;
        if (c=='('||c=='['||c=='{') {
            if (top >= (int)sizeof(stack)) return false;
            stack[top++] = c;
        } else if (c==')'||c==']'||c=='}') {
            if (top == 0) return false;
            char o = stack[--top];
            if ((c==')'&&o!='(')||(c==']'&&o!='[')||(c=='}'&&o!='{')) return false;
        }
    }
    return top == 0;
}

int main(void) {
    printf("P1  my_strlen(\"hello\")   = %zu\n", my_strlen("hello"));
    char buf[32];
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
