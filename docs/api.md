# Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`class `[`Notecard`](#class_notecard) | Class that stores state and functions for interacting with the Blues Wireless [Notecard](#class_notecard).

# class `Notecard` 

Class that stores state and functions for interacting with the Blues Wireless [Notecard](#class_notecard).

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public void `[`begin`](#class_notecard_1a247db54fb1607e567a2de17f6407cada)`(uint32_t i2cAddress,uint32_t i2cMax,TwoWire & wirePort)` | Initialize the [Notecard](#class_notecard) for I2C. This function configures the [Notecard](#class_notecard) to use the I2C bus for communication with the host.
`public void `[`begin`](#class_notecard_1a2d660a30d0241db00917d6dac865f7d5)`(HardwareSerial & serial,int speed)` | Initialize the [Notecard](#class_notecard) for Serial communication. This function configures the [Notecard](#class_notecard) to use Serial for communication with the host.
`public void `[`setDebugOutputStream`](#class_notecard_1a4f97e66bf4763cb24adfce9bdd2a73cb)`(Stream & dbgserial)` | Set the debug output source. This function takes a Stream object (for example, `Serial`) and configures it as a source for writing debug messages during development.
`public void `[`clearDebugOutputStream`](#class_notecard_1a21b2937a3c8adae0372c1571f737edaa)`(void)` | Clear the debug output source.
`public void `[`i2cTest`](#class_notecard_1a16127c3032c423753c8b65c4c214ed7a)`(int Adjustment)` | Adjust the I2C read length. Method enabling a developer to test the state of a known issue with the I2C HAL on some ST Microelectronics boards.
`public J * `[`newRequest`](#class_notecard_1a424d5ee778ec3e89bf4f408a6ac274d2)`(const char * request)` | Creates a new request object for population by the host. This function accepts a request string (for example, `"note.add"`) and initializes a JSON Object to return to the host.
`public bool `[`sendRequest`](#class_notecard_1a8016131b70f6af1e42c3054b577676d2)`(J * req)` | Sends a request to the [Notecard](#class_notecard). This function takes a populated `J` JSON request object and sends it to the [Notecard](#class_notecard).
`public J * `[`requestAndResponse`](#class_notecard_1a67bbafe4ef82a6772404283e4f78bf12)`(J * req)` | Sends a request to the [Notecard](#class_notecard) and returns the JSON Response. This function takes a populated `J` JSON request object and sends it to the [Notecard](#class_notecard).
`public void `[`deleteResponse`](#class_notecard_1a26db6cf767d034900033fd8794dbc8a2)`(J * rsp)` | Deletes a `J` JSON response object from memory.
`public void `[`logDebug`](#class_notecard_1a083c4738102bbb2b377c4cb4f3fa11ff)`(const char * message)` | Write a message to the serial debug stream.
`public void `[`logDebugf`](#class_notecard_1a4221529c1852f535c9fd09df01c36107)`(const char * format,...)` | Write a formatted message to the serial debug stream.
`public bool `[`debugSyncStatus`](#class_notecard_1a89b07d345de9ae988ff41ac3f5d1aea5)`(int pollFrequencyMs,int maxLevel)` | Periodically show [Notecard](#class_notecard) sync status, returning TRUE if something was displayed to the debug stream.
`public bool `[`responseError`](#class_notecard_1a1b9d4774c186e11c0bea8050d266fd66)`(J * rsp)` | Determines if there is an error string present in a response object.

## Members

#### `public void `[`begin`](#class_notecard_1a247db54fb1607e567a2de17f6407cada)`(uint32_t i2cAddress,uint32_t i2cMax,TwoWire & wirePort)` 

Initialize the [Notecard](#class_notecard) for I2C. This function configures the [Notecard](#class_notecard) to use the I2C bus for communication with the host.

#### Parameters
* `i2caddress` The I2C Address to use for the [Notecard](#class_notecard). 

* `i2cmax` The max length of each message to send from the host to the [Notecard](#class_notecard). Used to ensure the messages are sized appropriately for the host. 

* `wirePort` The TwoWire implementation to use for I2C communication.

#### `public void `[`begin`](#class_notecard_1a2d660a30d0241db00917d6dac865f7d5)`(HardwareSerial & serial,int speed)` 

Initialize the [Notecard](#class_notecard) for Serial communication. This function configures the [Notecard](#class_notecard) to use Serial for communication with the host.

#### Parameters
* `selectedSerialPort` The HardwareSerial bus to use. 

* `selectedSpeed` The baud rate to use for communicating with the [Notecard](#class_notecard) from the host.

#### `public void `[`setDebugOutputStream`](#class_notecard_1a4f97e66bf4763cb24adfce9bdd2a73cb)`(Stream & dbgserial)` 

Set the debug output source. This function takes a Stream object (for example, `Serial`) and configures it as a source for writing debug messages during development.

#### Parameters
* `dbgserial` The Stream object to use for debug output.

#### `public void `[`clearDebugOutputStream`](#class_notecard_1a21b2937a3c8adae0372c1571f737edaa)`(void)` 

Clear the debug output source.

#### `public void `[`i2cTest`](#class_notecard_1a16127c3032c423753c8b65c4c214ed7a)`(int Adjustment)` 

Adjust the I2C read length. Method enabling a developer to test the state of a known issue with the I2C HAL on some ST Microelectronics boards.

#### Parameters
* `Adjustment` The read length to override.

#### `public J * `[`newRequest`](#class_notecard_1a424d5ee778ec3e89bf4f408a6ac274d2)`(const char * request)` 

Creates a new request object for population by the host. This function accepts a request string (for example, `"note.add"`) and initializes a JSON Object to return to the host.

#### Parameters
* `request` The request name, for example, `note.add`. 

#### Returns
A `J` JSON Object populated with the request name.

#### `public bool `[`sendRequest`](#class_notecard_1a8016131b70f6af1e42c3054b577676d2)`(J * req)` 

Sends a request to the [Notecard](#class_notecard). This function takes a populated `J` JSON request object and sends it to the [Notecard](#class_notecard).

#### Parameters
* `req` A `J` JSON request object. 

#### Returns
`True` if the message was successfully sent to the [Notecard](#class_notecard), `False` if there was an error.

#### `public J * `[`requestAndResponse`](#class_notecard_1a67bbafe4ef82a6772404283e4f78bf12)`(J * req)` 

Sends a request to the [Notecard](#class_notecard) and returns the JSON Response. This function takes a populated `J` JSON request object and sends it to the [Notecard](#class_notecard).

#### Parameters
* `req` A `J` JSON request object. 

#### Returns
`J` JSON Object with the response from the [Notecard](#class_notecard).

#### `public void `[`deleteResponse`](#class_notecard_1a26db6cf767d034900033fd8794dbc8a2)`(J * rsp)` 

Deletes a `J` JSON response object from memory.

#### Parameters
* `rsp` A `J` JSON response object.

#### `public void `[`logDebug`](#class_notecard_1a083c4738102bbb2b377c4cb4f3fa11ff)`(const char * message)` 

Write a message to the serial debug stream.

#### Parameters
* `message` A string to log to the serial debug stream.

#### `public void `[`logDebugf`](#class_notecard_1a4221529c1852f535c9fd09df01c36107)`(const char * format,...)` 

Write a formatted message to the serial debug stream.

#### Parameters
* `format` A format string to log to the serial debug stream. 

* `...` one or more values to interpolate into the format string.

#### `public bool `[`debugSyncStatus`](#class_notecard_1a89b07d345de9ae988ff41ac3f5d1aea5)`(int pollFrequencyMs,int maxLevel)` 

Periodically show [Notecard](#class_notecard) sync status, returning TRUE if something was displayed to the debug stream.

#### Parameters
* `pollFrequencyMs` The frequency to poll the [Notecard](#class_notecard) for sync status. 

* `maxLevel` The maximum log level to output to the debug console. Pass -1 for all. 

#### Returns
`True` if a pending response was displayed to the debug stream.

#### `public bool `[`responseError`](#class_notecard_1a1b9d4774c186e11c0bea8050d266fd66)`(J * rsp)` 

Determines if there is an error string present in a response object.

#### Parameters
* `rsp` A `J` JSON Response object. 

#### Returns
`True` if the response object contains an error.

Generated by [Moxygen](https://sourcey.com/moxygen)