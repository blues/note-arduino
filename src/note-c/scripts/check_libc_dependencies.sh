#!/bin/bash

# Determine the directory of the script.
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
ROOT_SRC_DIR="$SCRIPT_DIR/.."

# Check for a CMakeLists.txt file, to ensure CMake operations
# are executed in the correct directory.
if [[ ! -f "$ROOT_SRC_DIR/CMakeLists.txt" ]]; then
    echo "ERROR: Unable to locate ${ROOT_SRC_DIR}/CMakeList.txt. Expecting to use '..' to access root directory. Did the location of check_libc_dependencies.sh change?"
    exit 1
fi

# Move to the root source directory.
pushd $ROOT_SRC_DIR $@ > /dev/null

# Whitelist of permitted libc functions.
LIBC_WHITELIST=(
    # These math functions are ok
    __divdi3    # dividing double-integer (64-bit) values
    __udivdi3   # unsigned dividing double-integer (64-bit) values
    __umoddi3   # unsigned modulo double-integer (64-bit) values
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
    "strtol"    # required by atoi in NoteGenEnvInt
    "vsnprintf" # required by NotePrintf
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

# Configure the build with the option to exclude libc.
cmake -B build/ -DNOTE_C_NO_LIBC=1
if [[ $? -ne 0 ]]; then
    echo "ERROR: CMake failed to configure build."
    popd > /dev/null
    exit 1
fi

# Build the project without `libc`. Then capture the build errors to parse for
# undefined references. Next, compile a list of the undefined references, and
# confirm they are found in our whitelist of permitted `libc` functions.
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
    echo "Performing 'libc' reference scan..."
    echo
    for UNDEF_REF in "${UNDEF_REFS[@]}"; do
        if element_in_array "$UNDEF_REF" "${LIBC_WHITELIST[@]}"; then
            echo " [OK]  $UNDEF_REF"
        else
            echo "[FAIL] $UNDEF_REF"
            FAIL=1
        fi
    done

    if [ "$FAIL" -eq 1 ]; then
        echo "ERROR: Unpermitted libc functions found."
        popd > /dev/null
        exit 1
    fi
else
    echo "ERROR: 'libc' not excluded, all symbols linked."
    popd > /dev/null
    exit 1
fi

echo
echo "SUCCESS: Only whitelisted functions found."
popd > /dev/null
