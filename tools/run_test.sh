#!/bin/bash
#
# run_test.sh - Compile, download, run, and monitor P2 test files
#
# Usage: run_test.sh [basename] [srcdir] [-t timeout] [-m] [-l]
#        Run without arguments to use defaults below.
#
# ============================================================
# CURRENT TEST CONFIGURATION (edit these for your test)
# ============================================================
DEFAULT_BASENAME="RT_smoke_test"
DEFAULT_SRCDIR="../tests"
DEFAULT_TIMEOUT="60"
# ============================================================
#
# Arguments (override defaults):
#   basename  - Source file name without .spin2 extension
#   srcdir    - Directory containing the source file
#   -t <sec>  - Timeout in seconds
#   -m        - (optional) Generate memory map file
#   -l        - (optional) Generate listing file
#
# Exit codes:
#   0 - Test completed successfully (END_SESSION found)
#   1 - Compilation failed
#   2 - Download/run failed
#   3 - Timeout expired (END_SESSION not found in time)
#   4 - Usage error
#
# Requirements:
#   - Test source MUST have: DEBUG_BAUD = 2_000_000 in CON section
#   - Test MUST output END_SESSION via debug() when complete
#
# Uses pnut-term-ts headless mode for CI/AI agent automation.
#

set +e

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# --- Functions ---

usage() {
    echo "Usage: $0 [basename] [srcdir] [-t timeout] [-m] [-l]"
    echo ""
    echo "Run without arguments to use defaults:"
    echo "  BASENAME: $DEFAULT_BASENAME"
    echo "  SRCDIR:   $DEFAULT_SRCDIR"
    echo "  TIMEOUT:  $DEFAULT_TIMEOUT seconds"
    echo ""
    echo "Arguments (override defaults):"
    echo "  basename  - Source file name without .spin2 extension"
    echo "  srcdir    - Directory containing the source file"
    echo "  -t <sec>  - Timeout in seconds"
    echo "  -m        - Generate memory map file"
    echo "  -l        - Generate listing file"
    echo ""
    echo "Requirements:"
    echo "  - Source MUST have: DEBUG_BAUD = 2_000_000 in CON section"
    echo "  - Test MUST output END_SESSION via debug() when complete"
    echo ""
    echo "Exit codes:"
    echo "  0 - Test passed (END_SESSION found)"
    echo "  1 - Compilation failed"
    echo "  2 - Download/run failed"
    echo "  3 - Timeout (END_SESSION not found)"
    echo "  4 - Usage error"
    exit 4
}

# --- Parse Arguments (use defaults if not provided) ---

BASENAME=""
SRCDIR=""
TIMEOUT_SECS=""
MAP_FLAG=""
LIST_FLAG=""

# Parse positional args first (basename, srcdir)
while [[ $# -gt 0 && ! "$1" == -* ]]; do
    if [[ -z "$BASENAME" ]]; then
        BASENAME="$1"
    elif [[ -z "$SRCDIR" ]]; then
        SRCDIR="$1"
    fi
    shift
done

# Parse optional flags
while [[ $# -gt 0 ]]; do
    case "$1" in
        -t)
            if [[ -z "$2" || "$2" == -* ]]; then
                echo -e "${RED}Error: -t requires a timeout value in seconds${NC}"
                usage
            fi
            TIMEOUT_SECS="$2"
            shift 2
            ;;
        -m) MAP_FLAG="-m"; shift ;;
        -l) LIST_FLAG="-l"; shift ;;
        -h|--help) usage ;;
        *) echo -e "${RED}Error: Unknown option: $1${NC}"; usage ;;
    esac
done

# Apply defaults for any missing values
BASENAME="${BASENAME:-$DEFAULT_BASENAME}"
SRCDIR="${SRCDIR:-$DEFAULT_SRCDIR}"
TIMEOUT_SECS="${TIMEOUT_SECS:-$DEFAULT_TIMEOUT}"

