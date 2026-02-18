# SD Card Driver Benchmark Results

**Document Purpose**: Record actual performance measurements from benchmark testing
**Test Program**: `src/UTILS/SD_performance_benchmark.spin2` (v2.0)
**Standard Protocol**: 350 MHz and 250 MHz sysclk, both producing exactly 25 MHz SPI clock
**Driver**: Smart Pin SPI + Multi-Sector (CMD18/CMD25) + TX Streamer Fix (commit 797f913)

Detailed per-card benchmark data is in each card's page under [DOCs/cards/](cards/). This document provides cross-card comparisons and analysis.

---

## Test Configuration

- **Hardware**: P2 Edge Module with microSD slot
- **Pins**: CS=P60, MOSI=P59, MISO=P58, SCK=P61
- **Iterations**: 10 per test (averaged)
- **Standard Protocol**: 350 MHz and 250 MHz sysclk both produce exactly 25,000 kHz SPI clock — this isolates Spin2 inter-transfer overhead from SPI bus speed
- **Test Dates**: 2026-01-21 (bit-banged baseline), 2026-02-07 through 2026-02-17 (current driver)

---

## Current Driver Results (350 MHz, 25 MHz SPI)

Test program: `src/UTILS/SD_performance_benchmark.spin2` v2.0
Measurements across three levels: raw single-sector, raw multi-sector (CMD18/CMD25), and file-level (handle API).

### Cross-Card Comparison (350 MHz, Best Numbers)

| Metric | SanDisk Industrial 16GB | Lexar V30 64GB | Lexar Blue 128GB | Samsung EVO 128GB | SanDisk Switch 128GB | SanDisk Extreme 64GB | Samsung PRO Endurance 128GB | Amazon Basics 64GB | PNY 16GB |
|--------|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
| **Mount** | 486 ms | 212 ms | 400 ms | **203 ms** | 233 ms | 233 ms | 243 ms | 233 ms | **202 ms** |
| **File Open** | 159 µs | 991 µs | **128 µs** | 151 µs | 146 µs | 145 µs | 143 µs | 135 µs | 177 µs |
| **Raw Read 1×512B** | 792 KB/s | 1,239 KB/s | 819 KB/s | 937 KB/s | 887 KB/s | 1,005 KB/s | **1,283 KB/s** | 1,245 KB/s | 734 KB/s |
| **Raw Read 64× (32KB)** | 2,393 KB/s | 2,376 KB/s | 2,420 KB/s | 2,353 KB/s | 2,406 KB/s | 2,408 KB/s | **2,427 KB/s** | 2,425 KB/s | 2,376 KB/s |
| **Raw Write 1×512B** | 361 KB/s | 674 KB/s | 680 KB/s | — † | 332 KB/s | 331 KB/s | 617 KB/s | **846 KB/s** | 50 KB/s ‖ |
| **Raw Write 64× (32KB)** | 2,170 KB/s | 2,251 KB/s | 2,275 KB/s | — † | 2,152 KB/s | 2,150 KB/s | **2,319 KB/s** | 2,305 KB/s | 1,037 KB/s |
| **File Read 256KB** | 745 KB/s ‡ | 1,378 KB/s | **1,444 KB/s** | 950 KB/s | 1,016 KB/s | 1,040 KB/s | 1,419 KB/s | 1,386 KB/s | 747 KB/s |
| **File Write 32KB** | 321 KB/s | 433 KB/s | 616 KB/s | 325 KB/s | 378 KB/s | 378 KB/s | 758 KB/s | **774 KB/s** | 192 KB/s |
| **Multi-sector gain** | 67% | 51% | 51% | 63% | 61% | 58% | 46% | 54% | 69% |
| **App Perf Class** | — | A2 ★ | A2 | — | A2 | A2 | A2 | A2 | — |
| **Detail** | [card page](cards/sandisk-sa16g-16gb.md) | [card page](cards/lexar-mssd0-64gb.md) | [card page](cards/lexar-mssd0-128gb.md) | [card page](cards/samsung-gd4qt-128gb.md) | [card page](cards/sandisk-sn128-128gb.md) | [card page](cards/sandisk-sn64g-64gb.md) | [card page](cards/samsung-jd1y7-128gb.md) | [card page](cards/amazon-basics-usd00-64gb.md) | [card page](cards/pny-sd16g-16gb.md) |

