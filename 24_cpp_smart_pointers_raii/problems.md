# 24 · C++ Smart Pointers & RAII

RAII is *the* C++ idea: tie resource lifetime to object lifetime so cleanup is automatic and
exception-safe. Smart pointers (`unique_ptr`, `shared_ptr`, `weak_ptr`) apply RAII to heap
memory. Heavily asked for modern C++ roles. Solutions in `solution.cpp`.

Compile:
```
g++ -std=c++17 -Wall -Wextra -O2 solution.cpp -o solution && ./solution
```

## Must-know (asked constantly)
1. **What is RAII?** Resource acquisition in constructor, release in destructor; why it beats manual `new/delete`.
2. **`std::unique_ptr`** — exclusive ownership, move-only, zero overhead; `std::make_unique`.
3. **`std::shared_ptr`** — shared ownership via reference count; `std::make_shared`.
4. **`std::weak_ptr`** — non-owning observer; break `shared_ptr` cycles.
5. **`unique_ptr` vs `shared_ptr`** — when to use each (default to unique).
6. **Why `make_shared`/`make_unique`** over raw `new` (exception safety, single allocation).
7. **Write a minimal RAII wrapper** for a resource (file/lock/handle) yourself.

## Frequently asked
8. **Reference-counting cycle leak** with two `shared_ptr`s pointing at each other; fix with `weak_ptr`.
9. **Custom deleter** for `unique_ptr`/`shared_ptr` (e.g. `fclose`, `free`).
10. **Transferring ownership** with `std::move` on a `unique_ptr`.
11. **`unique_ptr` as a class member** → automatic Rule-of-Zero.
12. **`get()`, `release()`, `reset()`** semantics.
13. **`shared_ptr` use_count** and thread-safety of the control block.

## Good to know / tricky
14. **Why `unique_ptr` is move-only** (copying would double-free).
15. **RAII for locks** (`std::lock_guard`/`std::scoped_lock`) — exception-safe unlocking.
16. **`enable_shared_from_this`.**
17. **Overhead of `shared_ptr`** (atomic refcount) vs `unique_ptr` (none).

## Rapid-fire viva questions
- What is RAII in one sentence?
- unique_ptr vs shared_ptr vs weak_ptr?
- How does shared_ptr know when to free?
- What problem does weak_ptr solve?
- Why prefer make_unique/make_shared?
- Why can't you copy a unique_ptr?
- How does RAII make code exception-safe?
