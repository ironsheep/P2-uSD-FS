# Card: SanDisk Extreme 64GB SDXC

**Label:** SanDisk Extreme 64GB U3 A2 microSD XC I V30
**Unique ID:** `SanDisk_SN64G_8.6_7E650771_202211`
**Test Date:** 2026-02-17 (full re-characterization)

### Card Designator

```
SanDisk SN64G SDXC 59GB [FAT32] SD 6.x rev8.6 SN:7E650771 2022/11
Class 10, U3, A2, V30, SPI 25 MHz  [P2FMTER]
```

### Raw Registers

```
CID: $03 $53 $44 $53 $4E $36 $34 $47 $86 $7E $65 $07 $71 $01 $6B $8D
CSD: $40 $0E $00 $32 $DB $79 $00 $01 $DB $D3 $7F $80 $0A $40 $40 $F9
OCR: $C0FF_8000
SCR: $02 $45 $84 $87 $00 $00 $00 $00
```

### CID Register (Card Identification) - All Fields

| Field | Bits | Raw | Value | Usage |
|-------|------|-----|-------|-------|
| MID | [127:120] | $03 | SanDisk / Western Digital | **[USED]** |
| OID | [119:104] | $53 $44 | "SD" (ASCII) | [INFO] |
| PNM | [103:64] | $53 $4E $36 $34 $47 | "SN64G" | [INFO] |
| PRV | [63:56] | $86 | 8.6 | [INFO] |
| PSN | [55:24] | $7E65_0771 | 2,120,025,969 | [INFO] |
| MDT | [19:8] | $16B | 2022-11 (November 2022) | [INFO] |
| CRC7 | [7:1] | $46 | $46 | [INFO] |

### CSD Register (Card Specific Data) - All Fields

| Field | Bits | Raw | Value | Usage |
|-------|------|-----|-------|-------|
| CSD_STRUCTURE | [127:126] | 1 | CSD Version 2.0 (SDHC/SDXC) | **[USED]** |
| TAAC | [119:112] | $0E | Read access time-1 | **[USED]** |
| NSAC | [111:104] | $00 | 0 CLK cycles | **[USED]** |
| TRAN_SPEED | [103:96] | $32 | 25 MHz max | **[USED]** |
| CCC | [95:84] | $DB7 | Classes 0,1,2,4,5,7,8,10 | [INFO] |
| READ_BL_LEN | [83:80] | 9 | 512 bytes | [INFO] |
| READ_BL_PARTIAL | [79] | 0 | Not allowed | [INFO] |
| WRITE_BLK_MISALIGN | [78] | 0 | Not allowed | [INFO] |
| READ_BLK_MISALIGN | [77] | 0 | Not allowed | [INFO] |
| DSR_IMP | [76] | 0 | DSR not implemented | [INFO] |
| C_SIZE | [69:48] | $1DBD3 | 121,811 (60,906 MB) | **[USED]** |
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
| CRC7 | [7:1] | $7C | $7C | [INFO] |

**Derived Values:**
- Read Timeout: 100 ms (calculated)
- Write Timeout: 250 ms (calculated)
- Total Sectors: 124,735,488
- Capacity: ~59 GB

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
| CMD_SUPPORT | [33:32] | $00 | — | [INFO] |

**SD Version:** 6.x (SD_SPEC=2, SD_SPEC3=1, SD_SPEC4=1, SD_SPECX=2)

### Filesystem (FAT32 - reformatted with P2FMTER)

| Field | Value |
|-------|-------|
| MBR Partition Type | $0C (FAT32 LBA) |
| VBR Sector | 8,192 |
| OEM Name | P2FMTER |
| Volume Label | P2-BENCH |
| Volume Serial | $058C_662D |
| FS Type | FAT32 |
| Bytes/Sector | 512 |
| Sectors/Cluster | 64 (32 KB clusters) |
| Reserved Sectors | 32 |
| Number of FATs | 2 |
| Sectors per FAT | 15,226 |
| Root Cluster | 2 |
| Total Sectors | 124,727,296 |
| Data Region Start | Sector 30,484 |
| Total Clusters | 1,948,387 |

