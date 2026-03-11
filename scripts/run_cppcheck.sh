#!/bin/bash
set -eo pipefail

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
SRC_DIR="$SCRIPT_DIR/.."

# Ensure build directory exists and compile_commands.json is generated
if [ ! -d "$SRC_DIR/build" ] || [ ! -f "$SRC_DIR/build/compile_commands.json" ]; then
    cmake -B "$SRC_DIR/build" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
fi

echo "Running Static Analysis..."
echo

# Create a function to generate the summary
generate_summary() {
    {
        # Initialize flag
        has_critical_issues=false

        echo

        # Always generate and display summary regardless of exit code
        echo "=== Static Analysis Summary ==="
        echo

        # Display critical issues
        echo "Critical Issues (Errors & Warnings):"
        echo "-----------------------------------"
        grep -E "error:|warning:" cppcheck_output.txt | grep -v "Checking " | grep -v "nofile:0:" | \
            sort | uniq | awk -F': ' '{printf "%-40s %s\n", $1, $4}' || echo "None found"
        echo

        # Display performance issues
        echo "Performance & Portability Issues:"
        echo "--------------------------------"
        grep -E "performance:|portability:" cppcheck_output.txt | grep -v "Checking " | \
            sort | uniq | awk -F': ' '{printf "%-40s %s\n", $1, $4}' || echo "None found"
        echo

        # Count issues by severity
        echo " Issue Count by Severity: "
        echo "--------------------------"
        for sev in error warning performance portability style missingInclude information debug; do
            count=$(grep -c "${sev}:" cppcheck_output.txt) || true
            printf "%-15s %3d issues\n" "${sev^^}:" "$count"

            # Check if 'sev' is 'error' or 'warning' and if 'count' is greater than 0
            if [[ "$sev" == "error" || "$sev" == "warning" ]] && [ "$count" -gt 0 ]; then
                has_critical_issues=true
            fi
        done
        echo

        # Display status and details
        if $has_critical_issues; then
            echo "Status: FAILED - Critical issues found"
            echo
            echo "Review and fix critical issues before proceeding"
        else
            echo "Status: PASSED - No critical issues found"
            echo
            echo "Note: Review non-critical issues for potential improvements"
        fi
    }

    # Return 1 if critical issues found, 0 otherwise
    if $has_critical_issues; then
        return 1
    else
        return 0
    fi
}

# Run cppcheck and capture output and exit code
cppcheck \
    --enable=all \
    --inconclusive \
    --std=c11 \
    --force \
    --inline-suppr \
    --suppress=unusedFunction \
    -i test \
    -i build/_deps \
    --template="{file}:{line}: {severity}: {id}: {message}" \
    --max-configs=32 \
    --check-library \
    --debug-warnings \
    . 2>&1 | tee cppcheck_output.txt

generate_summary

# Exit with cppcheck's status code
exit $?
