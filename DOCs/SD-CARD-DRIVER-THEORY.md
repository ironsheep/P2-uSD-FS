# SD_card_driver.spin2 - Theory of Operations

## Overview

The SD card driver provides full FAT32 filesystem access for the Parallax Propeller 2 (P2). It runs a dedicated worker cog that owns the SPI hardware, accepts commands from any calling cog via a shared mailbox, and serializes all card access through a hardware lock.

Key architectural features:
- **Smart pin SPI** with streamer DMA for hardware-accelerated sector transfers
- **Multi-file handle system** supporting up to 4 simultaneous file and directory handles
- **Per-cog current working directory** for safe multi-cog filesystem navigation
- **Single-writer policy** preventing concurrent write corruption
- **Hardware-accelerated CRC-16** using the P2's `GETCRC` instruction

## Architecture

```
  ┌──────────────────────────────────────────────────────────┐
  │  Calling Cog(s) (up to 8)                                │
  │  ┌──────────┐ ┌──────────┐ ┌──────────┐                  │
  │  │ Cog 0    │ │ Cog 1    │ │ Cog N    │                  │
  │  │ CWD: /   │ │ CWD: /A  │ │ CWD: /B  │                  │
  │  └────┬─────┘ └────┬─────┘ └────┬─────┘                  │
  │       │            │            │                         │
  │       └────────────┼────────────┘                         │
  │                    │ send_command()                        │
  │              ┌─────▼─────┐                                │
  │              │ Mailbox   │  pb_cmd, pb_param0..3          │
  │              │ (Hub RAM) │  pb_status, pb_data0..1        │
  │              └─────┬─────┘                                │
  │                    │                                      │
  │              ┌─────▼─────────────────┐                    │
  │              │ Worker Cog (fs_worker) │                    │
  │              │ - Owns SPI pins       │                    │
  │              │ - Command dispatch    │                    │
  │              │ - FAT32 operations    │                    │
  │              └─────┬─────────────────┘                    │
  │                    │                                      │
  │              ┌─────▼─────┐                                │
  │              │ Smart Pin │  P_TRANSITION (SCK)            │
  │              │ SPI + DMA │  P_SYNC_TX (MOSI)             │
  │              │           │  P_SYNC_RX (MISO)             │
  │              └─────┬─────┘                                │
  │                    │                                      │
  └────────────────────┼──────────────────────────────────────┘
                       │
                 ┌─────▼─────┐
                 │  SD Card  │
                 └───────────┘
```

## Driver Modes

The driver operates in three modes:

| Mode | Value | Set By | Allowed Operations |
|------|-------|--------|--------------------|
| `MODE_NONE` | 0 | Initial state | Only `mount()` or `initCardOnly()` |
| `MODE_RAW` | 1 | `initCardOnly()` | Raw sector read/write only |
| `MODE_FILESYSTEM` | 2 | `mount()` | Full filesystem + raw access |

Filesystem commands are rejected with `E_NOT_MOUNTED` when not in `MODE_FILESYSTEM`.

## Worker Cog and Command Protocol

### Mailbox Registers

All cog-to-worker communication flows through shared hub RAM variables:

| Register | Direction | Purpose |
|----------|-----------|---------|
| `pb_cmd` | Caller -> Worker | Command opcode (0 = idle) |
| `pb_param0..3` | Caller -> Worker | Command parameters |
| `pb_caller` | Caller -> Worker | Calling cog's ID (for COGATN wakeup) |
| `pb_status` | Worker -> Caller | Result status code |
| `pb_data0..1` | Worker -> Caller | Result data (handle, count, pointer) |

### Command Flow

1. Caller acquires hardware lock (prevents other cogs from sending commands)
2. Caller writes `pb_param0..3`, `pb_caller := COGID()`, then `pb_cmd := command`
3. Caller polls `pb_cmd` until it returns to `CMD_NONE` (worker clears it when done)
4. Worker wakes caller via `COGATN(1 << pb_caller)`
5. Caller reads `pb_status` and `pb_data0` for results
6. Caller releases hardware lock

### Command Opcodes

**Core Filesystem Commands:**

