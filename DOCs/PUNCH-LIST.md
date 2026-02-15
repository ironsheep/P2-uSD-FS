# Punch List

Items to investigate when time permits.

---

### Formatter: Switch FAT initialization to multi-sector writes (CMD25)

**Priority:** ESSENTIAL — implement tomorrow

The format utility (`isp_format_utility.spin2`) writes each FAT sector individually with `writeSectorRaw()` in a loop (line 431-432). For a 128GB card with ~30,500 sectors per FAT, that's ~61,000 single-sector writes — the slowest path.

Benchmark data from the Samsung GD4QT shows:
- Single-sector write: **1,305 us/sector** (392 KB/s)
- 64-sector CMD25 write: **235 us/sector** (2,176 KB/s) — **5.5x faster**

The FAT clearing loop writes identical zero-filled buffers to consecutive sectors — the ideal case for `writeSectorsRaw()` with CMD25. The change is straightforward:
- After writing FAT sector 0 (special entries), batch the remaining `sectorsPerFat - 1` sectors into 64-sector CMD25 writes
- Handle the remainder (last batch < 64 sectors) with a smaller multi-sector write
- Same approach for `initRootDirectory()` which also writes consecutive zero sectors

**Expected improvement:** Format time for both FATs on a 128GB card drops from ~80 seconds to ~14 seconds.

**File:** `src/UTILS/isp_format_utility.spin2`, `initFAT()` method (line 409)

*Noted: 2026-02-15*

---

### BUG: openFileRead -40 errors on Samsung GD4QT benchmark

**Priority:** ESSENTIAL — fix tomorrow

At 350 MHz, ALL file read benchmarks fail with `E_FILE_NOT_FOUND` (-40). At 250 MHz, 128KB read fails but 4KB, 32KB, and 256KB succeed. The file is created, written, and closed successfully — but `openFileRead` can't find it immediately after. Suggests a timing-sensitive directory entry caching or FAT flush race condition in the driver. The faster Spin2 at 350 MHz hits the race window consistently; at 250 MHz it's intermittent.

**File:** `src/SD_card_driver.spin2`, `openFileRead()` / directory search / FAT cache flush

*Noted: 2026-02-15*

---

### Samsung 00000 8GB — FAT32 format writes but doesn't persist

**Card:** samsung-00000-8gb
**Unique ID:** `Samsung_00000_1.0_D9FB539C_201408`
**Label:** Unlabeled 8GB microSD (Chinese text/no brand) - Card #2

```
Samsung 00000 SDHC 7GB [FAT16] SD 3.x rev1.0 SN:D9FB539C 2014/08
Class 6, SPI 25 MHz
```

**Raw Registers:**
```
CID: $1B $53 $4D $30 $30 $30 $30 $30 $10 $D9 $FB $53 $9C $00 $E8 $B1
CSD: $40 $0E $00 $32 $5B $59 $00 $00 $3A $CD $7F $80 $0A $40 $00 $97
OCR: $C0FF_8000
SCR: $02 $35 $80 $03 $00 $00 $00 $00
```

**ACMD13 SD Status (verified 2026-02-15):**
```
[00-0F]: $00 $00 $00 $00 $03 $00 $00 $00 $03 $03 $90 $00 $08 $11 $09 $00
[10-3F]: all $00
```
- SPEED_CLASS (byte 8): $03 = Class 6
- UHS_SPEED_GRADE (byte 14): $00 = U0 (not defined)
- VIDEO_SPEED_CLASS (byte 15): $00 = V0 (not defined)

**CSD write-protect bits:** PERM_WRITE_PROTECT=0, TMP_WRITE_PROTECT=0

**The Problem:**

Format utility (`SD_RT_format_tests.spin2`) reports FORMAT COMPLETE — it writes MBR (partition type $0C/FAT32 LBA), VBR (OEM "P2FMTER"), FSInfo, backup boot sector, both FATs (15,046 sectors each), and root directory cluster. All write operations appear to succeed (no errors returned). Format test result: 35/46 pass, 11 fail.

However, immediately re-reading the card shows the **original factory values are still present**:
- MBR partition type: `$0E` (FAT16 LBA) — should be `$0C` (FAT32 LBA)
- VBR OEM name: `MSWIN4.1` — should be `P2FMTER`
- mount() fails with error -22 (not FAT32)

**Reproduction (2026-02-15):**

1. First format attempt: download corrupted (`P2 checksum verification FAILED`), format output appeared but was running stale code. Card unchanged.
2. Second format attempt: download successful, FORMAT COMPLETE reported, 1,922,122 clusters, 15,046 sectors/FAT written. Card unchanged — still shows FAT16/$0E/MSWIN4.1.
3. Card info test after format: 8/16 pass (Phase 1 passes, Phase 2 mount fails).

**Possible Causes:**
- Card controller silently discarding writes to sector 0 (bad block remapping or internal write-protect)
- Card-level write protection not visible in CSD bits
- Old Samsung OEM controller firmware with SPI write quirks
- Card may accept writes to FAT area but not MBR area

**Card Background:**
- Manufactured August 2014 — over 11 years old
- Unlabeled Chinese-market card, no brand markings
- Samsung MID $1B + OID "SM" — genuine Samsung flash
- Product name "00000" — OEM/internal variant, not retail
- Factory formatted FAT16 with partition type $0E (FAT16 LBA)
- All other 16 cards in the collection format successfully

**Priority:** Low — old unlabeled card, no practical impact. All branded cards format fine.

*Noted: 2026-02-15*

---
