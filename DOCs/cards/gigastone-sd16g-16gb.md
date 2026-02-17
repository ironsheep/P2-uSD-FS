# Card: Gigastone "High Endurance" 16GB SDHC MLC

**Label:** Gigastone 10x High Endurance 16GB MLC microSD HC I U3 V30 4K
**Unique ID:** `BudgetOEM_SD16G_2.0_000003FB_202502`
**Test Date:** 2026-02-02 (characterization)

### Card Designator

```
Budget OEM SD16G SDHC 14GB [FAT32] SD 3.x rev2.0 SN:000003FB 2025/02
Class 10, U1, V10, SPI 25 MHz  [formatted by P2FMTER]
```

### Raw Registers

```
CID: $00 $34 $32 $53 $44 $31 $36 $47 $20 $00 $00 $03 $FB $01 $92 $D5
CSD: $40 $0E $00 $32 $5B $59 $00 $00 $77 $0B $7F $80 $0A $40 $00 $4F
OCR: $C0FF_8000
SCR: $02 $B5 $80 $43 $00 $00 $00 $00
```

### CID Register (Card Identification) - All Fields

| Field | Bits | Raw | Value | Usage |
|-------|------|-----|-------|-------|
| MID | [127:120] | $00 | Budget OEM | **[USED]** |
| OID | [119:104] | $34 $32 | "42" (ASCII) | [INFO] |
| PNM | [103:64] | $53 $44 $31 $36 $47 | "SD16G" | [INFO] |
| PRV | [63:56] | $20 | 2.0 | [INFO] |
| PSN | [55:24] | $0000_03FB | 1,019 | [INFO] |
| MDT | [19:8] | $192 | 2025-02 (February 2025) | [INFO] |
| CRC7 | [7:1] | $6A | $6A | [INFO] |

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
| C_SIZE | [69:48] | $770B | 30,475 (15,238 MB) | **[USED]** |
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
| CRC7 | [7:1] | $27 | $27 | [INFO] |

**Derived Values:**
- Read Timeout: 100 ms (calculated)
- Write Timeout: 250 ms (calculated)
- Total Sectors: 31,207,424
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
| DATA_STAT_AFTER_ERASE | [55] | 1 | Data = 1 after erase | [INFO] |
| SD_SECURITY | [54:52] | 3 | SDHC (security v2.00) | [INFO] |
| SD_BUS_WIDTHS | [51:48] | $5 | 1-bit and 4-bit supported | [INFO] |
| SD_SPEC3 | [47] | 1 | SD 3.0 support: Yes | [INFO] |
| EX_SECURITY | [46:43] | 0 | No extended security | [INFO] |
| SD_SPEC4 | [42] | 0 | SD 4.0 support: No | [INFO] |
| SD_SPECX | [41:38] | 1 | SD 5.x/6.x/7.x indicator | [INFO] |
| CMD_SUPPORT | [33:32] | $00 | — | [INFO] |

**SD Version:** 3.0x (SD_SPEC=2, SD_SPEC3=1)

### Filesystem (FAT32 - Factory)

| Field | Value |
|-------|-------|
| MBR Partition Type | $0C (FAT32 LBA) |
| VBR Sector | 8,192 |
| OEM Name | (blank) |
| Volume Label | (blank) |
| Volume Serial | $0403_0201 |
| FS Type | FAT32 |
| Bytes/Sector | 512 |
| Sectors/Cluster | 64 (32 KB clusters) |
| Reserved Sectors | 576 |
| Number of FATs | 2 |
| Sectors per FAT | 3,808 |
| Root Cluster | 2 |
| Total Sectors | 31,199,232 |
| Data Region Start | Sector 8,192 |
| Total Clusters | 487,360 |

### Speed Characterization Results

