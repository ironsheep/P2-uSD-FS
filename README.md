# P2-SD-Card-Driver

A high-performance FAT32-compliant microSD card filesystem driver for the Parallax Propeller 2 (P2) microcontroller.

![Project Status](https://img.shields.io/badge/status-active-brightgreen)
![Platform](https://img.shields.io/badge/platform-Propeller%202-blue)
![License](https://img.shields.io/badge/license-MIT-green)

## Overview

This project provides a robust, high-performance SD card driver for the P2 microcontroller with full FAT32 filesystem support. The driver uses P2 smart pins for hardware-accelerated SPI communication and supports multiple simultaneous file handles.

## Features

- **FAT32 Filesystem Support**: Full read/write access to FAT32-formatted SD cards
- **High-Performance SPI**: Smart pin hardware acceleration with streamer DMA
- **Multi-File Handles**: Up to 4 files open simultaneously (3 read + 1 write)
- **Cross-OS Compatibility**: Works with cards formatted on Windows, macOS, and Linux
- **SDHC/SDXC Support**: Block-addressed cards up to 32GB (FAT32 limit)
- **CRC Validation**: Hardware-accelerated CRC-16 on all data transfers
- **Directory Operations**: Create, navigate, and enumerate directories
- **File Operations**: Create, open, read, write, seek, rename, delete
- **Multi-Cog Safe**: Dedicated worker cog with hardware lock serialization

## Hardware Requirements

- Parallax Propeller 2 (P2) microcontroller
- P2 Edge Module (recommended) or custom board
- microSD card slot with SPI interface
- FAT32-formatted SD card

### Default Pin Configuration (P2 Edge)

| Signal | Pin | Description |
|--------|-----|-------------|
| CS (DAT3) | P60 | Chip Select |
| MOSI (CMD) | P59 | Master Out, Slave In |
| MISO (DAT0) | P58 | Master In, Slave Out |
| SCK (CLK) | P61 | Serial Clock |

## Quick Start

### Basic File Operations

```spin2
OBJ
    sd : "SD_card_driver"

CON
    SD_CS   = 60
    SD_MOSI = 59
    SD_MISO = 58
    SD_SCK  = 61

PUB main() | handle, buffer[128], bytes_read
    ' Mount the SD card
    if not sd.mount(SD_CS, SD_MOSI, SD_MISO, SD_SCK)
        debug("Mount failed!")
        return

    ' Read a file using handle-based API
    handle := sd.openFileRead(@"CONFIG.TXT")
    if handle >= 0
        bytes_read := sd.readHandle(handle, @buffer, 512)
        sd.closeFileHandle(handle)
        debug("Read ", udec(bytes_read), " bytes")

    ' Create and write a file
    handle := sd.createFileNew(@"OUTPUT.TXT")
    if handle >= 0
        sd.writeHandle(handle, @"Hello, P2!", 10)
        sd.closeFileHandle(handle)

    ' Unmount when done
    sd.unmount()
```

### Multi-File Example (Copy File)

```spin2
PUB copyFile(src_name, dest_name) | src_h, dest_h, buf[128], bytes
    src_h := sd.openFileRead(src_name)
    dest_h := sd.createFileNew(dest_name)

    if src_h >= 0 and dest_h >= 0
        repeat
            bytes := sd.readHandle(src_h, @buf, 512)
            if bytes == 0
                quit
            sd.writeHandle(dest_h, @buf, bytes)

        sd.closeFileHandle(src_h)
        sd.closeFileHandle(dest_h)
```

### Directory Navigation

```spin2
PUB listDirectory() | entry, p_entry
    entry := 0
    repeat
        p_entry := sd.readDirectory(entry++)
        if p_entry == 0
            quit
        if sd.attributes() & $10
            debug("[DIR]  ", zstr(sd.fileName()))
        else
            debug("[FILE] ", zstr(sd.fileName()), " ", udec(sd.fileSize()), " bytes")
```

## Documentation

- **[Driver Tutorial](DOCs/SD-Card-Driver-Tutorial.md)** - Complete guide with practical examples
- **[Regression Testing](REGRESSION-TESTING.md)** - Test infrastructure and validation
- **[Card Catalog](DOCs/CARD-CATALOG.md)** - Tested SD cards with performance data

## Project Structure

```
P2-SD-Card-Driver/
├── src/                        # Driver and utilities
│   ├── SD_card_driver.spin2        # The SD card driver
│   ├── UTILS/                      # Utility programs
│   │   ├── SD_format_utility.spin2     # FAT32 formatter
│   │   ├── SD_card_characterize.spin2  # Card register reader
│   │   ├── SD_speed_characterize.spin2 # Speed testing
│   │   └── SD_FAT32_audit.spin2        # Filesystem validator
│   └── DEMO/                       # Demo application
│       └── SD_demo_shell.spin2         # Interactive terminal shell
│
├── regression-tests/           # Test suite
│   ├── SD_RT_*.spin2               # Test files
│   ├── SD_RT_utilities.spin2       # Test framework
│   └── TestCard/                   # Test card validation
│
├── tools/                      # Build and test scripts
│   └── run_test.sh                 # Test runner
│
└── DOCs/                       # Documentation
```

## Performance

Typical performance at 270 MHz sysclk with 25 MHz SPI:

| Operation | Throughput |
|-----------|------------|
| Sequential Read | 200-300 KB/sec |
| Sequential Write | 150-250 KB/sec |
| Multi-sector Read | 400+ KB/sec |

Performance varies by card. See [Card Catalog](DOCs/CARD-CATALOG.md) for tested cards.

## API Overview

### Mounting
| Method | Description |
|--------|-------------|
| `mount(cs, mosi, miso, sck)` | Initialize and mount SD card |
| `unmount()` | Sync and unmount SD card |

### File Operations (Handle-Based)
| Method | Description |
|--------|-------------|
| `openFileRead(pPath)` | Open file for reading, returns handle |
| `openFileWrite(pPath)` | Open file for writing, returns handle |
| `createFileNew(pPath)` | Create new file, returns handle |
| `readHandle(handle, pBuf, count)` | Read bytes from file |
| `writeHandle(handle, pBuf, count)` | Write bytes to file |
| `seekHandle(handle, pos)` | Set file position |
| `closeFileHandle(handle)` | Close file handle |

### Directory Operations
| Method | Description |
|--------|-------------|
| `changeDirectory(pPath)` | Navigate to directory |
| `newDirectory(pName)` | Create new directory |
| `readDirectory(entry)` | Enumerate directory entries |
| `deleteFile(pName)` | Delete file or empty directory |
| `rename(pOld, pNew)` | Rename file or directory |

### Information
| Method | Description |
|--------|-------------|
| `freeSpace()` | Get free space in sectors |
| `volumeLabel()` | Get volume label string |
| `fileSize()` | Get size of open file |
| `error()` | Get last error code |

## Utilities

The `src/UTILS/` folder contains standalone utility programs:

- **SD_format_utility.spin2** - Format SD cards with FAT32
- **SD_card_characterize.spin2** - Read and display card registers (CID, CSD, SCR)
- **SD_speed_characterize.spin2** - Test maximum reliable SPI speed
- **SD_FAT32_audit.spin2** - Validate FAT32 filesystem structure
- **SD_performance_benchmark.spin2** - Measure read/write throughput

## Demo Shell

The `src/DEMO/SD_demo_shell.spin2` provides an interactive terminal interface:

```
SD:/> help
  mount        Mount SD card
  dir          List directory (alias: ls)
  cd <path>    Change directory
  type <file>  Display text file (alias: cat)
  copy <s> <d> Copy file (alias: cp)
  del <file>   Delete file (alias: rm)
  stats        Show filesystem statistics
  card         Show card identification
  benchmark    Quick speed test
```

Connect at 230,400 baud to use the shell.

## Testing

See [REGRESSION-TESTING.md](REGRESSION-TESTING.md) for complete test documentation.

Run tests from the `tools/` directory:

```bash
cd tools/
./run_test.sh ../regression-tests/SD_RT_mount_tests.spin2
./run_test.sh ../regression-tests/SD_RT_file_ops_tests.spin2
./run_test.sh ../regression-tests/SD_RT_read_write_tests.spin2
```

## Known Limitations

- 8.3 filename format (no long filename support)
- FAT32 only (no FAT12/FAT16/exFAT)
- SPI mode only (no SD native mode)
- Maximum 32GB cards (FAT32 specification limit)

## Credits

- **Original Driver Concept**: Chris Gadd (Parallax OBEX)
- **Driver Development**: Stephen M. Moraco, Iron Sheep Productions
- **Architecture & Testing**: Claude (Anthropic)

## License

MIT License - See [LICENSE](LICENSE) for details.

## Author

Stephen M. Moraco
Iron Sheep Productions, LLC
stephen@ironsheep.biz

---

*Part of the Iron Sheep Productions Propeller 2 Projects Collection*
