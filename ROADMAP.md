# P2-uSD-Study Roadmap

Project roadmap for SD card driver development and testing.

---

## Phase 1: Validation (Current)

**Status**: In Progress

- [x] Download and study OB4269 FAT32 driver
- [x] Create technical reference documentation
- [x] Analyze FAT32 spec compliance
- [x] Document cross-OS portability issues
- [x] Create performance study
- [x] Create test card content structure
- [x] Create test program with debug output
- [x] Add volumeLabel() method to driver
- [ ] Run read-only validation tests
- [ ] Verify all 14 tests pass

---

## Phase 2: Driver Fixes

**Status**: Planned

Apply fixes documented in `OB4269-Technical-Adjustments.md`:

- [ ] Fix 1: FAT mirroring in deleteFile() (1 line)
- [ ] Fix 2: Preserve high 4 bits when freeing clusters (1 line)
- [ ] Fix 3: FSInfo sector support (~45 lines)
- [ ] Re-run validation tests after fixes
- [ ] Test cross-OS round-trip (P2 → Windows → P2)

---

## Phase 3: Performance Optimization

**Status**: Planned

Implement Smart Pin SPI as documented in `OB4269-Performance-Study.md`:

- [ ] Phase 3a: Replace transfer() with Smart Pin version
- [ ] Phase 3b: Replace sector I/O with Smart Pin operations
- [ ] Phase 3c: Optimize clock generation
- [ ] Benchmark and compare to original driver
- [ ] Document performance gains

---

## Phase 4: Format Utility

**Status**: Future

Create standalone FAT32 format utility:

- [ ] Add CMD9 (SEND_CSD) to read card capacity
- [ ] Implement FAT32 parameter calculations per MS spec
- [ ] Write MBR, VBR, FSInfo, backup boot sector
- [ ] Initialize FAT tables (both copies)
- [ ] Initialize root directory with volume label
- [ ] Test format → mount → write → read cycle
- [ ] Test formatted card on Windows/Mac/Linux

**Estimated effort**: 200-300 lines, 2-3 hours

---

## Future Considerations

Items for potential future work (not scheduled):

- [ ] Long filename (LFN) support
- [ ] Multi-cog safe access
- [ ] Read-ahead caching
- [ ] Write-back caching
- [ ] exFAT support (for SDXC cards)
- [ ] Error code returns (vs simple true/false)

---

## Documents

| Document | Purpose |
|----------|---------|
| `OB4269-FAT32-SD-Card-Driver-Reference.md` | API and capabilities reference |
| `OB4269-FAT32-Compliance-Analysis.md` | FAT32 spec compliance issues |
| `OB4269-Technical-Adjustments.md` | Required code fixes |
| `OB4269-Performance-Study.md` | Performance analysis and Smart Pin plan |
| `TestCard/TEST-CARD-SPECIFICATION.md` | Test card structure and expected values |

---

*Last updated: 2026-01-14*
