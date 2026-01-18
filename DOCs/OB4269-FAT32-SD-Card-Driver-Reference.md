# OB4269 - FAT32 SD Card Driver Technical Reference

**OBEX Object**: 4269
**Title**: FAT32 SD card driver
**Author**: Chris Gadd
**Copyright**: (c) 2023 Chris Gadd
**License**: MIT
**Language**: Spin2/PASM2

---

## Overview

A complete FAT32 filesystem driver for SD/SDHC cards on the Parallax Propeller 2. This is a single-cog, bit-banged SPI implementation with inline PASM2 for high-speed sector transfers.

### Verified Hardware Compatibility

- Panasonic 512MB SD
- PNY 8GB SDHC
- SanDisk Extreme 32GB SDHC
- GSKILL 32GB Micro SDHC

**Requirement**: Cards must be formatted as FAT32.

---

## Hardware Interface

### Communication Protocol

- **Interface**: SPI (Serial Peripheral Interface)
- **Mode**: SPI Mode 0 (CPOL=0, CPHA=0)
- **Implementation**: Bit-banged with inline PASM2 for sector transfers

### Pin Connections

| Pin | Function | Direction |
|-----|----------|-----------|
| CS | Chip Select | Output (active low) |
| MOSI | Master Out Slave In | Output |
| MISO | Master In Slave Out | Input |
| SCK | Serial Clock | Output |

### Timing

- **Initialization clock**: 400 Kbps (SD card spec requirement)
- **Operational clock**: Maximum speed (bit_delay = 2 cycles)

### Typical Wiring (3.3V)

```
P2 Pin --> CS
P2 Pin --> MOSI
P2 Pin <-- MISO
P2 Pin --> SCK
GND    --> GND
3V3    --> Vcc
```

---

## Capabilities Summary

| Feature | Supported |
|---------|-----------|
| FAT32 filesystem | Yes |
| SD cards (up to 2GB) | Yes |
| SDHC cards (up to 32GB) | Yes |
| SDXC cards | No (FAT32 only) |
| Long filenames (LFN) | No (8.3 only) |
| Subdirectories | Yes |
| File read | Yes |
| File write | Yes |
| File append | Yes |
| Seek within file | Yes |
| File delete | Yes |
| File rename | Yes |
| File move | Yes |
| Date/time stamps | Yes |
| Free space query | Yes |
| Multi-cog access | No |

---

## Storage Limits

### Card Type Support

| Card Type | Capacity Range | Addressing Mode | Supported |
|-----------|---------------|-----------------|-----------|
| SD (SDSC) | Up to 2 GB | Byte addressing | Yes |
| SDHC | 4 GB - 32 GB | Block addressing | Yes |
| SDXC | 64 GB - 2 TB | Block addressing | No* |

*SDXC cards are formatted as exFAT by default. They can work if reformatted to FAT32, but this is not officially supported and may have compatibility issues.

### Addressing Modes

The driver automatically detects card type during initialization:

- **SD cards (SDSC)**: Use byte addressing (`hcs = 9`). Sector addresses are multiplied by 512.
- **SDHC cards**: Use block addressing (`hcs = 0`). Sector addresses are used directly.

```spin2
' From mount() - card type detection:
if (result & $40) == $40
    hcs := 0   ' SDHC: block addressing (no shift)
else
    hcs := 9   ' SD: byte addressing (shift by 9 = multiply by 512)
```

### FAT32 Filesystem Limits

| Limit | Value | Notes |
|-------|-------|-------|
| Maximum file size | 4 GB - 1 byte | 0xFFFFFFFF bytes (FAT32 specification limit) |
| Maximum volume size | 2 TB | Theoretical; practical limit ~32 GB for SDHC |
| Cluster size | 512 B - 32 KB | Determined by card format |
| Maximum clusters | 268,435,445 | FAT32 cluster count limit |
| Filename length | 8.3 format | 8 characters + 3 extension |

### Practical Recommendations

| Card Size | Recommended | Notes |
|-----------|-------------|-------|
| 512 MB - 2 GB | SD | Good for small data logging |
| 4 GB - 32 GB | SDHC | Optimal balance of capacity and compatibility |
| > 32 GB | Not recommended | Requires FAT32 reformat, slow FAT scanning |

### Performance Considerations

- **freeSpace()**: Scans entire FAT table. On a 32 GB card with 4 KB clusters, this is ~8 million entries.
- **Large files**: Files approaching 4 GB limit require many cluster allocations.
- **Fragmentation**: No defragmentation support; heavy write/delete cycles reduce performance.

---

## API Reference

### Management Methods

#### `mount(CS, MOSI, MISO, SCK) : result`

Initialize and mount an SD card.

**Parameters:**
- `CS` - Pin number for Chip Select
- `MOSI` - Pin number for Master Out Slave In
- `MISO` - Pin number for Master In Slave Out
- `SCK` - Pin number for Serial Clock

