# SD Card SPI Mode Implementation Reference

Extracted from: SD Physical Layer Simplified Specification Version 9.10

---

## 1. Pin Assignments (SPI Mode)

| Pin # | SD Mode | SPI Mode | Type | Description |
|-------|---------|----------|------|-------------|
| 1 | CD/DAT3 | CS | I | Chip Select (active low) |
| 2 | CMD | DI | I | Data In (MOSI) |
| 3 | VSS1 | VSS | S | Ground |
| 4 | VDD | VDD | S | Supply voltage (2.7-3.6V) |
| 5 | CLK | SCLK | I | Clock |
| 6 | VSS2 | VSS | S | Ground |
| 7 | DAT0 | DO | O | Data Out (MISO) |
| 8 | DAT1 | RSV | - | Reserved (directly to VDD) |
| 9 | DAT2 | RSV | - | Reserved (directly to VDD) |

**Note**: Pin 1 has a 50K pull-up resistor in the card. Drive CS LOW during CMD0 to select SPI mode.

---

## 2. Power-Up Sequence

```
1. Power off: VDD < 0.5V for at least 1ms
2. Power ramp: 0.1ms to 35ms to reach operating voltage (2.7-3.6V)
3. Wait: 1ms after VDD stable
4. Clock: Provide at least 74 clock cycles with CS HIGH
5. CMD0: Send GO_IDLE_STATE with CS LOW to enter SPI mode
```

---

## 3. SPI Mode Initialization Flow

```
Power On
    |
    v
Send 74+ clocks with CS=HIGH (card powers up in SD mode)
    |
    v
CMD0 + CS=LOW  -----> Card enters SPI mode, returns R1
    |
    v
CMD8 (SEND_IF_COND) with VHS=0x01, check_pattern=0xAA
    |
    +---> Illegal command? --> Ver 1.x card (SDSC only)
    |
    +---> Valid response echoes check pattern? --> Ver 2.0+ card
    |
    v
[Optional] CMD58 (READ_OCR) to verify voltage range
    |
    v
ACMD41 (SD_SEND_OP_COND) with HCS=1 (if host supports HC)
    |
    +---> Response has "in_idle_state"=1? --> Repeat ACMD41
    |
    +---> Response has "in_idle_state"=0? --> Card ready!
    |
    v
CMD58 (READ_OCR) to check CCS bit
    |
    +---> CCS=0: SDSC (Standard Capacity) - uses byte addressing
    +---> CCS=1: SDHC/SDXC (High/Extended Capacity) - uses block addressing
    |
    v
Card initialized and ready for data transfer
```

---

## 4. Command Format (6 bytes)

```
Bit:    47    46    [45:40]    [39:8]     [7:1]    0
        |     |       |          |          |      |
       '0'   '1'   cmd_idx   argument     CRC7   '1'
      start  tx                                  end
```

All commands are 6 bytes, MSB first:
- Byte 0: `0x40 | command_index` (start bit + transmission bit + cmd)
- Bytes 1-4: 32-bit argument (MSB first)
- Byte 5: `(CRC7 << 1) | 0x01` (CRC + end bit)

---

## 5. Essential Commands for SPI Mode

### CMD0 - GO_IDLE_STATE (Reset)
```
TX: 0x40, 0x00, 0x00, 0x00, 0x00, 0x95
Response: R1
```
**CRC is required** even in CRC-off mode (card still in SD mode when receiving CMD0).

### CMD8 - SEND_IF_COND (Check voltage)
```
Argument: [31:12]=0, [11:8]=VHS (0x01 for 2.7-3.6V), [7:0]=check_pattern (0xAA)
TX: 0x48, 0x00, 0x00, 0x01, 0xAA, 0x87
Response: R7 (5 bytes: R1 + 4 bytes echoing VHS and check_pattern)
```
**CRC is required** for CMD8.

### CMD58 - READ_OCR
```
TX: 0x7A, 0x00, 0x00, 0x00, 0x00, 0xFD
Response: R3 (5 bytes: R1 + 32-bit OCR)
```

### CMD55 - APP_CMD (Prefix for ACMD)
```
TX: 0x77, 0x00, 0x00, 0x00, 0x00, 0x65
Response: R1
```
Must precede every ACMD command.

### ACMD41 - SD_SEND_OP_COND (Initialize)
```
Argument: [30]=HCS (set to 1 for SDHC/SDXC support), [23:0]=voltage window
For SDHC support: 0x40000000
TX: 0x69, 0x40, 0x00, 0x00, 0x00, 0x77
Response: R1
```
Repeat until R1 bit 0 (in_idle_state) = 0.

### CMD9 - SEND_CSD
```
TX: 0x49, 0x00, 0x00, 0x00, 0x00, 0xAF
Response: R1 + 16-byte CSD + 2-byte CRC
```

### CMD10 - SEND_CID
```
TX: 0x4A, 0x00, 0x00, 0x00, 0x00, 0x1B
Response: R1 + 16-byte CID + 2-byte CRC
```

### CMD16 - SET_BLOCKLEN (SDSC only)
```
Argument: block length (typically 512)
TX: 0x50, 0x00, 0x00, 0x02, 0x00, 0x15
Response: R1
```
Not needed for SDHC/SDXC (always 512 bytes).

### CMD17 - READ_SINGLE_BLOCK
```
Argument: address (byte address for SDSC, block address for SDHC/SDXC)
Response: R1, then wait for data token (0xFE), then 512 bytes + 2-byte CRC
```

