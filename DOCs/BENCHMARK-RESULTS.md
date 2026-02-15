# SD Card Driver Benchmark Results

**Document Purpose**: Record actual performance measurements from benchmark testing
**Test Program**: `src/UTILS/SD_performance_benchmark.spin2` (v2.0)
**System Clock**: 320 MHz (default), 270 MHz (comparison runs)

Detailed per-card benchmark data is in each card's page under [DOCs/cards/](cards/). This document provides cross-card comparisons and analysis.

---

## Test Configuration

- **Hardware**: P2 Edge Module with microSD slot
- **Pins**: CS=P60, MOSI=P59, MISO=P58, SCK=P61
- **Iterations**: 10 per test (averaged)
- **Test Date**: 2026-01-21 (baseline), 2026-02-07 through 2026-02-09 (current driver)

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
SanDisk SN64G SDXC 59GB [FAT32] SD 6.x rev8.6 SN:7E650771 2022/11
Class 10, U3, V30, SPI 25 MHz  [formatted by P2FMTER]
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

## Current Driver Results (Smart Pin SPI + Multi-Sector)

Test program: `src/UTILS/SD_performance_benchmark.spin2` v2.0
Measurements across three levels: raw single-sector, raw multi-sector (CMD18/CMD25), and file-level (handle API).

### Cross-Card Comparison (320 MHz, Best Numbers)

| Metric | SanDisk Industrial 16GB | Lexar V30 64GB | Gigastone Camera+ 64GB | Samsung EVO 128GB | Gigastone HE 16GB | PNY 16GB |
|--------|:-:|:-:|:-:|:-:|:-:|:-:|
| **Mount** | 235 ms | 212 ms | 202 ms | 202 ms | 203 ms | 203 ms |
| **File Open** | 144 µs | 198 µs | 199 µs | 153 µs | 186 µs | **16,945 µs** |
| **Raw Read 1×512B** | 698 KB/s | **1,142 KB/s** | 942 KB/s | ERROR | 565 KB/s | 630 KB/s |
| **Raw Read 64× (32KB)** | 2,189 KB/s | **2,173 KB/s** | 1,950 KB/s | ERROR | 1,915 KB/s | 2,035 KB/s |
| **Raw Write 64× (32KB)** | 1,996 KB/s | **2,059 KB/s** | 1,969 KB/s | ERROR | 1,670 KB/s | 995 KB/s |
| **File Read 256KB** | 835 KB/s | **1,274 KB/s** | 1,029 KB/s | 770 KB/s | 631 KB/s | 662 KB/s |
| **File Write 32KB** | 321 KB/s | **501 KB/s** | 367 KB/s | 320 KB/s | 105 KB/s | 173 KB/s |
| **Multi-sector gain** | 65% | 50% | 51% | — | 70% | 67% |
| **Detail** | [card page](cards/sandisk-sa16g-16gb.md) | [card page](cards/lexar-mssd0-64gb.md) | [card page](cards/gigastone-astc-64gb.md) | [card page](cards/samsung-gd4qt-128gb.md) | [card page](cards/gigastone-sd16g-16gb.md) | [card page](cards/pny-sd16g-16gb.md) |

**Winner**: Lexar V30 U3 64GB — fastest across nearly all metrics.

Samsung raw sector results omitted due to test seeding issue (file-level results valid). See [card page](cards/samsung-gd4qt-128gb.md) for details.

---

## Performance Observations

### Card Controller Variance

The dramatic difference in file open times (293 µs vs 14,136 µs) demonstrates that SD card performance varies significantly based on the internal controller, not just the rated speed class.

### Write Speed Scaling

Write throughput increases with block size due to:
1. Fewer command/response cycles
2. Better alignment with card's internal erase blocks
3. Reduced FAT update overhead

| Block Size | Gigastone | PNY | SanDisk |
|------------|-----------|-----|---------|
| 512B | 85 KB/s | 52 KB/s | 90 KB/s |
| 4KB | 210 KB/s (2.5×) | 182 KB/s (3.5×) | 302 KB/s (3.4×) |
| 32KB | 325 KB/s (3.8×) | 216 KB/s (4.2×) | 425 KB/s (4.7×) |

### Read Performance

Sequential read performance is consistent and significantly faster than writes:
- Gigastone: 1,339 KB/s (4× faster than best write)
- PNY: 850 KB/s (4× faster than best write)
- SanDisk: 1,467 KB/s (3.5× faster than best write)

### Multi-Sector Improvement

CMD18/CMD25 multi-sector operations provide 50-70% improvement over repeated single-sector commands:
- Cards with slower internal controllers benefit more (Gigastone HE: 70%, PNY: 67%)
- Fast cards benefit less (Lexar: 50%, Gigastone Camera+: 51%)
- The improvement comes from eliminating per-sector command overhead

### PNY Phison Controller Anomalies

The PNY card (MID $27 Phison) has distinctive behavior:
- **File open: 16.9 ms** (85× slower than other cards' ~200 µs)
- **Raw single-sector write: 57 KB/s** with enormous variance (Min=2,965, Max=11,110 µs)
- **Multi-sector reads surprisingly fast**: 2,110 KB/s at 32 sectors — competitive with the best cards
- **Extremely consistent reads**: Min=Max for single-sector (812 µs) at 320 MHz — zero variance

---

## Theoretical Limits

At 320 MHz sysclk with Smart Pin SPI:
- **SPI clock**: ~22.9 MHz
- **Theoretical byte rate**: ~2,790 KB/s
- **Best raw achieved**: 2,189 KB/s read, 2,059 KB/s write (78% / 74% efficiency)
- **Best file-level achieved**: 1,274 KB/s read, 501 KB/s write

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
| Theoretical (22.9 MHz SPI) | 2,790 | 2,790 | 100% | 100% |
| Raw multi-sector (best) | 2,189 | 2,059 | 78% | 74% |
| File-level (best) | 1,274 | 501 | 46% | 18% |

The raw SPI layer is reasonably efficient. The largest remaining headroom is in file-level write throughput, where FAT metadata updates consume over 80% of available bandwidth.

---

## Test History

| Date | Driver Version | Change | Notes |
|------|----------------|--------|-------|
| 2026-01-21 | Baseline (bit-banged) | Initial benchmark | 3 cards tested |
| 2026-02-07 | Current (smart pin + multi-sector) | Benchmark v2.0 | SanDisk Industrial 16GB @ 320+270 MHz |
| 2026-02-08 | Current (smart pin + multi-sector) | Benchmark v2.0 | Lexar V30 U3 64GB @ 320+270 MHz |
| 2026-02-08 | Current (smart pin + multi-sector) | Benchmark v2.0 | Gigastone Camera Plus 64GB @ 320+270 MHz |
| 2026-02-08 | Current (smart pin + multi-sector) | Benchmark v2.0 | Samsung EVO Select 128GB @ 320+270 MHz (raw sector API errors) |
| 2026-02-08 | Current (smart pin + multi-sector) | Benchmark v2.0 | Gigastone High Endurance 16GB @ 320+270 MHz |
| 2026-02-09 | Current (smart pin + multi-sector) | Benchmark v2.0 | PNY 16GB @ 320+270 MHz |

---

*Document maintained as part of P2-uSD-Study project*
*Last updated: 2026-02-15*
