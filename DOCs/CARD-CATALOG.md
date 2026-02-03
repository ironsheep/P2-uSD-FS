# SD Card Catalog

This catalog documents SD cards tested with the P2 SD card driver. Cards are characterized using the `SD_card_characterize_v3.spin2` diagnostic tool.

---

## Register Field Reference

This section documents ALL fields available from SD card registers and indicates which are actively used by the V3 driver.

- **[USED]** = Field is actively used by the V3 driver for operation
- **[INFO]** = Informational only - available but not used by driver

### CID Register (Card Identification) - 16 bytes

| Field | Bits | Size | Usage | Description |
|-------|------|------|-------|-------------|
| MID | [127:120] | 8 bits | **[USED]** | Manufacturer ID - determines PNY cards ($27) for 20MHz limit |
| OID | [119:104] | 16 bits | [INFO] | OEM/Application ID (2 ASCII chars) |
| PNM | [103:64] | 40 bits | [INFO] | Product Name (5 ASCII chars) |
| PRV | [63:56] | 8 bits | [INFO] | Product Revision (BCD: major.minor) |
| PSN | [55:24] | 32 bits | [INFO] | Product Serial Number |
| MDT | [19:8] | 12 bits | [INFO] | Manufacturing Date (year + month) |
| CRC7 | [7:1] | 7 bits | [INFO] | CRC checksum |

**V3 Driver Usage:** MID is checked to identify PNY/Phison cards ($27) which require reduced SPI clock (20MHz vs 25MHz).

### CSD Register (Card Specific Data) - 16 bytes

| Field | Bits | Size | Usage | Description |
|-------|------|------|-------|-------------|
| CSD_STRUCTURE | [127:126] | 2 bits | **[USED]** | CSD version: 0=v1.0 (SDSC), 1=v2.0 (SDHC/SDXC) |
| TAAC | [119:112] | 8 bits | **[USED]** | Data read access time-1 (for timeout calculation) |
| NSAC | [111:104] | 8 bits | **[USED]** | Data read access time-2 in CLK cycles |
| TRAN_SPEED | [103:96] | 8 bits | **[USED]** | Max data transfer rate (determines SPI clock) |
| CCC | [95:84] | 12 bits | [INFO] | Card Command Classes supported |
| READ_BL_LEN | [83:80] | 4 bits | [INFO] | Max read data block length (always 9=512 bytes) |
| READ_BL_PARTIAL | [79] | 1 bit | [INFO] | Partial blocks for read allowed |
| WRITE_BLK_MISALIGN | [78] | 1 bit | [INFO] | Write block misalignment |
| READ_BLK_MISALIGN | [77] | 1 bit | [INFO] | Read block misalignment |
| DSR_IMP | [76] | 1 bit | [INFO] | DSR implemented |
| **CSD v1.0 only (SDSC):** |
| C_SIZE | [73:62] | 12 bits | **[USED]** | Device size (SDSC capacity calculation) |
| VDD_R_CURR_MIN | [61:59] | 3 bits | [INFO] | Max read current @ VDD min |
| VDD_R_CURR_MAX | [58:56] | 3 bits | [INFO] | Max read current @ VDD max |
| VDD_W_CURR_MIN | [55:53] | 3 bits | [INFO] | Max write current @ VDD min |
| VDD_W_CURR_MAX | [52:50] | 3 bits | [INFO] | Max write current @ VDD max |
| C_SIZE_MULT | [49:47] | 3 bits | **[USED]** | Device size multiplier (SDSC capacity) |
| **CSD v2.0 only (SDHC/SDXC):** |
| C_SIZE | [69:48] | 22 bits | **[USED]** | Device size (capacity = (C_SIZE+1) × 512KB) |
| **Common fields (both versions):** |
| ERASE_BLK_EN | [46] | 1 bit | [INFO] | Erase single block enable |
| SECTOR_SIZE | [45:39] | 7 bits | [INFO] | Erase sector size |
| WP_GRP_SIZE | [38:32] | 7 bits | [INFO] | Write protect group size |
| WP_GRP_ENABLE | [31] | 1 bit | [INFO] | Write protect group enable |
| R2W_FACTOR | [28:26] | 3 bits | **[USED]** | Write speed factor (write time = read time × 2^R2W) |
| WRITE_BL_LEN | [25:22] | 4 bits | [INFO] | Max write data block length |
| WRITE_BL_PARTIAL | [21] | 1 bit | [INFO] | Partial blocks for write allowed |
| FILE_FORMAT_GRP | [15] | 1 bit | [INFO] | File format group |
| COPY | [14] | 1 bit | [INFO] | Copy flag |
| PERM_WRITE_PROTECT | [13] | 1 bit | [INFO] | Permanent write protection |
| TMP_WRITE_PROTECT | [12] | 1 bit | [INFO] | Temporary write protection |
| FILE_FORMAT | [11:10] | 2 bits | [INFO] | File format |
| CRC7 | [7:1] | 7 bits | [INFO] | CRC checksum |

**V3 Driver Usage:**
- CSD_STRUCTURE determines SDSC vs SDHC/SDXC capacity formulas
- TRAN_SPEED calculates maximum SPI clock frequency
- TAAC/NSAC/R2W_FACTOR calculate read/write timeouts (SDSC only)
- C_SIZE + multipliers calculate card capacity in sectors

### SCR Register (SD Configuration) - 8 bytes

| Field | Bits | Size | Usage | Description |
|-------|------|------|-------|-------------|
| SCR_STRUCTURE | [63:60] | 4 bits | [INFO] | SCR structure version |
| SD_SPEC | [59:56] | 4 bits | **[USED]** | SD Physical Layer spec version |
| DATA_STAT_AFTER_ERASE | [55] | 1 bit | [INFO] | Data status after erase |
| SD_SECURITY | [54:52] | 3 bits | [INFO] | CPRM security support |
| SD_BUS_WIDTHS | [51:48] | 4 bits | [INFO] | DAT bus widths supported (bit 0=1-bit, bit 2=4-bit) |
| SD_SPEC3 | [47] | 1 bit | [INFO] | SD spec 3.0 support |
| EX_SECURITY | [46:43] | 4 bits | [INFO] | Extended security support |
| SD_SPEC4 | [42] | 1 bit | [INFO] | SD spec 4.0 support |
| SD_SPECX | [41:38] | 4 bits | [INFO] | SD spec 5.x/6.x/7.x indicator |
| CMD_SUPPORT | [33:32] | 2 bits | [INFO] | Command support bits |

**V3 Driver Usage:** SD_SPEC determines High Speed (CMD6) support availability.

### OCR Register (Operating Conditions) - 4 bytes

| Field | Bits | Size | Usage | Description |
|-------|------|------|-------|-------------|
| Power Up Status | [31] | 1 bit | [INFO] | Card power up status (1=ready) |
| CCS | [30] | 1 bit | **[USED]** | **CRITICAL** - Card Capacity Status |
| UHS-II Status | [29] | 1 bit | [INFO] | UHS-II card status |
| S18A | [24] | 1 bit | [INFO] | Switching to 1.8V accepted |
| Voltage Window | [23:15] | 9 bits | [INFO] | Supported voltage range (2.7V-3.6V) |

**V3 Driver Usage:** CCS bit is **CRITICAL** - determines addressing mode:
- CCS=0: SDSC byte addressing (sector << 9)
- CCS=1: SDHC/SDXC block addressing (sector number directly)

### Summary: Fields Used by V3 Driver

| Register | Field | Purpose |
|----------|-------|---------|
| CID | MID | PNY card detection (20MHz limit) |
| CSD | CSD_STRUCTURE | SDSC vs SDHC/SDXC formulas |
| CSD | TRAN_SPEED | SPI clock frequency |
| CSD | TAAC | Read timeout (SDSC) |
| CSD | NSAC | Read timeout (SDSC) |
| CSD | R2W_FACTOR | Write timeout calculation |
| CSD | C_SIZE | Capacity calculation |
| CSD | C_SIZE_MULT | Capacity calculation (SDSC) |
| SCR | SD_SPEC | High Speed (CMD6) support |
| OCR | CCS | **Addressing mode** (byte vs block) |

---

## Summary Table

**Speed Rating Key** (based on register values, not marketing claims):
- **A** = Video-optimized (CCC=$DB7 with Classes 1+11 for sustained writes)
- **B** = Fast (Premium brand, SD 4.xx spec, 25 MHz)
- **C** = Standard (SD 3.0x spec, 25 MHz)
- **D** = Limited (MID $27 triggers 20 MHz SPI limit)

