# SD Card SPI Compatibility Guide

This document captures research on microSD card SPI compatibility for embedded systems, specifically for use with the P2 SD Card Driver.

---

## Executive Summary

All microSD generations (microSD, microSDHC, microSDXC, microSDUC) are **required by the SD Physical Layer spec to support SPI mode**, but the quality of SPI implementations varies significantly by vendor and even by model/lot.

For robust SPI use:
- Treat "supports SPI" as a given at the standard level
- Choose conservative card types and specific brands/models with a track record of behaving well in SPI and embedded environments

---

## SD Card Generations and Capacities

The SD Association defines four main card families distinguished by capacity, not bus type:

| Generation | Capacity | Filesystem | Notes |
|------------|----------|------------|-------|
| **microSD (SDSC)** | Up to 2 GB | FAT12/16 | Standard Capacity |
| **microSDHC** | 2-32 GB | FAT32 | High Capacity |
| **microSDXC** | 32 GB - 2 TB | exFAT | Extended Capacity |
| **microSDUC** | 2-128 TB | exFAT | Ultra Capacity (rare) |

### Bus Modes

Cards can implement multiple bus modes:

| Bus Mode | Max Speed | Notes |
|----------|-----------|-------|
| Default / High-speed SD | 25 MB/s | Standard SD bus |
| UHS-I | 104 MB/s | Common on modern cards |
| UHS-II | 312 MB/s | Extra contacts, not on microSD |
| UHS-III / SD Express | Higher | Niche, no SPI benefit |

**Important:** The presence of UHS or newer features does NOT remove SPI support. SPI is an alternate protocol that must still be implemented for a device to carry the SD logo.

---

## Speed Class Markings

Speed logos describe minimum sequential write performance on the SD bus, **not SPI mode performance**:

| Marking | Minimum Write Speed |
|---------|---------------------|
| Speed Class C2, C4, C6, C10 | 2/4/6/10 MB/s |
| UHS Speed Class U1 | 10 MB/s |
| UHS Speed Class U3 | 30 MB/s |
| Video Speed Class V6-V90 | 6-90 MB/s |

### For SPI Use on Microcontrollers

- Any **C10 / U1** card is usually adequate
- The SPI bus itself will be the bottleneck
- Higher UHS/Video classes bring **no real benefit in SPI mode**
- Higher classes can imply more complex controllers and power behavior, which may be **less predictable** in embedded designs

---

## SPI Mode Support and Quirks

The microSD spec explicitly defines SPI as an alternate bus mode. Production cards normally support both SD mode and SPI mode.

### SPI Bus Properties

- Uses 4-wire interface: CS, SCK, MOSI, MISO
- Uses a subset of SD commands
- Some details (CRC behavior, command framing) differ from SD mode
- Bus rates far below SD/UHS maximums
- Typical MCU designs run under 25 MHz, often firmware-limited

### Common SPI-Specific Pitfalls

#### 1. Initialization Differences
- Cards can require **many CMD55+ACMD41 retries** before leaving idle
- Some need **extra dummy clocks** between commands
- Timing-sensitive during power-up sequence

#### 2. CRC Behavior
- CRC is supposed to be off by default in SPI (except CMD0/CMD8)
- **Some cards ship with CRC checking enabled**
- This breaks "bare" drivers that never compute CRC7/CRC16

#### 3. Voltage and Power-Up Timing
- Cards can be sensitive to ramp time and minimum off-times
- Marginal sockets or slow rise can cause "random SPI failures"

#### 4. Partial or Buggy Implementations
- Cheaper cards sometimes have insufficiently tested SPI microcode
- May pass phone/PC tests (SDIO mode) but misbehave under:
  - Long-running SPI workloads
  - Strange power cycles
  - Nonstandard command sequences

**Key insight:** Because SDIO hosts (phones, PCs, cameras) are the primary customer, some vendors pay less attention to SPI corner cases.

---

## Brand/Vendor Reliability for SPI

