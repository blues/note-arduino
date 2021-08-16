set -e

# WORKSPACE is the git repo
export WORKSPACE=${WORKSPACE:-$(pwd)}
[ ! -d $ARDUINO/libraries/note-arduino ] && ln -s $WORKSPACE $ARDUINO/libraries/note-arduino
echo "Add arduino library path $WORKSPACE"
boards="arduino:avr:uno"

# not sure why this is needed since it's already included in the dockerfile. Possibly changes to the current user
arduino-cli core install arduino:avr
arduino-cli lib install "Adafruit BME680 Library"

for board in $boards ; do 
echo "Building examples for board $board"
for ino in $(find . -name *.ino) ; do
    echo "Compiling example $ino for board $board"
    arduino-cli compile -b $board --build-property "build.extra_flags=-DNOTECARD_SERIAL=Serial" $ino
done
done