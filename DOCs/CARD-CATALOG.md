# SD Card Catalog

This catalog documents SD cards tested with the P2 SD card driver. Cards are characterized using the `SD_card_characterize.spin2` diagnostic tool.

---

## Summary Table

| Card ID | Manufacturer | Product | Capacity | Test Status |
|---------|-------------|---------|----------|-------------|
| Gigastone_ASTC_2.0_00000F14_202306 | Gigastone (Patriot OEM $12) | ASTC | ~58 GB | PASS |
| Hynix_MSSD0_6.1_31899471_202411 | Lexar (Longsys $AD) | MSSD0 | ~58 GB | PASS |
| Transcend_00000_0.0_000001C9_202307 | Gigastone (Transcend $74) | 00000 | ~29 GB | PASS |
| Unknown_00000_0.0_0001B9D5_202109 | Gigastone (Shared OEM $9F) | 00000 | ~7 GB | PASS |
| Unknown_SD16G_2.0_000003FB_202502 | Gigastone (Budget OEM $00) | SD16G | ~14 GB | PASS |
| SanDisk_SN64G_8.6_7E650771_202211 | SanDisk ($03) | SN64G | ~59 GB | PASS |
| Phison_SD16G_3.0_01CD5CF5_201808 | PNY (Phison $27) | SD16G | ~14 GB | PARTIAL |

---

## Card: Gigastone "Camera Plus" 64GB SDXC

**Label:** Gigastone "Camera Plus" microSD XC I, A1 V30 U3 64GB
**Unique ID:** `Gigastone_ASTC_2.0_00000F14_202306`
**Test Date:** 2026-01-20

### Hardware Identification

| Field | Value |
|-------|-------|
| Brand (Label) | Gigastone "Camera Plus" |
| Manufacturer ID | $12 (Patriot-related OEM) |
| OEM/Application | $34 $56 ("4V") |
| Product Name | ASTC |
| Product Revision | 2.0 |
| Serial Number | $0000_0F14 |
| Manufacturing Date | June 2023 |

### Card Capabilities

| Field | Value |
|-------|-------|
| CSD Version | 2.0 |
| Card Type | SDHC/SDXC (High/Extended Capacity) |
| Capacity | ~58 GB (59,638 MB) |
| Total Sectors | 122,138,624 |
| SD Spec Version | 3.0x |
| Bus Width Support | 1-bit, 4-bit |

### Operating Conditions

| Field | Value |
|-------|-------|
| OCR Register | $C0FF_8000 |
| Power Status | Ready |
| Capacity Status | High Capacity |
| Voltage Support | 2.7V - 3.6V |

### Raw Registers

```
CID: $12 $34 $56 $41 $53 $54 $43 $00 $20 $00 $00 $0F $14 $01 $76 $4D
CSD: $40 $0E $00 $32 $5B $59 $00 $01 $D1 $EB $7F $80 $0A $40 $00 $A1
OCR: $C0FF_8000
SCR: $02 $C5 $84 $83 $00 $00 $00 $00
```

### Filesystem Formatting

| Field | Value |
|-------|-------|
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
| CID Read | PASS | |
| CSD Read | PASS | |
| SCR Read | PASS | |
| MBR Read | PASS | FAT32 LBA ($0C) partition |
| VBR Read | PASS | |
| Mount | PASS | |
| Regression Tests | PASS | All tests pass |

### Notes

- Gigastone (gigastone.com) - Taiwanese flash memory manufacturer
- **"Camera Plus"** product line marketed for action cameras and drones
- **XC I** = SDXC UHS-I interface (up to 104 MB/s bus speed)
- **A1** = Application Performance Class 1 (1500 read IOPS, 500 write IOPS)
- **V30** = Video Speed Class 30 (30 MB/s sustained sequential write)
- **U3** = UHS Speed Class 3 (30 MB/s minimum write speed)
- Formatted with P2FMTER (P2 Flash Filesystem Formatter)
- Currently used as scratch/test card for development

---

