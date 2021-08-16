FROM alpine

RUN apk add --update --no-cache curl gcompat
# libc6-compat advice from here https://githubmemory.com/repo/arduino/arduino-cli/issues/1378
# but that doesn't work and gives the error described at https://www.gitmemory.com/issue/arduino/Arduino/5691/757103707
# using gcompat makes the cli and the compiler work

ENV ARDUINO=/Arduino
ENV ARDUINO_CLI=/Arduino/cli
ENV ARDUINO_DIRECTORIES_USER=$ARDUINO
ENV WORKSPACE=${GITHUB_WORKSPACE}
ENV PATH="${ARDUINO_CLI}/bin:${PATH}"

RUN curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh && \
    mkdir -p $ARDUINO_CLI/bin && \
    mkdir -p $ARDUINO/libraries && \
    arduino-cli config init && \
    arduino-cli core update-index && \
    arduino-cli core install arduino:avr && \
    arduino-cli core install arduino:avr && \
    arduino-cli core install arduino:samd

ENTRYPOINT ["ash", "-c", "./.github/actions/build-avr/build.sh"]
