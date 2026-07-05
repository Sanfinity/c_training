# 04 · Strings

C strings are just `char` arrays terminated by `'\0'`. Interviewers love these because they
expose off-by-one bugs, buffer handling, and pointer fluency. Implement the standard library
functions yourself. Solutions in `solution.c`.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **Implement `strlen`.**
2. **Implement `strcpy` / `strncpy`** and explain the difference (and `strncpy`'s no-terminator pitfall).
3. **Implement `strcmp`.**
4. **Implement `strcat`.**
5. **Reverse a string in place.**
6. **Check if a string is a palindrome.**
7. **Check if two strings are anagrams.**
8. **Count words** in a sentence.

## Frequently asked
9. **Reverse the order of words** in a sentence ("the sky is blue" → "blue is sky the").
10. **Find the first non-repeating character.**
11. **Remove duplicate characters** from a string.
12. **atoi** — convert a string to an integer (handle sign & overflow).
13. **itoa** — convert an integer to a string.
14. **Check for balanced parentheses.**

## Good to know / tricky
15. **Longest substring without repeating characters** (sliding window).
16. **String vs char array**: why is `char *s = "abc"` read-only but `char s[] = "abc"` writable?
17. **`sizeof` vs `strlen`** for `char buf[10] = "hi"`.
18. Explain the bug in `char *f(){ char b[8]; strcpy(b,"x"); return b; }`.

## Rapid-fire viva questions
- Where do string literals live, and what happens if you modify one?
- Difference between `strcpy` and `memcpy`?
- Why is `strncpy` not always safe? How do you guarantee termination?
- What does `strlen` return for `"a\0b"`?
- How many bytes does `"hello"` occupy in memory?