## Card: Lexar MicroSD XC V30 U3 64GB

**Label:** Lexar MicroSD XC V30 U3 64GB (10) 100 MB/s*
**Unique ID:** `Hynix_MSSD0_6.1_31899471_202411`
**Test Date:** 2026-01-20

### Hardware Identification

| Field | Value |
|-------|-------|
| Brand (Label) | Lexar |
| Manufacturer ID | $AD (Longsys - acquired Lexar brand) |
| OEM/Application | $4C $53 ("LS" = Longsys/Lexar) |
| Product Name | MSSD0 |
| Product Revision | 6.1 |
| Serial Number | $3189_9471 |
| Manufacturing Date | November 2024 |

### Card Capabilities

| Field | Value |
|-------|-------|
| CSD Version | 2.0 |
| Card Type | SDHC/SDXC (High/Extended Capacity) |
| Capacity | ~58 GB (59,700 MB) |
| Total Sectors | 122,265,600 |
| SD Spec Version | 3.0x |
| Bus Width Support | 1-bit, 4-bit |

### Operating Conditions

| Field | Value |
|-------|-------|
| OCR Register | $C0FF_8000 |
| Power Status | Ready |
| Capacity Status | High Capacity (SDHC/SDXC) |
| Voltage Support | 2.7V - 3.6V |

### Raw Registers

```
CID: $AD $4C $53 $4D $53 $53 $44 $30 $61 $31 $89 $94 $71 $71 $8B $7D
CSD: $40 $0E $00 $32 $DB $79 $00 $01 $D2 $67 $7F $80 $0A $40 $00 $59
OCR: $C0FF_8000
SCR: $02 $45 $84 $87 $33 $33 $30 $39
```

### CSD Register Details

| Field | Bits | Value | Meaning |
|-------|------|-------|---------|
| CSD_STRUCTURE | [127:126] | 1 | CSD Version 2.0 |
| TAAC | [119:112] | $0E | Data read access time |
| NSAC | [111:104] | $00 | Data read access time (CLK cycles) |
| TRAN_SPEED | [103:96] | $32 | Max transfer rate = 25 MHz |
| CCC | [95:84] | $5B7 | Card command classes |
| READ_BL_LEN | [83:80] | 9 | Max read block = 512 bytes |
| C_SIZE | [69:48] | $01D267 | Capacity = 59,700 MB |
| ERASE_BLK_EN | [46] | 1 | 512-byte erase supported |
| SECTOR_SIZE | [45:39] | 127 | Erase sector = 64 KB |
| R2W_FACTOR | [28:26] | 2 | Write 4x slower than read |
| WRITE_BL_LEN | [25:22] | 9 | Max write block = 512 bytes |

### SCR Register Details

| Field | Bits | Value | Meaning |
|-------|------|-------|---------|
| SCR_STRUCTURE | [63:60] | 0 | SCR Version 1.0 |
| SD_SPEC | [59:56] | 2 | SD Physical Layer 2.00+ |
| SD_SPEC3 | [47] | 1 | SD 3.0x supported |
| SD_BUS_WIDTHS | [51:48] | 5 | 1-bit and 4-bit supported |
| SD_SECURITY | [54:52] | 0 | No security |
| CMD_SUPPORT | [33:32] | $87 | CMD23, CMD20 supported |

### Filesystem Formatting (Factory)

| Field | Value |
|-------|-------|
| Partition Type | $07 (exFAT/NTFS) |
| Factory Format | exFAT (default for 64GB+) |

**Note:** Card shipped with exFAT filesystem. Needs FAT32 format for P2 SD driver compatibility.

### Test Results

| Test | Status | Notes |
|------|--------|-------|
| Card Init | PASS | CMD0/CMD8/ACMD41 sequence successful |
| CID Read | PASS | |
| CSD Read | PASS | |
| SCR Read | PASS | |
| MBR Read | PASS | exFAT partition detected |
| Mount | N/A | Requires FAT32 format first |

### Notes

