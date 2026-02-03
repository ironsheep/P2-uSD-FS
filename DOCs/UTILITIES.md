# SD Card Driver Utilities

This document describes the standalone utility programs included with the P2 SD Card Driver. These utilities help with card formatting, characterization, performance testing, and filesystem validation.

## Overview

The utilities are located in `src/UTILS/` and can be run independently using the test runner from the `tools/` directory.

### Utility Summary

| Utility | Purpose | Destructive? |
|---------|---------|:------------:|
| **SD_format_utility.spin2** | FAT32 card formatter (library) | Yes |
| **SD_format_card.spin2** | Format runner (standalone) | Yes |
| **SD_card_characterize.spin2** | Card register reader | No |
| **SD_speed_characterize.spin2** | SPI speed tester | No |
| **SD_frequency_characterize.spin2** | Sysclk frequency tester | No |
| **SD_performance_benchmark.spin2** | Throughput measurement | Yes* |
| **SD_FAT32_audit.spin2** | Filesystem validator | No |

*Creates temporary test files that are deleted after testing.

---

## Running Utilities

All utilities are run from the `tools/` directory using the test runner:

```bash
cd tools/
./run_test.sh ../src/UTILS/<utility>.spin2 [-t timeout]
```

---

## Utility Details

### 1. SD_format_utility.spin2 (Library)

**Purpose:** FAT32 format library for SD cards.

This is a library object that provides FAT32 formatting capability. It is not run directly but is used by other programs (like SD_format_card.spin2 or the regression tests).

**API:**
```spin2
OBJ
    fmt : "SD_format_utility"

PUB main()
    ' Format with default label "P2-XFER"
    result := fmt.format(SD_CS, SD_MOSI, SD_MISO, SD_SCK)

    ' Format with custom label (max 11 characters)
    result := fmt.formatWithLabel(SD_CS, SD_MOSI, SD_MISO, SD_SCK, @"MYVOLUME")
```

**Creates:**
- MBR with single FAT32 LBA partition (type $0C)
- 4MB-aligned partition start (sector 8192)
- VBR (Volume Boot Record) with standard BPB
- Backup VBR at sector 6
- FSInfo sector with free cluster tracking
- Backup FSInfo at sector 7
- Dual FAT tables (FAT1 and FAT2)
- Root directory with volume label entry

**Cross-OS Compatibility:**
- Windows, macOS, and Linux compatible
- Follows Microsoft FAT32 specification
- Uses standard sector sizes and alignments

---

### 2. SD_format_card.spin2

**Purpose:** Simple standalone format runner.

**Usage:**
```bash
./run_test.sh ../src/UTILS/SD_format_card.spin2 -t 120
```

**WARNING:** This will **ERASE ALL DATA** on the SD card!

**Output:**
```
======================================================
  SD Card Format Utility
======================================================

WARNING: This will ERASE ALL DATA on the card!

Formatting card with label 'P2-BENCH'...

FORMAT SUCCESSFUL!

END_SESSION
```

---

### 3. SD_card_characterize.spin2

**Purpose:** Extract and display all card register information.

**Usage:**
```bash
./run_test.sh ../src/UTILS/SD_card_characterize.spin2 -t 60
```

**Reads and Displays:**

| Register | Size | Information |
|----------|------|-------------|
| **CID** | 16 bytes | Manufacturer ID, OEM ID, Product Name, Revision, Serial Number, Manufacturing Date |
| **CSD** | 16 bytes | Card capacity, transfer speeds, command classes, read/write block sizes |
| **SCR** | 8 bytes | SD specification version, security features, bus widths supported |
| **OCR** | 4 bytes | Operating voltage ranges, card capacity status |
| **VBR/BPB** | 512 bytes | FAT32 filesystem parameters |

**Sample Output:**
```
┌──────────────────────────────────────────┐
│ SD Card Characterization Diagnostic      │
└──────────────────────────────────────────┘

========== CID (Card Identification) ==========
  Manufacturer ID:    $03 (SanDisk)
  OEM/Application ID: "SD"
  Product Name:       "SD64G"
  Product Revision:   8.0
  Serial Number:      $12345678
  Manufacturing Date: 2023/06

========== CSD (Card Specific Data) ==========
  CSD Version:        2.0 (SDHC/SDXC)
  Card Capacity:      59.48 GB
  Max Transfer Rate:  50 MHz
  Read Block Length:  512 bytes
  ...

========== Filesystem (VBR/BPB) ==========
  Volume Label:       P2-XFER
  Sectors per Cluster: 64
  Total Sectors:      124735488
  Free Space:         59.45 GB
```

