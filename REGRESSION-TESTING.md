# SD Card Driver Regression Testing

This document describes the comprehensive regression test suite for the P2 SD Card Driver project. The test infrastructure enables automated, repeatable validation of all driver functionality on real Propeller 2 hardware.

## Overview

The regression test suite consists of **6 specialized test files** covering different aspects of SD card functionality, supported by a test utilities framework and automated test runner. All tests execute on actual P2 hardware with a physical SD card, ensuring real-world validation.

### Test Summary

| Test Suite | Description | Tests |
|------------|-------------|-------|
| **Mount Tests** | Card initialization, mounting, unmounting | 17 |
| **File Operations** | Create, open, close, delete, rename files | 24 |
| **Read/Write Tests** | Data integrity, buffer operations, append | 30 |
| **Directory Tests** | Directory listing, navigation, attributes | 22 |
| **Seek Tests** | Random access, cross-sector seeks, position | 36 |
| **Format Tests** | FAT32 structure validation, cross-OS compatibility | 43 |
| **Total** | | **172** |

---

## Test Infrastructure

### Directory Structure

```
P2-uSD-Study/
├── regression-tests/           # Test source files
│   ├── SD_RT_utilities.spin2   # Test framework (shared)
│   ├── SD_RT_mount_tests.spin2
│   ├── SD_RT_file_ops_tests.spin2
│   ├── SD_RT_read_write_tests.spin2
│   ├── SD_RT_directory_tests.spin2
│   ├── SD_RT_seek_tests.spin2
│   ├── SD_RT_format_tests.spin2
│   └── logs/                   # Per-test logs (pnut-term-ts)
│
├── tools/                      # Test execution tools
│   ├── run_test.sh             # Automated test runner
│   └── logs/                   # Archived test logs
│
└── src/                        # Driver under test
    ├── SD_card_driver.spin2
    └── SD_format_utility.spin2
```

### Test Runner (`run_test.sh`)

The test runner automates the complete test cycle:

1. **Compile** - Uses `pnut-ts` compiler with debug enabled
2. **Download** - Transfers binary to P2 via serial
3. **Execute** - Runs in headless mode capturing all debug output
4. **Monitor** - Watches for `END_SESSION` marker to detect completion
5. **Log** - Archives output with timestamps for analysis

**Usage:**
```bash
cd tools/
./run_test.sh ../regression-tests/SD_RT_mount_tests.spin2
./run_test.sh ../regression-tests/SD_RT_format_tests.spin2 -t 120  # Extended timeout
```

**Exit Codes:**
- `0` - Test completed successfully (END_SESSION found)
- `1` - Compilation failed
- `2` - Download/run failed
- `3` - Timeout (END_SESSION not found)
- `4` - Usage error

### Test Utilities Framework (`SD_RT_utilities.spin2`)

The test utilities provide a consistent framework for all test files:

```spin2
OBJ
    utils : "SD_RT_utilities"

PUB testExample()
    utils.startTestGroup(@"Group Name")

    utils.startTest(@"Test description")
    result := sd.someOperation()
    utils.evaluateBool(result, @"operation succeeds", true)

    utils.startTest(@"Multi-value test")
    utils.setCheckCountPerTest(5)
    repeat idx from 0 to 4
        utils.evaluateSubValue(buffer[idx], @"byte value", expectedValues[idx])

    utils.ShowTestEndCounts()
```

**Key Functions:**
| Function | Purpose |
|----------|---------|
| `startTestGroup(pName)` | Begin a logical group of related tests |
| `startTest(pName)` | Begin a single test case |
| `evaluateBool(result, pMsg, expected)` | Check boolean result |
| `evaluateSingleValue(result, pMsg, expected)` | Check numeric value |
| `evaluateSubBool(...)` | Check boolean within multi-check test |
| `evaluateSubValue(...)` | Check value within multi-check test |
| `setCheckCountPerTest(n)` | Set expected sub-checks for current test |
| `ShowTestEndCounts()` | Display final pass/fail summary |

---

## Test Suites

### 1. Mount Tests (`SD_RT_mount_tests.spin2`)

**Purpose:** Validate SD card initialization, mounting, and unmounting operations.