- **Lexar-branded** card using Longsys silicon (Longsys acquired Lexar brand in 2017)
- MID $AD = Longsys (often incorrectly listed as Hynix in older databases)
- OEM code "LS" = Longsys/Lexar
- **V30** = Video Speed Class 30 (30 MB/s sustained write)
- **U3** = UHS Speed Class 3 (30 MB/s minimum write)
- **(10)** = Speed Class 10 (10 MB/s minimum write speed)
- **100 MB/s*** = Maximum read speed (asterisk indicates "up to")
- Factory formatted with exFAT (not FAT32)
- Very recent manufacture (November 2024)
- Needs FAT32 reformat before use with P2 SD driver

---

## Card: Gigastone 32GB SDHC

**Label:** Gigastone 32GB microSD HC I A1 U1 (10)
**Unique ID:** `Transcend_00000_0.0_000001C9_202307`
**Test Date:** 2026-01-20

### Hardware Identification

| Field | Value |
|-------|-------|
| Brand (Label) | Gigastone |
| Manufacturer ID | $74 (Transcend - silicon manufacturer) |
| OEM/Application | $4A $60 |
| Product Name | 00000 |
| Product Revision | 0.0 |
| Serial Number | $0000_01C9 |
| Manufacturing Date | July 2023 |

### Card Capabilities

| Field | Value |
|-------|-------|
| CSD Version | 2.0 |
| Card Type | SDHC/SDXC (High/Extended Capacity) |
| Capacity | ~29 GB (29,819 MB) |
| Total Sectors | 61,069,312 |
| SD Spec Version | 3.0x |
| Bus Width Support | 1-bit, 4-bit |

### Operating Conditions

| Field | Value |
|-------|-------|
| OCR Register | $C0FF_8000 |
| Power Status | Ready |
| Capacity Status | High Capacity (SDHC/SDXC) |
| Voltage Support | 2.7V - 3.6V |

### Raw Registers

```
CID: $74 $4A $60 $30 $30 $30 $30 $30 $00 $00 $00 $01 $C9 $01 $77 $09
CSD: $40 $0E $00 $32 $5B $59 $00 $00 $E8 $F5 $7F $80 $0A $40 $00 $D1
OCR: $C0FF_8000
SCR: $02 $B5 $80 $83 $00 $00 $00 $00
```

### Filesystem Formatting

| Field | Value |
|-------|-------|
| Formatter | SD Card Formatter for macOS v5.0.3 |
| OEM Name | MSWIN4.1 |
| Volume Label | P2-TEST |
| Volume Serial | $6A62_BDFC |
| FS Type | FAT32 |
| Bytes/Sector | 512 |
| Sectors/Cluster | 64 (32 KB clusters) |
| Reserved Sectors | 1,480 |
| Number of FATs | 2 |
| Sectors per FAT | 7,452 |
| Root Cluster | 2 |
| Total Sectors | 61,061,120 |
| Data Region Start | Sector 16,384 |
| Total Clusters | 953,824 |

### Test Results

| Test | Status | Notes |
|------|--------|-------|
| Card Init | PASS | CMD0/CMD8/ACMD41 sequence successful |
| CID Read | PASS | |
| CSD Read | PASS | |
| SCR Read | PASS | |
| MBR Read | PASS | FAT32 LBA ($0C) partition |
| VBR Read | PASS | |
| Mount | PASS | |

### Notes

- **Gigastone-branded** card using **Transcend** flash/controller silicon (MID $74)
- **HC I** = SDHC UHS-I interface (up to 104 MB/s bus speed)
- **A1** = Application Performance Class 1 (1500 read IOPS, 500 write IOPS)
- **U1** = UHS Speed Class 1 (10 MB/s minimum write speed)
- **(10)** = Speed Class 10 (10 MB/s minimum write speed)
- Product name "00000" and revision "0.0" suggest OEM/white-label product
- Formatted with official **SD Card Formatter** app for macOS v5.0.3
- OEM Name "MSWIN4.1" is standard Windows FAT32 signature
- 32 KB cluster size is optimal for this capacity
- VBR starts at sector 8,192 (standard for SD Card Formatter)

