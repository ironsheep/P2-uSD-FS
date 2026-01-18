# Design Exploration: Multi-File Handle Support

## The Requirement

Support multiple simultaneously open files with proper data safety guarantees.

---

## Handle Table Design Options

### Option A: Fixed Handle Count (Recommended)

```spin2
CON
  MAX_OPEN_FILES = 4          ' Or 8, configurable at compile time

DAT
  ' Per-handle state (~24 bytes per handle)
  h_flags       BYTE    0[MAX_OPEN_FILES]   ' HANDLE_FREE, HANDLE_READ, HANDLE_WRITE
  h_device      BYTE    0[MAX_OPEN_FILES]   ' For multi-device: DEV_SD, DEV_FLASH
  h_cluster     LONG    0[MAX_OPEN_FILES]   ' Current cluster in chain
  h_position    LONG    0[MAX_OPEN_FILES]   ' Byte offset in file
  h_size        LONG    0[MAX_OPEN_FILES]   ' File size (cached)
  h_start_clus  LONG    0[MAX_OPEN_FILES]   ' First cluster (for rewind)
  h_dir_entry   LONG    0[MAX_OPEN_FILES]   ' Directory entry sector:offset
```

**Memory cost**: ~24 bytes × N handles

| Handles | Memory |
|---------|--------|
| 4 | 96 bytes |
| 8 | 192 bytes |
| 16 | 384 bytes |

**Pros:**
- Simple, predictable memory usage
- No runtime allocation
- Fast handle lookup (array index)

**Cons:**
- Fixed limit at compile time
- Unused slots waste memory

---

### Option B: Linked List (Dynamic)

```spin2
DAT
  ' Pool of handle nodes
  h_pool        BYTE    0[MAX_OPEN_FILES * HANDLE_SIZE]
  h_free_list   LONG    0     ' Head of free list
  h_active_list LONG    0     ' Head of active list
```

**Pros:**
- Only allocated handles use "active" memory
- Could grow pool dynamically (if hub RAM available)

**Cons:**
- More complex allocation/deallocation
- Cache-unfriendly traversal
- Overhead of next/prev pointers
- No real benefit on P2 (hub RAM is fixed)

**Verdict**: Overkill for embedded. Use Option A.

---

## Handle Encoding Strategies

### Strategy 1: Simple Index

```spin2
PUB openFile(path) : handle
  '' Returns 0 to MAX_OPEN_FILES-1, or -1 on error

  handle := findFreeSlot()
  if handle == -1
    return set_error(E_TOO_MANY_FILES)
  ' ... open file into slot ...
  return handle
```

**Usage:**
```spin2
h := sd.openFile(@"data.txt")
sd.read(h, @buf, 512)
sd.closeFile(h)
```

### Strategy 2: Encoded Handle (for Multi-Device)

Encode device + slot in single value:

```spin2
CON
  ' Handle encoding: bits 7:6 = device, bits 5:0 = slot index
  HANDLE_DEVICE_MASK = %1100_0000
  HANDLE_INDEX_MASK  = %0011_1111

  DEV_SD    = 0 << 6    ' %0000_0000
  DEV_FLASH = 1 << 6    ' %0100_0000
  DEV_PSRAM = 2 << 6    ' %1000_0000

PRI encode_handle(device, slot) : handle
  return device | slot

PRI decode_handle(handle) : device, slot
  device := handle & HANDLE_DEVICE_MASK
  slot := handle & HANDLE_INDEX_MASK
```

**Usage:**
```spin2
h := fs.open(DEV_SD, @"source.bin")     ' Returns $00-$3F for SD
h := fs.open(DEV_FLASH, @"dest.bin")    ' Returns $40-$7F for Flash
```

### Strategy 3: Opaque Pointer

Return address of handle structure:

```spin2
PUB openFile(path) : handle
  handle := @h_flags[findFreeSlot()]
```

**Pros:** No decode needed
**Cons:** Harder to validate, exposes internals

**Recommendation**: Strategy 1 for single-device, Strategy 2 for multi-device combo driver.

---

## Buffer Management Strategies

### Strategy A: Shared Single Buffer

```spin2
DAT
  buf           BYTE    0[512]          ' One buffer for all handles
  buf_handle    LONG    -1              ' Which handle owns the buffer
  buf_sector    LONG    -1              ' Which sector is cached
  buf_dirty     BYTE    0               ' Needs write-back?
```

**Flow:**
1. Before any I/O, check if buffer belongs to this handle
2. If not, flush current buffer (if dirty), load new sector
3. Perform I/O
4. Mark dirty if write

**Pros:**
- Minimal memory (512 bytes total)
- Simple

**Cons:**
- Thrashing when alternating between files
- Every handle switch = potential disk I/O