| Command | Value | Parameters | Returns |
|---------|-------|------------|---------|
| `CMD_MOUNT` | 1 | (pins via DAT) | status |
| `CMD_UNMOUNT` | 2 | -- | status |
| `CMD_DELETE` | 10 | param0=filename | status |
| `CMD_RENAME` | 11 | param0=old, param1=new | status |
| `CMD_CHDIR` | 12 | param0=path | status |
| `CMD_READDIR` | 13 | param0=entry index | data0=entry ptr |
| `CMD_FREESPACE` | 15 | -- | data0=free sectors |
| `CMD_MOVEFILE` | 17 | param0=name, param1=dest | status |

**Handle-Based File Commands:**

| Command | Value | Parameters | Returns |
|---------|-------|------------|---------|
| `CMD_OPEN_READ` | 30 | param0=path | data0=handle |
| `CMD_OPEN_WRITE` | 31 | param0=path | data0=handle |
| `CMD_CREATE` | 32 | param0=path | data0=handle |
| `CMD_CLOSE_H` | 33 | param0=handle | status |
| `CMD_READ_H` | 34 | param0=handle, param1=buf, param2=count | data0=bytes |
| `CMD_WRITE_H` | 35 | param0=handle, param1=buf, param2=count | data0=bytes |
| `CMD_SEEK_H` | 36 | param0=handle, param1=position | status |
| `CMD_TELL_H` | 37 | param0=handle | data0=position |
| `CMD_FILESIZE_H` | 38 | param0=handle | data0=size |
| `CMD_SYNC_H` | 39 | param0=handle | status |
| `CMD_SYNC_ALL` | 40 | -- | status |
| `CMD_EOF_H` | 41 | param0=handle | data0=bool |

**Directory Handle Commands:**

| Command | Value | Parameters | Returns |
|---------|-------|------------|---------|
| `CMD_OPEN_DIR` | 43 | param0=path | data0=handle |
| `CMD_READ_DIR_H` | 44 | param0=handle | data0=entry ptr |
| `CMD_CLOSE_DIR_H` | 45 | param0=handle | status |

**Raw / Diagnostic Commands (conditional):**

| Command | Value | Guard | Purpose |
|---------|-------|-------|---------|
| `CMD_READ_SECTORS` | 18 | `SD_INCLUDE_RAW` | Multi-block read (CMD18) |
| `CMD_WRITE_SECTORS` | 19 | `SD_INCLUDE_RAW` | Multi-block write (CMD25) |
| `CMD_READ_SECTOR_RAW` | 20 | `SD_INCLUDE_RAW` | Single sector read |
| `CMD_WRITE_SECTOR_RAW` | 21 | `SD_INCLUDE_RAW` | Single sector write |
| `CMD_INIT_CARD_ONLY` | 22 | `SD_INCLUDE_RAW` | Raw mode init (no FS) |
| `CMD_GET_CARD_SIZE` | 23 | `SD_INCLUDE_RAW` | Card capacity in sectors |
| `CMD_READ_SCR` | 24 | `SD_INCLUDE_REGISTERS` | Read SCR register |
| `CMD_READ_CID` | 27 | `SD_INCLUDE_REGISTERS` | Read CID register |
| `CMD_READ_CSD` | 28 | `SD_INCLUDE_REGISTERS` | Read CSD register |

## Handle System

### Unified Handle Pool

File handles and directory handles share a single pool of `MAX_OPEN_FILES` slots (default 4, user-configurable). Each slot can hold either a file or a directory enumeration handle.

### Handle Flags

