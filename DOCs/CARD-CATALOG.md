# SD Card Catalog

This catalog documents SD cards tested with the P2 SD card driver. Cards are characterized using the `SD_card_characterize.spin2` diagnostic tool. Each card has a dedicated page in [DOCs/cards/](cards/) with full register dumps, field decodes, and test results.

---

## Register Field Reference

This section documents ALL fields available from SD card registers and indicates which are actively used by the driver.

- **[USED]** = Field is actively used by the driver for operation
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

**Driver Usage:** MID is checked to identify PNY/Phison cards ($27) which require reduced SPI clock (20MHz vs 25MHz).

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

**Driver Usage:**
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

**Driver Usage:** SD_SPEC determines High Speed (CMD6) support availability.

### OCR Register (Operating Conditions) - 4 bytes

| Field | Bits | Size | Usage | Description |
|-------|------|------|-------|-------------|
| Power Up Status | [31] | 1 bit | [INFO] | Card power up status (1=ready) |
| CCS | [30] | 1 bit | **[USED]** | **CRITICAL** - Card Capacity Status |
| UHS-II Status | [29] | 1 bit | [INFO] | UHS-II card status |
| S18A | [24] | 1 bit | [INFO] | Switching to 1.8V accepted |
| Voltage Window | [23:15] | 9 bits | [INFO] | Supported voltage range (2.7V-3.6V) |

**Driver Usage:** CCS bit is **CRITICAL** - determines addressing mode:
- CCS=0: SDSC byte addressing (sector << 9)
- CCS=1: SDHC/SDXC block addressing (sector number directly)

### Summary: Fields Used by Driver

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
| SanDisk_SA16G_8.0_93E9C0A1_202511 | SanDisk ($03) | SA16G | ~14 GB | **B** | PASS |
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

Cards tested with `SD_speed_characterize.spin2` have measured internal throughput data. This reflects the card's internal flash/controller performance, NOT the SPI bus speed.

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
| SanDisk_SA16G | SanDisk ($03) | ~14 GB | **824 KB/s** | 0.62 ms | HIGH | 25 MHz |
| Transcend_00000 | Gigastone ($74) | ~29 GB | — | — | — | not yet tested |
| Unknown_00000 | Gigastone ($9F) | ~7 GB | — | — | — | not yet tested |
| BudgetOEM_SD16G | Gigastone ($00) | ~14 GB | **368 KB/s** | 1.39 ms | MEDIUM | 25 MHz |
| Kingston_SD8GB | Kingston ($41) | ~7 GB | — | — | — | not yet tested |
| SanDisk_SU08G | Chinese #1 ($03) | ~7 GB | — | — | — | not yet tested |
| Samsung_00000 | Chinese #2 ($1B) | ~7 GB | — | — | — | not yet tested |
| SanDisk_SS08G | SanDisk Taiwan ($03) | ~7 GB | — | — | — | not yet tested |
| Phison_SD16G | PNY ($27) | ~14 GB | **31.3 KB/s** | 16.0 ms | LOW | 25 MHz |

**Tested: 8 of 16 cards**

**Key Observations:**
1. **Lexar V30 U3 64GB** - **Fastest card tested** (1,059 KB/s), 12% faster than Gigastone
2. **Gigastone Camera Plus 64GB** - Very fast (944 KB/s), excellent value
3. **SanDisk Extreme PRO 64GB** - High performance (866 KB/s), professional line
4. **SanDisk Industrial 16GB** - High performance (824 KB/s), industrial/embedded grade
5. **Samsung EVO Select 128GB** - High performance (783 KB/s), nearly identical to SanDisk
6. **SanDisk Nintendo Switch 128GB** - High performance (780 KB/s), designed for gaming
7. **Gigastone High Endurance 16GB** - Medium performance (368 KB/s), MLC flash endurance focus
8. **PNY 16GB** - Slow internal controller (31 KB/s); runs reliably at 25 MHz SPI but throughput limited by internal latency
9. All tested cards max out at 25 MHz SPI (CMD6 High Speed switch fails on all)

---

## Card Details

Each card has a dedicated page with full register dumps, field-by-field decode, filesystem info, test results, notes, and (where tested) SPI speed characterization and internal throughput data.

**Rating A** - Video-optimized:
| Card | File |
|------|------|
| SanDisk Extreme 64GB SDXC | [sandisk-sn64g-64gb.md](cards/sandisk-sn64g-64gb.md) |
| SanDisk Nintendo Switch 128GB SDXC | [sandisk-sn128-128gb.md](cards/sandisk-sn128-128gb.md) |
| Lexar MicroSD XC V30 U3 64GB | [lexar-mssd0-64gb.md](cards/lexar-mssd0-64gb.md) |

**Rating B** - Fast:
| Card | File |
|------|------|
| Samsung EVO Select 128GB SDXC | [samsung-gd4qt-128gb.md](cards/samsung-gd4qt-128gb.md) |
| SanDisk Extreme PRO 128GB SDXC | [sandisk-aggcf-128gb.md](cards/sandisk-aggcf-128gb.md) |
| SanDisk Extreme PRO 64GB SDXC | [sandisk-aggce-64gb.md](cards/sandisk-aggce-64gb.md) |
| Gigastone "Camera Plus" 64GB SDXC | [gigastone-astc-64gb.md](cards/gigastone-astc-64gb.md) |
| SanDisk Industrial 16GB SDHC | [sandisk-sa16g-16gb.md](cards/sandisk-sa16g-16gb.md) |

**Rating C** - Standard:
| Card | File |
|------|------|
| Gigastone 32GB SDHC | [gigastone-00000-32gb.md](cards/gigastone-00000-32gb.md) |
| Gigastone "High Endurance" 8GB SDHC MLC | [gigastone-00000-8gb.md](cards/gigastone-00000-8gb.md) |
| Gigastone "High Endurance" 16GB SDHC MLC | [gigastone-sd16g-16gb.md](cards/gigastone-sd16g-16gb.md) |
| Kingston 8GB SDHC | [kingston-sd8gb-8gb.md](cards/kingston-sd8gb-8gb.md) |
| "Chinese Made" #1 8GB SDHC (claims SanDisk) | [sandisk-su08g-8gb.md](cards/sandisk-su08g-8gb.md) |
| "Chinese Made" #2 8GB SDHC (Samsung inside) | [samsung-00000-8gb.md](cards/samsung-00000-8gb.md) |
| SanDisk 8GB SDHC (Taiwan) | [sandisk-ss08g-8gb.md](cards/sandisk-ss08g-8gb.md) |

**Rating D** - Limited:
| Card | File |
|------|------|
| PNY 16GB SDHC | [pny-sd16g-16gb.md](cards/pny-sd16g-16gb.md) |

---

## Template for New Cards

Create a new file in `DOCs/cards/` named `brand-product-capacity.md` (e.g., `sandisk-sn64g-64gb.md`).
Use the product name from the CID register (PNM field) and lowercase the brand.

```markdown
# Card: [Brand] [Model] [Capacity]

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
*Last updated: 2026-02-13*
*Cards cataloged: 16 (individual card pages in [DOCs/cards/](cards/))*
