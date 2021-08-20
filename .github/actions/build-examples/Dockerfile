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

RUN apt-get update && apt-get install -y curl
# libc6-compat advice from here https://githubmemory.com/repo/arduino/arduino-cli/issues/1378
# but that doesn't work and gives the error described at https://www.gitmemory.com/issue/arduino/Arduino/5691/757103707
# using gcompat makes the cli and the compiler work

ENV ARDUINO=/Arduino
ENV ARDUINO_CLI=/Arduino/cli
ENV ARDUINO_DIRECTORIES_USER=$ARDUINO
ENV PATH="${ARDUINO_CLI}/bin:${PATH}"
ENV BOARDS=$BOARDS
ENV PLATFORMS=$PLATFORMS

RUN curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh && \
    mkdir -p $ARDUINO_CLI/bin && \
    mkdir -p $ARDUINO/libraries && \
    arduino-cli config init
    
ENTRYPOINT ["bash", "-c", "./.github/actions/build-examples/entrypoint.sh"]
