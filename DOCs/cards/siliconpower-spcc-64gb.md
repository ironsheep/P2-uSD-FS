# Card: Silicon Power Elite MicroSD XC 64GB

**Label:** SP Elite microSD XC UHS-I U1 (10)
**Unique ID:** `SharedOEM_SPCC_0.7_00940105_202507`
**Test Date:** 2026-02-17 (characterization)

### Card Designator

```
SharedOEM SPCC SDXC 57GB [FAT32] SD 6.x rev0.7 SN:00940105 2025/07
Class 10, U3, A1, V30, SPI 25 MHz  [P2FMTER]
```

*Note: Physical label says U1, but ACMD13 SD Status register reports U3, A1, V30. Register data is authoritative.*

### Raw Registers

```
CID: $9F $54 $49 $53 $50 $43 $43 $20 $07 $00 $94 $01 $05 $71 $97 $6B
CSD: $40 $0E $00 $32 $DB $59 $00 $01 $CF $9B $7F $80 $0A $40 $00 $81
OCR: $C0FF_8000
SCR: $02 $45 $84 $8F $33 $33 $30 $39
```

### CID Register (Card Identification) - All Fields

| Field | Bits | Raw | Value | Usage |
|-------|------|-----|-------|-------|
| MID | [127:120] | $9F | Shared OEM (Silicon Power) | **[USED]** |
| OID | [119:104] | $54 $49 | "TI" | [INFO] |
| PNM | [103:64] | $53 $50 $43 $43 $20 | "SPCC " | [INFO] |
| PRV | [63:56] | $07 | 0.7 | [INFO] |
| PSN | [55:24] | $0094_0105 | 9,699,589 | [INFO] |
| MDT | [19:8] | $197 | 2025-07 (July 2025) | [INFO] |
| CRC7 | [7:1] | $35 | $35 | [INFO] |

### CSD Register (Card Specific Data) - All Fields

| Field | Bits | Raw | Value | Usage |
|-------|------|-----|-------|-------|
| CSD_STRUCTURE | [127:126] | 1 | CSD Version 2.0 (SDHC/SDXC) | **[USED]** |
| TAAC | [119:112] | $0E | Read access time-1 | **[USED]** |
| NSAC | [111:104] | $00 | 0 CLK cycles | **[USED]** |
| TRAN_SPEED | [103:96] | $32 | 25 MHz max | **[USED]** |
| CCC | [95:84] | $DB5 | Classes 0,2,4,5,7,8,10,11 | [INFO] |
| READ_BL_LEN | [83:80] | 9 | 512 bytes | [INFO] |
| READ_BL_PARTIAL | [79] | 0 | Not allowed | [INFO] |
| WRITE_BLK_MISALIGN | [78] | 0 | Not allowed | [INFO] |
| READ_BLK_MISALIGN | [77] | 0 | Not allowed | [INFO] |
| DSR_IMP | [76] | 0 | DSR not implemented | [INFO] |
| C_SIZE | [69:48] | $1CF9B | 118,683 (59,342 MB) | **[USED]** |
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
| CRC7 | [7:1] | $40 | $40 | [INFO] |

**Derived Values:**
- Read Timeout: 100 ms (calculated)
- Write Timeout: 250 ms (calculated)
- Total Sectors: 121,532,416
- Capacity: ~57 GB

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
| CMD_SUPPORT | [33:32] | $03 | CMD20 + CMD23 supported | [INFO] |

**SD Version:** 6.x (SD_SPEC=2, SD_SPEC3=1, SD_SPEC4=1, SD_SPECX=2)

### Filesystem (FAT32 - formatted with P2FMTER)

| Field | Value |
|-------|-------|
| Factory Format | exFAT ($07) - reformatted to FAT32 |
| MBR Partition Type | $0C (FAT32 LBA) |
| VBR Sector | 8,192 |
| OEM Name | P2FMTER |
| Volume Label | P2-BENCH |
| Volume Serial | $0466_FE25 |
| FS Type | FAT32 |
| Bytes/Sector | 512 |
| Sectors/Cluster | 64 (32 KB clusters) |
| Reserved Sectors | 32 |
| Number of FATs | 2 |
| Sectors per FAT | 14,835 |
| Root Cluster | 2 |
| Total Sectors | 121,524,224 |
| Data Region Start | Sector 29,702 |
| Total Clusters | 1,898,351 |

### Test Results

