# V2 SD Card Driver - Theory of Operations

## Overview

The V2 SD card driver (`SD_card_driver_v2.spin2`) provides high-performance FAT32 filesystem access for SD cards on the Parallax Propeller 2 microcontroller. This document explains the architecture, implementation details, and key design decisions.

## 1. Architecture Overview

### 1.1 Worker Cog Model

The driver uses a **singleton worker cog architecture** where all SD card communication is isolated to a dedicated cog:

```
┌─────────────────┐     ┌─────────────────┐     ┌─────────────────┐
│   User Cog 0    │     │   User Cog N    │     │   Worker Cog    │
│                 │     │                 │     │                 │
│  sd.read()  ────┼─────┼─────────────────┼────►│  SPI Pins       │
│  sd.write() ────┼─────┼─────────────────┼────►│  Smart Pins     │
│  sd.openFile()──┼─────┼─────────────────┼────►│  Streamer DMA   │
│                 │     │                 │     │                 │
└─────────────────┘     └─────────────────┘     └─────────────────┘
         │                      │                      ▲
         │    Parameter Block   │                      │
         └──────────────────────┴──────────────────────┘
                        (DAT section)
```

**Why a dedicated worker cog?**
- **Smart pins must be owned by one cog**: The P2's smart pins can only be configured and used by a single cog
- **Streamer DMA requires cog affinity**: The XINIT/WAITXFI streamer instructions operate on the executing cog's resources
- **Thread safety**: Serializing all operations through one cog eliminates race conditions

### 1.2 Command Queue

API calls from any cog are serialized via a parameter block in the DAT section:

| Field       | Purpose                              |
|-------------|--------------------------------------|
| `pb_cmd`    | Command code (0 = idle/done)         |
| `pb_status` | Result status code                   |
| `pb_caller` | Caller's cog ID (for COGATN signal)  |
| `pb_param0` | Parameter 0 (varies by command)      |
| `pb_param1` | Parameter 1                          |
| `pb_param2` | Parameter 2                          |
| `pb_param3` | Parameter 3                          |
| `pb_data0`  | Result data 0                        |
| `pb_data1`  | Result data 1                        |

The workflow:
1. Caller acquires hardware lock (`api_lock`)
2. Caller writes parameters to DAT section
3. Caller sets `pb_cmd` to desired operation
4. Caller waits for `pb_cmd` to return to 0 (or COGATN)
5. Caller reads results from `pb_status` and `pb_data*`
6. Caller releases lock

### 1.3 Driver Modes

The driver operates in three modes enforced by `driver_mode`:

| Mode              | Value | Description                                |
|-------------------|-------|--------------------------------------------|
| `MODE_NONE`       | 0     | Not initialized - only mount()/initCardOnly() allowed |
| `MODE_RAW`        | 1     | Raw sector access only (for formatting)    |
| `MODE_FILESYSTEM` | 2     | Full FAT32 filesystem access               |

## 2. Smart Pin SPI Implementation

### 2.1 Pin Configuration

The driver uses P2 smart pins for high-speed SPI communication:

| Pin  | Smart Pin Mode | Purpose |
|------|----------------|---------|
| SCK  | `P_TRANSITION` | Clock generation |
| MOSI | `P_SYNC_TX`    | Synchronized transmit (clocked by SCK) |
| MISO | `P_SYNC_RX`    | Synchronized receive (clocked by SCK) |
| CS   | Standard GPIO  | Manual chip select |

### 2.2 Mode Configuration

```spin2
spi_clk_mode := P_TRANSITION | P_OE                  ' Clock idle LOW (SPI mode 0)
spi_tx_mode := P_SYNC_TX | P_OE | P_PLUS2_B          ' TX clocked by SCK (pin+2)
spi_rx_mode := P_SYNC_RX | P_PLUS3_B                 ' RX clocked by SCK (pin+3)
```

The `P_PLUS2_B` and `P_PLUS3_B` settings link MOSI/MISO to the SCK pin for synchronized operation. The pin offset is determined by the physical pin arrangement.

### 2.3 NCO Frequency Calculation

Smart pins use NCO (Numerically Controlled Oscillator) for timing. The SPI clock frequency is set by:

```spin2
spi_period := clkfreq / (target_freq * 2)    ' Half-period in sysclocks
wxpin(sck, spi_period)                        ' Load X register with half-period
```

