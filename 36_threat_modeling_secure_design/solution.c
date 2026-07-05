/*
 * Topic 36: Threat Modeling & Secure Design
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * A toy, SELF-CONTAINED secure-design artifact built with ONLY the C standard
 * library: a hardware-root-of-trust firmware-update subsystem. The "hash"
 * (FNV-1a) and the "signature" (XOR with a derived key) are NOT cryptographically
 * secure -- they stand in for SHA-256 and an asymmetric signature so the design
 * CONCEPTS a threat-modeling interview probes (STRIDE defenses, fail-closed gates,
 * least privilege, root of trust, anti-rollback, secure debug unlock) stay
 * concrete. Each problem is a standalone function; main() demonstrates all of them.
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

/* A rated threat: five DREAD factors, each on a 0..10 scale. */
struct Threat {
    const char *name;
    unsigned damage, reproducibility, exploitability, affected, discoverability;
};

/* P1: DREAD risk rating -- the "rate" step of threat modeling
 * (decompose -> identify -> rate -> mitigate). Average the five factors into a
 * single 0..10 score so scarce mitigation effort targets the worst threats
 * first. (CVSS is the industry variant; DREAD is the classic teaching model.) */
unsigned dread_score(const struct Threat *t) {
    unsigned sum = t->damage + t->reproducibility + t->exploitability
                 + t->affected + t->discoverability;
    return sum / 5u;                     /* mean, same 0..10 scale */
}

/* P2: FNV-1a 64-bit -- a fast NON-cryptographic hash used as the image
 * "measurement". A real design uses a collision-resistant hash (SHA-256/384);
 * FNV only keeps this demo dependency-free. Integrity checks and verified boot
 * both build on a measurement like this. */
uint64_t fnv1a_64(const void *data, size_t n) {
    const uint8_t *p = (const uint8_t *)data;
    uint64_t h = 0xcbf29ce484222325ULL;          /* FNV offset basis */
    for (size_t i = 0; i < n; i++) {
        h ^= p[i];
        h *= 0x100000001b3ULL;                    /* FNV prime */
    }
    return h;
}

/* P3: constant-time byte-buffer equality. OR every byte difference together so
 * the loop NEVER exits early -- comparing a signature or unlock token this way
 * leaks no timing signal about WHERE or WHETHER it first differs (defends the
 * Information-disclosure leg of STRIDE). Returns 1 iff all n bytes match. */
int ct_eq_bytes(const uint8_t *a, const uint8_t *b, size_t n) {
    uint8_t diff = 0;
    for (size_t i = 0; i < n; i++) diff |= (uint8_t)(a[i] ^ b[i]);
    return diff == 0;
}

/* P4: root-of-trust key derivation. The immutable root secret lives in fuses/OTP
 * and NEVER leaves the RoT; each consumer gets a distinct sub-key = KDF(root,
 * label). Separating keys by purpose limits blast radius if one leaks, and
 * rotating a key means bumping its label/epoch (the key-management lifecycle).
 * Toy KDF = hash of (root || label). */
uint64_t rot_derive(uint64_t root_seed, const char *label) {
    uint8_t buf[8 + 32];
    size_t n = 0;
    for (int i = 0; i < 8; i++) buf[n++] = (uint8_t)(root_seed >> (8 * i));
    while (*label && n < sizeof buf) buf[n++] = (uint8_t)*label++;
    return fnv1a_64(buf, n);
}

/* An image = a named, versioned blob of code carrying its signature. */
struct Image {
    const char    *name;
    uint32_t       version;
    const uint8_t *code;
    size_t         len;
    uint64_t       signature;            /* toy: fnv1a(code) ^ signing_key */
};

/* P5: verify an image -- the complete-mediation gate EVERY image passes before it
 * is trusted (defends Spoofing + Tampering). Recompute the measurement, re-derive
 * the expected signature from the root of trust, and constant-time compare. One
 * flipped code bit changes the hash -> the signatures differ -> reject. 1 = ok. */
int verify_image(const struct Image *img, uint64_t signing_key) {
    uint64_t expected = fnv1a_64(img->code, img->len) ^ signing_key;
    uint8_t got[8], want[8];
    for (int i = 0; i < 8; i++) got[i]  = (uint8_t)(img->signature >> (8 * i));
    for (int i = 0; i < 8; i++) want[i] = (uint8_t)(expected      >> (8 * i));
    return ct_eq_bytes(got, want, sizeof got);
}

