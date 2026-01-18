# SD Card Driver Architecture Decisions

This document captures the architectural decisions for the multi-cog SD card driver. Each decision includes the P2-specific constraints that make it the correct choice. Use this document as a reference when implementing or reviewing the driver.

---

## Decision 1: Dedicated Worker Cog (Not Lock-Based Sharing)

### The Question
How should multiple cogs safely access the SD card?

### Options Considered
1. **Lock-based sharing**: Any cog acquires lock, does SPI, releases lock
2. **Dedicated worker cog**: One cog owns SPI; others send commands via hub memory

### The P2 Constraint That Decides This

**P2 pins are controlled by per-cog registers.** Each cog has its own private DIR and OUT registers at addresses `$1FA-$1FF`. When Cog 0 executes `PINH(pin)`, it sets bits in Cog 0's DIR/OUT registers. Cog 1's registers are unaffected.

This means:

```
Lock-Based Approach (PROBLEMATIC):
─────────────────────────────────────
Cog 0 acquires lock
  → PINH(cs), PINH(mosi), PINL(sck)     ← Sets Cog 0's DIR/OUT
  → Do SPI transfer
  → PINFLOAT(cs), PINFLOAT(mosi)...     ← MUST tri-state before release!
  → Release lock

Cog 1 acquires lock
  → PINH(cs), PINH(mosi), PINL(sck)     ← Sets Cog 1's DIR/OUT (again!)
  → Do SPI transfer
  → PINFLOAT all pins
  → Release lock
```

**Every operation requires full pin re-initialization.** If any cog forgets to tri-state before releasing the lock, multiple cogs have `DIR=1` on the same pin, causing undefined behavior.

```
Dedicated Cog Approach (CORRECT):
─────────────────────────────────────
Worker Cog (at startup, once):
  → PINH(cs), PINH(mosi), PINL(sck)     ← Done ONCE, never changes

  repeat forever:
    → Wait for command in hub memory
    → Execute (pins already configured)
    → Signal completion

Other cogs:
  → Write command to hub memory
  → Wait for completion
  → Never touch pins
```

### Decision
**Use a dedicated worker cog.** It eliminates per-operation pin overhead and removes the risk of pin conflicts entirely.

---

## Decision 2: Spin2 Worker via COGSPIN (Not Pure PASM2)

### The Question
Should the worker cog be pure PASM2 (started with `COGINIT`) or Spin2 (started with `COGSPIN`)?

### The Analysis

| Factor | Pure PASM2 | Spin2 + Inline PASM2 |
|--------|------------|---------------------|
| SPI bit-bang timing | Native | Inline PASM2 (same) |
| FAT32 logic (cluster chains, directories) | Complex, error-prone | Natural, readable |
| Code space | 496 longs max (cog RAM) | Unlimited (hub) |
| Maintainability | Difficult | Easy |
| SD card latency | ~1-10ms per operation | ~1-10ms per operation |

**The bottleneck is the SD card, not the P2.** SD card operations take milliseconds. Whether the FAT logic runs in 2µs (PASM2) or 20µs (Spin2) is irrelevant when the card takes 5,000µs to respond.

The existing inline PASM2 for SPI bit-banging is kept—that's the only timing-critical code. Everything else (FAT parsing, directory traversal, cluster allocation) benefits from Spin2's readability.

### Decision
**Use Spin2 worker via COGSPIN.** Keep inline PASM2 only for SPI transfers. This matches the P2-FLASH-FileSystem pattern.

---

## Decision 3: DAT Block Singleton Pattern

### The Question
How do we ensure all callers share the same driver instance?

### The Spin2 Memory Model

```spin2
VAR block: Each object INSTANCE gets its own copy
DAT block: SHARED across all instances of the object
```

When multiple `.spin2` files each declare `OBJ fs : "SD_FileSystem"`, they create separate object instances, but they all share the **same DAT block**:

```
Application.spin2                    ┌──────────────────────────┐
  OBJ fs : "SD_FileSystem"  ────────►│   SD_FileSystem DAT      │
                                     │   (one shared copy)      │
DataLogger.spin2                     │                          │
  OBJ fs : "SD_FileSystem"  ────────►│   cog_id = 3             │
                                     │   api_lock = 2           │
SensorManager.spin2                  │   param_block[...]       │
  OBJ fs : "SD_FileSystem"  ────────►│   buf[512]               │
                                     └──────────────────────────┘
```

### The Singleton Guard

```spin2
PUB start(cs, mosi, miso, sck) : result
  ' ─── SINGLETON GUARD ───
  if cog_id <> -1
    return true                 ' Already running - instant success

  ' First caller proceeds to start worker cog...
```

