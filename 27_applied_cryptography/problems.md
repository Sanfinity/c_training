# 27 · Applied Cryptography

The primitives a Hardware Security Lab lives on: ciphers, hashes, MACs, public-key math, and randomness. You will be grilled on *what each primitive guarantees*, *how they compose*, and *how they fail* (nonce reuse, ECB, timing). `solution.c` has tiny, **educational-only, NOT secure** toy versions in pure C — the concepts, not production code.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **Symmetric vs asymmetric.** Symmetric (AES, ChaCha20): one shared secret, fast, bulk data — problem is *key distribution*. Asymmetric (RSA, ECC): a public/private key pair, slow, solves key exchange & signatures. Real systems are **hybrid**: asymmetric to agree a key, symmetric to move the data. *(P8 = toy RSA.)*
2. **Block vs stream ciphers.** A block cipher (AES) transforms fixed 128-bit blocks and needs a *mode* + padding. A stream cipher (or a block cipher in CTR/CFB/OFB) produces a keystream XORed into the plaintext, byte-granular, no padding. XOR-with-keystream is the shape of every stream cipher. *(P1 = a toy XOR "stream".)*
3. **AES & modes of operation.** AES = 128-bit block, 128/192/256-bit keys, 10/12/14 rounds (SubBytes/ShiftRows/MixColumns/AddRoundKey). Modes matter more than the cipher:
   - **ECB** — same plaintext block → same ciphertext block. Leaks structure (the infamous "ECB penguin"). **Never use.**
   - **CBC** — chains each block with the previous ciphertext via XOR; needs a random **IV** and padding; malleable without a MAC (padding-oracle attacks).
   - **CTR** — turns AES into a stream cipher by encrypting a counter; parallelizable, no padding; **catastrophic on nonce reuse.**
   - **GCM (AEAD)** — CTR + a GHASH authentication tag: gives confidentiality **and** integrity in one pass, plus authenticated-but-unencrypted associated data. Prefer AEAD.
4. **Hash properties.** A cryptographic hash H is one-way and collision-resistant. Three guarantees, weakest→strongest to break: **preimage** (given h, find m: H(m)=h), **2nd-preimage** (given m1, find m2≠m1 with equal hash), **collision** (find *any* m1≠m2 colliding — birthday bound ≈ 2^(n/2)). Fixed-size output, avalanche, deterministic. **MD5/SHA-1 are broken** (practical collisions) — use **SHA-2** (SHA-256/512) or **SHA-3** (Keccak sponge). *(P2/P3 show FNV/djb2/CRC — fast checksums that are NOT cryptographic.)*
5. **MAC / HMAC.** A MAC gives *integrity + authenticity* using a shared key: only key-holders can produce/verify the tag. **HMAC** = H((K⊕opad) ‖ H((K⊕ipad) ‖ m)); the nested construction resists length-extension that plain H(K‖m) suffers on Merkle–Damgård hashes. Verify with a **constant-time** compare. *(P4 = toy HMAC over FNV.)*
6. **RSA.** Keygen: pick primes p,q; n=pq; φ=(p−1)(q−1); pick public e; **d = e⁻¹ mod φ** (the modular inverse). Encrypt/verify uses e, decrypt/sign uses d: c=mᵉ mod n, m=cᵈ mod n. Security rests on integer factorization. **Textbook RSA is insecure** — always pad: **OAEP** for encryption, **PSS** for signatures (deterministic/unpadded RSA leaks and is malleable). *(P6 modinv, P7 modexp, P8 round-trip with n=3233,e=17,d=2753.)*
7. **Constant-time comparison & why.** Comparing secrets (MAC tags, passwords) with `memcmp`/early-return leaks *where* the first mismatch is via timing — an attacker recovers the tag byte-by-byte. Compare by OR-ing all per-byte XORs and branch only on the final result; no data-dependent branches or table lookups. *(P9.)*

