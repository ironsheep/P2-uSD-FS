# SD Card Driver Regression Tests

Automated regression test suite for the P2 SD Card Driver. All tests execute on real Propeller 2 hardware with a physical SD card.

## Test Summary

| Test Suite | Description | Tests |
|------------|-------------|-------|
| **Mount Tests** | Card initialization, mounting, unmounting, pre-mount errors | 21 |
| **File Operations** | Create, open, close, delete, rename files, type mismatch | 22 |
| **Read/Write Tests** | Data integrity, buffer operations, boundary conditions | 29 |
| **Directory Tests** | Directory listing, navigation, deep nesting, boundaries | 27 |
| **Seek Tests** | Random access, cross-sector seeks, seek boundaries | 37 |
| **Format Tests** | FAT32 structure validation, cross-OS compatibility | 43 |
| **Multiblock Tests** | Multi-sector streamer DMA transfers | 12 |
| **Multicog Tests** | Singleton pattern, concurrent access, lock serialization | 18 |
| **Multihandle Tests** | Multiple simultaneous file handles | 22 |
| **Raw Sector Tests** | Direct sector read/write, large LBA addressing | 14 |
| **Error Handling Tests** | Error conditions, invalid handles, state errors | 6 |
| **Total** | | **251** |

## Prerequisites

- **pnut-ts** and **pnut-term-ts** - See detailed installation instructions for **[macOS](https://github.com/ironsheep/P2-vscode-langserv-extension/blob/main/TASKS-User-macOS.md#installing-pnut-term-ts-on-macos)**, **[Windows](https://github.com/ironsheep/P2-vscode-langserv-extension/blob/main/TASKS-User-win.md#installing-pnut-term-ts-on-windows)**, and **[Linux/RPi](https://github.com/ironsheep/P2-vscode-langserv-extension/blob/main/TASKS-User-RPi.md#installing-pnut-term-ts-on-rpilinux)**
- Parallax Propeller 2 (P2 Edge or P2 board with microSD add-on) connected via USB
- FAT32-formatted SD card (see [Test Card Requirements](#test-card-requirements))

## Compiling and Running Tests

From this `regression-tests/` directory:

```bash
# Compile a test suite
pnut-ts -d -I ../src SD_RT_mount_tests.spin2

# Download and run on P2
pnut-term-ts -r SD_RT_mount_tests.bin
```

The `-I ../src` flag tells the compiler to find `SD_card_driver.spin2` in the `src/` directory.

### Running All Test Suites

Compile and run each test suite one at a time:

```bash
# Core functionality tests
pnut-ts -d -I ../src SD_RT_mount_tests.spin2
pnut-term-ts -r SD_RT_mount_tests.bin

pnut-ts -d -I ../src SD_RT_file_ops_tests.spin2
pnut-term-ts -r SD_RT_file_ops_tests.bin

pnut-ts -d -I ../src SD_RT_read_write_tests.spin2
pnut-term-ts -r SD_RT_read_write_tests.bin

pnut-ts -d -I ../src SD_RT_directory_tests.spin2
pnut-term-ts -r SD_RT_directory_tests.bin

pnut-ts -d -I ../src SD_RT_seek_tests.spin2
pnut-term-ts -r SD_RT_seek_tests.bin

# Error handling tests
pnut-ts -d -I ../src SD_RT_error_handling_tests.spin2
pnut-term-ts -r SD_RT_error_handling_tests.bin

# Advanced feature tests
pnut-ts -d -I ../src SD_RT_multihandle_tests.spin2
pnut-term-ts -r SD_RT_multihandle_tests.bin

pnut-ts -d -I ../src SD_RT_multiblock_tests.spin2
pnut-term-ts -r SD_RT_multiblock_tests.bin

pnut-ts -d -I ../src SD_RT_raw_sector_tests.spin2
pnut-term-ts -r SD_RT_raw_sector_tests.bin

pnut-ts -d -I ../src SD_RT_multicog_tests.spin2
pnut-term-ts -r SD_RT_multicog_tests.bin

# Format test (WARNING: erases card!)
pnut-ts -d -I ../src SD_RT_format_tests.spin2
pnut-term-ts -r SD_RT_format_tests.bin
```

**Note:** Format tests will **erase all data** on the card.

---

## Test Card Requirements

Tests are designed to run on any FAT32-formatted SD card. Some tests (format, raw sector) may **erase data**.

### Recommended Test Card

- 32GB SDHC card (FAT32 formatted)
- Dedicated test card (no critical data)
- See `TestCard/TEST-CARD-SPECIFICATION.md` for detailed requirements

### Test Card Setup

The `TestCard/TESTROOT/` directory contains files that must be copied to the root of your test SD card before running the full test suite. These include test data files for read/write verification, seek testing, and directory navigation.

### Test Card Validation

Compile and run the validation test to verify your card meets requirements:

```bash
cd TestCard/
pnut-ts -d -I ../../src -I .. SD_RT_testcard_validation.spin2
pnut-term-ts -r SD_RT_testcard_validation.bin
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

---

## Test Utilities Framework

All test files use the shared `SD_RT_utilities.spin2` framework:

```spin2
OBJ
    utils : "SD_RT_utilities"

PUB testExample()
    utils.startTestGroup(@"Group Name")

    utils.startTest(@"Test description")
    result := sd.someOperation()
    utils.evaluateBool(result, @"operation succeeds", true)

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

## Hardware Configuration

The microSD add-on board connects to any 8-pin header group on the P2. Pins are defined as offsets from the base pin of the group:

| Offset | Signal | Description |
|--------|--------|-------------|
| +5 | CLK (SCK) | Serial Clock |
| +4 | CS (DAT3) | Chip Select |
| +3 | MOSI (CMD) | Master Out, Slave In |
| +2 | MISO (DAT0) | Master In, Slave Out |
| +1 | Insert Detect | Active low when card inserted (not used by driver) |

The default configuration uses base pin 56 (P2 Edge Module). Modify the `CON` section in the test files if using a different 8-pin group.

---

## License

MIT License - See LICENSE file for details.

Copyright (c) 2026 Iron Sheep Productions, LLC