**Test Groups:**
- **Card Initialization** - SPI communication, card detection, card type identification
- **Filesystem Mount** - FAT32 parsing, volume label reading, free space calculation
- **Mount/Unmount Cycle** - Repeated mount/unmount operations, resource cleanup
- **Error Handling** - Invalid pin configurations, missing card scenarios

**Key Validations:**
- Card responds to SPI commands
- SDHC/SDXC block addressing works correctly
- FAT32 boot sector parsed correctly
- Volume label extracted properly
- Free space reported accurately
- Clean unmount without data corruption

---

### 2. File Operations Tests (`SD_RT_file_ops_tests.spin2`)

**Purpose:** Validate basic file creation, opening, closing, deletion, and renaming.

**Test Groups:**
- **File Creation** - `newFile()` creates files in root directory
- **File Opening** - `openFile()` locates and opens existing files
- **File Closing** - `closeFile()` flushes buffers and updates directory
- **File Deletion** - `deleteFile()` removes files and frees clusters
- **File Existence** - `fileExists()` correctly reports file presence
- **File Renaming** - Rename operations preserve file content

**Key Validations:**
- Files appear in directory after creation
- File size reported correctly
- Deleted files no longer accessible
- Rename preserves data integrity
- Multiple files can coexist

---

### 3. Read/Write Tests (`SD_RT_read_write_tests.spin2`)

**Purpose:** Validate data integrity during read and write operations.

**Test Groups:**
- **Basic Write** - Write known patterns to files
- **Basic Read** - Read back and verify patterns
- **Large Transfers** - Multi-sector writes and reads
- **Append Operations** - Adding data to existing files
- **Boundary Conditions** - Sector boundary crossing
- **Buffer Management** - Partial buffer operations

**Key Validations:**
- Written data reads back identically
- Large files span multiple clusters correctly
- Append doesn't corrupt existing data
- Sector boundaries handled transparently
- Partial writes work correctly

---

### 4. Directory Tests (`SD_RT_directory_tests.spin2`)

**Purpose:** Validate directory listing and file attribute operations.

**Test Groups:**
- **Directory Enumeration** - `firstFile()`, `nextFile()` iteration
- **File Attributes** - Name, size, date/time retrieval
- **Multiple Files** - Listing with many directory entries
- **Special Entries** - Volume label, dot entries handling

**Key Validations:**
- All files enumerated without missing entries
- File names returned correctly (8.3 format)
- File sizes match actual content
- Directory iteration terminates properly
- Volume label distinguished from files

---

### 5. Seek Tests (`SD_RT_seek_tests.spin2`)

**Purpose:** Validate random access and seek operations within files.

**Test Groups:**
- **Basic Seek** - `seek()` to specific positions
- **Cross-Sector Seek** - Seeking across 512-byte boundaries
- **Random Access Pattern** - Non-sequential position jumps
- **readByte() with Position** - Single-byte random reads
- **Seek Edge Cases** - EOF boundary, seek beyond EOF
- **Sequential vs Seek** - Verify seek doesn't affect sequential reads

**Key Validations:**
- Seek to position 0 returns to file start
- Seek across sector boundaries works correctly
- Random access pattern reads correct data
- Seek beyond EOF fails appropriately
- Sequential read after seek continues correctly

---

### 6. Format Tests (`SD_RT_format_tests.spin2`)

**Purpose:** Validate FAT32 format structure for cross-OS compatibility (Windows, macOS, Linux).

**Test Groups:**

#### MBR Verification (5 tests)
- Boot signature ($AA55)
- Partition bootable flag ($00 or $80)
- Partition type ($0C = FAT32 LBA)
- Partition start sector (8192 for 4MB alignment)
- Partition size

#### VBR/BPB Verification (19 tests)
- Jump instruction ($EB or $E9)
- Boot signature ($AA55)
- OEM name (printable ASCII)
- Bytes per sector (512)
- Sectors per cluster (power of 2)
- Reserved sectors (32)
- Number of FATs (2)
- Root entry count (0 for FAT32)
- Total sectors 16-bit (0 for FAT32)
- Media type ($F8 fixed disk)
- FAT size 16-bit (0 for FAT32)
- Hidden sectors (matches partition start)
- Total sectors 32-bit
- FAT32 sectors per FAT
- Root cluster number (2)
- FSInfo sector location (1)
- Backup boot sector location (6)
- Extended boot signature ($29)
- File system type string ("FAT32   ")

