# Streamer SPI Timing Analysis

**Purpose**: Define exactly how to use the P2 streamer for SPI bulk transfers
**Status**: AUTHORITATIVE REFERENCE
**Created**: 2026-01-23
**Updates**: Corrects Decision 6 in ARCHITECTURE-DECISIONS.md

---

## Executive Summary

**The P2 streamer IS applicable for SPI serial transfers** when combined with a smart pin clock generator. This document corrects the earlier assertion that "Streamer Not Applicable for SPI" and provides the definitive timing analysis for implementing streamer-based sector reads and writes.

### Key Insight

The streamer operates bit-by-bit using its internal NCO (Numerically Controlled Oscillator). Combined with a smart pin generating the SPI clock, this creates a hardware SPI engine capable of ~50 MHz with zero CPU involvement during the transfer.

---

## Reference Implementation: flash_loader.spin2

Chip Gracey's `flash_loader.spin2` is the authoritative reference. It successfully uses the streamer for both reading and writing SPI flash at high speed.

### Clock Setup (One-Time Initialization)

```spin2
wrpin   #%01_00101_0,#spi_ck    ' P_TRANSITION mode with output enable
wxpin   #1,#spi_ck              ' 1 sysclk per transition (fastest possible)
drvl    #spi_ck                 ' Enable smart pin, start with pin LOW
```

**Key Constants:**
- `%01_00101` = P_TRANSITION mode (pin toggles on NCO overflow)
- Each `wypin` value specifies how many transitions to generate
- 2 transitions = 1 complete clock cycle (low→high→low)

### NCO Setup (Streamer Rate)

```spin2
clk2    long    $4000_0000      ' NCO value for sysclk/2 rate
setxfrq clk2                    ' Set streamer to sample/output at sysclk/2
```

**NCO Math:**
- The NCO is a 32-bit phase accumulator
- Each sysclk, the NCO adds the SETXFRQ value
- When the NCO overflows past $8000_0000, one streamer operation occurs
- Rate = sysclk × (XFRQ / $8000_0000)

For `$4000_0000`: Rate = sysclk × 0.5 = sysclk/2

---

## Critical Timing Difference: Read vs Write

### WRITE (Output) Sequence

```spin2
xinit   rmode, pa               ' Start streamer output FIRST
wypin   tranp, #spi_ck          ' Start clock SECOND
waitxfi                         ' Wait for completion
```

**Order: xinit → wypin**
**Reason**: Data should be present on MOSI before the first clock edge.

### READ (Input) Sequence

```spin2
wypin   x, #spi_ck              ' Start clock FIRST
waitx   #3                      ' Alignment delay
xinit   wmode, #0               ' Start capture SECOND
waitxfi                         ' Wait for completion
```

**Order: wypin → waitx → xinit**
**Reason**: Clock must be running when streamer samples the input pin.

The `waitx #3` delay aligns the first streamer sample with valid data on MISO.

---

## NCO Calculation for Different SPI Speeds

### The Formula

```
NCO_value = $4000_0000 / transition_period
```

Where `transition_period` is the wxpin value for the clock smart pin.

### Why This Works

The smart pin clock and streamer must operate at the same rate:
- Smart pin: transitions every `transition_period` sysclks
- Streamer: needs to sample/output once per bit = once per 2 transitions
- Therefore: streamer period = 2 × transition_period

For NCO to overflow every `2 × transition_period` sysclks:
```
$8000_0000 / NCO_value = 2 × transition_period
NCO_value = $8000_0000 / (2 × transition_period)
NCO_value = $4000_0000 / transition_period
```

### SD Card Speed Limits

Per SD specification:
- **Default Speed Mode**: Maximum 25 MHz
- **High Speed Mode**: Maximum 50 MHz

Our targets: **At or just under** these limits to stay within spec.

### Speed Table (320 MHz sysclk)

| Target | wxpin | Actual SPI | NCO Value | Notes |
|--------|-------|------------|-----------|-------|
| ~50 MHz | 4 | **40.0 MHz** | $1000_0000 | ★ Best for High Speed mode |
| (over) | 3 | 53.3 MHz | $1555_5555 | ✗ Exceeds 50 MHz limit |
| ~25 MHz | 7 | **22.9 MHz** | $0924_9249 | ★ Best for Default Speed mode |
| (over) | 6 | 26.7 MHz | $0AAA_AAAA | ✗ Exceeds 25 MHz limit |
| 20 MHz | 8 | 20.0 MHz | $0800_0000 | Safe fallback for problem cards |
| 16 MHz | 10 | 16.0 MHz | $0666_6666 | Conservative fallback |