/* P6: anti-rollback / downgrade protection, FAIL-CLOSED. A rolled-back image can
 * be perfectly authentic (P5) yet still forbidden -- an old signed build with a
 * known bug must not be re-installable -- so this is a SEPARATE gate. Reject any
 * version below the monotonic minimum burned into a fuse; on accept, raise it.
 * 1 = accept & commit the new floor, 0 = reject. */
int anti_rollback(uint32_t version, uint32_t *fuse_min) {
    if (version < *fuse_min) return 0;   /* downgrade attempt -> reject */
    *fuse_min = version;                 /* burn the monotonic floor forward */
    return 1;
}

/* Actors and operations for the access-control gates. */
enum Role { ROLE_GUEST, ROLE_OPERATOR, ROLE_VENDOR };
enum Op   { OP_READ_STATUS, OP_START_UPDATE, OP_DEBUG_UNLOCK };

/* P7: fail-closed authorization (default DENY / secure default). The decision
 * starts DENIED and only an explicit positive rule flips it to ALLOW -- an
 * unknown role, unknown op, or any fall-through path stays denied. This is the
 * safe inverse of a default-allow blacklist. Returns 1 = allow, 0 = deny. */
int authorize(enum Role role, enum Op op) {
    int allow = 0;                       /* SECURE DEFAULT: deny until proven */
    if (role == ROLE_VENDOR)
        allow = 1;                        /* vendor may perform every op */
    else if (role == ROLE_OPERATOR)
        allow = (op == OP_READ_STATUS || op == OP_START_UPDATE);
    else if (role == ROLE_GUEST)
        allow = (op == OP_READ_STATUS);
    return allow;                        /* anything unlisted stays denied */
}

/* Capabilities as one-hot bits -- grant a subject only the few it needs. */
enum Cap {
    CAP_FLASH_WRITE = 1u << 0,
    CAP_KEY_READ    = 1u << 1,
    CAP_DEBUG       = 1u << 2,
    CAP_NET         = 1u << 3
};

/* P8: least-privilege capability check. 'have' is the subject's granted set,
 * 'need' the set an operation demands; allow ONLY if have is a superset of need.
 * An OTA task holding just FLASH_WRITE|NET cannot read keys or open debug even if
 * it is compromised -- that is the blast-radius limit least privilege buys. */
int has_capability(unsigned have, unsigned need) {
    return (have & need) == need;        /* every required bit present */
}

#define MAX_IMAGE_LEN 4096u

/* An externally-supplied update request -- every field is untrusted. */
struct UpdateDescriptor {
    uint32_t version;
    uint32_t code_len;
    uint32_t slot;
};

/* P9: complete-mediation input validation. EVERY untrusted field is checked at a
 * single choke point BEFORE any of it is used -- nothing is assumed sane and
 * there is no bypass. Reject zero-length or oversized images (also a DoS guard)
 * and out-of-range slots up front, fail closed. Returns 1 = accept. */
int validate_descriptor(const struct UpdateDescriptor *d) {
    if (d->code_len == 0u || d->code_len > MAX_IMAGE_LEN) return 0;
    if (d->slot > 1u)     return 0;      /* only A/B slots 0 and 1 exist */
    if (d->version == 0u) return 0;      /* 0 is reserved / uninitialized */
    return 1;
}

/* The OTA update states and the events that drive them. */
enum OtaState { OTA_IDLE, OTA_DOWNLOAD, OTA_VERIFY, OTA_APPLY, OTA_COMMIT, OTA_ABORT };
enum OtaEvent { EV_START, EV_CHUNK_DONE, EV_VERIFY_OK, EV_VERIFY_FAIL, EV_APPLY_OK, EV_COMMIT_OK };

/* Human-readable state name (scaffolding, not a graded problem). */
const char *ota_name(enum OtaState s) {
    switch (s) {
        case OTA_IDLE:     return "Idle";
        case OTA_DOWNLOAD: return "Download";
        case OTA_VERIFY:   return "Verify";
        case OTA_APPLY:    return "Apply";
        case OTA_COMMIT:   return "Commit";
        case OTA_ABORT:    return "Abort";
    }
    return "?";
}

/* P10: the OTA update state machine, FAIL-CLOSED. The only legal flow is
 * Idle -> Download -> Verify -> Apply -> Commit -> Idle, and the image is
 * verified BEFORE it is applied. ANY unexpected event, or a verify failure, drops
 * the machine to ABORT rather than stumbling forward -- so a fuzzed or
 * out-of-order event can never skip the Verify step. Returns the next state. */