**Use Cases:**
- Identify card manufacturer and model
- Verify card capacity matches specification
- Check supported features before use
- Debug card compatibility issues
- Build a card database/catalog

---

### 4. SD_speed_characterize.spin2

**Purpose:** Find maximum reliable SPI clock speed for a specific card.

**Usage:**
```bash
./run_test.sh ../src/UTILS/SD_speed_characterize.spin2 -t 300
```

**Test Strategy:**

| Phase | Test | Purpose |
|-------|------|---------|
| **Phase 1** | 1,000 single-sector reads | Quick reliability check |
| **Phase 2** | 10,000 single-sector reads | Statistical confidence |
| **Phase 3** | 100 × 8-sector reads | Sustained transfer test |

Testing proceeds from lowest to highest speed. If any phase fails, testing stops for that speed and higher speeds are skipped.

**Speed Levels Tested:**
- 18 MHz, 20 MHz, 22 MHz, 25 MHz, 28 MHz
- 30 MHz, 33 MHz, 37 MHz, 40 MHz, 45 MHz, 50 MHz

**Output Includes:**
- Target frequency vs actual achievable frequency
- Delta percentage from ideal (due to P2 clock division)
- Pass/fail status for each phase
- CRC error counts and timeout counts
- Maximum reliable speed recommendation

**Sample Output:**
```
SD Card SPI Speed Characterization
==================================
Card: SanDisk Extreme 64GB

Speed Tests:
  18 MHz: Phase 1 PASS, Phase 2 PASS, Phase 3 PASS
  20 MHz: Phase 1 PASS, Phase 2 PASS, Phase 3 PASS
  25 MHz: Phase 1 PASS, Phase 2 PASS, Phase 3 PASS
  30 MHz: Phase 1 PASS, Phase 2 PASS, Phase 3 PASS
  33 MHz: Phase 1 PASS, Phase 2 FAIL (3 CRC errors)

Recommended Maximum Speed: 30 MHz
```

**Use Cases:**
- Determine safe operating speed for production
- Compare cards for speed capability
- Identify marginal cards with reliability issues
- Optimize driver configuration per card type

---

### 5. SD_frequency_characterize.spin2

**Purpose:** Find sysclk frequency boundaries for reliable streamer timing.

**Usage:**
```bash
./run_test.sh ../src/UTILS/SD_frequency_characterize.spin2 -t 300
```

**Test Method:**
This utility dynamically changes the P2 sysclk frequency using `clkset()` to identify exactly where multi-block operations fail. It helps find timing-sensitive frequency ranges and quantization boundaries.

**Test Frequencies:**
- 320 MHz (baseline)
- 310 MHz, 305 MHz, 300 MHz
- 295 MHz, 290 MHz, 280 MHz, 270 MHz
- 260 MHz, 255 MHz, 250 MHz, 240 MHz
- 220 MHz, 200 MHz

At each frequency, the test performs:
1. `writeSectorsRaw(8)` - Write 8 sectors (4KB)
2. `readSectorsRaw(8)` - Read 8 sectors back
3. Data integrity verification

**Output Includes:**
- Half-period value at each frequency
- Pass/fail status for multi-block operations
- Data integrity verification results
- Identification of working vs failing frequencies

**Use Cases:**
- Determine safe sysclk frequencies for production
- Identify timing boundaries for streamer operations
- Debug frequency-related failures
- Validate driver timing across frequency ranges

---

### 6. SD_performance_benchmark.spin2

**Purpose:** Measure read/write throughput for real-world performance data.

**Usage:**
```bash
./run_test.sh ../src/UTILS/SD_performance_benchmark.spin2 -t 180
```

**Measurements:**

| Category | Tests |
|----------|-------|
| **Mount/Unmount** | Timing for card initialization and filesystem mount |
| **Raw Sector** | Hardware-level read/write bypassing filesystem |
| **Multi-Sector** | CMD18/CMD25 bulk transfers (8, 32, 64 sectors) |
| **Filesystem** | Real-world file read/write through driver API |
| **Overhead** | File open/close timing |

**Test Sizes (Based on Embedded Use Cases):**

| Size | Use Case |
|------|----------|
| 512 B | Single log entry |
| 4 KB | Configuration file |
| 32 KB | Icon or small data batch |
| 128 KB | Small display image |
| 256 KB | Larger display image |

