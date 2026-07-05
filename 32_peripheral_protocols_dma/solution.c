/*
 * Topic 32: Peripheral Protocols & DMA
 * Compile: gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
 *
 * Toy, self-contained simulations of UART / SPI / I2C framing, an ISR-style ring
 * buffer, a CRC link check and DMA descriptors -- no real hardware registers, no
 * inline asm; "registers" are just plain struct/array variables.
 * Each problem is a standalone function; main() demonstrates all of them.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* P1: UART parity bit (even / odd).
 * A single-bit link check: EVEN parity is the XOR of all data bits (so the total
 * number of 1s, data+parity, is even); ODD parity is its complement. It catches
 * any odd number of flipped bits. */
int uart_parity(uint8_t byte, bool odd) {
    int p = 0;
    for (int i = 0; i < 8; i++) p ^= (byte >> i) & 1;   /* even parity = XOR of bits */
    return odd ? (p ^ 1) : p;                           /* odd parity flips it       */
}

/* P2: frame a byte into a UART bit stream.
 * The idle line sits HIGH; a frame is: START(0), 8 DATA bits LSB-first, a PARITY
 * bit, then STOP(1). The receiver locks onto the high->low START edge. Bits are
 * written one-per-slot into bits[] (11 slots for 8N... err, 8-data + parity). */
size_t uart_frame(uint8_t byte, bool odd, uint8_t *bits) {
    size_t n = 0;
    bits[n++] = 0;                                      /* START: line pulled low     */
    for (int i = 0; i < 8; i++) bits[n++] = (byte >> i) & 1;   /* 8 DATA bits, LSB first */
    bits[n++] = (uint8_t)uart_parity(byte, odd);        /* PARITY bit                 */
    bits[n++] = 1;                                      /* STOP: line returns high    */
    return n;                                           /* 11 bits total              */
}

/* P3: de-frame a UART bit stream back into a byte.
 * Validate the START(0) and STOP(1) slots (a bad STOP is a FRAMING error) and the
 * PARITY bit (mismatch is a PARITY error). Only then hand back the data byte. */
bool uart_deframe(const uint8_t *bits, size_t n, bool odd, uint8_t *out) {
    if (n != 11u) return false;
    if (bits[0] != 0 || bits[10] != 1) return false;    /* framing error */
    uint8_t b = 0;
    for (int i = 0; i < 8; i++) b |= (uint8_t)((bits[1 + i] & 1) << i);
    if (bits[9] != (uint8_t)uart_parity(b, odd)) return false;   /* parity error */
    *out = b;
    return true;
}

/* P4: SPI mode from CPOL / CPHA.
 * The four SPI modes are simply mode = (CPOL << 1) | CPHA. CPOL sets the idle
 * clock level; CPHA selects which clock edge SAMPLES the data (0 = first/leading
 * edge, 1 = second/trailing edge). Master and slave MUST agree on the mode. */
int spi_mode(bool cpol, bool cpha) {
    return (cpol ? 2 : 0) | (cpha ? 1 : 0);
}

/* P5: bit-banged full-duplex SPI transfer (mode-0 style, MSB first).
 * SPI is a shift ring: on every clock the master presents one MOSI bit and the
 * slave presents one MISO bit, and BOTH shift the sampled bit in. After 8 clocks
 * the two 8-bit shift registers have swapped contents -- the master receives what
 * the slave held, and the slave now holds what the master sent. */
uint8_t spi_transfer(uint8_t mosi, uint8_t *slave_reg) {
    uint8_t sreg = *slave_reg;                          /* slave's shift register       */
    uint8_t miso = 0;                                   /* master samples this from MISO */
    for (int i = 0; i < 8; i++) {                       /* MSB first, one bit per clock  */
        uint8_t out_bit = (uint8_t)((mosi >> 7) & 1u);  /* master drives MOSI */
        uint8_t in_bit  = (uint8_t)((sreg >> 7) & 1u);  /* slave  drives MISO */
        miso = (uint8_t)((miso << 1) | in_bit);         /* master shifts in   */
        sreg = (uint8_t)((sreg << 1) | out_bit);        /* slave  shifts in   */
        mosi = (uint8_t)(mosi << 1);
    }
    *slave_reg = sreg;                                  /* slave now holds master's byte */
    return miso;                                        /* master got the slave's byte   */
}