The first `start()` call initializes the worker cog. Subsequent calls see `cog_id <> -1` and return immediately. All callers share the same worker, buffer, and state.

### Decision
**Use DAT block for all shared state.** Initialize `cog_id` to `-1` at declaration. The singleton guard ensures exactly one worker cog regardless of how many objects are instantiated.

---

## Decision 4: Parameter Block + COGATN Signaling

### The Question
How do caller cogs communicate with the worker cog?

### Options Considered
1. **Hub polling**: Caller writes command, polls `cmd == 0` until done
2. **COGATN signaling**: Caller writes command, executes `WAITATN`, worker signals via `COGATN`

### Why COGATN Wins

| Aspect | Hub Polling | COGATN |
|--------|-------------|--------|
| Hub bandwidth during wait | Continuous reads | **Zero** |
| Caller cog state | Busy-looping | **Sleeping** |
| Wake latency | Variable (depends on poll rate) | **0 clocks** |
| Power efficiency | Poor | **Good** |

**COGATN is a hardware interrupt mechanism.** The caller cog truly sleeps—no instructions execute, no hub bandwidth consumed—until the worker sends attention.

### The Protocol

```
Caller Cog                              Worker Cog
───────────────────────────────────────────────────────────────
1. Acquire api_lock
2. pb_caller := COGID()
3. pb_param0..3 := parameters
4. pb_cmd := CMD_xxx  ─────────────────► 5. See cmd != 0
5. WAITATN (sleep)                       6. Execute operation
   │                                     7. pb_status := result
   │                                     8. pb_cmd := 0
   │                                     9. COGATN(1 << pb_caller)
   ▼                                         │
6. Wake instantly ◄──────────────────────────┘
7. Read pb_status, pb_data0
8. Release api_lock
```

### Decision
**Use COGATN for completion signaling.** The parameter block lives in hub DAT. Callers sleep efficiently via `WAITATN`. A hardware lock serializes API access.

---

## Decision 5: Keep Bit-Bang PASM2 for SPI (Not SmartPins)

### The Question
Should we use SmartPins (P_SYNC_TX, P_SYNC_RX, P_TRANSITION) for SPI, or keep the existing bit-bang PASM2?

### Deep Analysis Performed

We evaluated SmartPins against five criteria:

| Criterion | Bit-Bang | SmartPins | Verdict |
|-----------|----------|-----------|---------|
| Clock independence | Requires `clkfreq` calc | Requires `clkfreq` calc | **Tie** |
| Pin setup | Per-bit manipulation | One-time setup | SmartPins |
| Timeout handling | Need ADDCT1/POLLCT1 | Need POLLSE1/POLLSE2 | **Tie** |
| Raw throughput | ~10 MHz practical | ~25 MHz possible | **Tie** (card is bottleneck) |
| Proven reliability | Years of use | New implementation | Bit-bang |
| Code complexity | Explicit | More abstract | Bit-bang |

### The Critical Finding: No Multi-Event Blocking Wait

We hoped SmartPins + Events would allow elegant timeout handling:
```pasm2
' HOPED FOR (does not exist):
WAITANY SE1, SE2        ' Wait for timeout OR data - doesn't exist!
```

**P2 reality**: You can configure SE1-SE4 for different events, but `WAITSE1` only waits for SE1. To monitor multiple events, you must poll:

```pasm2
' ACTUAL PATTERN (from P2KB):
.wait_loop:
        POLLSE1 WC              ' Check timeout
  IF_C  JMP #.timeout_error
        POLLSE2 WC              ' Check data ready
  IF_C  JMP #.data_ready
        JMP #.wait_loop
```

This is the same polling pattern needed for bit-bang timeout handling. SmartPins don't provide an elegance advantage.

### The Existing Code Is Optimal

```pasm2
' Current sector transfer (proven, fast):
                rep       #4, #128          ' 128 iterations, 4 instructions
                ...
                wflong    result            ' Stream to hub via FIFO
```

The `REP` loop with `WRFAST`/`WFLONG` is already optimal. Rewriting gains nothing and risks introducing bugs.

### Decision
**Keep existing bit-bang PASM2.** Add timeout protection using `ADDCT1`/`POLLCT1` (see Task 2.4 in sprint plan). Consider SmartPins only if card compatibility issues emerge in the future.

---

## Decision 6: Streamer Not Applicable for SPI

### The Question
Can the P2 Streamer (DMA-like engine) improve SD card transfers?

### Why It Doesn't Apply

The Streamer is a **parallel** transfer engine:
- Drives 1-32 pins simultaneously
- Captures parallel pin groups to hub
- Great for video, parallel buses, memory interfaces

SPI is **serial**:
- 1 bit per clock on MOSI/MISO
- Requires bit-by-bit serialization

