# 28 · Secure Boot & Chain of Trust

The property every other security feature stands on: prove that only authentic, untampered firmware runs — from immutable ROM all the way up to the OS. `solution.c` is a toy, **SELF-CONTAINED** simulation of a boot chain in pure C (a stand-in "hash" and "signature"), built to make the concepts a Hardware Security Lab probes concrete — the ideas, **not** production code.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **Root of Trust (RoT).** The one thing you trust *without* verifying it, because it is **immutable**: the mask-ROM boot code burned into the SoC plus a hardware anchor (the vendor public-key **hash** in **OTP/eFuse**). It is the first instruction the core executes out of reset, cannot be patched in the field, and every later trust decision is chained back to it. A hardware RoT (ROM + fuses + crypto block) beats a software one because it cannot be reflashed by an attacker.
2. **Chain of trust.** Each stage **verifies the next stage BEFORE transferring control to it**: BootROM authenticates the SBL, the SBL authenticates the bootloader, the bootloader authenticates the kernel. Trust *propagates* from the RoT outward; break any link and everything above it is untrusted. The golden rule is **verify-then-execute**, never execute-then-check. *(P6 `verify_stage`, P10 `boot_chain`.)*
3. **Why a signature, not just a hash.** A bare hash gives **integrity** against *accidental* corruption only — an attacker who patches the image simply recomputes the hash. A **digital signature** = hash **then** encrypt-the-hash with a **private** key; the ROM verifies with the matching **public** key. That adds **authenticity** (only the vendor's private key could have produced it) on top of integrity. Hashes detect noise; signatures detect adversaries. *(P3 `sign_hash`, P5 `verify_signature`.)*
4. **Verified boot vs measured boot.** **Verified boot** *enforces*: a bad signature **halts** the boot (or drops to recovery). **Measured boot** *records*: each stage is hashed and the value **extended into a PCR** (`PCR = H(PCR || measurement)`); nothing is blocked, but the accumulated register is later handed to a verifier via **remote attestation**. Verified = "refuse to run bad code now"; measured = "prove afterwards exactly what ran". Real devices do both. *(P5 enforces, P9 `pcr_extend` measures.)*
5. **Public key in ROM/OTP + revocation.** ROM is small, so it usually stores only the **hash** of the vendor public key in **OTP**; the full key ships with the image and is checked against that fused hash. SoCs provision **multiple key slots** plus **revocation fuses** — blow a bit to permanently kill a compromised key while a backup slot keeps the device bootable. *(P8 `key_revoked`.)*
6. **Anti-rollback.** A properly signed **old** image is still **authentic**, so signature checking alone will happily boot it — yet it may contain a patched vulnerability. Defense: a **monotonic counter / version fuse** storing the minimum allowed version; reject any image whose version is below it, and burn the counter forward on update. This is a **separate** check from the signature precisely because the signature passes. *(P7 `anti_rollback`.)*
7. **Constant-time verification.** Compare signatures/tags with **no secret-dependent branch or early-out**, so boot timing does not leak how many bytes matched. Fold all per-byte differences together and branch only on the final result. *(P4 `ct_eq_u64`.)*

## Frequently asked
8. **Fuses / eFuse / OTP.** **One-Time-Programmable** memory: blowing a bit is physical and **permanent**. Holds the RoT public-key hash, the **secure-boot-enable** bit, monotonic anti-rollback counters, per-device secrets/HUK, and debug-lock bits. Because it cannot be rewritten, it is the trustworthy place to anchor policy an attacker must not change.
9. **Key hierarchy.** You never sign everything with one key. A **root key** signs subordinate/stage keys, which sign the images; this limits **blast radius** (one leaked stage key ≠ game over), enables **rotation** without touching the immutable ROM anchor, and separates duties (a manufacturing key vs a firmware-signing key). Only the top-level public-key hash lives in fuses.
10. **A real SoC boot flow.** Reset → **BootROM** (RoT) → **SBL/primary bootloader** (brings up DRAM, clocks) → **bootloader** (U-Boot/ABL) → **OS/kernel**, with the **secure world / TrustZone** monitor established early. Each arrow is an authenticated hand-off; the same pattern repeats for the DSP/modem/GPU firmware a Qualcomm-class part loads.
11. **Secure-boot-enable & debug lockdown.** Whether verification is actually *enforced* is itself gated by a fuse — dev parts boot unsigned, production parts must not. Production also **disables or authenticates JTAG/UART/DFT**; leaving a debug port open is a classic secure-boot bypass that skips the whole chain.

## Good to know / tricky
12. **TOCTOU during boot (verify-then-load races).** The image you **check** must be the image you **run**. Classic bug: verify a copy in one buffer, then **DMA the real payload into the execution buffer after** the check — an attacker (or a compromised peripheral) swaps the bytes in the **time-of-check-to-time-of-use** gap. Mitigations: verify **in place in the final execution location**, lock/`memcpy` before checking (not after), re-verify after any DMA, and keep the buffer inaccessible to other masters.
13. **Bypass via glitching a check (fault injection).** A voltage/clock/EM **glitch** can make the CPU **skip the `if (bad) halt;` branch** or corrupt the comparison result, so a failed verification is treated as success — defeating perfect crypto without breaking it. Mitigations: **redundant/double verification**, check the *inverse* condition too, boot-flow **step counters**, random delays, and storing "verified" as a hard-to-flip value. *(See Topic 26 — side channels & fault injection.)*
14. **DMA & peripheral trust.** A verified image sitting in RAM can be silently overwritten by an untrusted **bus master doing DMA** (USB, PCIe, a co-processor). Use an **IOMMU/SMMU** to fence off boot memory and (re)verify after any transfer; "verified once" is meaningless if someone else can write the buffer.
15. **Scheme pitfalls.** Textbook/unpadded RSA, a broken hash (SHA-1 collisions → forged images), or a **hardcoded/reused key** all sink secure boot even when the *architecture* is right. Use **SHA-256+**, padded signatures (**RSA-PSS / ECDSA / Ed25519**), and keep private keys in an HSM — never in firmware.
16. **Version binding & secure counters.** Anti-rollback only works if the **version lives inside the signed region** (so it cannot be edited) and the counter is stored somewhere replay-proof — a fuse, a TPM monotonic counter, or **RPMB** on eMMC/UFS. A version field an attacker can rewrite, or a counter they can reset, is no protection.

## Rapid-fire viva questions
- What is a Root of Trust, and why must it be immutable? Where does it physically live?
- State the chain-of-trust order on a typical SoC. What is the one invariant every link must obey?
- Integrity vs authenticity — which does a plain hash give, which needs a signature, and why?
- Verified boot vs measured boot: which one *halts*, which one *attests*, and what is a PCR extend?
- What exactly is stored in eFuses/OTP for secure boot? Why fuses and not flash?
- If an old firmware image is correctly signed, why can it still be dangerous — and what stops it?
- How does key revocation work with only a key hash in ROM? Why a key *hierarchy*?
- Give a concrete TOCTOU bug in a boot loader. How do you close the verify-then-load race?
- How can a hardware **glitch** bypass secure boot without breaking the crypto? Name two mitigations.
- Can an attacker with DMA defeat a "verified" image in RAM? What hardware stops them?
- Why compare a signature in constant time during boot?
- Can you ever update the Root of Trust in the field? What are the implications either way?
