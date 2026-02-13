# Card: PNY 16GB SDHC

**Label:** PNY 16GB microSD HC I
**Unique ID:** `Phison_SD16G_3.0_01CD5CF5_201808`
**Test Date:** 2026-02-02 (characterization)

### Raw Registers

```
CID: $27 $50 $48 $53 $44 $31 $36 $47 $30 $01 $CD $5C $F5 $01 $28 $1F
CSD: $40 $0E $00 $32 $5B $59 $00 $00 $74 $27 $7F $80 $0A $40 $00 $0F
OCR: $C0FF_8000
SCR: $02 $35 $80 $00 $01 $00 $00 $00
```

### CID Register (Card Identification) - All Fields

| Field | Bits | Raw | Value | Usage |
|-------|------|-----|-------|-------|
| MID | [127:120] | $27 | Phison | **[USED]** |
| OID | [119:104] | $50 $48 | "PH" (Phison) | [INFO] |
| PNM | [103:64] | $53 $44 $31 $36 $47 | "SD16G" | [INFO] |
| PRV | [63:56] | $30 | 3.0 | [INFO] |
| PSN | [55:24] | $01CD_5CF5 | 30,309,621 | [INFO] |
| MDT | [19:8] | $128 | 2018-08 (August 2018) | [INFO] |
| CRC7 | [7:1] | $0F | $0F | [INFO] |

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
| C_SIZE | [69:48] | $7427 | 29,735 (14,868 MB) | **[USED]** |
| ERASE_BLK_EN | [46] | 1 | 512-byte erase supported | [INFO] |
| SECTOR_SIZE | [45:39] | 127 | Erase sector = 64 KB | [INFO] |
| WP_GRP_SIZE | [38:32] | 0 | — | [INFO] |
| WP_GRP_ENABLE | [31] | 0 | Disabled | [INFO] |
| R2W_FACTOR | [28:26] | 2 | Write = Read x 4 | **[USED]** |
| WRITE_BL_LEN | [25:22] | 9 | 512 bytes | [INFO] |
| WRITE_BL_PARTIAL | [21] | 0 | Not allowed | [INFO] |
| FILE_FORMAT_GRP | [15] | 0 | — | [INFO] |
| COPY | [14] | 0 | Original | [INFO] |
| PERM_WRITE_PROTECT | [13] | 0 | Not protected | [INFO] |
| TMP_WRITE_PROTECT | [12] | 0 | Not protected | [INFO] |
| FILE_FORMAT | [11:10] | 0 | Hard disk-like | [INFO] |
| CRC7 | [7:1] | $07 | $07 | [INFO] |

**Derived Values:**
- Read Timeout: 100 ms (calculated)
- Write Timeout: 250 ms (calculated)
- Total Sectors: 30,449,664
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
| SD_SECURITY | [54:52] | 3 | SDHC (security v2.00) | [INFO] |
| SD_BUS_WIDTHS | [51:48] | $5 | 1-bit and 4-bit supported | [INFO] |
| SD_SPEC3 | [47] | 1 | SD 3.0 support: Yes | [INFO] |
| EX_SECURITY | [46:43] | 0 | No extended security | [INFO] |
| SD_SPEC4 | [42] | 0 | SD 4.0 support: No | [INFO] |
| SD_SPECX | [41:38] | 0 | — | [INFO] |
| CMD_SUPPORT | [33:32] | $01 | CMD20 (Speed Class) supported | [INFO] |

**SD Version:** 3.0x (SD_SPEC=2, SD_SPEC3=1)

### Filesystem (FAT32 - formatted with P2FMTER)

| Field | Value |
|-------|-------|
| MBR Partition Type | $0C (FAT32 LBA) |
| VBR Sector | 8,192 |
| OEM Name | P2FMTER |
| Volume Label | P2-XFER |
| Volume Serial | $0852_A9C2 |
| FS Type | FAT32 |
| Bytes/Sector | 512 |
| Sectors/Cluster | 16 (8 KB clusters) |
| Reserved Sectors | 32 |
| Number of FATs | 2 |
| Sectors per FAT | 14,864 |
| Root Cluster | 2 |
| Total Sectors | 30,441,472 |
| Data Region Start | Sector 29,760 |
| Total Clusters | 1,900,732 |

### Test Results

