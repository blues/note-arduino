# Container with dependencies necessary to run unit-tests and code coverage

# Build development environment
# $ docker build . --tag note-arduino-test

# Launch development environment (mount source root as /note-arduino/)
# $ docker run --rm --volume $(pwd)/../../../:/note-arduino/ --workdir /note-arduino/ note-arduino-test

# Base Image
FROM alpine


# Testing Dependencies
# if wanting to install cpp-coveralls via pip install --user cpp-coveralls, these deps are required:
# python3-dev py3-pip libffi-dev rust cargo openssl-dev git 

# install gcovr from the package manager, which gets most required dependencies (avoiding the need for libxml2-dev, libxslt-dev and setting the include path.)
# then install using pip to get the 5.x line with support for coveralls formatted reports.
RUN ["ash", "-c", "\
    apk add --no-cache \
      gcovr \
      py3-pip python3-dev \
      g++ \
      valgrind \
      && pip install -U gcovr \
"]

ENTRYPOINT ["./test/run_all_tests.sh"]