# Validate timeout is numeric
if ! [[ "$TIMEOUT_SECS" =~ ^[0-9]+$ ]]; then
    echo -e "${RED}Error: Timeout must be a positive integer${NC}"
    usage
fi

echo -e "${CYAN}Using: BASENAME=$BASENAME, SRCDIR=$SRCDIR, TIMEOUT=$TIMEOUT_SECS${NC}"

# --- Validate Arguments ---

if [[ ! -d "$SRCDIR" ]]; then
    echo -e "${RED}Error: Source directory does not exist: $SRCDIR${NC}"
    exit 4
fi

SOURCE_FILE="$SRCDIR/$BASENAME.spin2"
if [[ ! -f "$SOURCE_FILE" ]]; then
    echo -e "${RED}Error: Source file does not exist: $SOURCE_FILE${NC}"
    exit 4
fi

# --- Compilation ---

echo -e "${GREEN}=== Compiling $BASENAME.spin2 ===${NC}"

cd "$SRCDIR"

COMPILE_CMD="pnut-ts -d -I ../src $MAP_FLAG $LIST_FLAG $BASENAME.spin2"
echo "  Command: $COMPILE_CMD"

if ! $COMPILE_CMD; then
    echo -e "${RED}=== Compilation FAILED ===${NC}"
    exit 1
fi

BIN_FILE="$BASENAME.bin"
if [[ ! -f "$BIN_FILE" ]]; then
    echo -e "${RED}Error: Binary file not generated: $BIN_FILE${NC}"
    exit 1
fi

echo -e "${GREEN}=== Compilation successful ===${NC}"

# --- Run Test with Headless Mode ---

echo ""
echo -e "${GREEN}=== Running test (headless mode) ===${NC}"
echo "  Binary:  $BIN_FILE"
echo "  Timeout: ${TIMEOUT_SECS} seconds"
echo "  Baud:    2000000"
echo ""

# Record time before running (to find new log files)
BEFORE_TIME=$(date +%s)

# Run pnut-term-ts in headless mode
# Exit codes: 0 = END_SESSION found, 124 = timeout
pnut-term-ts --headless -r "$BIN_FILE" -b 2000000 --end-marker --timeout "$TIMEOUT_SECS"
PNUT_EXIT_CODE=$?

echo ""

# --- Find the log file ---

# Find newest headless_*.log created after we started
LOG_FILE=""
if [[ -d "./logs" ]]; then
    for f in $(ls -t ./logs/headless_*.log 2>/dev/null); do
        # Get file modification time
        if [[ "$(uname)" == "Darwin" ]]; then
            FILE_TIME=$(stat -f %m "$f" 2>/dev/null)
        else
            FILE_TIME=$(stat -c %Y "$f" 2>/dev/null)
        fi
        if [[ "$FILE_TIME" -ge "$BEFORE_TIME" ]]; then
            LOG_FILE="$f"
            break
        fi
    done
fi

# --- Report Results ---

case $PNUT_EXIT_CODE in
    0)
        echo -e "${GREEN}=== TEST PASSED (END_SESSION detected) ===${NC}"
        ;;
    124)
        echo -e "${RED}=== TEST FAILED (Timeout after ${TIMEOUT_SECS}s) ===${NC}"
        ;;
    *)
        echo -e "${RED}=== TEST FAILED (Exit code: $PNUT_EXIT_CODE) ===${NC}"
        ;;
esac

# Output log file path
if [[ -n "$LOG_FILE" ]]; then
    echo ""
    echo -e "${CYAN}Log file: $LOG_FILE${NC}"
    echo ""
    echo "Last 20 lines:"
    echo "----------------------------------------"
    tail -20 "$LOG_FILE"
    echo "----------------------------------------"
else
    echo ""
    echo -e "${YELLOW}No log file found${NC}"
fi

# Exit with appropriate code
case $PNUT_EXIT_CODE in
    0)   exit 0 ;;
    124) exit 3 ;;
    *)   exit 2 ;;
esac
