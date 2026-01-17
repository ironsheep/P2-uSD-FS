# OB4269 FAT32 SD Card Driver - API Reference

**Driver**: Chris Gadd's FAT32 SD Card Driver (OBEX 4269)
**Modified by**: Iron Sheep Productions
**Document Version**: 1.0
**Date**: 2026-01-16

---

## Overview

This document describes the public interface (PUB methods) of the SD card driver. The driver provides FAT32 filesystem access to SD/SDHC cards via SPI interface.

### Quick Start Example

```spin2
OBJ
  sd : "SD_card_driver"

PUB main() | buf[128]
  ' Mount card (pins for P2 Edge)
  if sd.mount(60, 59, 58, 61)
    ' Create and write a file
    sd.newFile(string("TEST.TXT"))
    sd.writeString(string("Hello, World!"))
    sd.closeFile()

    ' Read it back
    sd.openFile(string("TEST.TXT"))
    sd.read(@buf, sd.fileSize())
    sd.closeFile()

    ' Clean unmount
    sd.unmount()
```

---

## Card Management Methods

### mount()

```spin2
PUB mount(cs, mosi, miso, sck) : result
```

Initialize and mount an SD card.

| Parameter | Type | Description |
|-----------|------|-------------|
| `cs` | long | Pin number for Chip Select (directly connected to DAT3) |
| `mosi` | long | Pin number for Master Out Slave In (connected to CMD) |
| `miso` | long | Pin number for Master In Slave Out (connected to DAT0) |
| `sck` | long | Pin number for SPI Clock (connected to CLK) |

| Returns | Description |
|---------|-------------|
| `true` | Card mounted successfully |
| `false` | Mount failed (card not present, not FAT32, or init error) |

**Notes**:
- Card must be formatted as FAT32
- Supports SD (up to 2GB) and SDHC (up to 32GB) cards
- Sets default timestamp to 2009-01-27 07:00:00 if not previously set
- Reads FSInfo sector for free space tracking

**Example**:
```spin2
if not sd.mount(60, 59, 58, 61)
  debug("Mount failed!")
  return
```

---

### unmount()

```spin2
PUB unmount() : result
```

Cleanly unmount the SD card.

| Returns | Description |
|---------|-------------|
| `true` | FSInfo updated successfully |
| `false` | FSInfo update failed (but card still safe to remove) |

**Notes**:
- Closes any open file
- Updates FSInfo sector with correct free cluster count
- **Always call before removing card or power** to ensure other OSes see correct free space

**Example**:
```spin2
sd.unmount()
' Safe to remove card now
```

---

### sync()

```spin2
PUB sync() : result
```

Flush all pending writes without closing the file.

| Returns | Description |
|---------|-------------|
| `true` | Sync completed |

**Notes**:
- Writes buffered data to card
- Updates directory entry with current file size
- File remains open for continued access
- Use for checkpointing during long write operations

**Example**:
```spin2
repeat i from 0 to 999
  sd.write(@sensor_data, 64)
  if i // 100 == 0
    sd.sync()  ' Checkpoint every 100 records
```

---

## File Operations

### newFile()

```spin2
PUB newFile(name_ptr) : result
```

Create and open a new file in the current directory.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name_ptr` | pointer | Pointer to null-terminated 8.3 filename string |

| Returns | Description |
|---------|-------------|
| `true` | File created and opened |
| `false` | File already exists or creation failed |

**Notes**:
- Closes any previously open file
- File is opened for writing
- Filename must be 8.3 format (e.g., "FILENAME.TXT")
- Filename is converted to uppercase automatically

**Example**:
```spin2
if sd.newFile(string("DATA.LOG"))
  sd.writeString(string("Log started"))
  sd.closeFile()
```

---

### openFile()

```spin2
PUB openFile(name_ptr) : result
```

Open an existing file for reading or writing.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name_ptr` | pointer | Pointer to filename or path string |

