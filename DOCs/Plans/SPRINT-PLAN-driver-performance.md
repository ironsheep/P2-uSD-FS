# Sprint Plan: SD Card Driver Performance & Features

**Sprint Goal**: Transform the SD card driver into an adaptive, high-performance, feature-rich implementation

**Status**: ACTIVE

**Themes**: Adaptive timing, sysclk-independent performance, multiple file handles, FAT optimization

---

## Progress Journal

### 2026-01-26: Execution Order Revised & Full Scope Defined
- **DECISION**: All phases (1-6) must complete before driver release
- **ORDER CHANGE**: Phase 4 (File Handles) moved before Phase 5
  - Rationale: Multi-cog tests already exist but fail due to single-file limitation
  - Adding handles FIXES tests, doesn't require rewriting them
- **Current work**: Phase 1 completion (restore streamer, apply bug fixes, add CMD13 verification)
- **CMD13 addition**: All write operations will verify success via SEND_STATUS command
- **Bug fixes identified**: Operator precedence (`@buf + x & 511` → `@buf + (x & 511)`), cache invalidation, end-of-chain detection
- **See**: `/Users/stephen/.claude/plans/validated-baking-bumblebee.md` for detailed execution plan

### 2026-01-24: REV Instruction Bug Discovery & Fix
- **CRITICAL BUG FOUND**: `rev` followed by `shr #24` was destroying received data
- **Root cause**: After rdpin (data in bits [31:24]), REV moves byte to bits [7:0] AND reverses bit order. The subsequent `shr #24` shifted the data away to zero!
- **Fix applied**: Removed erroneous `shr #24` from 3 locations in SD_card_driver_v2.spin2:
  1. readSectors token wait (line ~2049)
  2. writeSectors data response (line ~2421)
  3. writeSectors busy loop (line ~2473)
- **Correct pattern**: `rdpin -> rev -> done` (byte now in [7:0], MSB-first)
- **P2KB reference**: p2kbArchSmartPin11101SyncSerialReceive confirms left-justified data
- **Current state**: Card initializes OK but mount fails reading MBR with "Error token"
- **Observation**: sp_transfer_8 shows raw=$0000_0000 during some transfers (MISO reading zeros when should be $FF)
- **Next steps**: Logic analyzer investigation to verify actual signal vs smart pin sampling

### 2026-01-23: Phase 1 Core Complete
- **Verified**: Streamer-based 512-byte DMA sector reads working on P2 Edge card
- **Commit**: "Add streamer-based readSector with correct NCO phase alignment"
- **Evidence**: All 17 mount tests pass with streamer DMA
- **Key fixes applied**:
  - xfrq calculation: `$4000_0000/spi_period` (was incorrectly doubled)
  - init_phase offset: `$8000_0000 - xfrq` for immediate first sample
  - Correct sequence: wypin(clk) -> waitx #3 -> xinit(mode, init_phase)
- **Hardware**: P2 Edge card socket (pins 58-61)
- **Next**: Full regression test suite to verify complete system

---

## Overview

This plan consolidates all performance and feature enhancements into a phased approach. Each phase builds on the previous, allowing incremental testing and validation.

### Design Principles

1. **Adaptive**: Driver configures itself based on card capabilities (TRAN_SPEED, brand)
2. **Performant**: Achieve 25-50 MHz SPI independent of system clock
3. **Rich**: Support multiple simultaneous open files for true multi-cog operation
4. **Robust**: Handle problematic cards (PNY) gracefully with automatic speed limiting

### Target Performance

| Metric | Current | Target |
|--------|---------|--------|
| SPI Clock (reads) | ~4.4 MHz | 25-50 MHz |
| SPI Clock (writes) | ~10 MHz | 25-50 MHz |
| Sysclk Independence | No (scales with clkfreq) | Yes (smart pins) |
| Open Files | 1 | 4-8 |
| Unmount Time (16GB) | ~10 seconds | <1 second |
| PNY Compatibility | Manual 20MHz cap | Auto-detect, auto-limit |

---

## Phase 1: Smart Pin SPI Implementation

### Goal
Replace bit-banged SPI with smart pins for sysclk-independent operation. This is the foundation for all speed improvements.

### Background

Current bit-bang timing at 80 MHz sysclk (from P2KB analysis):
- `transfer()` function: 18 cycles/bit = ~4.4 MHz
- Fast write path: 8 cycles/bit = ~10 MHz

These scale with sysclk, so HDMI systems at 270 MHz get different SD performance than 320 MHz systems.

