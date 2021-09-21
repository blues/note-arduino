# This docker image is best run from "act".  For more detailed development work, it may be necessary to launch
# the container directly.
#
# building the container:
#       docker build . -t note-arduino-examples
#
# to run the container
#       docker run -it --rm --volume $(pwd)/../../../:/note-arduino/ --workdir /note-arduino/ note-arduino-examples 
# 
# to drop into a shell
#       docker run -it --rm --entrypoint /bin/bash --volume $(pwd)/../../../:/note-arduino/ --workdir /note-arduino/ note-arduino-examples
# 
# Environment variables are used to install platforms and build the examples for the given boards
#       PLATFORMS="arduino:avr arduino:mbed_rp2040"
#       BOARDS="arduino:avr:uno arduino:mbed_rp2040:pico"

FROM debian:stable-slim

RUN echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections && \
    apt-get update -y && apt-get install -y curl python3 python3-serial && \
        ln -s /usr/bin/python3 /usr/bin/python

ENV ARDUINO=/Arduino
ENV ARDUINO_CLI=/Arduino/cli
ENV ARDUINO_DIRECTORIES_USER=$ARDUINO
ENV PATH="${ARDUINO_CLI}/bin:${PATH}"
 
# esp32 requires python3 to be the system python. 
# pre-install some platforms
RUN BINDIR=$ARDUINO_CLI/bin && mkdir -p $ARDUINO_CLI/bin && curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh && \
    mkdir -p $ARDUINO/libraries && \
    arduino-cli config init && \
    arduino-cli config add board_manager.additional_urls "https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json" && \
    arduino-cli config add board_manager.additional_urls "https://www.adafruit.com/package_adafruit_index.json" && \
    arduino-cli config add board_manager.additional_urls "https://github.com/stm32duino/BoardManagerFiles/raw/master/package_stmicroelectronics_index.json" && \
    arduino-cli update && \
    arduino-cli core update-index && \
    arduino-cli core install esp32:esp32 && \
    arduino-cli core install adafruit:nrf52 && \
    arduino-cli core install arduino:avr && \
    arduino-cli core install arduino:mbed_rp2040 && \
    arduino-cli core install STMicroelectronics:stm32 && \
    arduino-cli lib install "Adafruit BME680 Library"


ENTRYPOINT ["bash", "-c", "./.github/actions/build-examples/entrypoint.sh"]