**Best Read**: Samsung PRO Endurance 128GB — fastest single-sector and raw multi-sector reads; Lexar Blue 128GB — fastest file-level reads.
**Best Write**: Amazon Basics 64GB — fastest single-sector writes (846 KB/s) and file-level writes (774 KB/s); Samsung PRO Endurance 128GB — fastest raw multi-sector writes.

★ Lexar Red label says A1; ACMD13 register reports A2 (both physical units).
† Samsung EVO 350 MHz raw write results affected by flash controller housekeeping pause (wear leveling/GC). 250 MHz run was clean: Write 1×512B = 394 KB/s, Write 64× = 2,003 KB/s. See [card page](cards/samsung-gd4qt-128gb.md).
‡ SanDisk Industrial 350 MHz File Read 256KB showed high variance (Max=655 ms outlier); 250 MHz result of 790 KB/s is representative.
‖ PNY single-sector write has enormous variance (Min=2,936, Max=14,654 µs) — Phison controller has unpredictable write commit latency.

---

### Sysclk Effect (350 vs 250 MHz at same 25 MHz SPI)

Both speeds produce identical 25 MHz SPI clock — differences are purely Spin2 inter-transfer overhead.

| Test | SanDisk Industrial | Lexar V30 64GB | Lexar Blue 128GB | Samsung EVO 128GB | SanDisk Switch 128GB | SanDisk Extreme 64GB | Samsung PRO Endurance | Amazon Basics 64GB | PNY 16GB |
|------|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
| **Raw Read 64×** | +8% | +9% | +8% | +11% | +8% | +8% | +8% | +9% | +8% |
| **Raw Write 64×** | +9% | +10% | +9% | — | +12% | +9% | +10% | +9% | +4% |
| **File Read 256KB** | -6% ‡ | +16% | +15% | +26% | +11% | +11% | +15% | +15% | +7% |
| **File Write 32KB** | +4% | +9% | +12% | +0.3% | +5% | +5% | +7% | +13% | +4% |

**Pattern**: Raw multi-sector operations gain ~8-11% from faster Spin2 processing between streamer transfers. File-level reads gain 15-26% from reduced FAT traversal overhead. File-level writes are card-dependent — cards with longer flash programming times (Samsung) are dominated by card latency rather than sysclk speed.

‡ SanDisk Industrial 350 MHz had measurement artifacts; 250 MHz results more representative for this card.

---

## Baseline Results (Bit-Banged SPI)

These measurements represent the driver performance **before** Smart Pin optimization.

### Summary Table

| Metric | Gigastone 32GB | PNY 16GB | SanDisk Extreme 64GB |
|--------|----------------|----------|----------------------|
| **Mount** | 138 ms | 139 ms | 152 ms |
| **File Open** | 326 µs | 14,136 µs | **293 µs** |
| **Write 512B** | 85 KB/s | 52 KB/s | **90 KB/s** |
| **Write 4KB** | 210 KB/s | 182 KB/s | **302 KB/s** |
| **Write 32KB** | 325 KB/s | 216 KB/s | **425 KB/s** |
| **Read 256KB** | 1,339 KB/s | 850 KB/s | **1,467 KB/s** |

**Best Overall**: SanDisk Extreme 64GB

---

### Detailed Baseline Card Results

#### Gigastone 32GB (Transcend Silicon, Class 10, U1)

**Card Identification**:
- MID: $74 (Transcend OEM — Gigastone-branded card using Transcend flash/controller)
- Product Name: "00000" (white-label)
- CID: `$74 $49 $54 $30 $30 $30 $30 $30 $1E $62 $4D $D6 $D1 $00 $E7 $E3`

**Note**: This is a different physical card than the Gigastone 32GB in CARD-CATALOG.md (different OID and serial number), but the same model with identical Transcend MID $74 silicon.

**Results**:
| Test | Value | Notes |
|------|-------|-------|
| Mount | 138.5 ms | Includes card init + MBR/VBR parsing |
| File Open | 326 µs | Create new file in root |
| Write 512B (×10) | 85 KB/s | Single sector writes |
| Write 4KB (×10) | 210 KB/s | 8-sector writes |
| Write 32KB (×10) | 325 KB/s | 64-sector writes |
| Read 256KB (×10) | 1,339 KB/s | Sequential read |

