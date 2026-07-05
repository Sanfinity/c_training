# 22 · C++ OOP Basics

The transition from C to C++: classes, constructors/destructors, the Rule of Three/Five,
copy semantics, and encapsulation. Expect these in any C++ interview. Solutions in `solution.cpp`.

Compile:
```
g++ -std=c++17 -Wall -Wextra -O2 solution.cpp -o solution && ./solution
```

## Must-know (asked constantly)
1. **Class vs struct** in C++ (default access) and basic encapsulation (public/private).
2. **Constructor, destructor, member-init list** — why prefer the init list over assignment.
3. **The Rule of Three** — copy constructor, copy assignment, destructor (when you manage a resource).
4. **Deep copy vs shallow copy** — implement a class owning a heap buffer correctly.
5. **`this` pointer**, and returning `*this` for chaining.
6. **Static members** (shared across instances) and static methods.
7. **Operator overloading** (e.g. `operator+`, `operator==`, `operator<<`).

## Frequently asked
8. **The Rule of Five** — add move constructor and move assignment (C++11).
9. **`const` member functions** — what `const` after a method means.
10. **Initialization order** of members (declaration order, not init-list order) and base-before-derived.
11. **`explicit` constructors** — prevent implicit conversions.
12. **Delegating constructors** and in-class member initializers.
13. **Friend functions/classes.**

## Good to know / tricky
14. **Rule of Zero** — prefer members that manage themselves (containers/smart pointers) so you write none of the special members.
15. **`= default` and `= delete`.**
16. **Copy elision / RVO.**
17. **Object lifetime & destruction order** (reverse of construction).

## Rapid-fire viva questions
- Default access in `class` vs `struct`?
- What is the Rule of Three and when do you need it?
- Difference between shallow and deep copy?
- Why use a member-initializer list?
- What does a trailing `const` on a method promise?
- What does `explicit` prevent?
- In what order are members initialized?