| Card ID | Manufacturer | Product | Capacity | Speed | Test Status |
|---------|-------------|---------|----------|:-----:|-------------|
| SanDisk_SN64G_8.6_7E650771_202211 | SanDisk ($03) | SN64G | ~59 GB | **A** | PASS |
| SanDisk_SN128_8.0_F79E34F6_201912 | SanDisk ($03) | SN128 | ~119 GB | **A** | PASS |
| Longsys/Lexar_MSSD0_6.1_31899471_202411 | Lexar (Longsys $AD) | MSSD0 | ~58 GB | **A** | PASS |
| Samsung_GD4QT_3.0_C0305565_201805 | Samsung ($1B) | GD4QT | ~119 GB | **B** | PASS |
| SanDisk_AGGCF_8.0_E05C352B_201707 | SanDisk ($03) | AGGCF | ~119 GB | **B** | PASS |
| SanDisk_AGGCE_8.0_DD1C1144_201703 | SanDisk ($03) | AGGCE | ~59 GB | **B** | PASS |
| GigastoneOEM_ASTC_2.0_00000F14_202306 | Gigastone (OEM $12) | ASTC | ~58 GB | **B** | PASS |
| Transcend_00000_0.0_000001C9_202307 | Gigastone (Transcend $74) | 00000 | ~29 GB | **C** | PASS |
| Unknown_00000_0.0_0001B9D5_202109 | Gigastone (Shared OEM $9F) | 00000 | ~7 GB | **C** | PASS |
| BudgetOEM_SD16G_2.0_000003FB_202502 | Gigastone (Budget OEM $00) | SD16G | ~14 GB | **C** | PASS |
| Kingston_SD8GB_3.0_43F65DC9_201504 | Kingston ($41) | SD8GB | ~7 GB | **C** | PASS |
| SanDisk_SU08G_8.0_0AA81F11_201010 | Unknown (claims SanDisk $03) - Chinese #1 | SU08G | ~7 GB | **C** | PASS |
| Samsung_00000_1.0_D9FB539C_201408 | Samsung ($1B) - Chinese #2 | 00000 | ~7 GB | **C** | PASS |
| SanDisk_SS08G_3.0_DAAEE8AD_201509 | SanDisk ($03) - Taiwan | SS08G | ~7 GB | **C** | PASS |
| Phison_SD16G_3.0_01CD5CF5_201808 | PNY (Phison $27) | SD16G | ~14 GB | **D** | PARTIAL |

---

## Internal Throughput Summary

Cards tested with `SD_speed_characterize_v3.spin2` have measured internal throughput data. This reflects the card's internal flash/controller performance, NOT the SPI bus speed.

**Test Methodology:**
- 10,000 random single-sector reads at 25 MHz SPI
- CRC-16 verification on every read
- Throughput = sectors read / elapsed time

**Performance Class Definitions:**
- **HIGH** = >500 KB/s (fast controller, suitable for demanding applications)
- **MEDIUM** = 100-500 KB/s (adequate for most embedded uses)
- **LOW** = <100 KB/s (slow controller, SPI clock not the bottleneck)

| Card | Manufacturer | Capacity | Throughput | Latency | Class | Max SPI |
|------|-------------|----------|------------|---------|-------|---------|
| SanDisk_SN64G | SanDisk ($03) | ~59 GB | — | — | — | not yet tested |
| SanDisk_SN128 | SanDisk ($03) | ~119 GB | **780 KB/s** | 0.66 ms | HIGH | 25 MHz |
| Longsys_MSSD0 | Lexar ($AD) | ~58 GB | **1,059 KB/s** | 0.48 ms | HIGH | 25 MHz |
| Samsung_GD4QT | Samsung ($1B) | ~119 GB | **783 KB/s** | 0.65 ms | HIGH | 25 MHz |
| SanDisk_AGGCF | SanDisk ($03) | ~119 GB | — | — | — | not yet tested |
| SanDisk_AGGCE | SanDisk ($03) | ~59 GB | **866 KB/s** | 0.59 ms | HIGH | 25 MHz |
| GigastoneOEM_ASTC | Gigastone ($12) | ~58 GB | **944 KB/s** | 0.54 ms | HIGH | 25 MHz |
| Transcend_00000 | Gigastone ($74) | ~29 GB | — | — | — | not yet tested |
| Unknown_00000 | Gigastone ($9F) | ~7 GB | — | — | — | not yet tested |
| BudgetOEM_SD16G | Gigastone ($00) | ~14 GB | **368 KB/s** | 1.39 ms | MEDIUM | 25 MHz |
| Kingston_SD8GB | Kingston ($41) | ~7 GB | — | — | — | not yet tested |
| SanDisk_SU08G | Chinese #1 ($03) | ~7 GB | — | — | — | not yet tested |
| Samsung_00000 | Chinese #2 ($1B) | ~7 GB | — | — | — | not yet tested |
| SanDisk_SS08G | SanDisk Taiwan ($03) | ~7 GB | — | — | — | not yet tested |
| Phison_SD16G | PNY ($27) | ~14 GB | **31.3 KB/s** | 16.0 ms | LOW | 25 MHz |

**Tested: 7 of 15 cards**

**Key Observations:**
1. **Lexar V30 U3 64GB** - **Fastest card tested** (1,059 KB/s), 12% faster than Gigastone
2. **Gigastone Camera Plus 64GB** - Very fast (944 KB/s), excellent value
3. **SanDisk Extreme PRO 64GB** - High performance (866 KB/s), professional line
4. **Samsung EVO Select 128GB** - High performance (783 KB/s), nearly identical to SanDisk
5. **SanDisk Nintendo Switch 128GB** - High performance (780 KB/s), designed for gaming
6. **Gigastone High Endurance 16GB** - Medium performance (368 KB/s), MLC flash endurance focus
7. **PNY 16GB** - Slow internal controller (31 KB/s); runs reliably at 25 MHz SPI but throughput limited by internal latency
8. All tested cards max out at 25 MHz SPI (CMD6 High Speed switch fails on all)

---

## Card: Gigastone "Camera Plus" 64GB SDXC

**Label:** Gigastone "Camera Plus" microSD XC I, A1 V30 U3 64GB
**Unique ID:** `GigastoneOEM_ASTC_2.0_00000F14_202306`
**Test Date:** 2026-02-02 (V3 characterization)

### Raw Registers

```
CID: $12 $34 $56 $41 $53 $54 $43 $00 $20 $00 $00 $0F $14 $01 $76 $4D
CSD: $40 $0E $00 $32 $5B $59 $00 $01 $D1 $EB $7F $80 $0A $40 $00 $A1
OCR: $C0FF_8000
SCR: $02 $C5 $84 $83 $00 $00 $00 $00
```

### CID Register (Card Identification) - All Fields

| Field | Bits | Raw | Value | Usage |
|-------|------|-----|-------|-------|
| MID | [127:120] | $12 | Gigastone OEM | **[USED]** |
| OID | [119:104] | $34 $56 | "4V" (ASCII) | [INFO] |
| PNM | [103:64] | $41 $53 $54 $43 $00 | "ASTC" | [INFO] |
| PRV | [63:56] | $20 | 2.0 | [INFO] |
| PSN | [55:24] | $0000_0F14 | 3,860 | [INFO] |
| MDT | [19:8] | $176 | 2023-06 (June 2023) | [INFO] |
| CRC7 | [7:1] | $26 | $26 | [INFO] |

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
| C_SIZE | [69:48] | $1D1EB | 119,275 (59,638 MB) | **[USED]** |
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
| CRC7 | [7:1] | $50 | $50 | [INFO] |

**Derived Values:**
- Read Timeout: 100 ms (calculated)
- Write Timeout: 250 ms (calculated)
- Total Sectors: 122,138,624
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
| DATA_STAT_AFTER_ERASE | [55] | 1 | Data = 1 after erase | [INFO] |
| SD_SECURITY | [54:52] | 4 | SDXC (security v3.xx) | [INFO] |
| SD_BUS_WIDTHS | [51:48] | $5 | 1-bit and 4-bit supported | [INFO] |
| SD_SPEC3 | [47] | 1 | SD 3.0 support: Yes | [INFO] |
| EX_SECURITY | [46:43] | 0 | No extended security | [INFO] |
| SD_SPEC4 | [42] | 1 | SD 4.0 support: Yes | [INFO] |
| SD_SPECX | [41:38] | 2 | SD 5.x/6.x/7.x indicator | [INFO] |
| CMD_SUPPORT | [33:32] | $00 | — | [INFO] |