---

## Card: Gigastone "High Endurance" 8GB SDHC MLC

**Label:** Gigastone 10x High Endurance 8GB MLC microSD HC I U1
**Unique ID:** `Unknown_00000_0.0_0001B9D5_202109`
**Test Date:** 2026-01-20

### Hardware Identification

| Field | Value |
|-------|-------|
| Brand (Label) | Gigastone "High Endurance" |
| Manufacturer ID | $9F (Shared OEM - Amazon, Kingston, Kodak, Silicon Power) |
| OEM/Application | $54 $49 ("TI") |
| Product Name | 00000 |
| Product Revision | 0.0 |
| Serial Number | $0001_B9D5 |
| Manufacturing Date | September 2021 |

### Card Capabilities

| Field | Value |
|-------|-------|
| CSD Version | 2.0 |
| Card Type | SDHC/SDXC (High/Extended Capacity) |
| Capacity | ~7 GB (7,431 MB) |
| Total Sectors | 15,218,688 |
| SD Spec Version | 3.0x |
| Bus Width Support | 1-bit, 4-bit |

### Operating Conditions

| Field | Value |
|-------|-------|
| OCR Register | $C0FF_8000 |
| Power Status | Ready |
| Capacity Status | High Capacity (SDHC/SDXC) |
| Voltage Support | 2.7V - 3.6V |

### Raw Registers

```
CID: $9F $54 $49 $30 $30 $30 $30 $30 $00 $00 $01 $B9 $D5 $01 $59 $93
CSD: $40 $0E $00 $32 $5B $59 $00 $00 $3A $0D $7F $80 $0A $40 $00 $8D
OCR: $C0FF_8000
SCR: $02 $B5 $80 $43 $00 $00 $00 $00
```

### Filesystem Formatting

| Field | Value |
|-------|-------|
| Partition Type | $0B (FAT32 CHS) |
| OEM Name | MSDOS5.0 |
| Volume Label | NO NAME |
| Volume Serial | $747A_99D9 |
| FS Type | FAT32 |
| Bytes/Sector | 512 |
| Sectors/Cluster | 64 (32 KB clusters) |
| Reserved Sectors | 4,480 |
| Number of FATs | 2 |
| Sectors per FAT | 1,856 |
| Root Cluster | 2 |
| Total Sectors | 15,210,496 |
| Data Region Start | Sector 8,192 |
| Total Clusters | 237,536 |

### Test Results

| Test | Status | Notes |
|------|--------|-------|
| Card Init | PASS | CMD0/CMD8/ACMD41 sequence successful |
| CID Read | PASS | |
| CSD Read | PASS | |
| SCR Read | PASS | |
| MBR Read | PASS | FAT32 CHS ($0B) partition |
| VBR Read | PASS | |
| Mount | PASS | |

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
- Partition type $0B (FAT32 CHS) vs $0C (FAT32 LBA) - older formatting style
- Third different silicon source found in Gigastone cards

---

## Card: Gigastone "High Endurance" 16GB SDHC MLC

**Label:** Gigastone 10x High Endurance 16GB MLC microSD HC I U3 V30 4K
**Unique ID:** `Unknown_SD16G_2.0_000003FB_202502`
**Test Date:** 2026-01-20

### Hardware Identification

| Field | Value |
|-------|-------|
| Brand (Label) | Gigastone "High Endurance" |
| Manufacturer ID | $00 (Budget OEM - often used on white-label/rebrands) |
| OEM/Application | $34 $32 ("42") |
| Product Name | SD16G |
| Product Revision | 2.0 |
| Serial Number | $0000_03FB |
| Manufacturing Date | February 2025 |

### Card Capabilities

| Field | Value |
|-------|-------|
| CSD Version | 2.0 |
| Card Type | SDHC/SDXC (High/Extended Capacity) |
| Capacity | ~14 GB (15,238 MB) |
| Total Sectors | 31,207,424 |
| SD Spec Version | 3.0x |
| Bus Width Support | 1-bit, 4-bit |