Smart pins solve this by using hardware timing independent of instruction execution.

### Smart Pin Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                     SPI via Smart Pins                          │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  SCK Pin (P_TRANSITION mode)                                    │
│    - Generates clock at programmed frequency                    │
│    - X register sets period: freq = sysclk / (X * 2)            │
│    - Y register sets transition count (bits * 2)                │
│                                                                 │
│  MOSI Pin (P_SYNC_TX mode)                                      │
│    - Transmits data synchronized to SCK                         │
│    - Clocked by SCK pin (B-input)                               │
│    - LSB-first, needs REV for SD card MSB-first                 │
│                                                                 │
│  MISO Pin (P_SYNC_RX mode)                                      │
│    - Receives data synchronized to SCK                          │
│    - Clocked by SCK pin (B-input)                               │
│    - Data left-justified, needs shift                           │
│                                                                 │
│  CS Pin (standard GPIO)                                         │
│    - Manual control via PINH/PINL                               │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### Speed Calculation

To achieve target SPI frequency:
```spin2
X := clkfreq / (target_spi_freq * 2)
```

| sysclk | X for 25 MHz | Actual | X for 50 MHz | Actual |
|--------|--------------|--------|--------------|--------|
| 320 MHz | 6 | 26.7 MHz | 3 | 53 MHz |
| 270 MHz | 5 | 27 MHz | 3 | 45 MHz |
| 200 MHz | 4 | 25 MHz | 2 | 50 MHz |

### Tasks

- [x] **1.1** Create smart pin initialization for SPI ✓ COMPLETE
  - Configure SCK as P_TRANSITION
  - Configure MOSI as P_SYNC_TX with SCK as B-input
  - Configure MISO as P_SYNC_RX with SCK as B-input
  - CS remains standard GPIO

- [x] **1.2** Implement `smartpin_transfer()` function ✓ COMPLETE
  - `sp_transfer_8()` - smart pin byte transfer
  - Handle MSB-first via smart pin configuration
  - Return received data for reads

- [x] **1.3** Implement `smartpin_readSector()` ✓ COMPLETE (P2 Edge verified)
  - 512-byte bulk read using streamer DMA
  - Wait for data token (0xFE) via sp_transfer_8
  - Streamer: WRFAST + XINIT with correct NCO phase alignment
  - Handle CRC bytes

- [x] **1.4** Implement `smartpin_writeSector()` ✓ COMPLETE (needs verification)
  - Send data token, 512 bytes via streamer, CRC
  - Wait for data response (0x05)
  - Wait for busy complete (0xFF) - PNY-compatible 500ms timeout

- [x] **1.5** Add speed configuration function ✓ COMPLETE
  - `setSPISpeed(frequency)` calculates half-period
  - Store current speed for reference
  - Validate against card's TRAN_SPEED (Phase 2)

- [x] **1.6** Update `initCard()` to use smart pins ✓ COMPLETE
  - Start at 400 kHz for init sequence
  - Switch to 25 MHz after ACMD41
  - Recovery flush (4096 clocks) for stuck transfers

- [ ] **1.7** Regression test all existing tests with smart pin implementation
  - Mount tests: 17/17 PASS (P2 Edge)
  - File ops tests: PENDING
  - Read/write tests: PENDING
  - Directory tests: PENDING
  - Seek tests: PENDING

### Success Criteria
- All regression tests pass
- SPI speed measured at 25+ MHz (logic analyzer)
- Same performance at 270 MHz and 320 MHz sysclk

---

## Phase 2: Card Configuration & Adaptive Timing

### Goal
Read card capability registers and configure driver behavior automatically. Detect problematic brands and apply appropriate speed limits.

### Card Registers

#### CSD TRAN_SPEED (byte 3)
```
Bits [6:3] = Time value multiplier
Bits [2:0] = Transfer rate unit

Common values:
  0x32 = 25 MHz (Default Speed) - all cards support this
  0x5A = 50 MHz (High Speed capable)
```

#### CID Manufacturer ID (byte 0)
```
Known IDs:
  0x03 = SanDisk
  0x1B = Panasonic
  0x27 = Phison (PNY uses these controllers)
  0x28 = Lexar
  0x41 = Kingston
  0x74 = Transcend
  0x9F = Phison (alternate)
```

### Brand Detection Logic

