#!/bin/sh
# ./add_test.sh -u ../test/basic/arithmetic_logical.c ../src/output.txt :: To update previous reg tests 
# ./add_test.sh ../test/basic/arithmetic_logical.c ../src/output.txt :: To add new tests
set -e

if [[ $# -ne 2 && $# -ne 3 ]]; then
    usage # run usage function
    exit 1
fi

UPDATE_FLAG=0
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SRC_DIR="$SCRIPT_DIR/../src"
TESTS_DIR="$SCRIPT_DIR/tests"
 
if [ $1 == "-u" ]; then
    UPDATE_FLAG=1
    C_FILE=$2
    TXT_FILE=$3
    C_FILE_NAME=$(basename "$C_FILE")
    C_FILE_NAME=$(echo $C_FILE_NAME | cut -d "." -f 1)
    echo ${TESTS_DIR}/${C_FILE_NAME}
    rm -rf ${TESTS_DIR}/${C_FILE_NAME}*
else 
    C_FILE=$1
    TXT_FILE=$2
fi

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

create_test_dir() {

    C_FILE_NAME=$(basename "$C_FILE")
    C_FILE_NAME=$(echo $C_FILE_NAME | cut -d "." -f 1)
    if [ ! -d "$TESTS_DIR/$C_FILE_NAME" ]; then
        echo "$TESTS_DIR/$C_FILE_NAME"
    else 
        i=1
        while [ -d "$TESTS_DIR/${C_FILE_NAME}_${i}" ]; do
            i=$((i+1))
        done
        echo "$TESTS_DIR/${C_FILE_NAME}_${i}"
    fi
}

# Create test directory
NEW_TEST_DIR=$(create_test_dir)
echo $NEW_TEST_DIR
mkdir -p "$NEW_TEST_DIR"

# Function to copy file with conflict resolution
copy_with_unique_name() {
    src_path="$1"
    dest_dir="$2"
    filename=$(basename "$src_path")

    if [ ! -f "$dest_dir/$filename" ]; then
        cp "$src_path" "$dest_dir/$filename"
        echo "  - $dest_dir/$filename"
    else
        i=1
        while [ -f "$dest_dir/${i}_$filename" ]; do
            i=$((i + 1))
        done
        cp "$src_path" "$dest_dir/${i}_$filename"
        echo "  - $dest_dir/${i}_$filename"
    fi
}

echo "✅ Added test: $C_FILE"
copy_with_unique_name "$SRC_DIR/$C_FILE" "$NEW_TEST_DIR"
copy_with_unique_name "$SRC_DIR/$TXT_FILE" "$NEW_TEST_DIR"