**Recommended Settings at 320 MHz:**
- Default Speed: `wxpin #7` → 22.9 MHz (91.4% of 25 MHz limit)
- High Speed: `wxpin #4` → 40.0 MHz (80% of 50 MHz limit)

**Experimental (pending validation):**
- Over-spec Default: `wxpin #6` → 26.7 MHz (+7% if cards tolerate it)
- Over-spec High Speed: `wxpin #3` → 53.3 MHz (+7% if cards tolerate it)

See Task #2398 for planned over-spec testing.

### Speed Table (270 MHz sysclk)

| Target | wxpin | Actual SPI | NCO Value | Notes |
|--------|-------|------------|-----------|-------|
| ~50 MHz | 3 | **45.0 MHz** | $1555_5555 | ★ Best for High Speed mode |
| ~25 MHz | 6 | **22.5 MHz** | $0AAA_AAAA | ★ Best for Default Speed mode |
| (over) | 5 | 27.0 MHz | $0CCC_CCCC | ✗ Exceeds 25 MHz limit |
| 20 MHz | 7 | 19.3 MHz | $0924_9249 | Safe fallback |

**Recommended Settings at 270 MHz:**
- Default Speed: `wxpin #6` → 22.5 MHz (90% of 25 MHz limit)
- High Speed: `wxpin #3` → 45.0 MHz (90% of 50 MHz limit)