**SD Version:** 4.xx (SD_SPEC=2, SD_SPEC3=1, SD_SPEC4=1)

### Filesystem (FAT32 - formatted with P2FMTER)

| Field | Value |
|-------|-------|
| MBR Partition Type | $0C (FAT32 LBA) |
| VBR Sector | 8,192 |
| OEM Name | P2FMTER |
| Volume Label | P2-XFER |
| Volume Serial | $0371_6EA1 |
| FS Type | FAT32 |
| Bytes/Sector | 512 |
| Sectors/Cluster | 64 (32 KB clusters) |
| Reserved Sectors | 32 |
| Number of FATs | 2 |
| Sectors per FAT | 14,909 |
| Root Cluster | 2 |
| Total Sectors | 122,130,432 |
| Data Region Start | Sector 29,850 |
| Total Clusters | 1,907,821 |

### Test Results

| Test | Status | Notes |
|------|--------|-------|
| Card Init | PASS | CMD0/CMD8/ACMD41 sequence successful |
| CID Read | PASS | V3 worker cog routing |
| CSD Read | PASS | V3 worker cog routing |
| SCR Read | PASS | V3 worker cog routing |
| OCR Read | PASS | Cached during init |
| MBR Read | PASS | FAT32 LBA ($0C) partition |
| VBR Read | PASS | |
| Mount | PASS | |
| V3 Regression | PASS | All tests pass |

### Notes

- **Gigastone** (gigastone.com) - Taiwanese flash memory manufacturer
- MID $12 = Gigastone OEM (Patriot-related OEM in some databases)
- **"Camera Plus"** product line marketed for action cameras and drones
- **XC I** = SDXC UHS-I interface (up to 104 MB/s bus speed)
- **A1** = Application Performance Class 1 (1500 read IOPS, 500 write IOPS)
- **V30** = Video Speed Class 30 (30 MB/s sustained sequential write)
- **U3** = UHS Speed Class 3 (30 MB/s minimum write speed)
- **SD 4.xx spec** compliant (SD_SPEC4=1)
- CCC $5B5 - standard command classes (no video class bits despite V30 marketing)
- DATA_STAT_AFTER_ERASE=1 (erased data reads as 1s)
- Formatted with P2FMTER (P2 Flash Filesystem Formatter)
- Currently used as scratch/test card for development

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
| Phase 2 Duration (10,000 reads) | 5.42 seconds |
| Throughput | **944 KB/s** |
| Sectors/second | 1,845 |
| Latency per sector | 0.54 ms |

**Performance Class:** HIGH - Fastest card tested so far (21% faster than SanDisk Nintendo Switch).

---

## Card: Lexar MicroSD XC V30 U3 64GB

**Label:** Lexar MicroSD XC V30 U3 64GB (10) 100 MB/s*
**Unique ID:** `Longsys/Lexar_MSSD0_6.1_31899471_202411`
**Test Date:** 2026-02-02 (V3 characterization)

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
| CID Read | PASS | V3 worker cog routing |
| CSD Read | PASS | V3 worker cog routing |
| SCR Read | PASS | V3 worker cog routing |
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

---

## Card: Gigastone 32GB SDHC

**Label:** Gigastone 32GB microSD HC I A1 U1 (10)
**Unique ID:** `Transcend_00000_0.0_000001C9_202307`
**Test Date:** 2026-02-02 (V3 characterization)

### Raw Registers

```
CID: $74 $4A $60 $30 $30 $30 $30 $30 $00 $00 $00 $01 $C9 $01 $77 $09
CSD: $40 $0E $00 $32 $5B $59 $00 $00 $E8 $F5 $7F $80 $0A $40 $00 $D1
OCR: $C0FF_8000
SCR: $02 $B5 $80 $83 $00 $00 $00 $00
```

### CID Register (Card Identification) - All Fields

| Field | Bits | Raw | Value | Usage |
|-------|------|-----|-------|-------|
| MID | [127:120] | $74 | Transcend | **[USED]** |
| OID | [119:104] | $4A $60 | "J`" | [INFO] |
| PNM | [103:64] | $30 $30 $30 $30 $30 | "00000" | [INFO] |
| PRV | [63:56] | $00 | 0.0 | [INFO] |
| PSN | [55:24] | $0000_01C9 | 457 | [INFO] |
| MDT | [19:8] | $177 | 2023-07 (July 2023) | [INFO] |
| CRC7 | [7:1] | $04 | $04 | [INFO] |

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
| C_SIZE | [69:48] | $E8F5 | 59,637 (29,819 MB) | **[USED]** |
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
| CRC7 | [7:1] | $68 | $68 | [INFO] |

**Derived Values:**
- Read Timeout: 100 ms (calculated)
- Write Timeout: 250 ms (calculated)
- Total Sectors: 61,069,312
- Capacity: ~29 GB

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
| SD_SPECX | [41:38] | 2 | SD 5.x/6.x/7.x indicator | [INFO] |
| CMD_SUPPORT | [33:32] | $00 | — | [INFO] |

**SD Version:** 3.0x (SD_SPEC=2, SD_SPEC3=1)

### Filesystem (FAT32 - reformatted with P2FMTER)

| Field | Value |
|-------|-------|
| MBR Partition Type | $0C (FAT32 LBA) |
| VBR Sector | 8,192 |
| OEM Name | P2FMTER |
| Volume Label | P2-XFER |
| Volume Serial | $04AA_C74F |
| FS Type | FAT32 |
| Bytes/Sector | 512 |
| Sectors/Cluster | 32 (16 KB clusters) |
| Reserved Sectors | 32 |
| Number of FATs | 2 |
| Sectors per FAT | 14,908 |
| Root Cluster | 2 |
| Total Sectors | 61,061,120 |
| Data Region Start | Sector 29,848 |
| Total Clusters | 1,907,227 |

### Test Results

| Test | Status | Notes |
|------|--------|-------|
| Card Init | PASS | CMD0/CMD8/ACMD41 sequence successful |
| CID Read | PASS | V3 worker cog routing |
| CSD Read | PASS | V3 worker cog routing |
| SCR Read | PASS | V3 worker cog routing |
| OCR Read | PASS | Cached during init |
| MBR Read | PASS | FAT32 LBA ($0C) partition |
| VBR Read | PASS | |
| Mount | PASS | |
| V3 Regression | PASS | All tests pass |

### Notes

- **Gigastone-branded** card using **Transcend** flash/controller silicon (MID $74)
- **HC I** = SDHC UHS-I interface (up to 104 MB/s bus speed)
- **A1** = Application Performance Class 1 (1500 read IOPS, 500 write IOPS)
- **U1** = UHS Speed Class 1 (10 MB/s minimum write speed)
- **(10)** = Speed Class 10 (10 MB/s minimum write speed)
- Product name "00000" and revision "0.0" = OEM/white-label product
- Originally formatted with SD Card Formatter (MSWIN4.1), now P2FMTER
- 16 KB cluster size (smaller than typical 32 KB)
- DATA_STAT_AFTER_ERASE=1 (erased data reads as 1s)

---

## Card: Gigastone "High Endurance" 8GB SDHC MLC

**Label:** Gigastone 10x High Endurance 8GB MLC microSD HC I U1
**Unique ID:** `SharedOEM_00000_0.0_0001B9D5_202109`
**Test Date:** 2026-02-01 (V3 characterization)

### Raw Registers

```
CID: $9F $54 $49 $30 $30 $30 $30 $30 $00 $00 $01 $B9 $D5 $01 $59 $93
CSD: $40 $0E $00 $32 $5B $59 $00 $00 $3A $0D $7F $80 $0A $40 $00 $8D
OCR: $C0FF_8000
SCR: $02 $B5 $80 $43 $00 $00 $00 $00
```

### CID Register (Card Identification) - All Fields

| Field | Bits | Raw | Value | Usage |
|-------|------|-----|-------|-------|
| MID | [127:120] | $9F | Shared OEM (Amazon, Kingston, Kodak, Silicon Power) | **[USED]** |
| OID | [119:104] | $54 $49 | "TI" (ASCII) | [INFO] |
| PNM | [103:64] | $30 $30 $30 $30 $30 | "00000" | [INFO] |
| PRV | [63:56] | $00 | 0.0 | [INFO] |
| PSN | [55:24] | $0001_B9D5 | 113,109 | [INFO] |
| MDT | [19:8] | $159 | 2021-09 (September 2021) | [INFO] |
| CRC7 | [7:1] | $49 | $49 | [INFO] |

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
| C_SIZE | [69:48] | $3A0D | 14,861 (7,431 MB) | **[USED]** |
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
| CRC7 | [7:1] | $46 | $46 | [INFO] |

