# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [0.9.1] - 2026-02-09

Initial release.

- FAT32-compliant SD card filesystem driver for the Parallax Propeller 2
- Smart pin SPI with streamer DMA for hardware-accelerated transfers
- Dedicated worker cog with hardware lock serialization
- Multi-file handle system (up to 4 simultaneous file and directory handles)
- Per-cog current working directory for safe multi-cog navigation
- Handle-based file API: open, read, write, seek, close
- Handle-based directory enumeration API
- Directory operations: create, navigate, enumerate, delete, rename
- Low-level raw sector read/write and multi-sector (CMD18/CMD25) bulk transfers
- Hardware-accelerated CRC-16 validation on all data transfers
- FAT32 format utility with cross-OS compatibility (Windows, macOS, Linux)
- FSCK utility: 4-pass filesystem check and repair
- FAT32 audit utility: read-only filesystem validation
- Card characterization, SPI speed testing, and performance benchmark utilities
- Interactive demo shell with DOS and Unix-style commands
- 251 automated regression tests across 11 test suites
- Documentation: tutorial, theory of operations, card catalog, benchmark results

### Credits

- Original driver concept by Chris Gadd (OB4269 from Parallax OBEX)
- Driver development by Stephen M. Moraco, Iron Sheep Productions


[Unreleased]: https://github.com/ironsheep/P2-uSD-FileSystem/compare/v0.9.1...HEAD
[0.9.1]: https://github.com/ironsheep/P2-uSD-FileSystem/releases/tag/v0.9.1
