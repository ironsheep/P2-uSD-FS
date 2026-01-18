# Multi-Cog Interface Pattern for P2 Drivers

This document describes the multi-cog safe interface pattern used by the SD card driver. Other drivers (Flash, PSRAM) should follow this same pattern to enable future consolidation into unified multi-filesystem drivers.

---

## Overview

The pattern provides:
1. **Singleton worker cog** - One cog owns all hardware pins
2. **DAT-based shared state** - Singleton state visible to all object instances
3. **Lock-based serialization** - Hardware lock prevents concurrent API calls
4. **COGATN/WAITATN signaling** - Zero-cost completion notification
5. **Per-cog error storage** - Thread-safe error reporting

---

## Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                     SD_card_driver.spin2                             │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  DAT Block (singleton - shared across all object instances)          │
│  ┌────────────────────────────────────────────────────────────────┐ │
│  │ Singleton Control:                                              │ │
│  │   cog_id        LONG  -1    ' Worker cog ID (-1 = not started) │ │
│  │   api_lock      LONG  -1    ' Hardware lock ID                 │ │
│  │                                                                 │ │
│  │ Parameter Block (API ↔ worker communication):                   │ │
│  │   pb_cmd        LONG  0     ' Command code (0 = idle)          │ │
│  │   pb_status     LONG  0     ' Result status code               │ │
│  │   pb_caller     LONG  0     ' Caller's cog ID (for COGATN)     │ │
│  │   pb_param0-3   LONG  0     ' Input parameters                 │ │
│  │   pb_data0-1    LONG  0     ' Output data                      │ │
│  │                                                                 │ │
│  │ Worker Stack:                                                   │ │
│  │   cog_stack     LONG  0[128]                                   │ │
│  │                                                                 │ │
│  │ Hardware Config:                                                │ │
│  │   cs, mosi, miso, sck  LONG  0                                 │ │
│  │                                                                 │ │
│  │ Filesystem State (all driver state here):                       │ │
│  │   ...                                                           │ │
│  │                                                                 │ │
│  │ Per-Cog Error Storage:                                          │ │
│  │   last_error    LONG  0[8]  ' One slot per cog                 │ │
│  │                                                                 │ │
│  │ Buffers:                                                        │ │
│  │   buf           BYTE  0[512]                                   │ │
│  └────────────────────────────────────────────────────────────────┘ │
│                                                                      │
│  ┌──────────────────┐         ┌──────────────────────────────────┐ │
│  │  Public API       │         │  Worker Cog (fs_worker)          │ │
│  │  (any cog)        │         │  (dedicated cog, owns pins)      │ │
│  │                   │         │                                  │ │
│  │  start()    ──────┼────────►│  Launched by cogspin()           │ │
│  │  stop()           │         │  Initializes pins                │ │
│  │  mount()          │         │                                  │ │
│  │  openFile()  ─────┼──┐      │  Command loop:                   │ │
│  │  read()           │  │      │    repeat                        │ │
│  │  write()          │  │      │      wait for pb_cmd != 0        │ │
│  │  closeFile()      │  │      │      dispatch command            │ │
│  │  ...              │  │      │      pb_cmd := 0                 │ │
│  │                   │  │      │      COGATN(1 << pb_caller)      │ │
│  └──────────────────┘  │      └──────────────────────────────────┘ │
│                         │                                            │
│                         │      send_command() flow:                  │
│                         │      ──────────────────                    │
│                         └────► 1. locktry(api_lock)                  │
│                                2. pb_caller := COGID()               │
│                                3. pb_param0-3 := parameters          │
│                                4. pb_cmd := command                  │
│                                5. WAITATN()  ◄──────┐                │
│                                6. status := pb_status│                │
│                                7. lockrel(api_lock)  │                │
│                                                      │                │
│                                   COGATN signal ─────┘                │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Component Details

### 1. Singleton Control

```spin2
DAT
  cog_id        LONG    -1              ' Worker cog ID (-1 = not started)
  api_lock      LONG    -1              ' Hardware lock ID (-1 = not allocated)
```

- **cog_id**: Set to launched cog ID (0-7), or -1 if not running
- **api_lock**: Hardware lock index from `locknew()`, or -1 if not allocated
- Both initialized to -1 at compile time (not runtime)

### 2. Parameter Block

```spin2
DAT
  pb_cmd        LONG    0               ' Command (0 = idle/done)
  pb_status     LONG    0               ' Result status code
  pb_caller     LONG    0               ' Caller's cog ID (for COGATN signal)
  pb_param0     LONG    0               ' Parameter 0 (varies by command)
  pb_param1     LONG    0               ' Parameter 1
  pb_param2     LONG    0               ' Parameter 2
  pb_param3     LONG    0               ' Parameter 3
  pb_data0      LONG    0               ' Result data 0 (handle, count, etc.)
  pb_data1      LONG    0               ' Result data 1
```