---

#### PNY 16GB (Phison Controller)

**Card Identification**:
- MID: $27 (Phison)
- Product Name: "SD16G"
- CID: `$27 $50 $48 $53 $44 $31 $36 $47 $40 $D0 $2C $93 $C2 $01 $4E $83`

**Note**: This is a different physical PNY card (PRV 4.0) than the one used in the current-driver benchmarks below and in CARD-CATALOG.md (PRV 3.0, different serial number). Both are PNY 16GB with identical Phison MID $27 controllers.

**Results**:
| Test | Value | Notes |
|------|-------|-------|
| Mount | 139.2 ms | Similar to Gigastone |
| File Open | **14,136 µs** | **43× slower than Gigastone!** |
| Write 512B (×10) | 52 KB/s | Slowest write speed |
| Write 4KB (×10) | 182 KB/s | |
| Write 32KB (×10) | 216 KB/s | |
| Read 256KB (×10) | 850 KB/s | ~36% slower than Gigastone |

**Notes**: The PNY card shows significantly slower file operations. The 43× slower file open time suggests the card's internal controller has higher latency for metadata operations. This card required a writeSector() fix for busy-wait compatibility (see CARD-CATALOG.md).

---

#### SanDisk Extreme 64GB (SN64G, MID $03)

```
SanDisk SN64G SDXC 59GB [FAT32] SD 6.x rev8.6 SN:$7E65_0771 2022/11
Class 10, U3, A2, V30, SPI 25 MHz  [P2FMTER]
```

**Card Identification**:
- MID: $03 (SanDisk / Western Digital)
- Product Name: "SN64G"
- CID: `$03 $53 $44 $53 $4E $36 $34 $47 $86 $7E $65 $07 $71 $01 $6B $8D`
- Catalog ID: `SanDisk_SN64G_8.6_7E650771_202211`

**Results**:
| Test | Value | Notes |
|------|-------|-------|
| Mount | 152.3 ms | Slightly longer (larger FAT?) |
| File Open | **293 µs** | Fastest |
| Write 512B (×10) | **90 KB/s** | Fastest |
| Write 4KB (×10) | **302 KB/s** | Fastest |
| Write 32KB (×10) | **425 KB/s** | Fastest |
| Read 256KB (×10) | **1,467 KB/s** | Fastest |

**Notes**: Best overall performance. This is a UHS-I card with U3/V30 rating. Formatted as FAT32 by P2 format utility (originally shipped as exFAT).

---

## Legacy Cross-Card Reference (320 MHz, 22.9 MHz SPI)

These results were collected during initial smart-pin driver benchmarking (2026-02-07 through 2026-02-09) at 320 MHz sysclk, before the TX streamer fix. Raw sector results for Samsung were affected by a test seeding issue; file-level results were valid. This table is preserved for its broader card coverage (6 cards) until all cards are re-benchmarked at the standard 350/250 MHz protocol.

| Metric | SanDisk Industrial 16GB | Lexar V30 64GB | Gigastone Camera+ 64GB | Samsung EVO 128GB | Gigastone HE 16GB | PNY 16GB |
|--------|:-:|:-:|:-:|:-:|:-:|:-:|
| **Mount** | 235 ms | 212 ms | 202 ms | 202 ms | 203 ms | 203 ms |
| **File Open** | 144 µs | 198 µs | 199 µs | 153 µs | 186 µs | **16,945 µs** |
| **Raw Read 1×512B** | 698 KB/s | **1,142 KB/s** | 942 KB/s | — | 565 KB/s | 630 KB/s |
| **Raw Read 64× (32KB)** | 2,189 KB/s | **2,173 KB/s** | 1,950 KB/s | — | 1,915 KB/s | 2,035 KB/s |
| **Raw Write 64× (32KB)** | 1,996 KB/s | **2,059 KB/s** | 1,969 KB/s | — | 1,670 KB/s | 995 KB/s |
| **File Read 256KB** | 835 KB/s | **1,274 KB/s** | 1,029 KB/s | 770 KB/s | 631 KB/s | 662 KB/s |
| **File Write 32KB** | 321 KB/s | **501 KB/s** | 367 KB/s | 320 KB/s | 105 KB/s | 173 KB/s |
| **Multi-sector gain** | 65% | 50% | 51% | — | 70% | 67% |
| **Detail** | [card page](cards/sandisk-sa16g-16gb.md) | [card page](cards/lexar-mssd0-64gb.md) | [card page](cards/gigastone-astc-64gb.md) | [card page](cards/samsung-gd4qt-128gb.md) | [card page](cards/gigastone-sd16g-16gb.md) | [card page](cards/pny-sd16g-16gb.md) |

