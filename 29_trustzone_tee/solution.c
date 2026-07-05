/*
 * Topic 29: ARM TrustZone & Trusted Execution Environment
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * A tiny, self-contained STATE-MACHINE model of ARM TrustZone in plain C.
 * There is no real secure hardware here; we SIMULATE the security state (the
 * NS bit), the exception levels EL0..EL3, the SMC path into the EL3 secure
 * monitor, two secure services, a Trusted Application, and the memory firewall
 * (TZASC) that stops the normal world reading secure RAM.
 * Each problem is a standalone function; main() drives them.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* --- security state (NS bit) and privilege (exception level) --------------- */
enum World { NORMAL_WORLD = 0, SECURE_WORLD = 1 };   /* SCR_EL3.NS: 1=NS, 0=S   */
enum EL    { EL0 = 0, EL1 = 1, EL2 = 2, EL3 = 3 };   /* app/OS/hyp/monitor       */

/* return / status codes shared by every service */
enum Status { OK = 0, ACCESS_DENIED = 1, BAD_SERVICE = 2, BAD_INPUT = 3 };

/* the "CPU": current security world + current exception level (banked state) */
struct cpu_state { enum World world; enum EL el; };

/* SMC service identifiers (real SMC IDs are 32-bit function numbers) */
enum smc_service { SMC_GET_KEY = 0x01, SMC_GET_RANDOM = 0x02 };

/* Result payload = the non-secure SHARED buffer used to pass data back to the
 * normal world after an SMC (parameters in, results out). */
struct smc_result { uint32_t value; uint8_t key[16]; };

/* --- toy physical memory map (what TZASC would partition in real DRAM) ------ */
#define SECURE_BASE 0x08000000u          /* secure RAM behind the TZASC firewall */
#define SECURE_SIZE 0x00010000u          /* 64 KiB, non-secure masters cannot touch */
#define SHARED_BASE 0x10000000u          /* non-secure shared window for TEE comms  */
#define SHARED_SIZE 0x00010000u          /* 64 KiB, both worlds may read/write      */

#define TA_MAX_INPUT 32                  /* fixed TA input buffer -> must bound length */

/* fake device root key (external linkage so it never trips -Wunused in stubs) */
const uint8_t DEVICE_KEY[16] = {
    0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
    0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF
};

/* --- small pure helpers (used by main for printing) ------------------------ */
int ns_bit(enum World w)          { return w == NORMAL_WORLD ? 1 : 0; }
const char *world_name(enum World w) { return w == SECURE_WORLD ? "SECURE" : "NORMAL"; }
const char *el_name(enum EL e) {
    switch (e) { case EL0: return "EL0"; case EL1: return "EL1";
                 case EL2: return "EL2"; case EL3: return "EL3"; }
    return "EL?";
}
const char *status_name(enum Status s) {
    switch (s) { case OK: return "OK"; case ACCESS_DENIED: return "ACCESS_DENIED";
                 case BAD_SERVICE: return "BAD_SERVICE"; case BAD_INPUT: return "BAD_INPUT"; }
    return "?";
}

/* P1: world switch performed by the EL3 secure monitor.
 * Flips SCR_EL3.NS to enter/leave the secure world; returns the previous world. */
enum World world_switch(struct cpu_state *cpu, enum World target) {
    enum World prev = cpu->world;
    cpu->world = target;
    return prev;
}

/* P2: current-world / current-EL accessors (read the CPU's banked state). */
enum World current_world(const struct cpu_state *cpu) { return cpu->world; }
enum EL    current_el(const struct cpu_state *cpu)    { return cpu->el; }

/* P3: the NS-bit memory firewall (models a TZASC region check).
 * A NORMAL-world (NS=1) access to secure RAM is refused in hardware. */
enum Status ns_access_secure_memory(const struct cpu_state *cpu, uint32_t addr) {
    bool secure_addr = (addr >= SECURE_BASE) && (addr < SECURE_BASE + SECURE_SIZE);
    if (cpu->world == NORMAL_WORLD && secure_addr)
        return ACCESS_DENIED;            /* NS transaction blocked from secure region */
    return OK;
}

/* P4: secure service - hand back the device key ONLY to a secure-world caller
 * running at S-EL1 or above; otherwise deny and scrub the output buffer. */
