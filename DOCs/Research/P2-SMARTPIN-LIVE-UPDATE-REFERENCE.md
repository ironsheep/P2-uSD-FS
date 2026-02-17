# P2 Smart Pin Live Update Rules — Definitive Reference

**Source:** Silicon documentation (line 7855-7857), compiled for P2KB 2026-02-16

## The Core Rule

> "A smart pin should be configured while its DIR bit is low, holding it in reset.
> During that time, WRPIN/WXPIN/WYPIN can be used to establish the mode and related
> parameters. Once configured, DIR can be raised high and the smart pin will begin
> operating. After that, depending on the mode, you may feed it new data via
> WXPIN/WYPIN or retrieve results using RDPIN/RQPIN."

## What Needs Reset (DIR LOW) vs. What Doesn't

### WRPIN — Requires DIR=0 (reset)

Any change to the WRPIN configuration register requires the smart pin to be in reset:
- Changing the smart pin mode (%SSSSS bits)
- Changing input routing (AAAA, BBBB)
- Changing filtering (FFF)
- Changing TT bits or M bits
- **Any** WRPIN change

### WXPIN — Safe While Running (DIR=1)

WXPIN can be used while the smart pin is active. Effects are mode-specific:

| Mode | WXPIN While Running |
|------|-------------------|
| Pulse (%00100) | Changes base period/compare |
| Transition (%00101) | Changes base period |
| NCO freq (%00110) | Updates base period + sets Z phase |
| NCO duty (%00111) | Updates base period + sets Z phase |
| PWM tri/saw (%01000-01001) | Changes base period and frame |
| PWM SMPS (%01010) | Changes base period and frame |
| Counters (%01011-01111) | Changes measurement period |
| Timing (%10000-10111) | Changes measurement window |
| ADC (%11000-11001) | Changes sample mode/period |
| **Sync TX (%11100)** | **Changes update mode / bit count** |
| **Sync RX (%11101)** | **Changes sample position / bit count** |
| Async TX (%11110) | Changes baud rate / bit count |
| Async RX (%11111) | Changes baud rate / bit count |

### WYPIN — Safe While Running (DIR=1)

WYPIN is the **primary interaction method** for running smart pins:

| Mode | WYPIN While Running |
|------|-------------------|
| Pulse (%00100) | Triggers new pulse sequence |
| Transition (%00101) | Triggers new transitions |
| NCO freq (%00110) | Updates frequency (Y added to Z each period) |
| NCO duty (%00111) | Updates duty |
| PWM tri/saw (%01000-01001) | Updates duty (captured at frame start) |
| PWM SMPS (%01010) | Updates duty |
| Counters (%01011-01111) | Some use Y[0] for mode selection |
| Timing (%10000-10111) | Some use Y[1:0] for sensitivity |
| **Sync TX (%11100)** | **Loads next word to transmit** |
| Sync RX (%11101) | — |
| Async TX (%11110) | Loads next word to transmit |
| Async RX (%11111) | — |

## Important Side Effect: Acknowledgment

Every WRPIN, WXPIN, WYPIN, RDPIN, and AKPIN **acknowledges** the smart pin (clears IN).

From silicon doc line 7834:
> "A cog acknowledges a smart pin whenever it does a WRPIN, WXPIN, WYPIN, RDPIN
> or AKPIN on it. This causes the smart pin to lower its IN signal."

This means:
- Writing a new X or Y value has the side effect of clearing any pending IN flag
- This is by design — it's how the handshake works
- You cannot casually write WXPIN/WYPIN without also acknowledging any pending IN

## Caution: WXPIN on Serial Modes

Changing WXPIN on a serial mode (Sync TX/RX, Async TX/RX) while **actively transmitting/receiving** would change the bit count mid-stream. The silicon doc doesn't explicitly warn against this, but it would corrupt any in-progress transfer.

**Best practice:** Change WXPIN **between transactions** — after IN signals completion of the current transfer, before loading the next WYPIN.

## Relevance to SPI Driver

Our `sp_transfer_8()` does:
```pasm
wxpin   #$27, _mosi       ' 8 bits (%1_00111)
wypin   tx_data, _mosi     ' Load data to transmit
drvl    _mosi              ' Enable smart pin (or keep enabled)
```

Our `sp_transfer_32()` does:
```pasm
wxpin   #$3F, _mosi       ' 32 bits (%1_11111)
wypin   tx_data, _mosi     ' Load data to transmit
drvl    _mosi              ' Enable smart pin
```

**Per the silicon doc, this is CORRECT behavior.** WXPIN and WYPIN on a running smart pin (DIR=1) are explicitly supported. The 32→8 bit transition in `cmd()` is safe — WXPIN changes the bit count for the NEXT transfer.

**The caution applies:** We should ensure the previous transfer has completed (IN flagged) before changing WXPIN. Since we wait for MISO IN (receive complete) before returning from each sp_transfer call, the MOSI transfer is also complete (same clock drives both). The next call's WXPIN is between transactions. This is safe.

**What is NOT safe while running:** Changing WRPIN — which includes TT bits, smart pin mode, input routing. Any WRPIN change requires DIR=0 (reset) first.