**Derived Values:**
- Read Timeout: 100 ms (calculated)
- Write Timeout: 250 ms (calculated)
- Total Sectors: 15,218,688
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
| DATA_STAT_AFTER_ERASE | [55] | 1 | Data = 1 after erase | [INFO] |
| SD_SECURITY | [54:52] | 3 | SDHC (security v2.00) | [INFO] |
| SD_BUS_WIDTHS | [51:48] | $5 | 1-bit and 4-bit supported | [INFO] |
| SD_SPEC3 | [47] | 1 | SD 3.0 support: Yes | [INFO] |
| EX_SECURITY | [46:43] | 0 | No extended security | [INFO] |
| SD_SPEC4 | [42] | 0 | SD 4.0 support: No | [INFO] |
| SD_SPECX | [41:38] | 1 | SD 5.x/6.x/7.x indicator | [INFO] |
| CMD_SUPPORT | [33:32] | $00 | — | [INFO] |

**SD Version:** 3.0x

### Filesystem (FAT32)

| Field | Value |
|-------|-------|
| MBR Partition Type | $0C (FAT32 LBA) |
| VBR Sector | 8,192 |
| OEM Name | P2FMTER |
| Volume Label | P2-XFER |
| Volume Serial | $0562_1275 |
| FS Type | FAT32 |
| Bytes/Sector | 512 |
| Sectors/Cluster | 8 (4 KB clusters) |
| Reserved Sectors | 32 |
| Number of FATs | 2 |
| Sectors per FAT | 14,854 |
| Root Cluster | 2 |
| Total Sectors | 15,210,496 |
| Data Region Start | Sector 29,740 |
| Total Clusters | 1,897,594 |

### Test Results

| Test | Status | Notes |
|------|--------|-------|
| Card Init | PASS | CMD0/CMD8/ACMD41 sequence successful |
| CID Read | PASS | V3 worker cog routing |
| CSD Read | PASS | V3 worker cog routing |
| SCR Read | PASS | V3 worker cog routing |
| OCR Read | PASS | Cached during init |
| MBR Read | PASS | FAT32 LBA ($0C) partition |
| VBR Read | PASS | |
| Mount | PASS | |
| V3 Regression | PASS | All tests pass |

### Notes

- **Gigastone-branded** card using shared OEM silicon (MID $9F)
- MID $9F + OID "TI" = common across Amazon, Kingston, Kodak, Micro Center, Silicon Power
- Likely a large contract manufacturer supplying multiple brands
- **"High Endurance"** product line for continuous recording (dashcams, security cameras)
- **MLC** (Multi-Level Cell) flash - more durable than TLC for write-intensive applications
- **10x** = marketing claim for endurance vs standard cards
- **HC I** = SDHC UHS-I interface
- **U1** = UHS Speed Class 1 (10 MB/s minimum write)
- Product name "00000" and revision "0.0" = OEM/white-label product
- Currently formatted with P2FMTER and used as primary development card
- Third different silicon source found in Gigastone cards

---

## Card: Gigastone "High Endurance" 16GB SDHC MLC

**Label:** Gigastone 10x High Endurance 16GB MLC microSD HC I U3 V30 4K
**Unique ID:** `BudgetOEM_SD16G_2.0_000003FB_202502`
**Test Date:** 2026-02-02 (V3 characterization)

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
| R2W_FACTOR | [28:26] | 2 | Write = Read × 4 | **[USED]** |
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
| CID Read | PASS | V3 worker cog routing |
| CSD Read | PASS | V3 worker cog routing |
| SCR Read | PASS | V3 worker cog routing |
| OCR Read | PASS | Cached during init |
| MBR Read | PASS | FAT32 LBA ($0C) partition |
| VBR Read | PASS | |
| Mount | PASS | |
| V3 Regression | PASS | All tests pass |

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

---

## Card: SanDisk Extreme 64GB SDXC

**Label:** SanDisk Extreme 64GB U3 A2 microSD XC I V30
**Unique ID:** `SanDisk_SN64G_8.6_7E650771_202211`
**Test Date:** 2026-02-02 (V3 characterization)

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
| R2W_FACTOR | [28:26] | 2 | Write = Read × 4 | **[USED]** |
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

**SD Version:** 4.xx (SD_SPEC=2, SD_SPEC3=1, SD_SPEC4=1)

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
| CID Read | PASS | V3 worker cog routing |
| CSD Read | PASS | V3 worker cog routing |
| SCR Read | PASS | V3 worker cog routing |
| OCR Read | PASS | Cached during init |
| MBR Read | PASS | FAT32 LBA ($0C) partition |
| VBR Read | PASS | |
| Mount | PASS | |
| V3 Regression | PASS | All tests pass |

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
- SD 4.xx spec compliant (SD_SPEC4=1)

---

## Card: PNY 16GB SDHC

**Label:** PNY 16GB microSD HC I
**Unique ID:** `Phison_SD16G_3.0_01CD5CF5_201808`
**Test Date:** 2026-02-02 (V3 characterization)

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
| R2W_FACTOR | [28:26] | 2 | Write = Read × 4 | **[USED]** |
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
| CID Read | PASS | V3 worker cog routing |
| CSD Read | PASS | V3 worker cog routing |
| SCR Read | PASS | V3 worker cog routing |
| OCR Read | PASS | Cached during init |
| MBR Read | PASS | FAT32 LBA ($0C) partition |
| VBR Read | PASS | |
| Mount | PASS | |
| V3 Regression | PASS | All tests pass with V3 driver |

### Notes

- **PNY-branded** card using **Phison** controller silicon (MID $27)
- MID $27 + OID "PH" = Phison controller family (also used by Delkin, HP, Kingston, Lexar older, PNY)
- **V3 driver handles this card correctly** - MID $27 detected, SPI clock limited to 20 MHz
- **HC I** = SDHC UHS-I interface
- Older card (manufactured August 2018)
- CMD20 (Speed Class) supported per SCR CMD_SUPPORT field
- 8 KB clusters (smaller than typical 32 KB) due to P2FMTER formatting
- V3 driver fixes resolved previous V2 unmount hang issue

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
| 27 MHz | 4 clocks | 25.0 MHz | -7.4% | 0 OK, 1,000 timeout | — | — | **100%** |

**Summary:**
- **Maximum Reliable Speed: 25 MHz** (47,200 total sector reads at 0% failure rate)
- **25 MHz PASSED all phases** - 11,800 reads with 0 CRC errors, 0 timeouts
- CMD6 High Speed mode: Switch **failed** at 27 MHz, card became unresponsive (same behavior as SanDisk)
- Card has significantly slower internal processing than SanDisk cards (~20x slower throughput)

**Key Finding:** The 20 MHz limit for Phison/PNY cards is **overly conservative**. This card successfully ran at 25 MHz with **0 CRC errors across 47,200 reads** (4 speed levels × 11,800 reads each). The TRAN_SPEED register ($32 = 25 MHz) accurately reflects this card's capability.

**Recommendation:** The V3 driver's MID $27 → 20 MHz limit should be reconsidered. This PNY/Phison card handles 25 MHz reliably. The failure mode at 27 MHz (after CMD6 switch attempt) is identical to SanDisk cards - the CMD6 High Speed switch fails and the card becomes unresponsive, not a gradual degradation from clock speed.

### Internal Throughput (measured at 25 MHz SPI)

| Metric | Value |
|--------|-------|
| Phase 2 Duration (10,000 reads) | 160.1 seconds |
| Throughput | **31.3 KB/s** |
| Sectors/second | 62.5 |
| Latency per sector | 16.0 ms |

**Performance Class:** LOW - Slow internal controller (~25x slower than high-performance cards). SPI clock speed is not the bottleneck; internal flash/controller latency dominates.

---

## Card: "Chinese Made" #1 8GB SDHC (claims SanDisk)

**Label:** microSD HC 8GB (4) - Chinese text, no brand - Card #1
**Unique ID:** `SanDisk_SU08G_8.0_0AA81F11_201010`
**Test Date:** 2026-02-02 (V3 characterization)

### Raw Registers