enum Status secure_service_get_key(const struct cpu_state *cpu, uint8_t out[16]) {
    if (current_world(cpu) != SECURE_WORLD || current_el(cpu) < EL1) {
        if (out) { for (int i = 0; i < 16; i++) out[i] = 0; }  /* never leak stale data */
        return ACCESS_DENIED;
    }
    if (!out) return BAD_INPUT;
    for (int i = 0; i < 16; i++) out[i] = DEVICE_KEY[i];
    return OK;
}

/* P5: secure service - a toy PRNG, also gated to the secure world.
 * (A real TEE would draw from a hardware TRNG; this is NOT cryptographic.) */
enum Status secure_service_random(const struct cpu_state *cpu, uint32_t seed, uint32_t *out) {
    if (current_world(cpu) != SECURE_WORLD) return ACCESS_DENIED;
    if (!out) return BAD_INPUT;
    *out = seed * 1103515245u + 12345u;
    return OK;
}

/* P6: the EL3 secure-monitor dispatcher - route an SMC to the right service. */
enum Status secure_monitor_dispatch(struct cpu_state *cpu, uint32_t service_id,
                                    uint32_t arg, struct smc_result *out) {
    if (!out) return BAD_INPUT;
    switch (service_id) {
        case SMC_GET_KEY:    return secure_service_get_key(cpu, out->key);
        case SMC_GET_RANDOM: return secure_service_random(cpu, arg, &out->value);
        default:             return BAD_SERVICE;   /* unknown SMC function id */
    }
}

/* P7: the SMC path from the normal world.
 * SMC traps to EL3 -> monitor enters the secure world -> secure OS (S-EL1) runs
 * the service -> monitor returns and restores the caller's world & EL. */
enum Status smc_call(struct cpu_state *cpu, uint32_t service_id, uint32_t arg,
                     struct smc_result *out) {
    enum World caller_world = cpu->world;
    enum EL    caller_el    = cpu->el;

    cpu->el = EL3;                        /* SMC instruction traps to the monitor */
    world_switch(cpu, SECURE_WORLD);      /* monitor clears NS: enter secure world */
    cpu->el = EL1;                        /* hand off to the secure OS at S-EL1    */

    enum Status st = secure_monitor_dispatch(cpu, service_id, arg, out);

    world_switch(cpu, caller_world);      /* return path: restore NS bit ...       */
    cpu->el = caller_el;                  /* ... and the caller's exception level  */
    return st;
}

/* P8: HVC contrast. HVC traps to the hypervisor at EL2 and stays in the SAME
 * security world (no NS switch) - unlike SMC which crosses into the secure
 * world at EL3. (SVC, not shown, traps to the OS at EL1.) */
enum Status hvc_call(struct cpu_state *cpu, uint32_t hvc_id, uint32_t *out) {
    enum EL caller_el = cpu->el;
    cpu->el = EL2;                        /* hypercall traps to EL2, world unchanged */
    if (out) *out = hvc_id + 1;           /* trivial stand-in hypervisor service     */
    cpu->el = caller_el;
    return OK;
}

/* P9: a Trusted Application entry point running inside the TEE (S-EL0).
 * It copies caller-supplied input into a FIXED buffer, so it must reject any
 * oversized length FIRST - the classic TA input-validation bug prevention. */
enum Status trusted_app_entry(const struct cpu_state *cpu, const uint8_t *in,
                              size_t len, uint32_t *out) {
    if (current_world(cpu) != SECURE_WORLD) return ACCESS_DENIED;  /* TAs run in the TEE */
    if (len > TA_MAX_INPUT) return BAD_INPUT;   /* refuse before touching the buffer */
    uint8_t buf[TA_MAX_INPUT];
    uint32_t sum = 0;
    for (size_t i = 0; i < len; i++) { buf[i] = in ? in[i] : 0; sum += buf[i]; }
    if (out) *out = sum;                        /* toy "work" = checksum of the input */
    return OK;
}

/* P10: confused-deputy / "boomerang" guard.
 * A TA is handed a buffer pointer by the normal world. Before using it the TA
 * must confirm the WHOLE range lies inside non-secure SHARED memory - never in
 * secure RAM - so it cannot be tricked into touching secure data on the
 * caller's behalf. */
enum Status tee_check_shared_buffer(uint32_t addr, uint32_t len) {
    if (len == 0 || len > SHARED_SIZE) return BAD_INPUT;
    if (addr < SHARED_BASE) return ACCESS_DENIED;               /* below the window (e.g. secure RAM) */
    if (addr > SHARED_BASE + SHARED_SIZE - len) return ACCESS_DENIED; /* runs off the end */
    return OK;
}

