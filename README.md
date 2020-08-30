# note-arduino

The note-arduino Arduino library for communicating with [Blues Wireless][blues]
via serial or I²C. Includes example sketches in the
[examples directory](examples).

This library allows you to control a Notecard by writing an Arduino sketch in C
or C++. Your sketch may programmatically configure Notecard and send Notes to
[Notehub.io][notehub].

This library is a wrapper around the [note-c library][note-c], which it includes
as a git submodule.

## Installation

1. Download the library as a .zip file from [here][archive] or use the UI
   `Clone or download -> Download ZIP`
2. Start the Arduino IDE
3. Open a new or existing sketch which will talk to Notecard
4. Add the library to the Arduino IDE:
   ```
   Sketch -> Include Library -> Add .ZIP Library...
   ```
   and select the previously downloaded .zip file
5. Include the library in your sketch:
   ```
   Sketch -> Include Library -> Contributed libraries -> Blues Wireless Notecard
   ```
   This will have the effect of inserting this line of code at the top of your
   sketch:
   ```
   #include <Notecard.h>
   ```

## Usage

```cpp
#include <Notecard.h>
```

### Serial Configuration

```cpp
NoteInitSerial(Serial1, 9600);
```

### I2C Configuration

```cpp
NoteI2C();
```

### Sending Notecard Requests

Notecard requests use bundled `J` json package to allocate a `req`, is a JSON
object for the request to which we will then add Request arguments. The
function allocates a `"req` request structure using malloc() and initializes its
"req" field with the type of request.

```cpp
J *req = NoteNewRequest("service.set");
JAddStringToObject(req, "product", "com.mycompany.myproduct");
JAddStringToObject(req, "mode", "continuous");
NoteRequest(req);
```

### Reading Notecard Responses

If you need to read a response from the Notecard, use the `NoteRequestResponse`
api and `JGet*` helper methods to read numbers, strings, etc. from the JSON
response.

```cpp
J *rsp = NoteRequestResponse(NoteNewRequest("card.temp"));
if (rsp != NULL) {
   temperature = JGetNumber(rsp, "value");
   NoteDeleteResponse(rsp);
}
```

## Keeping up to date with note-c repo

This library depends on the blues [note-c repo][note-c] and utilizes
git subtrees to include those files in the src/note-c folder. To
update this repo with the latest from note-c:

```sh
rm -rf src/note-c
git commit -am'remove note-c before readd'
git subtree add --prefix=src/note-c --squash https://github.com/blues/note-c.git master
```

## Documentation

The documentation for this library can be found at the Blues Wireless
[wireless.dev](https//wireless.dev/reference/note-arduino) site.

# Examples

The [examples](examples/) directory contains examples for using this library
with:

- [Notecard Basics](examples/Example1_NotecardBasics/Example1_NotecardBasics.ino)
- [Performing Periodic Communications](examples/Example2_PeriodicCommunications/Example2_PeriodicCommunications.ino)
- [Handling inbound requests with polling](examples/Example3_InboundPolling/Example3_InboundPolling.ino)
- [Handling inbound interrupts](examples/Example4_InboundInterrupts/Example4_InboundInterrupts.ino)
- [Using Note templates](examples/Example5_UsingTemplates/Example5_UsingTemplates.ino)

## Contributing

We love issues, fixes, and pull requests from everyone. By participating in this
project, you agree to abide by the Blues Inc [code of conduct].

For details on contributions we accept and the process for contributing, see our
[contribution guide](CONTRIBUTING.md).

## More Information

For additional Notecard SDKs and Libraries, see:

* [note-c](note-c) for Standard C support
* [note-go](note-go) for Go
* [note-python](note-python) for Python

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


