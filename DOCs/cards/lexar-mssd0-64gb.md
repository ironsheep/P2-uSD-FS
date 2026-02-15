# Card: Lexar Red MicroSD XC V30 U3 64GB

**Label:** Lexar V30 U3 64GB microSD XC (Red card)
**Unique ID:** `Longsys/Lexar_MSSD0_6.1_31899471_202411`
**Test Date:** 2026-02-02 (characterization)

### Card Designator

```
Lexar MSSD0 SDXC 58GB [exFAT] SD 6.x rev6.1 SN:31899471 2024/11
Class 10, U3, V30, SPI 25 MHz
```

### Raw Registers

```
CID: $AD $4C $53 $4D $53 $53 $44 $30 $61 $31 $89 $94 $71 $71 $8B $7D
CSD: $40 $0E $00 $32 $DB $79 $00 $01 $D2 $67 $7F $80 $0A $40 $00 $59
OCR: $C0FF_8000
SCR: $02 $45 $84 $87 $33 $33 $30 $39
```

### CID Register (Card Identification) - All Fields

| Field | Bits | Raw | Value | Usage |
|-------|------|-----|-------|-------|
| MID | [127:120] | $AD | Longsys/Lexar | **[USED]** |
| OID | [119:104] | $4C $53 | "LS" (Longsys/Lexar) | [INFO] |
| PNM | [103:64] | $4D $53 $53 $44 $30 | "MSSD0" | [INFO] |
| PRV | [63:56] | $61 | 6.1 | [INFO] |
| PSN | [55:24] | $3189_9471 | 830,604,401 | [INFO] |
| MDT | [19:8] | $18B | 2024-11 (November 2024) | [INFO] |
| CRC7 | [7:1] | $3E | $3E | [INFO] |

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
| C_SIZE | [69:48] | $1D267 | 119,399 (59,700 MB) | **[USED]** |
| ERASE_BLK_EN | [46] | 1 | 512-byte erase supported | [INFO] |
| SECTOR_SIZE | [45:39] | 127 | Erase sector = 64 KB | [INFO] |
| WP_GRP_SIZE | [38:32] | 0 | — | [INFO] |
| WP_GRP_ENABLE | [31] | 0 | Disabled | [INFO] |
| R2W_FACTOR | [28:26] | 2 | Write = Read × 4 | **[USED]** |
| WRITE_BL_LEN | [25:22] | 9 | 512 bytes | [INFO] |
| WRITE_BL_PARTIAL | [21] | 0 | Not allowed | [INFO] |
| FILE_FORMAT_GRP | [15] | 0 | — | [INFO] |
| COPY | [14] | 0 | Original | [INFO] |
| PERM_WRITE_PROTECT | [13] | 0 | Not protected | [INFO] |
| TMP_WRITE_PROTECT | [12] | 0 | Not protected | [INFO] |
| FILE_FORMAT | [11:10] | 0 | Hard disk-like | [INFO] |
| CRC7 | [7:1] | $2C | $2C | [INFO] |

**Derived Values:**
- Read Timeout: 100 ms (calculated)
- Write Timeout: 250 ms (calculated)
- Total Sectors: 122,265,600
- Capacity: ~58 GB

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
| CMD_SUPPORT | [33:32] | $03 | CMD20+CMD23 supported | [INFO] |

**SD Version:** 4.xx (SD_SPEC=2, SD_SPEC3=1, SD_SPEC4=1)

### Filesystem (Factory - exFAT)

| Field | Value |
|-------|-------|
| MBR Partition Type | $07 (exFAT/NTFS) |
| Factory Format | exFAT (default for 64GB+) |

**Note:** Card ships with exFAT filesystem. Needs FAT32 format for P2 SD driver compatibility.

### Test Results

| Test | Status | Notes |
|------|--------|-------|
| Card Init | PASS | CMD0/CMD8/ACMD41 sequence successful |
| CID Read | PASS | Worker cog routing |
| CSD Read | PASS | Worker cog routing |
| SCR Read | PASS | Worker cog routing |
| OCR Read | PASS | Cached during init |
| MBR Read | PASS | exFAT partition detected |
| Mount | N/A | Requires FAT32 format first |

### Notes

