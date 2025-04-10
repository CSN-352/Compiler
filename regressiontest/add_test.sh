#!/bin/sh

set -e

# Expected usage:
# ./add_test.sh input.c input.txt

SRC_DIR="../src"
TESTS_DIR="./tests"

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
NEXT_NUM=1
while [ -d "$TESTS_DIR/test$NEXT_NUM" ]; do
    NEXT_NUM=$((NEXT_NUM + 1))
done

# Create test directory
NEW_TEST_DIR="$TESTS_DIR/test$NEXT_NUM"
mkdir -p "$NEW_TEST_DIR"

# Copy and rename files
cp "$SRC_DIR/$C_FILE" "$NEW_TEST_DIR/test$NEXT_NUM.c"
cp "$SRC_DIR/$TXT_FILE" "$NEW_TEST_DIR/test$NEXT_NUM.txt"

echo "✅ Added test$NEXT_NUM:"
echo "  - $NEW_TEST_DIR/test$NEXT_NUM.c"
echo "  - $NEW_TEST_DIR/test$NEXT_NUM.txt"
