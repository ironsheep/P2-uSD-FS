# Card: Lexar Blue MicroSD XC 128GB

**Label:** Lexar PLAY 128GB microSD XC (Blue card)
**Unique ID:** `Unknown_MSSD0_6.1_34490F1E_202504`
**Test Date:** 2026-02-14 (characterization)

### Card Designator

```
Lexar MSSD0 SDXC 117GB [FAT32] SD 6.x rev6.1 SN:34490F1E 2025/04
Class 10, U3, V30, SPI 25 MHz  [formatted by P2FMTER]
```

### Raw Registers

```
CID: $C9 $4D $60 $4D $53 $53 $44 $30 $61 $34 $49 $0F $1E $A1 $94 $B1
CSD: $40 $0E $00 $32 $DB $79 $00 $03 $A8 $53 $7F $80 $0A $40 $00 $F3
OCR: $C0FF_8000
SCR: $02 $45 $84 $87 $00 $00 $00 $00
```

### CID Register (Card Identification) - All Fields

| Field | Bits | Raw | Value | Usage |
|-------|------|-----|-------|-------|
| MID | [127:120] | $C9 | Unknown (suspected BIWIN/Lexar) | **[USED]** |
| OID | [119:104] | $4D $60 | "M`" | [INFO] |
| PNM | [103:64] | $4D $53 $53 $44 $30 | "MSSD0" | [INFO] |
| PRV | [63:56] | $61 | 6.1 | [INFO] |
| PSN | [55:24] | $3449_0F1E | 877,465,374 | [INFO] |
| MDT | [19:8] | $194 | 2025-04 (April 2025) | [INFO] |
| CRC7 | [7:1] | $58 | $58 | [INFO] |

### CSD Register (Card Specific Data) - All Fields

| Field | Bits | Raw | Value | Usage |
|-------|------|-----|-------|-------|
| CSD_STRUCTURE | [127:126] | 1 | CSD Version 2.0 (SDHC/SDXC) | **[USED]** |
| TAAC | [119:112] | $0E | Read access time-1 | **[USED]** |
| NSAC | [111:104] | $00 | 0 CLK cycles | **[USED]** |
| TRAN_SPEED | [103:96] | $32 | 25 MHz max | **[USED]** |
| CCC | [95:84] | $DB7 | Classes 0,1,2,4,5,7,8,10,11 | [INFO] |
| READ_BL_LEN | [83:80] | 9 | 512 bytes | [INFO] |
| READ_BL_PARTIAL | [79] | 0 | Not allowed | [INFO] |
| WRITE_BLK_MISALIGN | [78] | 0 | Not allowed | [INFO] |
| READ_BLK_MISALIGN | [77] | 0 | Not allowed | [INFO] |
| DSR_IMP | [76] | 0 | DSR not implemented | [INFO] |
| C_SIZE | [69:48] | $3A853 | 239,699 (119,850 MB) | **[USED]** |
| ERASE_BLK_EN | [46] | 1 | 512-byte erase supported | [INFO] |
| SECTOR_SIZE | [45:39] | 127 | Erase sector = 64 KB | [INFO] |
| WP_GRP_SIZE | [38:32] | 0 | -- | [INFO] |
| WP_GRP_ENABLE | [31] | 0 | Disabled | [INFO] |
| R2W_FACTOR | [28:26] | 2 | Write = Read x 4 | **[USED]** |
| WRITE_BL_LEN | [25:22] | 9 | 512 bytes | [INFO] |
| WRITE_BL_PARTIAL | [21] | 0 | Not allowed | [INFO] |
| FILE_FORMAT_GRP | [15] | 0 | -- | [INFO] |
| COPY | [14] | 0 | Original | [INFO] |
| PERM_WRITE_PROTECT | [13] | 0 | Not protected | [INFO] |
| TMP_WRITE_PROTECT | [12] | 0 | Not protected | [INFO] |
| FILE_FORMAT | [11:10] | 0 | Hard disk-like | [INFO] |
| CRC7 | [7:1] | $79 | $79 | [INFO] |

