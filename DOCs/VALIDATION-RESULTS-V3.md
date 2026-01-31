# V3 Driver Validation Results

**Date:** 2026-01-30
**Driver:** SD_card_driver_v3.spin2
**Test Hardware:** P2 Edge Module with SD slot

---

## Executive Summary

The V3 driver with multi-file handle API and singleton architecture has been validated at 320 MHz sysclk. All regression tests pass. The V3 driver introduces a handle-based file API supporting up to 4 simultaneous file opens, while maintaining backward compatibility with V2's raw sector operations.

### Key V3 Features Validated
- **Multi-file handle API**: Up to 4 files open simultaneously (3 read-only + 1 read-write)
- **Single-writer policy**: Enforced at open time to prevent data corruption
- **Singleton architecture**: DAT-based state shared across object instances
- **Worker cog pattern**: All SPI operations serialized through dedicated worker cog
- **Smart pin SPI**: Maintains V2's high-performance smart pin implementation

---

## Regression Test Results

### SD_RT_mount_tests.spin2

| Test | Result | Notes |
|------|--------|-------|
| Operations before mount fail gracefully | PASS | 3 tests |
| Mount with valid pins | PASS | |
| Verify card accessible (volume label) | PASS | |
| Verify free space is reasonable | PASS | |
| Unmount card | PASS | |
| Operations after unmount fail | PASS | |
| Remount card | PASS | |
| Card accessible after remount | PASS | |
| Multiple mount/unmount cycles (3x) | PASS | |
| **Total** | **17/17** | **All pass** |

### SD_RT_file_ops_tests.spin2

| Test | Result | Notes |
|------|--------|-------|
| Create file with handle API | PASS | |
| Write data via handle | PASS | |
| Close and reopen file | PASS | |
| Read data via handle | PASS | |
| Verify data integrity | PASS | |
| Seek operations | PASS | |
| File size queries | PASS | |
| Delete file | PASS | |
| Cleanup verification | PASS | |
| **Total** | **20/20** | **All pass** |

### SD_RT_format_tests.spin2

| Test Group | Result | Notes |
|------------|--------|-------|
| Format Operation | 1/1 | Format with default label |
| MBR Verification | 5/5 | Signature, partition table |
| VBR Verification | 19/19 | Boot sector, BPB, FAT32 fields |
| Backup VBR Verification | 1/1 | Primary/backup match |
| FSInfo Verification | 6/6 | Signatures, free cluster tracking |
| FAT Table Verification | 7/7 | Media type, EOC markers, mirroring |
| Root Directory Verification | 4/4 | Volume label, structure |
| Mount and Usability | 3/3 | Mount, read label, free space |
| **Total** | **46/46** | **All pass** |

---

## Bugs Fixed During Validation

### 1. Command Code Conflict (Critical)

**Symptom:** `initCardOnly()` would hang indefinitely after format operations.

**Root Cause:** Command codes were incorrectly numbered:
- `CMD_INIT_CARD_ONLY (22)` conflicted with `CMD_DEBUG_SLOW_READ (22)`
- `CMD_GET_CARD_SIZE (23)` conflicted with `CMD_DEBUG_CLEAR_ROOT (23)`

**Fix:** Renumbered debug commands to 25 and 26.

**Commit:** `2937546 Fix command code conflict that caused initCardOnly() to hang`

### 2. Singleton driver_mode State Bug (Critical)

**Symptom:** After calling `fmt.stop()`, subsequent `sd.initCardOnly()` would return success but all operations would fail (return zeros).

**Root Cause:** The `stop()` function reset `cog_id` and `api_lock` but did NOT reset `driver_mode`. Since the driver uses DAT-based singleton variables shared across all object instances:
- After `fmt.stop()`, `driver_mode` stayed as `MODE_RAW`
- When the test's `sd.initCardOnly()` was called, it saw `driver_mode == MODE_RAW` and returned immediately without starting the worker cog
- All subsequent operations failed because no worker cog was running

**Fix:** Added `driver_mode := MODE_NONE` in `stop()` to reset state.

**Commit:** `15af891 Fix singleton driver_mode state bug and add V3 format utilities`

---

## V3-Specific Utilities Created

### SD_format_utility_v3.spin2
- FAT32 format utility using V3 driver
- Includes `stop()` method to release worker cog after formatting
- Required for proper singleton handoff between format and verification

### SD_RT_testcard_validation_v3.spin2
- Test card validation using V3 driver
- Validates pre-formatted test card with known file contents
- Tests directory navigation, seek operations, file reading

---

## Architecture Notes

### Singleton Pattern
The V3 driver uses a singleton pattern with DAT section variables:
```spin2
DAT '' singleton control - SHARED across all object instances
  cog_id        LONG    -1              ' Worker cog ID (-1 = not started)
  api_lock      LONG    -1              ' Hardware lock ID (-1 = not allocated)
  driver_mode   LONG    0               ' Current mode: MODE_NONE, MODE_RAW, or MODE_FILESYSTEM
```

This means:
- Only ONE worker cog runs for ALL driver instances
- All API calls serialize through the same lock
- The `stop()` call from ANY instance affects ALL instances
- `driver_mode` must be reset in `stop()` for proper reinitialization

### Driver Modes
| Mode | Value | Description |
|------|-------|-------------|
| MODE_NONE | 0 | Not initialized, worker cog not running |
| MODE_RAW | 1 | Card initialized for raw sector access only |
| MODE_FILESYSTEM | 2 | Full filesystem mounted, file operations available |

---

## Known Issues

### 1. Testcard Validation Requires Pre-formatted Card
The testcard validation test expects specific files on the SD card per TEST-CARD-SPECIFICATION.md. Running format tests first will erase these files.

**Workaround:** Use separate SD cards for format testing vs testcard validation.

### 2. Format Time (Expected Behavior)
Formatting an 8GB card takes approximately 2 minutes due to:
- Writing 14,854 sectors to FAT1
- Writing 14,854 sectors to FAT2
- Individual sector writes (not multi-block optimized)

**Note:** This is expected behavior, not a bug. The format utility prioritizes reliability over speed.

---

## Test Environment

| Parameter | Value |
|-----------|-------|
| Sysclk | 320 MHz |
| SD Card | 8GB SDHC (P2-XFER label after format) |
| Compiler | pnut-ts v1.51.7 |
| Debug Terminal | pnut-term-ts v0.9.6 |

---

## Test Logs

All test logs saved to `tools/logs/`:
- `SD_RT_mount_tests_260130-*.log`
- `SD_RT_file_ops_tests_260130-*.log`
- `SD_RT_format_tests_260130-*.log`

---

## Recommendations

1. **Always call `stop()` when switching between driver instances** - The singleton architecture requires proper cleanup to allow reinitialization.

2. **Use V3 utilities with V3 driver** - Don't mix V2 format utility with V3 verification; use matching versions.

3. **Increase timeout for format tests** - 300 seconds (5 minutes) is sufficient for 8GB cards; larger cards may need more.

4. **Future: Multi-block format writes** - Format performance could be improved by using multi-sector writes for FAT initialization.
