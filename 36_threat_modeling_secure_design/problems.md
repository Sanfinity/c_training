# 36 · Threat Modeling & Secure Design

Thinking like an attacker on paper *before* you write a line: enumerate threats with STRIDE, rate them, then bake in fail-closed, least-privilege defenses — `solution.c` is a toy hardware-root-of-trust firmware-update subsystem that turns each principle into runnable code.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **STRIDE** — the canonical threat taxonomy you walk every data-flow element through: **S**poofing (forged identity), **T**ampering (unauthorized modification), **R**epudiation (deniable action), **I**nformation disclosure (leak), **D**enial of service (lost availability), **E**levation of privilege (gaining rights). Each letter attacks a property (authenticity, integrity, non-repudiation, confidentiality, availability, authorization) and has a matching defense.
2. **Spoofing → authentication** — prove identity before you extend trust: signed images, mutually-authenticated channels, hardware-bound tokens. A debug port must open only to a caller that presents the provisioned unlock token. → debug_unlock (P11)
3. **Tampering → integrity** — detect any post-signing modification with a measurement (hash) plus a signature over the image; one flipped bit changes the hash and fails verification. → verify_image (P5)
4. **Information disclosure → confidentiality** — don't leak secrets through memory, timing, or struct padding: wipe key material when done and compare secrets in constant time. → secure_scrub (P12), ct_eq_bytes (P3)
5. **Elevation of privilege → authorization** — the attacker gains rights they shouldn't; contain it with least privilege plus fail-closed authorization so an unlisted actor or operation is denied by default. → authorize (P7), has_capability (P8)
6. **Risk rating (DREAD / CVSS)** — quantify each threat so mitigation effort goes to the worst first. **DREAD** averages Damage / Reproducibility / Exploitability / Affected-users / Discoverability; **CVSS** scores Base / Temporal / Environmental on 0–10. Rating turns an unbounded threat list into a ranked backlog. → dread_score (P1)
7. **Attack surface & trust boundaries** — every input crossing from lower to higher trust (network, USB, debug pins, DMA, IPC, a syscall edge) is where a threat enters; enumerate these boundaries on the design and shrink the surface (disable unused interfaces, drop privileges, minimize exposed code).
8. **Least privilege** — grant each component the minimum capabilities it needs and nothing more, so a compromised task cannot reach keys or a debug port. Model rights as a capability set and require an exact superset match. → has_capability (P8)
9. **Secure defaults / fail-closed** — the default state denies; on any error, timeout, or unrecognized input the system moves to the *safe* (denied / aborted) state, never the permissive one. Fail-open is how "temporary" bypasses ship to production. → authorize (P7), ota_step (P10)
10. **Complete mediation** — check *every* access at a single choke point with no cached "already allowed" shortcut, and validate every field of untrusted input before any of it is used. → validate_descriptor (P9), verify_image (P5)

## Frequently asked
11. **Defense in depth** — layer independent controls (signed image + anti-rollback + least privilege + input validation + a fail-closed state machine) so that one failed control does not sink the ship. It is *independent* layers, not just "more checks" on the same assumption.
12. **Threat modeling process** — the loop: **decompose** the system (draw a DFD) → **identify** threats (STRIDE per element) → **rate** them (DREAD / CVSS) → **mitigate / accept / transfer** each → validate, then iterate as the design changes. Do it early — fixes on a diagram are free.
13. **Data-flow diagrams (DFD)** — model processes, data stores, external entities, and the flows between them, then draw trust boundaries where data crosses a privilege level. Those boundary-crossing flows are exactly where you apply STRIDE.
14. **Root of trust (RoT)** — an immutable, minimal anchor (mask-ROM code + fused keys) that every later trust decision chains from; keep it small and unchangeable, and derive purpose-specific sub-keys so the root secret never leaves it. → rot_derive (P4)
15. **Key management lifecycle** — generate → provision/store → use → rotate → revoke → destroy. Separate keys by purpose to limit blast radius, and zeroize copies the moment they die. → rot_derive (P4), secure_scrub (P12)
16. **Anti-rollback (downgrade protection)** — an old but still-validly-signed image with a known bug must not be re-installable; gate installs on a monotonic version fuse/counter, kept *separate* from the signature check. → anti_rollback (P6)
17. **Constant-time comparison** — compare secrets, signatures, and MACs with no data-dependent branch or early exit, so response timing can't reveal how many leading bytes matched (a padding-oracle / HMAC-timing class bug). → ct_eq_bytes (P3)
18. **Measurement / integrity hash** — a collision-resistant hash over the image is the integrity primitive that verified boot, secure update, and remote attestation all build on (SHA-256 in production; a toy FNV-1a here). → fnv1a_64 (P2)
19. **Design a secure OTA update** — the classic drill: an authenticated + integrity-checked image, anti-rollback, A/B slots with an atomic commit and auto-rollback on boot-failure, and a fail-closed state machine that *verifies before it applies*. → ota_step (P10), verify_image (P5), anti_rollback (P6)

## Good to know / tricky
20. **Design secure debug / JTAG unlock** — debug access is full elevation, so treat it as a lifecycle decision: fuse it off in production, or gate it behind an authenticated challenge–response token bound to that specific device, and re-lock on reset. → debug_unlock (P11)
21. **Design secure key storage** — never keep the raw root key in general-purpose memory: hold it in fuses/OTP or a secure element, derive per-purpose sub-keys, use them in place, and scrub every copy right after use. → rot_derive (P4), secure_scrub (P12)
22. **Repudiation → audit logging** — tamper-evident, append-only logs with monotonic counters or signatures so an action cannot later be denied or quietly erased; without them you cannot do incident forensics.
23. **DoS resistance** — bound every allocation and loop by validating sizes and lengths up front; reject zero-length and oversized inputs before doing any work, and rate-limit expensive operations. → validate_descriptor (P9)
24. **Confused deputy & TOCTOU** — a privileged component tricked into acting on an attacker's behalf; pass unforgeable capabilities/handles instead of names, and check-then-use atomically to close the race window.
25. **Assume breach / minimize blast radius** — design as though one layer is already owned: compartmentalize, rotate keys, and keep what any single compromise can reach as small as possible. This is the mindset behind least privilege, key separation, and defense in depth.

## Rapid-fire viva questions
- Expand STRIDE and name the security property each letter attacks.
- Where do you draw a trust boundary on a DFD, and why *there*?
- DREAD vs CVSS — what does each score, and why bother rating at all?
- Fail-open vs fail-closed — which does a security control pick, and why?
- What exactly is "complete mediation," and how does a cached permission break it?
- Why derive purpose-specific sub-keys instead of using the root key directly?
- Why must anti-rollback be a *separate* gate from signature verification?
- Why compare a signature or token in constant time?
- Design a secure OTA update in 60 seconds — list the controls in order.
- How do you lock down JTAG on a production board yet still allow RMA debug?
- Name three ways to shrink the attack surface of an embedded device.
- Why is "defense in depth" more than just adding more checks?