Note: SPI clock at 320 MHz = 22,857 kHz (non-integer divider: 320/25 = 12.8). The 350 MHz standard protocol produces a clean 25,000 kHz SPI clock.

---

## Performance Observations

### Card Controller Variance

The dramatic difference in file open times (128 µs vs 16,945 µs) demonstrates that SD card performance varies significantly based on the internal controller, not just the rated speed class.

### Write Speed Scaling (Baseline → Current)

Write throughput increases with block size due to:
1. Fewer command/response cycles
2. Better alignment with card's internal erase blocks
3. Reduced FAT update overhead

| Block Size | Baseline Best (SanDisk 64GB) | Current Best (Amazon Basics 64GB) | Improvement |
|------------|------------------------------|----------------------------------|-------------|
| 512B | 90 KB/s | 121 KB/s | +34% |
| 4KB | 302 KB/s | 483 KB/s | +60% |
| 32KB | 425 KB/s | 774 KB/s | +82% |

### Read Performance

Sequential read performance at file level (current driver, 350 MHz):
- Lexar Blue 128GB: **1,444 KB/s** (best)
- Samsung PRO Endurance 128GB: 1,419 KB/s
- Amazon Basics 64GB: 1,386 KB/s
- Lexar V30 64GB: 1,378 KB/s
- SanDisk Extreme 64GB: 1,040 KB/s
- SanDisk Switch 128GB: 1,016 KB/s
- Samsung EVO 128GB: 950 KB/s
- PNY 16GB: 747 KB/s
- SanDisk Industrial 16GB: 745 KB/s (measurement artifact — 790 KB/s at 250 MHz is representative)

### Multi-Sector Improvement

CMD18/CMD25 multi-sector operations provide 46-69% improvement over repeated single-sector commands:
- Cards with slower internal controllers benefit more (PNY: 69%, SanDisk Industrial: 67%, Samsung EVO: 63%)
- Fast cards benefit less (Samsung PRO Endurance: 46%, Lexar V30: 51%, Lexar Blue: 51%, Amazon Basics: 54%)
- The improvement comes from eliminating per-sector command overhead

### Mount Time Observations

The Samsung EVO Select 128GB holds the fastest mount time (203 ms) despite being the largest card without A-class certification. Possible factors:

- **Simpler command class set**: CCC=$5B5 (missing Classes 1, 11) vs $DB7 on A-rated cards — fewer internal subsystems to initialize
- **SD 3.x vs SD 6.x**: Older spec cards may skip initialization of A2/V30 performance guarantee infrastructure (internal queuing, IOPS state machines)
- **ACMD41 convergence**: Mount time is dominated by the power-up initialization loop; Samsung's controller may reach ready status in fewer iterations
- **Generational contrast**: Both Samsung cards bracket the field — EVO (SD 3.x, no A-class) = 203 ms, PRO Endurance (SD 6.x, A2) = 243 ms. Same silicon vendor, 20% difference

Without instrumented mount breakdown (ACMD41 loop vs filesystem reads), the leading hypothesis is that older controllers with fewer performance guarantees to set up simply reach ready state faster.

### PNY Phison Controller Anomalies (350 MHz data)

The PNY card (MID $27 Phison) has distinctive behavior:
- **File open: 177 µs avg** on empty directory (clean format) — previously 16.9 ms with many files, showing directory scan dominates open time
- **Raw single-sector write: 50 KB/s** with enormous variance (Min=2,936, Max=14,654 µs) — slowest writer in collection
- **Multi-sector reads competitive**: 2,376 KB/s at 64 sectors — matching or exceeding many premium cards
- **Extremely consistent reads**: Min≈Max for single-sector (697 µs) at 350 MHz — zero variance
- **Multi-sector writes slow**: 1,037 KB/s at 64 sectors — roughly half of other cards

