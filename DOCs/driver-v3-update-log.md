# SD Card Driver V3 - Update Log

## Overview

This document tracks issues discovered during multi-card regression testing of the V3 driver (SD_card_driver.spin2). The goal is to understand card-specific behaviors and stabilize the driver across all supported cards.

---

## Bug Fix: do_close_h() Indentation Error

**Date**: 2026-02-15
**Severity**: Critical (affected ALL cards)
**Status**: FIXED

### Problem
`do_close_h()` (line 1586-1588) had an indentation bug where `result := E_IO_ERROR` was at the `else` block level (6 spaces) instead of inside the `if not writeSector(...)` block (8 spaces). In Spin2, indentation determines scope, so this caused **every** write handle close to unconditionally return E_IO_ERROR, even when all I/O succeeded.

### Root Cause
```spin2
' BEFORE (broken):
      if not writeSector(h_dir_sector[handle], BUF_DIR)
        debug("  [do_close_h] Directory write FAILED")
      result := E_IO_ERROR    ' <-- WRONG: runs always, not just on failure

' AFTER (fixed):
      if not writeSector(h_dir_sector[handle], BUF_DIR)
        debug("  [do_close_h] Directory write FAILED")
        result := E_IO_ERROR  ' <-- CORRECT: only on failure
```

### Impact
- closeFileHandle() returned E_IO_ERROR (-7) on every write handle close
- Affected multihandle test #8 "Write persists through close/reopen cycle"
- Data was actually written correctly; only the return value was wrong

### Verification
- Gigastone 8GB: multihandle tests went from 18/19 to **19/19 pass**

---

## Feature: Write Path Warmup in initCardOnly()

**Date**: 2026-02-15
**Status**: REMOVED - caused corruption on SanDisk SA16G; unproven on Samsung

### What Was Added
Added a warmup sequence (read sector 0, write sector 0 back, read sector 0 again) at the end of `initCardOnly()` after successful card initialization. Located at line 2662-2671.

### Why It Was Added
The Samsung GD4QT 128GB card's first write after `initCardOnly()` doesn't persist. The warmup "primes" the write path so subsequent writes work correctly. Without it, the format utility's MBR write silently fails.

### Problem Discovered
**The warmup corrupts data on the SanDisk SA16G.** The write-back of sector 0 doesn't faithfully reproduce the data. After warmup, the MBR partition type reads back as `$06` (FAT16) instead of the `$0C` (FAT32 LBA) that was just formatted.

This means:
- Samsung GD4QT: warmup is **required** (first write fails without it)
- SanDisk SA16G: warmup is **destructive** (write corrupts sector 0 data)
- Gigastone ASTC 8GB: warmup is **harmless** (all tests pass with it)

### Resolution
**Warmup removed from initCardOnly()** on 2026-02-15. The warmup was unproven (only tested against the flaky Samsung card) and actively caused write corruption on the SanDisk SA16G. The Samsung's first-write issue will be investigated separately once stable cards pass cleanly.

---

## Card-Specific Test Results

### Card 1: SanDisk 128GB SDXC (problems first discovered)

**Status**: Tested in earlier sessions — problems first discovered on this card
**Cards available**: SanDisk SN128 (Nintendo Switch, SN:$F79E_34F6) and SanDisk AGGCF (Extreme PRO, SN:$E05C_352B) — both 128GB (~119GB usable), both characterized 2026-02-02
**Note**: Regression test results from these earlier sessions not captured in this log. The failures on this card prompted the multi-card testing effort documented below.

---

### Card 2: Gigastone ASTC 8GB

**Designator**: Gigastone ASTC Class 10, U1 (tested previously)
**Result**: **256/256 pass (100%)**

| Test Suite | Pass | Fail | Total |
|---|---|---|---|
| Format | 46 | 0 | 46 |
| Mount | 21 | 0 | 21 |
| File ops | 22 | 0 | 22 |
| Read/write | 29 | 0 | 29 |
| Directory | 28 | 0 | 28 |
| Seek | 37 | 0 | 37 |
| Multicog | 14 | 0 | 14 |
| Multihandle | 19 | 0 | 19 |
| Multiblock | 6 | 0 | 6 |
| Raw sector | 14 | 0 | 14 |

