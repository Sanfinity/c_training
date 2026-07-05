/*
 * Topic 33: Firmware Building Blocks
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * Each problem is a standalone function; main() demonstrates all of them.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ---- shared model: a tiny button/LED controller ----------------------------
 * States and events are file-scope enums shared by the switch FSM (P1) and the
 * table-driven FSM (P2). In real firmware these would live in a header. */
typedef enum { S_OFF, S_ON, S_BLINK, NSTATES } led_state_t;
typedef enum { E_PRESS, E_LONG, NEVENTS } led_event_t;

static const char *STATE_NAME[] = { "OFF", "ON", "BLINK" };

/* P1: switch-based FSM step -- next state from (state, event).
 * The control logic lives in code: readable for a few states, but it grows with
 * every state/event and is easy to get subtly wrong as the machine expands. */
led_state_t fsm_step_switch(led_state_t s, led_event_t e) {
    switch (s) {
        case S_OFF:   return (e == E_PRESS) ? S_ON  : S_BLINK;
        case S_ON:    return (e == E_PRESS) ? S_OFF : S_BLINK;
        case S_BLINK: return S_OFF;            /* any event returns to OFF */
        default:      return S_OFF;            /* defensive: unknown state */
    }
}

/* P2: table-driven FSM step -- identical behaviour expressed as pure DATA.
 * A [state][event] transition table separates the machine's SHAPE from code:
 * compact, auditable, and extended by editing one row. The engine is a lookup. */
led_state_t fsm_step_table(led_state_t s, led_event_t e) {
    static const uint8_t table[NSTATES][NEVENTS] = {
        /*            E_PRESS   E_LONG  */
        [S_OFF]   = { S_ON,     S_BLINK },
        [S_ON]    = { S_OFF,    S_BLINK },
        [S_BLINK] = { S_OFF,    S_OFF   },
    };
    if (s >= NSTATES || e >= NEVENTS) return S_OFF;   /* bounds guard */
    return (led_state_t)table[s][e];
}

/* ---- ring buffer / circular FIFO (the firmware workhorse) -------------------
 * Fixed storage plus head/tail indices and a 'full' flag to tell a full buffer
 * (head == tail) apart from an empty one (also head == tail). */
#define RB_SIZE 8
typedef struct {
    uint8_t buf[RB_SIZE];
    uint8_t head;      /* next write slot */
    uint8_t tail;      /* next read slot  */
    bool    full;
} RingBuffer;

static inline bool rb_empty(const RingBuffer *rb) {
    return !rb->full && (rb->head == rb->tail);
}
static inline bool rb_full(const RingBuffer *rb) {
    return rb->full;
}

/* P3: push one byte; refuse (return false) instead of overwriting when full. */
bool rb_put(RingBuffer *rb, uint8_t v) {
    if (rb->full) return false;
    rb->buf[rb->head] = v;
    rb->head = (uint8_t)((rb->head + 1) % RB_SIZE);
    rb->full = (rb->head == rb->tail);         /* head just caught tail -> full */
    return true;
}

/* P4: pop one byte in FIFO order; return false when empty. A successful get
 * always clears 'full' because it just opened a slot. */
bool rb_get(RingBuffer *rb, uint8_t *out) {
    if (rb_empty(rb)) return false;
    *out = rb->buf[rb->tail];
    rb->tail = (uint8_t)((rb->tail + 1) % RB_SIZE);
    rb->full = false;
    return true;
}

/* ---- cooperative software timer / scheduler --------------------------------
 * A table of periodic tasks; each has a reload 'period' and a live 'countdown'.
 * A hardware-timer ISR would call tick() at a fixed rate to drive it. */
typedef struct {
    uint16_t period;      /* reload value, in ticks */
    uint16_t countdown;   /* ticks remaining until due */
    uint32_t runs;        /* how many times it has fired */
} Task;

/* P5: advance every task by one tick; fire + reload those that reach zero.
 * Returns how many tasks became due on THIS tick. */
unsigned sched_tick(Task *tasks, size_t n) {
    unsigned fired = 0;
    for (size_t i = 0; i < n; i++) {
        if (tasks[i].countdown > 0) tasks[i].countdown--;
        if (tasks[i].countdown == 0) {
            tasks[i].runs++;
            tasks[i].countdown = tasks[i].period;   /* reload */
            fired++;
        }
    }
    return fired;
}

