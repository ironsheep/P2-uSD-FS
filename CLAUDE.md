# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## ⚡ SESSION START (EXECUTE IMMEDIATELY)
**Run this command FIRST, before any other work:**
```bash
mcp__todo-mcp__context_resume
```

---

## Project Overview

**P2-uSD-Study** is a study/development project for microSD card integration with the Parallax Propeller 2 (P2) microcontroller.

**Part of**: Iron Sheep Productions Propeller2 Projects Collection

**Status**: New/Development Phase

## Languages & Tools

- **Primary Language**: Spin2 (`.spin2` files) - the Propeller2 microcontroller language
- **Target Hardware**: Parallax Propeller 2 microcontroller
- **Development Tools**: VSCode + FlexProp compiler (or PNut)
- **Version Control**: Git

## Project Structure

```
P2-uSD-FileSystem/
├── src/                    # DRIVER DELIVERABLE
│   └── SD_card_driver.spin2    # The SD card driver (only deliverable file)
│
├── regression-tests/       # Regression test suite
│   ├── SD_RT_utilities.spin2   # Test framework utilities
│   ├── SD_RT_mount_tests.spin2
│   ├── SD_RT_file_ops_tests.spin2
│   ├── SD_RT_read_write_tests.spin2
│   ├── SD_RT_directory_tests.spin2
│   └── SD_RT_seek_tests.spin2
│
├── TestCard/               # Test card validation
│   ├── TEST-CARD-SPECIFICATION.md
│   ├── TESTROOT/               # Files to copy to test card
│   ├── SD_RT_testcard_validation.spin2
│   └── SD_Test_Suite.spin2
│
├── tools/                  # Scripts (run tests from here)
│   ├── run_test.sh             # Test runner script
│   └── logs/                   # Test output logs
│
├── DOCs/                   # Reference documentation
└── *.md                    # Project documentation
```

## ⛔ CRITICAL: WORKING DIRECTORY AND TEST EXECUTION

### ALWAYS cd to tools/ FIRST!

```bash
cd /Users/stephen/Projects/Projects-ExtGit/IronSheepProductionsLLC/Propeller2/P2-uSD-Study/tools
```

**Before running ANY test script, verify you are in the `tools/` directory.**
The run_test.sh script MUST be executed from tools/ - it will fail otherwise.

### Test Commands (from tools/ directory)

```bash
# Regression tests
./run_test.sh ../regression-tests/SD_RT_mount_tests.spin2
./run_test.sh ../regression-tests/SD_RT_file_ops_tests.spin2
./run_test.sh ../regression-tests/SD_RT_read_write_tests.spin2
./run_test.sh ../regression-tests/SD_RT_directory_tests.spin2
./run_test.sh ../regression-tests/SD_RT_seek_tests.spin2
./run_test.sh ../regression-tests/SD_RT_format_tests.spin2 -t 120

# Test card validation (longer timeout)
./run_test.sh ../TestCard/SD_RT_testcard_validation.spin2 -t 120
./run_test.sh ../TestCard/SD_Test_Suite.spin2 -t 120
```

### What run_test.sh does:
1. Compiles with pnut-ts (includes src/ for driver)
2. Downloads to P2 hardware
3. Captures debug output in headless mode
4. Saves logs to `tools/logs/`

**NEVER run flexspin or pnut-ts directly. ALWAYS use `./run_test.sh` from tools/.**

---

## Reference Projects

See sibling projects for established patterns:
- **P2-FLASH-FileSystem** - Most complete example (filesystem for P2 Edge FLASH chip)
- **P2-FLASH-RAM-FileSystem** - Hybrid flash/RAM filesystem
- **P2-OctoSerial** - Serial communication driver

## P2 Knowledge Base

Use the P2KB MCP tools for Propeller 2 documentation:
```bash
mcp__p2kb-mcp__p2kb_get query:"spin2 pinwrite"  # Natural language lookup
mcp__p2kb-mcp__p2kb_find                         # Browse categories
mcp__p2kb-mcp__p2kb_obex_get query:"sd card"    # Search OBEX objects
```

---

## 🎯 Todo MCP Mastery Operations

### Quick Recovery Commands
```bash
mcp__todo-mcp__context_resume     # "WHERE WAS I?" - primary recovery
mcp__todo-mcp__todo_next          # Smart task recommendation
mcp__todo-mcp__todo_archive       # Clean completed tasks
mcp__todo-mcp__context_stats      # Context health check
```

### Dual System Strategy (IRON RULE)
**MCP Tasks**: Persistent, session-spanning, permanent ID «#N»
**TodoWrite**: Current task breakdown only, cleared on completion

```bash
# CORRECT workflow (capture task_id from create response)
result = mcp__todo-mcp__todo_create content:"Feature implementation" estimate_minutes:180
# Response includes: «#N»

mcp__todo-mcp__todo_start task_id:"#N"  # Use permanent ID for reliability
TodoWrite: ["Step 1", "Step 2", "Step 3"]  # Single task breakdown only
# Work through steps...
mcp__todo-mcp__todo_complete task_id:"#N"
TodoWrite: []  # Clear for next task
```

**NEVER**: Multiple MCP task IDs in TodoWrite (quality degradation)

### Core Parameter Patterns
```bash
# 9 functions support both position_id AND task_id:
# todo_start, todo_pause, todo_resume, todo_complete,
# todo_update, todo_delete, todo_estimate, todo_tag_add, todo_tag_remove

# PREFER task_id (permanent) over position_id (ephemeral)
mcp__todo-mcp__todo_start task_id:"#49"    # Reliable - ID never changes
mcp__todo-mcp__todo_pause task_id:"#49" reason:"Blocked"
mcp__todo-mcp__todo_complete task_id:"#49"

# Critical data types
estimate_minutes:60        # Number, never string
priority:"high"           # lowercase: critical/high/medium/low/backlog
force:true               # Boolean, never string
task_id:"#49"            # String with # prefix
```

### Context Hygiene (40-Key Target)
```bash
# Persistent context (KEEP)
lesson_*, workaround_*, recovery_*, friction_*

# Temporary context (DELETE after use)
temp_*, current_*, session_*, task_#N_*

# Regular cleanup
mcp__todo-mcp__context_delete pattern:"temp_*"
mcp__todo-mcp__context_delete pattern:"task_#N_*"  # After task completion
```

### Data Safety (ALWAYS)
```bash
# SAFE archiving (preserves backup)
mcp__todo-mcp__todo_archive

# Complete backup before risky operations
mcp__todo-mcp__project_dump include_context:true

# Recovery
mcp__todo-mcp__project_restore file:"filename.json" mode:"replace"
```

### Task Lifecycle (ENFORCED)
1. **Start** before work: `todo_start task_id:"#N"`
2. **Complete** after work: `todo_complete task_id:"#N"`
3. **Archive** when done: `todo_archive`
4. Only ONE task `in_progress` at a time (auto-enforced)

---

## CRITICAL: Mastery Folder is Read-Only

**NEVER create, modify, or store files in `.todo-mcp/mastery/`**

This folder is managed content that gets **completely replaced** during upgrades.

**Where to store notes instead**:
- Project root or `docs/` folder
- `.todo-mcp/notes/` (create if needed)
- Context system: `mcp__todo-mcp__context_set`
