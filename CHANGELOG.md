# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [1.0.0] - 2026-01-16

### Added

- **SD Card Driver** (`SD_card_driver.spin2`)
  - Full FAT32 filesystem support for read/write operations
  - Cross-OS compatibility (Windows, macOS, Linux formatted cards)
  - SDHC/SDXC support with block addressing up to 2TB
  - Directory operations: create, navigate, enumerate
  - File operations: create, open, read, write, seek, rename, delete
  - Multi-cluster file support for files larger than cluster size
  - Dual FAT mirroring for FAT32 compliance
  - FSInfo sector updates for accurate free space reporting
  - Volume label support
  - Low-level methods for formatting: `initCardOnly()`, `cardSizeSectors()`, `readSectorRaw()`, `writeSectorRaw()`

- **FAT32 Format Utility** (`SD_format_utility.spin2`)
  - Format SD cards with FAT32 filesystem directly from P2
  - Default volume label "P2-XFER" or custom labels up to 11 characters
  - Proper partition alignment (4MB) for modern SD cards
  - Creates MBR, VBR, FSInfo, backup boot sector, dual FAT tables, and root directory
  - Automatic cluster size selection based on card capacity

- **Regression Test Suite**
  - `SD_RT_mount_tests.spin2` - Mount/unmount cycle tests
  - `SD_RT_file_ops_tests.spin2` - File create/open/close/delete/rename tests
  - `SD_RT_read_write_tests.spin2` - Data read/write pattern tests
  - `SD_RT_directory_tests.spin2` - Directory operation tests
  - `SD_RT_seek_tests.spin2` - Random access and seek tests
  - `SD_RT_format_tests.spin2` - FAT32 format verification tests
  - `SD_RT_utilities.spin2` - Shared test framework utilities

- **Test Card Validation**
  - `SD_RT_testcard_validation.spin2` - Cross-OS compatibility validation
  - `SD_Test_Suite.spin2` - Complete test suite runner
  - Test card specification for reproducible testing

- **Documentation**
  - API Reference documentation
  - Theory of Operations
  - FAT32 Compliance Analysis
  - SPI Bus State Analysis

### Fixed

- FAT32 compliance: Dual FAT table mirroring (both FAT1 and FAT2 updated on writes)
- FAT32 compliance: FSInfo sector properly updated on unmount
- FAT32 compliance: High 4 bits of FAT entries preserved during allocation
- Proper volume label reading from VBR

### Credits

- Original driver by Chris Gadd (OB4269 from Parallax OBEX)
- FAT32 compliance fixes by Stephen M. Moraco, Iron Sheep Productions
- Testing and documentation assistance by Claude (Anthropic)

[Unreleased]: https://github.com/ironsheep/P2-uSD-FileSystem/compare/v1.0.0...HEAD
[1.0.0]: https://github.com/ironsheep/P2-uSD-FileSystem/releases/tag/v1.0.0
