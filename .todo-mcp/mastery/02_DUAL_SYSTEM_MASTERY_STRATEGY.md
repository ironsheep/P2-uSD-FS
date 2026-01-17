# Dual System Mastery Strategy: TodoWrite + Todo MCP

## The Iron Rule (Learned from P2KB Violation)

> **TodoWrite NEVER tracks multiple Todo MCP task IDs**  
> **TodoWrite is ONLY a scratch pad for the CURRENT single MCP task**

## System Architecture

### Todo MCP (Persistent Layer)
- **Purpose**: Project features, session-spanning work, permanent tracking
- **Persistence**: Survives crashes, compaction, session changes
- **Identity**: Permanent task IDs «#N» for automation and reference
- **Lifecycle**: Multi-session, can span days/weeks
- **Backup**: Full project dumps with context preservation

### TodoWrite (Tactical Layer)  
- **Purpose**: Implementation steps, test fixes, current task breakdown
- **Persistence**: Temporary, cleared on MCP task completion
- **Identity**: Sequential steps for human cognitive scanning
- **Lifecycle**: Single session, supports one MCP task only
- **Backup**: Context preservation when switching tasks

## The Standard Workflow

### 1. Strategic Planning (MCP)
```bash
# Create persistent task for main work
mcp__todo-mcp__todo_create content:"Implement authentication system" estimate_minutes:240 priority:"high"

# Start the MCP task (use task_id from creation response)
mcp__todo-mcp__todo_start task_id:"#N"
```

### 2. Tactical Breakdown (TodoWrite)
```bash
# Break down current MCP task into implementation steps
TodoWrite: [
  "Research authentication patterns",
  "Design API endpoints", 
  "Implement core auth logic",
  "Add comprehensive tests",
  "Integration with existing system"
]
```

### 3. Context Bridge (Critical for Recovery)
```bash
# ALWAYS save TodoWrite state to context for crash recovery
mcp__todo-mcp__context_set key:"task_#N_steps" value:'["✓Research patterns","Design API","Implement core","Add tests","Integration"]'
```

### 4. Work Execution
- Work through TodoWrite steps sequentially
- Update TodoWrite status (✓ for completed steps)
- Update context bridge after each major step
- Promote discoveries to new MCP tasks if needed

### 5. Task Completion
```bash
# Complete MCP task (task_id preferred for reliability)
mcp__todo-mcp__todo_complete task_id:"#N"

# Clear TodoWrite (clean slate for next task)
TodoWrite: []

# Clean up task-specific context
mcp__todo-mcp__context_delete pattern:"task_#N_*"
```

## Advanced Patterns

### Task Switching Protocol
```bash
# Pause current work (save state first)
mcp__todo-mcp__context_set key:"task_#42_steps" value:'["✓Step1","→Step2","Step3"]'
mcp__todo-mcp__todo_pause task_id:"#42" reason:"Blocked on review"
TodoWrite: []

# Resume later
mcp__todo-mcp__todo_resume task_id:"#42"
mcp__todo-mcp__context_get key:"task_#42_steps"
# Reconstruct TodoWrite from returned context
```

### Discovery Promotion Pattern
```bash
# Work in TodoWrite (disposable, fast exploration)
TodoWrite: ["Research echo behavior", "Test with PST", "Document findings"]

# Promote important discoveries to permanent MCP tasks
mcp__todo-mcp__todo_create content:"[CRITICAL] Echo filtering mechanism needs refactoring" estimate_minutes:90

# Clean TodoWrite when MCP task created
TodoWrite: []
```

### Sprint Integration
```bash
# MCP tasks define sprint scope
mcp__todo-mcp__todo_create content:"Feature A implementation #sprint-v2.1" estimate_minutes:180
mcp__todo-mcp__todo_create content:"Feature B testing #sprint-v2.1" estimate_minutes:120

# TodoWrite provides daily execution breakdown
TodoWrite: ["Implement Feature A API", "Unit tests", "Integration tests"]
```

## Context Management Strategy

### Persistent Context (Keep)
- `lesson_*` - Learning from mistakes and successes
- `workaround_*` - Solutions to known tool limitations
- `recovery_*` - Emergency procedures and backup info
- `friction_*` - Tool improvement opportunities

