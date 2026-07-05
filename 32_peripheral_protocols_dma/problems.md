# 32 · Peripheral Protocols & DMA

How an MCU talks to the outside world: **UART / SPI / I2C** serial framing, interrupt-driven ring-buffer FIFOs, CRC link checks and **DMA** descriptor/scatter-gather engines — plus the `volatile`, cache-coherency and IOMMU pitfalls — all as self-contained toy simulations in `solution.c`.

Compile:
```
gcc -std=c11 -Wall -Wextra -O2 solution.c -o solution && ./solution
```

## Must-know (asked constantly)
1. **UART frame format** — idle line HIGH, then START(0), 8 DATA bits sent LSB-first, an optional PARITY bit, then STOP(1); the receiver locks onto the high→low start edge. → uart_frame (P2)
2. **Parity bit** — a 1-bit error check: EVEN parity is the XOR of the data bits, ODD parity its complement; it catches any *odd* number of flipped bits, nothing even. → uart_parity (P1)
3. **UART receive / de-framing** — sample in the middle of each bit-time; a wrong STOP slot is a *framing* error and a wrong parity slot a *parity* error, both reject the byte. → uart_deframe (P3)
4. **SPI modes (CPOL/CPHA)** — the four modes are just `mode = (CPOL<<1)|CPHA`; CPOL sets the idle clock level, CPHA picks which edge *samples*. Master and slave must agree. → spi_mode (P4)
5. **SPI transfer** — full-duplex: MOSI and MISO are one big shift ring, so every clock shifts one bit out and samples one bit in; after 8 clocks the master/slave bytes have swapped. → spi_transfer (P5)
6. **I2C address byte** — the 7-bit address sits in bits 7..1 with the R/W flag in the LSB (0=write, 1=read); it is the first byte after START. → i2c_addr_byte (P6)
7. **I2C ACK/NACK** — the receiver pulls SDA low on the 9th clock to ACK; a NACK (SDA left high) means "not addressed / no more data" and aborts the transfer. → i2c_write_reg (P7)

## Frequently asked
8. **Interrupt-driven RX FIFO** — the RX ISR *puts* bytes and the main loop *gets* them; a single-producer/single-consumer ring buffer decouples the two without dropping bytes at high line rates. → ring_put (P8)
9. **Power-of-two ring mask** — with a power-of-two size you index by `head & (SZ-1)` instead of `%`, and free-running head/tail counters make EMPTY (`head==tail`) vs FULL (`head-tail==SZ`) unambiguous with no lock. → ring_get (P9)
10. **CRC frame check** — polynomial division over GF(2) (e.g. CRC-16-CCITT: poly `0x1021`, init `0xFFFF`); bit *position* matters, so it catches burst errors a byte-sum checksum would miss. → crc16_ccitt (P10)
11. **Polling vs interrupt vs DMA** — polling burns the CPU spinning on a status bit (simple, low-latency, wasteful); interrupts fire per event (good, but ~µs of entry/exit overhead *per byte*); DMA moves whole blocks with *zero* CPU per byte and one IRQ at completion — the choice for bulk or high-rate transfers.
12. **DMA descriptor** — a control block `{src, dst, len, flags, next}` the engine consumes: the CPU builds it, kicks the engine, and gets a completion flag/IRQ while it did other work. → dma_run (P11)
13. **Scatter-gather DMA** — chain descriptors via `next` so one programmed transfer moves many *non-contiguous* regions (e.g. a packet header + payload sitting in different buffers). → dma_run_chain (P12)

## Good to know / tricky
14. **Memory-mapped registers & `volatile`** — peripheral registers alias I/O, so they must be `volatile` or the compiler will cache/reorder/elide the access; set a bit with `r|=1<<n`, clear with `r&=~(1<<n)`, test with `(r>>n)&1`. → reg_write_bit (P13)
15. **Double-buffering / ping-pong** — the DMA fills buffer B while the CPU drains buffer A, then they swap on each completion IRQ; this gives gap-free streaming (audio, ADC, video lines) with no tearing and no CPU copy.
16. **DMA & cache coherency** — on a non-coherent core you must *clean/flush* the cache before a memory→device DMA (so the device sees your writes) and *invalidate* it after a device→memory DMA (so the CPU doesn't read stale cached lines).
17. **DMA buffer alignment** — DMA buffers should be cache-line aligned and padded to a whole number of lines; otherwise an invalidate touching a shared line can corrupt an unrelated neighbouring variable (false sharing).
18. **DMA security / IOMMU** — a bus-master device can read/write *all* physical memory, so a malicious or compromised device (DMA attack over Thunderbolt/PCIe/FireWire) can steal keys or patch the kernel; an IOMMU/SMMU sandboxes each device to only the pages it was granted.
19. **Bit-banging vs hardware peripheral** — you can GPIO bit-bang any protocol in software (flexible, works on any pin, but CPU-heavy and timing-jittery) or use a dedicated UART/SPI/I2C block (fast, offloaded, fixed pinout) — a classic cost/flexibility trade.

## Rapid-fire viva questions
- What is the idle level of a UART line, and which edge marks the start bit?
- With CPOL=1, CPHA=1, on which clock edge is data sampled?
- Why does I2C need pull-up resistors and open-drain outputs?
- Why is `head & (N-1)` only valid when `N` is a power of two?
- How do you distinguish "full" from "empty" without a separate count variable?
- Why is a CRC-16 stronger than an 8-bit byte-sum checksum?
- Name a case where DMA is the *wrong* choice.
- What does `volatile` NOT give you (hint: atomicity, memory barriers)?
- How can a peripheral's DMA corrupt kernel memory, and what hardware stops it?
- Clean vs invalidate — which do you do *before* a device-to-memory transfer?
