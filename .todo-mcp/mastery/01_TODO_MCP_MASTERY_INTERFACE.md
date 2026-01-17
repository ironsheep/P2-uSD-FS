# Todo MCP Interface Reference (v1.9.9)

## The Fundamental Principle

**Every task has two identifiers:**
- **task_id** (`"#49"`) - Permanent. Never changes. Use this.
- **position_id** (`1`) - Ephemeral. Changes when list changes. Use only for quick interactive work.

**Rule: Always prefer task_id for reliability.**

When you create a task, capture the returned task_id and use it for all subsequent operations. This ensures your operations target the correct task regardless of list reordering.

## Dual-Parameter Resolution

**Nine functions** support both `position_id` and `task_id`:

| Function | position_id | task_id | Notes |
|----------|-------------|---------|-------|
| todo_start | `position_id:1` | `task_id:"#49"` | task_id preferred |
| todo_pause | `position_id:1` | `task_id:"#49"` | task_id preferred |
| todo_resume | `position_id:1` | `task_id:"#49"` | task_id preferred |
| todo_complete | `position_id:1` | `task_id:"#49"` | task_id preferred |
| todo_update | `position_id:1` | `task_id:"#49"` | task_id preferred |
| todo_delete | `position_id:1` | `task_id:"#49"` | task_id preferred |
| todo_estimate | `position_id:1` | `task_id:"#49"` | task_id preferred |
| todo_tag_add | `position_id:1` | `task_id:"#49"` | task_id preferred |
| todo_tag_remove | `position_id:1` | `task_id:"#49"` | task_id preferred |

**Parameter formats:**
- `task_id` = String with # prefix: `"#49"` (pattern: `^#\d+$`)
- `position_id` = Plain number: `1`, `2`, `3`

## Reliable Workflow Pattern

```bash
# 1. Create task - capture the returned task_id
result = mcp__todo-mcp__todo_create content:"Implement feature" estimate_minutes:60
# Response includes: «#49»

# 2. Use task_id for ALL subsequent operations
mcp__todo-mcp__todo_start task_id:"#49"
mcp__todo-mcp__todo_pause task_id:"#49" reason:"Waiting on API"
mcp__todo-mcp__todo_resume task_id:"#49"
mcp__todo-mcp__todo_complete task_id:"#49"
```

**Why this is reliable:**
- task_id "#49" never changes
- Works even if other tasks are added/completed/reordered
- No need to re-fetch the list between operations

## Task Lifecycle (Enforced)

```
Created → Started (in_progress) → Completed
              ↓
           Paused → Resumed → Completed
```

**Rules:**
1. Must `todo_start` before `todo_complete` (enforced)
2. Only ONE task can be `in_progress` at a time (auto-enforced)
3. Starting a new task automatically pauses the current one

```bash
# Correct sequence
mcp__todo-mcp__todo_start task_id:"#49"
# ... do work ...
mcp__todo-mcp__todo_complete task_id:"#49"

# This will fail - not started
mcp__todo-mcp__todo_complete task_id:"#49"  # Error!
```

## Parameter Data Types

```bash
# CORRECT types
estimate_minutes:60          # Number (not string)
priority:"high"              # String: critical/high/medium/low/backlog (lowercase)
status:"in_progress"         # String: created/in_progress/paused/completed
force:true                   # Boolean (not string)
include_context:true         # Boolean (not string)
task_id:"#49"                # String with # prefix
position_id:1                # Number (no quotes)
tags:["urgent","backend"]    # Array of strings

# WRONG - will fail
estimate_minutes:"60"        # String not accepted
priority:"HIGH"              # Must be lowercase
force:"true"                 # Must be boolean, not string
task_id:49                   # Must be string with #
```

## Complete Function Reference

### Task Creation

