# 29 · ARM TrustZone & Trusted Execution Environment

TrustZone splits one CPU into a **secure** and a **normal** world; the TEE is the trusted OS that lives in the secure world and guards keys, DRM, biometrics and payments. `solution.c` is a tiny C state-machine that models the NS bit, EL0–EL3, the SMC path into the EL3 monitor, secure services, a Trusted Application, and the TZASC memory firewall.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **ARM TrustZone** — one physical core presents two *virtual* cores: a **secure world** and a **normal (non-secure) world**, with hardware-enforced isolation. The **NS bit** (`SCR_EL3.NS`) is the security state: `NS=0` = secure, `NS=1` = normal. It is tagged onto every bus transaction so memory and peripherals can tell the worlds apart. *(P1/P3 model the NS bit.)*
2. **Exception levels EL0–EL3** — `EL0` unprivileged apps, `EL1` OS/kernel, `EL2` hypervisor, `EL3` secure monitor. Higher number = more privilege. The world (NS bit) is *orthogonal* to the EL, so you get S-EL0/S-EL1 (secure app / trusted OS) as well as NS-EL0/NS-EL1 (rich app / rich OS).
3. **Secure monitor at EL3** — the only code that can flip the NS bit and switch worlds. It is the trusted gatekeeper sitting *between* the two worlds; all world switches funnel through it.
4. **SMC vs HVC (vs SVC)** — `SMC` (Secure Monitor Call) traps to **EL3** and is how the normal world asks the secure world for a service (a world switch). `HVC` traps to the **EL2** hypervisor and stays in the *same* world (no NS change). `SVC` traps to the **EL1** OS. Know which instruction lands where. *(P7 = SMC, P8 = HVC.)*
5. **TEE concept** — a **Trusted Execution Environment** is an isolated secure-world OS running alongside the "Rich OS" / REE (Android/Linux). Examples: **OP-TEE** (open source), **Qualcomm QSEE/QTEE**, Trustonic Kinibi, Apple's SEP (a separate design). It provides confidentiality + integrity even if the Rich OS is fully compromised.
6. **Trusted Applications (TAs)** — the apps that run *inside* the TEE at S-EL0, behind the GlobalPlatform TEE Client/Internal APIs. A normal-world "client app" opens a session to a TA and invokes commands; the TA does the sensitive work and returns only results. *(P9 models a TA entry point.)*
7. **What runs in the TEE** — **keystore/Keymaster** (key generation, sign/verify without exposing keys), **DRM** (Widevine L1, PlayReady), **biometrics** (fingerprint/face template matching), **mobile payments** (tokenization), plus secure-boot key handling and device attestation. The rule: secrets and their operations stay in the secure world; the normal world only sees results.

## Frequently asked
8. **Secure vs non-secure memory (TZASC)** — the **TrustZone Address Space Controller** partitions DRAM into secure and non-secure regions. A transaction carrying `NS=1` simply cannot reach a secure region — the firewall rejects it in hardware, not in software. *(P3 models this check.)*
9. **Secure vs non-secure peripherals (TZPC)** — the **TrustZone Protection Controller** (and the interconnect) assign peripherals to a world: the crypto engine, fingerprint sensor, or a secure timer can be made reachable *only* from the secure world, so a compromised Rich OS can't drive them.
10. **World-switch cost / latency** — an SMC is not free: trap to EL3, save/restore banked registers and general-purpose state, possible cache/TLB effects, then return. So you **minimize switches** and **batch** work per SMC instead of chattering across the boundary.
11. **Shared memory for TEE communication** — parameters and results travel through a buffer in **non-secure shared memory** that both worlds map; the SMC passes a reference, the TA reads inputs and writes outputs there. The secure world *copies in* what it needs and validates it, rather than trusting the pointer. *(P4's `smc_result` models this shared buffer.)*
12. **Chain-of-trust tie-in (TF-A stages)** — the monitor and TEE don't appear by magic: secure boot verifies them. In Arm Trusted Firmware the stages are `BL1`→`BL2`→**`BL31` (EL3 monitor)**→**`BL32` (TEE, e.g. OP-TEE)**→`BL33` (normal-world U-Boot/UEFI). Each stage measures/verifies the next.

## Good to know / tricky
13. **Cache side-channels across worlds** — the two worlds often share L1/L2/LLC and branch predictors. Secure-world secrets can leak to a normal-world attacker via timing (Prime+Probe, Flush+Reload) even though the NS firewall blocks *direct* access. Constant-time crypto and cache partitioning are mitigations.
14. **Boomerang / confused-deputy** — the normal world hands a *pointer* to a privileged TA and tricks it into reading or writing **secure** memory on its behalf (the "boomerang" attack). A TA must verify that every caller-supplied buffer lies **entirely in non-secure shared memory** before touching it. *(P10 models this guard.)*
15. **Insecure TA input validation** — a TA runs with secure privilege, so an unchecked length or index from the normal world is a *secure-world* buffer overflow. Validate **every** REE-supplied parameter (length, pointer, range) before use — reject first, then act. *(P9 rejects an oversized length.)*
16. **World is orthogonal to privilege** — a common trap: people equate "secure" with "EL3". You can be secure at EL0 (a TA) or non-secure at EL1 (the Linux kernel). Security state (NS) and exception level (EL) are two independent axes.

## Rapid-fire viva questions
- What does the NS bit do, where does it live, and who is allowed to change it?
- Which exception level hosts the secure monitor, and how does the normal world reach it?
- SMC vs HVC vs SVC — where does each trap?
- Name three things that run inside a phone's TEE.
- Which hardware blocks enforce secure vs non-secure **memory** and **peripherals**? (TZASC / TZPC)
- Why is a world switch expensive, and how do you design around it?
- How should a TA safely accept a buffer pointer passed from the normal world? (boomerang / confused-deputy)
- Can the normal world read secure RAM directly? What stops it?
- Where do OP-TEE and Qualcomm QSEE sit in the EL / world model?
- Why are shared caches a problem even when the NS firewall works perfectly?
- True/false: "secure" always means EL3. (Why is that wrong?)