**Experimental (pending validation):**
- Over-spec Default: `wxpin #5` → 27.0 MHz (+8% if cards tolerate it)
- Note: 50 MHz over-spec not possible at 270 MHz (wxpin #2 would be 67.5 MHz, too high)

### Speed Calculation Function

```spin2
PRI calculateSPITiming(target_freq) : wxpin_val, nco_val | half_period
  '' Calculate wxpin and NCO values for target SPI frequency
  '' Ensures we stay AT OR UNDER the target (SD spec compliance)

  ' Half-period = sysclk / (2 × target_freq), rounded UP to stay under
  half_period := (clkfreq + target_freq * 2 - 1) / (target_freq * 2)

  ' Minimum half-period is 3 (practical limit)
  half_period := half_period #> 3

  wxpin_val := half_period
  nco_val := $4000_0000 / half_period

  ' Actual frequency = clkfreq / (2 × half_period)
  debug("Target: ", udec_(target_freq/1_000_000), " MHz")
  debug("Actual: ", udec_(clkfreq / (half_period * 2) / 1_000_000), " MHz")
  debug("wxpin: #", udec_(half_period), ", NCO: $", uhex_long_(nco_val))
```

---

## Alignment Delay Analysis

### Why `waitx #3` in flash_loader?

The flash_loader uses `wxpin #1` (1 sysclk per transition = sysclk/2 SPI clock).

Timeline with wxpin #1:
```
t=0: wypin instruction starts (2 cycles to execute)
t=2: wypin completes, smart pin receives transition count
t=3: first transition begins (LOW→HIGH = rising edge)
t=4: second transition (HIGH→LOW = falling edge)
t=5: xinit starts (after waitx #3)
t=7: xinit completes, streamer begins
t=9: first NCO overflow (2 sysclks after xinit at sysclk/2 rate)
```

The first sample at t=9 aligns with approximately bit 3's rising edge. The exact alignment depends on smart pin latency, which Chip empirically tuned with `waitx #3`.

### Scaling for Different Speeds

For `wxpin #N` (N sysclks per transition, 2N sysclks per bit):

**Naive scaling**: `waitx #(3 × N)` would preserve the same relative timing.

**Better approach**: Calculate the delay to position the first sample in the middle of the first bit's high period:
- Rising edge at t ≈ 2 + N (after wypin, first transition)
- High period lasts N sysclks
- Middle of high period at t ≈ 2 + N + N/2 = 2 + 1.5N
- First NCO overflow at t = delay + xinit_cycles + 2N
- Solve for delay

**Practical approach**: The tolerance window is larger for slower clocks (longer high period), so the exact delay is less critical. Start with `waitx #3` and adjust empirically if needed.

### For wxpin #7 (22.9 MHz SPI at 320 MHz sysclk)

```
Bit period = 14 sysclks
High period = 7 sysclks (plenty of margin)
Rising edges at: t=9, t=23, t=37, ... (relative to wypin end)

With waitx #3:
  xinit starts at t=5
  First NCO overflow at t=5 + 2 + 14 = t=21 (misses first bit!)

With waitx #7:
  xinit starts at t=9
  First NCO overflow at t=9 + 2 + 14 = t=25 (samples bit 1 at t=25, close to rising edge at t=23)
```

**Recommendation for wxpin #7**: Try `waitx #5` to `waitx #10`. The large tolerance window means several values may work.

---

## Initial Phase Offset (init_phase Parameter)

The `xinit` instruction's S operand can provide an initial phase offset:

```spin2
xinit   mode, init_phase
```

### When to Use Non-Zero Phase

For capture modes, setting `init_phase = $8000_0000 - NCO` causes the first sample to occur sooner (1 sysclk after xinit rather than 1 NCO period later).

**flash_loader uses `#0`** because at sysclk/2 rate, the alignment delay via `waitx` is sufficient.

For slower SPI speeds, a non-zero init_phase can help align the first sample:

```spin2
' For earlier first sample:
init_phase := $8000_0000 - xfrq   ' Sample 1 sysclk after xinit
```

---

## CRITICAL: Smart Pin Interference (CONFIRMED FIX)

### The Problem

When the streamer reads a pin in capture mode (X_1P_1DAC1_WFBYTE), it reads the **raw pin state**. If a smart pin is configured on that pin, it **interferes** with the streamer's direct pin reading.

**Symptom**: Captures `$00 $FF $FF...` instead of actual data.

### The Fix (Verified 2026-01-23)

Before streamer capture, **disable the MISO smart pin**:

```spin2
' Before streamer read - REQUIRED!
pinclear(miso)                  ' Clear smart pin mode
pinf(miso)                      ' Float pin (input mode)

' ... streamer capture (xinit, waitxfi) ...

' After streamer read, re-enable smart pin for subsequent byte transfers
wrpin(rx_mode, miso)            ' Reconfigure smart pin
wxpin(rx_config, miso)
dirh(miso)                      ' Re-enable
```

### Why This Works

- `pinclear()` removes the smart pin configuration
- `pinf()` sets the pin to floating input mode
- The streamer can now read the raw pin state directly
- After capture, smart pin can be re-enabled for command/response handling

### Test Evidence

From `SD_streamer_lut_test.spin2` on 2026-01-23:
```
Expected: $A0 $A0 $A0 $A0 $DE $AD $BE $EF
Actual:   $A0 $A0 $A0 $A0 $DE $AD $BE $EF
*** SUCCESS: Streamer captured correct data! ***
```

All 512 bytes of sector 100,000 captured correctly with this fix.

---

## Streamer Mode Words

### Capture Mode (Reading MISO → Hub)

```spin2
X_1P_1DAC1_WFBYTE = $C000_0000    ' Base mode: 1-pin input to WFBYTE
X_WRITE_ON        = $0080_0000    ' Enable WRFAST writes (CRITICAL!)
X_ALT_ON          = $0001_0000    ' MSB-first bit order

wmode := $C081_0000 | (miso_pin << 17) | bit_count
```

**Breaking down $C081_0000:**
- $C0xx_xxxx = 1-pin input mode (%1100 << 28)
- $xx8x_xxxx = X_WRITE_ON (enable hub writes)
- $xxx1_xxxx = X_ALT_ON (MSB first)
- Lower 12 bits = bit count (512 bytes × 8 bits = 4096 = $1000)

### Output Mode (Hub → MOSI)

```spin2
X_RFBYTE_1P_1DAC1 = $8000_0000    ' Base mode: RFBYTE to 1-pin output
X_PINS_ON         = $0080_0000    ' Enable pin output (same bit as X_WRITE_ON)
X_ALT_ON          = $0001_0000    ' MSB-first bit order

rmode := $8081_0000 | (mosi_pin << 17) | bit_count
```

---

## Complete Read Sector Implementation

```spin2
PRI readSector_streamer(sector) : result | xfrq, stream_mode, clk_count, init_phase
  '' Read 512 bytes using P2 streamer

  ' Send CMD17, wait for $FE token (use smart pins for this part)
  ' ... command/token handling ...

  ' === STREAMER TRANSFER ===

  ' 1. DISABLE MISO smart pin before streamer capture
  fltl    miso                              ' Reset smart pin

  ' 2. Calculate NCO for SPI speed
  xfrq := $4000_0000 / spi_half_period      ' e.g., $0924_9249 for wxpin #7

  ' 3. Build mode word
  stream_mode := $C081_0000 | (miso << 17) | (512 * 8)

  ' 4. Clock count: 512 bytes × 8 bits × 2 transitions
  clk_count := 512 * 8 * 2                  ' = 8192

  ' 5. Initial phase (0 or offset for timing adjustment)
  init_phase := 0                           ' Or: $8000_0000 - xfrq

  org
        setxfrq xfrq                        ' Set NCO rate
        wrfast  #0, p_buf                   ' Setup hub FIFO destination
        wypin   clk_count, sck              ' Start clock (FIRST for reads)
        waitx   #7                          ' Alignment delay (adjust empirically)
        xinit   stream_mode, init_phase     ' Start capture (SECOND for reads)
        waitxfi                             ' Wait for completion
  end

  ' 6. Reconfigure MISO smart pin if needed for subsequent operations
  wrpin   rx_mode, miso
  wxpin   rx_config, miso
  dirh    miso

  ' 7. Handle CRC bytes
  ' ...
```

---

## Complete Write Sector Implementation

```spin2
PRI writeSector_streamer(sector) : result | xfrq, stream_mode, clk_count
  '' Write 512 bytes using P2 streamer

  ' Send CMD24, send $FE token (use smart pins for this part)
  ' ... command/token handling ...

  ' === STREAMER TRANSFER ===

  ' For writes, smart pin on MOSI can stay active or be disabled
  ' Streamer drives the pin directly

  ' 1. Calculate NCO for SPI speed
  xfrq := $4000_0000 / spi_half_period

  ' 2. Build mode word
  stream_mode := $8081_0000 | (mosi << 17) | (512 * 8)

  ' 3. Clock count
  clk_count := 512 * 8 * 2

  org
        setxfrq xfrq                        ' Set NCO rate
        rdfast  #0, p_buf                   ' Setup hub FIFO source
        xinit   stream_mode, #0             ' Start output (FIRST for writes)
        wypin   clk_count, sck              ' Start clock (SECOND for writes)
        waitxfi                             ' Wait for completion
  end

  ' Handle CRC, response, busy-wait
  ' ...
```

---

## Debugging Checklist

When streamer transfers fail, verify:

1. **NCO Calculation**: `xfrq = $4000_0000 / wxpin_value`
2. **Mode Word**: Correct mode, pin position, bit count
3. **Order**: wypin → waitx → xinit (for reads), xinit → wypin (for writes)
4. **Smart Pin Interference**: MISO smart pin disabled before read
5. **FIFO Setup**: wrfast/rdfast before xinit
6. **X_WRITE_ON**: Bit 23 set for capture modes
7. **X_ALT_ON**: Bit 16 set for MSB-first SPI
8. **Alignment Delay**: Adjust waitx value if data is bit-shifted

### Symptoms and Likely Causes

| Symptom | Likely Cause |
|---------|--------------|
| All $00 | Smart pin interference, MISO stuck low |
| All $FF | Smart pin interference, MISO stuck high |
| $00 then $FF | Capture started before card began sending data |
| Bit-shifted data | Wrong alignment delay (waitx value) |
| Reversed bytes | X_ALT_ON not set, or set incorrectly |
| Partial data | Wrong bit count in mode word |
| Data at wrong address | FIFO pointer not set correctly |

---

## Correction to Decision 6

**ARCHITECTURE-DECISIONS.md Decision 6 ("Streamer Not Applicable for SPI")** is incorrect and should be updated:

### Corrected Decision 6

**The P2 Streamer IS applicable for SPI bulk transfers.** It provides:

1. **Zero CPU involvement** during 512-byte sector transfers
2. **Maximum throughput** - limited only by SPI clock speed
3. **DMA-like operation** - data streams directly to/from hub memory

The key is combining the streamer (for serial data) with a smart pin (for clock generation). This is the pattern used in `flash_loader.spin2` and should be used for SD card sector operations.

---

## References

- `REF/SPIN2-FlashLoader/flash_loader.spin2` - Chip Gracey's reference implementation
- P2 Silicon Documentation - Streamer section
- P2KB: `p2kbPasm2Xinit`, `p2kbSpin2StreamerSymbols`, `p2kbPasm2StreamerSmartpinControl`

---

*Document created: 2026-01-23*
*Status: AUTHORITATIVE REFERENCE*