### Temporary Context (Delete)
- `temp_*` - Scratch calculations and temporary state  
- `current_*` - Active work status (delete when work complete)
- `session_*` - Session-specific state (delete at session end)
- `task_#N_*` - Task-specific tracking (delete when task complete)

### Context Hygiene Rules
1. **Copy TodoWrite to context** after every significant change
2. **Delete task context** immediately after task completion
3. **Regular cleanup** of temporary keys (age-based)
4. **Pattern-based deletion** for efficiency
5. **Context stats monitoring** to prevent bloat

## Quality Patterns

### The TodoWrite Discipline Violation (Never Repeat)
**WRONG**: Creating TodoWrite "project tracker" with multiple MCP task IDs
```bash
# ❌ NEVER DO THIS
TodoWrite: [
  "Task #523: Implement feature",
  "Task #524: Add tests", 
  "Task #525: Documentation"
]
```

**CORRECT**: One MCP task at a time
```bash
# ✅ ALWAYS DO THIS  
Current MCP Task: #523
TodoWrite: [
  "Research implementation patterns",
  "Design feature architecture", 
  "Implement core functionality",
  "Add comprehensive tests"
]
```

### Process Consistency Rule
**Autonomy changes WHO makes decisions, not HOW work gets done.**

Whether user is present or absent:
- Same TodoWrite discipline (single task only)
- Same context preservation patterns  
- Same quality standards
- Same recovery procedures

## Recovery Excellence

### Primary Recovery (2-Minute Protocol)
```bash
# Step 1: Quick state check (30s)
mcp__todo-mcp__context_resume

# Step 2: Full context if needed (30s) 
mcp__todo-mcp__context_get_all

# Step 3: Verify task state (30s)
mcp__todo-mcp__todo_list

# Step 4: Rebuild TodoWrite from context (30s)
# Reconstruct current task steps from context_get
```

### Context Resume Enhancement
- **10-minute recent window**: Shows context keys modified in last 10 minutes
- **Automatic grouping**: Context organized by prefix patterns
- **Age indicators**: Clear visibility into stale vs fresh context
- **Task integration**: Active and paused tasks with elapsed time

### Success Metrics
- **Data preservation**: 100% (MCP tasks + critical context)
- **Recovery time**: <2 minutes average
- **Work continuity**: Resume at exact interruption point
- **Quality consistency**: No degradation during recovery

## Anti-Patterns to Avoid

### Critical Violations
1. **Multiple MCP task IDs in TodoWrite** → Quality degradation
2. **Not copying TodoWrite to context** → Lost progress on crash
3. **Not cleaning task context after completion** → Context bloat
4. **Using TodoWrite for long-term tracking** → Wrong tool for purpose

### Medium Issues  
1. **Forgetting to start MCP tasks** → No time tracking
2. **Not promoting important discoveries** → Lost insights
3. **Manual context cleanup only** → Unsustainable maintenance
4. **Trusting empty states without verification** → Missing data

## Meta-Learning: Why This Works

### Cognitive Benefits
- **TodoWrite**: Immediate visibility into current work breakdown
- **MCP**: Persistent memory across sessions and interruptions  
- **Context**: Communication channel between past and future work

### Technical Benefits
- **Crash resilience**: Multiple recovery mechanisms
- **Context hygiene**: Systematic cleanup prevents memory issues
- **Quality consistency**: Same standards under all conditions
- **Discovery preservation**: Important insights become permanent tasks

### Process Benefits
- **Clear boundaries**: Each system has specific purpose
- **Scalable patterns**: Works for simple tasks and complex projects
- **Recovery confidence**: Systematic procedures build trust
- **Continuous improvement**: Friction documentation drives tool evolution

## Summary

The dual system strategy provides both **tactical visibility** (TodoWrite) and **strategic persistence** (Todo MCP) while maintaining **crash resilience** and **quality consistency**. 

**Key Success Formula**: One MCP task + TodoWrite breakdown + Context bridge + Clean completion = Reliable, recoverable, high-quality workflows.