enum OtaState ota_step(enum OtaState s, enum OtaEvent e) {
    switch (s) {
        case OTA_IDLE:     if (e == EV_START)      return OTA_DOWNLOAD; break;
        case OTA_DOWNLOAD: if (e == EV_CHUNK_DONE) return OTA_VERIFY;   break;
        case OTA_VERIFY:
            if (e == EV_VERIFY_OK)   return OTA_APPLY;   /* verified -> apply */
            if (e == EV_VERIFY_FAIL) return OTA_ABORT;   /* bad image -> abort */
            break;
        case OTA_APPLY:    if (e == EV_APPLY_OK)   return OTA_COMMIT;   break;
        case OTA_COMMIT:   if (e == EV_COMMIT_OK)  return OTA_IDLE;     break;
        case OTA_ABORT:    break;                     /* terminal until reset */
    }
    return OTA_ABORT;                    /* FAIL CLOSED: anything unexpected aborts */
}

/* Device lifecycle states -- they gate what debug is permitted. */
enum Lifecycle { LC_DEV, LC_PRODUCTION, LC_RMA };

/* P11: authenticated debug / JTAG unlock gate, FAIL-CLOSED. Opening a debug port
 * is a full-elevation primitive, so it is refused unless BOTH hold: the lifecycle
 * permits it (production is fused off) AND the caller presents the exact
 * provisioned token, compared in constant time (P3) to defeat timing guesses.
 * Returns 1 = unlock granted. */
int debug_unlock(enum Lifecycle lc, const uint8_t *token, size_t token_len,
                 const uint8_t *expected, size_t expected_len) {
    if (lc == LC_PRODUCTION)       return 0;     /* debug fused off in production */
    if (token_len != expected_len) return 0;     /* length mismatch -> reject */
    return ct_eq_bytes(token, expected, expected_len);
}

/* P12: secure erase of key material (defends Information-disclosure). A plain
 * memset just before a buffer dies is a DEAD store the optimizer may delete,
 * leaving the key in freed stack/heap. A VOLATILE write must happen, so the
 * secret is really gone (this is what memset_s / explicit_bzero exist for). */
void secure_scrub(void *p, size_t n) {
    volatile uint8_t *v = (volatile uint8_t *)p;
    while (n--) *v++ = 0;
}