| Flag | Value | Meaning |
|------|-------|---------|
| `HF_FREE` | 0 | Slot available |
| `HF_READ` | 1 | Open for reading |
| `HF_WRITE` | 2 | Open for writing |
| `HF_DIR` | 4 | Directory enumeration handle |
| `HF_DIRTY` | $80 | Buffer has pending writes (OR'd with mode) |

### Per-Handle State

Each handle slot contains:

| Array | Type | File Use | Directory Use |
|-------|------|----------|---------------|
| `h_flags` | BYTE | HF_READ or HF_WRITE | HF_DIR |
| `h_attr` | BYTE | FAT attributes byte | Directory attributes |
| `h_start_clus` | LONG | First cluster of file | First cluster of directory |
| `h_cluster` | LONG | Current cluster in chain | Current cluster during enum |
| `h_sector` | LONG | Current data sector | Current directory sector |
| `h_position` | LONG | Byte offset in file | Entry index (0-based) |
| `h_size` | LONG | File size in bytes | 0 (unused) |
| `h_dir_sector` | LONG | Sector of directory entry | 0 (unused) |
| `h_dir_offset` | WORD | Offset within dir sector | 0 (unused) |
| `h_buf[512]` | BYTE | Per-handle data buffer | Per-handle sector cache |
| `h_buf_sector` | LONG | Sector in buffer (-1=none) | Sector in buffer (-1=none) |

### Handle Lifecycle

```
allocateHandle()     Find free slot (h_flags == HF_FREE)
       │
       ▼
  Populate state     Caller sets h_flags, h_start_clus, h_sector, etc.
       │
       ▼
  Use handle          readHandle / writeHandle / readDirectoryHandle
       │
       ▼
  closeFileHandle()   Flush dirty buffer, update dir entry, freeHandle()
  or closeDirectoryHandle()
       │
       ▼
  freeHandle()        Clear all state, h_flags := HF_FREE
```

### Handle Type Guards

File operations reject directory handles and vice versa:

- `readHandle()`, `writeHandle()`, `seekHandle()`, etc. check `h_flags[handle] & HF_DIR` and return `E_NOT_A_DIR_HANDLE` if set
- `readDirectoryHandle()` checks `h_flags[handle] <> HF_DIR` and returns `E_INVALID_HANDLE` if not a directory handle

### Single-Writer Policy

The driver prevents two handles from writing the same file simultaneously:

1. Each file is uniquely identified by its `(dir_sector, dir_offset)` pair
2. `isFileOpenForWrite()` scans all handles for a matching pair with `HF_WRITE` set
3. `openFileWrite()` and write operations call this check before proceeding
4. Multiple read handles to the same file are allowed
5. Violation returns `E_FILE_ALREADY_OPEN` (-92)

## Per-Cog Current Working Directory

Each P2 cog maintains its own current working directory:

```spin2
DAT
  cog_dir_sec   LONG    0[8]    ' Per-cog CWD sector (one per P2 cog)
```

- Indexed by `pb_caller` (the calling cog's ID)
- Initialized to `root_sec` for all 8 cogs at mount time
- `changeDirectory()` only modifies `cog_dir_sec[pb_caller]`
- `searchDirectory()` reads `cog_dir_sec[pb_caller]` as its starting point
- `readDirectory()` enumerates from `cog_dir_sec[pb_caller]`

This ensures Cog A can `cd /FOLDER1` while Cog B does `cd /FOLDER2` without interference.

## Buffer Management

### Shared Buffers

Three 512-byte shared buffers serve the worker cog's internal operations:

| Buffer | Cache Variable | Purpose |
|--------|---------------|---------|
| `buf` (512 bytes) | `sec_in_buf` | General data sector I/O |
| `dir_buf` (512 bytes) | `dir_sec_in_buf` | Directory sector reads |
| `fat_buf` (512 bytes) | `fat_sec_in_buf` | FAT table reads/writes |

Additionally, `entry_buffer` (32 bytes) holds the most recently read directory entry.

### Per-Handle Buffers

Each handle has its own 512-byte sector buffer:

```spin2
h_buf           BYTE    0[512 * MAX_OPEN_FILES]   ' 512 bytes per handle
h_buf_sector    LONG    0[MAX_OPEN_FILES]          ' Which sector is cached (-1 = none)
```

Per-handle buffers eliminate thrashing when alternating between multiple open files. The pointer to a handle's buffer is `@h_buf + (handle * 512)`.

### Memory Cost

| Component | Size |
|-----------|------|
| Shared buffers (buf + dir_buf + fat_buf) | 1,536 bytes |
| Entry buffer | 32 bytes |
| Per-handle state (28 bytes x 4) | 112 bytes |
| Per-handle buffers (512 bytes x 4) | 2,048 bytes |
| Per-cog CWD (8 LONGs) | 32 bytes |
| Worker cog stack | ~512 bytes |
| **Total (4 handles)** | **~4,272 bytes** |

## SPI Implementation

### Smart Pin Configuration

The driver uses three smart pin modes for SPI mode 0 (CPOL=0, CPHA=0):

| Pin | Smart Pin Mode | Purpose |
|-----|---------------|---------|
| SCK | `P_TRANSITION \| P_OE` | Clock generation, idle LOW |
| MOSI | `P_SYNC_TX \| P_OE \| P_PLUS2_B` | Synchronized transmit, clocked by SCK |
| MISO | `P_SYNC_RX \| P_PLUS3_B` | Synchronized receive, clocked by SCK |

The `P_PLUS2_B` and `P_PLUS3_B` selectors route the SCK signal to the TX and RX pins respectively, based on the pin assignments (MISO, MOSI, CS, SCK are consecutive pins).

### Frequency Control

`setSPISpeed(hz)` calculates the SCK half-period from the system clock:

```
half_period = (CLKFREQ / hz / 2) - 1
```

This provides sysclk-independent SPI timing. The driver starts at ~400 kHz for card initialization (SD specification requirement), then switches to the card's maximum speed (typically 25 MHz) after init.

### Streamer DMA

Sector transfers use the P2's streamer engine for hardware-accelerated bulk data movement:

**Read (512 bytes from card):**
```
xinit  stream_mode, init_phase   ' Start RX streamer
waitxfi                          ' Wait for 512 bytes received
```

**Write (512 bytes to card):**
```
rdfast #0, p_buf                 ' Setup hub read pointer
xinit  stream_mode, #0           ' Start TX streamer
wypin  clk_count, sck            ' Generate clock pulses
waitxfi                          ' Wait for completion
```

The streamer transfers data between hub RAM and the SPI pins at the full SPI clock rate, without per-byte cog intervention.

## Multi-Sector Operations

### CMD18 (READ_MULTIPLE_BLOCK)

`readSectors(start_sector, count, p_buffer)` reads consecutive sectors in a single command:

1. Send CMD18 with starting sector address
2. For each sector: wait for `$FE` token, streamer-receive 512 bytes, validate CRC
3. Send CMD12 (STOP_TRANSMISSION) to end the transfer
4. Verify card status with CMD13

### CMD25 (WRITE_MULTIPLE_BLOCK)

`writeSectors(start_sector, count, p_buffer)` writes consecutive sectors:

1. Send CMD25 with starting sector address
2. For each sector: send `$FC` token, streamer-transmit 512 bytes + CRC, wait for card busy
3. Send `$FD` stop token, wait for final programming
4. Verify card status with CMD13

Multi-sector operations are significantly faster than single-sector loops because they eliminate per-sector command overhead and allow the card's internal controller to optimize flash writes for sequential access.

## CRC-16 Validation

The driver validates data integrity using CRC-16-CCITT on every sector transfer:

```spin2
PRI calcDataCRC(pData, len) : crc | raw
  raw := GETCRC(pData, CRC_POLY_REFLECTED, len)
  crc := ((raw ^ CRC_BASE_512) REV 31) >> 16
```

- Uses the P2's hardware `GETCRC` instruction (no lookup table needed)
- `CRC_POLY_REFLECTED` ($8408) is the CRC-16-CCITT polynomial in LSB-first form
- `CRC_BASE_512` ($2C68) compensates for `GETCRC` initialization differences
- The `REV 31` + `>> 16` converts from reflected to standard bit order
- Match/mismatch counters available via `getCRCMatchCount()` and `getCRCMismatchCount()`
- Can be disabled for performance via `setCRCValidation(false)`

## Conditional Compilation

The driver supports conditional compilation to reduce code size for resource-constrained applications:

| Flag | Default | Features Included |
|------|---------|-------------------|
| `SD_MINIMAL` | not defined | When defined, excludes all optional features |
| `SD_INCLUDE_RAW` | off | Raw sector read/write, `initCardOnly()`, card size query |
| `SD_INCLUDE_REGISTERS` | off | CID, CSD, SCR register read methods |
| `SD_INCLUDE_SPEED` | off | CMD6 high-speed mode, `setOptimalSpeed()` |
| `SD_INCLUDE_ALL` | off | Enables all of the above |

The caller defines flags in their `CON` section before the `OBJ` declaration:

```spin2
CON
  SD_INCLUDE_ALL = 1    ' Enable all driver features

OBJ
  sd : "SD_card_driver"
```

## Error Codes

| Error | Value | Meaning |
|-------|-------|---------|
| `E_TIMEOUT` | -1 | Card didn't respond in time |
| `E_NO_RESPONSE` | -2 | Card not responding |
| `E_BAD_RESPONSE` | -3 | Unexpected response from card |
| `E_CRC_ERROR` | -4 | Data CRC mismatch |
| `E_WRITE_REJECTED` | -5 | Card rejected write operation |
| `E_CARD_BUSY` | -6 | Card busy timeout |
| `E_IO_ERROR` | -7 | General I/O error |
| `E_NOT_MOUNTED` | -20 | Filesystem not mounted |
| `E_INIT_FAILED` | -21 | Card initialization failed |
| `E_NOT_FAT32` | -22 | Card not formatted as FAT32 |
| `E_BAD_SECTOR_SIZE` | -23 | Sector size not 512 bytes |
| `E_FILE_NOT_FOUND` | -40 | File doesn't exist |
| `E_FILE_EXISTS` | -41 | File already exists |
| `E_NOT_A_FILE` | -42 | Expected file, found directory |
| `E_NOT_A_DIR` | -43 | Expected directory, found file |
| `E_FILE_NOT_OPEN` | -45 | File not open |
| `E_END_OF_FILE` | -46 | Read past end of file |
| `E_DISK_FULL` | -60 | No free clusters available |
| `E_NO_LOCK` | -64 | Could not acquire hardware lock |
| `E_TOO_MANY_FILES` | -90 | All handle slots in use |
| `E_INVALID_HANDLE` | -91 | Handle out of range or not open |
| `E_FILE_ALREADY_OPEN` | -92 | File already open for writing |
| `E_NOT_A_DIR_HANDLE` | -93 | Wrong handle type for operation |

## Public API Summary

### Mounting

| Method | Description |
|--------|-------------|
| `mount(cs, mosi, miso, sck)` | Initialize card and mount filesystem |
| `unmount()` | Flush all handles, sync, release worker cog |

### File Operations (Handle-Based)

| Method | Description |
|--------|-------------|
| `openFileRead(pPath) : handle` | Open existing file for reading |
| `openFileWrite(pPath) : handle` | Open existing file for writing (append) |
| `createFileNew(pPath) : handle` | Create new file for writing |
| `readHandle(handle, pBuf, count) : bytes` | Read bytes from file |
| `writeHandle(handle, pBuf, count) : bytes` | Write bytes to file |
| `seekHandle(handle, position) : result` | Seek to byte position |
| `tellHandle(handle) : position` | Get current byte position |
| `eofHandle(handle) : bool` | Check if at end of file |
| `fileSizeHandle(handle) : size` | Get file size in bytes |
| `syncHandle(handle) : result` | Flush pending writes |
| `closeFileHandle(handle) : result` | Close handle, flush writes |

### Directory Operations

| Method | Description |
|--------|-------------|
| `changeDirectory(pPath) : result` | Change calling cog's CWD |
| `newDirectory(pName) : result` | Create new directory |
| `readDirectory(entry) : pEntry` | Enumerate CWD by index |
| `openDirectory(pPath) : handle` | Open directory for handle-based enumeration |
| `readDirectoryHandle(handle) : pEntry` | Read next directory entry |
| `closeDirectoryHandle(handle)` | Close directory handle |
| `deleteFile(pName) : result` | Delete file or empty directory |
| `rename(pOld, pNew) : result` | Rename file or directory |

### Information

| Method | Description |
|--------|-------------|
| `freeSpace() : sectors` | Free space in 512-byte sectors |
| `volumeLabel() : pStr` | Pointer to volume label string |
| `fileName() : pStr` | Name from last directory read |
| `fileSize() : size` | Size from last directory read |
| `attributes() : attr` | Attributes from last directory read |
| `error() : code` | Last error code |

### Diagnostics

| Method | Description |
|--------|-------------|
| `getCRCMatchCount() : count` | Sector reads with valid CRC |
| `getCRCMismatchCount() : count` | Sector reads with CRC failure |
| `setCRCValidation(enabled)` | Enable/disable CRC checking |
| `getSPIFrequency() : hz` | Current SPI clock frequency |
| `getReadTimeout() : ms` | Card read timeout from CSD |
| `getWriteTimeout() : ms` | Card write timeout from CSD |

---

*Part of the [P2-SD-Card-Driver](../../README.md) project - Iron Sheep Productions*