```spin2
PRI detectCardBrand() : brand_flags | mid, cid[4]
  '' Read CID and determine brand-specific handling

  readCID(@cid)
  mid := cid.byte[0]

  case mid
    $27, $9F:                           ' Phison controller (PNY, etc.)
      brand_flags := BRAND_SPEED_LIMITED
      max_spi_speed := 20_000_000       ' Cap at 20 MHz
      debug("  [detectCardBrand] Phison/PNY detected - limiting to 20MHz")

    $03:                                ' SanDisk
      brand_flags := BRAND_RELIABLE

    $41:                                ' Kingston
      brand_flags := BRAND_RELIABLE

    other:
      brand_flags := BRAND_UNKNOWN
```

### Timeout Configuration

| Card Type | Read Timeout | Write Timeout | Source |
|-----------|--------------|---------------|--------|
| SDSC | TAAC + NSAC from CSD | Read × R2W_FACTOR | CSD fields |
| SDHC/SDXC | 100 ms (fixed) | 250 ms (fixed) | SD Spec mandate |

### Tasks

- [ ] **2.1** Implement TRAN_SPEED extraction from CSD
  - Parse byte 3 of CSD register
  - Decode time value and rate unit
  - Store as `card_max_speed` in Hz

- [ ] **2.2** Implement brand detection from CID
  - Read CID at mount time (already have `readCIDRaw()`)
  - Extract MID (byte 0)
  - Set brand flags and speed limits

- [ ] **2.3** Create adaptive speed selection
  ```spin2
  PRI selectSPISpeed() : speed
    '' Choose SPI speed based on card capability and brand
    speed := card_max_speed              ' From TRAN_SPEED
    if brand_flags & BRAND_SPEED_LIMITED
      speed := speed <# 20_000_000       ' Cap for problematic cards
    setSPISpeed(speed)
  ```

- [ ] **2.4** Implement timeout configuration
  - Calculate timeouts from CSD for SDSC cards
  - Use fixed timeouts for SDHC/SDXC
  - Apply to read/write operations

- [ ] **2.5** Add card info logging at mount
  - Log manufacturer, product name, revision
  - Log detected speed capability
  - Log any brand-specific handling applied

- [ ] **2.6** Test with PNY card
  - Verify auto-detection works
  - Verify 20 MHz speed limit applied
  - Verify all operations still work

- [ ] **2.7** Test with SanDisk/Kingston cards
  - Verify full speed (25-50 MHz) used
  - Verify no false positive speed limiting

### Success Criteria
- PNY cards automatically detected and speed-limited
- Good cards run at full TRAN_SPEED capability
- Card info logged at mount time

---

## Phase 3: High-Speed Operations

### Goal
Implement CMD6 for High Speed mode switching and multi-sector transfers for maximum throughput.

### CMD6 (SWITCH_FUNC)

High Speed mode doubles the maximum clock from 25 MHz to 50 MHz.

```
Query support:  CMD6 with arg = 0x00FFFFF1 (Check Function)
Switch to HS:   CMD6 with arg = 0x80FFFFF1 (Switch Function)
Response:       512-bit (64-byte) status structure
```

### Multi-Sector Commands

| Command | Name | Description |
|---------|------|-------------|
| CMD18 | READ_MULTIPLE_BLOCK | Read sectors until CMD12 (STOP) |
| CMD25 | WRITE_MULTIPLE_BLOCK | Write sectors until Stop Token |
| CMD12 | STOP_TRANSMISSION | Stop multi-block operation |

#### Multi-Read Protocol (CMD18)
```
1. Send CMD18 with starting sector
2. For each sector:
   - Wait for data token (0xFE)
   - Read 512 bytes
   - Read 2 CRC bytes
3. Send CMD12 to stop
4. Wait for card ready (0xFF)
```

#### Multi-Write Protocol (CMD25)
```
1. Send CMD25 with starting sector
2. For each sector:
   - Send data token (0xFC for multi-block)
   - Send 512 bytes
   - Send 2 CRC bytes
   - Wait for data response
   - Wait for busy complete
3. Send Stop Token (0xFD)
4. Wait for card ready (0xFF)
```

### Tasks

- [ ] **3.1** Implement CMD6 query function
  - `queryHighSpeed()` - Check if card supports HS mode
  - Parse 64-byte response structure
  - Return true if Function Group 1, Function 1 supported

- [ ] **3.2** Implement CMD6 switch function
  - `switchHighSpeed()` - Switch to 50 MHz mode
  - Verify switch successful from response
  - Update `card_max_speed` to 50 MHz

- [ ] **3.3** Integrate HS mode into mount sequence
  - Query HS support after basic init
  - Switch to HS if supported and not brand-limited
  - Set SPI speed to 50 MHz

