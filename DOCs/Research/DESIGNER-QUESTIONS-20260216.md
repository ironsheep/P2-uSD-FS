# Questions for Chip Designer — 2026-02-16

## Background: How We Use the SPI Bus

Our SD card driver uses four P2 pins for the SPI bus:

### Pin Assignments

| Signal | Pin | Function |
|--------|-----|----------|
| MISO   | P58 | Master In, Slave Out (card data to P2) |
| MOSI   | P59 | Master Out, Slave In (P2 data to card) |
| CS     | P60 | Chip Select (manual GPIO, active LOW) |
| SCK    | P61 | Serial Clock |

### Pin Modes Configured at Init (once)

**SCK (P61) — Clock Generation:**
- Mode: `P_TRANSITION | P_OE`
- TT = %01 (output enabled regardless of DIR, source = SMART/OUT)
- X register: half-period in sysclks (set by setSPISpeed)
- Operation: `wypin(count, sck)` fires exactly `count` transitions, then pin goes idle
- SPI Mode 0: clock idles LOW between bursts
- No spurious edges — counted transitions only fire when we load Y

**MOSI (P59) — Transmit:**
- Mode: `P_SYNC_TX | P_OE | P_PLUS2_B`
- TT = %01 (output enabled regardless of DIR, source = SMART/OUT)
- P_PLUS2_B = clock source is pin+2 = P61 (SCK)
- X register: `%1_00111` = start-stop mode, 8 bits (for command/response bytes)
- Operation: `wypin(data, mosi)` loads shift register, smart pin shifts on SCK edges
- For sector bulk data: we switch this pin to streamer TX mode (see below)

**MISO (P58) — Receive:**
- Mode: `P_SYNC_RX | P_PLUS3_B`
- TT = %00 (output disabled — this is an input pin)
- P_PLUS3_B = clock source is pin+3 = P61 (SCK)
- X register: `%1_00111` = on-edge sample, 8 bits
- Operation: `wypin(clk, sck)` clocks data in, `rdpin(miso)` reads result
- For sector bulk data: we switch this pin to streamer RX mode (see below)

**CS (P60) — Chip Select:**
- Standard GPIO, no smart pin mode
- `pinh(cs)` = deselect, `pinl(cs)` = select

### Two Operating Modes for MOSI/MISO

Each data transfer uses the pins in one of two ways:

**Mode 1: Smart Pin (command bytes, CRC, response tokens)**
- 8-bit or 32-bit transfers via `sp_transfer_8()` / `sp_transfer_32()`
- MOSI: P_SYNC_TX shifts data out on SCK edges
- MISO: P_SYNC_RX samples data in on SCK edges
- Used for: CMD bytes, arguments, CRC bytes, response polling, busy-wait polling

**Mode 2: Streamer (512-byte sector bulk data)**
- P2 hardware DMA via XINIT/WAITXFI
- RX: `STREAM_RX_BASE ($C081_0000)` — 1-pin MISO input to hub, MSB-first
- TX: `STREAM_TX_BASE ($8081_0000)` — hub to 1-pin MOSI output, MSB-first
- 4096 bits (512 bytes), NCO-paced at SPI clock rate
- SCK still runs via P_TRANSITION wypin(8192, sck) for all 4096 bits

### The Mode Transition Problem (What We're Debugging)

For every sector write, MOSI must transition: Smart Pin → Streamer → Smart Pin

**Current sequence in writeSector():**
```
1. CMD24 sent via smart pin (sp_transfer_8/sp_transfer_32)
2. Start token $FE sent via smart pin (sp_transfer_8)
3. --- TRANSITION: Smart Pin → Streamer ---
   a. wypin(0, mosi)           ← EXPERIMENT: force shift register to zero
   b. pinclear(mosi)           ← Clears WRPIN=0, DIR=0 (full teardown)
   c. pinl(mosi)               ← DIR=1, OUT=0 (drive low for streamer)
   d. XINIT/WYPIN/WAITXFI      ← Streamer ships 512 bytes
4. --- TRANSITION: Streamer → Smart Pin ---
   e. wrpin(mosi, spi_tx_mode) ← Reconfigure P_SYNC_TX | P_OE | P_PLUS2_B
   f. wxpin(mosi, %1_00111)    ← 8-bit start-stop mode
   g. pinh(mosi)               ← DIRH - enable smart pin
5. CRC bytes sent via smart pin (sp_transfer_8)
6. Response token read, busy-wait polling...
```

