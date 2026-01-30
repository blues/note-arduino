#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
NOTE_ARDUINO_DIR="$SCRIPT_DIR/.."

# Parameters:
# $1: FQBN (required)
# $2: SKETCH (required)
# $3: EXTRA_FLAGS (optional) - additional compiler flags

FQBN=$1
SKETCH=$2
EXTRA_FLAGS=$3

# If this is being run inside a Docker container (i.e. for GitHub actions CI),
# copy the latest note-arduino code into the appropriate place so that it can
# be consumed when building the example.
if [ -f /.dockerenv ]; then
    cp -r $NOTE_ARDUINO_DIR $HOME/Arduino/libraries/Blues_Wireless_Notecard
fi

# Extract platform from FQBN (vendor:architecture from vendor:architecture:board:options)
PLATFORM=$(echo "$FQBN" | cut -d: -f1-2)

# Install the platform (board URLs are pre-configured in Dockerfile)
echo "Installing board platform: $PLATFORM"
arduino-cli core install "$PLATFORM"

# Build compiler flags
COMPILER_FLAGS='-Wno-unused-parameter -Werror'
if [ -n "$EXTRA_FLAGS" ]; then
    COMPILER_FLAGS="$COMPILER_FLAGS $EXTRA_FLAGS"
fi

arduino-cli compile \
    --build-property "compiler.cpp.extra_flags=$COMPILER_FLAGS" \
    --fqbn "$FQBN" \
    --log-level trace \
    --verbose \
    --warnings all \
    "$SKETCH"
