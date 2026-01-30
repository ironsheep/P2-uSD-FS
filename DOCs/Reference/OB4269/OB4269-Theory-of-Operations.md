# OB4269 FAT32 SD Card Driver - Theory of Operations

**Driver**: Chris Gadd's FAT32 SD Card Driver (OBEX 4269)
**Modified by**: Iron Sheep Productions
**Document Version**: 1.0
**Date**: 2026-01-16

---

## Table of Contents

1. [Overview](#overview)
2. [Hardware Interface](#hardware-interface)
3. [Card Initialization](#card-initialization)
4. [FAT32 Filesystem Structure](#fat32-filesystem-structure)
5. [File Operations](#file-operations)
6. [Directory Operations](#directory-operations)
7. [Cluster Management](#cluster-management)
8. [Data Buffering Strategy](#data-buffering-strategy)
9. [SPI Communication Protocol](#spi-communication-protocol)
10. [Error Handling](#error-handling)

---

## Overview

This driver provides FAT32 filesystem access to SD and SDHC cards using the SPI bus protocol. It implements a single-file-open model with a 512-byte sector buffer, optimized for embedded systems with limited memory.

### Design Philosophy

1. **Minimal Memory Footprint**: Single 512-byte buffer, reused for all operations
2. **Simple API**: One file open at a time, no file handles
3. **Read/Write Support**: Full create, read, write, delete capabilities
4. **FAT32 Compliance**: Dual FAT mirroring, FSInfo support, reserved bit preservation

### Supported Hardware

| Card Type | Capacity | Addressing |
|-----------|----------|------------|
| SD (SDSC) | Up to 2 GB | Byte addressing (×512) |
| SDHC | 2 GB - 32 GB | Block addressing |
| SDXC | Not supported | (exFAT, not FAT32) |

---

## Hardware Interface

### Pin Connections

The driver uses 4-wire SPI mode:

```
P2 Pin          SD Card Pin     Function
────────────────────────────────────────────
CS (output)  →  DAT3 (pin 1)    Chip Select (active LOW)
MOSI (output)→  CMD  (pin 2)    Master Out, Slave In
MISO (input) ←  DAT0 (pin 7)    Master In, Slave Out
SCK (output) →  CLK  (pin 5)    Serial Clock
                VSS  (pin 3,6)  Ground
                VDD  (pin 4)    3.3V Power
```

### SPI Mode

The driver operates in **SPI Mode 0**:
- CPOL = 0: Clock idle LOW
- CPHA = 0: Data sampled on rising edge, changed on falling edge

```
        ┌───┐   ┌───┐   ┌───┐   ┌───┐
SCK  ───┘   └───┘   └───┘   └───┘   └───
           ↑       ↑       ↑       ↑
         Sample  Sample  Sample  Sample

MOSI/MISO ═╪═══════╪═══════╪═══════╪════
           D7      D6      D5      D4  ...
```

---

## Card Initialization

The `initCard()` function follows the SD Physical Layer Specification for SPI mode initialization.

### Initialization Sequence

```
┌─────────────────────────────────────────────────────────────┐
│                    Power-On                                  │
└─────────────────────────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ Step 1: Power-on delay (100ms minimum)                      │
│   - VDD stabilization                                        │
│   - Card internal init                                       │
└─────────────────────────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ Step 2: Configure SPI pins                                  │
│   - CS HIGH (deselected)                                    │
│   - MOSI HIGH                                               │
│   - SCK LOW (Mode 0 idle)                                   │
│   - Clock speed ~50 kHz                                     │
└─────────────────────────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ Step 3: Send 74+ clock pulses with CS HIGH                  │
│   - Card requires this to complete power-up                 │
│   - MOSI held HIGH during clocks                            │
└─────────────────────────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ Step 4: CMD0 (GO_IDLE_STATE)                                │
│   - First command, enters SPI mode                          │
│   - Expected response: $01 (in idle state)                  │
│   - Retry up to 5 times                                     │
└─────────────────────────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ Step 5: CMD8 (SEND_IF_COND)                                 │
│   - Detects SD version                                      │
│   - Argument: $000001AA (VHS=1, pattern=$AA)                │
│   - Response $1AA: Ver 2.0+ (SDHC capable)                  │
│   - No response: Ver 1.x (SDSC only)                        │
└─────────────────────────────────────────────────────────────┘
                         │
           ┌─────────────┴─────────────┐
           ▼                           ▼
     Ver 2.0+                      Ver 1.x
     HCS=1                         HCS=0
           │                           │
           └─────────────┬─────────────┘
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ Step 6: ACMD41 loop (SD_SEND_OP_COND)                       │
│   - Requires CMD55 prefix each time                         │
│   - Argument: $40000000 (HCS=1) or $00000000 (HCS=0)        │
│   - Repeat until response = $00                             │
│   - Timeout: 2 seconds                                      │
└─────────────────────────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ Step 7: CMD58 (READ_OCR)                                    │
│   - Read Operation Conditions Register                      │
│   - Bit 30 (CCS): 1=SDHC (block addr), 0=SDSC (byte addr)  │
│   - Sets 'hcs' variable: 0 or 9 (shift amount)             │
└─────────────────────────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ Step 8: Switch to fast SPI clock                            │
│   - bit_delay := 2 (maximum speed)                          │
│   - Card now ready for data transfer                        │
└─────────────────────────────────────────────────────────────┘
```

### Addressing Mode

After initialization, the driver determines addressing mode:

| Card Type | CCS Bit | `hcs` Value | Address Calculation |
|-----------|---------|-------------|---------------------|
| SDSC | 0 | 9 | `sector << 9` (byte address) |
| SDHC | 1 | 0 | `sector << 0` (block address) |

---

## FAT32 Filesystem Structure

### Disk Layout

```
┌─────────────────────────────────────────┐  Sector 0
│           Master Boot Record            │  - Partition table
│              (MBR)                      │  - Type code at $1C2
├─────────────────────────────────────────┤
│         Reserved Sectors                │
├─────────────────────────────────────────┤  VBR sector (from MBR)
│        Volume Boot Record               │  - Filesystem parameters
│              (VBR)                      │  - BPB (BIOS Parameter Block)
├─────────────────────────────────────────┤  VBR + 1
│         FSInfo Sector                   │  - Free cluster count
│                                         │  - Next free cluster hint
├─────────────────────────────────────────┤
│         More Reserved                   │
├─────────────────────────────────────────┤  fat_sec
│     File Allocation Table 1             │  - Cluster chain map
│            (FAT1)                       │  - sec_per_fat sectors
├─────────────────────────────────────────┤  fat2_sec
│     File Allocation Table 2             │  - Mirror of FAT1
│            (FAT2)                       │
├─────────────────────────────────────────┤  root_sec
│       Root Directory                    │  - Cluster 2
│         (Cluster 2)                     │
├─────────────────────────────────────────┤
│                                         │
│         Data Region                     │  - File contents
│        (Clusters 3+)                    │  - Subdirectories
│                                         │
└─────────────────────────────────────────┘
```

### Key Variables from VBR

| Offset | Size | Variable | Description |
|--------|------|----------|-------------|
| $0B | word | - | Bytes per sector (must be 512) |
| $0D | byte | `sec_per_clus` | Sectors per cluster (1,2,4,8,16,32,64,128) |
| $0E | word | reserved | Reserved sector count |
| $10 | byte | - | Number of FATs (must be 2) |
| $24 | long | `sec_per_fat` | Sectors per FAT |
| $30 | word | `fsinfo_sec` | FSInfo sector number |
| $47 | 11 bytes | `vol_label` | Volume label |

### Calculated Values

```spin2
fat_sec := vbr_sec + reserved             ' First FAT sector
fat2_sec := fat_sec + sec_per_fat         ' Second FAT sector
root_sec := fat_sec + 2 * sec_per_fat     ' Root directory (cluster 2)
cluster_offset := root_sec // sec_per_clus ' Alignment offset
```

---

## File Operations

### Opening a File

The `searchDirectory()` function locates files:

```
┌─────────────────────────────────────────────────────────────┐
│ Input: filename string (e.g., "/FOLDER/FILE.TXT")           │
└─────────────────────────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ 1. If path starts with "/", reset to root directory         │
└─────────────────────────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ 2. Parse path components (split on "/")                     │
│    "FOLDER" then "FILE.TXT"                                 │
└─────────────────────────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ 3. Convert each component to 8.3 format                     │
│    "FILE.TXT" → "FILE    TXT"                               │
│    Uppercase, pad with spaces                               │
└─────────────────────────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ 4. Scan directory entries (32 bytes each)                   │
│    - Compare first 11 bytes (name)                          │
│    - Skip entries with $E5 (deleted) or $00 (end)           │
│    - Cross cluster boundaries via FAT chain                 │
└─────────────────────────────────────────────────────────────┘
                         │
           ┌─────────────┴─────────────┐
           ▼                           ▼
       Found                       Not Found
           │                           │
           ▼                           ▼
┌─────────────────────┐    ┌─────────────────────┐
│ Copy 32-byte entry  │    │ Record position for │
│ to entry_buffer     │    │ newFile() use       │
│ Set n_sec to first  │    │ Return false        │
│ sector of file      │    │                     │
└─────────────────────┘    └─────────────────────┘
```

### Directory Entry Structure (32 bytes)

```
Offset  Size  Field
──────────────────────────────────────────
$00     11    Short filename (8.3 format, space-padded)
$0B      1    Attributes (see below)
$0C      1    Reserved
$0D      1    Create time (tenths of second)
$0E      2    Create time (hours:minutes:seconds)
$10      2    Create date
$12      2    Last access date
$14      2    First cluster HIGH word
$16      2    Last modified time
$18      2    Last modified date
$1A      2    First cluster LOW word
$1C      4    File size in bytes
```

### Reading a File

```
┌─────────────────────────────────────────────────────────────┐
│ read(p_buffer, count)                                       │
└─────────────────────────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ 1. Verify file is open and readable                         │
│    - F_OPEN flag set                                        │
│    - Not a directory/system/hidden file                     │
└─────────────────────────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ 2. Limit count to remaining bytes in file                   │
│    count <#= (fileSize() - file_idx)                        │
└─────────────────────────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ 3. Read current sector into buffer (if not cached)          │
└─────────────────────────────────────────────────────────────┘
                         │
                         ▼
        ┌────────────────┴────────────────┐
        ▼                                 │
┌───────────────────┐                     │
│ 4. Copy bytes from│                     │
│ buffer to user    │                     │
│ (up to sector end)│                     │
└───────────────────┘                     │
        │                                 │
        ▼                                 │
┌───────────────────┐    count > 0?       │
│ 5. If sector end  │─────────────────────┘
│ reached, call     │        Yes
│ readNextSector()  │
└───────────────────┘
        │
        ▼ count == 0
┌───────────────────┐
│ Return bytes read │
└───────────────────┘
```

### Writing a File

Writing is more complex because it may require cluster allocation:

```
┌─────────────────────────────────────────────────────────────┐
│ write(p_buffer, count)                                      │
└─────────────────────────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ 1. Check if new cluster needed                              │
│    - New file (size == 0)?                                  │
│    - At end of current cluster?                             │
└─────────────────────────────────────────────────────────────┘
                         │
           ┌─────────────┴─────────────┐
           ▼                           ▼
    Need cluster              Have space
           │                           │
           ▼                           │
┌─────────────────────┐               │
│ allocateCluster()   │               │
│ - Find free cluster │               │
│ - Link in FAT chain │               │
│ - Update entry_buf  │               │
└─────────────────────┘               │
           │                           │
           └─────────────┬─────────────┘
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ 2. Copy bytes from user buffer to sector buffer             │
│    - Increment file_idx                                     │
│    - Increment file size in entry_buffer                    │
│    - Set F_NEWDATA flag                                     │
└─────────────────────────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ 3. If sector full and more data:                            │
│    - writeSector() current sector                           │
│    - Allocate new cluster if at cluster boundary            │
│    - Clear buffer                                           │
└─────────────────────────────────────────────────────────────┘
                         │
                         ▼
                   Repeat until count == 0
```

---

## Directory Operations

### Directory Structure

Directories are files containing 32-byte entries. The root directory starts at cluster 2.

```
Directory Cluster
┌────────────────────────────────────────────────────────────┐
│ Entry 0:  ".          " (this directory)     $10 attrib   │
├────────────────────────────────────────────────────────────┤
│ Entry 1:  "..         " (parent directory)   $10 attrib   │
├────────────────────────────────────────────────────────────┤
│ Entry 2:  "MYFILE  TXT" (file)              $20 attrib   │
├────────────────────────────────────────────────────────────┤
│ Entry 3:  "SUBDIR     " (subdirectory)       $10 attrib   │
├────────────────────────────────────────────────────────────┤
│ Entry 4:  $E5... (deleted entry)                          │
├────────────────────────────────────────────────────────────┤
│ Entry 5:  $00... (end of directory marker)                │
├────────────────────────────────────────────────────────────┤
│ (remaining entries unused)                                 │
└────────────────────────────────────────────────────────────┘
```

### Special Entries

| First Byte | Meaning |
|------------|---------|
| $00 | End of directory (no more entries follow) |
| $E5 | Deleted entry (can be reused) |
| $05 | First character is actually $E5 (rare) |
| Other | Normal entry |

---

## Cluster Management

### FAT Entry Format

Each FAT entry is 32 bits, but only the low 28 bits are the cluster number:

```
Bit:  31  30  29  28  27  26  ...  3   2   1   0
      ├───────────────┼────────────────────────────┤
      │   Reserved    │      Cluster Number        │
      │   (4 bits)    │        (28 bits)           │
      └───────────────┴────────────────────────────┘
```

### Special FAT Values

| Value (low 28 bits) | Meaning |
|---------------------|---------|
| $0000000 | Free cluster |
| $0000001 | Reserved |
| $0000002 - $FFFFFEF | Next cluster in chain |
| $FFFFFF0 - $FFFFFF6 | Reserved |
| $FFFFFF7 | Bad cluster |
| $FFFFFF8 - $FFFFFFF | End of chain (EOF) |

### Cluster Allocation (`allocateCluster()`)

```
┌─────────────────────────────────────────────────────────────┐
│ allocateCluster(previous_cluster)                           │
└─────────────────────────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ 1. Scan FAT for free cluster (entry == 0)                   │
│    - Start from beginning (could use FSI_Nxt_Free hint)     │
│    - Read FAT sectors as needed                             │
└─────────────────────────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ 2. Mark found cluster as end-of-chain                       │
│    - Preserve high 4 bits: (old & $F0000000) | $0FFFFFFF    │
│    - Write to both FAT1 and FAT2                            │
└─────────────────────────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ 3. If previous_cluster != 0:                                │
│    - Link previous cluster to new cluster                   │
│    - Preserve high 4 bits in previous entry                 │
│    - Write to both FAT1 and FAT2                            │
└─────────────────────────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ 4. Return new cluster number                                │
└─────────────────────────────────────────────────────────────┘
```

### Cluster ↔ Sector Conversion

```spin2
PRI clus2sec(cluster) : result
  ' Cluster 2 is at root_sec
  return (cluster - 2) * sec_per_clus + root_sec

PRI sec2clus(sector) : result
  return (sector - root_sec) / sec_per_clus + 2
```

---

## Data Buffering Strategy

### Single Buffer Architecture

The driver uses a single 512-byte buffer (`buf[512]`) for all operations:

```
┌─────────────────────────────────────────────────────────────┐
│                    buf[512]                                  │
├─────────────────────────────────────────────────────────────┤
│ Used for:                                                   │
│ • Reading file data                                         │
│ • Writing file data                                         │
│ • Reading directory sectors                                 │
│ • Reading FAT sectors                                       │
│ • Reading MBR, VBR, FSInfo                                  │
└─────────────────────────────────────────────────────────────┘
```

### Buffer Cache Tracking

```spin2
VAR
  long sec_in_buf    ' Sector number currently in buffer (-1 = invalid)
```

The `readSector()` function checks if the requested sector is already in the buffer:

```spin2
PRI readSector(sector)
  if sector == sec_in_buf    ' Already cached?
    return                   ' Skip read
  sec_in_buf := sector       ' Update cache tag
  ' ... perform actual read ...
```

### Cache Invalidation

The cache must be invalidated when:
1. FAT operations load a different sector
2. Directory operations load a different sector
3. `countFreeClusters()` scans FAT (sets `sec_in_buf := -1`)

---

## SPI Communication Protocol

### Command Format

All SD commands are 6 bytes:

```
Byte 0:  0 1 [command number (6 bits)]
         └─┴─ Start bits (01)

Byte 1-4: 32-bit argument (MSB first)

Byte 5:  [CRC7 (7 bits)] 1
                          └─ Stop bit
```

### Response Types

| Type | Size | Description |
|------|------|-------------|
| R1 | 1 byte | Status flags |
| R3 | 5 bytes | R1 + 32-bit OCR |
| R7 | 5 bytes | R1 + 32-bit echo (CMD8) |

### R1 Response Format

```
Bit 7: Always 0
Bit 6: Parameter error
Bit 5: Address error
Bit 4: Erase sequence error
Bit 3: CRC error
Bit 2: Illegal command
Bit 1: Erase reset
Bit 0: In idle state
```

### Data Transfer

**Read Operation (CMD17)**:
```
Host:    [CMD17] ────────────────────────────────────────────►
Card:              [R1] [wait...] [Start Token $FE] [512 bytes] [CRC16]
```

**Write Operation (CMD24)**:
```
Host:    [CMD24] [R1] [Start Token $FE] [512 bytes] [CRC16] ──►
Card:                                                [Response] [Busy...]
```

---

## Error Handling

### Timeout Handling

| Operation | Timeout | Recovery |
|-----------|---------|----------|
| CMD0 retry | 5 attempts | Return false |
| ACMD41 init | 2 seconds | Return false |
| Command response | 1 second | Deselect, return false |
| Write busy | 1 second | Deselect, return false |

### Return Values

Most operations return boolean success/failure. The driver does not provide detailed error codes.

### Data Integrity Measures

1. **Dual FAT Updates**: All FAT modifications write to both FAT1 and FAT2
2. **Reserved Bit Preservation**: High 4 bits of FAT entries are preserved
3. **FSInfo Updates**: `unmount()` writes accurate free cluster count
4. **Sync Support**: `sync()` flushes data without closing file

---

## Appendix: State Diagram

### File State Machine

```
                    ┌──────────┐
                    │  CLOSED  │◄─────────────────────────────┐
                    └────┬─────┘                              │
                         │                                    │
         openFile() or newFile()                         closeFile()
                         │                                    │
                         ▼                                    │
                    ┌──────────┐                              │
             ┌─────►│   OPEN   │──────────────────────────────┤
             │      └────┬─────┘                              │
             │           │                                    │
             │      read()/write()                            │
             │           │                                    │
             │           ▼                                    │
             │      ┌──────────┐                              │
             │      │ MODIFIED │──────────────────────────────┘
             │      │(F_NEWDATA│
      sync() │      │ or       │
             │      │ F_NEWDIR)│
             │      └────┬─────┘
             │           │
             └───────────┘
```

---

*Theory of Operations for P2-uSD-Study project*