### Test Results

| Test | Status | Notes |
|------|--------|-------|
| Card Init | PASS | CMD0/CMD8/ACMD41 sequence successful |
| CID Read | PASS | Worker cog routing |
| CSD Read | PASS | Worker cog routing |
| SCR Read | PASS | Worker cog routing |
| OCR Read | PASS | Cached during init |
| SD Status | PASS | ACMD13 (64 bytes) — Class 10, U3, A2, V30 |
| MBR Read | PASS | FAT32 LBA ($0C) partition |
| VBR Read | PASS | |
| Format | PASS | FAT32 formatted with P2FMTER |
| Mount | PASS | (confirmed via benchmark) |
| FSCK | PASS | 0 errors, 0 repairs, CLEAN |

### Performance Benchmarks (350 MHz sysclk)

**SPI Clock:** 25,000 kHz | **Mount:** 233.3 ms | **Volume:** P2-BENCH | **Free:** 60,901 MB

| Test | Size | Min | Avg | Max | Throughput |
|------|------|-----|-----|-----|------------|
| **Raw Single-Sector** | | | | | |
| Read (1x512B) | 512B | 511 us | 511 us | 512 us | 1,001 KB/s |
| Write (1x512B) | 512B | 1,532 us | 1,846 us | 4,340 us | 277 KB/s |
| **Raw Multi-Sector Read (CMD18)** | | | | | |
| 8 sectors | 4 KB | 1,960 us | 1,960 us | 1,961 us | 2,089 KB/s |
| 32 sectors | 16 KB | 6,949 us | 6,949 us | 6,950 us | 2,357 KB/s |
| 64 sectors | 32 KB | 13,595 us | 13,595 us | 13,595 us | 2,410 KB/s |
| **Raw Multi-Sector Write (CMD25)** | | | | | |
| 8 sectors | 4 KB | 2,434 us | 2,493 us | 2,676 us | 1,643 KB/s |
| 32 sectors | 16 KB | 7,670 us | 7,740 us | 7,877 us | 2,116 KB/s |
| 64 sectors | 32 KB | 15,151 us | 15,278 us | 15,404 us | 2,144 KB/s |
| **File Write** | | | | | |
| create+write+close | 512B | 8,932 us | 9,064 us | 9,236 us | 56 KB/s |
| create+write+close | 4 KB | 17,805 us | 18,049 us | 20,042 us | 226 KB/s |
| create+write+close | 32 KB | 87,817 us | 88,354 us | 89,852 us | 370 KB/s |
| **File Read** | | | | | |
| open+read+close | 4 KB | 5,663 us | 5,664 us | 5,668 us | 723 KB/s |
| open+read+close | 32 KB | 33,074 us | 33,081 us | 33,087 us | 990 KB/s |
| open+read+close | 128 KB | 128,282 us | 128,302 us | 128,433 us | 1,021 KB/s |
| open+read+close | 256 KB | 254,005 us | 254,073 us | 254,512 us | 1,031 KB/s |
| **Overhead** | | | | | |
| File Open | — | 1,707 us | 1,707 us | 1,708 us | — |
| File Close | — | 20 us | 20 us | 21 us | — |
| Unmount | — | — | 3 ms | — | — |

**Multi-sector gain:** 64x single=32,642 us vs 1x multi=13,590 us → **58% improvement**

### Performance Benchmarks (250 MHz sysclk)

**SPI Clock:** 25,000 kHz | **Mount:** 235.7 ms | **Volume:** P2-BENCH | **Free:** 60,901 MB