For example, at 320 MHz sysclock targeting 25 MHz SPI:
- `spi_period = 320_000_000 / (25_000_000 * 2) = 6.4 → 6 sysclocks`
- Actual SPI frequency = 320_000_000 / (6 * 2) = 26.67 MHz

### 2.4 Sysclock Independence

The smart pin configuration adapts automatically to different system clock frequencies:
- At 320 MHz: Higher raw throughput
- At 200 MHz: Still achieves spec-compliant SPI timing
- No code changes needed when sysclock changes

## 3. Streamer DMA Integration

### 3.1 Why Streamer?

The P2 streamer provides hardware DMA between pins and hub memory. For SD card sector transfers (512 bytes), this provides **4-5x throughput** compared to byte-by-byte loops:

| Method | Approximate Speed | CPU Usage |
|--------|-------------------|-----------|
| Byte loop (`sp_transfer_8` x 512) | ~200 KB/s | 100% |
| Streamer DMA | ~1 MB/s | Near-zero |

### 3.2 Streamer Configuration

```spin2
' Mode word format: [31:28]=mode [27:24]=options [23:17]=pin [16:0]=count
STREAM_RX_BASE = $C081_0000   ' 1-pin input to hub, MSB-first
STREAM_TX_BASE = $8081_0000   ' hub to 1-pin output, MSB-first

' For 512-byte read:
stream_mode := STREAM_RX_BASE | (miso << 17) | (512 * 8)   ' 4096 bits
clk_count := 512 * 8 * 2                                    ' 8192 clock transitions
```

### 3.3 Phase Alignment

Critical to reliable operation is aligning the streamer sampling with the SPI clock:

```spin2
' NCO frequency: one sample per full SPI clock period
xfrq := $4000_0000 / spi_period

' Phase offset: sample in middle of each bit for stability
init_phase := $4000_0000

' Timing sequence:
wypin   clk_count, sck            ' Start clock transitions
waitx   spi_period                ' Wait one half-period (align to first rising edge)
xinit   stream_mode, init_phase   ' Start streamer
waitxfi                           ' Wait for completion
```

### 3.4 MISO Smart Pin Interaction

**Critical**: The MISO smart pin must be disabled before streamer capture because both try to read the same pin. The sequence is:

1. Disable MISO smart pin: `pinclear(miso)`, `pinf(miso)`
2. Run streamer transfer
3. Re-enable MISO smart pin for subsequent byte operations

## 4. Card Initialization Sequence

### 4.1 Power-On Reset (400 kHz)

SD cards require slow-speed initialization:

1. Assert CS HIGH
2. Send 74+ clock cycles with MOSI HIGH (card power-up)
3. CMD0 (GO_IDLE_STATE) - puts card in SPI mode
4. CMD8 (SEND_IF_COND) - check for SDHC/SDXC support
5. ACMD41 loop (APP_SEND_OP_COND) - wait for card ready

### 4.2 ACMD41 Loop

```spin2
repeat
  cmd(55, 0)                        ' CMD55 prefix for ACMD
  resp := cmd(41, $4000_0000)       ' ACMD41 with HCS bit
  if resp == 0
    quit                            ' Card is ready
  if timeout expired
    return error
```

### 4.3 Speed Upgrade

After initialization:

1. Read OCR (CMD58) to determine SDHC/SDXC vs SDSC
2. Read CID to identify manufacturer
3. Read CSD to get maximum speed and timeouts
4. Switch to optimal speed based on card capabilities

## 5. Adaptive Timing

### 5.1 TRAN_SPEED Extraction

The CSD register contains the maximum transfer speed in byte [3]:

```spin2
' CSD byte 3: TRAN_SPEED = transfer rate unit (bits 2:0) + time value (bits 6:3)
tran_speed := byte[p_csd + 3]
time_value := (tran_speed >> 3) & $0F
rate_unit := tran_speed & $07

' Time value lookup: 0=reserved, 1=1.0, 2=1.2, 3=1.3, ... F=8.0
' Rate unit: 0=100kbit/s, 1=1Mbit/s, 2=10Mbit/s, 3=100Mbit/s
```

### 5.2 Brand Detection

Some cards don't perform well at their claimed maximum speed. The driver identifies cards by manufacturer ID (MID) from the CID register:

| MID  | Manufacturer | Speed Limit |
|------|--------------|-------------|
| $1D  | PNY/AData    | 20 MHz      |
| $03  | SanDisk      | 25 MHz      |
| $27  | Samsung      | 25 MHz      |
| other| Unknown      | 25 MHz      |

### 5.3 Timeout Configuration from CSD