There is no single authoritative "SPI compatibility list", but trends emerge from endurance testing and embedded-user experience. **Always test a specific model/lot in your board.**

### Generally Recommended for SPI/Embedded

| Brand | Notes |
|-------|-------|
| **SanDisk** | Very commonly recommended for MCU and SPI-based devices. Multiple projects explicitly recommend SanDisk. |
| **Kingston** | Recent testing shows improved reliability (1 failure in 15 cards). Industrial lines rated strong. |
| **PNY** | Endurance-oriented tests show good performance in some models. |
| **Transcend** | Selected models perform well, especially industrial lines. |
| **Delkin** | Industrial/embedded series designed for harsh environments. |

### Mixed or Negative Reports

| Brand/Type | Issues |
|------------|--------|
| **Kingston older/consumer** | "Weak" compared to SanDisk in some markets, controller changes between lots. |
| **Silicon Power** | High failure rates in endurance testing. |
| **onn. (Walmart)** | Early sector error thresholds. |
| **Gigastone** | Some models showed issues in testing. |
| **Kioxia** (some models) | Variable results. |
| **XrayDisk** | High failure rates reported. |
| **No-name/cheap cards** | Highly variable, SPI often poorly validated. |

**Note:** Reliability here refers mostly to data integrity and endurance, but those characteristics correlate strongly with SPI embedded use survival (frequent power cycles, brownouts, nonstandard access patterns).

---

## Practical Selection Strategy for SPI Designs

For Propeller 2 and other MCU-based SPI hosts:

### Card Type Choices

| Recommendation | Reason |
|----------------|--------|
| **Prefer microSDHC (4-32 GB)** | Widely supported, avoids old SDSC corner cases |
| **Small microSDXC OK** | Well supported in modern drivers |
| **Avoid SD Express / SDUC** | No SPI benefit, adds complexity |

### Brand and Model Strategy

1. **Use major brands** with stable controller lines:
   - SanDisk Ultra/Extreme
   - Kingston Industrial
   - Transcend Industrial
   - Delkin Embedded/Industrial

2. **Standardize on one or two exact model numbers**

3. **Validate under worst-case conditions:**
   - Your actual SPI workload
   - Power cycling scenarios
   - Temperature extremes if applicable

### Driver and Hardware Best Practices

#### Initialization
- Implement full SPI initialization flow:
  - CMD0 (GO_IDLE_STATE)
  - CMD8 (SEND_IF_COND)
  - Repeated CMD55+ACMD41
  - CMD58 (READ_OCR)
  - Proper timeouts
  - Adequate dummy clocks

- Optionally implement CRC7/CRC16 to handle cards with CRC enabled

#### Clock Speed
- Keep SPI clock **modest during init** (100-400 kHz)
- Only raise clock after card leaves idle state
- Stay within conservative margin (e.g., ≤20 MHz) for long-running operation

#### Power Design
- Provide clean power-up sequence
- Full power-off between card swaps if applicable
- Adequate decoupling capacitors near card socket

---

## P2 SD Card Driver: Tested Cards

### Verified Working

| Card | Capacity | Class | Board | Status |
|------|----------|-------|-------|--------|
| GigaStone | 32 GB | SDHC | P2 Edge | Full pass (172 tests) |
| GigaStone | 64 GB | SDXC | P2 EC32 Mag | Full pass (172 tests) |

### Known Issues

| Card | Capacity | Board | Issue |
|------|----------|-------|-------|
| PNY | Unknown | P2 EC32 Mag | CMD0 returns $00 instead of $01 |

The PNY issue may be related to:
- Card-specific initialization timing requirements
- Board electrical characteristics
- Card lot/firmware variation

---

## References

- SD Physical Layer Simplified Specification
- SD Card endurance testing community reports
- Embedded systems community experience

---

## Document History

| Date | Change |
|------|--------|
| 2026-01-16 | Initial research compilation |

---

*This document is part of the P2-uSD-FileSystem project.*