- [ ] **3.4** Implement CMD18 multi-sector read
  - `readSectors(start_sector, count, p_buffer)`
  - Use CMD18 for count > 1, CMD17 for single
  - Handle CMD12 stop transmission

- [ ] **3.5** Implement CMD25 multi-sector write
  - `writeSectors(start_sector, count, p_buffer)`
  - Use CMD25 for count > 1, CMD24 for single
  - Use 0xFC token for multi-block data
  - Send 0xFD stop token at end

- [ ] **3.6** Update file read to use multi-sector
  - Calculate contiguous sectors in cluster chain
  - Use CMD18 for sequential reads
  - Fall back to CMD17 for fragmented reads

- [ ] **3.7** Update file write to use multi-sector
  - Pre-allocate cluster chain when possible
  - Use CMD25 for sequential writes
  - Handle cluster boundaries

- [ ] **3.8** Create throughput benchmark test
  - Sequential read: 1 MB in 512-byte blocks vs multi-sector
  - Sequential write: 1 MB comparison
  - Report MB/s achieved

### Success Criteria
- CMD6 successfully switches capable cards to 50 MHz
- Multi-sector reads achieve near-theoretical throughput
- Multi-sector writes achieve near-theoretical throughput
- Benchmark shows significant improvement over single-sector

---

## Phase 4: Multiple File Handles

### Goal
Support 4-8 simultaneously open files for true multi-cog file independence.

### Current Limitation

The driver currently supports only ONE open file. From multi-cog test comments:
> "Due to driver limitation (one open file at a time), interleaved open/read/close operations may fail when another cog's openFile() resets file state."

### Handle Table Design

```spin2
CON
  MAX_OPEN_FILES = 8

DAT
  ' File handle table - each entry tracks one open file
  file_handles
    fh_flags      BYTE    0[MAX_OPEN_FILES]     ' Flags: in_use, read, write, dirty
    fh_cluster    LONG    0[MAX_OPEN_FILES]     ' Current cluster
    fh_position   LONG    0[MAX_OPEN_FILES]     ' Position within file
    fh_size       LONG    0[MAX_OPEN_FILES]     ' File size in bytes
    fh_dir_sec    LONG    0[MAX_OPEN_FILES]     ' Directory sector (for updates)
    fh_dir_idx    BYTE    0[MAX_OPEN_FILES]     ' Directory entry index
    fh_start_clus LONG    0[MAX_OPEN_FILES]     ' Starting cluster
```

### API Changes

```spin2
' Current API (single file):
PUB openFile(name_ptr) : result
PUB closeFile()
PUB read(p_buffer, count) : bytes_read
PUB write(p_buffer, count) : bytes_written

' New API (multiple files):
PUB openFile(name_ptr) : handle            ' Returns handle 0-7, or -1 on error
PUB closeFile(handle) : result
PUB read(handle, p_buffer, count) : bytes_read
PUB write(handle, p_buffer, count) : bytes_written
PUB seek(handle, position) : result
PUB fileSize(handle) : size
PUB filePosition(handle) : position
```

### Sector Buffer Strategy

With multiple open files, we need a buffer strategy:

**Option A: Single shared buffer**
- Simplest implementation
- Must reload sector when switching between files
- Acceptable for most use cases

**Option B: Per-handle buffer pool**
- 2-4 shared buffers
- LRU replacement
- Better for interleaved access
- Higher RAM usage (1-2 KB)

**Recommended: Option A initially**, upgrade to B if needed.

### Tasks

- [ ] **4.1** Define handle table structure in DAT
  - Allocate arrays for all handle fields
  - Initialize all handles to "not in use"

- [ ] **4.2** Implement handle allocation/deallocation
  - `allocHandle()` - Find free handle, mark in use
  - `freeHandle(h)` - Mark handle as free

- [ ] **4.3** Refactor `do_open()` to use handles
  - Allocate handle on open
  - Store file state in handle table
  - Return handle ID (0-7)

- [ ] **4.4** Refactor `do_close()` to use handles
  - Flush any dirty data
  - Update directory entry if modified
  - Free the handle

- [ ] **4.5** Refactor `do_read()` to use handles
  - Load file state from handle table
  - Track position in handle
  - Handle sector buffer sharing

- [ ] **4.6** Refactor `do_write()` to use handles
  - Load file state from handle table
  - Mark handle dirty on write
  - Update position and size in handle