| Returns | Description |
|---------|-------------|
| `true` | File opened successfully |
| `false` | File not found or is a directory |

**Notes**:
- Closes any previously open file
- Supports paths with "/" separator
- Path starting with "/" resets to root directory
- Does not open directories, system files, or hidden files

**Example**:
```spin2
' Open file in current directory
sd.openFile(string("CONFIG.TXT"))

' Open file with absolute path
sd.openFile(string("/LOGS/DAY1.LOG"))

' Open file with relative path
sd.openFile(string("SUBDIR/FILE.TXT"))
```

---

### closeFile()

```spin2
PUB closeFile()
```

Close the currently open file and write all pending data.

**Notes**:
- Writes any buffered data to card
- Updates directory entry with final file size
- Must be called after writing to ensure data is saved
- Safe to call even if no file is open

**Example**:
```spin2
sd.newFile(string("OUTPUT.BIN"))
sd.write(@data, 1024)
sd.closeFile()  ' Data now safely on card
```

---

### deleteFile()

```spin2
PUB deleteFile(name_ptr) : result
```

Delete a file from the filesystem.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name_ptr` | pointer | Pointer to filename string |

| Returns | Description |
|---------|-------------|
| `true` | File deleted |
| `false` | File not found or protected |

**Notes**:
- Cannot delete read-only, system, hidden, or directory entries
- Frees all clusters in the file's chain
- Updates both FAT1 and FAT2 tables

**Example**:
```spin2
if sd.deleteFile(string("OLD.TXT"))
  debug("File deleted")
```

---

### rename()

```spin2
PUB rename(old_name, new_name) : result
```

Rename a file or directory.

| Parameter | Type | Description |
|-----------|------|-------------|
| `old_name` | pointer | Current filename (may include path) |
| `new_name` | pointer | New filename (path portion ignored) |

| Returns | Description |
|---------|-------------|
| `true` | Renamed successfully |
| `false` | Source not found or destination exists |

**Example**:
```spin2
sd.rename(string("TEMP.TXT"), string("FINAL.TXT"))
sd.rename(string("/LOGS/OLD.LOG"), string("NEW.LOG"))
```

---

### moveFile()

```spin2
PUB moveFile(name_ptr, dest_folder) : result
```

Move a file to a different directory.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name_ptr` | pointer | Source filename (may include path) |
| `dest_folder` | pointer | Destination directory name |

| Returns | Description |
|---------|-------------|
| `true` | File moved |
| `false` | Source not found, destination doesn't exist, or same directory |

**Example**:
```spin2
sd.moveFile(string("REPORT.TXT"), string("ARCHIVE"))
```

---

## Read/Write Methods

### read()

```spin2
PUB read(p_buffer, count) : result
```

Read bytes from the open file into a buffer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `p_buffer` | pointer | Destination buffer address |
| `count` | long | Maximum bytes to read |

| Returns | Description |
|---------|-------------|
| result | Actual bytes read (may be less if EOF reached) |

**Notes**:
- Automatically advances file position
- Handles cluster chain traversal transparently
- Returns 0 if file not open or at EOF

**Example**:
```spin2
VAR
  byte buf[512]

PUB readConfig()
  if sd.openFile(string("CONFIG.BIN"))
    bytes_read := sd.read(@buf, 512)
    sd.closeFile()
```

---

### readByte()

```spin2
PUB readByte(address) : result
```

Read a single byte from a specific position in the file.

| Parameter | Type | Description |
|-----------|------|-------------|
| `address` | long | Byte offset within file |

| Returns | Description |
|---------|-------------|
| result | Byte value at that position |

**Notes**:
- Seeks to position before reading
- Does not advance file position persistently

---

### write()

```spin2
PUB write(p_buffer, count) : result
```

Write bytes from a buffer to the open file.

| Parameter | Type | Description |
|-----------|------|-------------|
| `p_buffer` | pointer | Source buffer address |
| `count` | long | Number of bytes to write |

