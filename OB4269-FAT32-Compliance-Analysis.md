# OB4269 FAT32 Driver - Specification Compliance Analysis

**Driver**: Chris Gadd's FAT32 SD Card Driver (OBEX 4269)
**Specification**: Microsoft Extensible Firmware Initiative FAT32 File System Specification v1.03
**Analysis Date**: 2026-01-14

---

## Executive Summary

The OB4269 driver is a functional FAT32 implementation that works correctly with most SD/SDHC cards. However, it takes several shortcuts compared to the full Microsoft specification. Most of these deviations are pragmatic choices for an embedded system with limited resources, but some could cause issues with certain card configurations or data integrity in edge cases.

**Compliance Level**: Partial - Functional for typical use cases, but not fully spec-compliant.

---

## Compliance Issues

### 1. FSInfo Sector Not Supported

**Specification Requirement** (Section 5):
- FAT32 volumes MUST have an FSInfo sector at `BPB_FSInfo` (typically sector 1)
- Contains `FSI_Free_Count` (free cluster count) and `FSI_Nxt_Free` (hint for next free cluster)
- Signature bytes: 0x52526141 at offset 0, 0x72724161 at offset 484, 0x000055AA at offset 508

**Driver Behavior**:
- Never reads or writes FSInfo sector
- Calls `freeSpace()` to count free clusters by scanning entire FAT (slow on large cards)
- `allocateCluster()` searches from cluster 2 every time (no hint optimization)

**Impact**:
- Performance degradation on large cards (scanning 32GB FAT takes significant time)
- FSI_Free_Count on card becomes stale/incorrect after driver modifications
- Other systems reading the card may show incorrect free space

**Risk Level**: LOW - Functional, but slower and leaves stale metadata

---

### 2. Inconsistent FAT Mirroring

**Specification Requirement** (Section 4):
- Two copies of FAT exist (FAT1 and FAT2)
- If `BPB_ExtFlags` bit 7 = 0, FAT is mirrored (both copies updated)
- If `BPB_ExtFlags` bit 7 = 1, only active FAT (bits 0-3) is used

**Driver Behavior**:
```spin2
' In allocateCluster() - updates both FATs:
writeSector(fat_sec + (cluster >> 7))
writeSector(fat2_sec + (cluster >> 7))

' In deleteFile() - only updates FAT1:
writeSector(fat_sec + (c >> 7))
' FAT2 NOT updated!
```

**Impact**:
- After file deletion, FAT1 and FAT2 become inconsistent
- If FAT1 becomes corrupted, FAT2 recovery would restore deleted files as allocated
- Violates spec requirement for atomic mirroring

**Risk Level**: MEDIUM - Data integrity risk if FAT1 corruption occurs

---

### 3. BPB_ExtFlags Not Checked

**Specification Requirement**:
- Bits 0-3: Zero-based number of active FAT (if mirroring disabled)
- Bit 7: 0 = FAT is mirrored, 1 = only one FAT is active

**Driver Behavior**:
- Always assumes mirroring is enabled
- Always writes to both fat_sec and fat2_sec
- Never reads BPB_ExtFlags to determine actual configuration

**Impact**:
- If a card has mirroring disabled (bit 7 = 1), driver writes to wrong FAT copy
- Could corrupt filesystem on non-standard configurations

**Risk Level**: LOW - Most cards use default mirrored configuration

---

### 4. FAT32 Version Not Validated

**Specification Requirement** (Section 3.1):
- `BPB_FSVer` at offset 42: Major.Minor version number
- "This document defines the version as 0:0"
- "If this field is non-zero, back away from the volume"

**Driver Behavior**:
- Never reads `BPB_FSVer`
- Mounts any FAT32 volume regardless of version

**Impact**:
- Could attempt to mount future FAT32 versions with incompatible structures
- May cause data corruption on unknown formats

**Risk Level**: LOW - Current cards are all version 0:0

---

### 5. Boot Sector Signature Not Validated

**Specification Requirement** (Section 3):
- Bytes 510-511 of boot sector MUST be 0x55, 0xAA
- "This signature must be present for the media to be recognized"

**Driver Behavior**:
```spin2
PUB mount(CS_, MOSI_, MISO_, SCK_) : result | vbr_sec
  ' ... reads sector 0 ...
  vbr_sec := buf.long[$1C6 >> 2]      ' Gets VBR sector
  readSector(vbr_sec)                  ' Reads VBR
  ' NO signature check!
  fat_sec := vbr_sec + buf.word[$0E >> 1]  ' Proceeds directly
```

**Impact**:
- May attempt to parse non-FAT32 or corrupted volumes
- Could produce garbage results or crash

**Risk Level**: LOW - Initialization failures are obvious

---

### 6. High 4 Bits of FAT32 Entries Not Preserved

**Specification Requirement** (Section 4):
- FAT32 entries are 32 bits, but only low 28 bits are the cluster number
- "The high 4 bits of a FAT32 FAT entry are reserved"
- "When writing, preserve the high 4 bits; when reading, mask them off"

**Driver Behavior**:
```spin2
' In deleteFile() - clears entire 32 bits:
buf.long[(c & $7F) << 2] := 0         ' Should preserve high 4 bits!

' In allocateCluster() - writes full 32 bits:
buf.long[(next_cluster & $7F) << 2] := $0FFF_FFFF
```