/* ---- button debounce (shift-register) --------------------------------------
 * Each sample shifts into an 8-bit history; the stable level only flips after
 * 8 consecutive equal samples (0xFF -> pressed, 0x00 -> released), so contact
 * bounce in between is ignored. */
typedef struct {
    uint8_t history;
    bool    level;      /* debounced, stable level */
} Debounce;

/* P6: feed one raw sample, return the current debounced level. */
bool debounce_update(Debounce *d, bool raw) {
    d->history = (uint8_t)((d->history << 1) | (raw ? 1u : 0u));
    if (d->history == 0xFF)      d->level = true;
    else if (d->history == 0x00) d->level = false;
    /* otherwise still bouncing -> keep the previous stable level */
    return d->level;
}

/* P7: CRC-8 (poly 0x07, init 0x00) for stored config / firmware integrity.
 * Bitwise, so it needs no lookup table -- ideal for tiny ROM budgets. */
uint8_t crc8(const uint8_t *data, size_t len) {
    uint8_t crc = 0x00;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int b = 0; b < 8; b++)
            crc = (crc & 0x80) ? (uint8_t)((crc << 1) ^ 0x07)
                               : (uint8_t)(crc << 1);
    }
    return crc;
}

/* P8: CRC-16-CCITT (poly 0x1021, init 0xFFFF, no reflection). Stronger check
 * for larger images; "123456789" -> 0x29B1 is the classic test vector. */
uint16_t crc16_ccitt(const uint8_t *data, size_t len) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; i++) {
        crc ^= (uint16_t)((uint16_t)data[i] << 8);
        for (int b = 0; b < 8; b++)
            crc = (crc & 0x8000) ? (uint16_t)((crc << 1) ^ 0x1021)
                                 : (uint16_t)(crc << 1);
    }
    return crc;
}

/* ---- fixed-point Q16.16 arithmetic (no FPU required) ------------------------
 * Store reals as integers scaled by 2^16: the top 16 bits are the whole part,
 * the low 16 bits the fraction. */
typedef int32_t q16_t;
#define Q_SHIFT 16
#define Q_ONE   ((int32_t)1 << Q_SHIFT)   /* 65536 == 1.0 */

/* P9: convert a real number into Q16.16 (round to nearest). */
q16_t to_fixed(double x) {
    return (q16_t)(x * (double)Q_ONE + (x >= 0 ? 0.5 : -0.5));
}

/* P10: convert a Q16.16 value back to a real number. */
double from_fixed(q16_t v) {
    return (double)v / (double)Q_ONE;
}

/* P11: multiply two Q16.16 values. The raw product is Q32.32, so widen to
 * 64-bit and shift right by Q_SHIFT to renormalise back to Q16.16. */
q16_t fx_mul(q16_t a, q16_t b) {
    return (q16_t)(((int64_t)a * (int64_t)b) >> Q_SHIFT);
}

/* ---- RTOS-style event group (bit flags) ------------------------------------ */
typedef uint32_t evt_group_t;
#define EVT_RX  (1u << 0)
#define EVT_TX  (1u << 1)
#define EVT_ERR (1u << 2)

static inline evt_group_t evt_clear(evt_group_t *grp, uint32_t mask) {
    *grp &= ~mask;
    return *grp;
}

/* P12: set (OR-in) one or more event bits; returns the new group value. */
evt_group_t evt_set(evt_group_t *grp, uint32_t mask) {
    *grp |= mask;
    return *grp;
}

/* P13: wait-any -- true if ANY bit in 'mask' is currently set in the group. */
bool evt_wait_any(evt_group_t grp, uint32_t mask) {
    return (grp & mask) != 0u;
}