- **Lexar-branded** card using Longsys silicon (Longsys acquired Lexar brand in 2017)
- MID $AD = Longsys (often incorrectly listed as Hynix in older databases)
- OEM code "LS" = Longsys/Lexar
- **CCC $DB7** includes Class 1 (stream read) and Class 11 (video speed class) - video-optimized card
- **V30** = Video Speed Class 30 (30 MB/s sustained write)
- **U3** = UHS Speed Class 3 (30 MB/s minimum write)
- **(10)** = Speed Class 10 (10 MB/s minimum write speed)
- **100 MB/s*** = Maximum read speed (asterisk indicates "up to")
- **SD 4.xx spec** compliant (SD_SPEC4=1) - newer than most cards in catalog
- **CMD23 supported** - Set Block Count for multi-block operations
- Factory formatted with exFAT (not FAT32)
- Very recent manufacture (November 2024)
- Needs FAT32 reformat before use with P2 SD driver
- DATA_STAT_AFTER_ERASE=0 (erased data reads as 0s)
- **Benchmark data available** — see Benchmark Results section below for detailed throughput measurements (raw, multi-sector, and file-level)

### SPI Speed Characterization

**Test Date:** 2026-02-02
**Test Configuration:**
- SYSCLK: 200 MHz
- Phase 1: 1,000 single-sector reads (quick check)
- Phase 2: 10,000 single-sector reads (statistical confidence)
- Phase 3: 100 × 8-sector multi-block reads (800 sectors, sustained transfer)
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
| 27 MHz | 4 clocks | 25.0 MHz | -7.4% | timeout* | — | — | **100%** |

*At 27 MHz, CMD6 High Speed switch failed; card became unresponsive during Phase 1.

**Summary:**
- **Maximum Reliable Speed: 25 MHz** (47,200 total sector reads at 0% failure rate)
- CMD6 High Speed mode: Switch **failed** at 27 MHz, card became unresponsive

### Internal Throughput (measured at 25 MHz SPI)

| Metric | Value |
|--------|-------|
| Phase 2 Duration (10,000 reads) | 4.84 seconds |
| Throughput | **1,059 KB/s** |
| Sectors/second | 2,068 |
| Latency per sector | 0.48 ms |

**Performance Class:** HIGH - **Fastest card tested** (12% faster than Gigastone Camera Plus, 35% faster than Samsung/SanDisk).

### Benchmark Results (Smart Pin SPI + Multi-Sector)

**Test Program**: SD_performance_benchmark.spin2 v2.0

#### 350 MHz Run

**SysClk**: 350 MHz | **SPI**: 25,000 kHz | **Mount**: 358.0 ms

| Test | Min (us) | Avg (us) | Max (us) | KB/s |
|------|----------|----------|----------|------|
| **Raw Single-Sector** | | | | |
| Read 1x512B | 402 | 413 | 505 | **1,239** |
| Write 1x512B | 746 | 756 | 841 | **677** |
| **Raw Multi-Sector** | | | | |
| Read 8 sectors (4 KB) | 1,886 | 1,896 | 1,985 | **2,160** |
| Read 32 sectors (16 KB) | 6,974 | 6,985 | 7,073 | **2,345** |
| Read 64 sectors (32 KB) | 13,758 | 13,769 | 13,861 | **2,379** |
| Write 8 sectors (4 KB) | 2,221 | 2,221 | 2,221 | **1,844** |
| Write 32 sectors (16 KB) | 7,405 | 7,405 | 7,405 | **2,212** |
| Write 64 sectors (32 KB) | 14,576 | 14,576 | 14,577 | **2,248** |
| **File-Level** | | | | |
| File Write 512B | 4,794 | 5,534 | 5,905 | **92** |
| File Write 4 KB | 12,213 | 12,985 | 13,702 | **315** |
| File Write 32 KB | 53,626 | 69,860 | 87,850 | **469** |
| File Read 4 KB | 3,094 | 3,147 | 3,625 | **1,301** |
| File Read 32 KB | 23,788 | 23,842 | 24,337 | **1,374** |
| File Read 128 KB | 95,725 | 95,821 | 96,690 | **1,367** |
| File Read 256 KB | 171,060 | 171,152 | 171,988 | **1,531** |
| **Overhead** | | | | |
| File Open | 103 | 149 | 570 | — |
| File Close | 20 | 20 | 20 | — |
| Mount | — | 358,000 | — | — |

Multi-sector improvement: 64x single reads = 28,498 us vs 1x CMD18 = 13,758 us (**51% faster**)

#### 320 MHz Run

**SysClk**: 320 MHz | **SPI**: 22,857 kHz | **Mount**: 212.3 ms