/* P6: assemble an I2C address byte.
 * The 7-bit slave address occupies bits 7..1; the LSB is the R/W flag
 * (0 = write, 1 = read). This single byte is what the master clocks out right
 * after the START condition. */
uint8_t i2c_addr_byte(uint8_t addr7, bool read) {
    return (uint8_t)((addr7 << 1) | (read ? 1u : 0u));
}

/* P7: toy I2C register write with ACK / NACK.
 * After the address byte the addressed slave pulls SDA low on the 9th clock to
 * ACK; anything else is a NACK and the transfer aborts. Here the "slave" ACKs
 * (stores the byte) only when the address matches, the R/W bit says WRITE, and
 * the register index is in range; otherwise it NACKs (returns false). */
typedef struct { uint8_t addr7; uint8_t reg[16]; } I2CSlave;

bool i2c_write_reg(I2CSlave *slv, uint8_t addr_byte, uint8_t index, uint8_t value) {
    if ((uint8_t)(addr_byte >> 1) != slv->addr7) return false;  /* NACK: not addressed  */
    if (addr_byte & 1u)                          return false;  /* NACK: R bit on write */
    if (index >= 16)                             return false;  /* NACK: bad register   */
    slv->reg[index] = value;                                    /* ACK + store          */
    return true;                                                /* ACK                  */
}

/* P8/P9: interrupt-driven RX FIFO (single-producer/single-consumer ring buffer).
 * The RX ISR PUTs bytes, the main loop GETs them. Free-running head/tail counters
 * plus a power-of-two size let us index with a cheap mask (head & (SZ-1)) instead
 * of '%', and tell EMPTY (head==tail) from FULL (head-tail == SZ) with no wasted
 * slot and no shared count that both sides would have to lock. */
#define RING_SZ 8u                                       /* MUST be a power of two */
typedef struct { uint8_t buf[RING_SZ]; uint16_t head, tail; } Ring;

/* P8: enqueue one byte; false if the FIFO is full. */
bool ring_put(Ring *r, uint8_t v) {
    if ((unsigned)(uint16_t)(r->head - r->tail) == RING_SZ) return false;  /* full */
    r->buf[r->head & (RING_SZ - 1u)] = v;                /* power-of-two mask */
    r->head++;
    return true;
}

/* P9: dequeue one byte; false if the FIFO is empty. */
bool ring_get(Ring *r, uint8_t *out) {
    if (r->head == r->tail) return false;                /* empty */
    *out = r->buf[r->tail & (RING_SZ - 1u)];
    r->tail++;
    return true;
}

/* P10: CRC-16-CCITT link/frame check (poly 0x1021, init 0xFFFF, no reflection).
 * A CRC is polynomial division over GF(2): far stronger than a byte-sum checksum
 * because bit position matters, so it catches bursts and reordering a sum would
 * miss. Appended to a frame; the receiver recomputes and compares. */
uint16_t crc16_ccitt(const uint8_t *data, size_t len) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; i++) {
        crc ^= (uint16_t)((uint16_t)data[i] << 8);
        for (int b = 0; b < 8; b++) {
            if (crc & 0x8000) crc = (uint16_t)((crc << 1) ^ 0x1021);
            else              crc = (uint16_t)(crc << 1);
        }
    }
    return crc;
}

/* P11/P12: DMA descriptor + scatter-gather chain.
 * A DMA descriptor is a control block the engine consumes: {src, dst, len,
 * flags, next}. The CPU builds it, kicks the engine, and the engine moves the
 * block with NO per-byte CPU cost, raising a completion flag/IRQ when done. */
