# CLAUDE.md Integration Guide

## What To Do

If you have mastery documentation in `.todo-mcp/mastery/`, complete these steps:

1. **Read** the file `CLAUDE_MD_INSERT.md` in this setup folder
2. **Install Part 1** (Session Trigger) at the **TOP** of your CLAUDE.md
3. **Install Part 2** (Reference Material) in your CLAUDE.md body
4. **Begin** using the dual-system workflow immediately

## CRITICAL: Placement Matters

**Part 1 MUST be at the TOP of your CLAUDE.md file:**
```markdown
## ⚡ SESSION START (EXECUTE IMMEDIATELY)
**Run this command FIRST, before any other work:**
```bash
mcp__todo-mcp__context_resume
```
```

This ensures Claude executes it automatically at session start. Placement in the middle of the file causes it to be treated as "information" rather than an immediate action.

## After Installation

Every session should automatically start with:
```bash
mcp__todo-mcp__context_resume
```

Use the dual-system workflow:
- **MCP tasks**: Persistent, session-spanning work
- **TodoWrite**: Current task breakdown only (never multiple MCP task IDs)

## Success Indicators

You're using mastery patterns correctly when:
- [ ] Sessions start with `context_resume`
- [ ] Only ONE MCP task at a time in TodoWrite
- [ ] Context cleaned after task completion
- [ ] Backups before risky operations

## Available Documentation

This mastery folder contains:
- **01_TODO_MCP_MASTERY_INTERFACE.md** - Complete command reference
- **02_DUAL_SYSTEM_MASTERY_STRATEGY.md** - Workflow patterns
- **03_ANTI_PATTERN_ENFORCEMENT.md** - Quality protection
- **setup/CLAUDE_MD_INSERT.md** - Ready-to-deploy CLAUDE.md section
- **FRICTION_LOG.md** - Track your experience

Refer to these documents for comprehensive guidance on Todo MCP usage.