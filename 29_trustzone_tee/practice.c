/*
 * Topic 29: ARM TrustZone & Trusted Execution Environment -- PRACTICE TEMPLATE
 * Fill in each function body marked TODO. Answers are in solution.c.
 * Compile: gcc -std=c11 -Wall -O2 practice.c -o practice && ./practice
 *   (add -Wextra once you have implemented the stubs)
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
    /* TODO: implement. HINT: save cpu->world, overwrite it with target, return the saved value. */
    return NORMAL_WORLD; /* placeholder */
}

/* P2: current-world / current-EL accessors (read the CPU's banked state). */
enum World current_world(const struct cpu_state *cpu) {
    /* TODO: implement. HINT: return the world field of *cpu. */
    return NORMAL_WORLD; /* placeholder */
}
enum EL current_el(const struct cpu_state *cpu) {
    /* TODO: implement. HINT: return the el field of *cpu. */
    return EL0; /* placeholder */
}

/* P3: the NS-bit memory firewall (models a TZASC region check).
 * A NORMAL-world (NS=1) access to secure RAM is refused in hardware. */
enum Status ns_access_secure_memory(const struct cpu_state *cpu, uint32_t addr) {
    /* TODO: implement. HINT: if world==NORMAL_WORLD AND addr is inside [SECURE_BASE, SECURE_BASE+SECURE_SIZE) -> ACCESS_DENIED; else OK. */
    return OK; /* placeholder */
}

/* P4: secure service - hand back the device key ONLY to a secure-world caller
 * running at S-EL1 or above; otherwise deny and scrub the output buffer. */
enum Status secure_service_get_key(const struct cpu_state *cpu, uint8_t out[16]) {
    /* TODO: implement. HINT: deny (scrub out to zero, return ACCESS_DENIED) unless world==SECURE and el>=EL1; on success copy DEVICE_KEY into out. */
    return ACCESS_DENIED; /* placeholder */
}

/* P5: secure service - a toy PRNG, also gated to the secure world.
 * (A real TEE would draw from a hardware TRNG; this is NOT cryptographic.) */
enum Status secure_service_random(const struct cpu_state *cpu, uint32_t seed, uint32_t *out) {
    /* TODO: implement. HINT: require SECURE world and non-NULL out, then *out = seed*1103515245u + 12345u; return OK. */
    return ACCESS_DENIED; /* placeholder */
}

/* P6: the EL3 secure-monitor dispatcher - route an SMC to the right service. */
enum Status secure_monitor_dispatch(struct cpu_state *cpu, uint32_t service_id,
                                    uint32_t arg, struct smc_result *out) {
    /* TODO: implement. HINT: switch on service_id -> SMC_GET_KEY calls get_key(out->key), SMC_GET_RANDOM calls random(arg,&out->value), default BAD_SERVICE. */
    return BAD_SERVICE; /* placeholder */
}

/* P7: the SMC path from the normal world.
 * SMC traps to EL3 -> monitor enters the secure world -> secure OS (S-EL1) runs
 * the service -> monitor returns and restores the caller's world & EL. */
enum Status smc_call(struct cpu_state *cpu, uint32_t service_id, uint32_t arg,
                     struct smc_result *out) {
    /* TODO: implement. HINT: save caller world+el; set el=EL3; world_switch to SECURE; el=EL1; dispatch; then restore the saved world+el; return the status. */
    return OK; /* placeholder */
}

/* P8: HVC contrast. HVC traps to the hypervisor at EL2 and stays in the SAME
 * security world (no NS switch) - unlike SMC which crosses into the secure
 * world at EL3. (SVC, not shown, traps to the OS at EL1.) */
enum Status hvc_call(struct cpu_state *cpu, uint32_t hvc_id, uint32_t *out) {
    /* TODO: implement. HINT: save el, set el=EL2 (world unchanged), if out set *out=hvc_id+1, restore el, return OK. */
    return OK; /* placeholder */
}

/* P9: a Trusted Application entry point running inside the TEE (S-EL0).
 * It copies caller-supplied input into a FIXED buffer, so it must reject any
 * oversized length FIRST - the classic TA input-validation bug prevention. */
enum Status trusted_app_entry(const struct cpu_state *cpu, const uint8_t *in,
                              size_t len, uint32_t *out) {
    /* TODO: implement. HINT: require SECURE world; if len>TA_MAX_INPUT return BAD_INPUT BEFORE reading; else sum the bytes into *out and return OK. */
    return OK; /* placeholder */
}

/* P10: confused-deputy / "boomerang" guard.
 * A TA is handed a buffer pointer by the normal world. Before using it the TA
 * must confirm the WHOLE range lies inside non-secure SHARED memory - never in
 * secure RAM - so it cannot be tricked into touching secure data on the
 * caller's behalf. */
enum Status tee_check_shared_buffer(uint32_t addr, uint32_t len) {
    /* TODO: implement. HINT: reject len==0 or len>SHARED_SIZE (BAD_INPUT); reject addr<SHARED_BASE or addr>SHARED_BASE+SHARED_SIZE-len (ACCESS_DENIED); else OK. */
    return OK; /* placeholder */
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