| Test | Size | Min | Avg | Max | Throughput |
|------|------|-----|-----|-----|------------|
| **Raw Single-Sector** | | | | | |
| Read (1x512B) | 512B | 573 us | 573 us | 574 us | 893 KB/s |
| Write (1x512B) | 512B | 1,591 us | 1,619 us | 1,648 us | 316 KB/s |
| **Raw Multi-Sector Read (CMD18)** | | | | | |
| 8 sectors | 4 KB | 2,152 us | 2,152 us | 2,153 us | 1,903 KB/s |
| 32 sectors | 16 KB | 7,546 us | 7,546 us | 7,546 us | 2,171 KB/s |
| 64 sectors | 32 KB | 14,731 us | 14,731 us | 14,731 us | 2,224 KB/s |
| **Raw Multi-Sector Write (CMD25)** | | | | | |
| 8 sectors | 4 KB | 2,650 us | 2,907 us | 4,769 us | 1,409 KB/s |
| 32 sectors | 16 KB | 8,291 us | 8,366 us | 8,546 us | 1,958 KB/s |
| 64 sectors | 32 KB | 16,408 us | 16,510 us | 16,618 us | 1,984 KB/s |
| **File Write** | | | | | |
| create+write+close | 512B | 9,546 us | 9,698 us | 9,823 us | 52 KB/s |
| create+write+close | 4 KB | 18,511 us | 18,656 us | 18,761 us | 219 KB/s |
| create+write+close | 32 KB | 91,205 us | 91,970 us | 93,718 us | 356 KB/s |
| **File Read** | | | | | |
| open+read+close | 4 KB | 6,394 us | 6,394 us | 6,400 us | 640 KB/s |
| open+read+close | 32 KB | 36,870 us | 36,873 us | 36,877 us | 888 KB/s |
| open+read+close | 128 KB | 141,792 us | 141,861 us | 142,338 us | 923 KB/s |
| open+read+close | 256 KB | 281,920 us | 282,015 us | 282,479 us | 929 KB/s |
| **Overhead** | | | | | |
| File Open | — | 1,980 us | 1,980 us | 1,981 us | — |
| File Close | — | 29 us | 29 us | 29 us | — |
| Unmount | — | — | 3 ms | — | — |

**Multi-sector gain:** 64x single=36,997 us vs 1x multi=14,737 us → **60% improvement**

### Sysclk Effect (350 vs 250 MHz, same 25 MHz SPI)

| Metric | 350 MHz | 250 MHz | Delta |
|--------|---------|---------|-------|
| Raw read 1x (KB/s) | 1,001 | 893 | +12% |
| Raw read 64x (KB/s) | 2,410 | 2,224 | +8% |
| Raw write 64x (KB/s) | 2,144 | 1,984 | +8% |
| File read 256KB (KB/s) | 1,031 | 929 | +11% |
| File write 32KB (KB/s) | 370 | 356 | +4% |
| Multi-sector gain | 58% | 60% | — |

### Notes

- **Genuine SanDisk** - MID $03 + OID "SD" = authentic SanDisk/Western Digital product
- **"Extreme"** product line - premium consumer tier
- **XC I** = SDXC UHS-I interface (up to 104 MB/s bus speed)
- **A2** = Application Performance Class 2 (4000 read IOPS, 2000 write IOPS) - higher than A1
- **V30** = Video Speed Class 30 (30 MB/s sustained sequential write)
- **U3** = UHS Speed Class 3 (30 MB/s minimum write speed)
- Product name "SN64G" = SanDisk Nomenclature 64GB
- Originally shipped with exFAT, now reformatted with P2FMTER for FAT32
- Volume label "P2-BENCH" indicates use as benchmark/test card
- **SD 6.x** spec compliant (SD_SPEC4=1, SD_SPECX=2) — newest spec generation
- **A2** confirmed by ACMD13 SD Status register (previously known from label only)
- **CMD_SUPPORT $00** — no CMD20 or CMD23 support
- **COPY flag = 1** — may indicate factory content was pre-loaded
- **Full benchmark data** — 350+250 MHz, see tables above and BENCHMARK-RESULTS.md