**Output Format:**
```
SD Card Performance Benchmark
=============================
Card: Gigastone 32GB

Raw Sector Performance:
  Read:  512 bytes in 0.42 ms (1.19 MB/s)
  Write: 512 bytes in 2.31 ms (0.22 MB/s)

Multi-Sector Performance (64 sectors = 32KB):
  Read:  32768 bytes in 18.2 ms (1.76 MB/s)
  Write: 32768 bytes in 45.7 ms (0.70 MB/s)

Filesystem Performance:
  File Read (32KB):  62.3 ms (513 KB/s)
  File Write (32KB): 89.1 ms (359 KB/s)
  File Open:         12.4 ms
  File Close:        8.7 ms
```

**Statistics:**
- Each measurement repeated 10 times
- Reports min/avg/max values
- Calculates throughput in KB/s and MB/s

**Use Cases:**
- Establish performance baselines
- Compare different SD cards
- Measure impact of driver optimizations
- Verify production card performance

---

### 7. SD_FAT32_audit.spin2

**Purpose:** Verify FAT32 filesystem integrity without modifying the card.

**Usage:**
```bash
./run_test.sh ../src/UTILS/SD_FAT32_audit.spin2 -t 60
```

**Read-Only:** This tool does NOT modify any data on the card.

**Checks Performed:**

| Structure | Validations |
|-----------|-------------|
| **MBR** | Boot signature, partition type, partition boundaries |
| **VBR** | Jump instruction, BPB fields, extended signature |
| **Backup VBR** | Matches primary VBR (sector 6) |
| **FSInfo** | All three signatures valid, free cluster count reasonable |
| **Backup FSInfo** | Matches primary FSInfo (sector 7) |
| **FAT Tables** | FAT1 and FAT2 match, media descriptor valid |
| **Root Directory** | Volume label present, structure valid |
| **Mount Test** | Driver can mount and read filesystem |

**Sample Output:**
```
==============================================
  FAT32 Filesystem Audit Tool
  (Read-only - does not modify card)
==============================================

* Initializing card...
Card initialized successfully

=== MBR Structure ===
[PASS] Boot signature: $AA55
[PASS] Partition type: $0C (FAT32 LBA)
[PASS] Partition start: 8192 (4MB aligned)

=== VBR Structure ===
[PASS] Jump instruction: $EB
[PASS] Bytes per sector: 512
[PASS] Sectors per cluster: 64
[PASS] Reserved sectors: 32
...

=== FAT Consistency ===
[PASS] FAT1 media descriptor: $F8
[PASS] FAT2 matches FAT1

=== Summary ===
Tests: 24, Passed: 24, Failed: 0
Filesystem integrity: OK

END_SESSION
```

**Use Cases:**
- Verify filesystem after running tests
- Check card health before deployment
- Debug mount failures
- Validate format utility output

---

## Directory Structure

```
src/UTILS/
├── SD_format_utility.spin2         # FAT32 formatter (library)
├── SD_format_card.spin2            # Format runner (standalone)
├── SD_card_characterize.spin2      # Card register reader
├── SD_speed_characterize.spin2     # SPI speed tester
├── SD_frequency_characterize.spin2 # Sysclk frequency tester
├── SD_performance_benchmark.spin2  # Throughput measurement
├── SD_FAT32_audit.spin2            # Filesystem validator
└── logs/                           # Utility output logs
```

---

## Recommended Workflow

### New Card Setup

1. **Characterize** - Read card registers to identify the card
   ```bash
   ./run_test.sh ../src/UTILS/SD_card_characterize.spin2 -t 60
   ```

2. **Speed Test** - Find maximum reliable SPI speed
   ```bash
   ./run_test.sh ../src/UTILS/SD_speed_characterize.spin2 -t 300
   ```

3. **Format** - Create clean FAT32 filesystem
   ```bash
   ./run_test.sh ../src/UTILS/SD_format_card.spin2 -t 120
   ```

4. **Audit** - Verify filesystem structure
   ```bash
   ./run_test.sh ../src/UTILS/SD_FAT32_audit.spin2 -t 60
   ```

5. **Benchmark** - Measure performance baseline
   ```bash
   ./run_test.sh ../src/UTILS/SD_performance_benchmark.spin2 -t 180
   ```

### After Testing

Run the audit tool to verify filesystem integrity:
```bash
./run_test.sh ../src/UTILS/SD_FAT32_audit.spin2 -t 60
```

---

## Hardware Configuration

All utilities use the P2 Edge default pin configuration:

```spin2
CON
    SD_CS   = 60    ' Chip Select
    SD_MOSI = 59    ' Master Out Slave In
    SD_MISO = 58    ' Master In Slave Out
    SD_SCK  = 61    ' Serial Clock
```

Modify the `CON` section in each utility if using different pins.

---

## License

MIT License - See LICENSE file for details.

Copyright (c) 2026 Iron Sheep Productions, LLC