For SDHC/SDXC cards (CSD version 2.0), fixed timeouts per SD spec:
- Read timeout: 100 ms
- Write timeout: 250 ms

For SDSC cards (CSD version 1.0), calculated from CSD fields:
- TAAC (time unit + value)
- NSAC (clock cycles)
- R2W_FACTOR (read to write time ratio)

## 6. Multi-Sector Operations

### 6.1 CMD18 (READ_MULTIPLE_BLOCK)

Multi-sector reads use CMD18 instead of repeated CMD17:

```
Protocol:
CMD18(start_sector) → R1
  └─→ wait $FE → 512 bytes → CRC16  ─┐
      wait $FE → 512 bytes → CRC16   │ repeat N times
      ...                           ─┘
CMD12(STOP_TRANSMISSION) → R1b
```

**Benefits:**
- Single command overhead instead of N
- No CS deassert/reassert gaps between sectors
- Card can pre-fetch sequentially

### 6.2 CMD25 (WRITE_MULTIPLE_BLOCK)

Multi-sector writes use CMD25 instead of repeated CMD24:

```
Protocol:
CMD25(start_sector) → R1
  └─→ $FC + 512 bytes + CRC16 → response token → wait busy  ─┐
      $FC + 512 bytes + CRC16 → response token → wait busy   │ repeat N times
      ...                                                   ─┘
$FD (stop token) → wait busy
```

**Key differences from CMD24:**
- Data blocks start with $FC (not $FE)
- End with $FD stop token (not CMD12)
- Each block gets a response token before busy wait

### 6.3 File API Integration

The `do_read()` and `do_write()` functions automatically use multi-sector operations when beneficial:

```spin2
' Calculate sectors available before cluster boundary
sectors_to_boundary := sec_per_clus - ((n_sec - cluster_offset) & (sec_per_clus - 1))
full_sectors := count >> 9
multi_count := sectors_to_boundary <# full_sectors

if multi_count >= 2
  ' Use multi-sector operation
  readSectors(n_sec, multi_count, p_buffer)
else
  ' Fall back to single-sector
  readSector(n_sec, BUF_DATA)
```

### 6.4 Cluster Boundary Handling

Multi-sector operations cannot cross cluster boundaries (clusters may not be contiguous). When a read/write reaches a cluster boundary:

1. Complete the transfer up to the boundary
2. Follow the FAT chain to find the next cluster
3. Continue with a new multi-sector operation from the next cluster

## 7. FAT32 Implementation

### 7.1 Filesystem Structures

| Structure | Description |
|-----------|-------------|
| Boot Sector | Contains BPB (BIOS Parameter Block) with filesystem geometry |
| FAT | File Allocation Table - cluster chain linkage |
| Root Directory | Starting directory (not special in FAT32) |
| Data Area | Actual file/directory content |

### 7.2 Cluster Chains

Files are stored as chains of clusters. Each FAT entry points to the next cluster or contains an end-of-chain marker:

```
FAT Entry Values:
$0000_0000        = Free cluster
$0000_0002-FFFF_FFEF = Next cluster number
$0FFF_FFF8-FFFF_FFFF = End of chain
```

### 7.3 FSInfo Caching

The FSInfo sector contains:
- `free_count`: Number of free clusters
- `next_free`: Hint for next free cluster search

The driver caches these values and updates them incrementally rather than scanning the entire FAT on every operation.

### 7.4 Incremental Free Count Tracking

When allocating clusters:
```spin2
decrementFreeCount()   ' Decrement FSInfo free count
' ... allocate cluster ...
' No need to scan FAT - count is tracked
```

When freeing clusters:
```spin2
incrementFreeCount()   ' Increment FSInfo free count
' ... mark cluster as free ...
```

## 8. Error Handling

### 8.1 Error Codes

| Code | Constant | Description |
|------|----------|-------------|
| 0    | SUCCESS  | Operation completed |
| -1   | E_TIMEOUT | Card didn't respond |
| -2   | E_NO_RESPONSE | Card not responding |
| -3   | E_BAD_RESPONSE | Unexpected response |
| -4   | E_CRC_ERROR | Data CRC mismatch |
| -5   | E_WRITE_REJECTED | Card rejected write |
| -7   | E_IO_ERROR | General I/O error |
| -40  | E_FILE_NOT_FOUND | File doesn't exist |
| -60  | E_DISK_FULL | No free clusters |

### 8.2 CMD13 Status Verification

After sector operations, CMD13 (SEND_STATUS) verifies the card state:

```spin2
PRI checkCardStatus(caller_name) : result
  sp_transfer_8($FF)
  pinl(cs)
  sp_transfer_8($4D)                ' CMD13 = $40 | 13
  sp_transfer_8(0)                  ' Argument = 0
  sp_transfer_8(0)
  sp_transfer_8(0)
  sp_transfer_8(0)
  sp_transfer_8($FF)                ' CRC (ignored in SPI)
  r1 := sp_transfer_8($FF)          ' R1 response
  status := sp_transfer_8($FF)      ' Status byte
  pinh(cs)

  if r1 | status
    debug("ERROR: CMD13 returned R1=$", r1, " status=$", status)
    return E_IO_ERROR
  return SUCCESS
```

### 8.3 Recovery Strategies

On transient errors:
1. Retry the operation (typically 3 attempts)
2. Re-initialize the card if retries fail
3. Return error to caller if recovery fails

## 9. Performance Characteristics

### 9.1 Measured Throughput (320 MHz sysclock)

| Operation | Single-Sector | Multi-Sector |
|-----------|---------------|--------------|
| Read | ~400 KB/s | ~1.2 MB/s |
| Write | ~200 KB/s | ~600 KB/s |

### 9.2 Optimization Guidelines

For best performance:
1. Use large sequential reads/writes (multi-sector benefits)
2. Avoid frequent file open/close cycles
3. Write in multiples of 512 bytes when possible
4. Use sync() sparingly (forces flush to card)

## 10. Memory Architecture

### 10.1 Spin2 Interpreter Memory Usage

Understanding where the Spin2 interpreter runs is critical for memory optimization decisions:

| Memory Region | Size | Contents When Running Spin2 |
|---------------|------|----------------------------|
| **Cog RAM** | 2KB (512 longs) | Core Spin2 interpreter code |
| **LUT RAM** | 2KB (512 longs) | Additional interpreter code (loaded by interpreter) |
| **Hub RAM** | Variable | Bytecode, more interpreter code (hub-exec), user data |

The complete Spin2 interpreter is approximately **4,784 bytes** - larger than cog RAM alone. It spans:
- Cog RAM (loaded at boot)
- LUT RAM (loaded by the interpreter itself)
- Hub RAM (for hub-exec portions)

**Key implication**: When running Spin2, both cog RAM and LUT RAM are occupied by the interpreter. User data (VAR, DAT sections) must reside in hub RAM.

### 10.2 Buffer Architecture

The driver uses three separate 512-byte sector buffers:

| Buffer | Purpose | Cache Tracking Variable |
|--------|---------|------------------------|
| `dir_buf` | Directory sector operations | `dir_sec_in_buf` |
| `fat_buf` | FAT sector operations | `fat_sec_in_buf` |
| `buf` | Data sector operations | `sec_in_buf` |

Plus supporting buffers:
- `entry_buffer` (32 bytes): Current directory entry
- `vol_label` (12 bytes): Volume label string

**Total buffer memory**: ~1,580 bytes in hub RAM

**Why separate buffers?**
- **FAT operations interleave with data operations**: During file read/write, we frequently access FAT sectors to follow cluster chains while also reading/writing data sectors. A single buffer would thrash.
- **Directory operations are distinct**: Directory scanning happens during open/search, separate from data I/O.
- **Simplified cache tracking**: Each buffer has its own cache variable, avoiding complex invalidation logic.

**Potential consolidation**: The directory buffer could potentially merge with the data buffer (2 buffers instead of 3), saving 512 bytes. Directory and data operations are mostly sequential, not interleaved. The FAT buffer should remain separate.

### 10.3 DAT Section Variables

All driver state variables reside in the DAT section (hub RAM):

| Category | Variables | Access Frequency |
|----------|-----------|------------------|
| **SPI control** | `cs`, `mosi`, `miso`, `sck`, `spi_period` | Every SPI operation |
| **Cache state** | `sec_in_buf`, `dir_sec_in_buf`, `fat_sec_in_buf` | Every sector read |
| **File state** | `n_sec`, `file_idx`, `flags`, `entry_address` | Every file operation |
| **FS geometry** | `fat_sec`, `root_sec`, `sec_per_clus`, `cluster_offset` | Mount and cluster calculations |
| **Card info** | `card_mfr_id`, `card_max_speed_hz`, timeouts | Mount and timing |
| **API state** | `cog_id`, `api_lock`, `driver_mode` | Start/stop only |

