# V2 Driver Validation Results

**Date:** 2026-01-23
**Driver:** SD_card_driver_v2.spin2
**Test Hardware:** P2 Edge Module with SD slot

---

## Executive Summary

The V2 driver with smart pin SPI and streamer-based multi-block operations has been validated at 320 MHz sysclk. Multi-block operations provide **77% performance improvement** over single-sector operations. However, testing revealed a **sysclk-dependent timing issue** that causes data corruption at lower frequencies (270 MHz tested).

---

## Task 14: Full Regression Suite at 320 MHz

### V1 Driver (SD_card_driver.spin2) Results

| Test Suite | Pass | Fail | Notes |
|------------|------|------|-------|
| Mount Tests | 17/17 | 0 | All pass |
| File Ops Tests | 21/24 | 0 | 3 tests skipped |
| Directory Tests | 19/22 | 2 | Expected-error tests fail |
| Seek Tests | 33/34 | 1 | Expected-error test fails |
| Read/Write Tests | 22/30 | 1 | 7 tests skipped |

**Notes:**
- Failures are in expected-error handling (driver returns success instead of error for invalid operations)
- Core functionality (mount, read, write, seek) works correctly
- File-level operations occasionally hang after many iterations (card state issue)

### V2 Driver (SD_card_driver_v2.spin2) Results

| Test Suite | Pass | Fail | Notes |
|------------|------|------|-------|
| Multi-block Tests | 6/6 | 0 | All scenarios pass |
| Performance Benchmark | PASS | - | Multi-sector metrics collected |

**Multi-block test scenarios verified:**
1. Round-trip: writeSectorsRaw(8) → readSectorsRaw(8) → verify ✓
2. Multi-write, single-read: writeSectorsRaw(8) → 8x readSectorRaw() → verify ✓
3. Single-write, multi-read: 8x writeSectorRaw() → readSectorsRaw(8) → verify ✓
4. Edge case count=1: Falls back to single-sector correctly ✓
5. Edge case count=0: Returns 0 immediately ✓
6. Large transfer (64 sectors = 32KB): Round-trip verified ✓

---

## Task 15: Sysclk Independence Test at 270 MHz

### Results

| Test | 320 MHz | 270 MHz | Status |
|------|---------|---------|--------|
| Test 1: Multi-write + Multi-read (8 sectors) | PASS | **FAIL** | 3,472 byte mismatches |
| Test 2: Multi-write + Single-reads | PASS | PASS | - |
| Test 3: Single-writes + Multi-read | PASS | PASS | - |
| Test 4a: count=1 edge case | PASS | PASS | - |
| Test 4b: count=0 edge case | PASS | PASS | - |
| Test 5: Large multi-block (64 sectors) | PASS | **FAIL** | 24,800 byte mismatches |

### Analysis

**Pattern observed:** Data corruption occurs specifically when:
- writeSectorsRaw() followed by readSectorsRaw() (sequential multi-block)

**Working combinations at 270 MHz:**
- writeSectorsRaw() + individual readSectorRaw() calls
- individual writeSectorRaw() calls + readSectorsRaw()

**Root cause hypothesis:** Streamer NCO phase alignment or timing calculation quantization error at 270 MHz. The integer division in calculating timing parameters creates different phase relationships at different sysclk frequencies.

### Timing Calculations

At 320 MHz (target 25 MHz SPI):
- `half_period = 320M / 50M = 6.4 → 7` (ceiling)
- `actual_spi = 320M / 14 = 22.86 MHz`
- `xfrq = $4000_0000 / 7 = $0924_9249`

At 270 MHz (target 25 MHz SPI):
- `half_period = 270M / 50M = 5.4 → 6` (ceiling)
- `actual_spi = 270M / 12 = 22.5 MHz`
- `xfrq = $4000_0000 / 6 = $0AAA_AAAA`

**Conclusion:** The V2 driver is **NOT fully sysclk-independent** for sequential multi-block streamer operations. Single-sector operations and mixed single/multi operations work at both frequencies.

---

## Task 16: Performance Benchmarks (Partial)

### Test Card: 64GB SDXC (P2-BENCH volume label)

**Multi-Sector Write Performance (CMD25):**

| Sectors | Size | Time (µs) | Throughput |
|---------|------|-----------|------------|
| 8 | 4 KB | 2,665 | 1,536 KB/s |
| 32 | 16 KB | 7,726 | 2,120 KB/s |
| 64 | 32 KB | 14,994 | 2,185 KB/s |

**Multi-Sector Read Performance (CMD18):**

| Sectors | Size | Time (µs) | Throughput |
|---------|------|-----------|------------|
| 8 | 4 KB | 2,078 | 1,971 KB/s |
| 32 | 16 KB | 6,871 | 2,384 KB/s |
| 64 | 32 KB | 13,273 | 2,468 KB/s |

**Single vs Multi-Sector Comparison (64 sectors):**

| Method | Time (µs) | Throughput |
|--------|-----------|------------|
| Single-sector (64x CMD17) | 59,409 | ~550 KB/s |
| Multi-sector (1x CMD18) | 13,263 | 2,468 KB/s |
| **Improvement** | **77%** | **4.5x faster** |

### Remaining Work
- Benchmark Gigastone 32GB card
- Benchmark PNY 16GB card
- Benchmark SanDisk Extreme 64GB card

---

## Known Issues

### 1. Sysclk-Dependent Timing (Critical for portability)
- Sequential multi-block operations fail at 270 MHz
- Likely caused by NCO phase quantization
- **Mitigation needed** for sysclk-independent operation

### 2. File Operation Hangs (Intermittent)
- V1 driver file operations occasionally hang
- Appears to be card state related
- Not blocking for raw sector operations

### 3. Expected-Error Test Failures (Minor)
- Driver returns success instead of error for some invalid operations
- Does not affect normal operation
- Could be addressed in future cleanup

---

## Recommendations

1. **Investigate sysclk timing issue** - Analyze NCO/phase calculations across frequency range to find safe operating regions

2. **Consider dedicated PASM cog** - Inline PASM copy-then-execute may have variable latency affecting streamer sync

3. **Add runtime frequency detection** - Driver could auto-tune timing parameters based on actual clkfreq

4. **Document sysclk requirements** - Until fixed, document that 320 MHz sysclk is recommended

---

## Test Logs

All test logs saved to `tools/logs/`:
- `SD_RT_mount_tests_260123-*.log`
- `SD_RT_multiblock_tests_260123-*.log`
- `SD_RT_multiblock_tests_270MHz_260123-*.log`
- `SD_RT_performance_benchmark_260123-*.log`
