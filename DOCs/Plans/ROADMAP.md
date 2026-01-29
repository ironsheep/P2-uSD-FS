# P2-uSD-Study Roadmap

Project roadmap for SD card driver development and testing.

---

## Completed Work

### Phase 0: Foundation (Complete)
- [x] Download and study OB4269 FAT32 driver
- [x] Create technical reference documentation
- [x] Analyze FAT32 spec compliance
- [x] Document cross-OS portability issues
- [x] Create test card content structure
- [x] Create test program with debug output
- [x] Add volumeLabel() method to driver

### Driver Fixes (Complete)
- [x] Fix 1: FAT mirroring in deleteFile()
- [x] Fix 2: Preserve high 4 bits when freeing clusters
- [x] Fix 3: FSInfo sector support
- [x] Fix 4: Add unmount() method
- [x] Fix 5: Add sync() method

### Card Characterization (Complete)
- [x] Create card catalog system
- [x] Characterize 6 SD cards with CID data
- [x] Document PNY card SPI timing issues
- [x] Create performance benchmark suite
- [x] Document baseline measurements

### Phase 1: Smart Pin SPI (Complete - 2026-01-29)
- [x] initSPIPins() - Configure P_TRANSITION, P_SYNC_TX, P_SYNC_RX
- [x] setSPISpeed() - Sysclk-independent frequency control
- [x] sp_transfer_8() - Smart pin byte transfer
- [x] Streamer DMA for 512-byte sector reads/writes
- [x] Full regression tests (129/129 passing)

### Phase 2: Adaptive Timing (Complete - 2026-01-29)
- [x] CID manufacturer identification (PNY, SanDisk, Samsung)
- [x] CSD TRAN_SPEED parsing for max speed
- [x] CSD timeout extraction (TAAC, NSAC, R2W_FACTOR)
- [x] Brand-specific speed limiting
- [x] FAT performance optimization (incremental free count)

### Phase 3: Multi-Sector Operations (Complete - 2026-01-29)
- [x] readSectors() - CMD18 multi-block read
- [x] writeSectors() - CMD25 multi-block write
- [x] File API integration (do_read/do_write optimization)
- [x] Cluster boundary handling with FAT chain following
- [x] Performance verified: ~4x speedup over single-sector

### Phase 4: Format Utility (Complete - 2026-01-28)
- [x] Add CMD9 (SEND_CSD) to read card capacity
- [x] Implement FAT32 parameter calculations
- [x] Write MBR, VBR, FSInfo, backup boot sector
- [x] Initialize FAT tables and root directory
- [x] FAT32 audit tool for validation
- [x] 46 format validation tests passing

---

## Current Status

**V2 Driver Certification**: PASS (129/129 tests at 320 MHz)

| Test Suite | Pass | Total |
|------------|------|-------|
| Mount | 17 | 17 |
| File Ops | 24 | 24 |
| Read/Write | 30 | 30 |
| Directory | 22 | 22 |
| Seek | 36 | 36 |

---

## Future Phases

### Phase 5: Multiple File Handles (Deferred)
**Status**: Future (user requested deferral)

- [ ] Handle table in DAT section (8 handles)
- [ ] Refactor do_open/close/read/write/seek for handle parameter
- [ ] Handle allocation/deallocation logic
- [ ] Multi-cog file independence

### Phase 6: Safe FSCK Utility
**Status**: Future

Create a filesystem check/repair utility for SD cards:

- [ ] Sync backup VBR with primary VBR
- [ ] Sync backup FSInfo with primary FSInfo
- [ ] Sync FAT2 with FAT1
- [ ] Recalculate and fix FSInfo free cluster count
- [ ] Update FSInfo next-free cluster hint
- [ ] Validate and fix directory . and .. entries

### Phase 7: Advanced Features
**Status**: Future

- [ ] Long filename (LFN) support
- [ ] Read-ahead caching
- [ ] Write-back caching
- [ ] High-Speed mode (50 MHz via CMD6)

---

## Document Map

### Planning
| Document | Purpose |
|----------|---------|
| `ROADMAP.md` | High-level project timeline (this file) |
| `SPRINT-PLAN-driver-performance.md` | Performance sprint details |
| `PHASE1-SMARTPIN-SPI.md` | Phase 1 implementation plan |

### Reference
| Document | Purpose |
|----------|---------|
| `V2-THEORY-OF-OPERATIONS.md` | V2 driver architecture and internals |
| `V2-DRIVER-CERTIFICATION-STATUS.md` | Test results and certification |
| `THEORY-OF-OPERATIONS.md` | V1 driver (historical) |
| `BENCHMARK-RESULTS.md` | Performance measurements |
| `CARD-CATALOG.md` | SD card compatibility data |

---

## Milestones

| Milestone | Target | Status |
|-----------|--------|--------|
| Baseline documented | 2026-01-21 | Complete |
| Phase 1 (Smart Pin SPI) | 2026-01-29 | Complete |
| Phase 2 (Adaptive Timing) | 2026-01-29 | Complete |
| Phase 3 (Multi-Sector) | 2026-01-29 | Complete |
| Phase 4 (Format Utility) | 2026-01-28 | Complete |
| V2 Driver Certified | 2026-01-29 | Complete (129/129 tests) |
| Phase 5 (Multiple Handles) | TBD | Deferred |
| Phase 6 (FSCK Utility) | TBD | Future |

---

*Last updated: 2026-01-29*