| Speed | Half Period | Actual | Phase 1 | Phase 2 | Phase 3 | Result |
|-------|-------------|--------|---------|---------|---------|--------|
| 18 MHz | 6 clocks | 16.6 MHz | 1,000/1,000 | 10,000/10,000 | 800/800 | PASS |
| 20 MHz | 5 clocks | 20.0 MHz | 1,000/1,000 | 10,000/10,000 | 800/800 | PASS |
| 22 MHz | 5 clocks | 20.0 MHz | 1,000/1,000 | 10,000/10,000 | 800/800 | PASS |
| 25 MHz | 4 clocks | 25.0 MHz | 1,000/1,000 | 10,000/10,000 | 800/800 | PASS |
| 27 MHz | 4 clocks | 25.0 MHz | — | — | — | CMD6 FAIL |

**Maximum Reliable SPI Speed:** 25 MHz (limited by SYSCLK=200 MHz, not card)

### Internal Throughput

| Metric | Value |
|--------|-------|
| Test Iterations | 10,000 single-sector reads |
| Elapsed Time | ~13.9 seconds |
| Throughput | **368 KB/s** |
| Average Latency | 1.39 ms/sector |
| Performance Class | MEDIUM |

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

- **Gigastone-branded** card using budget OEM silicon (MID $00)
- MID $00 + OID "42" = common on white-label/rebrand cards
- **Different silicon than 8GB "High Endurance"** - even same product line uses multiple sources
- **"High Endurance"** product line for continuous recording (dashcams, security cameras)
- **MLC** (Multi-Level Cell) flash - more durable than TLC
- **10x** = marketing claim for endurance vs standard cards
- **HC I** = SDHC UHS-I interface
- **U3** = UHS Speed Class 3 (30 MB/s minimum write)
- **V30** = Video Speed Class 30 (30 MB/s sustained write)
- **4K** = Suitable for 4K video recording
- **SD 3.0x spec** (SD_SPEC3=1, SD_SPEC4=0) - older spec than some cards
- CCC $5B5 - standard command classes (no video class bits despite V30 marketing)
- Factory formatted with blank OEM name and volume label
- Very recent manufacture (February 2025)
- Fourth different silicon source found in Gigastone cards
- DATA_STAT_AFTER_ERASE=1 (erased data reads as 1s)
- **MEDIUM throughput (368 KB/s)** - slower than premium cards but reliable
- MLC flash may prioritize endurance over raw speed
- **Benchmark data available** — see Benchmark Results section below; slowest card tested (file write 32 KB at 105 KB/s, raw single-sector write latency 3.6 ms)

### Benchmark Results (Smart Pin SPI + Multi-Sector)

**Test Program**: SD_performance_benchmark.spin2 v2.0 | **SPI**: ~22.8 kHz @ 320 MHz, ~22.5 kHz @ 270 MHz

#### 320 MHz Run

**SysClk**: 320 MHz | **SPI**: 22,857 kHz | **Mount**: 203.0 ms

| Test | Min (us) | Avg (us) | Max (us) | KB/s |
|------|----------|----------|----------|------|
| **Raw Single-Sector** | | | | |
| Read 1x512B | 887 | 906 | 1,072 | **565** |
| Write 1x512B | 3,456 | 3,601 | 4,171 | **142** |
| **Raw Multi-Sector** | | | | |
| Read 8 sectors (4 KB) | 2,687 | 2,708 | 2,872 | **1,512** |
| Read 32 sectors (16 KB) | 8,859 | 8,878 | 9,044 | **1,845** |
| Read 64 sectors (32 KB) | 17,088 | 17,110 | 17,273 | **1,915** |
| Write 8 sectors (4 KB) | 5,173 | 5,390 | 6,028 | **759** |
| Write 32 sectors (16 KB) | 11,211 | 11,294 | 11,930 | **1,450** |
| Write 64 sectors (32 KB) | 18,924 | 19,621 | 25,085 | **1,670** |
| **File-Level** | | | | |
| File Write 512B | 20,607 | 22,072 | 28,404 | **23** |
| File Write 4 KB | 46,168 | 46,545 | 47,058 | **88** |
| File Write 32 KB | 296,680 | 310,332 | 380,277 | **105** |
| File Read 4 KB | 6,129 | 6,221 | 7,047 | **658** |
| File Read 32 KB | 54,665 | 54,835 | 56,337 | **597** |
| File Read 128 KB | 208,092 | 208,292 | 209,816 | **629** |
| File Read 256 KB | 414,901 | 415,200 | 417,118 | **631** |
| **Overhead** | | | | |
| File Open | 95 | 186 | 1,013 | — |
| File Close | 22 | 22 | 22 | — |
| Mount | — | 203,000 | — | — |

