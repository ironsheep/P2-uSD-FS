# Card: SanDisk Industrial 16GB SDHC

**Label:** SanDisk Industrial microSD HC I, U1 C10, 16GB
**Unique ID:** `SanDisk_SA16G_8.0_93E9C0A1_202511`
**Test Date:** 2026-02-07 (characterization + speed characterization)

### Raw Registers

```
CID: $03 $53 $44 $53 $41 $31 $36 $47 $80 $93 $E9 $C0 $A1 $01 $9B $59
CSD: $40 $0E $00 $32 $5B $59 $00 $00 $76 $B2 $7F $80 $0A $40 $40 $13
OCR: $C0FF_8000
SCR: $02 $35 $84 $43 $00 $00 $00 $00
```

### CID Register (Card Identification) - All Fields

| Field | Bits | Raw | Value | Usage |
|-------|------|-----|-------|-------|
| MID | [127:120] | $03 | SanDisk | **[USED]** |
| OID | [119:104] | $53 $44 | "SD" (ASCII) | [INFO] |
| PNM | [103:64] | $53 $41 $31 $36 $47 | "SA16G" | [INFO] |
| PRV | [63:56] | $80 | 8.0 | [INFO] |
| PSN | [55:24] | $93E9_C0A1 | 2,481,291,425 | [INFO] |
| MDT | [19:8] | $19B | 2025-11 (November 2025) | [INFO] |
| CRC7 | [7:1] | $2C | $2C | [INFO] |

### CSD Register (Card Specific Data) - All Fields

| Field | Bits | Raw | Value | Usage |
|-------|------|-----|-------|-------|
| CSD_STRUCTURE | [127:126] | 1 | CSD Version 2.0 (SDHC/SDXC) | **[USED]** |
| TAAC | [119:112] | $0E | Read access time-1 | **[USED]** |
| NSAC | [111:104] | $00 | 0 CLK cycles | **[USED]** |
| TRAN_SPEED | [103:96] | $32 | 25 MHz max | **[USED]** |
| CCC | [95:84] | $5B5 | Classes 0,2,4,5,7,8,10 | [INFO] |
| READ_BL_LEN | [83:80] | 9 | 512 bytes | [INFO] |
| READ_BL_PARTIAL | [79] | 0 | Not allowed | [INFO] |
| WRITE_BLK_MISALIGN | [78] | 0 | Not allowed | [INFO] |
| READ_BLK_MISALIGN | [77] | 0 | Not allowed | [INFO] |
| DSR_IMP | [76] | 0 | DSR not implemented | [INFO] |
| C_SIZE | [69:48] | $76B2 | 30,386 (15,193 MB) | **[USED]** |
| ERASE_BLK_EN | [46] | 1 | 512-byte erase supported | [INFO] |
| SECTOR_SIZE | [45:39] | 127 | Erase sector = 64 KB | [INFO] |
| WP_GRP_SIZE | [38:32] | 0 | — | [INFO] |
| WP_GRP_ENABLE | [31] | 0 | Disabled | [INFO] |
| R2W_FACTOR | [28:26] | 2 | Write = Read x 4 | **[USED]** |
| WRITE_BL_LEN | [25:22] | 9 | 512 bytes | [INFO] |
| WRITE_BL_PARTIAL | [21] | 0 | Not allowed | [INFO] |
| FILE_FORMAT_GRP | [15] | 0 | — | [INFO] |
| COPY | [14] | 1 | Copy | [INFO] |
| PERM_WRITE_PROTECT | [13] | 0 | Not protected | [INFO] |
| TMP_WRITE_PROTECT | [12] | 0 | Not protected | [INFO] |
| FILE_FORMAT | [11:10] | 0 | Hard disk-like | [INFO] |
| CRC7 | [7:1] | $09 | $09 | [INFO] |

**Derived Values:**
- Read Timeout: 100 ms (calculated)
- Write Timeout: 250 ms (calculated)
- Total Sectors: 31,115,264
- Capacity: ~14 GB

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
| SD_SECURITY | [54:52] | 3 | SDHC Card (security v2.00) | [INFO] |
| SD_BUS_WIDTHS | [51:48] | $5 | 1-bit and 4-bit supported | [INFO] |
| SD_SPEC3 | [47] | 1 | SD 3.0 support: Yes | [INFO] |
| EX_SECURITY | [46:43] | 0 | No extended security | [INFO] |
| SD_SPEC4 | [42] | 1 | SD 4.0 support: Yes | [INFO] |
| SD_SPECX | [41:38] | 1 | SD 5.x/6.x/7.x indicator | [INFO] |
| CMD_SUPPORT | [33:32] | $00 | — | [INFO] |