```
CID: $03 $53 $44 $53 $55 $30 $38 $47 $80 $0A $A8 $1F $11 $00 $AA $03
CSD: $40 $0E $00 $32 $5B $59 $00 $00 $3B $37 $7F $80 $0A $40 $40 $AF
OCR: $C0FF_8000
SCR: $02 $35 $80 $01 $00 $00 $00 $00
```

### CID Register (Card Identification) - All Fields

| Field | Bits | Raw | Value | Usage |
|-------|------|-----|-------|-------|
| MID | [127:120] | $03 | Claims SanDisk (suspicious) | **[USED]** |
| OID | [119:104] | $53 $44 | "SD" (SanDisk) | [INFO] |
| PNM | [103:64] | $53 $55 $30 $38 $47 | "SU08G" | [INFO] |
| PRV | [63:56] | $80 | 8.0 | [INFO] |
| PSN | [55:24] | $0AA8_1F11 | 178,987,793 | [INFO] |
| MDT | [19:8] | $0AA | 2010-10 (October 2010) | [INFO] |
| CRC7 | [7:1] | $01 | $01 | [INFO] |

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
| C_SIZE | [69:48] | $3B37 | 15,159 (7,580 MB) | **[USED]** |
| ERASE_BLK_EN | [46] | 1 | 512-byte erase supported | [INFO] |
| SECTOR_SIZE | [45:39] | 127 | Erase sector = 64 KB | [INFO] |
| WP_GRP_SIZE | [38:32] | 0 | — | [INFO] |
| WP_GRP_ENABLE | [31] | 0 | Disabled | [INFO] |
| R2W_FACTOR | [28:26] | 2 | Write = Read × 4 | **[USED]** |
| WRITE_BL_LEN | [25:22] | 9 | 512 bytes | [INFO] |
| WRITE_BL_PARTIAL | [21] | 0 | Not allowed | [INFO] |
| FILE_FORMAT_GRP | [15] | 0 | — | [INFO] |
| COPY | [14] | 1 | Copy | [INFO] |
| PERM_WRITE_PROTECT | [13] | 0 | Not protected | [INFO] |
| TMP_WRITE_PROTECT | [12] | 0 | Not protected | [INFO] |
| FILE_FORMAT | [11:10] | 0 | Hard disk-like | [INFO] |
| CRC7 | [7:1] | $57 | $57 | [INFO] |

**Derived Values:**
- Read Timeout: 100 ms (calculated)
- Write Timeout: 250 ms (calculated)
- Total Sectors: 15,523,840
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
| CMD_SUPPORT | [33:32] | $00 | — | [INFO] |

**SD Version:** 3.0x (SD_SPEC=2, SD_SPEC3=1, SD_SPEC4=0)

### Filesystem (Corrupt FAT32)

| Field | Value |
|-------|-------|
| MBR Partition Type | $0C (FAT32 LBA) |
| VBR Sector | 2,048 |
| OEM Name | mkdosfs (Linux formatter residue) |
| Status | **CORRUPT** - VBR fields contain garbage |

**Note:** Card was formatted with Linux `mkdosfs` but filesystem is corrupt. Needs FAT32 reformatting before use.

### Test Results

| Test | Status | Notes |
|------|--------|-------|
| Card Init | PASS | CMD0/CMD8/ACMD41 sequence successful |
| CID Read | PASS | V3 worker cog routing |
| CSD Read | PASS | V3 worker cog routing |
| SCR Read | PASS | V3 worker cog routing |
| OCR Read | PASS | Cached during init |
| MBR Read | PASS | FAT32 LBA ($0C) partition |
| VBR Read | PASS | But data is corrupt |
| Mount | N/A | Needs FAT32 reformat first |

### Notes

- **First "Chinese" card** - See also "Chinese Made" #2 (MID $1B, genuine Samsung)
- **Unlabeled card** with speed class 4 marking, Chinese text
- CID claims **SanDisk** (MID $03, OEM "SD") - suspicious for no-name card
- Possible scenarios:
  1. Genuine SanDisk card that was relabeled/repackaged
  2. Counterfeit card spoofing SanDisk's MID (common with fakes)
  3. Recycled/refurbished SanDisk flash with new controller
- **Very old** - manufactured October 2010 (15+ years old)
- **COPY=1** - Card marked as a copy (not original)
- **SD 3.0x spec** compliant (SD_SPEC3=1, but SD_SPEC4=0)
- Filesystem corrupt - needs FAT32 reformatting before use
- **(4)** = Speed Class 4 (4 MB/s minimum write speed) - slowest class
- DATA_STAT_AFTER_ERASE=0 (erased data reads as 0s)

---

## Card: Kingston 8GB SDHC

**Label:** Kingston 8GB microSD HC I ui (10) "Taiwan" F(c)o
**Unique ID:** `Kingston_SD8GB_3.0_43F65DC9_201504`
**Test Date:** 2026-02-02 (V3 characterization)

### Raw Registers

```
CID: $41 $34 $32 $53 $44 $38 $47 $42 $30 $43 $F6 $5D $C9 $00 $F4 $A1
CSD: $40 $0E $00 $32 $5B $59 $00 $00 $3A $8F $7F $80 $0A $40 $00 $87
OCR: $C0FF_8000
SCR: $02 $B5 $80 $00 $00 $00 $00 $00
```

### CID Register (Card Identification) - All Fields

| Field | Bits | Raw | Value | Usage |
|-------|------|-----|-------|-------|
| MID | [127:120] | $41 | Kingston | **[USED]** |
| OID | [119:104] | $34 $32 | "42" (ASCII) | [INFO] |
| PNM | [103:64] | $53 $44 $38 $47 $42 | "SD8GB" | [INFO] |
| PRV | [63:56] | $30 | 3.0 | [INFO] |
| PSN | [55:24] | $43F6_5DC9 | 1,140,530,633 | [INFO] |
| MDT | [19:8] | $0F4 | 2015-04 (April 2015) | [INFO] |
| CRC7 | [7:1] | $50 | $50 | [INFO] |

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
| C_SIZE | [69:48] | $3A8F | 14,991 (7,496 MB) | **[USED]** |
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
| CRC7 | [7:1] | $43 | $43 | [INFO] |

**Derived Values:**
- Read Timeout: 100 ms (calculated)
- Write Timeout: 250 ms (calculated)
- Total Sectors: 15,351,808
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
| DATA_STAT_AFTER_ERASE | [55] | 1 | Data = 1 after erase | [INFO] |
| SD_SECURITY | [54:52] | 3 | SDHC (security v2.00) | [INFO] |
| SD_BUS_WIDTHS | [51:48] | $5 | 1-bit and 4-bit supported | [INFO] |
| SD_SPEC3 | [47] | 1 | SD 3.0 support: Yes | [INFO] |
| EX_SECURITY | [46:43] | 0 | No extended security | [INFO] |
| SD_SPEC4 | [42] | 0 | SD 4.0 support: No | [INFO] |
| SD_SPECX | [41:38] | 0 | — | [INFO] |
| CMD_SUPPORT | [33:32] | $00 | — | [INFO] |

**SD Version:** 3.0x (SD_SPEC=2, SD_SPEC3=1)

### Filesystem (FAT32 - Factory CHS format)

| Field | Value |
|-------|-------|
| MBR Partition Type | $0B (FAT32 CHS) |
| VBR Sector | 8,192 |
| OEM Name | SD |
| Volume Label | (blank) |
| Volume Serial | $0403_0201 |
| FS Type | FAT32 |
| Bytes/Sector | 512 |
| Sectors/Cluster | 64 (32 KB clusters) |
| Reserved Sectors | 4,446 |
| Number of FATs | 2 |
| Sectors per FAT | 1,873 |
| Root Cluster | 2 |
| Total Sectors | 15,343,616 |
| Data Region Start | Sector 8,192 |
| Total Clusters | 239,616 |

### Test Results

| Test | Status | Notes |
|------|--------|-------|
| Card Init | PASS | CMD0/CMD8/ACMD41 sequence successful |
| CID Read | PASS | V3 worker cog routing |
| CSD Read | PASS | V3 worker cog routing |
| SCR Read | PASS | V3 worker cog routing |
| OCR Read | PASS | Cached during init |
| MBR Read | PASS | FAT32 CHS ($0B) partition |
| VBR Read | PASS | |
| Mount | PASS | |
| V3 Regression | PASS | All tests pass |

### Notes