| Test | Min (us) | Avg (us) | Max (us) | KB/s |
|------|----------|----------|----------|------|
| **Raw Single-Sector** | | | | |
| Read 1x512B | 438 | 448 | 538 | **1,142** |
| Write 1x512B | 781 | 790 | 872 | **648** |
| **Raw Multi-Sector** | | | | |
| Read 8 sectors (4 KB) | 2,063 | 2,073 | 2,163 | **1,975** |
| Read 32 sectors (16 KB) | 7,637 | 7,647 | 7,737 | **2,142** |
| Read 64 sectors (32 KB) | 15,068 | 15,078 | 15,168 | **2,173** |
| Write 8 sectors (4 KB) | 2,398 | 2,400 | 2,402 | **1,706** |
| Write 32 sectors (16 KB) | 8,080 | 8,080 | 8,080 | **2,027** |
| Write 64 sectors (32 KB) | 15,912 | 15,912 | 15,916 | **2,059** |
| **File-Level** | | | | |
| File Write 512B | 4,950 | 5,723 | 6,101 | **89** |
| File Write 4 KB | 11,149 | 12,463 | 14,551 | **328** |
| File Write 32 KB | 59,684 | 65,391 | 71,017 | **501** |
| File Read 4 KB | 3,353 | 3,429 | 4,113 | **1,194** |
| File Read 32 KB | 25,810 | 25,888 | 26,590 | **1,265** |
| File Read 128 KB | 102,825 | 102,948 | 104,053 | **1,273** |
| File Read 256 KB | 205,513 | 205,648 | 206,863 | **1,274** |
| **Overhead** | | | | |
| File Open | 101 | 198 | 1,080 | — |
| File Close | 22 | 22 | 22 | — |
| Mount | — | 212,300 | — | — |

Multi-sector improvement: 64x single reads = 30,644 us vs 1x CMD18 = 15,068 us (**50% faster**)

#### 270 MHz Run

**SysClk**: 270 MHz | **SPI**: 22,500 kHz | **Mount**: 213.4 ms

| Test | Min (us) | Avg (us) | Max (us) | KB/s |
|------|----------|----------|----------|------|
| **Raw Single-Sector** | | | | |
| Read 1x512B | 473 | 483 | 573 | **1,060** |
| Write 1x512B | 811 | 820 | 901 | **624** |
| **Raw Multi-Sector** | | | | |
| Read 8 sectors (4 KB) | 2,185 | 2,195 | 2,285 | **1,866** |
| Read 32 sectors (16 KB) | 8,053 | 8,063 | 8,153 | **2,031** |
| Read 64 sectors (32 KB) | 15,889 | 15,898 | 15,988 | **2,061** |
| Write 8 sectors (4 KB) | 2,533 | 2,533 | 2,533 | **1,617** |
| Write 32 sectors (16 KB) | 8,543 | 8,543 | 8,544 | **1,917** |
| Write 64 sectors (32 KB) | 16,812 | 16,812 | 16,812 | **1,949** |
| **File-Level** | | | | |
| File Write 512B | 5,219 | 6,004 | 6,400 | **85** |
| File Write 4 KB | 13,139 | 13,899 | 14,618 | **294** |
| File Write 32 KB | 58,388 | 67,086 | 87,308 | **488** |
| File Read 4 KB | 3,611 | 3,677 | 4,271 | **1,113** |
| File Read 32 KB | 27,687 | 27,754 | 28,365 | **1,180** |
| File Read 128 KB | 110,258 | 110,373 | 111,413 | **1,187** |
| File Read 256 KB | 220,354 | 220,481 | 221,633 | **1,188** |
| **Overhead** | | | | |
| File Open | 120 | 207 | 999 | — |
| File Close | 26 | 26 | 27 | — |
| Mount | — | 213,400 | — | — |

Multi-sector improvement: 64x single reads = 32,857 us vs 1x CMD18 = 15,889 us (**51% faster**)

#### Sysclk Effect (350 vs 320 vs 270 MHz)

| Test | 350 MHz (KB/s) | 320 MHz (KB/s) | 270 MHz (KB/s) | 350→270 Delta |
|------|----------------|----------------|----------------|---------------|
| Raw Read 1x512B | 1,239 | 1,142 | 1,060 | +17% |
| Raw Read 64x (32 KB) | 2,379 | 2,173 | 2,061 | +15% |
| Raw Write 64x (32 KB) | 2,248 | 2,059 | 1,949 | +15% |
| File Read 256 KB | 1,531 | 1,274 | 1,188 | +29% |
| File Write 32 KB | 469 | 501 | 488 | -4% |

**Note:** The jump from 320→350 MHz crosses an SPI divider boundary (22.9→25.0 MHz), giving a disproportionate throughput gain. File write 32 KB variance is dominated by card-internal flash programming time.
