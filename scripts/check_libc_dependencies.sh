#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
ROOT_SRC_DIR="$SCRIPT_DIR/.."

if [[ ! -f "$ROOT_SRC_DIR/CMakeLists.txt" ]]; then
    echo "Failed to find note-c root directory. (did the location of check_libc_dependencies.sh change?)"
    exit 1
fi

pushd $ROOT_SRC_DIR $@ > /dev/null

CMAKE_OPTIONS="-DNOTE_C_NO_LIBC=1"

cmake -B build/ $CMAKE_OPTIONS
if [[ $? -ne 0 ]]; then
    echo "Failed to run CMake."
    popd $@ > /dev/null
    exit 1
fi

PERMITTED_FNS=(
    # The mem* functions are ok.
    "memchr"
    "memcmp"
    "memcpy"
    "memmove"
    "memset"
    # These string functions are ok.
    "strchr"
    "strcmp"
    "strlen"
    "strncmp"
    "strstr"
    # strtol comes from us using atoi in NoteGetEnvInt.
    "strtol"
    # Used by NotePrintf.
    "vsnprintf"
)

# Function to check if an element is in an array.
element_in_array() {
    local element="$1"
    local array=("${@:2}")

    for item in "${array[@]}"; do
        if [ "$item" == "$element" ]; then
            return 0  # Element found in array
        fi
    done
    return 1  # Element not found in array
}

BUILD_OUTPUT=$(cmake --build build/ -j 2>&1)
if [[ $? -ne 0 ]]; then
    # Iterate over the lines from the build output to get all the undefined
    # references.
    UNDEF_REFS=()
    while IFS= read -r LINE; do
        PATTERN="undefined reference to \`(.*)'"
        if [[ $LINE =~ $PATTERN ]]; then
            UNDEF_REFS+=("${BASH_REMATCH[1]}")
        fi
    done <<< "$BUILD_OUTPUT"

    # Remove duplicates
    UNDEF_REFS=($(printf "%s\n" "${UNDEF_REFS[@]}" | sort -u))

    # Check if each function that caused an undefined reference error is
    # permitted.
    FAIL=0
    for UNDEF_REF in "${UNDEF_REFS[@]}"; do
        if element_in_array "$UNDEF_REF" "${PERMITTED_FNS[@]}"; then
            echo "$UNDEF_REF is permitted."
        else
            echo "$UNDEF_REF is NOT permitted"
            FAIL=1
        fi
    done

    if [ "$FAIL" -eq 1 ]; then
        echo "Unpermitted libc functions found."
        popd $@ > /dev/null
        exit 1
    fi
else
    echo "Build unexpectedly succeeded. The build should fail because certain permitted libc functions shouldn't be found when linking."
    popd $@ > /dev/null
    exit 1
fi

popd $@ > /dev/null
