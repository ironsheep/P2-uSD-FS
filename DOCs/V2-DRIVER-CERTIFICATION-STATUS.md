# V2 Driver Certification Status

**Date**: 2026-01-25
**Driver**: `src/SD_card_driver_v2.spin2`
**Test Card**: Gigastone 32GB (FAT32, properly formatted)

## Overview

The V2 driver underwent comprehensive certification testing across three phases. The driver is **functional at 320 MHz** but has issues that need investigation before production use.

## Test Results Summary

### Phase 1: Core Functionality

| Test | Pass | Fail | Total | Status | Notes |
|------|------|------|-------|--------|-------|
| Mount | 17 | 0 | 17 | PASS | All mount/unmount cycles work |
| File Ops | 16 | 5 | 21 | PARTIAL | openFile/fileSize return value issues |
| Read/Write | 23 | 0 | 23 | PASS | Basic read/write works correctly |
| Directory | ~21 | 0 | ~21 | PASS* | All tests pass, timeout during cleanup |
| Seek | 33 | 1 | 34 | PARTIAL | One byte value mismatch (254 vs 220) |

### Phase 2: V2-Specific Features

| Test | Pass | Fail | Total | Status | Notes |
|------|------|------|-------|--------|-------|
| Raw Sector | 10 | 2 | 12 | PARTIAL | Tail pattern/sector ID mismatch |
| Multi-Block 320MHz | 6 | 0 | 6 | PASS | Full functionality at default clock |
| Multi-Block 270MHz | 4 | 2 | 6 | PARTIAL | 3,968 byte mismatches - timing issues |

### Phase 3: Stress & Performance

| Test | Status | Notes |
|------|--------|-------|
| Multi-Cog | PASS | All tests pass, timeout during cleanup |
| Performance | PASS | Data collected successfully |
| Frequency Sweep | PARTIAL | Baseline pass, hung at 310MHz test |

## Performance Metrics (320 MHz)

```
Mount time: 239 ms
File open:  1,768 us
File close: 219 us

Multi-Block Read (64 sectors/32KB):
  - Throughput: 2,168 KB/s
  - Time: 15,113 us

Multi-Block Write (64 sectors/32KB):
  - Throughput: 2,468 KB/s
  - Time: 13,272 us

Single vs Multi-Block Comparison (64 sectors):
  - Single-sector (64x CMD17): 59,634 us
  - Multi-sector (1x CMD18):   13,263 us
  - Speedup: 4.5x
```

## Known Issues

### Critical (Must Fix)

1. **Raw Sector Tail Corruption**
   - Last bytes of sector show pattern/ID mismatch
   - Affects: `SD_RT_raw_sector_tests`
   - Impact: Data integrity risk

2. **270 MHz Timing Failures**
   - Massive byte mismatches (3,968 bytes) at lower clock
   - Affects: `SD_RT_multiblock_tests_270MHz`
   - Impact: Not sysclk-independent as designed

### Medium (Should Fix)

3. **File Operations Return Values**
   - `openFile()` returns 0 instead of -1 on some conditions
   - `fileSize()` returns 0 instead of expected value
   - Affects: `SD_RT_file_ops_tests`
   - Impact: Error handling may not work correctly

4. **Seek Byte Mismatch**
   - One test shows byte value 254 when 220 expected
   - Affects: `SD_RT_seek_tests`
   - Impact: Possible seek offset calculation error

### Low (Monitor)

5. **Cleanup Hangs**
   - Card gets stuck during delete operations in cleanup phase
   - Tests pass but timeout waiting for cleanup
   - May indicate card busy-wait issues

6. **Frequency Sweep Hang**
   - Test hangs when changing to 310 MHz
   - May be frequency change timing issue

## Comparison with V1 Driver

| Test | V1 | V2 | Notes |
|------|----|----|-------|
| Mount | 17/17 | 17/17 | Equal |
| File Ops | 21/21 | 16/21 | V2 has issues |
| Read/Write | 23/23 | 23/23 | Equal |
| Directory | 21/21 | ~21/21 | V2 cleanup timeout |
| Seek | 34/34 | 33/34 | V2 has 1 failure |

## Next Steps

1. Investigate raw sector tail corruption - check sector buffer handling
2. Debug 270 MHz timing issues - compare with V1 at same clock
3. Fix file ops return values - align with V1 behavior
4. Investigate seek byte mismatch - check offset calculations
5. Add card recovery after cleanup to prevent hangs

## Test Environment

- **Hardware**: P2 Edge module with SD slot
- **Pins**: CS=60, MOSI=59, MISO=58, SCK=61
- **System Clock**: 320 MHz (primary), 270 MHz (secondary tests)
- **SPI Clock**: 25 MHz (data transfer), 400 kHz (init)
- **Compiler**: pnut-ts 1.51.7

## Files Tested

```
regression-tests-v2/
├── SD_RT_mount_tests.spin2
├── SD_RT_file_ops_tests.spin2
├── SD_RT_read_write_tests.spin2
├── SD_RT_directory_tests.spin2
├── SD_RT_seek_tests.spin2
├── SD_RT_raw_sector_tests.spin2
├── SD_RT_multiblock_tests.spin2
├── SD_RT_multiblock_tests_270MHz.spin2
├── SD_RT_multicog_tests.spin2
├── SD_RT_performance_benchmark.spin2
└── SD_RT_frequency_sweep.spin2
```
