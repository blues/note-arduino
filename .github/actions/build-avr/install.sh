# Exit immediately if a command exits with a non-zero status.
set -e

export ARDUINO=${ARDUINO:-${WORKSPACE}/Arduino}
export ARDUINO_CLI=${ARDUINO_CLI:-$ARDUINO/cli}
echo $ARDUINO. $ARDUINO_CLI

# remove existing ones
[ -d "$ARDUINO_CLI/bin" ] && rm -rf $ARDUINO_CLI/bin
[ -d "$ARDUINO/libraries" ] && rm -rf $ARDUINO/libraries
mkdir -p $ARDUINO_CLI/bin
mkdir -p $ARDUINO/libraries

cd $ARDUINO_CLI
# Install Arduino IDE
export PATH=$PATH:$ARDUINO_CLI/bin
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
arduino-cli config init &&
arduino-cli core update-index &&
arduino-cli core install arduino:avr