The Streamer cannot serialize data to a single pin with clock synchronization.

### What We Already Use

The current code already uses the Streamer's **FIFO** capability optimally:

```pasm2
wrfast  ##$8000_0000, ptr       ' Setup hub write FIFO
...
wflong  data                     ' Stream 32 bits to hub (single instruction)
```

This is the correct use: FIFO streams data to/from hub while PASM2 handles bit serialization.

### Decision
**Do not use Streamer for SPI.** Current FIFO usage (`wrfast`/`rdfast`/`wflong`/`rflong`) is already optimal. Streamer would only apply if we implemented SDIO 4-bit parallel mode (not planned).

---

## Decision 7: Error Code Design

### The Question
How should errors be reported to callers?

### Convention
```
0           = Success
Negative    = Error (specific code indicates type)
Positive    = Valid data (byte count, handle, etc.)
```

### Error Code Ranges

| Range | Category | Examples |
|-------|----------|----------|
| 0 | Success | `SUCCESS` |
| -1 to -19 | SPI/Communication | `E_TIMEOUT`, `E_CRC_ERROR` |
| -20 to -39 | Card/Mount | `E_NOT_MOUNTED`, `E_NOT_FAT32` |
| -40 to -59 | File Operations | `E_FILE_NOT_FOUND`, `E_END_OF_FILE` |
| -60 to -79 | Resources | `E_DISK_FULL`, `E_NO_HANDLE` |
| -80 to -89 | Seek | `E_SEEK_PAST_END` |

### Thread-Safe Error Storage

Multiple cogs may call the API concurrently. Each cog needs its own error storage:

```spin2
DAT
  last_error    LONG    0[8]    ' One slot per possible cog

PUB error() : code
  return LONG[@last_error][COGID()]

PRI set_error(code) : code
  LONG[@last_error][COGID()] := code
  return code
```

### Decision
**Use negative error codes with per-cog storage.** This matches P2-FLASH-FileSystem and enables safe multi-cog operation.

---

## Decision 8: Timeout Policy (Not Retries)

### The Question
How should the driver handle communication failures?

### The Policy

**Layer 1 (SPI)**: Use timeouts, not retries.
```
If a transfer times out → return error immediately
The caller decides whether to retry
```

**Layer 2 (Card Init)**: Limited retries during initialization only.
```
CMD0 (GO_IDLE): Up to 5 attempts, 10ms apart
ACMD41 (init): Up to 200 attempts (spec allows 1 second)
Normal commands: No retries
```

**Layer 3 (API)**: Caller's responsibility.
```spin2
' Caller retry pattern:
repeat 3
  status := sd.read(handle, @buffer, 512)
  if status == sd.SUCCESS
    quit
  waitms(100)
```

### Rationale

The driver cannot know if a retry is safe:
- Was partial data written before failure?
- Is the card in an inconsistent state?
- Should we re-seek before retrying?

Only the caller has context to make these decisions.

### Critical Bug Fix Required

The current `readSector()` has an infinite loop waiting for the start token:

```pasm2
' CURRENT (BUG - hangs forever):
.startloop
                testp     _miso         wc
  if_c          jmp       #.startloop       ' ← No timeout!

' FIXED:
                GETCT     timeout
                ADDCT1    timeout, ##clkfreq    ' 1 second
.startloop
                POLLCT1   WC
  if_c          jmp       #.timeout_error       ' Bail on timeout
                drvh      _sck
                drvl      _sck
                testp     _miso         wc
  if_c          jmp       #.startloop
```

### Decision
**Use timeouts at SPI level; let callers decide on retries.** Fix the `readSector()` timeout bug immediately (Sprint Task 2.4).

---

## Summary: Why This Architecture

| Component | Decision | P2-Specific Reason |
|-----------|----------|-------------------|
| Cog model | Dedicated worker | Per-cog DIR/OUT registers |
| Worker language | Spin2 + inline PASM2 | SD card is bottleneck, not P2 |
| State sharing | DAT block singleton | Spin2 memory model |
| Signaling | COGATN | Zero-cost waiting, instant wake |
| SPI method | Bit-bang with timeouts | No multi-event wait instruction |
| Streamer | Not used | Parallel engine, SPI is serial |
| Errors | Negative codes, per-cog | Thread-safe multi-cog access |
| Failures | Timeouts, not retries | Caller has context to decide |

These decisions work together to create a driver that is:
- **Safe**: Multiple cogs can call APIs without conflicts
- **Efficient**: COGATN signaling, optimal FIFO usage
- **Reliable**: Timeout protection prevents hangs
- **Maintainable**: Spin2 for logic, PASM2 only where needed

---

*Document created: 2026-01-17*
*For use by: Implementation agents, code reviewers*