| Returns | Description |
|---------|-------------|
| result | (not currently used) |

**Notes**:
- Appends to end of file
- Automatically allocates new clusters as needed
- Data is buffered; call `closeFile()` or `sync()` to ensure it's written

**Example**:
```spin2
sd.newFile(string("DATA.BIN"))
sd.write(@sensor_readings, 256)
sd.write(@more_data, 128)
sd.closeFile()
```

---

### writeByte()

```spin2
PUB writeByte(char) : result
```

Write a single byte to the file.

| Parameter | Type | Description |
|-----------|------|-------------|
| `char` | byte | Byte value to write |

---

### writeString()

```spin2
PUB writeString(p_str) : result
```

Write a null-terminated string to the file.

| Parameter | Type | Description |
|-----------|------|-------------|
| `p_str` | pointer | Pointer to null-terminated string |

**Notes**:
- Does not write the null terminator
- Equivalent to `write(p_str, strsize(p_str))`

**Example**:
```spin2
sd.newFile(string("LOG.TXT"))
sd.writeString(string("Timestamp: "))
sd.writeString(string("2026-01-16"))
sd.writeByte(13)  ' CR
sd.writeByte(10)  ' LF
sd.closeFile()
```

---

### seek()

```spin2
PUB seek(pos) : result
```

Set the file position for subsequent read operations.

| Parameter | Type | Description |
|-----------|------|-------------|
| `pos` | long | Byte offset from start of file |

| Returns | Description |
|---------|-------------|
| `true` | Position set |
| `false` | Position beyond EOF |

**Notes**:
- Follows cluster chain to find correct sector
- Loads appropriate sector into buffer

**Example**:
```spin2
sd.openFile(string("BIGFILE.DAT"))
sd.seek(1024)           ' Skip first 1KB
sd.read(@buf, 512)      ' Read bytes 1024-1535
sd.closeFile()
```

---

## Directory Methods

### newDirectory()

```spin2
PUB newDirectory(name_ptr) : result
```

Create a new subdirectory in the current directory.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name_ptr` | pointer | Directory name (8.3 format) |

| Returns | Description |
|---------|-------------|
| `true` | Directory created |
| `false` | Already exists or creation failed |

**Notes**:
- Creates "." and ".." entries automatically
- Allocates one cluster for the new directory

**Example**:
```spin2
sd.newDirectory(string("LOGS"))
sd.changeDirectory(string("LOGS"))
sd.newFile(string("DAY1.LOG"))
```

---

### changeDirectory()

```spin2
PUB changeDirectory(name_ptr) : result
```

Change the current working directory.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name_ptr` | pointer | Directory name or path |

| Returns | Description |
|---------|-------------|
| `true` | Directory changed |
| `false` | Not found or not a directory |

**Notes**:
- Use ".." to go up one level
- Use "/" prefix for absolute path from root
- Closes any open file

**Example**:
```spin2
sd.changeDirectory(string("SUBDIR"))
sd.changeDirectory(string(".."))        ' Back to parent
sd.changeDirectory(string("/"))         ' Back to root
sd.changeDirectory(string("/LOGS/2026")) ' Absolute path
```

---

### readDirectory()

```spin2
PUB readDirectory(entry) : result
```

Iterate through directory entries.

| Parameter | Type | Description |
|-----------|------|-------------|
| `entry` | long | Entry index (0, 1, 2, ...) |

| Returns | Description |
|---------|-------------|
| `@entry_buffer` | Pointer to 32-byte directory entry |
| `false` | No more entries |

**Notes**:
- Skips deleted entries ($E5) and special entries
- Does not show volume ID, system, or hidden files
- Use with `fileName()` to get human-readable name

**Example**:
```spin2
PUB listDirectory() | i
  i := 0
  repeat until sd.readDirectory(i++) == 0
    debug(zstr(sd.fileName()))
    if sd.attributes() & $10
      debug(" <DIR>")
    else
      debug(" ", udec(sd.fileSize()), " bytes")
```