For sector reads, MISO has the same pattern:
```
1. CMD17 + wait for $FE token via smart pin
2. --- TRANSITION: Smart Pin → Streamer ---
   a. pinclear(miso)           ← Full teardown
   b. pinf(miso)               ← Float pin (input mode for streamer)
   c. XINIT/WYPIN/WAITXFI      ← Streamer captures 512 bytes
3. --- TRANSITION: Streamer → Smart Pin ---
   d. wrpin(miso, spi_rx_mode) ← Reconfigure P_SYNC_RX | P_PLUS3_B
   e. wxpin(miso, %1_00111)    ← 8-bit on-edge sample
   f. pinh(miso)               ← DIRH - enable smart pin
4. CRC bytes read via smart pin
```

### RX Streamer Timing (Works Perfectly)

```pasm
wypin   clk_count, _sck      ' Start clock transitions
waitx   align_delay           ' Wait one half-period (align with first rising edge)
xinit   stream_mode, init_phase  ' Start streamer with phase=$4000_0000
waitxfi                       ' Wait for completion
```

### TX Streamer Timing (Bug on Some Cards)

```pasm
setxfrq xfrq                 ' Set NCO rate
rdfast  #0, p_buf            ' Setup RDFAST from hub
xinit   stream_mode, #0      ' Start streamer with phase=0 (natural NCO ramp)
wypin   clk_count, _sck      ' Start clock
waitxfi                       ' Wait for completion
```

Phase=0 for TX was reverted from $4000_0000 (which the previous developer tried). V2 of the driver used phase=0 and worked.

---

## The Bug

**Symptom:** On the SanDisk Industrial 64GB card (but NOT on Gigastone ASTC 64GB), every other writeSector produces data that is **exactly 1 bit right-shifted** when read back.

**Signature of the shift:** Every byte value is exactly halved with a carry-in from the previous byte:
- Expected `$A3` → got `$D1` (shifted right 1 with MSB=1)
- Expected `$DEADBEEF` → got `$EF56DF77`
- Expected `$55AA` (MBR signature) → got `$2AD5`

**Pattern:** First write to a sector works. Second write (different sector) fails. Third works. Fourth fails. It's the alternating writes that fail, specifically sectors 100,001 and 100,004 in our test (with the wypin(0) experiment active).

**Card-specific:** The Gigastone has more relaxed SPI setup/hold timing and tolerates whatever glitch is happening. The SanDisk Industrial has tighter margins and samples the glitch as a real data bit.

---

## Question 1: Can We Use TT to Toggle Smart Pin Output Without Teardown?

### Context

We confirmed experimentally:
- P_SYNC_TX in start-stop mode **idles HIGH** after completing a byte transfer
- Streamer output and smart pin output are **OR'd** on the physical pin (designer confirmed)
- Therefore: smart pin idle-HIGH OR'd with streamer data = all 1s (we saw all-$FE)
- Currently we **must** pinclear (full teardown) before the streamer to get clean data
- After the streamer, we must wrpin/wxpin/pinh to rebuild the smart pin

### The Idea

Instead of pinclear (which destroys WRPIN), could we toggle just the TT bits in WRPIN?

**Before streamer:** Change TT from %01 to %00
- This **disables** the smart pin's output (bit 6 = 0)
- WRPIN, WXPIN registers are preserved — the smart pin mode is still configured
- The smart pin's internal state (shift register, counters) remains

**After streamer:** Change TT back from %00 to %01
- This **re-enables** the smart pin's output
- No need for wrpin/wxpin/pinh rebuild

### The Specific Question

When TT=%00 disables the smart pin's output path, **does the streamer still have its own independent path to drive the physical pin?**

The streamer uses XINIT with a mode word that includes the pin number. Does the streamer write directly to the pin hardware, bypassing the TT output-enable gate? Or does the streamer's output also go through the TT/SMART/OUT path and get blocked by TT=%00?

**In other words:** Is the streamer's pin drive completely independent of the smart pin's TT output enable? If yes, TT=%00 would silence the smart pin while the streamer drives freely. If no, we need a different approach.

