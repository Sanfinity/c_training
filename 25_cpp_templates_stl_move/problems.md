# 25 · C++ Templates, STL & Move Semantics

Generic programming (templates), the Standard Template Library (containers, iterators,
algorithms), and move semantics / perfect forwarding. Rounds out a modern C++ interview.
Solutions in `solution.cpp`.

Compile:
```
g++ -std=c++17 -Wall -Wextra -O2 solution.cpp -o solution && ./solution
```

## Must-know (asked constantly)
1. **Function template** — write a generic `my_max<T>` / `swap<T>`.
2. **Class template** — write a simple generic `Stack<T>` or `Pair<A,B>`.
3. **Template specialization** (full/partial) — customize behavior for a type.
4. **STL containers overview** — `vector`, `list`, `map`, `unordered_map`, `set`; when to use which and their complexities.
5. **Iterators** — traverse a container; `begin()/end()`, range-based for.
6. **STL algorithms** — `sort`, `find`, `accumulate`, `count_if`, with lambdas.
7. **lvalue vs rvalue; move semantics** — what `std::move` does and why it avoids a copy.

## Frequently asked
8. **Move constructor / move assignment** — implement and show it's chosen for temporaries.
9. **Perfect forwarding** — `template<class T> void f(T&&)` + `std::forward<T>`.
10. **`auto` and `decltype`.**
11. **Variadic templates** — a type-safe `print(args...)`.
12. **Lambdas & captures** (`[=]`, `[&]`, capture by value/ref, mutable).
13. **`std::vector` growth** — capacity vs size, amortized O(1) push_back, reserve.

## Good to know / tricky
14. **Copy elision / RVO** and why returning by value is cheap.
15. **`emplace_back` vs `push_back`** (in-place construction).
16. **Universal (forwarding) references vs rvalue references** — `T&&` in a deduced context.
17. **SFINAE / `if constexpr`** for compile-time branching.

## Rapid-fire viva questions
- Difference between a function template and a class template?
- What does `std::move` actually do? (a cast to rvalue — it moves nothing itself.)
- lvalue vs rvalue?
- vector vs list vs map complexities?
- Why is `emplace_back` sometimes better than `push_back`?
- What is perfect forwarding and why `std::forward`?
- What is RVO?