**Notes**: All tests pass cleanly. The warmup in initCardOnly() has no adverse effect. This card is the baseline for driver correctness.

---

### Card 3: SanDisk SA16G SDHC 16GB (industrial)

**Designator**: SanDisk SA16G SDHC 14GB [FAT16] SD 5.x rev8.0 SN:$93E9_C0A1 2025/11
**Card ID**: Class 10, U1, V10, SPI 25 MHz
**CID**: MID=$03 (SanDisk), OID="SD", PNM="SA16G", PRV=8.0
**Total Sectors**: 31,116,288 (from format), 31,115,264 (from CSD C_SIZE)
**Write Timeout**: 250ms (from CSD R2W_FACTOR=2)
**Result**: **192/236 pass (81%)**

| Test Suite | Pass | Fail | Total |
|---|---|---|---|
| Format | 44 | 2 | 46 |
| Mount | 21 | 0 | 21 |
| File ops | 14 | 7 | 22 |
| Read/write | 16 | 4+9 skip | 29 |
| Directory | 25 | 3 | 28 |
| Seek | 36 | 1 | 37 |
| Multicog | 14 | 0 | 14 |
| Multihandle | 8 | 11 | 19 |
| Multiblock | 3 | 3 | 6 |
| Raw sector | 11 | 3 | 14 |

**Failure Pattern**: Write data corruption across all test suites:
- **Format**: Backup VBR byte 0 corrupted ($F5 instead of $EB); root directory volume label garbled
- **Multiblock**: 11,241 byte mismatches out of 32,768 bytes (34% corruption in 32KB write/readback)
- **Raw sector**: Write patterns don't survive readback verification
- **File ops**: Written file content doesn't match on readback; deleteFile/rename return FALSE despite succeeding
- **Seek**: Single byte value mismatch after seek+read (238 vs 220 expected)
- **MBR corruption**: After initCardOnly() warmup, MBR partition type changes from $0C to $06

**Root Cause Hypothesis** (REVISED after Lexar testing): Originally suspected card-specific SPI timing issue. **DISPROVEN** — Lexar card shows identical failure counts (see Card 4), proving these are **driver code bugs**, not card-specific write corruption. The Gigastone 8GB passing 100% was a false positive — its smaller filesystem layout avoided the bug.

---

### Card 4: Longsys/Lexar MSSD0 SDXC 58GB

**Designator**: Longsys/Lexar MSSD0 SDXC 58GB [FAT32] SD 6.x rev6.1 SN:$3354_9024 2024/11
**Card ID**: Class 10, U3, V30, SPI 25 MHz
**CID**: MID=$AD (Longsys/Lexar), OID="LS", PNM="MSSD0", PRV=6.1
**Total Sectors**: 122,257,216 (from freeSpace)
**Result**: **178/236 pass (75%)**
**Note**: Warmup REMOVED before this test run (warmup was causing SanDisk corruption)

| Test Suite | Pass | Fail | Total |
|---|---|---|---|
| Format | 41 | 5 | 46 |
| Mount | 21 | 0 | 21 |
| File ops | 14 | 7 | 22 |
| Read/write | 16 | 6+7 skip | 29 |
| Directory | 26 | 2 | 28 |
| Seek | 30 | 7 | 37 |
| Multicog | 13 | 1 | 14 |
| Multihandle | 8 | 11 | 19 |
| Multiblock | 1 | 5 | 6 |
| Raw sector | 12 | 2 | 14 |

