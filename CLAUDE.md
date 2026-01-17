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

## Expected Project Structure

When populated, this project will follow the standard Iron Sheep P2 project pattern:

```
P2-uSD-Study/
├── *.spin2              # Main Spin2 source files
├── README.md            # Project documentation
├── LICENSE
├── .gitignore
├── .vscode/             # VSCode settings
├── .github/             # GitHub workflows
├── REF/                 # Reference implementations
├── DOCs/                # Documentation (datasheets, etc.)
└── RegresssionTests/    # Test files
```

## ⛔ CRITICAL: NEVER RUN FLEXSPIN DIRECTLY

**If you find yourself about to run `flexspin`, STOP IMMEDIATELY. You are doing it wrong.**

This project has a test infrastructure that handles compilation and deployment to hardware automatically. Always use:

```bash
./run_now.sh
```

This script:
1. Compiles the code with flexspin
2. Downloads to the P2 hardware
3. Captures debug output in headless mode
4. Saves logs to the `logs/` directory

**NEVER run flexspin manually. ALWAYS use the test script.**

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