**todo_create** - Create single task
```bash
mcp__todo-mcp__todo_create content:"Fix authentication bug" estimate_minutes:30 priority:"high"
```
Required: `content`, `estimate_minutes`
Optional: `priority` (default: medium)
Returns: Task with task_id (e.g., «#49»)

**todo_batch_create** - Create multiple tasks
```bash
# Array mode
mcp__todo-mcp__todo_batch_create tasks:[{content:"Task 1",estimate_minutes:30},{content:"Task 2",estimate_minutes:45}]

# Markdown mode
mcp__todo-mcp__todo_batch_create markdown:"- [ ] Task 1\n- [ ] Task 2"
```

### Task Operations

**todo_start** - Begin working on task
```bash
mcp__todo-mcp__todo_start task_id:"#49"
```
Sets status to `in_progress`, starts time tracking.

**todo_pause** - Pause current work
```bash
mcp__todo-mcp__todo_pause task_id:"#49" reason:"Blocked on review"
```
Sets status to `paused`, preserves elapsed time.

**todo_resume** - Resume paused task
```bash
mcp__todo-mcp__todo_resume task_id:"#49"
```
Sets status back to `in_progress`, continues time tracking.

**todo_complete** - Mark task done
```bash
mcp__todo-mcp__todo_complete task_id:"#49"
```
Requires task to be `in_progress`. Records completion time.

**todo_update** - Modify task properties
```bash
mcp__todo-mcp__todo_update task_id:"#49" content:"Updated description" priority:"critical"
```
Supports: content, status, priority, estimate_minutes, tags, dependencies, sequence

**Position-Based Priority Adoption (v1.1.0):** The `sequence` parameter is now a display position. Moving to a position adopts that position's priority:
```bash
mcp__todo-mcp__todo_update task_id:"#49" sequence:2  # Moves to position 2, adopts its priority
```

**todo_delete** - Permanently remove task
```bash
mcp__todo-mcp__todo_delete task_id:"#49"
```

**todo_estimate** - Update time estimate
```bash
mcp__todo-mcp__todo_estimate task_id:"#49" minutes:120
```

### Tag Operations

**todo_tag_add** - Add tags to task
```bash
mcp__todo-mcp__todo_tag_add task_id:"#49" tags:["urgent","backend"]
```
Features 85% similarity detection to prevent near-duplicates.

**todo_tag_remove** - Remove tags from task
```bash
mcp__todo-mcp__todo_tag_remove task_id:"#49" tags:["old-tag"]
```

### Task Discovery

**todo_list** - List all tasks
```bash
mcp__todo-mcp__todo_list
mcp__todo-mcp__todo_list status:"created"
mcp__todo-mcp__todo_list status:"in_progress"
```
Returns tasks in Standard Task Format (STF).

**todo_next** - Get recommended next task
```bash
mcp__todo-mcp__todo_next
mcp__todo-mcp__todo_next tags:["backend"]
mcp__todo-mcp__todo_next priority:"high"
```
Uses intelligent algorithm: Dependencies → Priority → Sequence → Age

### Bulk Operations

**todo_bulk** - Atomic operations on multiple tasks
```bash
# By task_ids (preferred)
mcp__todo-mcp__todo_bulk operation:"set_priority" task_ids:["#12","#15"] data:{priority:"high"}

# By filters
mcp__todo-mcp__todo_bulk operation:"add_tags" filters:{status:"created"} data:{tags:["sprint-1"]}
```
Operations: add_tags, remove_tags, set_priority, set_status, resequence
All-or-nothing: if any task fails, entire operation rolls back.

**todo_archive** - Archive completed tasks
```bash
mcp__todo-mcp__todo_archive
```
Exports to markdown then clears. Safe approach.

**todo_clear_all** - Clear all tasks
```bash
mcp__todo-mcp__todo_clear_all force:true
```
Requires `force:true` to delete incomplete tasks.

### Context Operations

**context_set** - Store persistent data
```bash
mcp__todo-mcp__context_set key:"lesson_estimation" value:"Always double initial estimates"
```

**context_get** - Retrieve by key or pattern
```bash
mcp__todo-mcp__context_get key:"lesson_estimation"
mcp__todo-mcp__context_get pattern:"lesson_*"
mcp__todo-mcp__context_get pattern:"temp_*" minutes_back:60
```

**context_get_all** - Full context inventory
```bash
mcp__todo-mcp__context_get_all
```

**context_delete** - Remove by key or pattern
```bash
mcp__todo-mcp__context_delete key:"temp_calculation"
mcp__todo-mcp__context_delete pattern:"temp_*"
mcp__todo-mcp__context_delete pattern:"temp_*" dry_run:true  # Preview only
```

**context_clear** - Delete ALL context
```bash
mcp__todo-mcp__context_clear force:true
```
Requires `force:true`. Irreversible.

**context_stats** - Context health metrics
```bash
mcp__todo-mcp__context_stats
```

**context_resume** - Recovery command
```bash
mcp__todo-mcp__context_resume
```
Shows: active tasks, recent context, recommendations. Primary recovery command.

### Session & Project

**session_summary** - Today's work summary
```bash
mcp__todo-mcp__session_summary
```

**project_status** - Comprehensive project health
```bash
mcp__todo-mcp__project_status
```

**project_dump** - Full backup
```bash
mcp__todo-mcp__project_dump include_context:true
```

**project_restore** - Restore from backup
```bash
mcp__todo-mcp__project_restore file:"project_dump_20250101_120000.json" mode:"replace" include_context:true
```
Modes: replace, merge, append

### Export

**todo_export** - Export to various formats
```bash
mcp__todo-mcp__todo_export format:"markdown"
mcp__todo-mcp__todo_export format:"json"
mcp__todo-mcp__todo_export format:"csv"
```

**export_markdown** - Clean markdown output
```bash
mcp__todo-mcp__export_markdown include_completed:true
```

**version** - Server version info
```bash
mcp__todo-mcp__version
```

## Standard Task Format (STF)

```
N. [status] [priority] content (estimate) «#ID»
   🏷️ #tag1 #tag2
   ⛔ Blocked by: «#42»
```

**Status symbols:** ○ created, ◐ in_progress, ⊘ paused, ● completed
**Priority symbols:** ⚡ critical, 🔴 high, 🟡 medium, 🟢 low, 🔵 backlog

## Tag Auto-Extraction

When creating/updating tasks, tags are automatically extracted from content:

**Trailing tags removed:**
```
"Fix bug #urgent #backend" → Content: "Fix bug", Tags: [urgent, backend]
```

**Inline tags keep word:**
```
"Fix #urgent bug" → Content: "Fix urgent bug", Tags: [urgent]
```

## Priority and Execution Order

Default ordering hierarchy:
1. Dependencies (hard constraint)
2. Priority (critical → high → medium → low → backlog)
3. Within priority: non-sequenced before sequenced
4. Sequenced tasks in order (1, 2, 3...)
5. Creation time (tie-breaker)

**Position-Based Priority Adoption:** Moving a task to a display position (via `sequence` parameter or bulk `resequence`) causes the task to adopt the priority of that position. This enables intuitive drag-and-drop reordering where "move to top" naturally makes a task Critical.

## Best Practices Summary

1. **Always use task_id** for operations after creation
2. **Capture task_id** from create response immediately
3. **Start before complete** - enforced lifecycle
4. **One active task** - single focus rule
5. **Use context_resume** for session recovery
6. **Use project_dump** before risky operations
7. **Use todo_archive** instead of clear_completed
8. **Verify empty states** - run context_get_all if context_resume shows empty