### Operating Conditions

| Field | Value |
|-------|-------|
| OCR Register | $C0FF_8000 |
| Power Status | Ready |
| Capacity Status | High Capacity (SDHC/SDXC) |
| Voltage Support | 2.7V - 3.6V |

### Raw Registers

```
CID: $00 $34 $32 $53 $44 $31 $36 $47 $20 $00 $00 $03 $FB $01 $92 $D5
CSD: $40 $0E $00 $32 $5B $59 $00 $00 $77 $0B $7F $80 $0A $40 $00 $4F
OCR: $C0FF_8000
SCR: $02 $B5 $80 $43 $00 $00 $00 $00
```

### Filesystem Formatting (Factory)

| Field | Value |
|-------|-------|
| Partition Type | $0C (FAT32 LBA) |
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

### Test Results

| Test | Status | Notes |
|------|--------|-------|
| Card Init | PASS | CMD0/CMD8/ACMD41 sequence successful |
| CID Read | PASS | |
| CSD Read | PASS | |
| SCR Read | PASS | |
| MBR Read | PASS | FAT32 LBA ($0C) partition |
| VBR Read | PASS | |
| Mount | PASS | |

### Notes

- **Gigastone-branded** card using budget OEM silicon (MID $00)
- MID $00 + OID "42" = common on white-label/rebrand cards (CSV: Gigastone, Patriot, fake cards)
- **Different silicon than 8GB "High Endurance"** - even same product line uses multiple sources
- **"High Endurance"** product line for continuous recording (dashcams, security cameras)
- **MLC** (Multi-Level Cell) flash - more durable than TLC
- **10x** = marketing claim for endurance vs standard cards
- **HC I** = SDHC UHS-I interface
- **U3** = UHS Speed Class 3 (30 MB/s minimum write)
- **V30** = Video Speed Class 30 (30 MB/s sustained write)
- **4K** = Suitable for 4K video recording
- Factory formatted with blank OEM name and volume label
- Very recent manufacture (February 2025)
- Fourth different silicon source found in Gigastone cards

---

## Card: SanDisk Extreme 64GB SDXC

**Label:** SanDisk Extreme 64GB U3 A2 microSD XC I V30
**Unique ID:** `SanDisk_SN64G_8.6_7E650771_202211`
**Test Date:** 2026-01-20

### Hardware Identification

| Field | Value |
|-------|-------|
| Brand (Label) | SanDisk Extreme |
| Manufacturer ID | $03 (SanDisk / Western Digital) |
| OEM/Application | $53 $44 ("SD") |
| Product Name | SN64G |
| Product Revision | 8.6 |
| Serial Number | $7E65_0771 |
| Manufacturing Date | November 2022 |

### Card Capabilities

| Field | Value |
|-------|-------|
| CSD Version | 2.0 |
| Card Type | SDHC/SDXC (High/Extended Capacity) |
| Capacity | ~59 GB (60,906 MB) |
| Total Sectors | 124,735,488 |
| SD Spec Version | 3.0x |
| Bus Width Support | 1-bit, 4-bit |

### Operating Conditions

| Field | Value |
|-------|-------|
| OCR Register | $C0FF_8000 |
| Power Status | Ready |
| Capacity Status | High Capacity (SDHC/SDXC) |
| Voltage Support | 2.7V - 3.6V |

### Raw Registers

```
CID: $03 $53 $44 $53 $4E $36 $34 $47 $86 $7E $65 $07 $71 $01 $6B $8D
CSD: $40 $0E $00 $32 $DB $79 $00 $01 $DB $D3 $7F $80 $0A $40 $40 $F9
OCR: $C0FF_8000
SCR: $02 $45 $84 $87 $00 $00 $00 $00
```

### Filesystem Formatting (Factory)

| Field | Value |
|-------|-------|
| Partition Type | $07 (exFAT/NTFS) |
| Factory Format | exFAT (default for 64GB+) |