**Impact**:
- Destroys reserved bits that may be used by other systems
- Could cause compatibility issues with some implementations

**Risk Level**: LOW - Most systems ignore high 4 bits anyway

---

### 7. Backup Boot Sector Not Used

**Specification Requirement** (Section 3.1):
- `BPB_BkBootSec` at offset 50: Sector number of backup boot sector (typically 6)
- "If the main boot sector is damaged, the backup can be used"

**Driver Behavior**:
- Only attempts to read primary boot sector
- No fallback to backup if primary is corrupted

**Impact**:
- Cards with damaged primary boot sector cannot be recovered
- Reduces fault tolerance

**Risk Level**: LOW - Primary sector corruption is rare

---

### 8. Root Directory Cluster Hardcoded

**Specification Requirement** (Section 3.1):
- `BPB_RootClus` at offset 44: First cluster of root directory
- "This is usually 2 but not required to be 2"

**Driver Behavior**:
```spin2
' Calculates root_sec based on cluster 2:
root_sec := cluster_offset + ((2 - 2) * sec_per_clus)
' This simplifies to: root_sec := cluster_offset

' Should read BPB_RootClus and calculate:
' root_sec := cluster_offset + ((BPB_RootClus - 2) * sec_per_clus)
```

**Impact**:
- If root directory is not at cluster 2, driver reads wrong location
- Would fail on some valid FAT32 volumes

**Risk Level**: LOW - Root is almost always at cluster 2

---

### 9. No Bad Cluster Handling

**Specification Requirement** (Section 4):
- Cluster value 0x0FFFFFF7 marks a bad cluster
- Bad clusters should never be allocated

**Driver Behavior**:
```spin2
' In allocateCluster():
repeat cluster from 2 to (sec_per_fat << 7)
  if buf.long[(cluster & $7F) << 2] == 0
    ' Allocates if zero - doesn't check for bad cluster marker
```

**Impact**:
- If a bad cluster was marked but entry corrupted to 0, driver could allocate it
- Data written to bad cluster would be lost

**Risk Level**: VERY LOW - Requires specific corruption pattern

---

### 10. End-of-Chain Detection

**Specification Requirement** (Section 4):
- EOC (End of Cluster Chain) is any value >= 0x0FFFFFF8
- Range 0x0FFFFFF8 through 0x0FFFFFFF all mean EOC

**Driver Behavior**:
```spin2
' Uses exact EOC marker:
buf.long[(next_cluster & $7F) << 2] := $0FFF_FFFF
buf.long[(cluster & $7F) << 2] := $0FFF_FFFF

' Reading - correctly handles range:
next_cluster := buf.long[(cluster & $7F) << 2] & $0FFF_FFFF
if next_cluster => $0FFF_FFF8   ' Correct range check
```

**Status**: COMPLIANT - Writes 0x0FFFFFFF, reads entire EOC range correctly

---

## Specification Deviations Summary

| Issue | Spec Section | Risk | Impact |
|-------|--------------|------|--------|
| No FSInfo support | 5 | Low | Performance, stale metadata |
| Inconsistent FAT mirroring | 4 | Medium | Data integrity on FAT1 corruption |
| BPB_ExtFlags ignored | 3.1 | Low | Wrong FAT on non-mirrored configs |
| Version not checked | 3.1 | Low | Future format incompatibility |
| No boot signature check | 3 | Low | May parse invalid volumes |
| High 4 bits not preserved | 4 | Low | Reserved bit destruction |
| No backup boot sector | 3.1 | Low | No recovery from primary damage |
| Root cluster hardcoded | 3.1 | Low | Fails on non-standard root location |
| No bad cluster handling | 4 | Very Low | Extremely rare scenario |

---

## Recommendations for Improvement

### High Priority (Data Integrity)

1. **Fix FAT mirroring in deleteFile()**
   - Add `writeSector(fat2_sec + (c >> 7))` after FAT1 write
   - Ensures both FAT copies remain synchronized

2. **Preserve high 4 bits in FAT entries**
   - Read existing value, mask in new low 28 bits
   - `buf.long[idx] := (buf.long[idx] & $F000_0000) | new_value`

### Medium Priority (Robustness)

3. **Add boot sector signature validation**
   - Check bytes 510-511 are 0x55, 0xAA before parsing
   - Return mount failure if invalid

4. **Read BPB_RootClus instead of assuming cluster 2**
   - `root_cluster := buf.long[$2C >> 2]`
   - Calculate root_sec from actual cluster number

### Low Priority (Performance/Compatibility)

5. **Implement FSInfo sector support**
   - Read/write FSI_Free_Count and FSI_Nxt_Free
   - Major performance improvement for freeSpace() calls

6. **Check BPB_FSVer before mounting**
   - Refuse to mount if version != 0:0

---

## Conclusion

The OB4269 driver is suitable for typical embedded applications with standard SD/SDHC cards. The most significant issue is the inconsistent FAT mirroring in deleteFile(), which could cause data integrity problems if FAT1 becomes corrupted and recovery from FAT2 is attempted.

For production use, consider applying the high-priority fixes above. For hobbyist/prototype use, the driver works reliably with standard cards formatted by Windows or similar tools.

---

*Analysis based on Microsoft EFI FAT32 File System Specification v1.03 and OB4269 source code review.*
