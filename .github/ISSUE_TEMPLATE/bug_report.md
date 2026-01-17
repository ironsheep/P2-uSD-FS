---
name: Bug Report
about: Report a bug or unexpected behavior
title: '[BUG] '
labels: bug
assignees: ''
---

## Bug Description

A clear and concise description of what the bug is.

## Environment

- **P2 Board**: (e.g., P2 Edge Module, P2 Eval Board, custom)
- **SD Card**: (brand, capacity, type - e.g., SanDisk 32GB SDHC)
- **Card Format**: (how was the card formatted - Windows, macOS, Linux, P2 formatter)
- **Compiler**: (e.g., pnut-ts 1.51.7, FlexProp 6.x)
- **Driver Version**: (e.g., v1.0.0)

## Steps to Reproduce

1.
2.
3.

## Expected Behavior

What you expected to happen.

## Actual Behavior

What actually happened.

## Debug Output

If applicable, paste relevant debug output here:

```
[paste debug output here]
```

## Minimal Code Example

If possible, provide a minimal code example that reproduces the issue:

```spin2
' Minimal example that reproduces the bug
OBJ
    sd : "SD_card_driver"

PUB go()
    ' Your code here
```

## Additional Context

Add any other context about the problem here. Screenshots, logic analyzer captures, etc.

## Checklist

- [ ] I have searched existing issues to ensure this is not a duplicate
- [ ] I have tested with the latest version of the driver
- [ ] I have tried with a different SD card (if applicable)