## Frequently asked
8. **Diffie–Hellman (DH).** Public g,p; each side picks secret a,b; exchange g^a, g^b mod p; both compute the shared g^ab mod p. An eavesdropper faces the discrete-log problem. Use **ephemeral** DH (DHE/ECDHE) for **forward secrecy**; DH alone is unauthenticated → needs signatures to stop man-in-the-middle. *(P7 mod_exp is the core operation.)*
9. **ECC / ECDH vs RSA.** Elliptic-curve crypto gives equivalent security with far smaller keys (**256-bit ECC ≈ 3072-bit RSA**) → less RAM, less bandwidth, faster on constrained/embedded parts. ECDH = DH over a curve group; ECDSA/EdDSA for signatures. Hardware labs love ECC for exactly the size/power reasons.
10. **Digital signatures.** Sign the *hash* with the **private** key, verify with the **public** key: gives integrity, authenticity, and **non-repudiation** (a MAC cannot — both parties share the key). RSA-PSS, ECDSA, Ed25519. This is the trust root of secure boot (Topic 28).
11. **RNG: TRNG vs PRNG vs CSPRNG/DRBG.** A **TRNG** harvests physical entropy (jitter, thermal noise) — true randomness but slow/biased, needs conditioning. A **PRNG** (LCG, xorshift, Mersenne Twister) is fast and deterministic from a seed — **predictable, never for keys**. A **CSPRNG/DRBG** (Fortuna, CTR-DRBG, `/dev/urandom`) is a PRNG that's unpredictable even given past output, **seeded from a TRNG**. Keys/IVs/nonces MUST come from a CSPRNG. *(P5 = LCG + xorshift, deliberately weak.)*
12. **Nonce / IV reuse dangers.** Reusing a nonce is often *worse* than a weak cipher: in CTR/stream ciphers two messages under the same keystream give C1⊕C2 = P1⊕P2 (keystream cancels → plaintext recovery); in GCM a repeated nonce leaks the authentication key H and lets an attacker forge tags. IVs must be unique (CTR/GCM) and often unpredictable (CBC).

## Good to know / tricky
13. **Key management & rotation.** The hard part isn't the algorithm — it's key lifecycle: generation (CSPRNG), storage (**HSM / secure element / OTP fuses / TrustZone**, never in plaintext flash), distribution, **rotation** (limit data-per-key and blast radius of a leak), and destruction/zeroization. Use a KDF/HKDF to derive many keys from one master; never hardcode keys in firmware.
14. **XOR "cipher" reality (P1).** XOR with a repeating key is trivially broken (frequency analysis / crib-dragging); only a **one-time pad** — key as long as the message, truly random, never reused — is information-theoretically secure, and it's impractical for the same reasons. Toy demo only.
15. **CRC is not a MAC (P3).** CRC/checksums (and FNV/djb2, P2) detect *accidental* corruption and are linear/forgeable — they give **zero** cryptographic integrity. An attacker can flip bits and fix the CRC. Use HMAC or an AEAD tag for adversarial integrity.
16. **Modular-arithmetic building blocks.** Public-key math reduces to **gcd / extended Euclid (modular inverse, P6)** and **modular exponentiation by square-and-multiply (P7)**. Naïve big-number code leaks via timing/power → real libs use Montgomery ladders and constant-time paths (ties into Topic 26 side-channels).
17. **Padding oracles & AEAD.** A server that reveals "bad padding" vs "bad MAC" as distinct errors (or different timing) leaks enough to decrypt CBC ciphertext. Fixes: **encrypt-then-MAC** or, better, an AEAD mode (GCM/ChaCha20-Poly1305) so there's nothing to probe.

## Rapid-fire viva questions
- Why is ECB insecure even though AES is strong? What does the "ECB penguin" show?
- Symmetric vs asymmetric — which encrypts bulk data and why? What is a hybrid cryptosystem?
- Define preimage, second-preimage, and collision resistance. What's the birthday bound for an n-bit hash?
- Why are MD5 and SHA-1 deprecated? What do you use instead?
- MAC vs digital signature — which gives non-repudiation, and why doesn't the other?
- Why must textbook RSA never be used? What are OAEP and PSS for?
- What exactly breaks if you reuse a nonce/IV in CTR mode? In GCM?
- PRNG vs CSPRNG vs TRNG — which may generate an AES key, and what seeds it?
- Why compare HMAC tags in constant time? What leaks if you don't?
- 256-bit ECC vs RSA — roughly what RSA key size is equivalent, and why prefer ECC on embedded?
- Is a CRC a valid integrity check against an attacker? Why not?
- In RSA, what is `d` and how is it computed from `e` and φ(n)?