**Derived Values:**
- Read Timeout: 100 ms (calculated)
- Write Timeout: 250 ms (calculated)
- Total Sectors: 245,452,800
- Capacity: ~117 GB

### OCR Register (Operating Conditions) - All Fields

| Field | Bits | Raw | Value | Usage |
|-------|------|-----|-------|-------|
| Power Up Status | [31] | 1 | Ready | [INFO] |
| CCS | [30] | 1 | SDHC/SDXC (block addressing) | **[USED]** |
| UHS-II Status | [29] | 0 | Not UHS-II | [INFO] |
| S18A | [24] | 0 | 3.3V only | [INFO] |
| 3.5-3.6V | [23] | 1 | Supported | [INFO] |
| 3.4-3.5V | [22] | 1 | Supported | [INFO] |
| 3.3-3.4V | [21] | 1 | Supported | [INFO] |
| 3.2-3.3V | [20] | 1 | Supported | [INFO] |
| 3.1-3.2V | [19] | 1 | Supported | [INFO] |
| 3.0-3.1V | [18] | 1 | Supported | [INFO] |
| 2.9-3.0V | [17] | 1 | Supported | [INFO] |
| 2.8-2.9V | [16] | 1 | Supported | [INFO] |
| 2.7-2.8V | [15] | 1 | Supported | [INFO] |

**OCR Value:** $C0FF_8000

### SCR Register (SD Configuration) - All Fields

| Field | Bits | Raw | Value | Usage |
|-------|------|-----|-------|-------|
| SCR_STRUCTURE | [63:60] | 0 | SCR Version 1.0 | [INFO] |
| SD_SPEC | [59:56] | 2 | SD Physical Layer 2.00+ | **[USED]** |
| DATA_STAT_AFTER_ERASE | [55] | 0 | Data = 0 after erase | [INFO] |
| SD_SECURITY | [54:52] | 4 | SDXC (security v3.xx) | [INFO] |
| SD_BUS_WIDTHS | [51:48] | $5 | 1-bit and 4-bit supported | [INFO] |
| SD_SPEC3 | [47] | 1 | SD 3.0 support: Yes | [INFO] |
| EX_SECURITY | [46:43] | 0 | No extended security | [INFO] |
| SD_SPEC4 | [42] | 1 | SD 4.0 support: Yes | [INFO] |
| SD_SPECX | [41:38] | 2 | SD 5.x/6.x/7.x indicator | [INFO] |
| CMD_SUPPORT | [33:32] | $00 | -- | [INFO] |

**SD Version:** 4.xx (SD_SPEC=2, SD_SPEC3=1, SD_SPEC4=1)

### Filesystem (FAT32 - formatted with P2FMTER)

| Field | Value |
|-------|-------|
| Factory Format | exFAT ($07) - reformatted to FAT32 |
| MBR Partition Type | $0C (FAT32 LBA) |
| VBR Sector | 8,192 |
| OEM Name | P2FMTER |
| Volume Label | P2-BENCH |
| Volume Serial | $04B4_1B51 |
| FS Type | FAT32 |
| Bytes/Sector | 512 |
| Sectors/Cluster | 64 (32 KB clusters) |
| Reserved Sectors | 32 |
| Number of FATs | 2 |
| Sectors per FAT | 29,962 |
| Root Cluster | 2 |
| Total Sectors | 245,444,608 |
| Data Region Start | Sector 59,956 |
| Total Clusters | 3,834,135 |

### Test Results

| Test | Status | Notes |
|------|--------|-------|
| Card Init | PASS | CMD0/CMD8/ACMD41 sequence successful |
| CID Read | PASS | Worker cog routing |
| CSD Read | PASS | Worker cog routing |
| SCR Read | PASS | Worker cog routing |
| OCR Read | PASS | Cached during init |
| MBR Read | PASS | exFAT partition detected (factory) |
| Format | PASS | FAT32 formatted with P2FMTER |
| Mount | PASS | Verified after format |

### Notes

