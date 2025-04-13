#!/bin/sh

set -e

# Get the directory where this script lives (always correct no matter where it's called from)
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SRC_DIR="$SCRIPT_DIR/../src"
TESTS_DIR="$SCRIPT_DIR/tests"

C_FILE=$1
TXT_FILE=$2

if [ -z "$C_FILE" ] || [ -z "$TXT_FILE" ]; then
    echo "Usage: $0 <input.c> <input.txt>"
    exit 1
fi

# Check if input files exist
if [ ! -f "$SRC_DIR/$C_FILE" ]; then
    echo "Error: '$SRC_DIR/$C_FILE' not found."
    exit 1
fi

if [ ! -f "$SRC_DIR/$TXT_FILE" ]; then
    echo "Error: '$SRC_DIR/$TXT_FILE' not found."
    exit 1
fi

# Find next test number
NEXT_NUM=$(find "$TESTS_DIR" -maxdepth 1 -type d -name "test[0-9]*" | sed -E 's/.*test([0-9]+)/\1/' | sort -n | tail -n 1)
if [ -z "$NEXT_NUM" ]; then
    NEXT_NUM=1
else
    NEXT_NUM=$((NEXT_NUM + 1))
fi

# Create test directory
NEW_TEST_DIR="$TESTS_DIR/test$NEXT_NUM"
mkdir -p "$NEW_TEST_DIR"

# Copy and rename files
cp "$SRC_DIR/$C_FILE" "$NEW_TEST_DIR/test$NEXT_NUM.c"
cp "$SRC_DIR/$TXT_FILE" "$NEW_TEST_DIR/test$NEXT_NUM.txt"

echo "✅ Added test$NEXT_NUM:"
echo "  - $NEW_TEST_DIR/test$NEXT_NUM.c"
echo "  - $NEW_TEST_DIR/test$NEXT_NUM.txt"
