# SD Card Quick Reference — Canonical 2-Line Designators

Every SD card in the project has a canonical 2-line designator derived from register data and the physical label. This file is the single source of truth — the designator block for each card must be character-for-character identical everywhere it appears (card files, CARD-CATALOG.md, BENCHMARK-RESULTS.md).

**Line 1** — Identity & Filesystem (from CID, CSD, OCR, SCR, MBR registers):
```
<Manufacturer> <ProductName> <CardType> <CapacityGB>GB [<Filesystem>] SD <SpecVersion> rev<Major>.<Minor> SN:<Serial> <Year>/<Month>
```

**Line 2** — Speed & Capabilities (from ACMD13 SD Status + label + SPI):
```
Class <N>, U<N>, V<N>, SPI <Freq> MHz  [<optional notes>]
```

**Note:** Speed class fields on Line 2 currently come from physical label markings. As cards are re-characterized with the updated `SD_card_characterize.spin2` (which now reads ACMD13), Line 2 values will be verified against register data.

---

## Rating A — Video-optimized

### sandisk-sn64g-64gb

**Label:** SanDisk Extreme 64GB U3 A2 microSD XC I V30

```
SanDisk SN64G SDXC 59GB [FAT32] SD 6.x rev8.6 SN:7E650771 2022/11
Class 10, U3, V30, SPI 25 MHz  [formatted by P2FMTER]
```

### sandisk-sn128-128gb

**Label:** SanDisk 128GB Nintendo Switch microSD XC I

```
SanDisk SN128 SDXC 119GB [exFAT] SD 6.x rev8.0 SN:F79E34F6 2019/12
SPI 25 MHz
```

### lexar-mssd0-64gb

**Label:** Lexar V30 U3 64GB microSD XC (Red card)

```
Lexar MSSD0 SDXC 58GB [exFAT] SD 6.x rev6.1 SN:31899471 2024/11
Class 10, U3, V30, SPI 25 MHz
```

### lexar-mssd0-128gb

**Label:** Lexar PLAY 128GB microSD XC (Blue card)

```
Lexar MSSD0 SDXC 117GB [FAT32] SD 6.x rev6.1 SN:34490F1E 2025/04
SPI 25 MHz  [formatted by P2FMTER]
```

---

## Rating B — Fast

### samsung-gd4qt-128gb

**Label:** Samsung EVO Select microSD XC I U3

```
Samsung GD4QT SDXC 119GB [FAT32] SD 3.x rev3.0 SN:C0305565 2018/05
U3, SPI 25 MHz  [formatted by P2FMTER]
```

### sandisk-aggcf-128gb

**Label:** SanDisk Extreme PRO 128GB microSD XC I V30 U3 A1

```
SanDisk AGGCF SDXC 119GB [exFAT] SD 5.x rev8.0 SN:E05C352B 2017/07
U3, V30, SPI 25 MHz
```

### sandisk-aggce-64gb

**Label:** SanDisk Extreme PRO 64GB microSD XC I V30 U3

```
SanDisk AGGCE SDXC 59GB [FAT32] SD 5.x rev8.0 SN:DD1C1144 2017/03
Class 10, U3, V30, SPI 25 MHz  [formatted by P2FMTER]
```

### gigastone-astc-64gb

**Label:** Gigastone "Camera Plus" microSD XC I, A1 V30 U3 64GB

```
Gigastone ASTC SDXC 58GB [FAT32] SD 6.x rev2.0 SN:00000F14 2023/06
U3, V30, SPI 25 MHz  [formatted by P2FMTER]
```

### sandisk-sa16g-16gb

**Label:** SanDisk Industrial microSD HC I, U1 C10, 16GB

```
SanDisk SA16G SDHC 14GB [FAT32] SD 5.x rev8.0 SN:93E9C0A1 2025/11
Class 10, U1, SPI 25 MHz
```

---

## Rating C — Standard

### gigastone-00000-32gb

**Label:** Gigastone 32GB microSD HC I A1 U1 (10)

```
Gigastone 00000 SDHC 29GB [FAT32] SD 3.x rev0.0 SN:000001C9 2023/07
Class 10, U1, SPI 25 MHz  [formatted by P2FMTER]
```

### gigastone-00000-8gb

**Label:** Gigastone 10x High Endurance 8GB MLC microSD HC I U1

```
Gigastone 00000 SDHC 7GB [FAT32] SD 3.x rev0.0 SN:0001B9D5 2021/09
U1, SPI 25 MHz  [formatted by P2FMTER]
```

### gigastone-sd16g-16gb

**Label:** Gigastone 10x High Endurance 16GB MLC microSD HC I U3 V30 4K

```
Gigastone SD16G SDHC 14GB [FAT32] SD 3.x rev2.0 SN:000003FB 2025/02
U3, V30, SPI 25 MHz
```

### kingston-sd8gb-8gb

**Label:** Kingston 8GB microSD HC I ui (10) "Taiwan"

```
Kingston SD8GB SDHC 7GB [FAT32] SD 3.x rev3.0 SN:43F65DC9 2015/04
Class 10, SPI 25 MHz
```

### sandisk-su08g-8gb

**Label:** microSD HC 8GB (4) - Chinese text, no brand - Card #1

```
SanDisk SU08G SDHC 7GB [FAT32] SD 3.x rev8.0 SN:0AA81F11 2010/10
Class 4, SPI 25 MHz
```

### samsung-00000-8gb

**Label:** Unlabeled 8GB microSD (Chinese text/no brand) - Card #2

```
Samsung 00000 SDHC 7GB [FAT16] SD 3.x rev1.0 SN:D9FB539C 2014/08
SPI 25 MHz
```

### sandisk-ss08g-8gb

**Label:** SanDisk 8GB (4) microSD HC, Made in Taiwan

```
SanDisk SS08G SDHC 7GB [FAT32] SD 3.x rev3.0 SN:DAAEE8AD 2015/09
Class 4, SPI 25 MHz
```

---

## Rating D — Limited

### pny-sd16g-16gb

**Label:** PNY 16GB microSD HC I

```
PNY SD16G SDHC 14GB [FAT32] SD 3.x rev3.0 SN:01CD5CF5 2018/08
SPI 25 MHz  [formatted by P2FMTER]
```

---

## Re-characterization Status

Cards that need to be re-run through the updated `SD_card_characterize.spin2` to get verified ACMD13 data for Line 2:

| Card | ACMD13 Status | Line 2 Source |
|------|:---:|---|
| sandisk-sn64g-64gb | pending | label |
| sandisk-sn128-128gb | pending | label |
| lexar-mssd0-64gb | pending | label |
| lexar-mssd0-128gb | pending | label |
| samsung-gd4qt-128gb | pending | label |
| sandisk-aggcf-128gb | pending | label |
| sandisk-aggce-64gb | **verified** | ACMD13 (Class 10, U3, V30) |
| gigastone-astc-64gb | pending | label |
| sandisk-sa16g-16gb | pending | label |
| gigastone-00000-32gb | pending | label |
| gigastone-00000-8gb | pending | label |
| gigastone-sd16g-16gb | pending | label |
| kingston-sd8gb-8gb | pending | label |
| sandisk-su08g-8gb | pending | label |
| samsung-00000-8gb | pending | label |
| sandisk-ss08g-8gb | pending | label |
| pny-sd16g-16gb | pending | label |

---

*Created: 2026-02-15*
*Cards: 17*