---

## Information Methods

### fileName()

```spin2
PUB fileName() : result
```

Get the filename of the currently open/selected file.

| Returns | Description |
|---------|-------------|
| pointer | Pointer to 8.3 filename string with dot separator |

**Example**:
```spin2
sd.openFile(string("README.TXT"))
debug("Opened: ", zstr(sd.fileName()))  ' "README.TXT"
```

---

### fileSize()

```spin2
PUB fileSize() : result
```

Get the size of the currently open file.

| Returns | Description |
|---------|-------------|
| long | File size in bytes |

---

### attributes()

```spin2
PUB attributes() : result
```

Get the attribute byte of the current file.

| Returns | Description |
|---------|-------------|
| byte | FAT32 attribute flags |

| Bit | Mask | Meaning |
|-----|------|---------|
| 0 | $01 | Read-only |
| 1 | $02 | Hidden |
| 2 | $04 | System |
| 3 | $08 | Volume ID |
| 4 | $10 | Directory |
| 5 | $20 | Archive |

**Example**:
```spin2
if sd.attributes() & $10
  debug("This is a directory")
if sd.attributes() & $01
  debug("This is read-only")
```

---

### volumeLabel()

```spin2
PUB volumeLabel() : result
```

Get the volume label of the mounted card.

| Returns | Description |
|---------|-------------|
| pointer | Pointer to null-terminated volume label string (max 11 chars) |

---

### freeSpace()

```spin2
PUB freeSpace() : result
```

Calculate free space on the card.

| Returns | Description |
|---------|-------------|
| long | Free space in sectors (multiply by 512 for bytes) |

**Notes**:
- **Warning**: This scans the entire FAT table
- Can take several seconds on large cards
- Consider using FSInfo cached value for quick estimate

---

## Utility Methods

### setDate()

```spin2
PUB setDate(year, month, day, hour, minute, second)
```

Set the timestamp for newly created files and directories.

| Parameter | Type | Range |
|-----------|------|-------|
| `year` | long | 1980-2107 |
| `month` | long | 1-12 |
| `day` | long | 1-31 |
| `hour` | long | 0-23 |
| `minute` | long | 0-59 |
| `second` | long | 0-59 (2-second resolution) |

**Example**:
```spin2
sd.setDate(2026, 1, 16, 14, 30, 0)
sd.newFile(string("STAMPED.TXT"))  ' Will show 2026-01-16 2:30 PM
```

---

## Debug Methods

These methods are for development and troubleshooting:

### displaySector()

```spin2
PUB displaySector()
```

Display the current sector buffer contents in hex + ASCII format via DEBUG.

### displayEntry()

```spin2
PUB displayEntry()
```

Display the current 32-byte directory entry buffer via DEBUG.

### displayFAT()

```spin2
PUB displayFAT(cluster)
```

Read and display the FAT sector containing the specified cluster.

---

## Error Handling

The driver uses simple boolean returns rather than error codes:

| Situation | Return | Recovery |
|-----------|--------|----------|
| Card not present | `mount()` returns false | Check wiring, insert card |
| Not FAT32 | `mount()` returns false | Reformat card as FAT32 |
| File not found | `openFile()` returns false | Check filename/path |
| File exists | `newFile()` returns false | Delete first or use different name |
| Disk full | Write operations may fail | Delete files or use larger card |
| Write protected | Operations return false | Remove write protection |

---

## Thread Safety

**This driver is NOT thread-safe.**

- Only one cog should access the driver at a time
- If multiple cogs need SD access, implement a mutex or use a dedicated I/O cog

---

## Memory Usage

| Resource | Size |
|----------|------|
| VAR (instance data) | 628 bytes |
| Code | ~10.2 KB |
| Stack requirement | ~100 longs recommended |

---

*API Reference for P2-uSD-Study project*