#### Backup Structure Verification (7 tests)
- Backup VBR matches primary (sector 6)
- FSInfo lead signature ($41615252 "RRaA")
- FSInfo structure signature ($61417272 "rrAa")
- FSInfo trail signature ($AA550000)
- FSInfo free cluster count
- FSInfo next free cluster hint
- Backup FSInfo matches primary (sector 7)

#### FAT Table Verification (5 tests)
- FAT[0] = $0FFFFFF8 (media type)
- FAT[1] = $0FFFFFFF (end of chain)
- FAT[2] = $0FFFFFFF (root directory EOC)
- FAT[3] = 0 (free)
- FAT2 mirrors FAT1

#### Root Directory Verification (4 tests)
- Volume label entry present (attribute $08)
- Volume label characters valid
- Second entry is end marker or empty
- Remaining bytes zeroed

#### Mount and Usability (3 tests)
- Mount succeeds after format
- Volume label readable
- Free space reported correctly

---

## Running the Full Test Suite

To run all regression tests:

```bash
cd tools/

# Run each test suite
./run_test.sh ../regression-tests/SD_RT_mount_tests.spin2
./run_test.sh ../regression-tests/SD_RT_file_ops_tests.spin2
./run_test.sh ../regression-tests/SD_RT_read_write_tests.spin2
./run_test.sh ../regression-tests/SD_RT_directory_tests.spin2
./run_test.sh ../regression-tests/SD_RT_seek_tests.spin2
./run_test.sh ../regression-tests/SD_RT_format_tests.spin2 -t 120
```

**Note:** Format tests require extended timeout (`-t 120`) due to FAT initialization time on large cards.

---

## Test Card Requirements

Tests are designed to run on any FAT32-formatted SD card. The format tests will **erase all data** on the card.

**Recommended Test Card:**
- 32GB or 64GB SDHC/SDXC card
- FAT32 formatted (or will be formatted by format tests)
- No critical data (tests create/delete files)

**Hardware Configuration:**
```spin2
CON
    SD_CS   = 60    ' Chip Select
    SD_MOSI = 59    ' Master Out Slave In
    SD_MISO = 58    ' Master In Slave Out
    SD_SCK  = 61    ' Serial Clock
```

---

## Interpreting Results

### Successful Test Output

```
==============================================
  SD Card Driver - Mount Tests
==============================================

=== Test Group: Card Initialization ===

* Test #1: Initialize card
   mount() returns true: result = 4_294_967_295
    -> pass

...

============================================================
* 17 Tests - Pass: 17, Fail: 0
============================================================

* Mount/Unmount Tests Complete
END_SESSION
```

### Failed Test Output

```
* Test #5: Verify file content
   byte at 0: result = 65 (expected 0)
    -> FAIL
```

### Test Count Warnings

```
*  BAD TEST COUNT: 17 <> 16 (missing 1 tests)
```

This indicates a mismatch between `startTest()` calls and `evaluate*()` calls. Usually benign if all tests pass.

---

## Extending the Test Suite

### Adding a New Test File

1. Create `regression-tests/SD_RT_<name>_tests.spin2`
2. Include the utilities object: `utils : "SD_RT_utilities"`
3. Follow the test pattern with `startTestGroup()`, `startTest()`, `evaluate*()`
4. End with `utils.ShowTestEndCounts()` and `debug("END_SESSION")`

### Adding Tests to Existing Files

```spin2
' Add to existing test group
utils.startTest(@"New test description")
result := sd.newOperation()
utils.evaluateBool(result, @"operation name", expectedValue)
```

---

## Continuous Integration

The test suite is designed for automated CI execution. The test runner returns appropriate exit codes for CI systems to detect pass/fail status.

**GitHub Actions Integration:**
- Build verification on push/PR
- Full regression suite on release tags
- Test logs archived as artifacts

---

## Version History

| Date | Changes |
|------|---------|
| Jan 2026 | Initial test framework and 5 test suites |
| Jan 2026 | Added comprehensive FAT32 format tests (43 tests) |
| Jan 2026 | Fixed backup FSInfo write in formatter |

---

## License

MIT License - See LICENSE file for details.

Copyright (c) 2026 Iron Sheep Productions, LLC