### Strategy B: Per-Handle Buffers

```spin2
DAT
  h_buffer      BYTE    0[MAX_OPEN_FILES * 512]   ' 512 bytes per handle
  h_buf_sector  LONG    0[MAX_OPEN_FILES]         ' Cached sector per handle
  h_buf_dirty   BYTE    0[MAX_OPEN_FILES]         ' Dirty flag per handle
```

**Memory cost**: 512 bytes × N handles

| Handles | Buffer Memory | Total with State |
|---------|---------------|------------------|
| 4 | 2 KB | 2.1 KB |
| 8 | 4 KB | 4.2 KB |

**Pros:**
- No thrashing between handles
- Better performance for interleaved access

**Cons:**
- Significant memory cost
- May exceed available hub RAM

### Strategy C: Small Per-Handle + Shared Large

```spin2
DAT
  ' Small buffer per handle for metadata/directory
  h_small_buf   BYTE    0[MAX_OPEN_FILES * 64]    ' 64 bytes per handle

  ' Shared large buffer for data transfer
  shared_buf    BYTE    0[512]
  shared_owner  LONG    -1
```

**Pros:**
- Balance of memory and performance
- Directory entry cached per-handle
- Data transfers share buffer

**Cons:**
- More complex buffer management

### Strategy D: No Buffering (Direct I/O)

```spin2
PUB read(handle, p_user_buf, count) : bytes_read
  '' Read directly into user buffer, no intermediate copy
```

**Pros:**
- Zero buffer memory
- No copy overhead

**Cons:**
- User buffer must be sector-aligned for best performance
- Partial sector reads still need temp buffer
- Write-back caching not possible

**Recommendation**:
- **Resource-constrained**: Strategy A (shared buffer) or D (direct I/O)
- **Performance-focused**: Strategy B (per-handle buffers)
- **Balanced**: Strategy C (small per-handle + shared)

---

## Data Safety: Write Strategies

### Write-Through (Immediate Flush)

Every write immediately goes to disk:

```spin2
PRI do_write(handle, p_buf, count) : result
  ' ... write data to sector buffer ...
  writeSector(sector)           ' IMMEDIATE flush
  ' ... update FAT if cluster allocated ...
  writeFATSector()              ' IMMEDIATE flush
  ' ... update directory entry ...
  writeDirectoryEntry()         ' IMMEDIATE flush
```

**Data loss risk**: LOW
- Only lose data if crash DURING a write
- FAT and directory always consistent

**Performance**: POOR
- Multiple sector writes per operation
- ~10-20ms per write (SD card latency)

### Write-Back (Deferred Flush)

Buffer writes, flush on close or sync:

```spin2
PRI do_write(handle, p_buf, count) : result
  ' ... write data to buffer ...
  h_buf_dirty[handle] := TRUE
  ' FAT updates cached in memory
  ' Directory entry updated on close

PUB closeFile(handle)
  if h_buf_dirty[handle]
    flushBuffer(handle)
  updateDirectoryEntry(handle)  ' Size, timestamp
  updateFAT()                   ' If clusters allocated
```

**Data loss risk**: HIGHER
- Crash before close = lost writes + inconsistent FAT
- File size wrong in directory
- Cluster chain may be incomplete

**Performance**: GOOD
- Many writes batched
- Single flush on close

### Hybrid: Write-Through Data, Deferred Metadata

```spin2
PRI do_write(handle, p_buf, count) : result
  ' Data written immediately
  writeSector(sector)           ' IMMEDIATE

  ' Metadata deferred
  h_size_dirty[handle] := TRUE
  h_fat_dirty := TRUE

PUB sync(handle)
  '' Explicit flush point - call periodically for safety
  if h_fat_dirty
    writeFATSectors()
  if h_size_dirty[handle]
    updateDirectoryEntry(handle)
```

**Data loss risk**: MEDIUM
- Data on disk, but file size/FAT may be stale
- Recoverable: FAT scan can find orphaned clusters

**Performance**: MEDIUM
- Data writes immediate
- Metadata batched

**Recommendation**: Hybrid with periodic sync, or configurable per-handle.

---

## Crash/Brown-Out Safety Analysis

### What Can Go Wrong

| Scenario | Write-Through | Write-Back | Hybrid |
|----------|---------------|------------|--------|
| Crash during data write | Partial sector | Partial sector | Partial sector |
| Crash after write, before close | Data safe, metadata safe | Data LOST, metadata stale | Data safe, metadata stale |
| Crash during FAT update | FAT inconsistent | FAT inconsistent | FAT inconsistent |
| Crash during dir entry update | Size wrong | Size wrong | Size wrong |

### Mitigation Strategies

