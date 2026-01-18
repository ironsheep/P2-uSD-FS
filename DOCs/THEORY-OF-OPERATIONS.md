# SD Card Driver - Theory of Operations

## Overview

This is an **SPI-mode only** SD card driver for the Parallax Propeller 2. The driver uses a dedicated worker cog to handle all SD card I/O, providing thread-safe access from multiple application cogs.

### Key Characteristics

- **Protocol**: SPI Mode (not native SD mode)
- **Speed Modes**: Default Speed (25 MHz), High Speed (50 MHz via CMD6)
- **Card Support**: SDSC, SDHC, SDXC (not SDUC - no SPI support)
- **Filesystem**: FAT32
- **Multi-Cog Safe**: Yes, via hardware lock and dedicated worker cog

---

## Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                        APPLICATION COGS                              │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐                              │
│  │  Cog A  │  │  Cog B  │  │  Cog C  │  ... any number of callers   │
│  └────┬────┘  └────┬────┘  └────┬────┘                              │
│       │            │            │                                    │
│       └────────────┼────────────┘                                    │
│                    │ Public API calls                                │
│                    ▼                                                 │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │                    SD CARD DRIVER OBJECT                     │    │
│  │  ┌─────────────────────────────────────────────────────┐    │    │
│  │  │  DAT Section (Shared State - Singleton)             │    │    │
│  │  │  ┌──────────────┐  ┌──────────────────────────────┐ │    │    │
│  │  │  │ Control      │  │ Parameter Block              │ │    │    │
│  │  │  │ - cog_id     │  │ - pb_cmd      (command)      │ │    │    │
│  │  │  │ - api_lock   │  │ - pb_status   (result)       │ │    │    │
│  │  │  └──────────────┘  │ - pb_caller   (who's waiting)│ │    │    │
│  │  │                    │ - pb_param0-3 (arguments)    │ │    │    │
│  │  │                    │ - pb_data0-1  (return data)  │ │    │    │
│  │  │                    └──────────────────────────────┘ │    │    │
│  │  │  ┌──────────────────────────────────────────────────┐│    │    │
│  │  │  │ Filesystem State                                 ││    │    │
│  │  │  │ - buf[512]        (sector buffer)                ││    │    │
│  │  │  │ - entry_buffer[32](directory entry)              ││    │    │
│  │  │  │ - FAT pointers, cluster info, file position...   ││    │    │
│  │  │  └──────────────────────────────────────────────────┘│    │    │
│  │  └─────────────────────────────────────────────────────┘    │    │
│  └─────────────────────────────────────────────────────────────┘    │
│                    │                                                 │
│                    │ send_command()                                  │
│                    ▼                                                 │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │                    WORKER COG (fs_worker)                    │    │
│  │  - Owns SPI pins (exclusive access)                         │    │
│  │  - Executes all SD card operations                          │    │
│  │  - Signals completion via COGATN                            │    │
│  └─────────────────────────────────────────────────────────────┘    │
│                    │                                                 │
│                    │ SPI Bus                                         │
│                    ▼                                                 │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │                      SD CARD                                 │    │
│  │  Pins: CS=60, MOSI=59, MISO=58, SCK=61                      │    │
│  └─────────────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Singleton Pattern

The driver uses a **singleton pattern** via DAT section variables. This means:

1. **All object instances share the same state** - Only one SD card can be mounted at a time
2. **Only one worker cog runs** - First `start()` call launches it, subsequent calls return existing cog ID
3. **Thread-safe by design** - Hardware lock serializes all API access

```spin2
DAT
  cog_id        LONG    -1              ' -1 = not started (singleton check)
  api_lock      LONG    -1              ' Hardware lock ID

PUB start(cs, mosi, miso, sck) : result
  if cog_id <> -1                       ' Already running?
    return cog_id                       ' Return existing cog

  api_lock := locknew()                 ' Allocate hardware lock
  cog_id := cogspin(NEWCOG, fs_worker(), @cog_stack)
  return cog_id
```

---

## Command Protocol

### Command Flow

```
  Caller Cog                              Worker Cog
  ──────────                              ──────────
      │                                       │
      │  1. Acquire api_lock                  │
      │  2. Set pb_caller = COGID()           │
      │  3. Set pb_param0-3 = arguments       │
      │  4. Set pb_cmd = command        ──────│──► (sees pb_cmd ≠ 0)
      │                                       │
      │  5a. if WAIT:                         │  6. Execute operation
      │        WAITATN() ◄────────────────────│  7. Set pb_status = result
      │      (blocked, no CPU)                │  8. Set pb_cmd = CMD_NONE
      │                                       │  9. COGATN(1 << pb_caller)
      │  5b. if NO_WAIT:                      │
      │        return immediately             │
      │        (poll pb_cmd later)            │
      │                                       │
      │  10. Read pb_status                   │
      │  11. Release api_lock                 │
      │                                       │
```

### Parameter Block

The parameter block is the communication mailbox between caller and worker:

| Field | Size | Direction | Purpose |
|-------|------|-----------|---------|
| `pb_cmd` | LONG | Caller→Worker | Command code (0 = idle/complete) |
| `pb_status` | LONG | Worker→Caller | Result/error code |
| `pb_caller` | LONG | Caller→Worker | Caller's cog ID for COGATN signal |
| `pb_param0` | LONG | Caller→Worker | Argument 0 (varies by command) |
| `pb_param1` | LONG | Caller→Worker | Argument 1 |
| `pb_param2` | LONG | Caller→Worker | Argument 2 |
| `pb_param3` | LONG | Caller→Worker | Argument 3 |
| `pb_data0` | LONG | Worker→Caller | Return data 0 (bytes read, etc.) |
| `pb_data1` | LONG | Worker→Caller | Return data 1 |

### Command Codes

```spin2
CON '' command codes
  CMD_NONE      = 0         ' Idle / command complete
  CMD_MOUNT     = 1         ' Mount filesystem
  CMD_UNMOUNT   = 2         ' Unmount filesystem
  CMD_OPEN      = 3         ' Open file: param0=filename ptr
  CMD_CLOSE     = 4         ' Close file
  CMD_READ      = 5         ' Read: param0=buffer ptr, param1=count
  CMD_WRITE     = 6         ' Write: param0=buffer ptr, param1=count
  CMD_SEEK      = 7         ' Seek: param0=position
  CMD_NEWFILE   = 8         ' Create file: param0=filename ptr
  CMD_NEWDIR    = 9         ' Create directory: param0=dirname ptr
  CMD_DELETE    = 10        ' Delete: param0=filename ptr
  CMD_RENAME    = 11        ' Rename: param0=old, param1=new
  CMD_CHDIR     = 12        ' Change dir: param0=dirname ptr
  CMD_READDIR   = 13        ' Read dir entry: param0=index
  CMD_FILESIZE  = 14        ' Get file size (returns in pb_data0)
  CMD_FREESPACE = 15        ' Get free space (returns in pb_data0)
  CMD_SYNC      = 16        ' Flush buffers
  CMD_MOVEFILE  = 17        ' Move file: param0=name, param1=dest
```

---

## Blocking vs Non-Blocking Operations

### Wait Mode Constants

```spin2
CON '' wait mode
  WAIT    = true            ' Block until operation completes
  NO_WAIT = false           ' Return immediately, caller polls
```

### send_command() Implementation

```spin2
PRI send_command(op_cmd, p0, p1, p2, p3, block) : status
  '' Send command to worker cog.
  '' block = WAIT:    Block until complete, return status
  '' block = NO_WAIT: Return immediately, caller polls pb_cmd

  ' Verify worker is running
  if cog_id == -1
    return E_NOT_MOUNTED

  ' Acquire API lock (serialize multi-cog access)
  repeat until locktry(api_lock)

  ' Set up parameter block
  pb_caller := COGID()
  pb_param0 := p0
  pb_param1 := p1
  pb_param2 := p2
  pb_param3 := p3

  ' Issue command
  pb_cmd := op_cmd

  ' Wait or return based on mode
  if block
    WAITATN()                           ' Efficient sleep until signaled
    status := pb_status
    lockrel(api_lock)                   ' Release lock
  else
    status := 0                         ' Caller will poll
    ' NOTE: Lock remains held until caller completes!
    ' Caller MUST call command_complete() to release
```

### Polling for Completion

When using `NO_WAIT`, the caller must poll and release the lock:

```spin2
PRI command_complete() : status | done
  '' Check if async command completed.
  '' Returns: done=true when complete, status contains result.
  '' MUST be called after NO_WAIT to release the lock.

  if pb_cmd == CMD_NONE
    done := true
    status := pb_status
    lockrel(api_lock)                   ' Release lock when done
  else
    done := false
    status := 0
```

### Usage Patterns

**Blocking (simple, recommended for most use):**
```spin2
result := send_command(CMD_READ, @buffer, 512, 0, 0, WAIT)
' Operation complete, result contains status
```

**Non-blocking (for concurrent operations):**
```spin2
send_command(CMD_READ, @buffer, 512, 0, 0, NO_WAIT)
' Do other work while SD operation proceeds...
repeat
  result, done := command_complete()
until done
' Now operation is complete
```

---

## Inter-Cog Signaling

### Why COGATN/WAITATN?

The P2 provides efficient inter-cog signaling:

- **WAITATN()**: Puts cog to sleep with zero CPU usage until signaled
- **COGATN(mask)**: Wakes specific cog(s) identified by bitmask

This is far more efficient than polling a shared variable.

### Signal Flow

```
Caller                                  Worker
──────                                  ──────
pb_caller := COGID()
pb_cmd := CMD_READ
                                        (sees pb_cmd ≠ 0)
WAITATN()  ─────┐                       ... do work ...
  (sleeping)    │                       pb_status := SUCCESS
                │                       pb_cmd := CMD_NONE
                │◄──────────────────────COGATN(1 << pb_caller)
  (wakes up)────┘
status := pb_status                     (ready for next command)
```

### Worker Cog Command Loop

```spin2
PRI fs_worker() | cur_cmd
  ' Initialize SPI pins (this cog owns them)
  ...

  repeat
    ' Wait for command
    repeat until (cur_cmd := pb_cmd) <> CMD_NONE

    ' Dispatch and execute
    case cur_cmd
      CMD_MOUNT:    pb_status := do_mount()
      CMD_READ:     pb_data0 := do_read(pb_param0, pb_param1)
                    pb_status := SUCCESS
      ...

    ' Signal completion
    pb_cmd := CMD_NONE
    COGATN(1 << pb_caller)              ' Wake the caller
```

---

## Lock Handling

### Hardware Lock Purpose

The `api_lock` ensures only one caller cog can issue a command at a time:

```spin2
' Acquire lock (blocks if another cog has it)
repeat until locktry(api_lock)

' ... do work ...

' Release lock
lockrel(api_lock)
```

### Lock Lifecycle

| Phase | Lock State | Owner |
|-------|------------|-------|
| Before send_command() | Free | None |
| During command setup | Held | Caller |
| During WAITATN (blocking) | Held | Caller |
| After completion (blocking) | Released | None |
| During NO_WAIT return | Held | Caller |
| After command_complete() | Released | None |

### Critical: NO_WAIT Lock Responsibility

When using `NO_WAIT`, the caller **must** call `command_complete()` to release the lock, even if the operation fails. Failure to do so will deadlock other cogs.

---

## Error Handling

### Error Codes

```spin2
CON '' error codes
  SUCCESS           = 0
  E_TIMEOUT         = -1        ' Card didn't respond
  E_NO_RESPONSE     = -2        ' Card not responding
  E_BAD_RESPONSE    = -3        ' Unexpected response
  E_NOT_MOUNTED     = -20       ' Filesystem not mounted
  E_FILE_NOT_FOUND  = -40       ' File doesn't exist
  E_FILE_EXISTS     = -41       ' File already exists
  E_NOT_A_DIR       = -43       ' Expected directory
  E_END_OF_FILE     = -46       ' Read past EOF
  E_DISK_FULL       = -60       ' No free clusters
```

### Per-Cog Error Storage

Each cog can retrieve the last error it encountered:

```spin2
DAT
  last_error        LONG    0[8]        ' Error code per cog (8 cogs max)

PUB error() : err_code
  '' Returns last error code for calling cog
  return last_error[COGID()]

PRI set_error(code)
  '' Store error for calling cog
  last_error[COGID()] := code
```

---

## SPI Protocol

### Pin Configuration

| Pin | Function | Direction |
|-----|----------|-----------|
| 60 | CS (Chip Select) | Output |
| 59 | MOSI (Data to card) | Output |
| 58 | MISO (Data from card) | Input |
| 61 | SCK (Clock) | Output |

### Speed Modes

| Mode | Clock | Throughput | Selection |
|------|-------|------------|-----------|
| Initialization | ~400 kHz | - | Required for card init |
| Default Speed | 25 MHz | 3.125 MB/s | After init |
| High Speed | 50 MHz | 6.25 MB/s | After CMD6 switch |

### Clock Timing

```spin2
DAT
  bit_delay       LONG    0             ' Clock timing delay

' During init (~400 kHz for compatibility)
bit_delay := clkfreq / 400_000

' Default Speed (~25 MHz)
bit_delay := 2

' High Speed (~50 MHz) - after CMD6 switch
bit_delay := 1
```

---

## Initialization Sequence

```
1. start(cs, mosi, miso, sck)
   └─► Launches worker cog, allocates lock

2. mount()
   └─► Worker executes:
       a. initCard() - SPI init, CMD0, CMD8, ACMD41, CMD58
       b. Read MBR, find FAT32 partition
       c. Read VBR, extract filesystem parameters
       d. Read FSInfo for free cluster count
       e. Set flags |= F_MOUNTED
```

---

## Filesystem State

All filesystem state is in the DAT section (shared singleton):

```spin2
DAT '' filesystem state
  cs, mosi, miso, sck   LONG    0, 0, 0, 0      ' SPI pins
  fat_sec               LONG    0               ' FAT start sector
  fat2_sec              LONG    0               ' FAT2 start sector
  sec_per_fat           LONG    0               ' Sectors per FAT
  sec_per_clus          LONG    0               ' Sectors per cluster
  root_sec              LONG    0               ' Root directory sector
  dir_sec               LONG    0               ' Current directory sector
  entry_address         LONG    0               ' Current entry address
  n_sec                 LONG    0               ' Current sector number
  file_idx              LONG    0               ' File position (0..filesize)
  flags                 LONG    0               ' State flags

  buf                   BYTE    0[512]          ' Sector buffer
  entry_buffer          BYTE    0[32]           ' Directory entry
  vol_label             BYTE    0[12]           ' Volume label
```

---

## Thread Safety Summary

| Aspect | Mechanism |
|--------|-----------|
| API serialization | Hardware lock (`api_lock`) |
| Pin ownership | Single worker cog |
| Command signaling | COGATN/WAITATN |
| Shared state | DAT section (singleton) |
| Error storage | Per-cog array |

The driver is fully thread-safe: any number of cogs can call the public API simultaneously. The hardware lock ensures commands are serialized, and COGATN provides efficient wake-up signaling.
