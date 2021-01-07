# note-c

The note-arduino Arduino library for communicating with the
[Blues Wireless][blues] Notecard via serial or I²C.

This library allows you to control a Notecard by writing a C
or C++ program. Your program may programmatically configure Notecard and send
Notes to [Notehub.io][notehub].

This library is used by the [note-arduino library][note-arduino], which includes
it as a git submodule.

## Documentation

The documentation for this library can be found
[here](https://blues.github.io/note-c/html/index.html).

## Contributing

We love issues, fixes, and pull requests from everyone. Please run the
unit-tests ([as described in the folllowing section](#note-arduino-tests),
prior to submitting your PR. By participating in this project, you agree to
abide by the Blues Inc [code of conduct].

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
`{test_name, "test_name"}`. _Don't forget to add a comma to the end of the array,
before adding your new entry._

## More Information

For additional Notecard SDKs and Libraries, see:

* [note-arduino](note-arduino) for Arduino support
* [note-python](note-python) for Python
* [note-go](note-go) for Go

## To learn more about Blues Wireless, the Notecard and Notehub, see:

* [blues.com](https://blues.io)
* [notehub.io][notehub]
* [wireless.dev](https://wireless.dev)

## License

Copyright (c) 2019 Blues Inc. Released under the MIT license. See
[LICENSE](LICENSE) for details.

[blues]: https://blues.com
[notehub]: https://notehub.io
[note-arduino]: https://github.com/blues/note-arduino
[note-go]: https://github.com/blues/note-go
[note-python]: https://github.com/blues/note-python
[code of conduct]: https://blues.github.io/opensource/code-of-conduct