#### 1. Write Ordering (Critical)

Always write in this order:
1. **Data sectors first** - actual content
2. **FAT sectors second** - cluster chain
3. **Directory entry last** - file size, timestamp

If crash after step 1: Data exists, FAT scan can recover
If crash after step 2: Data + chain exist, size recoverable
If crash after step 3: Fully consistent

#### 2. FAT Mirroring

FAT32 has two FAT copies. Update both:

```spin2
PRI writeFATEntry(cluster, value)
  ' Write to FAT1
  writeSector(fat_sec + offset)
  ' Write to FAT2
  writeSector(fat2_sec + offset)
```

If crash between: FAT2 is backup for recovery

#### 3. Journaling (Advanced)

Reserve a journal area on disk:

```spin2
PRI journaledWrite(sector, p_data)
  ' 1. Write intent to journal
  writeJournalEntry(INTENT_WRITE, sector, p_data)
  ' 2. Perform actual write
  writeSector(sector)
  ' 3. Mark journal entry complete
  markJournalComplete()
```

On mount, replay incomplete journal entries.

**Pros:** Full crash recovery
**Cons:** Double write overhead, complexity

#### 4. Sync Points

Expose explicit sync for application control:

```spin2
PUB sync(handle)
  '' Flush this handle's buffers and metadata

PUB syncAll()
  '' Flush all handles

PUB setSyncPolicy(handle, policy)
  '' SYNC_IMMEDIATE, SYNC_ON_CLOSE, SYNC_MANUAL
```

Let application decide safety vs. performance tradeoff.

---

## Recommended Design

### For SD Card Driver (Single Device)

```spin2
CON
  MAX_OPEN_FILES = 4

  ' Handle flags
  HF_FREE       = 0
  HF_READ       = 1
  HF_WRITE      = 2
  HF_DIRTY      = $80         ' OR'd with mode

DAT
  ' Handle table (~28 bytes per handle = 112 bytes for 4 handles)
  h_flags       BYTE    0[MAX_OPEN_FILES]
  h_cluster     LONG    0[MAX_OPEN_FILES]   ' Current cluster
  h_position    LONG    0[MAX_OPEN_FILES]   ' Byte position
  h_size        LONG    0[MAX_OPEN_FILES]   ' File size
  h_start_clus  LONG    0[MAX_OPEN_FILES]   ' First cluster
  h_dir_sector  LONG    0[MAX_OPEN_FILES]   ' Directory entry location
  h_dir_offset  WORD    0[MAX_OPEN_FILES]   ' Offset within sector

  ' Shared buffer (512 bytes)
  buf           BYTE    0[512]
  buf_handle    LONG    -1
  buf_sector    LONG    -1
  buf_dirty     BYTE    0

  ' Write policy
  write_policy  BYTE    POLICY_HYBRID       ' IMMEDIATE, DEFERRED, HYBRID
```

**Total memory**: ~650 bytes (vs. ~550 bytes current single-file)

### API Changes

```spin2
PUB openFile(path, mode) : handle
  '' Open file, returns handle 0-3 or negative error
  '' mode: MODE_READ, MODE_WRITE, MODE_APPEND

PUB closeFile(handle) : result
  '' Close file, flush buffers

PUB read(handle, p_buffer, count) : bytes_read
  '' Read from file

PUB write(handle, p_buffer, count) : bytes_written
  '' Write to file

PUB seek(handle, position) : result
  '' Seek to position

PUB tell(handle) : position
  '' Get current position

PUB fileSize(handle) : size
  '' Get file size

PUB sync(handle) : result
  '' Flush handle's buffers

PUB syncAll() : result
  '' Flush all dirty buffers
```

### For Multi-Device Combo Driver

Same as above, but:
- Handle encodes device ID in upper bits
- Per-device handle pools, or shared pool with device tag
- Cross-device copy uses two handles

---

## Implementation Priority

1. **Phase 1**: Handle table with shared buffer
   - Basic multi-file support
   - Hybrid write policy
   - ~100 bytes additional RAM

2. **Phase 2**: Per-handle small buffers
   - 64-byte directory cache per handle
   - Reduces thrashing for metadata

3. **Phase 3**: Configurable policies
   - Per-handle write policy
   - Application-controlled sync points

4. **Phase 4** (optional): Journaling
   - Full crash recovery
   - Only if data integrity is critical

---

## Open Questions

1. **Handle count**: 4? 8? Compile-time configurable?
2. **Default write policy**: Hybrid seems like good default?
3. **Backward compatibility**: Keep old single-file API as convenience wrappers?
4. **Error handling**: What if closeFile() fails mid-flush?

---

*Document created: 2026-01-17*
*Status: EXPLORATION - Needs decision*