- **Genuine Kingston** card - MID $41 is registered Kingston
- **HC I** = SDHC UHS-I interface
- **ui** = UHS-I marking on label
- **(10)** = Speed Class 10 (10 MB/s minimum write speed)
- **"Taiwan"** = Country of manufacture
- **F(c)o** = likely FCC compliance marking
- **SD 3.0x spec** (SD_SPEC3=1, SD_SPEC4=0, SD_SPECX=0) - older spec
- CCC $5B5 - standard command classes
- Manufactured April 2015 (~10 years old)
- Kingston is a recommended brand for embedded SPI use
- Uses FAT32 CHS partition type ($0B) vs LBA ($0C)
- DATA_STAT_AFTER_ERASE=1 (erased data reads as 1s)

---

## Card: Samsung EVO Select 128GB SDXC

**Label:** Samsung EVO Select microSD XC I U3
**Unique ID:** `Samsung_GD4QT_3.0_C0305565_201805`
**Test Date:** 2026-02-02 (V3 characterization)

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
| R2W_FACTOR | [28:26] | 2 | Write = Read × 4 | **[USED]** |
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
| CID Read | PASS | V3 worker cog routing |
| CSD Read | PASS | V3 worker cog routing |
| SCR Read | PASS | V3 worker cog routing |
| OCR Read | PASS | Cached during init |
| MBR Read | PASS | FAT32 LBA ($0C) partition |
| VBR Read | PASS | |
| Mount | PASS | |
| V3 Regression | PASS | All tests pass |

### Notes

- **Genuine Samsung** - MID $1B + OID "SM" confirms authentic Samsung
- **EVO Select** = Amazon-exclusive variant of EVO Plus line
- **XC I** = SDXC UHS-I interface (up to 104 MB/s bus speed)
- **U3** = UHS Speed Class 3 (30 MB/s minimum write speed)
- Used in DJI Phantom 4 drone - known reliable for video recording
- Originally factory formatted with exFAT, now reformatted with P2FMTER
- Largest card in catalog (128GB / ~119GB usable)
- DATA_STAT_AFTER_ERASE=1 (erased data reads as 1s, not 0s)

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
| Phase 2 Duration (10,000 reads) | 6.53 seconds |
| Throughput | **783 KB/s** |
| Sectors/second | 1,531 |
| Latency per sector | 0.65 ms |

**Performance Class:** HIGH - Very similar to SanDisk Nintendo Switch (within 0.4%).

---

## Card: SanDisk Extreme PRO 128GB SDXC

**Label:** SanDisk Extreme PRO 128GB microSD XC I V30 U3 A1
**Unique ID:** `SanDisk_AGGCF_8.0_E05C352B_201707`
**Test Date:** 2026-02-02 (V3 characterization)

### Raw Registers

```
CID: $03 $53 $44 $41 $47 $47 $43 $46 $80 $E0 $5C $35 $2B $01 $17 $F9
CSD: $40 $0E $00 $32 $5B $59 $00 $03 $B8 $AB $7F $80 $0A $40 $40 $79
OCR: $C0FF_8000
SCR: $02 $45 $84 $43 $00 $00 $00 $00
```

### CID Register (Card Identification) - All Fields

| Field | Bits | Raw | Value | Usage |
|-------|------|-----|-------|-------|
| MID | [127:120] | $03 | SanDisk | **[USED]** |
| OID | [119:104] | $53 $44 | "SD" (SanDisk) | [INFO] |
| PNM | [103:64] | $41 $47 $47 $43 $46 | "AGGCF" | [INFO] |
| PRV | [63:56] | $80 | 8.0 | [INFO] |
| PSN | [55:24] | $E05C_352B | 3,764,118,827 | [INFO] |
| MDT | [19:8] | $117 | 2017-07 (July 2017) | [INFO] |
| CRC7 | [7:1] | $7C | $7C | [INFO] |

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
| C_SIZE | [69:48] | $3B8AB | 243,883 (121,942 MB) | **[USED]** |
| ERASE_BLK_EN | [46] | 1 | 512-byte erase supported | [INFO] |
| SECTOR_SIZE | [45:39] | 127 | Erase sector = 64 KB | [INFO] |
| WP_GRP_SIZE | [38:32] | 0 | — | [INFO] |
| WP_GRP_ENABLE | [31] | 0 | Disabled | [INFO] |
| R2W_FACTOR | [28:26] | 2 | Write = Read × 4 | **[USED]** |
| WRITE_BL_LEN | [25:22] | 9 | 512 bytes | [INFO] |
| WRITE_BL_PARTIAL | [21] | 0 | Not allowed | [INFO] |
| FILE_FORMAT_GRP | [15] | 0 | — | [INFO] |
| COPY | [14] | 1 | Copy | [INFO] |
| PERM_WRITE_PROTECT | [13] | 0 | Not protected | [INFO] |
| TMP_WRITE_PROTECT | [12] | 0 | Not protected | [INFO] |
| FILE_FORMAT | [11:10] | 0 | Hard disk-like | [INFO] |
| CRC7 | [7:1] | $3C | $3C | [INFO] |

**Derived Values:**
- Read Timeout: 100 ms (calculated)
- Write Timeout: 250 ms (calculated)
- Total Sectors: 249,737,216
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
| DATA_STAT_AFTER_ERASE | [55] | 0 | Data = 0 after erase | [INFO] |
| SD_SECURITY | [54:52] | 4 | SDXC (security v3.xx) | [INFO] |
| SD_BUS_WIDTHS | [51:48] | $5 | 1-bit and 4-bit supported | [INFO] |
| SD_SPEC3 | [47] | 1 | SD 3.0 support: Yes | [INFO] |
| EX_SECURITY | [46:43] | 0 | No extended security | [INFO] |
| SD_SPEC4 | [42] | 1 | SD 4.0 support: Yes | [INFO] |
| SD_SPECX | [41:38] | 1 | SD 5.x/6.x/7.x indicator | [INFO] |
| CMD_SUPPORT | [33:32] | $00 | — | [INFO] |

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
| CID Read | PASS | V3 worker cog routing |
| CSD Read | PASS | V3 worker cog routing |
| SCR Read | PASS | V3 worker cog routing |
| OCR Read | PASS | Cached during init |
| MBR Read | PASS | exFAT partition detected |
| Mount | N/A | Requires FAT32 format first |

### Notes

- **Genuine SanDisk** - MID $03 + OID "SD" confirms authentic SanDisk/Western Digital
- **Extreme PRO** = SanDisk's premium professional line (highest tier)
- **XC I** = SDXC UHS-I interface (up to 104 MB/s bus speed)
- **V30** = Video Speed Class 30 (30 MB/s sustained sequential write)
- **U3** = UHS Speed Class 3 (30 MB/s minimum write speed)
- **A1** = Application Performance Class 1 (1500 read IOPS, 500 write IOPS)
- Product name "AGGCF" = internal SanDisk nomenclature
- **CCC $5B5** - Standard classes only (no video stream class unlike Nintendo Switch edition)
- **SD 4.xx spec** compliant (SD_SPEC4=1)
- Factory formatted with exFAT (not FAT32)
- Needs FAT32 reformat before use with P2 SD driver
- SanDisk recommended for embedded SPI use
- DATA_STAT_AFTER_ERASE=0 (erased data reads as 0s)

---

## Card: SanDisk Extreme PRO 64GB SDXC

**Label:** SanDisk Extreme PRO 64GB microSD XC I V30 U3
**Unique ID:** `SanDisk_AGGCE_8.0_DD1C1144_201703`
**Test Date:** 2026-02-02 (V3 characterization)

### Raw Registers

```
CID: $03 $53 $44 $41 $47 $47 $43 $45 $80 $DD $1C $11 $44 $01 $13 $55
CSD: $40 $0E $00 $32 $5B $59 $00 $01 $DB $D3 $7F $80 $0A $40 $40 $DF
OCR: $C0FF_8000
SCR: $02 $45 $84 $43 $00 $00 $00 $00
```

### CID Register (Card Identification) - All Fields

