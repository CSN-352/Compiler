#!/bin/bash

set -e

TESTS_DIR="./tests"
OUTPUT_DIR="./output"
COMPILER="../src/build/bin/parser"  # adjust as per your actual binary

mkdir -p "$OUTPUT_DIR"

echo "🔍 Running tests..."
echo

FAILURES=0
TOTAL=0

for test_folder in "$TESTS_DIR"/*/; do
    TEST_FILE="$test_folder*.c"
    TEST_FILE=$(echo $TEST_FILE)
    EXPECTED_OUTPUT="$test_folder*.txt"
    EXPECTED_OUTPUT=$(echo $EXPECTED_OUTPUT)
    ACTUAL_OUTPUT="$OUTPUT_DIR/$(basename $test_folder).txt"

    echo "▶️  Test $TEST_NUM:"

    if [ ! -f "$TEST_FILE" ]; then
        echo "❌ Missing input file: $TEST_FILE"
        continue
    fi

    if [ ! -f "$EXPECTED_OUTPUT" ]; then
        echo "❌ Missing expected output: $EXPECTED_OUTPUT"
        continue
    fi

    # Run the compiler and capture output
    if ! "$COMPILER" "$TEST_FILE" > "$ACTUAL_OUTPUT" 2>&1; then
        echo "💥 Execution failed for $TEST_FILE"
        FAILURES=$((FAILURES + 1))
        TOTAL=$((TOTAL + 1))
        continue
    fi

    # Compare output
    if diff -q "$EXPECTED_OUTPUT" "$ACTUAL_OUTPUT" > /dev/null; then
        echo "✅ PASSED"
    else
        echo "❌ FAILED"
        echo "   Diff:"
        diff "$EXPECTED_OUTPUT" "$ACTUAL_OUTPUT" || true
        FAILURES=$((FAILURES + 1))
    fi

    TOTAL=$((TOTAL + 1))
    echo
done

echo "🎯 Test Summary: $((TOTAL - FAILURES)) / $TOTAL passed"
exit $FAILURES
