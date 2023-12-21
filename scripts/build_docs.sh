#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
ROOT_SRC_DIR="$SCRIPT_DIR/.."

pushd $ROOT_SRC_DIR $@ > /dev/null

cmake -B build/ -DNOTE_C_BUILD_DOCS=1
if [[ $? -ne 0 ]]; then
    echo "Failed to run CMake."
    popd $@ > /dev/null
    exit 1
fi

cmake --build build/ -- docs -j
if [[ $? -ne 0 ]]; then
    echo "Failed to build docs."
    popd $@ > /dev/null
    exit 1
fi

echo "Built docs."
popd $@ > /dev/null
