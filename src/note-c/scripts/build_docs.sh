#!/bin/bash

# Determine the directory of the script.
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
ROOT_SRC_DIR="$SCRIPT_DIR/.."

# Check for a CMakeLists.txt file, to ensure CMake operations
# are executed in the correct directory.
if [[ ! -f "$ROOT_SRC_DIR/CMakeLists.txt" ]]; then
    echo "ERROR: Unable to locate ${ROOT_SRC_DIR}/CMakeList.txt. Expecting to use '..' to access root directory. Did the location of build_docs.sh change?"
    exit 1
fi

# Move to the root source directory.
pushd $ROOT_SRC_DIR $@ > /dev/null

# Configure the build with the flag to build docs.
cmake -B build/ -DNOTE_C_BUILD_DOCS:BOOL=ON
if [[ $? -ne 0 ]]; then
    echo "ERROR: CMake failed to configure build."
    popd > /dev/null
    exit 1
fi

# Build the `docs` target.
cmake --build build/ -- docs -j
if [[ $? -ne 0 ]]; then
    echo "ERROR: CMake failed to build docs."
    popd > /dev/null
    exit 1
fi

echo "SUCCESS: Built docs."
popd > /dev/null
