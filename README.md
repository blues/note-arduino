[![Coverage Status](https://coveralls.io/repos/github/blues/note-arduino/badge.svg?branch=master)](https://coveralls.io/github/blues/note-arduino?branch=master)

# note-arduino

The note-arduino Arduino library for communicating with the
[Blues][blues] Notecard via serial or I²C. Includes example sketches in
the [examples directory](examples).

This library allows you to control a Notecard by writing an Arduino sketch in C
or C++. Your sketch may programmatically configure Notecard and send Notes to
[Notehub.io][notehub].

This library is a wrapper around the [note-c library][note-c], which it includes
as a git subtree.

## Installation

1. Open the Arduino IDE and click **Tools > Manage Libraries...**.
2. Search for "Blues" in the input box and click the "Install" button next
   to the "Blues Notecard" result.

   ![](https://wireless.dev/images/guides/first-sensor/arduino/install-library.gif)

3. Create a new sketch and select the Sketch > Include Library > Contributed
Libraries > Blues Notecard menu option, to add the following include to
your sketch:

   ```
   #include <Notecard.h>
   ```

## Usage

```cpp
#include <Notecard.h>

// Create an instance of the Notecard class.
Notecard notecard;
```

Both configuration methods use the `begin()` method, though the parameters
differ depending on your approach.

### Serial Configuration

For Serial, pass in the Serial object and baud rate.

```cpp
notecard.begin(Serial1, 9600);
```

### I2C Configuration

For I2C, you may simply call Notecard `begin()` with no parameters.

```cpp
notecard.begin();
```

### Sending Notecard Requests

Notecard requests use bundled `J` json package to allocate a `req`, is a JSON
object for the request to which we will then add Request arguments. The
function allocates a `"req` request structure using malloc() and initializes its
"req" field with the type of request.

```cpp
J *req = notecard.newRequest("hub.set");
JAddStringToObject(req, "product", "com.[mycompany].[myproduct]");
JAddStringToObject(req, "mode", "continuous");
notecard.sendRequest(req);
```

### Reading Notecard Responses

If you need to read a response from the Notecard, use the `requestAndResponse()`
method and `JGet*` helper methods to read numbers, strings, etc. from the JSON
response.

```cpp
J *rsp = notecard.requestAndResponse(notecard.newRequest("card.temp"));
if (rsp != NULL) {
   temperature = JGetNumber(rsp, "value");
   notecard.deleteResponse(rsp);
}
```

## Documentation

The documentation for this library can be found
[here](https://dev.blues.io/tools-and-sdks/arduino-library/).

## Examples

The [examples](examples/) directory contains examples for using this library
with:

- [Notecard Basics](examples/Example1_NotecardBasics/Example1_NotecardBasics.ino)
- [Performing Periodic Communications](examples/Example2_PeriodicCommunications/Example2_PeriodicCommunications.ino)
- [Handling inbound requests with polling](examples/Example3_InboundPolling/Example3_InboundPolling.ino)
- [Handling inbound interrupts](examples/Example4_InboundInterrupts/Example4_InboundInterrupts.ino)
- [Using Note templates](examples/Example5_UsingTemplates/Example5_UsingTemplates.ino)
- [Sensor tutorial](examples/Example6_SensorTutorial/Example6_SensorTutorial.ino)
- [Power control](examples/Example7_PowerControl/Example7_PowerControl.ino)
- [Notecard Outboard Firmware Update](examples/Example10_OutboardFirmwareUpdate/Example10_OutboardFirmwareUpdate.ino)

Before running an example, you will need to set the Product Identifier, either
in code or on your connected Notecard. Steps on how to do this can be found at
[https://dev.blues.io/tools-and-sdks/samples/product-uid](https://dev.blues.io/tools-and-sdks/samples/product-uid).

## Contributing

We love issues, fixes, and pull requests from everyone. Please run the
unit-tests ([as described in the following section](#note-arduino-tests)), prior
to submitting your PR. By participating in this project, you agree to abide by
the Blues Inc [code of conduct].

For details on contributions we accept and the process for contributing, see our
[contribution guide](CONTRIBUTING.md).

## Note Arduino Tests

The unit tests use [Catch2](https://github.com/catchorg/Catch2) v3.3.0 (fetched
by CMake) and [FFF](https://github.com/meekrosoft/fff) fakes, built with CMake
and run with CTest, the same framework as [note-c](https://github.com/blues/note-c).
Catch2 v3 requires C++14, so the unit-test build uses `-std=c++14` (the library
itself is unchanged).

- `test/src/<unit>_test.cpp`: one Catch2 executable per unit under test
  (`Notecard`, `NoteI2c_Arduino`, `NoteLog_Arduino`, `NoteSerial_Arduino`,
  `NoteTxn_Arduino`). `NoteI2c_Arduino` is also built with `-DWIRE_HAS_END`;
  those cases carry the ` [WIRE_HAS_END]` suffix in CTest.
- `test/include/arduino_fakes.hpp`, `test/src/arduino_fakes.cpp`: FFF fakes for
  the Arduino core and the note-c API, plus minimal stateless `Stream`,
  `HardwareSerial`, `SoftwareSerial` and `TwoWire` stubs that forward to FFF
  fakes (FFF cannot fake C++ class methods).
- `test/include/note_iface_fakes.hpp`, `test/src/note_iface_fakes.cpp`: FFF
  fakes for the note-arduino interfaces, used by the `Notecard` tests.
- `test/include/fff.h`: byte-identical to note-c's `test/include/fff.h`.

### Dependencies

`cmake` (3.20+), `make`, `g++`, `git`, and optionally `lcov` (coverage) and
`valgrind` (memory checks). Catch2 is downloaded by CMake. The CI environment
is defined in `.devcontainer/ci/Dockerfile`.

### Invocation

From the `note-arduino` folder:

```none
./scripts/run_unit_tests.sh [--coverage] [--mem-check] [--verbose]
```

- `--coverage` writes `coverage/lcov.info` (src/ only).
- `--mem-check` runs every test under valgrind; a leak fails the run.
- The build uses `-j2` by default; set `NOTE_ARDUINO_BUILD_JOBS=<n>` to change it.

Or use CMake directly:

```none
cmake -B build -S test
cmake --build build
ctest --test-dir build --output-on-failure
```

Run only the `-DWIRE_HAS_END` variants with
`ctest --test-dir build -R 'WIRE_HAS_END\]'`.

To use the CI container:

```none
docker build . --file .devcontainer/ci/Dockerfile --tag note-arduino-ci
docker run --rm --volume $(pwd):/note-arduino/ --workdir /note-arduino/ \
    --entrypoint ./scripts/run_unit_tests.sh note-arduino-ci --coverage --mem-check
```

### Success

Upon success, the script ends with:

```none
100% tests passed, 0 tests failed out of 250
...
SUCCESS: Tests passed.
```

### Adding a New Test

Add a `TEST_CASE("test_<name>", "[<Unit>]")` to `test/src/<unit>_test.cpp`.
Call `resetFakes();` first, then follow the **Arrange**, **Action** and
**Assert** sections of the existing tests. CTest discovers new cases
automatically. A new unit needs one `add_test(<unit>)` line in
`test/CMakeLists.txt`.

## Notestation Dev Environment (Setup and Usage)

To gain access to the Notestation array you must authenticate with our Tailscale
VPN. Create a `.env` file based on `.env.example` in the `.devcontainer/notestation/`
folder. Replace the placeholder TS_AUTHKEY with your actual Tailscale authentication
key. Treat the .env file as sensitive and do not commit it to version control or
share it publicly. This will set up the necessary environment variables for VPN access.

### Launching the Dev Container

Run:

```sh
devcontainer up --config .devcontainer/notestation/devcontainer.json --workspace-folder .
```

Verify containers are running:

```sh
docker ps -a --filter "name=note-arduino"
```
Confirm the following containers are up and running:

```
note-arduino_devcontainer
note-arduino_tailnet-connection
```

Exec into the dev container:

```sh
docker exec -it note-arduino_devcontainer bash
```

Inside the container, check Notestation CLI availability and connectivity:

```sh
pipenv run notestation --version
pipenv run notestation client list
```

### Compiling and Flashing Examples

Inside the dev container:

1. Compile for target firmware (i.e. `.ino` file) for the desired host MCU (e.g., CYGNET or SWAN_R5):

   ```sh
   arduino-cli compile ${input:example} --fqbn STMicroelectronics:stm32:Blues:pnum=${input:board},opt=ogstd,dbg=enable_sym --output-dir ./build/${input:board} --log-level trace --verbose --warnings all
   ```

   a. Replace ${input:example} with the path to your `.ino` file
   b. Replace ${input:board} with either `SWAN_R5` or `CYGNET`

2. Reserve a Notestation with the appropriate hardware support (e.g., mcu_cygnet for CYGNET or mcu_swan for SWAN_R5):

   ```sh
   pipenv run notestation client reserve --tags '["mcu_cygnet"]' &
   ```

   > _**NOTE:** This call is blocking, and therefore must be run it its own shell, or as a background task._

3. Flash to Notestation (e.g., barcelona-notestation-2 for CYGNET):

   ```sh
   pipenv run notestation client flash --file ${input:binary}
   ```
   a. Replace ${input:binary} with the path to the newly created `.elf` file

   > _**HINT:** Look in folder `./build/${input:board}`_

4. Free the reserved Notestation by terminating the PID of the associated process.

### Capturing Notestation Output (i.e. device logs)

1. Capture application output using `tio` by monitoring the associated the pseudo-tty devices:

   ```sh
   tio ~/.notestation/pid-<pid>_<hostname>/host_mcu_usb
   ```

   > _**HINT:** The pseudo-tty devices are located in a folder related to the Notestattion reservations (e.g. `~/.notestation/pid-<pid>_<hostname>/<ptty_device>`)._

   > _**HINT:** The devices are named according to their function, for example `host_mcu_usb` is a connection to the host MCU USB._

### Troubleshooting

- Tailscale errors: Verify TS_AUTHKEY, restart containers.
- Client not found: Check `pipenv run notestation client list --all`; use online alternatives.
- Stuck reservations: Kill lingering processes with `pkill -f notestation` or `kill -9 <PID>` for reservation PIDs.
- Queue issues: Check queue with `pipenv run notestation client list`; if queue doesn't decrease, kill and retry.

## Generating a Release

### Update `note-c` dependency

This library depends on the Blues [`note-c` library][note-c]. To update this
repo with the latest from `note-c`, run the `update_note_c.sh` shell script
from the `scripts/` folder:

```none
scripts/update_note_c.sh
```

_**NOTE:** It is important to use the script. It utilizes git subtrees to
include the appropriate files in the `src/note-c` folder. It also eliminates
folders that necessary for testing `note-c` individually, but interfere with
the Arduino build environment._

### Update Version Files

When generating a release of the `note-arduino` library, you will need to update
the version in two places.

- `NoteDefines.h`

    The `note-arduino` library provides preprocessor defines to allow for
    programmatic access to the version number. The following preprocessor
    defines should be updated with the version you wish to publish:

    - `NOTE_ARDUINO_VERSION_MAJOR`
    - `NOTE_ARDUINO_VERSION_MINOR`
    - `NOTE_ARDUINO_VERSION_PATCH`

- `library.properties`

    Update the `version` key to reflect the version you wish to publish. The
    version string should follow the form "\<major\>.\<minor\>.\<patch\>" (e.g.
    `1.6.4`). This value will be used by the Arduino Library Manager.

### Arduino Library Manager Release

Publishing a release on GitHub will trigger the Arduino Library Manager. The
Arduino Library Manager will look to the `library.properties` file, and generate
a release based solely on the `version` value it finds there.

_**NOTE:** The GitHub version is not evaluated by the Arduino Library Manager._

## More Information

For additional Notecard SDKs and Libraries, see:

* [note-c][note-c] for Standard C support
* [note-go][note-go] for Go
* [note-python][note-python] for Python

## To learn more about Blues, the Notecard and Notehub, see:

* [blues.com](https://blues.com)
* [notehub.io][notehub]
* [wireless.dev](https://wireless.dev)

## License

Copyright (c) 2019 Blues Inc. Released under the MIT license. See
[LICENSE](LICENSE) for details.

[blues]: https://blues.com
[notehub]: https://notehub.io
[note-c]: https://github.com/blues/note-c
[note-go]: https://github.com/blues/note-go
[note-python]: https://github.com/blues/note-python
[archive]: https://github.com/blues/note-arduino/archive/master.zip
[code of conduct]: https://blues.github.io/opensource/code-of-conduct
[notehub]: https://notehub.io