- **MID $C9** - Not in standard manufacturer ID table; suspected **BIWIN** (Chinese OEM that manufactures Lexar-branded cards under license from Longsys)
- **Same product family** as Lexar 64GB (lexar-mssd0-64gb.md): identical PNM "MSSD0", PRV 6.1, CCC $DB7, SCR bytes 0-3
- **Different MID**: $C9 vs $AD (Longsys/Lexar) - different fab or updated MID assignment
- **Different OID**: "M`" ($4D $60) vs "LS" ($4C $53) - supports different manufacturer theory
- **CCC $DB7** includes Class 1 (stream read) and Class 11 (video speed class) - video-optimized card
- **SD 4.xx spec** compliant (SD_SPEC4=1)
- **CMD_SUPPORT $00** - neither CMD20 nor CMD23 (unlike the 64GB Lexar which had both)
- Factory formatted with exFAT (not FAT32)
- Very recent manufacture (April 2025)
- Needs FAT32 reformat before use with P2 SD driver
- DATA_STAT_AFTER_ERASE=0 (erased data reads as 0s)

### SPI Speed Characterization

**Test Date:** 2026-02-14
**Test Configuration:**
- SYSCLK: 200 MHz
- Phase 1: 1,000 single-sector reads (quick check)
- Phase 2: 10,000 single-sector reads (statistical confidence)
- Phase 3: 100 x 8-sector multi-block reads (800 sectors, sustained transfer)
- Total reads per speed level: 11,800 sector reads
- Test sectors: 1,000,000 to 1,010,000 (safe area away from FAT)
- CRC-16 verification on every read

**Results:**

| Target | Half Period | Actual | Delta | Phase 1 | Phase 2 | Phase 3 | Failure % |
|--------|-------------|--------|-------|---------|---------|---------|-----------|
| 18 MHz | 6 clocks | 16.6 MHz | -7.4% | 1,000 OK | 10,000 OK | 800 OK | **0.000%** |
| 20 MHz | 5 clocks | 20.0 MHz | +0.0% | 1,000 OK | 10,000 OK | 800 OK | **0.000%** |
| 22 MHz | 5 clocks | 20.0 MHz | -9.0% | 1,000 OK | 10,000 OK | 800 OK | **0.000%** |
| 25 MHz | 4 clocks | 25.0 MHz | +0.0% | 1,000 OK | 10,000 OK | 800 OK | **0.000%** |
| 27 MHz | 4 clocks | 25.0 MHz | -7.4% | timeout* | -- | -- | **100%** |

*At 27 MHz, CMD6 High Speed switch failed; card became unresponsive during Phase 1.

**Summary:**
- **Maximum Reliable Speed: 25 MHz** (47,200 total sector reads at 0% failure rate)
- CMD6 High Speed mode: Switch **failed** at 27 MHz, card became unresponsive
- Identical behavior to Lexar 64GB sibling and all other cards tested

### Internal Throughput (measured at 25 MHz SPI)

| Metric | Value |
|--------|-------|
| Phase 2 Duration (10,000 reads) | 4.28 seconds |
| Throughput | **1,196 KB/s** |
| Sectors/second | 2,335 |
| Latency per sector | 0.43 ms |

**Performance Class:** HIGH - **Fastest card tested** (13% faster than Lexar 64GB at 1,059 KB/s, 27% faster than Gigastone Camera Plus at 944 KB/s).

### Benchmark Results (Smart Pin SPI + Multi-Sector)

**Test Program**: SD_performance_benchmark.spin2 v2.0 | **SPI**: 25,000 kHz @ 350 MHz, 25,000 kHz @ 250 MHz

#### 350 MHz Run

**SysClk**: 350 MHz | **SPI**: 25,000 kHz | **Mount**: 232.7 ms

