# Container with dependencies necessary to run unit-tests and code coverage

# Build development environment
# $ docker build . --tag note-arduino-test

# Launch development environment (mount source root as /nes-rob/)
# $ docker run --rm --volume $(pwd)/../../../:/note-arduino/ --workdir /note-arduino/ note-arduino-test

# Base Image
FROM alpine

# Testing Dependencies
RUN ["ash", "-c", "\
    apk add --no-cache \
      gcovr \
      g++ \
      valgrind \
"]

ENTRYPOINT ["./test/run_all_tests.sh"]
