# Testing

This directory contains note-c unit tests written in C++ using Catch2. The
[Fake Function Framework](https://github.com/meekrosoft/fff) (fff) is used for
mocking and is bundled with the test code (see fff.h).

## Dependencies

- [CMake](https://cmake.org/install), version 3.13+.
- [Catch2](https://github.com/catchorg/Catch2), v3.
  - If Catch2 isn't installed, we'll attempt to download and build Catch2 from
    source.
  - We require C++14, which is the minimum expected by Catch2 v3.

## Running the Tests

```sh
mkdir build
cd build
cmake ..
make -j
ctest
```

Alternatively, scripts/run_unit_tests.sh will do all these steps for you. This
script is also used by our CI pipeline (see the Dockerfile in
.github/actions/run_unit_tests).

### Notes

- We forbid in-source builds, so creating a build directory (e.g. `mkdir build`)
and building in there is mandatory.

## CMake Options

- BUILD_CATCH: Download and build Catch2 from source, rather than looking for an
installed verison. Default: OFF.
- COVERAGE: Adds the target `coverage` to the build. Requires lcov. To generate
a coverage report, run `make coverage`.

## Development

For the most part, each function in the API gets its own test executable in
test/src. The naming convention is FunctionName_test.cpp. Some functions make
sense to test together, like the NotePayload* family, so they're exercised in
one executable (NotePayload_test.cpp, in this case). Any functions that are
mocked need to be added to the list of mocked functions in the root
CMakeLists.txt (see MOCKED_FNS). Once written, the test needs to be added to
test/CMakeLists.txt with a call to the `add_test` macro.

Assuming you ran cmake with coverage enabled (`-DCOVERAGE=1`), ran `make
coverage`, and are in the build directory, you can view the coverage report as
HTML with:

```sh
cd test/coverage
genhtml lcov.info -o tmp
```

This will generate HTML under test/coverage/tmp. Open index.html in your
browser to view the report.