#define DMA_DONE 0x1u                                    /* completion status bit (toy) */
typedef struct DmaDesc {
    const void     *src;
    void           *dst;
    size_t          len;
    uint32_t        flags;
    struct DmaDesc *next;
} DmaDesc;

/* P11: "execute" one descriptor -- move len bytes and flag completion. */
size_t dma_run(DmaDesc *d) {
    memcpy(d->dst, d->src, d->len);                      /* the engine copies the block */
    d->flags |= DMA_DONE;                                /* set completion status       */
    return d->len;                                       /* bytes transferred           */
}

/* P12: scatter-gather -- walk a linked descriptor chain, moving each region. */
size_t dma_run_chain(DmaDesc *head) {
    size_t total = 0;
    for (DmaDesc *d = head; d != NULL; d = d->next)      /* follow ->next */
        total += dma_run(d);
    return total;
}

/* P13: memory-mapped control-register bit ops (set / clear one bit).
 * Real peripheral registers are 'volatile' so the compiler never caches,
 * reorders or elides the access. Read-modify-write with a mask keeps every OTHER
 * bit intact: SET is r |= (1<<n), CLEAR is r &= ~(1<<n) (note the ~), and TEST is
 * just (r >> n) & 1 (shown inline in main). */
void reg_write_bit(volatile uint32_t *reg, unsigned bit, bool set) {
    if (set) *reg |=  (1u << bit);                       /* OR-mask sets one bit           */
    else     *reg &= ~(1u << bit);                       /* AND-with-inverse clears one bit */
}