**Note:** Card shipped with exFAT filesystem. Needs FAT32 format for P2 SD driver compatibility.

### Test Results

| Test | Status | Notes |
|------|--------|-------|
| Card Init | PASS | CMD0/CMD8/ACMD41 sequence successful |
| CID Read | PASS | |
| CSD Read | PASS | |
| SCR Read | PASS | |
| MBR Read | PASS | exFAT partition detected |
| Mount | N/A | Requires FAT32 format first |

### Notes

- **Genuine SanDisk** - first card in catalog using manufacturer's own silicon
- MID $03 + OID "SD" = authentic SanDisk/Western Digital product
- **"Extreme"** product line - premium consumer tier
- **XC I** = SDXC UHS-I interface (up to 104 MB/s bus speed)
- **A2** = Application Performance Class 2 (4000 read IOPS, 2000 write IOPS) - higher than A1
- **V30** = Video Speed Class 30 (30 MB/s sustained sequential write)
- **U3** = UHS Speed Class 3 (30 MB/s minimum write speed)
- Product name "SN64G" = SanDisk Nomenclature 64GB
- Factory formatted with exFAT (not FAT32)
- Needs FAT32 reformat before use with P2 SD driver

---

## Card: PNY 16GB SDHC

**Label:** PNY 16GB microSD HC I
**Unique ID:** `Phison_SD16G_3.0_01CD5CF5_201808`
**Test Date:** 2026-01-20

### Hardware Identification

| Field | Value |
|-------|-------|
| Brand (Label) | PNY |
| Manufacturer ID | $27 (Phison) |
| OEM/Application | $50 $48 ("PH" = Phison) |
| Product Name | SD16G |
| Product Revision | 3.0 |
| Serial Number | $01CD_5CF5 |
| Manufacturing Date | August 2018 |

### Card Capabilities

| Field | Value |
|-------|-------|
| CSD Version | 2.0 |
| Card Type | SDHC (High Capacity) |
| Capacity | ~14 GB (14,868 MB) |
| Total Sectors | 30,449,664 |
| SD Spec Version | 3.0x |
| Bus Width Support | 1-bit, 4-bit |

### Operating Conditions

| Field | Value |
|-------|-------|
| OCR Register | $C0FF_8000 |
| Power Status | Ready |
| Capacity Status | High Capacity (SDHC) |
| Voltage Support | 2.7V - 3.6V |

### Raw Registers

```
CID: $27 $50 $48 $53 $44 $31 $36 $47 $30 $01 $CD $5C $F5 $01 $28 $1F
CSD: $40 $0E $00 $32 $5B $59 $00 $00 $74 $27 $7F $80 $0A $40 $00 $0F
OCR: $C0FF_8000
SCR: $02 $35 $80 $00 $01 $00 $00 $00
```

### Filesystem Formatting

| Field | Value |
|-------|-------|
| Formatter | P2FMTER (P2 Flash Filesystem Formatter) |
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
| Card Init | PASS | Required PNY fixes: 20MHz SPI + CS handling |
| CID Read | PASS | |
| CSD Read | PASS | |
| SCR Read | PASS | |
| MBR Read | PASS | FAT32 LBA ($0C) partition |
| VBR Read | PASS | |
| Mount | PASS | |
| Unmount | FAIL | Hangs during FSInfo write - under investigation |

### Notes

- **PNY-branded** card using **Phison** controller silicon (MID $27)
- MID $27 + OID "PH" = Phison controller family (also used by Delkin, HP, Kingston, Lexar older, PNY)
- **First PNY card to work** with P2 driver after implementing PNY compatibility fixes
- **Fixes required**: Lower SPI clock to ~20 MHz, proper CS HIGH/LOW handling during speed change
- **HC I** = SDHC UHS-I interface
- Older card (manufactured August 2018)
- Reads work perfectly; writes hang (FSInfo update during unmount)
- Write issue to be investigated on expendable card

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
*Last updated: 2026-01-20*
*Cards cataloged: 7*
