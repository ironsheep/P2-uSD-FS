# Card: SanDisk 8GB SDHC (Taiwan)

**Label:** SanDisk 8GB (4) microSD HC, Made in Taiwan
**Unique ID:** `SanDisk_SS08G_3.0_DAAEE8AD_201509`
**Test Date:** 2026-02-02 (characterization)

### Raw Registers

```
CID: $03 $50 $54 $53 $53 $30 $38 $47 $30 $DA $AE $E8 $AD $00 $F9 $BB
CSD: $40 $0E $00 $32 $5B $59 $00 $00 $39 $B7 $7F $80 $0A $40 $00 $3B
OCR: $C0FF_8000
SCR: $02 $35 $80 $02 $01 $00 $00 $00
```

### CID Register (Card Identification) - All Fields

| Field | Bits | Raw | Value | Usage |
|-------|------|-----|-------|-------|
| MID | [127:120] | $03 | SanDisk | **[USED]** |
| OID | [119:104] | $50 $54 | "PT" (Taiwan production?) | [INFO] |
| PNM | [103:64] | $53 $53 $30 $38 $47 | "SS08G" | [INFO] |
| PRV | [63:56] | $30 | 3.0 | [INFO] |
| PSN | [55:24] | $DAAE_E8AD | 3,668,748,461 | [INFO] |
| MDT | [19:8] | $0F9 | 2015-09 (September 2015) | [INFO] |
| CRC7 | [7:1] | $5D | $5D | [INFO] |

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
| C_SIZE | [69:48] | $39B7 | 14,775 (7,388 MB) | **[USED]** |
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
| CRC7 | [7:1] | $1D | $1D | [INFO] |

**Derived Values:**
- Read Timeout: 100 ms (calculated)
- Write Timeout: 250 ms (calculated)
- Total Sectors: 15,130,624
- Capacity: ~7 GB

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
| SD_SPEC4 | [42] | 0 | SD 4.0 support: No | [INFO] |
| SD_SPECX | [41:38] | 0 | — | [INFO] |
| CMD_SUPPORT | [33:32] | $01 | CMD20 Speed Class supported | [INFO] |

**SD Version:** 3.0x (SD_SPEC=2, SD_SPEC3=1, SD_SPEC4=0)

### Filesystem (FAT32)

| Field | Value |
|-------|-------|
| MBR Partition Type | $0B (FAT32 CHS) |
| VBR Sector | 8,192 |
| OEM Name | (blank) |
| Volume Label | NO NAME |
| Volume Serial | $1DBC_FED9 |
| FS Type | FAT32 |
| Bytes/Sector | 512 |
| Sectors/Cluster | 64 (32 KB clusters) |
| Reserved Sectors | 4,500 |
| Number of FATs | 2 |
| Sectors per FAT | 1,846 |
| Root Cluster | 2 |
| Total Sectors | 15,122,432 |
| Total Clusters | 236,160 |

### Test Results

| Test | Status | Notes |
|------|--------|-------|
| Card Init | PASS | CMD0/CMD8/ACMD41 sequence successful |
| CID Read | PASS | Worker cog routing |
| CSD Read | PASS | Worker cog routing |
| SCR Read | PASS | Worker cog routing |
| OCR Read | PASS | Cached during init |
| MBR Read | PASS | FAT32 CHS partition |
| VBR Read | PASS | Valid FAT32 filesystem |
| Mount | READY | FAT32 formatted - ready for use |

### Notes

- **Genuine SanDisk** - MID $03 confirms authentic SanDisk
- **OID "PT"** - Unusual (not "SD" like most SanDisk cards) - possibly Taiwan production code
- **Made in Taiwan** - Label indicates Taiwan manufacturing
- Product name "SS08G" = SanDisk Standard 8GB
- **SDHC** - High Capacity (8GB class, ~7GB usable)
- **SD 3.0x spec** compliant (SD_SPEC3=1, but SD_SPEC4=0)
- **CMD20 supported** - Speed Class command available (unusual for Class 4)
- **COPY=0** - Original card (not a copy)
- **Properly formatted** - FAT32 ready for immediate use with P2 SD driver
- **(4)** = Speed Class 4 (4 MB/s minimum write speed)
- SanDisk recommended for embedded SPI use
- DATA_STAT_AFTER_ERASE=0 (erased data reads as 0s)