Total variable storage: ~180 longs (~720 bytes) plus the 128-long worker stack.

### 10.4 LUT RAM Feasibility Analysis

**Question**: Could LUT RAM be used for buffers or variables to improve performance?

**Analysis**:

1. **Spin2 interpreter occupies LUT RAM**: As described above, the interpreter uses LUT RAM for code. There's no room for user data when running Spin2.

2. **Streamer cannot write to LUT**: The P2 streamer's capture modes (`X_1P_1DAC1_WFBYTE`, etc.) write to hub RAM via WRFAST. There is no streamer mode that writes directly to LUT or cog RAM. Data flow is always:
   ```
   SD Card → Streamer → Hub RAM (required) → [optional copy to LUT/cog]
   ```

3. **LUT-to-pins modes are OUTPUT only**: The `X_*_LUT` streamer modes read FROM LUT to output pins (for video/audio). They cannot capture input TO LUT.

4. **Even with pure PASM, hub copy required**: If we rewrote the driver in pure PASM (running from LUT, with cog RAM free for data), we'd still need:
   ```
   SD Card → Streamer → Hub staging → SETQ+RDLONG → Cog RAM buffer
   ```
   This adds copy overhead that may negate the faster cog RAM access.

**Conclusion**: For this Spin2-based driver, **all data must remain in hub RAM**. The LUT RAM optimization path would require:
- Complete rewrite in PASM (losing Spin2's maintainability)
- Still requiring hub RAM as streamer intermediary
- Modest benefit for the significant complexity increase

### 10.5 When LUT/Cog RAM IS Beneficial

LUT and cog RAM provide significant benefits for:

| Use Case | Why It Works |
|----------|--------------|
| **Lookup tables** (sine, color palettes) | Load once, access many times via RDLUT (3 cycles) |
| **PASM driver code** | Execute from LUT, use cog RAM for data |
| **Frequently-accessed constants** | Single-cycle access from cog RAM |
| **Real-time DSP** | Data processing without hub access latency |

For our SD driver, the access pattern (write once per sector read, process, write back) doesn't match LUT's sweet spot of "load once, access repeatedly."

### 10.6 Memory Map Summary

```
┌─────────────────────────────────────────────────────────────────┐
│                         HUB RAM (~512 KB)                       │
├─────────────────────────────────────────────────────────────────┤
│  Spin2 Bytecode + Interpreter (hub-exec portions)               │
│  ───────────────────────────────────────────────────            │
│  Driver DAT Section:                                            │
│    • Singleton control (cog_id, api_lock, driver_mode)          │
│    • Parameter block (pb_cmd, pb_status, pb_param0-3, etc.)     │
│    • Worker stack (128 longs)                                   │
│    • SPI pin configuration                                      │
│    • Filesystem state variables                                 │
│    • Sector buffers (dir_buf, fat_buf, buf = 1536 bytes)        │
│    • Entry buffer (32 bytes), vol_label (12 bytes)              │
│  ───────────────────────────────────────────────────            │
│  User application code and data                                 │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│                   WORKER COG (dedicated to SD driver)           │
├─────────────────────────────────────────────────────────────────┤
│  Cog RAM (512 longs = 2KB):  Spin2 Interpreter (core)           │
│  LUT RAM (512 longs = 2KB):  Spin2 Interpreter (extended)       │
│                                                                 │
│  Smart Pins: SCK (P_TRANSITION), MOSI (P_SYNC_TX),              │
│              MISO (P_SYNC_RX), CS (GPIO)                        │
│  Streamer: Configured for sector DMA via XINIT/WAITXFI          │
└─────────────────────────────────────────────────────────────────┘
```

## 11. Usage Example

```spin2
OBJ
  sd : "SD_card_driver_v2"

PUB main() | buf[128]
  ' Mount card (pins: CS, MOSI, MISO, SCK)
  if sd.mount(SD_CS, SD_MOSI, SD_MISO, SD_SCK)
    debug("Card mounted successfully")

    ' Create and write a file
    sd.newFile(string("TEST.TXT"))
    sd.writeString(string("Hello, SD Card!"))
    sd.closeFile()

    ' Read it back
    sd.openFile(string("TEST.TXT"))
    sd.read(@buf, sd.fileSize())
    sd.closeFile()

    debug(zstr(@buf))
  else
    debug("Mount failed: ", sdec_(sd.error()))
```

---

*Document generated: 2026-01-29*
*Driver version: V2 with smart pins, streamer DMA, and multi-sector operations*