- [ ] **4.7** Refactor `do_seek()` to use handles
  - Validate handle
  - Update position in handle table
  - Recalculate current cluster if needed

- [ ] **4.8** Update public API methods
  - Add handle parameter to all file operations
  - Validate handle on each call
  - Return appropriate errors for invalid handles

- [ ] **4.9** Update worker cog command dispatch
  - Pass handle through parameter block
  - Route to correct handle's state

- [ ] **4.10** Backward compatibility wrapper (optional)
  - Single-file API using handle 0
  - For existing code migration

### Success Criteria
- Can open 8 files simultaneously
- Each file maintains independent position
- Interleaved read/write operations work correctly
- No data corruption between files

---

## Phase 5: FAT Performance Optimization

### Goal
Eliminate the ~10 second unmount delay by tracking free clusters incrementally instead of scanning the entire FAT.

### Current Problem

`updateFSInfo()` calls `countFreeClusters()` which scans the entire FAT:

```spin2
PRI countFreeClusters() : count | fat_idx, entry
  count := 0
  fat_idx := 8                              ' Start at cluster 2
  repeat
    if fat_idx & 511 == 0                   ' Sector boundary
      if fat_idx >> 9 >= sec_per_fat        ' Past end of FAT
        quit
      readSector(fat_sec + fat_idx >> 9)    ' Read FAT sector
    entry := long[@buf + fat_idx & 511]
    if (entry & $0FFF_FFFF) == 0            ' Free cluster
      count++
    fat_idx += 4
```

For a 16GB card: ~1M clusters = ~8000 FAT sectors to read = ~10 seconds.

### Solution: Incremental Tracking

The FSInfo sector already caches:
- `FSI_Free_Count` (offset 488) - Number of free clusters
- `FSI_Nxt_Free` (offset 492) - Hint for next free cluster

**Strategy:**
1. Trust FSInfo values at mount (already doing this)
2. Decrement `fsi_free_count` when allocating clusters
3. Increment `fsi_free_count` when freeing clusters
4. Update `fsi_nxt_free` hint during allocation
5. On unmount, just write cached values (no scan)
6. Only scan if `fsi_free_count == $FFFF_FFFF` (invalid/unknown)

### Tasks

- [ ] **5.1** Add cluster allocation tracking
  - In `allocCluster()`: decrement `fsi_free_count`
  - Update `fsi_nxt_free` to allocated+1
  - Mark FSInfo dirty

- [ ] **5.2** Add cluster deallocation tracking
  - In `freeCluster()`: increment `fsi_free_count`
  - Optionally update `fsi_nxt_free` if freed < current
  - Mark FSInfo dirty

- [ ] **5.3** Refactor `updateFSInfo()`
  - Remove call to `countFreeClusters()`
  - Just write cached `fsi_free_count` and `fsi_nxt_free`
  - Only scan if values are invalid ($FFFF_FFFF)

- [ ] **5.4** Add FSInfo validation option
  - `validateFreeCount()` - Optional full scan
  - Compare with cached value
  - For debugging/verification

- [ ] **5.5** Handle edge cases
  - What if FSInfo was invalid at mount? Flag it, scan on unmount.
  - What if allocation fails mid-chain? Ensure count stays accurate.
  - Format operation should reset count.

- [ ] **5.6** Test unmount performance
  - Measure unmount time before/after
  - Target: < 1 second vs current ~10 seconds

### Success Criteria
- Unmount completes in < 1 second on 16GB card
- Free space reporting remains accurate
- No data corruption after repeated mount/unmount cycles

---

## Phase 6: Enhanced Multi-Cog Testing

### Goal
Update multi-cog regression tests to exercise multiple file handles and verify true file independence.

### Current Test Limitations

From `SD_RT_multicog_tests.spin2`:
- Tests acknowledge single-file limitation
- Concurrent tests expect some failures due to shared state
- Cannot test true parallel file access

### New Test Scenarios

#### 6.1 Independent File Access
```
Cog 0: Opens FILE_A, reads 1KB, closes
Cog 1: Opens FILE_B, reads 1KB, closes
Cog 2: Opens FILE_C, reads 1KB, closes
Cog 3: Opens FILE_D, reads 1KB, closes

All should succeed simultaneously (different handles).
```

#### 6.2 Interleaved Operations
```
Cog 0: Open A, read 100 bytes, [pause]
Cog 1: Open B, read 100 bytes, [pause]
Cog 0: Read 100 more bytes from A
Cog 1: Read 100 more bytes from B

File positions must be independent.
```

