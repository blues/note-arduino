#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
NOTE_ARDUINO_DIR="$SCRIPT_DIR/.."

# If this is being run inside a Docker container (i.e. for GitHub actions CI),
# copy the latest note-arduino code into the appropriate place so that it can
# be consumed when building the example.
if [ -f /.dockerenv ]; then
    cp -r $NOTE_ARDUINO_DIR $HOME/Arduino/libraries/Blues_Wireless_Notecard
fi

arduino-cli compile \
    --build-property compiler.cpp.extra_flags='-Wno-unused-parameter -Werror' \
    --fqbn $1 \
    --log-level trace \
    --verbose \
    --warnings all \
    $2
