# SD Card Driver Benchmark Results

**Document Purpose**: Record actual performance measurements from benchmark testing
**Test Program**: `regression-tests/SD_RT_performance_benchmark.spin2`
**System Clock**: 320 MHz

---

## Test Configuration

- **Hardware**: P2 Edge Module with microSD slot
- **Pins**: CS=P60, MOSI=P59, MISO=P58, SCK=P61
- **Iterations**: 10 per test (averaged)
- **Test Date**: 2026-01-21

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

## Detailed Card Results

### Gigastone 32GB (Class 10, U1)

**Card Identification**:
- MID: $74
- Product Name: "00000"
- CID: `$74 $49 $54 $30 $30 $30 $30 $30 $1E $62 $4D $D6 $D1 $00 $E7 $E3`

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

### PNY 16GB (Phison Controller)

**Card Identification**:
- MID: $27
- Product Name: "SD16G"
- CID: `$27 $50 $48 $53 $44 $31 $36 $47 $40 $D0 $2C $93 $C2 $01 $4E $83`

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

### SanDisk Extreme 64GB

**Card Identification**:
- MID: $03 (SanDisk)
- Product Name: "SN64G"
- CID: `$03 $53 $44 $53 $4E $36 $34 $47 $86 $7E $65 $07 $71 $01 $6B $8D`

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

---

## Theoretical Limits

At 320 MHz sysclk with current bit-banged SPI:
- **SPI clock**: ~20 MHz (bit_delay = 8)
- **Theoretical max**: ~2.5 MB/s
- **Achieved**: ~1.5 MB/s (60% efficiency)

The gap is due to:
- Command overhead per sector
- FAT table reads during write operations
- Card busy time after writes

---

## Optimization Targets

Based on these baseline measurements, the Smart Pin SPI implementation targets:

| Metric | Current Best | Target | Expected Gain |
|--------|--------------|--------|---------------|
| Read 256KB | 1,467 KB/s | 4,000+ KB/s | 2.7× |
| Write 32KB | 425 KB/s | 1,200+ KB/s | 2.8× |
| Mount | 152 ms | 100 ms | 1.5× |

---

## Test History

| Date | Driver Version | Change | Notes |
|------|----------------|--------|-------|
| 2026-01-21 | Baseline (bit-banged) | Initial benchmark | 3 cards tested |

---

*Document maintained as part of P2-uSD-Study project*