### CMD18 - READ_MULTIPLE_BLOCK
```
Same as CMD17, but continues sending blocks until CMD12 (STOP_TRANSMISSION)
```

### CMD24 - WRITE_BLOCK
```
Argument: address
Response: R1, then host sends: Start token (0xFE) + 512 bytes + 2-byte CRC
Card responds with data response token, then goes busy (DO=LOW)
```

### CMD25 - WRITE_MULTIPLE_BLOCK
```
Same as CMD24, but use token 0xFC for each block, 0xFD to stop
```

### CMD12 - STOP_TRANSMISSION
```
TX: 0x4C, 0x00, 0x00, 0x00, 0x00, 0x61
Response: R1b (R1 + busy signal)
```

---

## 6. Response Formats

### R1 (1 byte)
```
Bit 7: Always 0
Bit 6: Parameter error
Bit 5: Address error
Bit 4: Erase sequence error
Bit 3: CRC error
Bit 2: Illegal command
Bit 1: Erase reset
Bit 0: In idle state (1 = initializing, 0 = ready)
```
**R1 = 0x00** means success and ready.
**R1 = 0x01** means success but still initializing.

### R1b (R1 + Busy)
Same as R1, followed by busy signal (DO held LOW while busy).

### R2 (2 bytes) - Status
First byte = R1, second byte = additional status flags.

### R3 (5 bytes) - OCR
First byte = R1, next 4 bytes = OCR register.

### R7 (5 bytes) - Interface Condition
First byte = R1, next 4 bytes = command version + voltage accepted + check pattern echo.

---

## 7. OCR Register (32 bits)

| Bit | Field |
|-----|-------|
| 31 | Card power up status (1=ready, 0=busy) |
| 30 | CCS - Card Capacity Status (0=SDSC, 1=SDHC/SDXC) |
| 29 | UHS-II status |
| 24 | S18A - 1.8V switching accepted |
| 23 | 3.5-3.6V |
| 22 | 3.4-3.5V |
| 21 | 3.3-3.4V |
| 20 | 3.2-3.3V |
| 19 | 3.1-3.2V |
| 18 | 3.0-3.1V |
| 17 | 2.9-3.0V |
| 16 | 2.8-2.9V |
| 15 | 2.7-2.8V |
| 14-0 | Reserved |

---

## 8. Data Tokens

### Start Block Token (Single block read/write, Multiple block read)
```
0xFE = 11111110
```

### Start Block Token (Multiple block write)
```
0xFC = 11111100
```

### Stop Tran Token (Multiple block write)
```
0xFD = 11111101
```

### Data Response Token (After write)
```
Bits [4:1] = Status:
  010 = Data accepted
  101 = Data rejected (CRC error)
  110 = Data rejected (write error)
```

### Data Error Token (Read error)
```
0x0E or similar (bits 0-3 indicate error type)
Bit 0: Error
Bit 1: CC Error
Bit 2: Card ECC Failed
Bit 3: Out of Range
```

---

## 9. Data CRC

Data blocks use CRC-16-CCITT: polynomial x^16 + x^12 + x^5 + 1

---

## 10. Timing Notes (from spec)

**The detailed timing values are REMOVED from the simplified specification.**

General guidelines:
- After sending a command, poll DO for response (will see 0xFF while waiting)
- Response comes within NCR clock cycles (typically 0-8 bytes of 0xFF)
- After write, card holds DO LOW while busy - poll until DO goes HIGH
- Use conservative delays between commands during development

---

## 11. Clock Speeds

| Mode | Max Frequency |
|------|---------------|
| Identification (init) | 100-400 kHz recommended |
| Default Speed | 25 MHz |
| High Speed | 50 MHz |

**Start at low speed (100-400 kHz) for initialization, then switch to higher speed after card is ready.**

---

## 12. Addressing

| Card Type | Address Unit |
|-----------|--------------|
| SDSC (≤2GB) | Byte address (multiply block number by 512) |
| SDHC (2-32GB) | Block address (block number directly) |
| SDXC (32GB-2TB) | Block address |

---

## 13. Quick Reference: Minimal Init Sequence

```
1. Power up, wait 1ms
2. Send 80+ clock pulses with CS=HIGH, DI=HIGH
3. CS=LOW
4. CMD0 (0x40,0x00,0x00,0x00,0x00,0x95) -> expect R1=0x01
5. CMD8 (0x48,0x00,0x00,0x01,0xAA,0x87) -> expect R7 echoing 0x01,0xAA
6. Loop:
   CMD55 (0x77,0x00,0x00,0x00,0x00,0x65) -> R1
   ACMD41 (0x69,0x40,0x00,0x00,0x00,0x77) -> R1
   Until R1=0x00
7. CMD58 -> Read CCS bit to determine addressing mode
8. [SDSC only] CMD16 to set block size to 512
9. Ready for read/write!
```

---

## 14. Important Notes for SPI Mode

1. **CRC is OFF by default** after entering SPI mode (except CMD0 and CMD8 which always need valid CRC)
2. **SDUC cards do NOT support SPI mode**
3. **Speed Class is not guaranteed** in SPI mode - treat as Class 0
4. **Block length is fixed to 512 bytes** for SDHC/SDXC
5. **Write protect commands (CMD28,29,30) not supported** for SDHC/SDXC
6. **Boot functionality not supported** in SPI mode
7. **No UHS-I high-speed modes** in SPI mode
