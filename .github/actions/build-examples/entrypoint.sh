# ensure commands that return non-zero cause the script to fail
set -e

boards_default=arduino:avr:uno
platforms_default=arduino:avr

boards=${BOARDS:-$boards_default}
cores=${PLATFORMS:-$platforms_default}

echo "Building library examples for $cores ($boards)"

# not sure why this is needed since it's already included in the dockerfile. Possibly changes to the current user
# arduino:nrf52:primo and esp32 fail with a g++ error arm-none-eabi-g++: obstack_free: symbol not found
# https://github.com/esphome/esphome/issues/19#issuecomment-392430924
# May be simplest to move to ubuntu as the base docker image

# not all exampels compile on all platforms. This variable contains the combinations that should be excluded
# example2 requires a button definition
exclude="  STMicroelectronics:stm32:Disco-./examples/Example2_PeriodicCommunications/Example2_PeriodicCommunications.ino
STMicroelectronics:stm32:GenF4-./examples/Example0_LibrarylessCommunication/Example0_LibrarylessCommunication.ino esp32:esp32:esp32-./examples/Example6_SensorTutorial/Example6_SensorTutorial.ino arduino:mbed_rp2040:pico-./examples/Example2_PeriodicCommunications/Example2_PeriodicCommunications.ino "

export WORKSPACE=${GITHUB_WORKSPACE:-/note-arduino}
[ ! -d $ARDUINO/libraries ] && mkdir -p $ARDUINO/libraries
[ ! -d $ARDUINO/libraries/note-arduino ] && ln -s $WORKSPACE $ARDUINO/libraries/
echo "Add arduino library path $WORKSPACE"

# installation done by the dockerfile
# arduino-cli core update-index

# install the cores
# for core in $cores; do
#     echo "Installing core $core"
#     arduino-cli core install $core
# done

# install the libraries
# arduino-cli lib install "Adafruit BME680 Library"

# attempt to work around the crash on github.
arduino-cli update
arduino-cli core update-index

extraFlags="-DNOTECARD_SERIAL=Serial $EXTRA_FLAGS"

# build the examples for each board, unless that combination has been excluded
for board in $boards ; do 
    echo "Building library examples for board $board"
    for ino in $(find . -name *.ino) ; do
        combo="$board-$ino"
        if [ -z "${exclude##*$combo*}" ]; then
            echo "Build for $combo is excluded. Skipping."
        else
            echo "Compiling example $combo"
            echo arduino-cli compile --fqbn $board --build-property "compiler.cpp.extra_flags=$extraFlags" $ino
            # ideally we want to use the build-property build.extra_flags, but the ESP32 defines this in the boards file,
            # and those definitions get lost when we override it.
            # https://github.com/arduino/arduino-cli/issues/846
            arduino-cli compile --fqbn $board --build-property "compiler.cpp.extra_flags=$extraFlags" $ino
        fi
    done
done
