#!/bin/bash

BASE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && cd .. && pwd)"

SRC="$BASE_DIR/src/parser"
TEST_DIR="$BASE_DIR/Lexer//Lexer/test/lexer_tests"
OUTPUT_DIR="$BASE_DIR/test/output"

# Clean function
if [[ "$1" == "clean" ]]; then
    echo "Cleaning output directory..."
    rm -rf "$OUTPUT_DIR"
    echo "Clean complete."
    exit 0
fi

# Find all test files in subdirectories
test_files=$(find "$TEST_DIR" -type f -name "*.c")

# Ensure output directory exists
mkdir -p "$OUTPUT_DIR"

# Define a fixed column width for alignment
COL_WIDTH=30

# Run tests
for test_file in $test_files; do
    test_name=$(basename "$test_file" .c)  # Extract filename without extension
    output_file="$OUTPUT_DIR/${test_file#$TEST_DIR/}"
    output_file="${output_file%.c}.txt"

    # Ensure subdirectories exist
    mkdir -p "$(dirname "$output_file")"

    # Print test name with fixed width
    printf "Running test: %-*s" "$COL_WIDTH" "$test_name"

    # Run the test, capturing stdout and stderr separately
    "$SRC" "$test_file" > "$output_file" 2> "${output_file%.txt}.err"

    # Check if there were any errors
    if [[ -s "${output_file%.txt}.err" ]]; then
        echo "❌"
        rm -f "$output_file"
    else 
        echo "✅"
        rm -f "${output_file%.txt}.err"
    fi
done

echo "All tests completed."