int main(void) {
    uint64_t root_seed = 0xA5A5F00DDEADBEEFULL;      /* fused root secret (toy) */

    /* ---- P1: threat modeling -- rate before you mitigate ---- */
    puts("== Threat modeling: rate, then mitigate (DREAD) ==");
    struct Threat t_ota  = { "unsigned OTA image", 9, 8, 8, 9, 6 };
    struct Threat t_jtag = { "open JTAG in field", 10, 9, 7, 7, 8 };
    unsigned s_ota  = dread_score(&t_ota);
    unsigned s_jtag = dread_score(&t_jtag);
    printf("P1  %-28s = %u/10 (%s)\n", t_ota.name, s_ota,
           s_ota  >= 7u ? "HIGH" : s_ota  >= 4u ? "MED" : "LOW");
    printf("P1  %-28s = %u/10 (%s)\n", t_jtag.name, s_jtag,
           s_jtag >= 7u ? "HIGH" : s_jtag >= 4u ? "MED" : "LOW");

    /* ---- P2..P4: root of trust, measurement, key derivation ---- */
    puts("\n== Root of trust: measure & derive keys ==");
    uint64_t img_key  = rot_derive(root_seed, "image-signing");
    uint64_t stor_key = rot_derive(root_seed, "data-storage");
    printf("P2  %-28s = 0x%016" PRIx64 "\n", "fnv1a_64(\"firmware\")",
           fnv1a_64("firmware", 8));
    uint8_t e1[4] = { 1,2,3,4 }, e2[4] = { 1,2,3,4 }, d1[4] = { 1,2,3,9 };
    printf("P3  %-28s = %d / %d\n", "ct_eq_bytes equal / differ",
           ct_eq_bytes(e1, e2, 4), ct_eq_bytes(e1, d1, 4));
    printf("P4  %-28s = %s\n", "rot_derive keys distinct",
           img_key != stor_key ? "yes (per-purpose)" : "no");

    /* ---- P5/P6: firmware image gates ---- */
    puts("\n== Firmware image gates (verify + anti-rollback) ==");
    static const uint8_t app_code[] = { 0x41,0x50,0x50,0x00, 0xCA,0xFE,0xBA,0xBE, 0x13,0x37 };
    struct Image app = { "app", 5, app_code, sizeof app_code, 0 };
    app.signature = fnv1a_64(app.code, app.len) ^ img_key;   /* signed at vendor */
    int v_ok = verify_image(&app, img_key);

    uint8_t bad_code[sizeof app_code];
    memcpy(bad_code, app_code, sizeof app_code);
    bad_code[4] ^= 0x01u;                                    /* attacker patch */
    struct Image app_bad = { "app", 5, bad_code, sizeof bad_code, app.signature };
    int v_bad = verify_image(&app_bad, img_key);
    printf("P5  %-28s = %s / %s\n", "verify_image good / tampered",
           v_ok ? "PASS" : "FAIL", v_bad ? "PASS" : "reject");

    uint32_t fuse_min = 4;
    int rb_up = anti_rollback(5, &fuse_min);                 /* 5 >= 4 -> accept */
    uint32_t floor_after = fuse_min;                         /* capture first    */
    int rb_dn = anti_rollback(3, &fuse_min);                 /* 3 <  5 -> reject */
    printf("P6  %-28s = v5 %s (floor->%u) / v3 %s\n", "anti_rollback up / down",
           rb_up ? "accept" : "reject", floor_after,
           rb_dn ? "accept(BUG)" : "reject");

    /* ---- P7..P9: access control (fail-closed, least priv, mediation) ---- */
    puts("\n== Access control ==");
    printf("P7  %-28s = guest:%s operator:%s vendor:%s\n", "authorize(DEBUG_UNLOCK)",
           authorize(ROLE_GUEST,    OP_DEBUG_UNLOCK) ? "allow" : "deny",
           authorize(ROLE_OPERATOR, OP_DEBUG_UNLOCK) ? "allow" : "deny",
           authorize(ROLE_VENDOR,   OP_DEBUG_UNLOCK) ? "allow" : "deny");
    unsigned updater = CAP_FLASH_WRITE | CAP_NET;            /* least privilege */
    printf("P8  %-28s = flash:%s key:%s\n", "has_capability(OTA task)",
           has_capability(updater, CAP_FLASH_WRITE) ? "yes" : "no",
           has_capability(updater, CAP_KEY_READ)    ? "yes" : "no");
    struct UpdateDescriptor good = { 5, 2048, 1 };
    struct UpdateDescriptor huge = { 5, 999999u, 1 };        /* oversized -> DoS */
    printf("P9  %-28s = good:%s oversized:%s\n", "validate_descriptor",
           validate_descriptor(&good) ? "accept" : "reject",
           validate_descriptor(&huge) ? "accept" : "reject");

    /* ---- P10: the fail-closed OTA state machine ---- */
    puts("\n== Secure OTA update state machine (fail-closed) ==");
    enum OtaState s = OTA_IDLE;
    s = ota_step(s, EV_START);          /* Idle     -> Download */
    s = ota_step(s, EV_CHUNK_DONE);     /* Download -> Verify   */
    s = ota_step(s, EV_VERIFY_OK);      /* Verify   -> Apply    */
    s = ota_step(s, EV_APPLY_OK);       /* Apply    -> Commit   */
    printf("P10 %-28s = %s\n", "clean run reaches", ota_name(s));
    enum OtaState aborted = ota_step(OTA_VERIFY, EV_VERIFY_FAIL);  /* bad image   */
    enum OtaState skipped = ota_step(OTA_IDLE,   EV_APPLY_OK);     /* out of order */
    printf("P10 %-28s = verify-fail:%s skip-order:%s\n", "rejection paths",
           ota_name(aborted), ota_name(skipped));

    /* ---- P11/P12: debug unlock & secret hygiene ---- */
    puts("\n== Debug unlock & secret hygiene ==");
    static const uint8_t token[] = { 0x0B,0xAD,0xC0,0xDE };
    static const uint8_t wrong[] = { 0x0B,0xAD,0xC0,0x00 };
    int dbg_dev   = debug_unlock(LC_DEV,        token, sizeof token, token, sizeof token);
    int dbg_prod  = debug_unlock(LC_PRODUCTION, token, sizeof token, token, sizeof token);
    int dbg_wrong = debug_unlock(LC_DEV,        wrong, sizeof wrong, token, sizeof token);
    printf("P11 %-28s = dev:%s prod:%s wrong:%s\n", "debug_unlock",
           dbg_dev ? "unlock" : "deny", dbg_prod ? "unlock" : "deny",
           dbg_wrong ? "unlock" : "deny");

    uint8_t secret[8] = { 0xDE,0xAD,0xBE,0xEF, 0xFE,0xED,0xFA,0xCE };
    secure_scrub(secret, sizeof secret);
    unsigned acc = 0;
    for (size_t i = 0; i < sizeof secret; i++) acc |= secret[i];
    printf("P12 %-28s = OR-of-bytes=0x%02X (%s)\n", "secure_scrub", acc,
           acc == 0 ? "wiped" : "LEAK");

    return 0;
}