### How We'd Change TT

To change TT from %01 to %00, we'd need to write a new WRPIN value with bit 6 cleared. Our current WRPIN for MOSI is:
```
spi_tx_mode := P_SYNC_TX | P_OE | P_PLUS2_B
```

We'd precompute a "muted" version:
```
spi_tx_muted := P_SYNC_TX | P_PLUS2_B       ' Same but without P_OE (TT bit 6 = 0)
```

The transition would be:
```
wrpin(mosi, spi_tx_muted)    ' TT=%00: disable smart pin output
' ... streamer runs ...
wrpin(mosi, spi_tx_mode)     ' TT=%01: re-enable smart pin output
```

**Sub-question:** Does wrpin on an active smart pin (DIR=1) cause a reset? Or does it just update the mode register? If wrpin resets the smart pin, this approach won't work either.

---

## Question 2: WXPIN on Active Smart Pin — Is This Defined Behavior?

### Context

Our `sp_transfer_8()` and `sp_transfer_32()` both do `wxpin` on a running smart pin without resetting it first (no DIRL before wxpin). The P2KB documentation says:

> "Smart pins MUST be reset (DIR=0) before any configuration."

### What Our Code Does

Every call to sp_transfer_8:
```pasm
wxpin   #$27, _mosi       ' 8 bits, start-stop mode (%1_00111)
wypin   tx_data, _mosi     ' Load data
drvl    _mosi              ' Enable (or keep enabled)
```

Every call to sp_transfer_32:
```pasm
wxpin   #$3F, _mosi       ' 32 bits, start-stop mode (%1_11111)
wypin   tx_data, _mosi     ' Load data
drvl    _mosi              ' Enable (or keep enabled)
```

After the first call, the smart pin is LEFT RUNNING (DIR stays HIGH). The next call does wxpin on a live smart pin.

### The Critical Transition

In `cmd()`, we send the 32-bit argument then immediately switch to 8-bit mode:
```
sp_transfer(parm, 32)    → wxpin #$3F (32 bits) on live pin
sp_transfer($95, 8)      → wxpin #$27 (8 bits) on live pin — X register changes!
```

### The Question

1. Is `wxpin` on an active smart pin (DIR=1) defined behavior in the silicon?
2. Does it update the X register cleanly mid-operation, or can it corrupt the shift register / bit counter / output state?
3. Could a wxpin during the smart pin's idle-between-transfers state (after completing one transfer, before loading the next) cause a brief output glitch?
4. Should we be doing `dirl` → `wxpin` → `dirh` every time we change the bit count?

### Why This Matters

If wxpin on an active pin causes even a brief glitch on MOSI, timing-sensitive cards (SanDisk Industrial) would see it as a data bit. This could explain the 1-bit shift that only appears on cards with tight SPI margins.

---

## Question 3: What is P_SYNC_TX's Idle Output State?

### What We Observed

When we removed the pinclear/rebuild cycle (left smart pin running during streamer), readback was all $FE — every byte was all-1s. This means the smart pin's idle output is HIGH, and it OR'd with the streamer data to produce all-1s.

### The Question

In P_SYNC_TX start-stop mode, after the smart pin finishes shifting its last bit:

1. What drives the pin output? Is it the SMART path holding the last shift register bit? Or does it revert to the OUT fallback?
2. Is there any way to force the smart pin's output to zero while it's idle, without clearing the mode? (e.g., wypin(0) to load zeros into the shift register?)
3. With TT=%01 (SMART/OUT), if SMART is idle, does OUT (which the streamer writes to) actually become visible? Or does SMART always hold control once the mode is running?

### What We Tried

