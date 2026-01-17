# P2-uSD-FileSystem

A FAT32-compliant microSD card filesystem driver for the Parallax Propeller 2 (P2) microcontroller.

![Project Status](https://img.shields.io/badge/status-development-orange)
![Platform](https://img.shields.io/badge/platform-Propeller%202-blue)
![License](https://img.shields.io/badge/license-MIT-green)

## Overview

This project provides a robust SD card driver for the P2 microcontroller with full FAT32 filesystem support. The driver is based on Chris Gadd's OB4269 SD card driver from the Parallax OBEX, enhanced with FAT32 compliance fixes and comprehensive testing.

## Features

- **FAT32 Filesystem Support**: Full read/write access to FAT32-formatted SD cards
- **Cross-OS Compatibility**: Works with cards formatted on Windows, macOS, and Linux
- **SDHC/SDXC Support**: Block-addressed cards up to 2TB
- **Directory Operations**: Create, navigate, and enumerate directories
- **File Operations**: Create, open, read, write, seek, rename, delete
- **Multi-cluster Support**: Handles files larger than cluster size
- **Dual FAT Mirroring**: Proper FAT32 compliance with both FAT copies
- **FSInfo Sector Updates**: Maintains free cluster count for fast mounting

## Hardware Requirements

- Parallax Propeller 2 (P2) microcontroller
- P2 Edge Module (recommended) or custom board
- microSD card slot with SPI interface

### Default Pin Configuration (P2 Edge)

| Signal | Pin | Description |
|--------|-----|-------------|
| CS (DAT3) | P60 | Chip Select |
| MOSI (CMD) | P59 | Master Out, Slave In |
| MISO (DAT0) | P58 | Master In, Slave Out |
| SCK (CLK) | P61 | Serial Clock |

## Quick Start

### Basic Usage

```spin2
OBJ
    sd : "SD_card_driver"

CON
    SD_CS   = 60
    SD_MOSI = 59
    SD_MISO = 58
    SD_SCK  = 61

PUB main() | result
    ' Mount the SD card
    result := sd.mount(SD_CS, SD_MOSI, SD_MISO, SD_SCK)
    if result == false
        debug("Mount failed!")
        return

    ' Create and write a file
    sd.newFile(@"TEST.TXT")
    sd.writeString(@"Hello, P2!")
    sd.closeFile()

    ' Unmount when done
    sd.unmount()
```

### Reading Files

```spin2
PUB readExample() | buffer[128], bytesRead
    sd.openFile(@"DATA.TXT")
    bytesRead := sd.read(@buffer, 128)
    sd.closeFile()
```

### Directory Navigation

```spin2
PUB directoryExample() | result, pEntry
    ' Change to subdirectory
    sd.changeDirectory(@"LOGS")

    ' Create new directory
    sd.newDirectory(@"2026")

    ' List directory contents
    repeat
        pEntry := sd.readDirectory(idx++)
        if pEntry == 0
            quit
        debug("File: ", zstr_(sd.fileName()))
```

## API Reference

See [OB4269-API-Reference.md](OB4269-API-Reference.md) for complete API documentation.

### Key Methods

| Method | Description |
|--------|-------------|
| `mount(cs, mosi, miso, sck)` | Initialize and mount SD card |
| `unmount()` | Sync and unmount SD card |
| `openFile(pFilename)` | Open existing file for reading |
| `newFile(pFilename)` | Create new file for writing |
| `read(pBuffer, count)` | Read bytes from file |
| `write(pBuffer, count)` | Write bytes to file |
| `seek(position)` | Set file position |
| `closeFile()` | Close current file |
| `changeDirectory(pPath)` | Navigate to directory |
| `newDirectory(pName)` | Create new directory |

## Documentation

- [API Reference](OB4269-API-Reference.md) - Complete public interface documentation
- [Theory of Operations](OB4269-Theory-of-Operations.md) - How the driver works internally
- [Performance Study](OB4269-Performance-Study.md) - Timing analysis and optimization notes
- [FAT32 Compliance Analysis](OB4269-FAT32-Compliance-Analysis.md) - FAT32 standard compliance details
- [SPI Bus State Analysis](SPI-Bus-State-Analysis.md) - Multi-device SPI bus sharing analysis

## Testing

### Regression Tests

The `src/` folder contains regression tests for validating driver functionality:

- `SD_RT_mount_tests.spin2` - Mount/unmount cycles
- `SD_RT_file_ops_tests.spin2` - File create/open/close/delete/rename
- `SD_RT_read_write_tests.spin2` - Data read/write patterns
- `SD_RT_directory_tests.spin2` - Directory operations
- `SD_RT_seek_tests.spin2` - Random access and seek
- `SD_RT_testcard_validation.spin2` - Cross-OS compatibility validation

### Test Card Setup

For cross-OS compatibility testing, see [TestCard/TEST-CARD-SPECIFICATION.md](TestCard/TEST-CARD-SPECIFICATION.md) for instructions on preparing a test card with known file patterns.

## Project Structure

```
P2-uSD-FileSystem/
├── src/                    # Source files
│   ├── SD_card_driver.spin2    # Main SD card driver
│   ├── SD_RT_*.spin2           # Regression tests
│   └── run_now.sh              # Test runner script
├── DOCs/                   # Reference documentation
├── TestCard/               # Test card specification
├── REF/                    # Reference implementations
└── *.md                    # Project documentation
```

## Performance

Typical performance on P2 at 320 MHz:

| Operation | Throughput |
|-----------|------------|
| Sequential Read | ~20 KB/sec |
| Sequential Write | ~15 KB/sec |

Performance is primarily limited by the SPI clock rate during initialization and the sector-at-a-time access pattern.

## Known Limitations

- Single file open at a time
- 8.3 filename format (no long filename support)
- FAT32 only (no FAT12/FAT16/exFAT)
- SPI mode only (no SD native mode)

## Credits

- **Original Driver**: Chris Gadd (OB4269 from Parallax OBEX)
- **FAT32 Compliance Fixes**: Stephen M. Moraco, Iron Sheep Productions
- **Testing & Documentation**: Claude (Anthropic)

## License

MIT License - See [LICENSE](LICENSE) for details.

## Author

Stephen M. Moraco
Iron Sheep Productions, LLC
stephen@ironsheep.biz

---

*Part of the Iron Sheep Productions Propeller 2 Projects Collection*