---

## Theoretical Limits

At 350 MHz sysclk with Smart Pin SPI:
- **SPI clock**: 25.0 MHz (exact — 350/25 = 14, integer divider)
- **Theoretical byte rate**: ~3,052 KB/s (25 MHz / 8 bits)
- **Best raw achieved**: 2,427 KB/s read, 2,319 KB/s write (80% / 76% efficiency)
- **Best file-level achieved**: 1,444 KB/s read, 774 KB/s write

The raw-to-theoretical gap is due to:
- Command/response framing per transfer
- CRC computation and checking
- Token wait time (especially on writes)

The file-to-raw gap is due to:
- FAT chain traversal during reads
- FAT + directory updates during writes
- Cluster boundary management
- Handle API overhead

---

## Efficiency Summary

| Level | Read (KB/s) | Write (KB/s) | Read % | Write % |
|-------|-------------|--------------|--------|---------|
| Theoretical (25 MHz SPI) | 3,052 | 3,052 | 100% | 100% |
| Raw multi-sector (best) | 2,427 | 2,319 | 80% | 76% |
| File-level (best) | 1,444 | 774 | 47% | 25% |

The raw SPI layer is reasonably efficient (~78% average). File-level write throughput is best with Amazon Basics 64GB (774 KB/s, 25% of theoretical), which edges out Samsung PRO Endurance (758 KB/s). FAT metadata updates still consume most of available bandwidth.

---

## Test History

| Date | Driver Version | Change | Notes |
|------|----------------|--------|-------|
| 2026-01-21 | Baseline (bit-banged) | Initial benchmark | 3 cards tested |
| 2026-02-07 | Smart pin + multi-sector | Benchmark v2.0 | SanDisk Industrial 16GB @ 320+270 MHz |
| 2026-02-08 | Smart pin + multi-sector | Benchmark v2.0 | Lexar V30 U3 64GB @ 320+270+350 MHz |
| 2026-02-08 | Smart pin + multi-sector | Benchmark v2.0 | Gigastone Camera Plus 64GB @ 320+270 MHz |
| 2026-02-08 | Smart pin + multi-sector | Benchmark v2.0 | Samsung EVO Select 128GB @ 320+270 MHz (raw sector errors) |
| 2026-02-08 | Smart pin + multi-sector | Benchmark v2.0 | Gigastone High Endurance 16GB @ 320+270 MHz |
| 2026-02-09 | Smart pin + multi-sector | Benchmark v2.0 | PNY 16GB @ 320+270 MHz |
| 2026-02-16 | TX streamer fix (797f913) | Standard protocol | SanDisk Industrial 16GB @ 350+250 MHz |
| 2026-02-16 | TX streamer fix (797f913) | Standard protocol | Lexar Blue 128GB @ 350+250 MHz |
| 2026-02-17 | TX streamer fix (797f913) | Standard protocol | Samsung EVO Select 128GB @ 350+250 MHz |
| 2026-02-17 | TX streamer fix (797f913) | Standard protocol | Lexar V30 U3 64GB @ 350+250 MHz |
| 2026-02-17 | TX streamer fix (797f913) | Standard protocol | SanDisk Nintendo Switch 128GB @ 350+250 MHz |
| 2026-02-17 | TX streamer fix (797f913) | Standard protocol | SanDisk Extreme 64GB @ 350+250 MHz |
| 2026-02-17 | TX streamer fix (797f913) | Standard protocol | Samsung PRO Endurance 128GB @ 350+250 MHz |
| 2026-02-17 | TX streamer fix (797f913) | Standard protocol | Amazon Basics 64GB @ 350+250 MHz |
| 2026-02-17 | TX streamer fix (797f913) | Re-benchmark | SanDisk Extreme 64GB @ 350+250 MHz (re-run) |
| 2026-02-17 | TX streamer fix (797f913) | Re-benchmark | PNY 16GB @ 350+250 MHz (re-run) |

---

*Document maintained as part of P2-uSD-Study project*
*Last updated: 2026-02-17*