| Field | Bits | Raw | Value | Usage |
|-------|------|-----|-------|-------|
| MID | [127:120] | $03 | SanDisk | **[USED]** |
| OID | [119:104] | $53 $44 | "SD" (SanDisk) | [INFO] |
| PNM | [103:64] | $41 $47 $47 $43 $45 | "AGGCE" | [INFO] |
| PRV | [63:56] | $80 | 8.0 | [INFO] |
| PSN | [55:24] | $DD1C_1144 | 3,709,178,180 | [INFO] |
| MDT | [19:8] | $113 | 2017-03 (March 2017) | [INFO] |
| CRC7 | [7:1] | $2A | $2A | [INFO] |

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
| C_SIZE | [69:48] | $1DBD3 | 121,811 (60,906 MB) | **[USED]** |
| ERASE_BLK_EN | [46] | 1 | 512-byte erase supported | [INFO] |
| SECTOR_SIZE | [45:39] | 127 | Erase sector = 64 KB | [INFO] |
| WP_GRP_SIZE | [38:32] | 0 | — | [INFO] |
| WP_GRP_ENABLE | [31] | 0 | Disabled | [INFO] |
| R2W_FACTOR | [28:26] | 2 | Write = Read × 4 | **[USED]** |
| WRITE_BL_LEN | [25:22] | 9 | 512 bytes | [INFO] |
| WRITE_BL_PARTIAL | [21] | 0 | Not allowed | [INFO] |
| FILE_FORMAT_GRP | [15] | 0 | — | [INFO] |
| COPY | [14] | 1 | Copy | [INFO] |
| PERM_WRITE_PROTECT | [13] | 0 | Not protected | [INFO] |
| TMP_WRITE_PROTECT | [12] | 0 | Not protected | [INFO] |
| FILE_FORMAT | [11:10] | 0 | Hard disk-like | [INFO] |
| CRC7 | [7:1] | $6F | $6F | [INFO] |

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
| SD_SPECX | [41:38] | 1 | SD 5.x/6.x/7.x indicator | [INFO] |
| CMD_SUPPORT | [33:32] | $00 | — | [INFO] |

**SD Version:** 4.xx (SD_SPEC=2, SD_SPEC3=1, SD_SPEC4=1)

### Filesystem (Factory - exFAT)

| Field | Value |
|-------|-------|
| MBR Partition Type | $07 (exFAT/NTFS) |
| Factory Format | exFAT (default for 64GB+) |

**Note:** Card ships with exFAT filesystem. Needs FAT32 format for P2 SD driver compatibility.

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
| Elapsed Time | ~5.9 seconds |
| Throughput | **866 KB/s** |
| Average Latency | 0.59 ms/sector |
| Performance Class | HIGH |

### Test Results

| Test | Status | Notes |
|------|--------|-------|
| Card Init | PASS | CMD0/CMD8/ACMD41 sequence successful |
| CID Read | PASS | V3 worker cog routing |
| CSD Read | PASS | V3 worker cog routing |
| SCR Read | PASS | V3 worker cog routing |
| OCR Read | PASS | Cached during init |
| MBR Read | PASS | exFAT partition detected |
| Mount | N/A | Requires FAT32 format first |

### Notes

- **Genuine SanDisk** - MID $03 + OID "SD" confirms authentic SanDisk/Western Digital
- **Extreme PRO** = SanDisk's premium professional line (highest tier)
- **XC I** = SDXC UHS-I interface (up to 104 MB/s bus speed)
- **V30** = Video Speed Class 30 (30 MB/s sustained sequential write)
- **U3** = UHS Speed Class 3 (30 MB/s minimum write speed)
- Product name "AGGCE" = internal SanDisk nomenclature (64GB variant)
- Related to AGGCF (128GB variant) - same product line, different capacity
- **CCC $5B5** - Standard classes only (no video stream class)
- **SD 4.xx spec** compliant (SD_SPEC4=1)
- Factory formatted with exFAT (not FAT32)
- Needs FAT32 reformat before use with P2 SD driver
- **High throughput (866 KB/s)** - excellent for embedded applications
- DATA_STAT_AFTER_ERASE=0 (erased data reads as 0s)
- Manufactured March 2017 (4 months earlier than 128GB AGGCF variant)

---

## Card: SanDisk Nintendo Switch 128GB SDXC

**Label:** SanDisk 128GB Nintendo Switch microSD XC I
**Unique ID:** `SanDisk_SN128_8.0_F79E34F6_201912`
**Test Date:** 2026-02-02 (V3 characterization)

### Raw Registers

```
CID: $03 $53 $44 $53 $4E $31 $32 $38 $80 $F7 $9E $34 $F6 $01 $3C $63
CSD: $40 $0E $00 $32 $DB $79 $00 $03 $B8 $AB $7F $80 $0A $40 $40 $5F
OCR: $C0FF_8000
SCR: $02 $45 $84 $87 $00 $00 $00 $00
```

### CID Register (Card Identification) - All Fields

| Field | Bits | Raw | Value | Usage |
|-------|------|-----|-------|-------|
| MID | [127:120] | $03 | SanDisk | **[USED]** |
| OID | [119:104] | $53 $44 | "SD" (SanDisk) | [INFO] |
| PNM | [103:64] | $53 $4E $31 $32 $38 | "SN128" | [INFO] |
| PRV | [63:56] | $80 | 8.0 | [INFO] |
| PSN | [55:24] | $F79E_34F6 | 4,154,373,366 | [INFO] |
| MDT | [19:8] | $13C | 2019-12 (December 2019) | [INFO] |
| CRC7 | [7:1] | $31 | $31 | [INFO] |

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
| C_SIZE | [69:48] | $3B8AB | 243,883 (121,942 MB) | **[USED]** |
| ERASE_BLK_EN | [46] | 1 | 512-byte erase supported | [INFO] |
| SECTOR_SIZE | [45:39] | 127 | Erase sector = 64 KB | [INFO] |
| WP_GRP_SIZE | [38:32] | 0 | — | [INFO] |
| WP_GRP_ENABLE | [31] | 0 | Disabled | [INFO] |
| R2W_FACTOR | [28:26] | 2 | Write = Read × 4 | **[USED]** |
| WRITE_BL_LEN | [25:22] | 9 | 512 bytes | [INFO] |
| WRITE_BL_PARTIAL | [21] | 0 | Not allowed | [INFO] |
| FILE_FORMAT_GRP | [15] | 0 | — | [INFO] |
| COPY | [14] | 1 | Copy | [INFO] |
| PERM_WRITE_PROTECT | [13] | 0 | Not protected | [INFO] |
| TMP_WRITE_PROTECT | [12] | 0 | Not protected | [INFO] |
| FILE_FORMAT | [11:10] | 0 | Hard disk-like | [INFO] |
| CRC7 | [7:1] | $2F | $2F | [INFO] |

**Derived Values:**
- Read Timeout: 100 ms (calculated)
- Write Timeout: 250 ms (calculated)
- Total Sectors: 249,737,216
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
| DATA_STAT_AFTER_ERASE | [55] | 0 | Data = 0 after erase | [INFO] |
| SD_SECURITY | [54:52] | 4 | SDXC (security v3.xx) | [INFO] |
| SD_BUS_WIDTHS | [51:48] | $5 | 1-bit and 4-bit supported | [INFO] |
| SD_SPEC3 | [47] | 1 | SD 3.0 support: Yes | [INFO] |
| EX_SECURITY | [46:43] | 0 | No extended security | [INFO] |
| SD_SPEC4 | [42] | 1 | SD 4.0 support: Yes | [INFO] |
| SD_SPECX | [41:38] | 2 | SD 5.x/6.x/7.x indicator | [INFO] |
| CMD_SUPPORT | [33:32] | $00 | — | [INFO] |

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
| CID Read | PASS | V3 worker cog routing |
| CSD Read | PASS | V3 worker cog routing |
| SCR Read | PASS | V3 worker cog routing |
| OCR Read | PASS | Cached during init |
| MBR Read | PASS | exFAT partition detected |
| Mount | N/A | Requires FAT32 format first |

### Notes

- **Genuine SanDisk** - MID $03 + OID "SD" confirms authentic SanDisk/Western Digital
- **Nintendo Switch Edition** = Officially licensed Nintendo-branded SanDisk card
- Product name "SN128" = SanDisk Nintendo Switch 128GB
- **XC I** = SDXC UHS-I interface (up to 104 MB/s bus speed)
- **CCC $DB7** includes Class 1 (stream read) and Class 11 (video speed class) - video-optimized
- **SD 4.xx spec** compliant (SD_SPEC4=1)
- Features Nintendo mushroom (Super Mario) branding on label
- Designed for Nintendo Switch gaming console storage expansion
- Factory formatted with exFAT (not FAT32)
- Needs FAT32 reformat before use with P2 SD driver
- Second SanDisk 128GB card in catalog with different product name (SN128 vs AGGCF)
- DATA_STAT_AFTER_ERASE=0 (erased data reads as 0s)

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
| 27 MHz | 4 clocks | 25.0 MHz | -7.4% | 0 OK, 1,000 timeout | — | — | **100%** |

