# CLAUDE.md Mastery Insert: Todo MCP

## Deploy-Ready CLAUDE.md Section

**Purpose**: Immediate mastery-level operation, zero friction startup

---

## Installation (CRITICAL - READ FIRST)

This insert has **TWO parts** with specific placement requirements:

### Part 1: Session Trigger (TOP OF FILE)
**Place this at the VERY TOP of your CLAUDE.md, before any other content.**

```markdown
## ⚡ SESSION START (EXECUTE IMMEDIATELY)
**Run this command FIRST, before any other work:**
```bash
mcp__todo-mcp__context_resume
```
```

This leverages recency/primacy effects - it's the last thing Claude reads before responding, ensuring automatic execution.

### Part 2: Reference Material (MAIN BODY)
**Place the rest of this document in your CLAUDE.md body** alongside other project documentation. This provides command reference and workflow patterns.

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

# position_id only for quick interactive work
mcp__todo-mcp__todo_start position_id:1    # Ephemeral - changes on reorder

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

### Anti-Pattern Prevention

**Policy Override Prevention**:
- Never ignore explicit instructions for perceived efficiency
- Maintain same process standards whether user present or absent
- Confirm before violating established workflow rules

**TodoWrite Discipline**:
- ONE MCP task breakdown only
- Clear TodoWrite on task completion
- Save TodoWrite state to context for crash recovery

**Parameter Verification**:
- Always verify function parameter requirements
- Use correct data types (number vs string vs boolean)
- Test new patterns before assuming they work

### Task Lifecycle (ENFORCED)
1. **Start** before work: `todo_start task_id:"#N"`
2. **Complete** after work: `todo_complete task_id:"#N"`
3. **Archive** when done: `todo_archive`
4. Only ONE task `in_progress` at a time (auto-enforced)

### Version Transition Protocol
**Safe transition points** (preference order):
1. Between sprints (optimal)
2. After task completion + archive (safe)
3. Between tasks (acceptable)
4. Emergency with state preservation (risky)

**Always backup before version changes**:
```bash
mcp__todo-mcp__project_dump include_context:true
```

---

## CRITICAL: Mastery Folder is Read-Only

**NEVER create, modify, or store files in `.todo-mcp/mastery/`**

This folder is managed content that gets **completely replaced** during upgrades. Any files you add will be deleted.

**Exception**: `FRICTION_LOG.md` - Your entries are preserved during upgrades (moved to timestamped backup).

**Where to store your notes instead**:
- Project root or `docs/` folder
- `.todo-mcp/notes/` (create if needed)
- Context system: `mcp__todo-mcp__context_set`

---

## Implementation Notes

### For New Projects
1. Install this section in CLAUDE.md
2. Test basic operations
3. Establish context hygiene routine
4. Begin with dual-system workflow

### For Existing Projects
1. Backup current state: `mcp__todo-mcp__project_dump include_context:true`
2. Preserve valuable existing context and patterns
3. Gradually adopt the dual-system workflow
4. Validate improvements before full commitment

### Key Success Patterns
- **Context bridge**: Save TodoWrite state for crash recovery
- **Verification**: Always check "empty" responses with get_all
- **Process consistency**: Same quality standards under all conditions
- **Recovery confidence**: Systematic procedures, not ad-hoc fixes

---

**This insert provides immediate mastery-level operation based on learnings from multiple Claude instances. Deploy for zero-friction Todo MCP usage.**