# Container with dependencies necessary to run unit-tests and code coverage

# Build development environment
# $ docker build . --tag note-arduino-coverage

# Launch development environment (mount source root as /note-arduino/)
# $ docker run --rm --volume $(pwd)/../../../:/note-arduino/ --workdir /note-arduino/ note-arduino-coverage


# Base Image - here we reuse the test image since it has all the dependencies required 
FROM note-arduino-test

ENTRYPOINT ["ash", "-c", "./.github/actions/publish-coverage/publish-to-coveralls.sh"]
