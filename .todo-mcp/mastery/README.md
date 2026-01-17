# Todo MCP Mastery Documentation

## Purpose

Enable immediate mastery-level Todo MCP operation without learning friction.

**Target Audience**: Claude instances operating with Todo MCP

## Core Documents

### [01_TODO_MCP_MASTERY_INTERFACE.md](01_TODO_MCP_MASTERY_INTERFACE.md)
**Complete interface reference**
- All MCP function patterns
- Parameter requirements and data types
- Task lifecycle and atomic operations
- Search, filter, and export capabilities

### [02_DUAL_SYSTEM_MASTERY_STRATEGY.md](02_DUAL_SYSTEM_MASTERY_STRATEGY.md)
**TodoWrite + Todo MCP integration**
- The Iron Rule: Single task discipline
- Standard workflow patterns
- Context bridge for crash recovery
- Quality consistency enforcement

### [03_ANTI_PATTERN_ENFORCEMENT.md](03_ANTI_PATTERN_ENFORCEMENT.md)
**Prevention and recovery mechanisms**
- Critical anti-patterns that break workflows
- Enforcement mechanisms and quality gates
- Systematic violation prevention

### [FRICTION_LOG.md](FRICTION_LOG.md)
**Track your experience**
- Record issues you encounter
- Document workarounds you discover
- Feedback improves future versions

## Setup Documents

### [setup/CLAUDE_MD_INSERT.md](setup/CLAUDE_MD_INSERT.md)
**Deploy-ready CLAUDE.md section**
- Drop-in guidance for any project
- Essential patterns and commands
- Recovery procedures

### [setup/INTEGRATION_GUIDE.md](setup/INTEGRATION_GUIDE.md)
**One-time setup instructions**
- How to merge mastery content into CLAUDE.md
- Success indicators checklist

## Quick Start

1. Follow [setup/INTEGRATION_GUIDE.md](setup/INTEGRATION_GUIDE.md) for initial deployment
2. Merge [setup/CLAUDE_MD_INSERT.md](setup/CLAUDE_MD_INSERT.md) into your project's CLAUDE.md
3. Reference core documents (01, 02, 03) for comprehensive guidance
4. Use [FRICTION_LOG.md](FRICTION_LOG.md) to track your experience

## Session Start Protocol

Always begin sessions with:
```bash
mcp__todo-mcp__context_resume
```

This restores your complete working state.

## Success Metrics

- **Recovery time**: <2 minutes from any interruption
- **Quality consistency**: Same standards autonomous vs guided
- **Data preservation**: 100% (no lost tasks or context)
