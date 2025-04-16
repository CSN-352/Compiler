#!/bin/bash

BASE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && cd .. && pwd)"

SRC="$BASE_DIR/src/build/bin/compiler"
TEST_DIR="$BASE_DIR/test"
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
    test_name=$(basename "$test_file" .c)
    relative_path="${test_file#$TEST_DIR/}"
    output_txt="$OUTPUT_DIR/${relative_path%.c}.txt"
    output_err="$OUTPUT_DIR/${relative_path%.c}.err"

    rm -f "$output_txt" "$output_err"

    # Ensure subdirectories exist
    mkdir -p "$(dirname "$output_txt")"

    # Print test name
    printf "Running test: %-*s" "$COL_WIDTH" "$test_name"

    # Run the parser and capture stdout and stderr
    stdout_file=$(mktemp)
    stderr_file=$(mktemp)
    "$SRC" "$test_file" >"$stdout_file" 2>"$stderr_file"
    exit_code=$?

    # Determine outcome
    if [[ $exit_code -eq 0 && ! -s "$stderr_file" ]]; then
        mv "$stdout_file" "$output_txt"
        rm -f "$stderr_file"
        echo "✅"
    else
        if [[ $exit_code -eq 139 ]]; then
            echo "💥 Segmentation Fault" > "$stderr_file"
        elif [[ $exit_code -ne 0 ]]; then
            echo "❌ Exit Code $exit_code" >> "$stderr_file"
        fi
        cat "$stdout_file" >> "$stderr_file"
        mv "$stderr_file" "$output_err"
        rm -f "$stdout_file"
        echo "❌"
    fi
done

echo "All tests completed."