**SD Version:** 4.xx (SD_SPEC=2, SD_SPEC3=1, SD_SPEC4=1)

### Filesystem (FAT32)

| Field | Value |
|-------|-------|
| MBR Partition Type | $0C (FAT32 LBA) |
| VBR Sector | 8,192 |
| OEM Name | (blank) |
| Volume Label | NO NAME |
| Volume Serial | $3161_3063 |
| FS Type | FAT32 |
| Bytes/Sector | 512 |
| Sectors/Cluster | 64 (32 KB clusters) |
| Reserved Sectors | 598 |
| Number of FATs | 2 |
| Sectors per FAT | 3,797 |
| Root Cluster | 2 |
| Total Sectors | 31,108,096 |
| Total Clusters | 485,936 |

### Test Results

| Test | Status | Notes |
|------|--------|-------|
| Card Init | PASS | CMD0/CMD8/ACMD41 sequence successful |
| CID Read | PASS | Worker cog routing |
| CSD Read | PASS | Worker cog routing |
| SCR Read | PASS | Worker cog routing |
| OCR Read | PASS | Cached during init |
| MBR Read | PASS | FAT32 LBA ($0C) partition |
| VBR Read | PASS | |

### Notes

- **SanDisk Industrial** - designed for embedded and industrial applications
- MID $03 = SanDisk / Western Digital (standard, not PNY-limited)
- **"SA16G"** product name - Industrial product line
- **HC I** = SDHC UHS-I interface
- **U1** = UHS Speed Class 1 (10 MB/s minimum write speed)
- **C10** = Speed Class 10 (10 MB/s minimum write speed)
- **SD 4.xx spec** compliant (SD_SPEC4=1, SD_SPECX=1)
- CCC $5B5 - standard command classes including Class 10 (switch)
- **COPY=1** - indicates card contents were copied during manufacturing
- DATA_STAT_AFTER_ERASE=0 (erased data reads as 0s)
- Card arrived pre-formatted as FAT32 with 64 sectors/cluster (32 KB clusters)
- OEM name blank, volume label "NO NAME" - factory default format
- Industrial cards are recommended for embedded SPI use due to wider temperature range and longer endurance
- **Benchmark data available** -- see Benchmark Results section below for detailed throughput measurements at both 320 and 270 MHz sysclk

### SPI Speed Characterization

**Test Date:** 2026-02-07
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
| 27 MHz | 4 clocks | 25.0 MHz | -7.4% | timeout* | — | — | **100%** |

*At 27 MHz, CMD6 High Speed switch failed; card became unresponsive during Phase 1.

**Summary:**
- **Maximum Reliable Speed: 25 MHz** (47,200 total sector reads at 0% failure rate)
- CMD6 High Speed mode: Switch **failed** at 27 MHz, card became unresponsive

### Internal Throughput (measured at 25 MHz SPI)

| Metric | Value |
|--------|-------|
| Phase 2 Duration (10,000 reads) | 6.21 seconds |
| Throughput | **824 KB/s** |
| Sectors/second | 1,609 |
| Latency per sector | 0.62 ms |

**Performance Class:** HIGH - Strong performance from the industrial product line, comparable to SanDisk consumer cards.

### Benchmark Results (Smart Pin SPI + Multi-Sector)

**Test Program**: SD_performance_benchmark.spin2 v2.0 | **SPI**: ~22.8 kHz @ 320 MHz, ~22.5 kHz @ 270 MHz

#### 320 MHz Run

**SysClk**: 320 MHz | **SPI**: 22,857 kHz | **Mount**: 234.9 ms