int main(void) {
    /* P1: even / odd parity of a byte */
    uint8_t ch = 'A';                                    /* 0x41 = 0100_0001, two 1-bits */
    int pe = uart_parity(ch, false);
    int po = uart_parity(ch, true);
    printf("P1  uart_parity     = 'A'=0x%02X  even=%d odd=%d\n", (unsigned)ch, pe, po);

    /* P2 / P3: frame a byte, then de-frame it back */
    uint8_t bits[11] = {0};
    size_t nb = uart_frame(ch, false, bits);
    printf("P2  uart_frame      = %zu bits: start=%d data=%d%d%d%d%d%d%d%d par=%d stop=%d\n",
           nb, bits[0], bits[8], bits[7], bits[6], bits[5], bits[4], bits[3], bits[2],
           bits[1], bits[9], bits[10]);
    uint8_t got = 0;
    bool ok3 = uart_deframe(bits, nb, false, &got);
    printf("P3  uart_deframe    = ok=%s byte=0x%02X ('%c')\n",
           ok3 ? "yes" : "no", (unsigned)got, got);

    /* P4: the four SPI modes come from CPOL / CPHA */
    printf("P4  spi_mode        = (0,0)->%d (0,1)->%d (1,0)->%d (1,1)->%d\n",
           spi_mode(false, false), spi_mode(false, true),
           spi_mode(true, false),  spi_mode(true, true));

    /* P5: bit-banged full-duplex exchange with a toy slave shift register */
    uint8_t slave = 0xA5;                                /* byte the slave clocks out */
    uint8_t master_tx = 0x3C;
    uint8_t master_rx = spi_transfer(master_tx, &slave);
    printf("P5  spi_transfer    = master sent 0x%02X, got 0x%02X; slave now 0x%02X\n",
           (unsigned)master_tx, (unsigned)master_rx, (unsigned)slave);

    /* P6 / P7: build an address byte, then write a register on a toy slave */
    I2CSlave dev = { .addr7 = 0x3C };
    uint8_t wa = i2c_addr_byte(0x3C, false);             /* write to 0x3C */
    uint8_t ra = i2c_addr_byte(0x3C, true);              /* read  from 0x3C */
    bool ack_ok  = i2c_write_reg(&dev, wa, 2, 0x7E);                          /* ACK  */
    bool ack_bad = i2c_write_reg(&dev, i2c_addr_byte(0x22, false), 2, 0x11);  /* NACK */
    printf("P6  i2c_addr_byte   = write=0x%02X read=0x%02X\n", (unsigned)wa, (unsigned)ra);
    printf("P7  i2c_write_reg   = addressed=%s (reg[2]=0x%02X)  wrong-addr=%s\n",
           ack_ok ? "ACK" : "NACK", (unsigned)dev.reg[2], ack_bad ? "ACK" : "NACK");

    /* P8 / P9: interrupt-style FIFO (power-of-two ring buffer) */
    Ring rb = { .head = 0, .tail = 0 };
    int pushed = 0;
    for (uint8_t v = 1; v <= 10; v++) if (ring_put(&rb, v)) pushed++;   /* 8 fit, 2 dropped */
    uint8_t a = 0, b = 0;
    ring_get(&rb, &a);
    ring_get(&rb, &b);
    unsigned remaining = (unsigned)(uint16_t)(rb.head - rb.tail);
    printf("P8  ring_put        = pushed %d of 10 into a cap-%u FIFO (extra dropped)\n",
           pushed, (unsigned)RING_SZ);
    printf("P9  ring_get        = FIFO order out=%u,%u  remaining=%u\n",
           (unsigned)a, (unsigned)b, remaining);

    /* P10: CRC-16-CCITT frame check */
    const uint8_t msg[] = { 'D', 'M', 'A' };
    uint16_t crc = crc16_ccitt(msg, sizeof msg);
    printf("P10 crc16_ccitt     = crc(\"DMA\") = 0x%04X\n", (unsigned)crc);

    /* P11: execute one DMA descriptor (block move + completion flag) */
    uint8_t srcA[4] = { 0x11, 0x22, 0x33, 0x44 }, dstA[4] = {0};
    DmaDesc d0 = { .src = srcA, .dst = dstA, .len = sizeof srcA, .flags = 0, .next = NULL };
    size_t moved0 = dma_run(&d0);
    printf("P11 dma_run         = moved %zu bytes, done=%u, dst=%02X %02X %02X %02X\n",
           moved0, (unsigned)(d0.flags & DMA_DONE),
           (unsigned)dstA[0], (unsigned)dstA[1], (unsigned)dstA[2], (unsigned)dstA[3]);

    /* P12: scatter-gather DMA over a chain of descriptors */
    uint8_t s1[2] = { 0xAA, 0xBB }, s2[3] = { 0xC1, 0xC2, 0xC3 }, s3[1] = { 0xFF };
    uint8_t o1[2] = {0}, o2[3] = {0}, o3[1] = {0};
    DmaDesc c3 = { .src = s3, .dst = o3, .len = 1, .flags = 0, .next = NULL };
    DmaDesc c2 = { .src = s2, .dst = o2, .len = 3, .flags = 0, .next = &c3 };
    DmaDesc c1 = { .src = s1, .dst = o1, .len = 2, .flags = 0, .next = &c2 };
    size_t movedC = dma_run_chain(&c1);
    printf("P12 dma_run_chain   = scatter-gather moved %zu bytes across 3 descriptors\n",
           movedC);

    /* P13: set / clear / test bits in a (simulated) control register */
    volatile uint32_t ctrl = 0;
    reg_write_bit(&ctrl, 3, true);                       /* enable bit 3 */
    reg_write_bit(&ctrl, 7, true);                       /* enable bit 7 */
    reg_write_bit(&ctrl, 3, false);                      /* disable bit 3 again */
    unsigned bit3 = (ctrl >> 3) & 1u;                    /* TEST a status bit */
    unsigned bit7 = (ctrl >> 7) & 1u;
    printf("P13 reg_write_bit   = ctrl=0x%08X  bit3=%u bit7=%u\n",
           (unsigned)ctrl, bit3, bit7);

    return 0;
}
