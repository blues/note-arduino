# note-arduino

The note-arduino Arduino library for communicating with the
[Blues Wireless][blues] Notecard via serial or I²C. Includes example sketches in
the [examples directory](examples).

This library allows you to control a Notecard by writing an Arduino sketch in C
or C++. Your sketch may programmatically configure Notecard and send Notes to
[Notehub.io][notehub].

This library is a wrapper around the [note-c library][note-c], which it includes
as a git submodule.

## Installation

1. Open the Arduino IDE and click **Tools > Manage Libraries...**.
2. Search for "Blues" in the input box and click the "Install" button next
   to the "Blues Wireless Notecard" result.

   ![](https://wireless.dev/images/guides/first-sensor/arduino/install-library.gif)

3. Create a new sketch and select the Sketch > Include Library > Contributed
Libraries > Blues Wireless Notecard menu option, to add the following include to
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

For I2C, initialize `Wire` by calling `begin()` and call Notecard `begin()` with
no parameters.

```cpp
Wire.begin();

notecard.begin();
```

### Sending Notecard Requests

Notecard requests use bundled `J` json package to allocate a `req`, is a JSON
object for the request to which we will then add Request arguments. The
function allocates a `"req` request structure using malloc() and initializes its
"req" field with the type of request.

```cpp
J *req = notecard.newRequest("service.set");
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

## Keeping up to date with note-c repo

This library depends on the blues [note-c repo][note-c] and utilizes
git subtrees to include those files in the src/note-c folder. To
update this repo with the latest from note-c:

```sh
rm -rf src/note-c
git commit -am 'remove note-c before readd'
git subtree add --prefix=src/note-c --squash https://github.com/blues/note-c.git master
```

## Documentation

The documentation for this library can be found [here](https://blues.github.io/note-arduino/html/index.html).

# Examples

The [examples](examples/) directory contains examples for using this library
with:

- [Notecard Basics](examples/Example1_NotecardBasics/Example1_NotecardBasics.ino)
- [Performing Periodic Communications](examples/Example2_PeriodicCommunications/Example2_PeriodicCommunications.ino)
- [Handling inbound requests with polling](examples/Example3_InboundPolling/Example3_InboundPolling.ino)
- [Handling inbound interrupts](examples/Example4_InboundInterrupts/Example4_InboundInterrupts.ino)
- [Using Note templates](examples/Example5_UsingTemplates/Example5_UsingTemplates.ino)

## Contributing

We love issues, fixes, and pull requests from everyone. Please run the
unit-tests ([as described in the following section](#note-arduino-tests)), prior
to submitting your PR. By participating in this project, you agree to abide by
the Blues Inc [code of conduct].

For details on contributions we accept and the process for contributing, see our
[contribution guide](CONTRIBUTING.md).

## Note Arduino Tests

From the `test` directory, execute the `run_tests.sh` script.

### Success

Upon success, you will see the following message:

```none
...
[passed] test_notecard_debugSyncStatus_does_not_modify_note_c_result_value_before_returning_to_caller
[passed] test_notecard_responseError_does_not_modify_j_object_parameter_value_before_passing_to_note_c
[passed] test_notecard_responseError_does_not_modify_note_c_result_value_before_returning_to_caller

All tests passed!
```

### Failure

When a test fails, you will see a message similar to the following:

```none
...
[passed] test_notecard_sendRequest_does_not_modify_j_object_parameter_value_before_passing_to_note_c
[passed] test_notecard_sendRequest_does_not_modify_note_c_result_value_before_returning_to_caller
[FAILED] test_notecard_requestAndResponse_does_not_modify_j_object_parameter_value_before_passing_to_note_c

Test <27> failed!
```

Here, the failed test is the last reported test,
`test_notecard_requestAndResponse_does_not_modify_j_object_parameter_value_before_passing_to_note_c`,
and the related test number is `27`. The return value of each test is unique,
therefore this number can be used to find the test in `notecard-test.cpp` by
searching for "`result = 27`".

### Adding a New Test

The signature of a test function is `int(*test_fn)(void)`, so to say, a test
function takes NO parameters and returns an integer.

To add a new test to `notecard-test.cpp`, add your new test immediately below
the last test and increment the error value, `result`, by 1. At the time of
writing, the last test returns an error value of 36, therefore the error
condition of a new test should set the `result` value to 37.

To add this test to the runner, copy the test's name and use it to create an
entry in the `tests` array in the `main` function. The entry will occupy it's
own line at the end of the array, and syntax should be as follows,
`{test_name, "test_name"}`. _Don't forget to add a comma to the end of the
array, before adding your new entry._

## More Information

For additional Notecard SDKs and Libraries, see:

* [note-c][note-c] for Standard C support
* [note-go][note-go] for Go
* [note-python][note-python] for Python

## To learn more about Blues Wireless, the Notecard and Notehub, see:

* [blues.com](https://blues.io)
* [notehub.io][Notehub]
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
[Notehub]: https://notehub.io