| Test | Min (us) | Avg (us) | Max (us) | KB/s |
|------|----------|----------|----------|------|
| **Raw Single-Sector** | | | | |
| Read 1x512B | 402 | 616 | 2,486 | **831** |
| Write 1x512B | 747 | 760 | 771 | **673** |
| **Raw Multi-Sector** | | | | |
| Read 8 sectors (4 KB) | 1,851 | 1,853 | 1,873 | **2,210** |
| Read 32 sectors (16 KB) | 6,876 | 6,878 | 6,903 | **2,382** |
| Read 64 sectors (32 KB) | 13,517 | 13,519 | 13,543 | **2,423** |
| Write 8 sectors (4 KB) | 2,188 | 2,207 | 2,225 | **1,855** |
| Write 32 sectors (16 KB) | 7,390 | 7,402 | 7,422 | **2,213** |
| Write 64 sectors (32 KB) | 14,406 | 14,422 | 14,431 | **2,272** |
| **File-Level** | | | | |
| File Write 512B | 5,183 | 5,605 | 5,686 | **91** |
| File Write 4 KB | 10,987 | 11,065 | 11,156 | **370** |
| File Write 32 KB | 53,385 | 54,185 | 56,307 | **604** |
| File Read 4 KB | 2,949 | 2,989 | 3,310 | **1,370** |
| File Read 32 KB | 22,821 | 22,869 | 23,205 | **1,432** |
| File Read 128 KB | 89,987 | 90,673 | 92,887 | **1,445** |
| File Read 256 KB | 179,945 | 181,539 | 183,086 | **1,444** |
| **Overhead** | | | | |
| File Open | 92 | 127 | 448 | — |
| File Close | 20 | 20 | 20 | — |
| Mount | — | 232,700 | — | — |

Multi-sector improvement: 64x single reads = 27,709 us vs 1x CMD18 = 13,526 us (**51% faster**)

#### 250 MHz Run

**SysClk**: 250 MHz | **SPI**: 25,000 kHz | **Mount**: 234.8 ms

| Test | Min (us) | Avg (us) | Max (us) | KB/s |
|------|----------|----------|----------|------|
| **Raw Single-Sector** | | | | |
| Read 1x512B | — | — | — | ERROR* |
| Write 1x512B | — | — | — | ERROR* |
| **Raw Multi-Sector** | | | | |
| Read 8 sectors (4 KB) | — | — | — | ERROR* |
| Read 32 sectors (16 KB) | — | — | — | ERROR* |
| Read 64 sectors (32 KB) | — | — | — | ERROR* |
| Write 8 sectors (4 KB) | — | — | — | ERROR* |
| Write 32 sectors (16 KB) | — | — | — | ERROR* |
| Write 64 sectors (32 KB) | — | — | — | ERROR* |
| **File-Level** | | | | |
| File Write 512B | 5,251 | 5,645 | 5,719 | **90** |
| File Write 4 KB | 11,472 | 11,573 | 12,015 | **353** |
| File Write 32 KB | 57,366 | 57,517 | 57,808 | **569** |
| File Read 4 KB | 3,412 | 3,457 | 3,821 | **1,184** |
| File Read 32 KB | 26,025 | 26,121 | 26,517 | **1,254** |
| File Read 128 KB | 102,767 | 103,531 | 105,813 | **1,266** |
| File Read 256 KB | 206,041 | 207,518 | 209,042 | **1,263** |
| **Overhead** | | | | |
| File Open | 137 | 177 | 540 | — |
| File Close | 28 | 28 | 29 | — |
| Mount | — | 234,800 | — | — |

Multi-sector improvement: 64x single reads = 33,241 us vs 1x CMD18 = 14,646 us (**55% faster**)

*\*Raw sector API errors were caused by missing CMD25 stuff byte bug (fixed in commit 58f6347). 250 MHz benchmark not yet re-run with fix.*

#### Sysclk Effect (350 vs 250 MHz)

| Test | 350 MHz (KB/s) | 250 MHz (KB/s) | Delta |
|------|----------------|----------------|-------|
| File Write 512B | 91 | 90 | +1.1% |
| File Write 4 KB | 370 | 353 | +4.8% |
| File Write 32 KB | 604 | 569 | +6.2% |
| File Read 4 KB | 1,370 | 1,184 | +15.7% |
| File Read 32 KB | 1,432 | 1,254 | +14.2% |
| File Read 128 KB | 1,445 | 1,266 | +14.1% |
| File Read 256 KB | 1,444 | 1,263 | +14.3% |