**Failure Pattern**: Very similar to SanDisk SA16G — strongly suggests CODE BUGS, not card issues:
- **File ops**: 14/22 (identical to SanDisk) — readHandle content mismatch, deleteFile/rename return FALSE
- **Read/write**: 16/29 (identical to SanDisk) — written data doesn't match on readback
- **Multihandle**: 8/19 (identical to SanDisk) — write handle operations fail
- **Multiblock**: 10,416 byte mismatches in 32KB write/readback
- **Format**: 5 FSInfo failures (may be caused by download checksum error during test)
- **Seek**: 7 failures (worse than SanDisk's 1)

**Critical Observation**: The identical failure counts in file ops (14/22), read/write (16/29), and multihandle (8/19) across BOTH the SanDisk and Lexar cards — but NOT the Gigastone 8GB — strongly suggests these are **driver code bugs that are card-size or filesystem-layout dependent**, not card-specific write corruption issues. The Gigastone is 8GB while SanDisk is 16GB and Lexar is 64GB.

---

### Card 5: Samsung GD4QT 128GB (deferred)

**Designator**: Samsung GD4QT SDXC 128GB (tested in previous sessions)
**Status**: Deferred until stable cards pass cleanly

**Known Issues**:
1. **First write after initCardOnly() doesn't persist** — requires warmup (read/write/read of sector 0) to prime the write path
2. **Persistent error state** — after multiple failed format attempts, card enters unrecoverable write-failure state. CMD0/CMD8/ACMD41 reinit does NOT clear it. Physical power cycle (card removal/reinsertion) required.
3. **Write busy timeout** — card needs 500ms write timeout (default 250ms from CSD too short)
4. **CMD24 R1 response** — returns non-zero R1 that caused false failures; relaxed check to fatal-only ($64) mask

---

### Card 6: Gigastone SD16G SDHC 16GB (10x High Endurance)

**Designator**: Gigastone SD16G SDHC 14GB [FAT32] SD 3.x rev2.0 SN:$0000_03FB 2025/02
**Card ID**: Class 10, U1, V10, SPI 25 MHz
**CID**: MID=$00 (Gigastone/Budget OEM), OID="42", PNM="SD16G", PRV=2.0
**Total Sectors**: 31,207,424 (from CSD C_SIZE=30,475)
**Write Timeout**: 250ms (from CSD R2W_FACTOR=2)
**Result**: **236/236 pass (100%)**

| Test Suite | Pass | Fail | Total |
|---|---|---|---|
| Format | 46 | 0 | 46 |
| Mount | 21 | 0 | 21 |
| File ops | 22 | 0 | 22 |
| Read/write | 29 | 0 | 29 |
| Directory | 28 | 0 | 28 |
| Seek | 37 | 0 | 37 |
| Multicog | 14 | 0 | 14 |
| Multihandle | 19 | 0 | 19 |
| Multiblock | 6 | 0 | 6 |
| Raw sector | 14 | 0 | 14 |

**Significance**: This result **disproves the card-size hypothesis**. This 16GB card passes 100% while the SanDisk SA16G (also 16GB) fails at 81%. The failures are card-specific, not size-dependent.

---

## Key Finding: Failures Are Card-Specific, Not Size-Dependent

~~Previously hypothesized that failures were card-size dependent.~~ **DISPROVEN** by Budget OEM SD16G 16GB passing 100% — same size as the failing SanDisk SA16G 16GB.

| Test Suite | Gigastone 8GB | Gigastone 16GB | SanDisk SA16G 16GB | Lexar MSSD0 64GB |
|---|---|---|---|---|
| File ops | 22/22 | 22/22 | 14/22 | 14/22 |
| Read/write | 29/29 | 29/29 | 16/29 | 16/29 |
| Multihandle | 19/19 | 19/19 | 8/19 | 8/19 |
| Multiblock | 6/6 | 6/6 | 3/6 | 1/6 |
| Raw sector | 14/14 | 14/14 | 11/14 | 12/14 |
| **Total** | **256/256** | **236/236** | **192/236** | **178/236** |

**Pattern**: Passing cards are both Gigastone (MID=$00, SD 3.x). Failing cards are name-brand, higher-spec (SanDisk MID=$03 SD 5.x, Lexar MID=$AD SD 6.x). The identical failure counts (14/22, 16/29, 8/19) between SanDisk and Lexar prove a **deterministic trigger**, not random corruption.

**Possible card-specific triggers**:
- CMD6 (High Speed) switching behavior — higher-spec cards support this
- SPI write-busy timing differences between budget and name-brand cards
- Multi-block protocol behavior differences
- Different response patterns to certain commands
- Write data latching timing at 25 MHz SPI

## TX Streamer Bit-Shift Bug (Bug 3) — Investigation in Progress

**Date**: 2026-02-16
**Status**: ACTIVE — deep investigation with chip designer
**Detailed Investigation**: See [TX-TRANSITION-INVESTIGATION.md](Research/TX-TRANSITION-INVESTIGATION.md)
**Designer Questions**: See [DESIGNER-QUESTIONS-20260216.md](Research/DESIGNER-QUESTIONS-20260216.md)
**Hardware Reference**: See [P2-TT-BITS-REFERENCE.md](Research/P2-TT-BITS-REFERENCE.md) and [P2-SMARTPIN-LIVE-UPDATE-REFERENCE.md](Research/P2-SMARTPIN-LIVE-UPDATE-REFERENCE.md)

### Root Cause Discovery

Instrumented mount test on SanDisk SA16G revealed:
- Mount failure was **E_NOT_FAT32 (-22)**, NOT CMD13 or CRC
- CMD13 was clean ($0000), CRC was perfect (match=1, mismatch=0, retry=0)
- MBR data on card was **bit-shifted by 1 position** — partition type $06 instead of $0C
- The format utility's MBR readback had already detected this: `MBR READBACK MISMATCH! sig=$D52A type=$06`
- **100% reproducible** on SanDisk SA16G card

### Experiments Run

| # | Configuration | MBR Result | VBR Result |
|---|---|---|---|
| 1 | No init_phase, no delay | Right-shifted 1 bit ($D52A) | Correct ($AA55) |
| 2 | init_phase=$4000_0000, no delay | Right-shifted | Right-shifted (WORSE) |
| 3 | No init_phase, delay=2*spi_period | Left-shifted 1 bit ($54AB) | Left-shifted (WORSE) |
| 4 | init_phase=$4000_0000, delay=spi_period | **CORRECT ($AA55)** | Byte $1FE=$D5 not $55 |

### Current State (Experiment 4)

TX streamer PASM block now mirrors RX timing with reversed order:
```
RX (works): wypin → waitx(spi_period) → xinit(phase=$4000_0000)
TX (current): xinit(phase=$4000_0000) → waitx(spi_period) → wypin
```

- **Mount tests: 21/21 pass** — MBR is correct, VBR parsed OK despite sig byte error
- **File ops: 10/22 pass** — subsequent writes (directory, FAT entries) still corrupted
- **Volume label garbled** — "P2-[garbage]" instead of "P2-BENCH"

### Key Insight

The fix corrects the FIRST writeSector call but not subsequent ones. Between writes, the smart pin re-enable cycle (wrpin/wxpin/pinh to restore MOSI for sp_transfer_8) changes the pin state, causing the next streamer xinit to produce shifted data.

### Next Steps

1. **Read original V2 writeSector** from git (commit 288d051, lines ~5270-5320) — the V2 code had NO pinclear/pinl before streamer, NO init_phase, NO align_delay, yet worked on all cards
2. Understand what the V3 changes (pinclear/pinl, CMD13 checks, CRC retry) introduced that broke the TX timing
3. The original V2 likely let the smart pin stay configured during streamer TX, which may have provided correct timing accidentally

---

## Open Questions

1. **Why does first writeSector work but subsequent don't?** The pinclear/wrpin/wxpin/pinh cycle between writes changes MOSI pin state. Original V2 code may not have had pinclear before streamer.

2. **What SPI-level protocol difference triggers the failures?** Deep research needed into how the driver interacts differently with name-brand vs budget cards. Focus on CMD6, write responses, busy-wait behavior.

3. **Is the multiblock corruption related to the file-level failures?** SanDisk shows 11,241 mismatches, Lexar shows 10,416 — both ~32% corruption in 32KB. Could be same root cause or separate timing issue.

4. **Warmup status**: Removed from initCardOnly(). Samsung's first-write issue will need a different solution once the core bugs are fixed.

---

## Test Environment

- **Hardware**: Parallax Propeller 2 (P2X8C4M64P), Rev G silicon
- **Clock**: 350 MHz (sysclk), 25 MHz SPI clock
- **Connection**: SPI mode via smart pins + streamer
- **Compiler**: pnut-ts v1.52.1
- **Serial**: 2 Mbps via pnut-term-ts v0.9.6
- **Test Framework**: SD_RT_utilities.spin2 (10 test suites, 236 total tests)
