# P2 SD Card Driver Tutorial

**A practical guide to FAT32 file operations on the Parallax Propeller 2**

This tutorial shows how to perform common filesystem operations using the `SD_card_driver.spin2` driver. If you're familiar with standard FAT32 APIs (FatFs, POSIX file I/O), this guide maps those concepts to our driver's interface.

> **Reference:** For background on FAT32 internals and standard API concepts, see [FAT32-API-Concepts-Reference.md](Reference/FAT32-API-Concepts-Reference.md)

---

## Table of Contents

1. [Quick Start](#quick-start)
2. [Mounting the Card](#mounting-the-card)
3. [Working with Directories](#working-with-directories)
4. [Searching for Files](#searching-for-files)
5. [Reading Files](#reading-files)
6. [Writing Files](#writing-files)
7. [Seeking and Random Access](#seeking-and-random-access)
8. [File Information](#file-information)
9. [Error Handling](#error-handling)
10. [Complete Examples](#complete-examples)

---

## Quick Start

```spin2
OBJ
  sd : "SD_card_driver"

CON
  ' Define your SD card pins
  SD_CS   = 61
  SD_MOSI = 59
  SD_MISO = 58
  SD_SCK  = 60

PUB main() | buf[128], bytes_read
  ' Mount the card
  if not sd.mount(SD_CS, SD_MOSI, SD_MISO, SD_SCK)
    debug("Mount failed!")
    return

  ' Open and read a file
  if sd.openFile(string("TEST.TXT"))
    bytes_read := sd.read(@buf, 512)
    sd.closeFile()
    debug("Read ", udec(bytes_read), " bytes")

  ' Clean shutdown
  sd.unmount()
```

---

## Mounting the Card

### Concept

Before any filesystem operations, you must mount the card. This initializes the SPI interface, reads the boot sector, and locates the FAT and root directory.

### Standard API Equivalent
```
mount(volume_id, callbacks, &fs_object)
```

### Our Driver

```spin2
PUB mount(_cs, _mosi, _miso, _sck) : result
```

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `_cs` | pin number | Chip Select (directly wired to SD card CS) |
| `_mosi` | pin number | Master Out, Slave In (data to card) |
| `_miso` | pin number | Master In, Slave Out (data from card) |
| `_sck` | pin number | Serial Clock |

**Returns:** `true` on success, `false` on failure

**Example:**
```spin2
CON
  SD_CS   = 61
  SD_MOSI = 59
  SD_MISO = 58
  SD_SCK  = 60

PUB setup()
  if sd.mount(SD_CS, SD_MOSI, SD_MISO, SD_SCK)
    debug("Card mounted successfully")
    debug("Volume: ", zstr(sd.volumeLabel()))
  else
    debug("Mount failed, error: ", sdec(sd.error()))
```

### Unmounting

Always unmount cleanly to ensure data integrity:

```spin2
sd.unmount()
```

This flushes any pending writes and updates the FSInfo sector with the correct free cluster count.

---

## Working with Directories

### Concept

The driver maintains a "current directory" that affects all file operations. You navigate the directory tree by changing directories, and can use absolute paths (starting with `/`) or relative paths.

### Changing the Current Directory

**Standard API Equivalent:**
```
chdir(path)
```

**Our Driver:**
```spin2
PUB changeDirectory(name_ptr) : result
```

**Examples:**
```spin2
' Navigate to a subdirectory
sd.changeDirectory(string("LOGS"))

' Navigate to root
sd.changeDirectory(string("/"))

' Navigate using absolute path
sd.changeDirectory(string("/DATA/2026/JAN"))

' Navigate up one level (to parent)
sd.changeDirectory(string(".."))
```

### Creating a New Directory

**Standard API Equivalent:**
```
mkdir(path)
```

**Our Driver:**
```spin2
PUB newDirectory(name_ptr) : result
```

**Example:**
```spin2
' Create a new directory in current location
if sd.newDirectory(string("BACKUP"))
  debug("Directory created")
else
  debug("Failed - may already exist")
```

### Enumerating Directory Contents

**Standard API Equivalent:**
```
opendir(path, &dir)
readdir(&dir, &info)
```

**Our Driver:**
```spin2
PUB readDirectory(entry) : result
```

This function iterates through entries in the current directory. Call it with entry index 0, 1, 2, etc. to get successive entries. Returns a pointer to the internal entry buffer, or 0 when no more entries exist.

**Getting Entry Information:**

After `readDirectory()` returns successfully, use these methods:

| Method | Returns | Description |
|--------|---------|-------------|
| `fileName()` | string pointer | 8.3 formatted filename |
| `fileSize()` | long | Size in bytes (0 for directories) |
| `attributes()` | byte | Attribute flags (see below) |

**Attribute Flags:**
| Value | Meaning |
|-------|---------|
| `$01` | Read-only |
| `$02` | Hidden |
| `$04` | System |
| `$08` | Volume label |
| `$10` | Directory |
| `$20` | Archive |

**Example - List All Files in Current Directory:**
```spin2
PUB listDirectory() | entry_num, p_entry
  entry_num := 0
  repeat
    p_entry := sd.readDirectory(entry_num)
    if p_entry == 0
      quit                                    ' No more entries

    ' Check if it's a directory or file
    if sd.attributes() & $10
      debug("[DIR]  ", zstr(sd.fileName()))
    else
      debug("[FILE] ", zstr(sd.fileName()), " (", udec(sd.fileSize()), " bytes)")

    entry_num++
```

---

## Searching for Files

### Concept

Unlike some APIs that provide a separate search function, our driver combines searching with opening. When you call `openFile()` with a path, the driver walks the directory tree to find the file.

### Search for a File in Current Directory

```spin2
if sd.openFile(string("CONFIG.TXT"))
  ' File found and opened
  sd.closeFile()
else
  debug("File not found")
```

### Search Using Full Path

The driver supports absolute paths starting with `/`:

```spin2
' Search from root, regardless of current directory
if sd.openFile(string("/SETTINGS/USER.CFG"))
  ' Found it
  sd.closeFile()
```

### Check if Directory Exists

```spin2
if sd.changeDirectory(string("LOGS"))
  debug("LOGS directory exists")
  sd.changeDirectory(string(".."))          ' Go back up
else
  debug("LOGS directory not found")
```

### Recursive Directory Search (Manual Implementation)

The driver doesn't have built-in recursive search, but you can implement it:

```spin2
VAR
  byte search_name[13]
  byte found_path[64]

PUB findFileRecursive(name, current_path) : found | entry_num, p_entry, attr
  ' Save the search name
  bytemove(@search_name, name, strsize(name) + 1)

  entry_num := 0
  repeat
    p_entry := sd.readDirectory(entry_num)
    if p_entry == 0
      return false

    attr := sd.attributes()

    ' Check for match (case-insensitive handled by driver)
    if strcomp(sd.fileName(), @search_name)
      ' Found it!
      return true

    ' If it's a directory (not . or ..), recurse into it
    if (attr & $10) and (byte[sd.fileName()] <> ".")
      if sd.changeDirectory(sd.fileName())
        if findFileRecursive(@search_name, 0)
          return true
        sd.changeDirectory(string(".."))

    entry_num++

  return false
```

---

## Reading Files

### Concept

File reading is byte-oriented. You provide a buffer and a count, and the driver returns however many bytes were actually read. The driver handles all the cluster chain following, sector boundary crossing, and buffering internally.

### Opening a File for Reading

```spin2
PUB openFile(name_ptr) : result
```

Returns `true` if the file was found and opened, `false` otherwise.

### Reading Data

**Standard API Equivalent:**
```
read(file_handle, buffer, count, &bytes_read)
```

**Our Driver:**
```spin2
PUB read(p_buffer, count) : bytes_read
```

**Parameters:**
| Parameter | Description |
|-----------|-------------|
| `p_buffer` | Pointer to your receive buffer |
| `count` | Maximum bytes to read |

**Returns:** Actual number of bytes read (may be less than requested at end of file)

### Reading Examples

**Read Entire Small File:**
```spin2
VAR
  byte buffer[512]

PUB readSmallFile()
  if sd.openFile(string("MESSAGE.TXT"))
    sd.read(@buffer, sd.fileSize())
    buffer[sd.fileSize()] := 0              ' Null-terminate for string use
    debug(zstr(@buffer))
    sd.closeFile()
```

**Read Large File in Chunks:**
```spin2
CON
  CHUNK_SIZE = 512

VAR
  byte chunk[CHUNK_SIZE]

PUB readLargeFile() | total_read, bytes_this_read
  if not sd.openFile(string("BIGFILE.DAT"))
    return

  total_read := 0
  repeat
    bytes_this_read := sd.read(@chunk, CHUNK_SIZE)
    if bytes_this_read == 0
      quit                                  ' End of file

    ' Process chunk here...
    processData(@chunk, bytes_this_read)

    total_read += bytes_this_read

  debug("Total bytes read: ", udec(total_read))
  sd.closeFile()
```

**Read Single Byte at Specific Position:**
```spin2
PUB readByteAt(position) : value
  value := sd.readByte(position)
```

Note: `readByte()` combines seek and read internally.

---

## Writing Files

### Concept

Writing works similarly to reading. For new files, use `newFile()` which creates the file and leaves it open for writing. For existing files, open them, seek to the desired position, and write.

### Creating a New File

**Standard API Equivalent:**
```
open(path, O_CREAT | O_WRONLY)
```

**Our Driver:**
```spin2
PUB newFile(name_ptr) : result
```

Creates a new file and opens it for writing. Returns `false` if file already exists.

### Writing Data

**Standard API Equivalent:**
```
write(file_handle, buffer, count, &bytes_written)
```

**Our Driver:**
```spin2
PUB write(p_buffer, count) : result
```

**Parameters:**
| Parameter | Description |
|-----------|-------------|
| `p_buffer` | Pointer to data to write |
| `count` | Number of bytes to write |

### Writing Examples

**Create and Write a Small File:**
```spin2
PUB createTextFile()
  if sd.newFile(string("HELLO.TXT"))
    sd.writeString(string("Hello, World!"))
    sd.closeFile()
    debug("File created successfully")
  else
    debug("Could not create file (may already exist)")
```

**Write Binary Data:**
```spin2
VAR
  long sensor_data[100]

PUB saveSensorData()
  if sd.newFile(string("SENSOR.DAT"))
    sd.write(@sensor_data, 100 * 4)         ' 100 longs = 400 bytes
    sd.closeFile()
```

**Append to Existing File:**
```spin2
PUB appendToLog(message)
  if sd.openFile(string("LOG.TXT"))
    sd.seek(sd.fileSize())                  ' Move to end
    sd.writeString(message)
    sd.writeString(string(13, 10))          ' CR+LF
    sd.closeFile()
```

**Write Large Data in Chunks:**
```spin2
PUB writeLargeData(p_data, total_bytes) | offset, chunk_size
  if not sd.newFile(string("DUMP.BIN"))
    return false

  offset := 0
  repeat while offset < total_bytes
    chunk_size := (total_bytes - offset) <# 512
    sd.write(p_data + offset, chunk_size)
    offset += chunk_size

  sd.closeFile()
  return true
```

### Flushing Data Without Closing

Use `sync()` to flush pending writes without closing the file:

```spin2
PUB longRunningWrite() | i
  if sd.newFile(string("DATA.LOG"))
    repeat i from 0 to 999
      sd.writeString(string("Data point "))
      ' ... write data ...

      ' Checkpoint every 100 entries
      if i // 100 == 99
        sd.sync()

    sd.closeFile()
```

---

## Seeking and Random Access

### Concept

The driver maintains a file position that advances automatically with each read/write. You can change this position with `seek()`.

### Seek to Position

**Standard API Equivalent:**
```
lseek(fd, offset, SEEK_SET)
```

**Our Driver:**
```spin2
PUB seek(pos) : result
```

**Parameters:**
| Parameter | Description |
|-----------|-------------|
| `pos` | Byte offset from start of file |

**Returns:** `true` on success, `false` if position is beyond end of file

### Seeking Examples

**Read from Middle of File:**
```spin2
VAR
  byte header[64]

PUB readFileHeader()
  if sd.openFile(string("DATA.BIN"))
    sd.seek(256)                            ' Skip first 256 bytes
    sd.read(@header, 64)                    ' Read 64-byte header
    sd.closeFile()
```

**Random Access Read:**
```spin2
PUB readRecordAt(record_num) | record[16]
  ' Assuming 64-byte records
  if sd.openFile(string("DATABASE.DAT"))
    sd.seek(record_num * 64)
    sd.read(@record, 64)
    sd.closeFile()
  return @record
```

**Modify Data in Middle of File:**
```spin2
PUB updateRecord(record_num, p_new_data)
  if sd.openFile(string("DATABASE.DAT"))
    sd.seek(record_num * 64)
    sd.write(p_new_data, 64)
    sd.closeFile()
```

---

## File Information

### Getting File Size

```spin2
PUB fileSize() : result
```

Returns the size of the currently open file in bytes.

```spin2
if sd.openFile(string("IMAGE.BMP"))
  debug("File size: ", udec(sd.fileSize()), " bytes")
  sd.closeFile()
```

### Getting File Name

```spin2
PUB fileName() : result
```

Returns pointer to 8.3 formatted filename of currently open file.

### Getting Attributes

```spin2
PUB attributes() : result
```

Returns attribute byte (see attribute flags table above).

### Volume Information

```spin2
' Get volume label
debug("Volume: ", zstr(sd.volumeLabel()))

' Get free space (returns sectors, not bytes)
debug("Free sectors: ", udec(sd.freeSpace()))
```

### Setting Timestamps

Set the date/time that will be applied to new files:

```spin2
PUB setDate(year, month, day, hour, minute, second)
```

```spin2
' Set timestamp before creating files
sd.setDate(2026, 1, 26, 14, 30, 0)
sd.newFile(string("TIMESTAMPED.TXT"))
sd.closeFile()
```

---

## Error Handling

### Checking for Errors

```spin2
PUB error() : status
```

Returns the error code from the most recent operation. Thread-safe (each cog has its own error slot).

### Error Codes

| Code | Constant | Description |
|------|----------|-------------|
| 0 | `SUCCESS` | No error |
| -1 | `E_TIMEOUT` | Card didn't respond in time |
| -2 | `E_NO_RESPONSE` | Card not responding |
| -3 | `E_BAD_RESPONSE` | Unexpected response from card |
| -4 | `E_CRC_ERROR` | Data CRC mismatch |
| -5 | `E_WRITE_REJECTED` | Card rejected write operation |
| -6 | `E_CARD_BUSY` | Card busy |
| -20 | `E_NOT_MOUNTED` | Filesystem not mounted |
| -21 | `E_INIT_FAILED` | Card initialization failed |
| -22 | `E_NOT_FAT32` | Card not formatted as FAT32 |
| -23 | `E_BAD_SECTOR_SIZE` | Sector size not 512 bytes |
| -40 | `E_FILE_NOT_FOUND` | File doesn't exist |
| -41 | `E_FILE_EXISTS` | File already exists |
| -42 | `E_NOT_A_FILE` | Expected file, found directory |
| -43 | `E_NOT_A_DIR` | Expected directory, found file |
| -45 | `E_FILE_NOT_OPEN` | File not open |
| -46 | `E_END_OF_FILE` | Read past end of file |
| -60 | `E_DISK_FULL` | No free clusters |
| -64 | `E_NO_LOCK` | Couldn't allocate hardware lock |

### Error Handling Pattern

```spin2
PUB safeFileOperation()
  if not sd.mount(SD_CS, SD_MOSI, SD_MISO, SD_SCK)
    case sd.error()
      sd.E_TIMEOUT:
        debug("Card not inserted or not responding")
      sd.E_NOT_FAT32:
        debug("Card not formatted as FAT32")
      other:
        debug("Mount failed, error: ", sdec(sd.error()))
    return

  if not sd.openFile(string("DATA.TXT"))
    if sd.error() == sd.E_FILE_NOT_FOUND
      ' Create the file
      sd.newFile(string("DATA.TXT"))

  ' ... continue operations ...

  sd.closeFile()
  sd.unmount()
```

---

## Complete Examples

### Example 1: Configuration File Reader

```spin2
CON
  SD_CS = 61, SD_MOSI = 59, SD_MISO = 58, SD_SCK = 60
  MAX_LINE = 80

OBJ
  sd : "SD_card_driver"

VAR
  byte line_buffer[MAX_LINE]

PUB readConfig() | bytes, i, ch
  if not sd.mount(SD_CS, SD_MOSI, SD_MISO, SD_SCK)
    return false

  if not sd.openFile(string("CONFIG.INI"))
    sd.unmount()
    return false

  ' Read file line by line
  repeat
    i := 0
    repeat
      if sd.read(@ch, 1) == 0               ' End of file
        quit
      if ch == 10                           ' Newline
        quit
      if ch <> 13 and i < MAX_LINE - 1      ' Skip CR, check buffer
        line_buffer[i++] := ch

    line_buffer[i] := 0                     ' Null terminate

    if i > 0
      processConfigLine(@line_buffer)

    if ch == 0                              ' EOF reached
      quit

  sd.closeFile()
  sd.unmount()
  return true
```

### Example 2: Data Logger

```spin2
CON
  SD_CS = 61, SD_MOSI = 59, SD_MISO = 58, SD_SCK = 60

OBJ
  sd : "SD_card_driver"

VAR
  byte log_name[13]
  long log_count

PUB startLogging()
  if not sd.mount(SD_CS, SD_MOSI, SD_MISO, SD_SCK)
    return false

  ' Set timestamp
  sd.setDate(2026, 1, 26, 12, 0, 0)

  ' Create logs directory if needed
  if not sd.changeDirectory(string("LOGS"))
    sd.newDirectory(string("LOGS"))
    sd.changeDirectory(string("LOGS"))

  ' Find next available log number
  log_count := 0
  repeat
    formatLogName(log_count)
    if not sd.openFile(@log_name)
      quit                                  ' Found unused name
    sd.closeFile()
    log_count++

  ' Create the new log file
  sd.newFile(@log_name)
  sd.writeString(string("=== Log Started ===", 13, 10))

  return true

PUB logEntry(message)
  sd.writeString(message)
  sd.writeString(string(13, 10))
  sd.sync()                                 ' Ensure data is saved

PUB stopLogging()
  sd.writeString(string("=== Log Ended ===", 13, 10))
  sd.closeFile()
  sd.changeDirectory(string("/"))
  sd.unmount()

PRI formatLogName(num) | tens, ones
  ' Format as "LOG00.TXT" through "LOG99.TXT"
  tens := num / 10
  ones := num // 10
  bytemove(@log_name, string("LOG00.TXT"), 10)
  log_name[3] := "0" + tens
  log_name[4] := "0" + ones
```

### Example 3: Binary File Handler

```spin2
CON
  SD_CS = 61, SD_MOSI = 59, SD_MISO = 58, SD_SCK = 60
  RECORD_SIZE = 32

OBJ
  sd : "SD_card_driver"

VAR
  byte record[RECORD_SIZE]

PUB readRecord(filename, record_num, p_dest) : success
  if not sd.mount(SD_CS, SD_MOSI, SD_MISO, SD_SCK)
    return false

  if not sd.openFile(filename)
    sd.unmount()
    return false

  ' Check if record exists
  if (record_num + 1) * RECORD_SIZE > sd.fileSize()
    sd.closeFile()
    sd.unmount()
    return false

  ' Seek to record position and read
  sd.seek(record_num * RECORD_SIZE)
  sd.read(p_dest, RECORD_SIZE)

  sd.closeFile()
  sd.unmount()
  return true

PUB writeRecord(filename, record_num, p_src) : success
  if not sd.mount(SD_CS, SD_MOSI, SD_MISO, SD_SCK)
    return false

  ' Open existing or create new
  if not sd.openFile(filename)
    if not sd.newFile(filename)
      sd.unmount()
      return false

  ' Seek to record position and write
  sd.seek(record_num * RECORD_SIZE)
  sd.write(p_src, RECORD_SIZE)

  sd.closeFile()
  sd.unmount()
  return true

PUB appendRecord(filename, p_src) : success
  if not sd.mount(SD_CS, SD_MOSI, SD_MISO, SD_SCK)
    return false

  ' Open existing or create new
  if sd.openFile(filename)
    sd.seek(sd.fileSize())                  ' Go to end
  else
    if not sd.newFile(filename)
      sd.unmount()
      return false

  sd.write(p_src, RECORD_SIZE)

  sd.closeFile()
  sd.unmount()
  return true
```

---

## API Quick Reference

### Lifecycle
| Method | Description |
|--------|-------------|
| `mount(cs, mosi, miso, sck)` | Mount card, returns true/false |
| `unmount()` | Unmount card cleanly |
| `sync()` | Flush pending writes |

### Directories
| Method | Description |
|--------|-------------|
| `changeDirectory(name)` | Change current directory |
| `newDirectory(name)` | Create new directory |
| `readDirectory(index)` | Get entry at index |

### Files
| Method | Description |
|--------|-------------|
| `openFile(name)` | Open existing file |
| `newFile(name)` | Create and open new file |
| `closeFile()` | Close current file |
| `deleteFile(name)` | Delete file |
| `rename(old, new)` | Rename file/directory |
| `moveFile(name, dest)` | Move file to directory |

### Read/Write
| Method | Description |
|--------|-------------|
| `read(buffer, count)` | Read bytes, returns count read |
| `write(buffer, count)` | Write bytes |
| `readByte(address)` | Read single byte at position |
| `writeByte(char)` | Write single byte |
| `writeString(str)` | Write null-terminated string |
| `seek(pos)` | Set file position |

### Information
| Method | Description |
|--------|-------------|
| `fileSize()` | Size of open file |
| `fileName()` | Name of open file |
| `attributes()` | Attributes of open file |
| `volumeLabel()` | Card volume label |
| `freeSpace()` | Free sectors on card |
| `error()` | Last error code |

### Timestamps
| Method | Description |
|--------|-------------|
| `setDate(y,m,d,h,mi,s)` | Set date for new files |

---

## What The Driver Handles For You

The driver abstracts away all FAT32 complexity:

- **Cluster chains:** Following and allocating clusters automatically
- **FAT updates:** Maintaining both FAT copies
- **Sector buffering:** Managing the 512-byte sector buffer
- **Directory parsing:** Converting 8.3 names and navigating entries
- **Path resolution:** Walking the directory tree for absolute paths
- **Multi-cog safety:** Serializing access through a worker cog

You just work with files and bytes; the driver handles the rest.