**Command flow:**
1. Caller sets `pb_param0-3` with input parameters
2. Caller sets `pb_caller` to its own cog ID
3. Caller sets `pb_cmd` to command code (triggers worker)
4. Worker processes command, sets `pb_status` and `pb_data0-1`
5. Worker sets `pb_cmd := 0` and signals `COGATN(1 << pb_caller)`
6. Caller wakes from `WAITATN()`, reads results

### 3. Command Codes

```spin2
CON '' command codes for worker cog
  CMD_NONE      = 0         ' Idle / command complete
  CMD_MOUNT     = 1         ' Mount filesystem
  CMD_UNMOUNT   = 2         ' Unmount filesystem
  CMD_OPEN      = 3         ' Open file: param0=filename ptr
  CMD_CLOSE     = 4         ' Close file
  CMD_READ      = 5         ' Read: param0=buffer ptr, param1=count
  CMD_WRITE     = 6         ' Write: param0=buffer ptr, param1=count
  CMD_SEEK      = 7         ' Seek: param0=position
  CMD_NEWFILE   = 8         ' Create new file: param0=filename ptr
  CMD_NEWDIR    = 9         ' Create new directory: param0=dirname ptr
  CMD_DELETE    = 10        ' Delete file: param0=filename ptr
  CMD_RENAME    = 11        ' Rename: param0=oldname, param1=newname
  CMD_CHDIR     = 12        ' Change directory: param0=dirname ptr
  CMD_READDIR   = 13        ' Read directory entry: param0=index
  CMD_FILESIZE  = 14        ' Get file size
  CMD_FREESPACE = 15        ' Get free space
  CMD_SYNC      = 16        ' Flush buffers
  CMD_MOVEFILE  = 17        ' Move file: param0=name, param1=dest folder
```

### 4. Error Codes

```spin2
CON '' error codes (negative values)
  SUCCESS           = 0
  E_TIMEOUT         = -1        ' Card didn't respond in time
  E_NO_RESPONSE     = -2        ' Card not responding
  E_BAD_RESPONSE    = -3        ' Unexpected response from card
  E_CRC_ERROR       = -4        ' Data CRC mismatch
  E_WRITE_REJECTED  = -5        ' Card rejected write operation
  E_CARD_BUSY       = -6        ' Card busy
  E_NOT_MOUNTED     = -20       ' Filesystem not mounted
  E_INIT_FAILED     = -21       ' Card initialization failed
  E_NOT_FAT32       = -22       ' Card not formatted as FAT32
  E_BAD_SECTOR_SIZE = -23       ' Sector size not 512 bytes
  E_FILE_NOT_FOUND  = -40       ' File doesn't exist
  E_FILE_EXISTS     = -41       ' File already exists
  E_NOT_A_FILE      = -42       ' Expected file, found directory
  E_NOT_A_DIR       = -43       ' Expected directory, found file
  E_FILE_NOT_OPEN   = -45       ' File not open
  E_END_OF_FILE     = -46       ' Read past end of file
  E_DISK_FULL       = -60       ' No free clusters
  E_NO_LOCK         = -64       ' Couldn't allocate hardware lock
```

### 5. Per-Cog Error Storage

```spin2
DAT
  last_error    LONG    0[8]            ' One slot per possible cog

PUB error() : status
  '' Returns the error code from the most recent operation on this cog.
  '' Thread-safe: each cog has its own error slot.
  return LONG[@last_error][COGID()]

PRI set_error(code) : code_out
  '' Store error code in per-cog slot and return the code.
  LONG[@last_error][COGID()] := code
  return code
```

---

## Implementation

### start() - Launch Worker Cog

```spin2
PUB start(_cs, _mosi, _miso, _sck) : result
  '' Initialize driver and launch dedicated worker cog.
  '' Idempotent - calling multiple times is safe (returns existing cog ID).
  '' Returns cog ID (0-7) on success, -1 on failure.

  ' Check if already started (singleton pattern)
  if cog_id <> -1
    return cog_id

  ' Store pin configuration
  longmove(@cs, @_cs, 4)

  ' Allocate hardware lock for API serialization
  api_lock := locknew()
  if api_lock == -1
    return set_error(E_NO_LOCK)

  ' Initialize parameter block
  pb_cmd := CMD_NONE
  pb_status := SUCCESS

  ' Launch worker cog
  cog_id := cogspin(NEWCOG, fs_worker(), @cog_stack)
  if cog_id == -1
    lockret(api_lock)
    api_lock := -1
    return set_error(E_NO_LOCK)

  return cog_id
```

### stop() - Stop Worker Cog

```spin2
PUB stop()
  '' Cleanly shut down the worker cog and release resources.
  '' Safe to call even if not started.

  if cog_id <> -1
    ' Send unmount command if mounted
    if flags & F_MOUNTED
      send_command(CMD_UNMOUNT, 0, 0, 0, 0)

    ' Stop the worker cog
    cogstop(cog_id)
    cog_id := -1

  ' Release hardware lock
  if api_lock <> -1
    lockret(api_lock)
    api_lock := -1
```

### send_command() - API to Worker Communication