#### 6.3 Mixed Read/Write
```
Cog 0: Reading from FILE_A
Cog 1: Writing to FILE_B
Cog 2: Reading from FILE_C
Cog 3: Writing to FILE_D

All operations complete without interference.
```

#### 6.4 Handle Exhaustion
```
Open 8 files (max handles)
Attempt to open 9th file
Verify graceful failure (returns -1)
Close one file
Open new file succeeds
```

#### 6.5 Stress Test with Handles
```
4 cogs, each:
  - Opens unique file
  - Performs 100 read/write cycles
  - Closes file
  - Repeats 10 times

Verify: No crashes, no data corruption, all handles released.
```

### Tasks

- [ ] **6.1** Update test file to remove single-file workarounds
  - Remove comments about "expected failures"
  - Each worker uses its own file handle

- [ ] **6.2** Implement independent file access test
  - 4 workers open different files
  - All read simultaneously
  - Verify all succeed

- [ ] **6.3** Implement interleaved operations test
  - Workers interleave reads on different handles
  - Verify file positions are independent

- [ ] **6.4** Implement mixed read/write test
  - Some workers read, others write
  - Different files, simultaneous operations

- [ ] **6.5** Implement handle exhaustion test
  - Open max files, verify 9th fails
  - Close and reopen, verify success

- [ ] **6.6** Implement stress test with handles
  - Extended duration test
  - Verify handle cleanup
  - Check for memory/resource leaks

- [ ] **6.7** Add data integrity verification
  - Write known patterns
  - Read back and verify
  - Detect any cross-file corruption

### Success Criteria
- All multi-cog tests pass with 0 expected failures
- Handles properly isolated between cogs
- No data corruption under concurrent access
- Clean resource cleanup on test completion

---

## Implementation Order & Dependencies

```
Phase 1: Smart Pin SPI (restore streamer, bug fixes, CMD13)
    │
    └──► Phase 4: Multiple File Handles (unblocks multi-cog tests)
              │
              └──► Phase 5: FAT Optimization (fast unmount)
                        │
                        └──► Phase 2: Card Configuration (auto-detect)
                                  │
                                  └──► Phase 3: High-Speed Operations (50MHz, file-level multi-sector)
                                            │
                                            └──► Phase 6: Enhanced Multi-Cog Testing (final validation)
```

**REVISED execution order (2026-01-26):**
1. Phase 1 (Smart Pins) - Fix regressions, add CMD13 verification
2. **Phase 4 (File Handles) - MOVED UP: Multi-cog tests exist but fail without this**
3. Phase 5 (FAT Optimization) - Fix unacceptable 10-second unmount
4. Phase 2 (Card Configuration) - Auto-detect card capabilities
5. Phase 3 (High-Speed Ops) - Maximize performance
6. Phase 6 (Multi-Cog Tests) - Final validation
7. 270 MHz Validation - Sysclk independence verification

**Rationale for change:** Phase 4 was moved before Phase 5 because multi-cog tests
are already written correctly but cannot pass due to single-file limitation. Adding
file handles FIXES existing tests rather than requiring new tests to be written.

---

## Risk Assessment

| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| Smart pin timing edge cases | Medium | High | Extensive testing, fallback to bit-bang |
| Multi-sector protocol errors | Medium | High | Start with single card, expand testing |
| Handle table memory pressure | Low | Medium | 8 handles = ~300 bytes, acceptable |
| FAT count drift over time | Low | High | Periodic validation option |
| PNY detection false positives | Low | Medium | Conservative MID matching |

---

## Testing Requirements

### Hardware
- P2 Edge module with SD card slot
- Logic analyzer for SPI speed verification
- Multiple SD card brands (SanDisk, Kingston, PNY, Gigastone)

### Test Cards
- SanDisk Ultra (High Speed capable)
- Kingston Canvas (High Speed capable)
- PNY 16GB (known problematic - speed limiting)
- Gigastone 32GB (baseline testing)
- Older SDSC card if available

### Benchmarks
- Single-sector read/write throughput (baseline)
- Multi-sector read/write throughput (improvement)
- Mount/unmount time
- Free space calculation time

---

## References

- SD Physical Layer Simplified Specification v9.10
- P2 Silicon Documentation - Smart Pins
- P2KB MCP - Instruction timing, smart pin modes
- DOCs/Research/PNY-MICROSD-SPI-ISSUES.md
- Previous sprint plans (archived)

---

*Document created: 2026-01-20*
*Status: ACTIVE*
*Phases: 6*
