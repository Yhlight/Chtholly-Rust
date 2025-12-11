#!/bin/bash

# Define an array of tests that are expected to fail
declare -a failing_tests=(
    "./test_lifetime_dangle.cns"
    "./test_lifetime_dangle_intermediate.cns"
    "./test_lifetime_scope.cns"
    "./test_move.cns"
    "./test_ownership.cns"
    "./test_lifetime_dangle_3.cns"
)

# Function to check if a test is in the failing_tests array
is_expected_to_fail() {
    for test in "${failing_tests[@]}"; do
        if [[ "$test" == "$1" ]]; then
            return 0
        fi
    done
    return 1
}

# Counter for passed tests
passed_tests=0
total_tests=0

# Iterate over all .cns files
for f in $(find . -maxdepth 1 -name "*.cns"); do
    ((total_tests++))
    echo "----------------------------------------"
    echo "Compiling $f"
    ./build/chtholly "$f" > /dev/null 2>&1
    exit_code=$?

    if is_expected_to_fail "$f"; then
        if [ $exit_code -ne 0 ]; then
            echo "PASS (as expected): $f"
            ((passed_tests++))
        else
            echo "FAIL (unexpectedly passed): $f"
        fi
    else
        if [ $exit_code -eq 0 ]; then
            echo "PASS: $f"
            ((passed_tests++))
        else
            echo "FAIL (unexpectedly failed): $f"
        fi
    fi
done

echo "----------------------------------------"
echo "Summary: $passed_tests / $total_tests tests passed."

# Exit with a non-zero status code if any tests failed
if [ $passed_tests -ne $total_tests ]; then
    exit 1
fi