**Returns:** `true` if mount successful, `false` otherwise

**Example:**
```spin2
if sd.mount(0, 1, 2, 3)
    ' Card mounted successfully
```

#### `setDate(year, month, day, hour, minute, second)`

Set the timestamp to be applied to newly created files and directories.

**Parameters:**
- `year` - Full year (e.g., 2024)
- `month` - Month (1-12)
- `day` - Day (1-31)
- `hour` - Hour (0-23)
- `minute` - Minute (0-59)
- `second` - Second (0-59, stored with 2-second resolution)

**Default:** January 27, 2009 at 07:00 AM

---

### File Operations

#### `newFile(name_ptr) : result`

Create a new file in the current directory and open it for writing.

**Parameters:**
- `name_ptr` - Pointer to null-terminated filename string

**Returns:** `true` if created, `false` if file already exists

**Notes:** File remains open after creation. Call `closeFile()` when done.

#### `openFile(name_ptr) : result`

Open an existing file in the current directory (or by path).

**Parameters:**
- `name_ptr` - Pointer to filename or path string

**Returns:** `true` if opened, `false` if not found

**Path support:**
- `string("fileA.txt")` - File in current directory
- `string("/folder1/fileA.txt")` - Absolute path (leading `/` resets to root)

#### `closeFile()`

Close the currently open file and write any pending data to the card.

**Important:** Always call this after writing to ensure data integrity.

#### `deleteFile(name_ptr) : result`

Delete a file from the current directory.

**Parameters:**
- `name_ptr` - Pointer to filename string

**Returns:** `true` if deleted, `false` if not found or protected

**Restrictions:** Only unprotected files can be deleted (archive attribute only).

#### `rename(old_name, new_name) : result`

Rename a file or directory.

**Parameters:**
- `old_name` - Pointer to current name (can include path)
- `new_name` - Pointer to new name

**Returns:** `true` if renamed, `false` if old name not found or new name exists

#### `moveFile(name_ptr, dest_folder) : result`

Move a file from one directory to another.

**Parameters:**
- `name_ptr` - Pointer to source filename/path
- `dest_folder` - Pointer to destination folder path

**Returns:** `true` if moved, `false` on error

---

### Directory Operations

#### `newDirectory(name_ptr) : result`

Create a new subdirectory in the current directory.

**Parameters:**
- `name_ptr` - Pointer to directory name string

**Returns:** `true` if created, `false` if already exists

**Notes:** Automatically creates `.` and `..` entries.

#### `changeDirectory(name_ptr) : result`

Change the current working directory.

**Parameters:**
- `name_ptr` - Pointer to directory name or path

**Returns:** `true` if changed, `false` if not found

**Special paths:**
- `string("..")` - Parent directory
- `string("/folder")` - Absolute path from root

#### `readDirectory(entry) : result`

Iterate through entries in the current directory.

**Parameters:**
- `entry` - Entry number (0-based)

**Returns:** Pointer to entry buffer, or `false` (0) if entry doesn't exist

**Usage pattern:**
```spin2
i := 0
repeat until sd.readDirectory(i++) == 0
    debug(zstr(sd.fileName()))
```

---

### Read/Write Operations

#### `read(p_buffer, count) : result`

Read bytes from the open file into a buffer.

**Parameters:**
- `p_buffer` - Pointer to destination buffer
- `count` - Number of bytes to read

**Returns:** Number of bytes actually read

**Notes:** Automatically limited to bytes remaining in file.

#### `readByte(address) : result`

Read a single byte from a specific position in the open file.

**Parameters:**
- `address` - Byte offset within file

**Returns:** Byte value at that position

#### `write(p_buffer, count) : result`

Write bytes from a buffer to the open file.

**Parameters:**
- `p_buffer` - Pointer to source buffer
- `count` - Number of bytes to write

**Notes:** Automatically allocates clusters as needed.

#### `writeByte(char) : result`

Write a single byte to the open file.

**Parameters:**
- `char` - Byte value to write

#### `writeString(p_str) : result`

Write a null-terminated string to the open file.

**Parameters:**
- `p_str` - Pointer to null-terminated string

---

### Navigation

#### `seek(pos) : result`

Set the read/write position within the open file.

**Parameters:**
- `pos` - Byte offset from start of file

**Returns:** `true` if successful, `false` if position beyond EOF

---

### Information Methods

#### `fileSize() : result`

Get the size of the currently open file.

**Returns:** File size in bytes

#### `fileName() : result`

Get the 8.3 filename of the currently open file.

**Returns:** Pointer to filename string (e.g., "FILENAME.TXT")

#### `attributes() : result`

Get the attribute byte of the currently open file.

**Returns:** Attribute byte with flags:

| Bit | Attribute |
|-----|-----------|
| 0 | Read-only |
| 1 | Hidden |
| 2 | System |
| 3 | Volume ID |
| 4 | Directory |
| 5 | Archive |

#### `freeSpace() : result`

Calculate the number of free sectors on the card.

**Returns:** Number of free 512-byte sectors

**Note:** This operation scans the entire FAT and may take significant time on large cards.

---

### Debug Methods

#### `displaySector()`

Output the current sector buffer contents to debug terminal in hex/ASCII format.

#### `displayEntry()`

Output the current directory entry buffer to debug terminal.

#### `displayFAT(cluster)`

Read and display the FAT sector containing a specific cluster number.

---

## Internal Architecture

### Memory Layout (VAR block)

| Variable | Size | Purpose |
|----------|------|---------|
| cs, mosi, miso, sck | 4 longs | SPI pin numbers |
| fat_sec, fat2_sec | 2 longs | FAT sector addresses |
| sec_per_fat | 1 long | Sectors per FAT |
| sec_per_clus | 1 long | Sectors per cluster |
| root_sec | 1 long | Root directory sector |
| cluster_offset | 1 long | Cluster alignment offset |
| dir_sec | 1 long | Current directory sector |
| entry_address | 1 long | Current entry byte address |
| date_stamp | 1 long | Timestamp for new files |
| n_sec | 1 long | Current sector number |
| file_idx | 1 long | Position within file (0 to filesize) |
| flags | 1 long | Open/new file/new data flags |
| sec_in_buf | 1 long | Cached sector number |
| bit_delay, hcs | 2 longs | SPI timing and card type |
| buf | 512 bytes | Main data buffer |
| entry_buffer | 32 bytes | Directory entry buffer |

### Flags

| Flag | Value | Meaning |
|------|-------|---------|
| F_OPEN | bit 0 | File is open |
| F_NEWDIR | bit 1 | New directory entry pending |
| F_NEWDATA | bit 2 | New data written, needs flush |

### SPI Implementation Details

The driver uses inline PASM2 for high-speed sector transfers:

- **readSector**: Uses `wfast`/`rdfast` streaming with REP loops
- **writeSector**: Uses `rdfast` streaming with REP loops
- **transfer**: General-purpose bit transfer with configurable delay

The sector read/write routines achieve maximum throughput by:
1. Using FIFO-based hub access (`wrfast`/`rdfast`/`wflong`/`rflong`)
2. REP-based tight loops for 32-bit transfers
3. Minimal instruction overhead in critical paths

---

## Usage Example

```spin2
OBJ
    sd : "SD card driver"

VAR
    byte buffer[512]

PUB main() | status

    ' Mount the SD card
    if sd.mount(CS_PIN, MOSI_PIN, MISO_PIN, SCK_PIN)

        ' Set current date/time
        sd.setDate(2024, 1, 15, 14, 30, 0)

        ' Create a directory
        sd.newDirectory(string("mydata"))
        sd.changeDirectory(string("mydata"))

        ' Create and write a file
        sd.newFile(string("log.txt"))
        sd.writeString(string("Data logging started"))
        sd.writeByte(13)  ' CR
        sd.writeByte(10)  ' LF
        sd.closeFile()

        ' Read the file back
        sd.openFile(string("log.txt"))
        sd.read(@buffer, sd.fileSize())
        sd.closeFile()

        debug(zstr(@buffer))
```

---

## Limitations

1. **8.3 filenames only** - No long filename (LFN) support
2. **Single file open** - Only one file can be open at a time
3. **No multi-cog** - Not thread-safe for concurrent access
4. **FAT32 only** - No FAT16/FAT12/exFAT support
5. **No error codes** - Functions return true/false without detailed error info
6. **Blocking I/O** - All operations are synchronous

---

## FAT32 Reference (from source comments)

### Master Boot Record (Sector 0)

- Offset $1C2: Partition type ($0B or $0C = FAT32)
- Offset $1C6: Volume Boot Record sector address

### Volume Boot Record

| Offset | Size | Field |
|--------|------|-------|
| $0B | 16-bit | Bytes per sector (512) |
| $0D | 8-bit | Sectors per cluster |
| $0E | 16-bit | Reserved sectors |
| $10 | 8-bit | Number of FATs (2) |
| $24 | 32-bit | Sectors per FAT |
| $2C | 32-bit | Root directory first cluster |

### Directory Entry (32 bytes)

| Offset | Size | Field |
|--------|------|-------|
| $00 | 11 bytes | Short filename (8.3) |
| $0B | 1 byte | Attributes |
| $0E | 4 bytes | Creation date/time |
| $14 | 2 bytes | First cluster high |
| $16 | 4 bytes | Modification date/time |
| $1A | 2 bytes | First cluster low |
| $1C | 4 bytes | File size |