| Test | Min (us) | Avg (us) | Max (us) | KB/s |
|------|----------|----------|----------|------|
| **Raw Single-Sector** | | | | |
| Read 1x512B | 529 | 733 | 1,157 | **698** |
| Write 1x512B | 1,323 | 1,433 | 2,142 | **357** |
| **Raw Multi-Sector** | | | | |
| Read 8 sectors (4 KB) | 2,130 | 2,228 | 2,753 | **1,838** |
| Read 32 sectors (16 KB) | 7,613 | 7,807 | 8,237 | **2,098** |
| Read 64 sectors (32 KB) | 14,865 | 14,964 | 15,478 | **2,189** |
| Write 8 sectors (4 KB) | 2,702 | 3,023 | 3,631 | **1,354** |
| Write 32 sectors (16 KB) | 8,585 | 9,469 | 14,459 | **1,730** |
| Write 64 sectors (32 KB) | 16,112 | 16,414 | 17,277 | **1,996** |
| **File-Level** | | | | |
| File Write 512B | 8,213 | 8,473 | 9,569 | **60** |
| File Write 4 KB | 17,943 | 18,778 | 25,497 | **218** |
| File Write 32 KB | 100,761 | 101,954 | 102,412 | **321** |
| File Read 4 KB | 4,907 | 5,027 | 5,519 | **814** |
| File Read 32 KB | 38,157 | 38,279 | 38,785 | **856** |
| File Read 128 KB | 152,885 | 153,361 | 155,033 | **854** |
| File Read 256 KB | 312,611 | 313,629 | 315,305 | **835** |
| **Overhead** | | | | |
| File Open | 95 | 144 | 588 | — |
| File Close | 22 | 22 | 22 | — |
| Mount | — | 234,900 | — | — |

Multi-sector improvement: 64x single reads = 43,549 us vs 1x CMD18 = 14,870 us (**65% faster**)

#### 270 MHz Run

**SysClk**: 270 MHz | **SPI**: 22,500 kHz | **Mount**: 235.7 ms

| Test | Min (us) | Avg (us) | Max (us) | KB/s |
|------|----------|----------|----------|------|
| **Raw Single-Sector** | | | | |
| Read 1x512B | 563 | 667 | 1,176 | **767** |
| Write 1x512B | 1,347 | 1,459 | 2,168 | **350** |
| **Raw Multi-Sector** | | | | |
| Read 8 sectors (4 KB) | 2,245 | 2,436 | 2,858 | **1,681** |
| Read 32 sectors (16 KB) | 7,984 | 8,085 | 8,598 | **2,026** |
| Read 64 sectors (32 KB) | 15,616 | 15,716 | 16,247 | **2,085** |
| Write 8 sectors (4 KB) | 2,929 | 3,228 | 3,966 | **1,268** |
| Write 32 sectors (16 KB) | 9,002 | 9,841 | 14,692 | **1,664** |
| Write 64 sectors (32 KB) | 16,994 | 17,209 | 17,937 | **1,904** |
| **File-Level** | | | | |
| File Write 512B | 7,688 | 9,356 | 16,512 | **54** |
| File Write 4 KB | 18,446 | 20,072 | 25,931 | **204** |
| File Write 32 KB | 102,787 | 106,053 | 114,192 | **308** |
| File Read 4 KB | 5,180 | 5,262 | 5,901 | **778** |
| File Read 32 KB | 40,224 | 40,353 | 40,868 | **812** |
| File Read 128 KB | 160,846 | 161,244 | 162,150 | **812** |
| File Read 256 KB | 328,164 | 329,147 | 330,276 | **796** |
| **Overhead** | | | | |
| File Open | 113 | 163 | 614 | — |
| File Close | 26 | 26 | 27 | — |
| Mount | — | 235,700 | — | — |

Multi-sector improvement: 64x single reads = 45,737 us vs 1x CMD18 = 15,622 us (**65% faster**)

#### Sysclk Effect (320 vs 270 MHz)

| Test | 320 MHz (KB/s) | 270 MHz (KB/s) | Delta |
|------|----------------|----------------|-------|
| Raw Read 1x512B | 698 | 767 | +9.9% |
| Raw Read 64x (32 KB) | 2,189 | 2,085 | -4.7% |
| Raw Write 64x (32 KB) | 1,996 | 1,904 | -4.6% |
| File Read 256 KB | 835 | 796 | -4.7% |
| File Write 32 KB | 321 | 308 | -4.0% |

SPI frequency changes slightly (22,857 vs 22,500 kHz = -1.6%) but the Spin2 overhead between SPI transfers runs slower at 270 MHz, accounting for the ~4-5% throughput reduction on multi-sector and file-level operations. The single-sector raw read anomaly (+9.9%) is within card latency variance.