int main(void) {
    /* P1 + P2: the two FSMs must agree on every transition */
    led_state_t s1 = fsm_step_switch(S_OFF, E_PRESS);
    led_state_t s2 = fsm_step_table(S_OFF, E_PRESS);
    printf("P1  fsm_step_switch = OFF +PRESS -> %s\n", STATE_NAME[s1]);
    printf("P2  fsm_step_table  = OFF +PRESS -> %s (agrees with switch: %s)\n",
           STATE_NAME[s2], (s1 == s2) ? "yes" : "no");

    /* P3 + P4: ring buffer FIFO */
    RingBuffer rb = {0};
    bool put_ok = true;
    for (uint8_t i = 0; i < 5; i++)
        put_ok = rb_put(&rb, (uint8_t)(10 + i)) && put_ok;
    printf("P3  rb_put          = pushed 5 bytes ok=%s full=%d\n",
           put_ok ? "yes" : "no", rb_full(&rb));
    uint8_t a = 0, b = 0;
    bool g1 = rb_get(&rb, &a);      /* out-params read in SEPARATE statements */
    bool g2 = rb_get(&rb, &b);
    printf("P4  rb_get          = g1=%d(%u) g2=%d(%u) FIFO order, empty=%d\n",
           g1, (unsigned)a, g2, (unsigned)b, rb_empty(&rb));

    /* P5: run the scheduler for 6 ticks (task periods 2, 3, 5) */
    Task tasks[] = { {2, 2, 0}, {3, 3, 0}, {5, 5, 0} };
    size_t nt = sizeof tasks / sizeof tasks[0];
    unsigned total_fired = 0;
    for (int t = 0; t < 6; t++) total_fired += sched_tick(tasks, nt);
    printf("P5  sched_tick      = after 6 ticks runs A=%u B=%u C=%u (fired=%u)\n",
           tasks[0].runs, tasks[1].runs, tasks[2].runs, total_fired);

    /* P6: debounce a bouncy rising input */
    Debounce db = {0};
    const uint8_t samples[] = {1,0,1,0,1,1,1,1,1,1,1,1};
    size_t nsamp = sizeof samples / sizeof samples[0];
    bool prev = false;
    int edge_at = -1;
    for (size_t i = 0; i < nsamp; i++) {
        bool lvl = debounce_update(&db, samples[i] != 0);
        if (lvl && !prev) edge_at = (int)i;
        prev = lvl;
    }
    printf("P6  debounce_update = clean rising edge at sample %d, level=%d\n",
           edge_at, prev);

    /* P7 + P8: config integrity, and catching a single-bit flip */
    uint8_t config[] = {0x01, 0x02, 0x03, 0x04, 0xAA, 0x55};
    size_t clen = sizeof config;
    uint8_t  c8  = crc8(config, clen);
    uint16_t c16 = crc16_ccitt(config, clen);
    printf("P7  crc8            = 0x%02X\n", (unsigned)c8);
    printf("P8  crc16_ccitt     = 0x%04X\n", (unsigned)c16);
    config[2] ^= 0x01;                          /* corrupt one bit */
    uint8_t c8_bad = crc8(config, clen);
    printf("P7  crc8 (flipped)  = 0x%02X (differs -> corruption caught)\n",
           (unsigned)c8_bad);

    /* P9 + P10 + P11: fixed-point 1.5 * 2.5 = 3.75 without an FPU */
    q16_t fa = to_fixed(1.5);
    q16_t fb = to_fixed(2.5);
    q16_t fp = fx_mul(fa, fb);
    printf("P9  to_fixed        = 1.5 -> %ld raw (= %.4f)\n",
           (long)fa, from_fixed(fa));
    printf("P10 from_fixed      = raw %ld -> %.4f\n", (long)fp, from_fixed(fp));
    printf("P11 fx_mul          = 1.5 * 2.5 = %ld raw (= %.4f)\n",
           (long)fp, from_fixed(fp));

    /* P12 + P13: event group set / wait-any / clear */
    evt_group_t events = 0;
    evt_set(&events, EVT_RX);
    evt_set(&events, EVT_ERR);
    printf("P12 evt_set         = flags=0x%02X (RX|ERR)\n", (unsigned)events);
    bool any_io  = evt_wait_any(events, EVT_RX | EVT_TX);
    bool has_err = evt_wait_any(events, EVT_ERR);
    evt_clear(&events, EVT_ERR);
    bool err_gone = !evt_wait_any(events, EVT_ERR);
    printf("P13 evt_wait_any    = anyIO=%d hasERR=%d; after clearing ERR gone=%d\n",
           any_io, has_err, err_gone);

    return 0;
}
