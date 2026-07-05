# 26 · Side-Channel & Fault-Injection Attacks

Attacks that break correct crypto on real silicon — leaking secrets through timing/power/EM/cache, or actively glitching the chip to skip a check — plus the countermeasures you must be able to code.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **Timing attack / constant-time compare** — a secret-dependent early-out (`memcmp`, `strcmp`) leaks *where* the first mismatch is, so response time scales with the length of the matching prefix. Write a compare that ORs the per-byte XORs and returns only at the end, so timing depends on length alone, never on the data. → `constant_time_eq` (P1).
2. **Branchless / constant-time select** — replace `cond ? a : b` (a secret-dependent branch → timing + branch-predictor leak) with a mask: `mask = -(cond)` is all-ones when `cond` is 1 and all-zeros when 0, then `(a & mask) | (b & ~mask)`. → `constant_time_select` (P2).
3. **Constant-time conditional copy/move** — copy `src`→`dst` only when a *secret* condition holds, without a branch or a secret-dependent memory-access pattern: touch every byte through the same mask either way. → `ct_conditional_copy` (P3).
4. **Glitch-hardened booleans** — a normal bool is one bit; a single fault flips it (`0`→`1` = access granted). Store true/false as far-apart magic words (`0x3C3C3C3C` / `0xC3C3C3C3`) and treat *anything else* as a detected fault. → `hardened_verify` (P4).
5. **Redundancy / double-check** — one `if` is one instruction a glitch can skip. Evaluate the security decision twice and require both results to agree; disagreement means a fault was injected. → `redundant_verify` (P5).
6. **Control-flow integrity / step counter** — increment a counter at every security stage and assert the final total equals the expected number of stages; a glitch that skips a stage leaves the count short. → `flow_counter_check` (P6).
7. **Fault-tolerant reads (double-read a fuse)** — a transient during a single sample of a fuse/OTP/config bit can flip it; read the location twice and require agreement before trusting it. → `double_read_fuse` (P7).
8. **Masking / blinding** — XOR the secret with a fresh random mask so the intermediate values on the bus and in registers (what DPA measures) are randomized; the logical result is unchanged. → `masked_compare` (P8).

## Frequently asked
9. **Side-channel vs fault-injection** — a **side-channel** attack is *passive*: observe time/power/EM/cache to infer the key without touching execution. **Fault injection** is *active*: perturb the chip to change what it does. Different threat models, frequently combined (profile with SCA, bypass with FI).
10. **Power analysis: SPA vs DPA vs CPA** — **SPA** reads one/few traces by eye (you can see the square-and-multiply of an RSA exponent). **DPA** uses statistics over many traces plus a key guess, partitioning traces on a predicted intermediate bit. **CPA** correlates measured power against a leakage model (Hamming weight/distance) of an intermediate — the modern workhorse.
11. **EM (electromagnetic) analysis** — a near-field probe over the die picks up the same data-dependent leakage as power analysis, but *localized* (target one block) and *without* a series shunt resistor, so it works even where you cannot cut the power rail.
12. **Fault-injection methods** — **voltage/Vcc glitching** (a brief brown-out), **clock glitching** (a too-short cycle corrupts a fetch/latch), **EM fault** (a coil pulse over the die), and **laser/optical** (flip a specific transistor — very precise, needs decapsulation), plus temperature/body-bias. Effect: skip an instruction, corrupt a comparison result, or garble a memory read.
13. **What glitching bypasses** — the classic targets are the *single decision points*: a **signature/hash verify** branch (force "valid"), a **PIN/password compare**, a **secure-boot** "is this image authentic?" test, debug-lock / life-cycle / **fuse** checks, and loop counters (e.g. shorten a key-erase or key-copy loop).
14. **Cache-timing / Spectre & Meltdown (concept)** — data-dependent memory accesses leave footprints in the cache; **Flush+Reload** / **Prime+Probe** time later accesses to learn which line was touched. Spectre/Meltdown add *speculative* execution: the CPU transiently accesses secret-dependent addresses, the microarchitectural cache state survives the rollback, and it is then read out by timing.

## Good to know / tricky
15. **Why `memcmp`/`strcmp` are unsafe for secrets** — they early-return on the first differing byte, so time is proportional to the matching-prefix length; an attacker recovers a MAC/tag/PIN byte-by-byte. Always use a **constant-time compare** for tags, PINs, HMACs, and secure-boot hashes.
16. **Random delays / jitter** — inserting random dummy work desynchronizes power traces and misaligns fault timing, raising attacker effort — but it only adds noise (trace re-alignment and averaging defeat it), so it is a *complement*, never a standalone fix.
17. **Hardware sensors & shields** — on-die voltage/clock/temperature/light/EM **glitch detectors**, active metal meshes over sensitive logic, and redundant self-checking flip-flops that raise a tamper alarm (and wipe keys) when an anomaly is seen.
18. **The compiler is your enemy for constant-time code** — `-O2` can re-introduce a branch, pick a variable-latency instruction, or fold a masked select back into a `cmov`/branch. Use `volatile` sinks, memory barriers, or vetted primitives, and inspect the emitted assembly.
19. **Masking order & higher-order DPA** — first-order masking (one random share) falls to *second-order* DPA that combines two leakage points; production designs use higher-order masking and re-randomize the masks on every run.
20. **Defense-in-depth** — no single trick suffices: combine constant-time + masking (against SCA) with redundancy + flow counters + hardened booleans + sensors (against FI), because real attackers chain SCA to locate the target and FI to bypass it.

## Rapid-fire viva questions
- Passive vs active hardware attack — give one concrete example of each.
- Why does `if (memcmp(tag, exp, 16) == 0)` leak, and how does a constant-time compare fix it?
- How does `mask = -(cond)` produce a branchless select, and what must `cond` be reduced to first?
- Why store a boolean as `0x3C3C3C3C` instead of `1`?
- A single glitch skips one `if` — name two code-level countermeasures that catch it.
- SPA vs DPA vs CPA — what does CPA add over DPA?
- What is Flush+Reload, and which attack class relies on it?
- Name three fault-injection methods and one thing each can bypass.
- Why read a security fuse twice? Why verify a signature twice?
- Why can `-O2` break your constant-time C, and how do you defend against it?
- Is masking/blinding protecting against timing or power? What breaks first-order masking?
- Name two on-chip hardware countermeasures against glitching.
