# 23 · C++ Virtual Functions & Polymorphism

Runtime polymorphism is the most-asked C++ interview area: virtual functions, vtables, abstract
base classes, and the critical "why do base classes need a virtual destructor" question.
Solutions in `solution.cpp`.

Compile:
```
g++ -std=c++17 -Wall -Wextra -O2 solution.cpp -o solution && ./solution
```

## Must-know (asked constantly)
1. **Virtual function** — how it enables runtime dispatch through a base pointer/reference.
2. **Pure virtual function / abstract base class / interface** (`= 0`).
3. **Why a base class needs a `virtual` destructor** — deleting a derived via a base pointer without it is UB / leaks.
4. **`override` and `final`** specifiers — catch signature mistakes at compile time.
5. **Static (compile-time) vs dynamic (runtime) binding.**
6. **vtable / vptr** — the usual implementation of virtual dispatch (conceptual).
7. **Function overriding vs overloading vs hiding.**

## Frequently asked
8. **Calling a virtual function from a constructor/destructor** — why you get the base version.
9. **`dynamic_cast`** and RTTI — safe downcasting; returns nullptr / throws on failure.
10. **Abstract class cannot be instantiated**; a derived must implement all pure virtuals.
11. **Diamond inheritance & `virtual` inheritance** (the classic multiple-inheritance problem).
12. **Slicing** — passing a derived object by value as a base loses the derived part.
13. **Pure virtual with a body** (rare but legal).

## Good to know / tricky
14. **Cost of virtual calls** (indirection, no inlining) — matters for embedded/perf.
15. **`override` prevents silent non-overrides** when the base signature changes.
16. **CRTP** — static polymorphism as a zero-overhead alternative.
17. **When NOT to use virtual** (value types, hot paths, no inheritance intended → mark `final`).

## Rapid-fire viva questions
- What makes a function dispatch at runtime?
- Why must a polymorphic base class have a virtual destructor?
- What is object slicing and how do you avoid it?
- Static vs dynamic binding — give an example of each.
- What does `override` buy you?
- What happens if you call a virtual function inside the constructor?
- What is `dynamic_cast` used for?
