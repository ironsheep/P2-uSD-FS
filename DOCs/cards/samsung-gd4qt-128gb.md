# Card: Samsung EVO Select 128GB SDXC

**Label:** Samsung EVO Select microSD XC I U3
**Unique ID:** `Samsung_GD4QT_3.0_C0305565_201805`
**Test Date:** 2026-02-02 (characterization)

### Raw Registers

```
CID: $1B $53 $4D $47 $44 $34 $51 $54 $30 $C0 $30 $55 $65 $01 $25 $E9
CSD: $40 $0E $00 $32 $5B $59 $00 $03 $B9 $FF $7F $80 $0A $40 $40 $AB
OCR: $C0FF_8000
SCR: $02 $C5 $80 $03 $00 $00 $00 $00
```

### CID Register (Card Identification) - All Fields

| Field | Bits | Raw | Value | Usage |
|-------|------|-----|-------|-------|
| MID | [127:120] | $1B | Samsung | **[USED]** |
| OID | [119:104] | $53 $4D | "SM" (Samsung) | [INFO] |
| PNM | [103:64] | $47 $44 $34 $51 $54 | "GD4QT" | [INFO] |
| PRV | [63:56] | $30 | 3.0 | [INFO] |
| PSN | [55:24] | $C030_5565 | 3,224,646,005 | [INFO] |
| MDT | [19:8] | $125 | 2018-05 (May 2018) | [INFO] |
| CRC7 | [7:1] | $74 | $74 | [INFO] |

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
| C_SIZE | [69:48] | $3B9FF | 244,223 (122,112 MB) | **[USED]** |
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
| CRC7 | [7:1] | $55 | $55 | [INFO] |

**Derived Values:**
- Read Timeout: 100 ms (calculated)
- Write Timeout: 250 ms (calculated)
- Total Sectors: 250,085,376
- Capacity: ~119 GB

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
| DATA_STAT_AFTER_ERASE | [55] | 1 | Data = 1 after erase | [INFO] |
| SD_SECURITY | [54:52] | 4 | SDXC (security v3.xx) | [INFO] |
| SD_BUS_WIDTHS | [51:48] | $5 | 1-bit and 4-bit supported | [INFO] |
| SD_SPEC3 | [47] | 1 | SD 3.0 support: Yes | [INFO] |
| EX_SECURITY | [46:43] | 0 | No extended security | [INFO] |
| SD_SPEC4 | [42] | 0 | SD 4.0 support: No | [INFO] |
| SD_SPECX | [41:38] | 0 | — | [INFO] |
| CMD_SUPPORT | [33:32] | $00 | — | [INFO] |

**SD Version:** 3.0x (SD_SPEC=2, SD_SPEC3=1)

### Filesystem (FAT32 - reformatted with P2FMTER)

| Field | Value |
|-------|-------|
| MBR Partition Type | $0C (FAT32 LBA) |
| VBR Sector | 8,192 |
| OEM Name | P2FMTER |
| Volume Label | P2-XFER |
| Volume Serial | $04AF_0E27 |
| FS Type | FAT32 |
| Bytes/Sector | 512 |
| Sectors/Cluster | 64 (32 KB clusters) |
| Reserved Sectors | 32 |
| Number of FATs | 2 |
| Sectors per FAT | 30,527 |
| Root Cluster | 2 |
| Total Sectors | 250,077,184 |
| Data Region Start | Sector 61,086 |
| Total Clusters | 3,906,501 |

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
| Mount | PASS | |
| Regression | PASS | All tests pass |

### Notes

- **Genuine Samsung** - MID $1B + OID "SM" confirms authentic Samsung
- **EVO Select** = Amazon-exclusive variant of EVO Plus line
- **XC I** = SDXC UHS-I interface (up to 104 MB/s bus speed)
- **U3** = UHS Speed Class 3 (30 MB/s minimum write speed)
- Used in DJI Phantom 4 drone - known reliable for video recording
- Originally factory formatted with exFAT, now reformatted with P2FMTER
- Largest card in catalog (128GB / ~119GB usable)
- DATA_STAT_AFTER_ERASE=1 (erased data reads as 1s, not 0s)
- **Benchmark data available** — see Benchmark Results section below for file-level throughput measurements (raw sector results pending re-test)

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
| 27 MHz | 4 clocks | 25.0 MHz | -7.4% | timeout* | — | — | **100%** |

*At 27 MHz, CMD6 High Speed switch failed; card became unresponsive during Phase 1.