- `wypin(0, mosi)` before pinclear: **Moved** the failing sectors (from sector 4 to sectors 2 and 5), but didn't eliminate the failures. Suggests pin state matters but something else is also involved.
- Removing pinclear entirely (no teardown): All $FE readback (smart pin idle-HIGH OR'd with everything)
- DIRL/DIRH (disable/re-enable without clearing mode): All $00 readback (DIRL disables pin output, streamer can't drive)

---

## Question 4: Does DRVL (pinl) Reset the Smart Pin State Machine?

### Context

`sp_transfer_8()` does `drvl _mosi` on every call. DRVL sets DIR=1 and OUT=0.

If the smart pin was already running (DIR was already 1), does DRVL trigger a 0→1 transition on DIR? Or does it see DIR is already 1 and just update OUT?

**Why this matters:** If DRVL causes a DIR 0→1 transition even when DIR was already 1, it would reset the smart pin state machine on every single sp_transfer_8 call. If it does NOT cause a reset (because DIR was already 1), then the wxpin in the same function is modifying a truly running smart pin.

---

## Summary of End Goal

We believe the correct architecture is:
1. Configure MOSI smart pin **once** at init (wrpin + wxpin + pinh)
2. For command/CRC bytes: just `wypin(data)` — smart pin shifts it out
3. For streamer bulk data: **silence** the smart pin output somehow, run the streamer, then **re-enable** the smart pin
4. Never tear down and rebuild the smart pin configuration

The TT bits look like the mechanism to achieve step 3. But we need to confirm:
- The streamer can drive a pin independently of TT output enable
- wrpin (to change TT) doesn't reset the smart pin
- Or if there's a better mechanism we haven't considered

---

## UPDATE: Fix Found and Verified (2026-02-16 evening)

### Question 2 — Answered

The silicon documentation confirms: **WXPIN/WYPIN on active smart pins (DIR=1) is explicitly designed behavior.** Only WRPIN requires a reset (DIR=0→1). This eliminates Q2 as a concern — our sp_transfer_8/sp_transfer_32 code is correct.

### Root Cause Identified: Clock-to-Data Timing

The 1-bit shift was caused by the first SCK rising edge arriving **before** the first streamer data bit was on MOSI. The card sampled the stale pin state (LOW from pinl) as an extra data bit.

**The original (buggy) TX streamer sequence:**
```pasm
xinit   stream_mode, #0          ' Start streamer; NCO begins from phase=0
wypin   clk_count, _sck          ' Start clock immediately after xinit
waitxfi
```

**Timing analysis (all times in sysclk cycles, relative to xinit execution):**

| Event | Time | Why |
|-------|------|-----|
| xinit executes | T = 0 | Starts NCO from phase 0 |
| wypin executes | T = 2 | Next instruction, 2 cycles after xinit |
| First SCK rising edge | T = 2 + spi_period | P_TRANSITION fires after its half-period |
| First streamer data bit on MOSI | T = 2 * spi_period | NCO must accumulate from $0000_0000 to $8000_0000 |

At 350 MHz sysclk / 25 MHz SPI: `spi_period = 7`

- First SCK rising edge: T = 2 + 7 = **9**
- First data bit on MOSI: T = 2 × 7 = **14**

**The clock arrives 5 cycles before the data.** The card samples MOSI at T=9 and gets whatever was there (LOW from pinl), which becomes an extra 0-bit injected before the real data stream. This produces the systematic 1-bit right-shift.

### Why RX Worked But TX Didn't

The RX path has the **opposite** order — clock starts first, then the streamer:

```pasm
wypin   clk_count, _sck          ' Start clock first
waitx   align_delay               ' Wait one half-period
xinit   stream_mode, init_phase   ' Start streamer after clock is running
waitxfi
```

This works because the card starts clocking data out on SCK edges, and the streamer begins capturing after the first edge has already occurred. The streamer doesn't need to **produce** data before the clock — it needs to **sample** data that the card is producing.

For TX, the P2 must have data on MOSI **before** the clock edge so the card can sample it. The original code started both simultaneously, and the NCO's ramp-up was slower than the clock's first transition.

### The Fix: Delay Clock Start by spi_period - 2

```pasm
xinit   stream_mode, #0          ' Start streamer; NCO ramps from zero
waitx   align_delay               ' Wait for NCO to place first data bit
wypin   clk_count, _sck          ' Start clock — now aligned with data
waitxfi
```

With `align_delay := spi_period - 2`:

| Event | Time | Why |
|-------|------|-----|
| xinit executes | T = 0 | Starts NCO from phase 0 |
| waitx begins | T = 2 | 2 instruction cycles after xinit |
| waitx completes | T = 2 + (spi_period - 2) = spi_period | |
| wypin executes | T = spi_period | Starts clock generation |
| First SCK rising edge | T = spi_period + spi_period = **2 * spi_period** | |
| First data bit on MOSI | T = **2 * spi_period** | NCO rollover |

**Perfect alignment.** The first data bit and first clock edge arrive at the same time. The card's setup time is satisfied because the data bit is stable on MOSI before the rising edge samples it (the NCO output transitions at the start of the period, the clock edge arrives later within that same period).

### Verification Results

Applied to both `writeSector()` (CMD24, single-sector) and `writeSectors()` (CMD25, multi-sector).

**SanDisk Industrial SA16G (the card that exposed the bug):**

| Test Suite | Before Fix | After Fix |
|---|---|---|
| Raw Sector (14 tests) | 8 pass | **14/14 PASS** |
| Multiblock (6 tests) | 3 pass | **6/6 PASS** |
| File Ops (22 tests) | 17 pass | **22/22 PASS** |
| Read/Write (29 tests) | 14 pass | **29/29 PASS** |
| Format (46 tests) | 38 pass | **46/46 PASS** |
| Mount (21 tests) | 21 pass | **21/21 PASS** |
| All 10 suites (236 tests) | 205 pass | **236/236 PASS** |

**Cross-card verification (no regressions):**

| Card | Controller | Raw Sector | Multiblock | Mount |
|------|-----------|-----------|-----------|-------|
| SanDisk Industrial SA16G 16GB | SanDisk $03 | 14/14 | 6/6 | 21/21 |
| Gigastone High Endurance 16GB | Budget OEM $00 | 14/14 | 6/6 | 21/21 |
| Lexar PLAY 128GB | Longsys $AD | 14/14 | 6/6 | 21/21 |

(Additional cards being tested)

### Questions for Designer Review

The fix works empirically, but we'd like the designer to validate our timing model and flag any concerns:

**Q5: Is the NCO timing model correct?**

We assume the streamer NCO (started by XINIT with phase=0) produces its first output bit at exactly `2 * spi_period` sysclks after XINIT, where `spi_period = $4000_0000 / xfrq`. Is this the correct model? Specifically:
- Does XINIT zero the phase accumulator synchronously (on the cycle it executes)?
- Does the NCO begin accumulating on the very next sysclk after XINIT?
- Is the first output bit driven when the accumulator first reaches/crosses $8000_0000?
- Are there any pipeline stages that add latency between the NCO rollover and the physical pin changing?

**Q6: Is there a hazard with the 2-instruction-cycle assumption?**

We account for xinit taking exactly 2 sysclk cycles (so waitx begins at T=2). Is this guaranteed for all PASM instructions in the cog? Could cog-hub contention, debug mode, or other factors add cycles to instruction execution?

**Q7: Does phase=0 vs phase=$4000_0000 affect streamer output quality?**

We use `init_phase = #0` (NCO ramps from zero). The previous developer tried `$4000_0000` (half-phase head start). With phase=0, the NCO has a full ramp-up period before the first output, which provides natural setup time. Is there any reason to prefer a non-zero initial phase for TX streaming?

**Q8: Streamer + P_TRANSITION interaction**

The clock pin uses P_TRANSITION mode. When we do `wypin(clk_count, sck)`, the P_TRANSITION smart pin starts counting transitions. Is there any interaction between the streamer and the P_TRANSITION smart pin? Specifically, does the streamer's NCO and the clock pin's transition counter run on independent timebases, or is there any synchronization mechanism? We're relying on them being independent but phase-aligned by our `waitx` delay.

**Q9: Is pinclear/pinl still the right transition?**

Before the streamer, we do:
```spin2
pinclear(_mosi)      ' WRPIN=0, DIR=0 — full smart pin teardown
pinl(_mosi)          ' DIR=1, OUT=0 — drive low for streamer
```

After the streamer, we rebuild:
```spin2
wrpin(_mosi, spi_tx_mode)    ' Reconfigure P_SYNC_TX
wxpin(_mosi, %1_00111)       ' 8-bit start-stop mode
pinh(_mosi)                  ' DIRH - enable smart pin
```

This full teardown/rebuild works with the timing fix. But if TT toggling (Q1) is viable, it would be cleaner. Even with the timing fix working, is there a preferred way to multiplex smart pin and streamer on the same pin?
