#!/bin/bash

#
# This script will automatically format all version-controlled C and C++ files
# in the repository using the astyle tool with the One True Brace Style (OTBS).
# If a file shouldn't be formatted, it needs to be listed in the EXCLUDE_FILES
# array below with a path relative to the source code root.
#
# Options:
#   --check: After formatting, check if any necessary formatting changes weren't
#            committed. If there are uncommitted changes, fail. This allows us
#            to ensure all committed code has been formatted prior to being
#            merged.
#

if [[ ! $(which astyle) ]]; then
    echo "astyle binary not found. Please install it and try again."
    exit 1
fi

# These files won't be formatted.
EXCLUDE_FILES=(
    test/include/fff.h
)

CHECK=0

while [[ "$#" -gt 0 ]]; do
    case $1 in
        --check) CHECK=1 ;;
        *) echo "Unknown parameter: $1"; exit 1 ;;
    esac
    shift
done

if [ $CHECK -eq 1 ]; then
    if [[ `git status --porcelain --untracked=no` ]]; then
        echo "Uncommitted changes detected. Please commit or discard any working tree changes prior to running this script with --check."
        exit 1
    fi
fi

# Run astyle on all version-controlled C and C++ files.
for FILE in $(git ls-files '*.c' '*.h' '*.cpp' '*.hpp')
do
    # Skip files listed in EXCLUDE_FILES.
    if [[ ! " ${EXCLUDE_FILES[*]} " =~ " ${FILE} " ]]; then
        astyle --suffix=none --style=otbs $FILE
    fi
done

# If --check was given, fail the script if running astyle produced any new,
# uncommitted changes.
if [ $CHECK -eq 1 ]; then
    if [[ `git status --porcelain --untracked=no` ]]; then
        echo "Unformatted files detected. Run scripts/run_astyle.sh and amend your commit with the resulting formatting changes."
        exit 1
    else
        echo "Formatting clean."
    fi
fi