**Summary:**
- **Maximum Reliable Speed: 25 MHz** (47,200 total sector reads at 0% failure rate)
- CMD6 High Speed mode: Supported (detected) but **switch failed**
- Failure mode at 27 MHz: After failed CMD6 switch attempt, card became completely unresponsive (all 1,000 reads timed out)
- At SYSCLK=200 MHz with minimum half_period=4 clocks, 25 MHz is the hardware maximum achievable SPI clock

**Recommendation:** Use 25 MHz SPI clock for this card (matches TRAN_SPEED register value $32 = 25 MHz).

### Internal Throughput (measured at 25 MHz SPI)

| Metric | Value |
|--------|-------|
| Phase 2 Duration (10,000 reads) | 6.56 seconds |
| Throughput | **780 KB/s** |
| Sectors/second | 1,524 |
| Latency per sector | 0.66 ms |

**Performance Class:** HIGH - Fast internal controller, suitable for demanding applications.

---

## Card: "Chinese Made" #2 8GB SDHC (Samsung inside)

**Label:** Unlabeled 8GB microSD (Chinese text/no brand) - Card #2
**Unique ID:** `Samsung_00000_1.0_D9FB539C_201408`
**Test Date:** 2026-02-02 (V3 characterization)

### Raw Registers

```
CID: $1B $53 $4D $30 $30 $30 $30 $30 $10 $D9 $FB $53 $9C $00 $E8 $B1
CSD: $40 $0E $00 $32 $5B $59 $00 $00 $3A $CD $7F $80 $0A $40 $00 $97
OCR: $C0FF_8000
SCR: $02 $35 $80 $03 $00 $00 $00 $00
```

### CID Register (Card Identification) - All Fields

| Field | Bits | Raw | Value | Usage |
|-------|------|-----|-------|-------|
| MID | [127:120] | $1B | Samsung | **[USED]** |
| OID | [119:104] | $53 $4D | "SM" (Samsung) | [INFO] |
| PNM | [103:64] | $30 $30 $30 $30 $30 | "00000" | [INFO] |
| PRV | [63:56] | $10 | 1.0 | [INFO] |
| PSN | [55:24] | $D9FB_539C | 3,657,818,012 | [INFO] |
| MDT | [19:8] | $0E8 | 2014-08 (August 2014) | [INFO] |
| CRC7 | [7:1] | $58 | $58 | [INFO] |

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
| C_SIZE | [69:48] | $3ACD | 15,053 (7,527 MB) | **[USED]** |
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
| CRC7 | [7:1] | $4B | $4B | [INFO] |

**Derived Values:**
- Read Timeout: 100 ms (calculated)
- Write Timeout: 250 ms (calculated)
- Total Sectors: 15,415,296
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
| CMD_SUPPORT | [33:32] | $00 | — | [INFO] |

**SD Version:** 3.0x (SD_SPEC=2, SD_SPEC3=1, SD_SPEC4=0)

### Filesystem (Current State)

| Field | Value |
|-------|-------|
| MBR Partition Type | $0E (Unknown/non-standard) |
| Format Status | Unknown (not FAT32) |

**Note:** Partition type $0E is unusual. Card may need reformatting for P2 SD driver compatibility.

### Test Results

| Test | Status | Notes |
|------|--------|-------|
| Card Init | PASS | CMD0/CMD8/ACMD41 sequence successful |
| CID Read | PASS | V3 worker cog routing |
| CSD Read | PASS | V3 worker cog routing |
| SCR Read | PASS | V3 worker cog routing |
| OCR Read | PASS | Cached during init |
| MBR Read | PASS | Non-standard partition type detected |
| Mount | N/A | May require reformatting |

### Notes

- **Genuine Samsung inside** - MID $1B + OID "SM" confirms authentic Samsung flash
- **Second "Chinese" card** - See also "Chinese Made" 8GB (MID $03, claims SanDisk)
- **OEM/Generic card** - Product name "00000" suggests internal/OEM variant without retail naming
- Unlabeled physical card - Chinese text, no brand markings visible
- **SDHC** - High Capacity (8GB class, ~7GB usable)
- **SD 3.0x spec** compliant (SD_SPEC3=1, but SD_SPEC4=0)
- **COPY=0** - Original card (not a copy)
- Manufactured August 2014 (older card)
- Partition type $0E is unusual - may be FAT16 LBA or custom partition scheme
- Samsung recommended for embedded SPI use (major manufacturer)
- DATA_STAT_AFTER_ERASE=0 (erased data reads as 0s)

---

## Card: SanDisk 8GB SDHC (Taiwan)

**Label:** SanDisk 8GB (4) microSD HC, Made in Taiwan
**Unique ID:** `SanDisk_SS08G_3.0_DAAEE8AD_201509`
**Test Date:** 2026-02-02 (V3 characterization)

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
| R2W_FACTOR | [28:26] | 2 | Write = Read × 4 | **[USED]** |
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
| CID Read | PASS | V3 worker cog routing |
| CSD Read | PASS | V3 worker cog routing |
| SCR Read | PASS | V3 worker cog routing |
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

---

## Template for New Cards

Copy this template when adding a new card:

```markdown
## Card: [Brand] [Model] [Capacity]

**Unique ID:** `[Manufacturer]_[ProductName]_[Rev]_[Serial]_[YYYYMM]`
**Test Date:** YYYY-MM-DD

### Hardware Identification

| Field | Value |
|-------|-------|
| Manufacturer ID | $XX ([Name]) |
| OEM/Application | $XX $XX |
| Product Name | [name] |
| Product Revision | X.X |
| Serial Number | $XXXX_XXXX |
| Manufacturing Date | [Month] [Year] |

### Card Capabilities

| Field | Value |
|-------|-------|
| CSD Version | X.0 |
| Card Type | [SDSC/SDHC/SDXC] |
| Capacity | ~XX GB |
| Total Sectors | XXX,XXX,XXX |
| SD Spec Version | X.Xx |
| Bus Width Support | 1-bit, 4-bit |

### Raw Registers

```
CID: [16 bytes hex]
CSD: [16 bytes hex]
OCR: $XXXX_XXXX
SCR: [8 bytes hex]
```

### Test Results

| Test | Status | Notes |
|------|--------|-------|
| Card Init | ? | |
| Mount | ? | |
| Read | ? | |
| Write | ? | |
| Seek | ? | |
| Format | ? | |

### Notes

- [Any observations about this card]
```

---

## Known Manufacturer IDs (Heuristic)

**Note:** This table is heuristic - mappings are inferred from observed CIDs and may change as OEM relationships shift. Source: Matt Cole's survey + CID/OEM compilation.

| MID | OID | Manufacturer/Controller | Example Brands |
|-----|-----|------------------------|----------------|
| $00 | "42" | Unknown OEM, many rebrands | Gigastone, Patriot, fake cards |
| $02 | "TM" | Kioxia (Toshiba) | Toshiba, Kioxia |
| $03 | "SD" | SanDisk / Western Digital | SanDisk, WD |
| $09 | "AP" | ATP Electronics | ATP industrial |
| $12 | "4V" | Patriot-related OEM | Patriot, Gigastone |
| $1B | "SM" | Samsung | Samsung EVO/PRO |
| $1D | "AD" | ADATA | ADATA retail |
| $27 | "PH" | Phison controller family | Delkin, HP, Kingston, Lexar (older), PNY |
| $28 | "BE" | Lexar (pre-Longsys) | Older Lexar cards |
| $31 | — | Silicon Power | Silicon Power |
| $41 | — | Kingston | Kingston |
| $45 | "-B" | TEAMGROUP | TEAMGROUP |
| $6F | $0303 | Hiksemi / Longsys-related | Hiksemi, HP, Kodak, Lenovo, Netac |
| $74 | $4A60 | Gigastone / Transcend OEM | Gigastone, Transcend |
| $76 | — | Patriot Memory | Patriot |
| $82 | — | Sony | Sony |
| $89 | $0303 | Netac | Netac |
| $9C | — | Angelbird / Hoodman | Angelbird, Hoodman |
| $9F | "TI" | Shared OEM (many brands) | Amazon, Kingston, Kodak, Silicon Power |
| $AD | "LS" | Longsys | Amazon Basics, newer Lexar, RPi |
| $FE | "42" | Unknown OEM, budget brands | HP, ORICO, TEAMGROUP, fake SanDisk |

---

*Catalog created: 2026-01-20*
*Last updated: 2026-02-02*
*Cards cataloged: 14 (all with V3 deep characterization)*