Multi-sector improvement: 64x single reads = 57,030 us vs 1x CMD18 = 17,088 us (**70% faster**)

#### 270 MHz Run

**SysClk**: 270 MHz | **SPI**: 22,500 kHz | **Mount**: 204.3 ms

| Test | Min (us) | Avg (us) | Max (us) | KB/s |
|------|----------|----------|----------|------|
| **Raw Single-Sector** | | | | |
| Read 1x512B | 923 | 941 | 1,104 | **544** |
| Write 1x512B | 3,488 | 3,634 | 4,205 | **140** |
| **Raw Multi-Sector** | | | | |
| Read 8 sectors (4 KB) | 2,839 | 2,857 | 3,020 | **1,433** |
| Read 32 sectors (16 KB) | 9,407 | 9,425 | 9,589 | **1,738** |
| Read 64 sectors (32 KB) | 18,165 | 18,183 | 18,347 | **1,802** |
| Write 8 sectors (4 KB) | 5,311 | 5,459 | 6,049 | **750** |
| Write 32 sectors (16 KB) | 11,561 | 11,705 | 12,277 | **1,399** |
| Write 64 sectors (32 KB) | 19,731 | 19,917 | 20,452 | **1,645** |
| **File-Level** | | | | |
| File Write 512B | 20,644 | 21,574 | 22,150 | **23** |
| File Write 4 KB | 46,641 | 56,020 | 128,008 | **73** |
| File Write 32 KB | 299,025 | 312,727 | 384,028 | **104** |
| File Read 4 KB | 6,374 | 6,468 | 7,318 | **633** |
| File Read 32 KB | 55,549 | 55,718 | 57,240 | **588** |
| File Read 128 KB | 213,018 | 213,187 | 214,709 | **614** |
| File Read 256 KB | 425,880 | 426,049 | 427,570 | **615** |
| **Overhead** | | | | |
| File Open | 113 | 206 | 1,051 | — |
| File Close | 26 | 26 | 27 | — |
| Mount | — | 204,300 | — | — |

Multi-sector improvement: 64x single reads = 59,351 us vs 1x CMD18 = 18,165 us (**69% faster**)

#### Sysclk Effect (320 vs 270 MHz)

| Test | 320 MHz (KB/s) | 270 MHz (KB/s) | Delta |
|------|----------------|----------------|-------|
| Raw Read 1x512B | 565 | 544 | -3.7% |
| Raw Read 64x (32 KB) | 1,915 | 1,802 | -5.9% |
| Raw Write 64x (32 KB) | 1,670 | 1,645 | -1.5% |
| File Read 256 KB | 631 | 615 | -2.5% |
| File Write 32 KB | 105 | 104 | -1.0% |

Slowest card tested. File write 32 KB at 105 KB/s is 3.5x slower than the Gigastone Camera Plus (367 KB/s) and 4.8x slower than the Lexar (501 KB/s). Raw single-sector write latency (3,601 us) is 2.8x higher than the Gigastone Camera Plus (1,274 us), indicating a very slow internal flash controller. The 70% multi-sector improvement is the highest observed, because single-sector command overhead is proportionally larger on this slow card. Sysclk effect is muted (1-6%) since the card controller latency dominates.
