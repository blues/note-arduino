[![Coverage Status][coverage badge]][coverage details]

# note-c

The note-c C library for communicating with the
[Blues Wireless][blues] Notecard via serial or I²C.

This library allows you to control a Notecard by writing a C
or C++ program. Your program may programmatically configure Notecard and send
Notes to [Notehub.io][notehub].

This library is used by the [note-arduino library][note-arduino], which includes
it as a git subtree.

## API Documentation

The API documentation for this library can be found [here][note-c API docs].

## Logging Control

`note-c` provides a comprehensive and flexible logging functionality.

To activate logging, you must provide a callback for logging via
`hookDebugOutput()`. The callback takes the following form:

```c
typedef size_t (*debugOutputFn) (const char *text);
```

The callback is responsible for taking a character array (C-style string) and
returning the number of bytes processed (written out) as confirmation. The
exact implementation will be up to the user who provided the function pointer,
but its presence will active logging in the library.

### Library Logging

#### Log Levels

`note-c` provides for four (4) levels of logging. Here they are listed from
most severe to most verbose:

0. `NOTE_C_LOG_LEVEL_ERROR`
1. `NOTE_C_LOG_LEVEL_WARN`
2. `NOTE_C_LOG_LEVEL_INFO`
3. `NOTE_C_LOG_LEVEL_DEBUG`

By default, `note-c` logs at `NOTE_C_LOG_LEVEL_INFO`.

#### Default Logging Behavior

To modify the default behavior, you may specify the desired log level at compile
time, as follows:

```sh
-DNOTE_C_LOG_LEVEL=0
```

_**NOTE:** In the example above, you will notice we used zero (`0`), instead of
`NOTE_C_LOG_LEVEL_ERROR`. This is because the warning constants are internal to
the library, and not available in the context of the command line._

Here, we have decided to show only the most severe (i.e. `[ERROR]`) logs.
Alternatively, you may set the level to any of the values listed above.

#### Dynamic Logging Behavior

In the previous section, we discussed setting the base (or default) logging
behavior for the library. However, you may also set the log level dynamically,
during runtime, by using the `NoteSetLogLevel()` API.

```c
NoteSetLogLevel(NOTE_C_LOG_LEVEL_WARN);
```

### Notecard Sync Logging (`[SYNC]`)

Tangential to the standard logging behavior, `note-c` also provides a helper
function to invoke/extract synchronization logs from the Notecard.

- `NoteDebugSyncStatus()`

Instead of toggling features inside the library, this helper functions sends a
request to the Notecard to inquire about its synchronization status and logs
those details.

The function is designed to be called in a loop and throttled by a parameter.
See [the documentation page][NoteDebugSyncStatus] for more information.

## Versioning

The `note-c` versioning scheme is a variant of [Semantic
Versioning](https://semver.org/).

Below is a high-level overview of the major/minor/patch versions:

- Major Version: Signals incompatible API changes.
- Minor Version: Signals added functionality in a backward compatible manner.
- Patch Version: Signals backward compatible bug fixes.

Beyond the SemVer foundation, Blues has imposed additional requirements for a
version to be considered valid:

- Major/minor/patch versions SHALL NOT be zero.
- For anything other than major version, version numbers MUST NOT contain
EITHER leading zeroes OR trailing zeroes (e.g. version `1.10.2` is invalid).

> Example version progression:
>
> `1.8.1`, `1.9.1`, `1.9.2`, `1.11.1`, `1.11.2`, `1.11.3`, `1.12.1`, `2.1.1`

These additional constraints have been observed to help disambiguate versions
and reduce support burden.

### Version Artifacts

The version can be referenced/tested programmatically via the following
preprocessor defined integers found in `note.h`:

- `NOTE_C_VERSION_MAJOR`
- `NOTE_C_VERSION_MINOR`
- `NOTE_C_VERSION_PATCH`

The version may also be logged via the preprocessor defined string literal,
`NOTE_C_VERSION`.

## Contributing

We love issues, fixes, and pull requests from everyone. By participating in this
project, you agree to abide by the Blues Inc [code of conduct].

For details on contributions we accept and the process for contributing, see our
[contribution guide](CONTRIBUTING.md).

## More Information

For additional Notecard SDKs and Libraries, see:

- [note-arduino][note-arduino] for Arduino support
- [note-python][note-python] for Python
- [note-go][note-go] for Go

## To learn more about Blues Wireless, the Notecard and Notehub, see:

- [blues.com](https://blues.io)
- [notehub.io][notehub]
- [wireless.dev](https://wireless.dev)

## License

Copyright (c) 2019 Blues Inc. Released under the MIT license. See
[LICENSE](LICENSE) for details.

[blues]: https://blues.com
[code of conduct]: https://blues.github.io/opensource/code-of-conduct
[coverage badge]: https://coveralls.io/repos/github/blues/note-c/badge.svg?branch=master
[coverage details]: https://coveralls.io/github/blues/note-c?branch=master
[NoteDebugSyncStatus]: https://blues.github.io/note-c/api_reference.html#c.NoteDebugSyncStatus
[notehub]: https://notehub.io
[note-arduino]: https://github.com/blues/note-arduino
[note-c API docs]: https://blues.github.io/note-c/index.html
[note-go]: https://github.com/blues/note-go
[note-python]: https://github.com/blues/note-python