int main(void) {
    struct cpu_state cpu = { NORMAL_WORLD, EL0 };   /* boot as a normal-world app at EL0 */
    printf("== TrustZone toy model ==  (NS bit: 0=secure world, 1=normal world)\n");
    printf("boot: world=%s el=%s\n\n", world_name(cpu.world), el_name(cpu.el));

    /* P1: the secure monitor performs a world switch */
    enum World prev = world_switch(&cpu, SECURE_WORLD);
    printf("%-33s = %s (was %s, NS %d->%d)\n", "P1  world_switch NORMAL->SECURE",
           world_name(cpu.world), world_name(prev), ns_bit(prev), ns_bit(cpu.world));
    world_switch(&cpu, NORMAL_WORLD);               /* back to the normal-world app */

    /* P2: read the current banked state */
    printf("%-33s = %s / %s\n", "P2  current world / EL",
           world_name(current_world(&cpu)), el_name(current_el(&cpu)));

    /* P3: normal world tries to read secure RAM (blocked) and shared RAM (ok) */
    enum Status a1 = ns_access_secure_memory(&cpu, SECURE_BASE);
    enum Status a2 = ns_access_secure_memory(&cpu, SHARED_BASE);
    printf("%-33s = %s\n", "P3  NS-world read secure RAM", status_name(a1));
    printf("%-33s = %s\n", "P3  NS-world read shared RAM", status_name(a2));

    /* P4: normal-world app requests the key via SMC (secure world serves it) */
    struct smc_result res = {0};
    enum Status st_key = smc_call(&cpu, SMC_GET_KEY, 0, &res);
    printf("%-33s = %s key=%02X%02X%02X%02X..\n", "P4  SMC get_key (NS->TEE->NS)",
           status_name(st_key), res.key[0], res.key[1], res.key[2], res.key[3]);

    /* P5: SMC to the PRNG service */
    struct smc_result res2 = {0};
    enum Status st_rnd = smc_call(&cpu, SMC_GET_RANDOM, 0xABCDu, &res2);
    uint32_t rnd = res2.value;
    printf("%-33s = %s value=0x%08X\n", "P5  SMC get_random", status_name(st_rnd), rnd);

    /* P6: SMC with an unknown function id */
    struct smc_result res3 = {0};
    enum Status st_bad = smc_call(&cpu, 0x99u, 0, &res3);
    printf("%-33s = %s\n", "P6  SMC unknown service 0x99", status_name(st_bad));

    /* P7: normal world calls the secure service DIRECTLY (no SMC) -> denied */
    uint8_t nskey[16] = {0};
    enum Status st_direct = secure_service_get_key(&cpu, nskey);
    printf("%-33s = %s\n", "P7  direct get_key from NS world", status_name(st_direct));

    /* P8: an HVC hypercall (EL2, same world) for contrast with SMC */
    uint32_t hvc_out = 0;
    enum Status st_hvc = hvc_call(&cpu, 0x10u, &hvc_out);
    printf("%-33s = %s (same world, no NS switch)\n",
           "P8  HVC hypercall (traps to EL2)", status_name(st_hvc));

    /* --- now enter the TEE to exercise the Trusted Application --- */
    world_switch(&cpu, SECURE_WORLD);
    cpu.el = EL0;                                   /* the TA runs at S-EL0 */

    /* P9: TA accepts a well-sized input, then rejects an oversized one */
    uint8_t good_in[8] = { 1,2,3,4,5,6,7,8 };
    uint32_t ta_sum = 0;
    enum Status st_ta = trusted_app_entry(&cpu, good_in, sizeof good_in, &ta_sum);
    printf("%-33s = %s sum=%u\n", "P9  TA input len=8", status_name(st_ta), ta_sum);
    enum Status st_ta_bad = trusted_app_entry(&cpu, good_in, 99, NULL);
    printf("%-33s = %s\n", "P9  TA input len=99 (oversized)", status_name(st_ta_bad));

    /* P10: TA validates a caller-supplied buffer pointer (boomerang guard) */
    enum Status st_boom = tee_check_shared_buffer(SECURE_BASE, 64);
    printf("%-33s = %s (boomerang blocked)\n",
           "P10 TA buf ptr in secure RAM", status_name(st_boom));
    enum Status st_shared = tee_check_shared_buffer(SHARED_BASE, 64);
    printf("%-33s = %s\n", "P10 TA buf ptr in shared RAM", status_name(st_shared));

    world_switch(&cpu, NORMAL_WORLD);               /* leave the TEE */
    cpu.el = EL0;
    return 0;
}