**Summary:**
- **Maximum Reliable Speed: 25 MHz** (47,200 total sector reads at 0% failure rate)
- CMD6 High Speed mode: Switch **failed** at 27 MHz, card became unresponsive

### Internal Throughput (measured at 25 MHz SPI)

| Metric | Value |
|--------|-------|
| Phase 2 Duration (10,000 reads) | 6.53 seconds |
| Throughput | **783 KB/s** |
| Sectors/second | 1,531 |
| Latency per sector | 0.65 ms |

**Performance Class:** HIGH - Very similar to SanDisk Nintendo Switch (within 0.4%).

### Benchmark Results (Smart Pin SPI + Multi-Sector)

**Test Program**: SD_performance_benchmark.spin2 v2.0 | **SPI**: ~22.8 kHz @ 320 MHz, ~22.5 kHz @ 270 MHz

**Raw Sector Results Incomplete**: The initial benchmark run returned 0 for all raw sector tests due to a test seeding issue (not a card compatibility problem -- file-level operations, which use the same underlying sector I/O, work correctly). Raw sector results need to be re-collected; file-level results below are valid.

#### 320 MHz Run

**SysClk**: 320 MHz | **SPI**: 22,857 kHz | **Mount**: 202.3 ms

| Test | Min (us) | Avg (us) | Max (us) | KB/s |
|------|----------|----------|----------|------|
| **Raw Single-Sector** | | | | |
| Read 1x512B | — | — | — | **ERROR** |
| Write 1x512B | — | — | — | **ERROR** |
| **Raw Multi-Sector** | | | | |
| Read 8/32/64 sectors | — | — | — | **ERROR** |
| Write 8/32/64 sectors | — | — | — | **ERROR** |
| **File-Level** | | | | |
| File Write 512B | 7,821 | 8,027 | 8,537 | **63** |
| File Write 4 KB | 16,686 | 17,253 | 18,142 | **237** |
| File Write 32 KB | 84,730 | 102,330 | 121,699 | **320** |
| File Read 4 KB | 6,209 | 6,286 | 6,933 | **651** |
| File Read 32 KB | 35,227 | 35,296 | 35,769 | **928** |
| File Read 128 KB | 158,710 | 158,857 | 159,994 | **825** |
| File Read 256 KB | 340,113 | 340,286 | 341,577 | **770** |
| **Overhead** | | | | |
| File Open | 101 | 153 | 628 | — |
| File Close | 22 | 22 | 22 | — |
| Mount | — | 202,300 | — | — |

#### 270 MHz Run

**SysClk**: 270 MHz | **SPI**: 22,500 kHz | **Mount**: 214.4 ms

| Test | Min (us) | Avg (us) | Max (us) | KB/s |
|------|----------|----------|----------|------|
| **Raw Single-Sector** | | | | |
| Read 1x512B | — | — | — | **ERROR** |
| Write 1x512B | — | — | — | **ERROR** |
| **Raw Multi-Sector** | | | | |
| Read 8/32/64 sectors | — | — | — | **ERROR** |
| Write 8/32/64 sectors | — | — | — | **ERROR** |
| **File-Level** | | | | |
| File Write 512B | 8,083 | 8,278 | 8,773 | **61** |
| File Write 4 KB | 17,630 | 18,128 | 18,611 | **225** |
| File Write 32 KB | 87,594 | 100,385 | 122,577 | **326** |
| File Read 4 KB | 6,563 | 6,638 | 7,318 | **617** |
| File Read 32 KB | 37,533 | 37,594 | 38,110 | **871** |
| File Read 128 KB | 154,502 | 154,634 | 155,691 | **847** |
| File Read 256 KB | 342,910 | 343,077 | 344,231 | **764** |
| **Overhead** | | | | |
| File Open | 120 | 189 | 816 | — |
| File Close | 26 | 26 | 27 | — |
| Mount | — | 214,400 | — | — |

#### Sysclk Effect (320 vs 270 MHz)

| Test | 320 MHz (KB/s) | 270 MHz (KB/s) | Delta |
|------|----------------|----------------|-------|
| File Read 4 KB | 651 | 617 | -5.2% |
| File Read 256 KB | 770 | 764 | -0.8% |
| File Write 32 KB | 320 | 326 | +1.9% |

File reads at small sizes show the typical ~5% sysclk effect. At large sizes, the card controller's internal read latency dominates, making the sysclk difference negligible. File write results are within noise. This Samsung card has notably slower file reads (770 KB/s at 256 KB) compared to other cards (1,029-1,274 KB/s), likely due to higher internal controller latency.
