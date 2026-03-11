#!/bin/bash

COVERAGE=0
HEARTBEAT_CALLBACK=0
MEM_CHECK=0
LOW_MEM=0
NO_DEBUG=0
SHOW_MALLOC=0
SINGLE_PRECISION=0
VERBOSE=0

# Parse command line arguments.
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --coverage) COVERAGE=1 ;;
        --heartbeat-callback) HEARTBEAT_CALLBACK=1 ;;
        --low-mem) LOW_MEM=1 ;;
        --mem-check) MEM_CHECK=1 ;;
        --no-debug) NO_DEBUG=1 ;;
        --show-malloc) SHOW_MALLOC=1 ;;
        --single-precision) SINGLE_PRECISION=1 ;;
        --verbose) VERBOSE=1 ;;
        *) echo "Unknown parameter: $1"; exit 1 ;;
    esac
    shift
done

# Determine the directory of the script.
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
ROOT_SRC_DIR="$SCRIPT_DIR/.."

# Check for a CMakeLists.txt file, to ensure CMake operations
# are executed in the correct directory.
if [[ ! -f "$ROOT_SRC_DIR/CMakeLists.txt" ]]; then
    echo "ERROR: Unable to locate ${ROOT_SRC_DIR}/CMakeList.txt. Expecting to use '..' to access root directory. Did the location of run_unit_tests.sh change?"
    exit 1
fi

# Move to the root source directory.
pushd $ROOT_SRC_DIR $@ > /dev/null

CMAKE_OPTIONS="-DNOTE_C_BUILD_TESTS:BOOL=ON"
BUILD_OPTIONS=""
CTEST_OPTIONS=""
if [[ $COVERAGE -eq 1 ]]; then
    CMAKE_OPTIONS="${CMAKE_OPTIONS} -DNOTE_C_COVERAGE:BOOL=ON"
    BUILD_OPTIONS="${BUILD_OPTIONS} coverage"
fi
if [[ $LOW_MEM -eq 1 ]]; then
    CMAKE_OPTIONS="${CMAKE_OPTIONS} -DNOTE_C_LOW_MEM:BOOL=ON"
fi
if [[ $MEM_CHECK -eq 1 ]]; then
    CMAKE_OPTIONS="${CMAKE_OPTIONS} -DNOTE_C_MEM_CHECK:BOOL=ON"
    CTEST_OPTIONS="${CTEST_OPTIONS} -T memcheck"

    # This fixes a problem when running valgrind in a Docker container when the
    # host machine is running Fedora. See https://stackoverflow.com/a/75293014.
    ulimit -n 1024
fi
if [[ $NO_DEBUG -eq 1 ]]; then
    CMAKE_OPTIONS="${CMAKE_OPTIONS} -DNOTE_NODEBUG:BOOL=ON"
fi
if [[ $SHOW_MALLOC -eq 1 ]]; then
    CMAKE_OPTIONS="${CMAKE_OPTIONS} -DNOTE_C_SHOW_MALLOC:BOOL=ON"
fi
if [[ $SINGLE_PRECISION -eq 1 ]]; then
    CMAKE_OPTIONS="${CMAKE_OPTIONS} -DNOTE_C_SINGLE_PRECISION:BOOL=ON"
fi
if [[ $HEARTBEAT_CALLBACK -eq 1 ]]; then
    CMAKE_OPTIONS="${CMAKE_OPTIONS} -DNOTE_C_HEARTBEAT_CALLBACK:BOOL=ON"
fi
if [[ $VERBOSE -eq 1 ]]; then
    CMAKE_OPTIONS="${CMAKE_OPTIONS} -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON --log-level=VERBOSE"
fi

# Configure the build with the flag to build docs.
cmake -B build/ $CMAKE_OPTIONS
if [[ $? -ne 0 ]]; then
    echo "ERROR: CMake failed to configure build."
    popd > /dev/null
    exit 1
fi

# Build `note-c` and the unit-tests.
cmake --build build/ -- $BUILD_OPTIONS -j
if [[ $? -ne 0 ]]; then
    echo "ERROR: CMake failed to build note-c and the unit-tests."
    popd > /dev/null
    exit 1
fi

# Run the unit tests.
ctest --test-dir build/ --output-on-failure ${CTEST_OPTIONS}
if [[ $? -ne 0 ]]; then
    echo "ERROR: Unit-tests did not pass."
    popd > /dev/null
    exit 1
fi

echo "SUCCESS: Tests passed."
popd > /dev/null