| Test | Status | Notes |
|------|--------|-------|
| Card Init | PASS | CMD0/CMD8/ACMD41 sequence successful |
| CID Read | PASS | Worker cog routing |
| CSD Read | PASS | Worker cog routing |
| SCR Read | PASS | Worker cog routing |
| OCR Read | PASS | Cached during init |
| SD Status | PASS | ACMD13 (64 bytes) |
| MBR Read | PASS | exFAT partition detected (factory) |
| Format | PASS | FAT32 formatted with P2FMTER |
| Mount | **FAIL** | E_IO_ERROR (-7) — CMD18 warmup read times out (see below) |
| Benchmark | **BLOCKED** | Cannot mount — characterization incomplete |
| Regression | **BLOCKED** | Cannot mount — characterization incomplete |

### SPI Speed Characterization

**Test Date:** 2026-02-17
**Test Configuration:**
- SYSCLK: 200 MHz
- Phase 1: 1,000 single-sector reads (quick check)
- Phase 2: 10,000 single-sector reads (statistical confidence)
- Phase 3: 100 x 8-sector multi-block reads (800 sectors, sustained transfer)
- Total reads per speed level: 11,800 sector reads
- Test sectors: 1,000,000 to 1,010,000 (safe area away from FAT)
- CRC-16 verification on every read

**Results:**

| Target | Half Period | Actual | Delta | Phase 1 | Phase 2 | Phase 3 | Status |
|--------|-------------|--------|-------|---------|---------|---------|--------|
| 18 MHz | 6 clocks | 16.6 MHz | -7.4% | 1,000 OK | 10,000 OK | 0/800 (timeout) | **FAIL** |

**Summary:**
- **Single-sector reads (CMD17): 100% reliable** at 16.6 MHz (11,000 reads, 0 CRC errors)
- **Multi-block reads (CMD18): 100% timeout** — all 800 sectors timed out waiting for data token ($FE)
- Characterizer stopped at 18 MHz because Phase 3 failed

### Outstanding Issue: CMD18 Multi-Block Read Timeout

**Status:** OPEN — investigation required before characterization can continue

**Symptom:** CMD18 (READ_MULTIPLE_BLOCK) times out on this card. The card never sends the $FE data token after CMD18 is issued. Single-sector CMD17 reads work perfectly (11,000 consecutive reads, 0 errors). CMD18 fails 100% of the time.

**Register contradiction:** The card's CCC register ($DB5) includes Class 2 (block read commands), which explicitly covers CMD18. The card advertises CMD18 support. The SCR CMD_SUPPORT field ($03) also indicates CMD23 (SET_BLOCK_COUNT) support — a command specifically designed for defined-length multi-block transfers.

**Mount failure chain:** The driver's `do_mount()` function (micro_sd_fat32_fs.spin2:1173) issues a CMD18 warmup read after successfully reading MBR, VBR, and FSInfo. Because CMD18 times out, mount returns E_IO_ERROR (-7) even though all single-sector reads succeeded. The mount test confirmed: volumeLabel() returns "P2-BENCH" (VBR was cached before the warmup), but mount() returns failure.

**Investigation leads:**
1. Does this card require CMD23 (SET_BLOCK_COUNT) before CMD18? Some cards that advertise CMD23 support may require it.
2. Is there a difference in how the card responds to CMD18 in SPI mode vs SD mode?
3. Does CMD25 (multi-block write) also fail, or is it CMD18-specific?
4. Is the CMD18 R1 response $00 (accepted) before the data token timeout?

**Impact:** Characterization incomplete — benchmark and regression testing blocked until mount works. This card cannot be fully evaluated until the CMD18 issue is resolved.

### Notes

- **MID $9F** - "Shared OEM" code, not assigned to a specific manufacturer in the SD Association registry. Silicon Power Computer Communications (SPCC) is a Taiwanese company that contracts flash manufacturing.
- **OID "TI"** ($54 $49) — contract manufacturer identifier (not Texas Instruments)
- **PNM "SPCC"** — Silicon Power Computer Communications Corporation
- **Label discrepancy**: Physical label says "UHS-I U1 (10)" but ACMD13 SD Status reports U3, A1, V30. The card may be marketed conservatively or the label may be for a different SKU in the same product line.
- **CCC $DB5** includes Class 11 (video speed class) — consistent with V30 rating
- **CMD_SUPPORT $03** — supports both CMD20 (speed class control) and CMD23 (set block count). CMD23 can improve multi-sector write setup.
- **SD 6.x** spec compliant (SD_SPEC4=1, SD_SPECX=2) — newest spec version we've seen
- **DATA_STAT_AFTER_ERASE=0** (erased data reads as 0s)
- Very recent manufacture (July 2025)
- Factory formatted with exFAT — needs FAT32 reformat before use with P2 SD driver