Reads show a significant **14-16% improvement** at 350 MHz vs 250 MHz, while writes show only 1-6% difference. Both produce exact 25.000 MHz SPI — the difference is entirely Spin2 driver overhead, which scales inversely with sysclk. Reads are more CPU-bound (FAT traversal, buffer management), while writes are card-controller-limited (flash programming latency dominates).

#### Cross-Card Comparison: Blue 128GB vs Red 64GB (both at 350 MHz / 25 MHz SPI)

Both cards tested at identical conditions: 350 MHz sysclk, 25,000 kHz SPI — true apples-to-apples comparison.

| Test | Blue 128GB (KB/s) | Red 64GB (KB/s) | Delta | Notes |
|------|-------------------|-----------------|-------|-------|
| **Raw Single-Sector** | | | | |
| Read 1x512B | 831 | 1,239 | -33% | 64GB has much lower latency |
| Write 1x512B | 673 | 677 | 0% | Identical |
| **Raw Multi-Sector** | | | | |
| Read 64 sectors (32 KB) | 2,423 | 2,379 | +2% | Nearly identical at bulk |
| Write 64 sectors (32 KB) | 2,272 | 2,248 | +1% | Nearly identical at bulk |
| **File-Level** | | | | |
| File Write 32 KB | 604 | 469 | +29% | 128GB significantly faster |
| File Read 4 KB | 1,370 | 1,301 | +5% | |
| File Read 256 KB | 1,444 | 1,531 | -6% | 64GB slightly faster |
| **Overhead** | | | | |
| Mount | 232.7 ms | 358.0 ms | -35% | 128GB mounts faster |
| File Open | 127 us | 149 us | -15% | 128GB opens faster |

**Key observations:**
- **Single-sector read latency**: 64GB is 33% faster (413 vs 616 us avg). The 128GB has higher first-read variance (Max=2,486 us), suggesting longer internal access time for its larger flash array.
- **Bulk transfer**: At the same SPI speed, both cards deliver nearly identical multi-sector throughput (~2,380 KB/s read, ~2,260 KB/s write). The SPI bus is the bottleneck, not the flash.
- **File-level writes**: 128GB is 29% faster at 32 KB writes — likely better flash controller parallelism or larger write buffers in the larger card.
- **File-level reads at 256 KB**: 64GB is 6% faster, possibly due to its faster single-sector access benefiting the FAT lookup overhead between clusters.
- **Same product family**: Both are Lexar MSSD0 PRV 6.1 with identical CCC, CSD timing, and SCR capabilities. The 128GB uses a different MID ($C9 vs $AD) and OID ("M`" vs "LS"), suggesting a different fab or OEM partner.

### Regression Test Results

**Test Date:** 2026-02-14 | **Driver:** SD_card_driver.spin2 (smart pin SPI + worker cog)

| Test Suite | 270 MHz | 350 MHz | Notes |
|------------|---------|---------|-------|
| Mount (21 tests) | **21/21 PASS** | **21/21 PASS** | |
| File Ops (22 tests) | **22/22 PASS** | **22/22 PASS** | |
| Read/Write (29 tests) | **29/29 PASS** | **29/29 PASS** | |
| Directory (28 tests) | **28/28 PASS** | **28/28 PASS** | |
| Seek (37 tests) | **37/37 PASS** | **37/37 PASS** | |
| Multicog (14 tests) | **14/14 PASS** | **14/14 PASS** | |
| Multihandle (19 tests) | **19/19 PASS** | **19/19 PASS** | |
| Multiblock (6 tests) | **6/6 PASS** | **6/6 PASS** | |
| Raw Sector (14 tests) | **14/14 PASS** | **14/14 PASS** | |
| Format (46 tests) | **46/46 PASS** | **46/46 PASS** | |

All tests pass at both clock speeds after fixing CMD25 missing stuff byte and do_close spurious write bugs.