```spin2
PRI send_command(op_cmd, p0, p1, p2, p3) : status
  '' Acquire lock, send command to worker cog, wait for completion.
  '' Returns status code from worker.

  ' Verify worker cog is running
  if cog_id == -1
    return E_NOT_MOUNTED

  ' Acquire API lock (serialize multi-cog access)
  repeat until locktry(api_lock)

  ' Store caller ID and parameters
  pb_caller := COGID()
  pb_param0 := p0
  pb_param1 := p1
  pb_param2 := p2
  pb_param3 := p3

  ' Issue command (triggers worker)
  pb_cmd := op_cmd

  ' Wait for completion using WAITATN (efficient sleep)
  WAITATN()

  ' Get result
  status := pb_status

  ' Release lock
  lockrel(api_lock)

  return status
```

### fs_worker() - Worker Cog Main Loop

```spin2
PRI fs_worker() | cur_cmd
  '' This method runs in the dedicated worker cog.
  '' It owns the SPI pins and handles all filesystem operations.

  ' Initialize SPI pins (this cog owns them)
  pinh(cs)                    ' CS HIGH = deselected
  pinh(mosi)                  ' MOSI HIGH idle
  pinl(sck)                   ' SCK LOW (SPI mode 0)

  ' Main command loop
  repeat
    ' Wait for command
    repeat until (cur_cmd := pb_cmd) <> CMD_NONE

    ' Dispatch command
    case cur_cmd
      CMD_MOUNT:     pb_status := do_mount()
      CMD_UNMOUNT:   pb_status := do_unmount()
      CMD_OPEN:      pb_status := do_open(pb_param0)
      CMD_CLOSE:     do_close()
                     pb_status := SUCCESS
      CMD_READ:      pb_status := do_read(pb_param0, pb_param1)
      CMD_WRITE:     pb_status := do_write(pb_param0, pb_param1)
      CMD_SEEK:      pb_status := do_seek(pb_param0)
      CMD_NEWFILE:   pb_status := do_newfile(pb_param0)
      CMD_NEWDIR:    pb_status := do_newdir(pb_param0)
      CMD_DELETE:    pb_status := do_delete(pb_param0)
      CMD_CHDIR:     pb_status := do_chdir(pb_param0)
      CMD_FILESIZE:  pb_data0 := fileSize()
                     pb_status := SUCCESS
      CMD_FREESPACE: pb_data0 := do_freespace()
                     pb_status := SUCCESS
      CMD_SYNC:      do_sync()
                     pb_status := SUCCESS
      CMD_RENAME:    pb_status := do_rename(pb_param0, pb_param1)
      CMD_MOVEFILE:  pb_status := do_movefile(pb_param0, pb_param1)
      CMD_READDIR:   pb_status := do_readdir(pb_param0)
      other:         pb_status := E_BAD_RESPONSE

    ' Signal completion
    pb_cmd := CMD_NONE
    COGATN(1 << pb_caller)                ' Wake the caller
```

### Public API Methods

Each public API method follows this pattern:

```spin2
PUB openFile(name_ptr) : result
  '' Opens an existing file for reading/writing.
  '' Returns true on success, false if not found.
  result := send_command(CMD_OPEN, name_ptr, 0, 0, 0)
  if result == SUCCESS
    return true
  else
    set_error(result)
    return false

PUB read(p_buffer, count) : result
  '' Read bytes from file.
  '' Returns number of bytes read, or negative error code.
  result := send_command(CMD_READ, p_buffer, count, 0, 0)
  if result == SUCCESS
    return pb_data0           ' Actual bytes read
  else
    set_error(result)
    return result
```

---

## Key Points for Other Drivers

1. **All state in DAT block** - Not VAR, so it's shared across all object instances
2. **Initialize in start(), not DAT** - Only compile-time constants in DAT initialization
3. **One worker cog** - Launched with `cogspin()`, owns all hardware pins
4. **Hardware lock** - Allocated with `locknew()`, used by all API calls
5. **WAITATN/COGATN** - Zero-cost signaling, no polling
6. **Caller stores COGID()** - Worker uses this to signal the correct cog
7. **Negative error codes** - Positive values are success/data
8. **Per-cog error slots** - Thread-safe error() method

---

## Testing Multi-Cog Safety

The `SD_RT_multicog_tests.spin2` test file verifies:

1. **Singleton pattern** - Multiple `start()` calls return same cog ID
2. **Multi-cog singleton** - Worker cogs calling `start()` get same cog ID
3. **Concurrent access** - Lock prevents crashes when multiple cogs access driver
4. **Stress test** - Rapid operations from multiple cogs complete safely

**Note**: The current driver supports only one open file at a time. Concurrent open/read/close operations may interleave, causing some operations to fail. This is a design limitation, not a bug. The lock mechanism correctly serializes individual operations.

---

## Future Enhancements

1. **Handle table** - Support multiple simultaneous open files
2. **WAITATN in worker** - Currently polls; could use WAITATN for efficiency
3. **Batch operations** - Reduce lock overhead for sequential operations

---

*Document created: 2026-01-17*
*Based on: SD_card_driver.spin2*
*For use in: P2-FLASH-FileSystem, P2-FLASH-RAM-FileSystem driver transformations*