| Test | Status | Notes |
|------|--------|-------|
| Card Init | PASS | MID $27 triggers 20 MHz SPI limit |
| CID Read | PASS | Worker cog routing |
| CSD Read | PASS | Worker cog routing |
| SCR Read | PASS | Worker cog routing |
| OCR Read | PASS | Cached during init |
| MBR Read | PASS | FAT32 LBA ($0C) partition |
| VBR Read | PASS | |
| Mount | PASS | |
| Regression | PASS | All tests pass with driver |

### Notes

- **PNY-branded** card using **Phison** controller silicon (MID $27)
- MID $27 + OID "PH" = Phison controller family (also used by Delkin, HP, Kingston, Lexar older, PNY)
- **driver handles this card correctly** - MID $27 detected, SPI clock limited to 20 MHz
- **HC I** = SDHC UHS-I interface
- Older card (manufactured August 2018)
- CMD20 (Speed Class) supported per SCR CMD_SUPPORT field
- 8 KB clusters (smaller than typical 32 KB) due to P2FMTER formatting
- driver fixes resolved previous V2 unmount hang issue
- **File open latency: 16.9 ms** (85x slower than other cards' ~200 us) — the Phison controller's internal metadata lookup is extremely slow, dominating small-file operations
- **Benchmark data available** — see Benchmark Results section below for detailed throughput measurements; multi-sector reads surprisingly competitive (2,110 KB/s) despite slow single-sector and write performance

### SPI Speed Characterization

**Test Date:** 2026-02-02
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
| 27 MHz | 4 clocks | 25.0 MHz | -7.4% | 0 OK, 1,000 timeout | — | — | **100%** |

**Summary:**
- **Maximum Reliable Speed: 25 MHz** (47,200 total sector reads at 0% failure rate)
- **25 MHz PASSED all phases** - 11,800 reads with 0 CRC errors, 0 timeouts
- CMD6 High Speed mode: Switch **failed** at 27 MHz, card became unresponsive (same behavior as SanDisk)
- Card has significantly slower internal processing than SanDisk cards (~20x slower throughput)

**Key Finding:** The 20 MHz limit for Phison/PNY cards is **overly conservative**. This card successfully ran at 25 MHz with **0 CRC errors across 47,200 reads** (4 speed levels x 11,800 reads each). The TRAN_SPEED register ($32 = 25 MHz) accurately reflects this card's capability.

**Recommendation:** The driver's MID $27 -> 20 MHz limit should be reconsidered. This PNY/Phison card handles 25 MHz reliably. The failure mode at 27 MHz (after CMD6 switch attempt) is identical to SanDisk cards - the CMD6 High Speed switch fails and the card becomes unresponsive, not a gradual degradation from clock speed.

### Internal Throughput (measured at 25 MHz SPI)

| Metric | Value |
|--------|-------|
| Phase 2 Duration (10,000 reads) | 160.1 seconds |
| Throughput | **31.3 KB/s** |
| Sectors/second | 62.5 |
| Latency per sector | 16.0 ms |

**Performance Class:** LOW - Slow internal controller (~25x slower than high-performance cards). SPI clock speed is not the bottleneck; internal flash/controller latency dominates.

### Benchmark Results (Smart Pin SPI + Multi-Sector)

**Test Program**: SD_performance_benchmark.spin2 v2.0 | **SPI**: ~22.8 kHz @ 320 MHz, ~22.5 kHz @ 270 MHz

#### 320 MHz Run

**SysClk**: 320 MHz | **SPI**: 22,857 kHz | **Mount**: 202.7 ms

| Test | Min (us) | Avg (us) | Max (us) | KB/s |
|------|----------|----------|----------|------|
| **Raw Single-Sector** | | | | |
| Read 1x512B | 812 | 812 | 812 | **630** |
| Write 1x512B | 2,965 | 8,974 | 11,110 | **57** |
| **Raw Multi-Sector** | | | | |
| Read 8 sectors (4 KB) | 3,121 | 3,121 | 3,121 | **1,312** |
| Read 32 sectors (16 KB) | 7,762 | 7,763 | 7,772 | **2,110** |
| Read 64 sectors (32 KB) | 15,038 | 16,098 | 16,220 | **2,035** |
| Write 8 sectors (4 KB) | 11,652 | 11,779 | 12,025 | **347** |
| Write 32 sectors (16 KB) | 24,956 | 25,157 | 26,276 | **651** |
| Write 64 sectors (32 KB) | 32,815 | 32,916 | 33,279 | **995** |
| **File-Level** | | | | |
| File Write 512B | 29,598 | 35,981 | 93,163 | **14** |
| File Write 4 KB | 43,018 | 43,219 | 44,657 | **94** |
| File Write 32 KB | 178,247 | 188,682 | 201,906 | **173** |
| File Read 4 KB | 22,885 | 22,993 | 23,568 | **178** |
| File Read 32 KB | 64,651 | 64,775 | 65,349 | **505** |
| File Read 128 KB | 207,824 | 207,937 | 208,623 | **630** |
| File Read 256 KB | 395,348 | 395,454 | 396,071 | **662** |
| **Overhead** | | | | |
| File Open | 16,838 | 16,945 | 17,581 | — |
| File Close | 22 | 22 | 22 | — |
| Mount | — | 202,700 | — | — |

Multi-sector improvement: 64x single reads = 46,658 us vs 1x CMD18 = 15,033 us (**67% faster**)

#### 270 MHz Run

**SysClk**: 270 MHz | **SPI**: 22,500 kHz | **Mount**: 203.9 ms

| Test | Min (us) | Avg (us) | Max (us) | KB/s |
|------|----------|----------|----------|------|
| **Raw Single-Sector** | | | | |
| Read 1x512B | 760 | 760 | 760 | **673** |
| Write 1x512B | 9,759 | 9,795 | 9,833 | **52** |
| **Raw Multi-Sector** | | | | |
| Read 8 sectors (4 KB) | 3,282 | 3,282 | 3,283 | **1,248** |
| Read 32 sectors (16 KB) | 8,152 | 8,153 | 8,158 | **2,009** |
| Read 64 sectors (32 KB) | 15,796 | 16,930 | 17,057 | **1,935** |
| Write 8 sectors (4 KB) | 11,698 | 11,837 | 12,075 | **346** |
| Write 32 sectors (16 KB) | 25,237 | 25,432 | 26,543 | **644** |
| Write 64 sectors (32 KB) | 33,501 | 33,559 | 33,618 | **976** |
| **File-Level** | | | | |
| File Write 512B | 29,325 | 34,644 | 51,184 | **14** |
| File Write 4 KB | 44,287 | 45,344 | 52,753 | **90** |
| File Write 32 KB | 181,724 | 191,479 | 213,768 | **171** |
| File Read 4 KB | 23,923 | 24,062 | 24,689 | **170** |
| File Read 32 KB | 67,401 | 67,539 | 68,227 | **485** |
| File Read 128 KB | 216,214 | 216,324 | 216,974 | **605** |
| File Read 256 KB | 411,067 | 411,174 | 411,826 | **637** |
| **Overhead** | | | | |
| File Open | 17,624 | 17,778 | 18,456 | — |
| File Close | 26 | 26 | 27 | — |
| Mount | — | 203,900 | — | — |

Multi-sector improvement: 64x single reads = 49,650 us vs 1x CMD18 = 15,801 us (**68% faster**)

#### Sysclk Effect (320 vs 270 MHz)

| Test | 320 MHz (KB/s) | 270 MHz (KB/s) | Delta |
|------|----------------|----------------|-------|
| Raw Read 1x512B | 630 | 673 | +6.8% |
| Raw Read 64x (32 KB) | 2,035 | 1,935 | -4.9% |
| Raw Write 64x (32 KB) | 995 | 976 | -1.9% |
| File Read 256 KB | 662 | 637 | -3.8% |
| File Write 32 KB | 173 | 171 | -1.2% |

The PNY Phison controller has the most distinctive behavior of all cards tested:
- **File open: 16.9 ms** (85x slower than other cards' ~200 us). This dominates small file operations -- a 4 KB file read takes 23 ms, of which 17 ms is just opening the file.
- **Raw single-sector write: 57 KB/s** with enormous variance (Min=2,965, Max=11,110 us) -- the Phison controller has unpredictable write commit latency.
- **Multi-sector reads surprisingly fast**: 2,110 KB/s at 32 sectors -- competitive with the best cards. The Phison sustains excellent sequential read throughput once past the open overhead.
- **Multi-sector writes very slow**: 995 KB/s at 64 sectors -- roughly half of other cards, indicating the write bottleneck is in the flash controller, not the SPI interface.
- **Extremely consistent reads**: Min=Max for single-sector (812 us) and 8-sector reads (3,121 us) at 320 MHz -- zero variance, suggesting the Phison controller has deterministic read paths.
