#!/bin/bash
set -euo pipefail

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
NOTE_ARDUINO_DIR="$SCRIPT_DIR/.."

FQBN=""
SKETCH_DIR=""
PRODUCT_UID=""

usage() {
    echo "Usage: $0 --fqbn <fqbn> --sketch-dir <path> [--product-uid <uid>]"
    exit 1
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --fqbn)
            FQBN="$2"
            shift 2
            ;;
        --sketch-dir)
            SKETCH_DIR="$2"
            shift 2
            ;;
        --product-uid)
            PRODUCT_UID="$2"
            shift 2
            ;;
        *)
            usage
            ;;
    esac
done

if [[ -z "$FQBN" || -z "$SKETCH_DIR" ]]; then
    usage
fi

# If running inside container, update Arduino library
if [ -f /.dockerenv ]; then
    cp -r "$NOTE_ARDUINO_DIR" "$HOME/Arduino/libraries/Blues_Wireless_Notecard"
fi

# Build properties
BUILD_PROPS=(
    --build-property "build.extra_flags=-g3 -O0"
    --build-property "compiler.cpp.extra_flags=-g3 -O0"
    --build-property "compiler.c.extra_flags=-g3 -O0"
    --build-property "compiler.ldflags=-g3"
)

if [[ -n "$PRODUCT_UID" ]]; then
    BUILD_PROPS+=(--build-property "compiler.cpp.extra_flags=-DPRODUCT_UID=\"$PRODUCT_UID\"")
fi

BUILD_OUT_DIR="$SKETCH_DIR/build-out"
mkdir -p $BUILD_OUT_DIR

arduino-cli compile \
    --fqbn "$FQBN" \
    --build-path "$BUILD_OUT_DIR" \
    "${BUILD_PROPS[@]}" \
    --log-level trace \
    --verbose \
    --warnings all \
    "$SKETCH_DIR"
