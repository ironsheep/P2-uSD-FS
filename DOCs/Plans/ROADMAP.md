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

---

## Current Sprint: Driver Performance (Phase 1)

**Status**: ACTIVE
**Plan Document**: `PHASE1-SMARTPIN-SPI.md`
**Sprint Plan**: `SPRINT-PLAN-driver-performance.md`

### Part A: Smart Pin Foundation
- [ ] Task 1.1: initSPIPins() - Configure smart pin modes
- [ ] Task 1.2: setSPISpeed() - Sysclk-independent frequency control
- [ ] Task 1.3: sp_transfer() - Smart pin byte/word transfer
- [ ] Task 1.7: Regression tests (partial)

### Part B: Smart Pin Sector Operations
- [ ] Task 1.4: sp_readSector() - Optimized 512-byte read
- [ ] Task 1.5: sp_writeSector() - Optimized 512-byte write
- [ ] Task 1.6: Integration with initCard()
- [ ] Task 1.7: Full regression tests

### Part C: Multi-Block Operations
- [ ] Task 1.8: readSectors() - CMD18 multi-block read
- [ ] Task 1.9: writeSectors() - CMD25 multi-block write
- [ ] Task 1.10: Multi-block benchmark

### Performance Targets

| Metric | Baseline | Target |
|--------|----------|--------|
| Read 256KB | 1,467 KB/s | 4,000+ KB/s |
| Write 32KB | 425 KB/s | 1,200+ KB/s |
| SPI Clock | ~20 MHz | 25-50 MHz |

---

## Future Phases

### Phase 2: Adaptive Speed Control
**Status**: Planned (from SPRINT-PLAN-driver-performance.md)

- [ ] Automatic card speed detection
- [ ] Per-card speed profiles
- [ ] Runtime speed adjustment on errors
- [ ] Speed validation tests

### Phase 3: Sequential I/O Optimization
**Status**: Planned

- [ ] Integrate multi-block into file read/write
- [ ] Read-ahead caching for sequential access
- [ ] Write coalescing for small writes
- [ ] Cluster-aligned buffer strategy

### Phase 4: Format Utility
**Status**: Complete (2026-01-28)

- [x] Add CMD9 (SEND_CSD) to read card capacity
- [x] Implement FAT32 parameter calculations
- [x] Write MBR, VBR, FSInfo, backup boot sector
- [x] Initialize FAT tables and root directory
- [x] FAT32 audit tool for validation
- [x] 46 format validation tests passing

### Phase 5: Safe FSCK Utility
**Status**: Future

Create a filesystem check/repair utility for SD cards:

- [ ] Sync backup VBR with primary VBR
- [ ] Sync backup FSInfo with primary FSInfo
- [ ] Sync FAT2 with FAT1
- [ ] Recalculate and fix FSInfo free cluster count
- [ ] Update FSInfo next-free cluster hint
- [ ] Validate and fix directory . and .. entries
- [ ] (Advanced) Detect and report lost clusters
- [ ] (Advanced) Detect and report cross-linked files

**Note**: Start with "safe fixes" (backup sync, FAT sync, free count) that cover 90% of real-world corruption. Advanced fixes (lost clusters, cross-links) require full FAT scan and are more complex.

### Phase 6: Advanced Features
**Status**: Future

- [ ] Long filename (LFN) support
- [ ] Read-ahead caching
- [ ] Write-back caching
- [ ] Multiple file handles

---

## Document Map

### Planning
| Document | Purpose |
|----------|---------|
| `ROADMAP.md` | High-level project timeline (this file) |
| `SPRINT-PLAN-driver-performance.md` | Current sprint overview |
| `PHASE1-SMARTPIN-SPI.md` | Detailed Phase 1 implementation plan |

### Decisions
| Document | Purpose |
|----------|---------|
| `ARCHITECTURE-DECISIONS.md` | Core architectural choices |
| `TECHNICAL-DECISIONS.md` | Technical research and decisions |
| `SD-Driver-Improvements.md` | Issue tracking and fixes |

### Reference
| Document | Purpose |
|----------|---------|
| `THEORY-OF-OPERATIONS.md` | How the driver works |
| `BENCHMARK-RESULTS.md` | Performance measurements |
| `CARD-CATALOG.md` | SD card compatibility data |
| `SPI_SD_Implementation_Reference.md` | Protocol reference |

---

## Milestones

| Milestone | Target | Status |
|-----------|--------|--------|
| Baseline documented | 2026-01-21 | ✅ Complete |
| Phase 1 plan approved | 2026-01-21 | ✅ Complete |
| Smart Pin SPI working | TBD | 🔄 In Progress |
| Multi-block working | TBD | 🔄 In Progress |
| 4 MB/s read achieved | TBD | ⏳ Pending |
| Format utility complete | 2026-01-28 | ✅ Complete |
| Safe FSCK utility | TBD | ⏳ Future |

---

*Last updated: 2026-01-28*
