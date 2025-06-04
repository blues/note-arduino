#include "mock-arduino.hpp"

Delay_Parameters delay_Parameters;
DigitalRead_Parameters digitalRead_Parameters;
DigitalWrite_Parameters digitalWrite_Parameters;
Millis_Parameters millis_Parameters;
PinMode_Parameters pinMode_Parameters;

HardwareSerialAvailable_Parameters hardwareSerialAvailable_Parameters;
HardwareSerialBegin_Parameters hardwareSerialBegin_Parameters;
HardwareSerialEnd_Parameters hardwareSerialEnd_Parameters;
HardwareSerialFlush_Parameters hardwareSerialFlush_Parameters;
HardwareSerialOperatorBool_Parameters hardwareSerialOperatorBool_Parameters;
HardwareSerialRead_Parameters hardwareSerialRead_Parameters;
HardwareSerialWrite_Parameters hardwareSerialWrite_Parameters;

SoftwareSerialAvailable_Parameters softwareSerialAvailable_Parameters;
SoftwareSerialBegin_Parameters softwareSerialBegin_Parameters;
SoftwareSerialEnd_Parameters softwareSerialEnd_Parameters;
SoftwareSerialFlush_Parameters softwareSerialFlush_Parameters;
SoftwareSerialOperatorBool_Parameters softwareSerialOperatorBool_Parameters;
SoftwareSerialRead_Parameters softwareSerialRead_Parameters;
SoftwareSerialWrite_Parameters softwareSerialWrite_Parameters;

StreamPrint_Parameters streamPrint_Parameters;

TwoWireBegin_Parameters twoWireBegin_Parameters;
TwoWireBeginTransmission_Parameters twoWireBeginTransmission_Parameters;
TwoWireEnd_Parameters twoWireEnd_Parameters;
TwoWireEndTransmission_Parameters twoWireEndTransmission_Parameters;
TwoWireRead_Parameters twoWireRead_Parameters;
TwoWireRequestFrom_Parameters twoWireRequestFrom_Parameters;
TwoWireWriteByte_Parameters twoWireWriteByte_Parameters;
TwoWireWriteBuffer_Parameters twoWireWriteBuffer_Parameters;

// Global Arduino Objects (Singletons)
HardwareSerial Serial;
SoftwareSerial SoftSerial;
TwoWire Wire;

void
delay (
    unsigned long ms
) {
    // Record invocation(s)
    ++delay_Parameters.invoked;

    // Stash parameter(s)
    delay_Parameters.ms = ms;

    // Emulate the passing time for timeout loops
    if (delay_Parameters.mock_time) {
        millis_Parameters.default_result += ms;
    }
}

int
digitalRead (
    uint8_t pin
) {
    // Record invocation(s)
    ++digitalRead_Parameters.invoked[pin];

    // Stash parameter(s)
    digitalRead_Parameters.pin = pin;

    // Return user-supplied result
    if (digitalRead_Parameters.result[pin].size() < digitalRead_Parameters.invoked[pin]) {
        return digitalRead_Parameters.default_result[pin];
    } else {
        return digitalRead_Parameters.result[pin][(digitalRead_Parameters.invoked[pin] - 1)];
    }
}

void
digitalWrite (
    uint8_t pin,
    uint8_t val
) {
    // Record invocation(s)
    ++digitalWrite_Parameters.invoked[pin];

    // Stash parameter(s)
    digitalWrite_Parameters.pin_val[pin].push_back(val);
}

unsigned long
millis (
    void
) {
    // Record invocation(s)
    ++millis_Parameters.invoked;

    // Return user-supplied result
    if (millis_Parameters.result.size() < millis_Parameters.invoked) {
        return millis_Parameters.default_result;
    } else {
        return millis_Parameters.result[(millis_Parameters.invoked - 1)];
    }
}

void
pinMode (
    uint8_t pin,
    uint8_t mode
) {
    // Record invocation(s)
    ++pinMode_Parameters.invoked[pin];

    // Stash parameter(s)
    pinMode_Parameters.pin_mode[pin].push_back(mode);
}

HardwareSerial::operator bool (
    void
) {
    // Record invocation(s)
    ++hardwareSerialOperatorBool_Parameters.invoked;

    // Return user-supplied result
    return hardwareSerialOperatorBool_Parameters.result;
}

unsigned int
HardwareSerial::available (
    void
) {
    // Record invocation(s)
    ++hardwareSerialAvailable_Parameters.invoked;

    // Return user-supplied result
    return hardwareSerialAvailable_Parameters.result;
}

void
HardwareSerial::begin (
    unsigned int baud
) {
    // Record invocation(s)
    ++hardwareSerialBegin_Parameters.invoked;

    // Stash parameter(s)
    hardwareSerialBegin_Parameters.baud = baud;
}

void
HardwareSerial::end (
    void
) {
    // Record invocation(s)
    ++hardwareSerialEnd_Parameters.invoked;
}

void
HardwareSerial::flush (
    void
) {
    // Record invocation(s)
    ++hardwareSerialFlush_Parameters.invoked;
}

char
HardwareSerial::read (
    void
) {
    // Record invocation(s)
    ++hardwareSerialRead_Parameters.invoked;

    // Return user-supplied result
    return hardwareSerialRead_Parameters.result;
}

size_t
HardwareSerial::write (
    uint8_t * buffer,
    size_t size
) {
    // Record invocation(s)
    ++hardwareSerialWrite_Parameters.invoked;

    // Stash parameter(s)
    hardwareSerialWrite_Parameters.buffer = buffer;
    hardwareSerialWrite_Parameters.buffer_cache = reinterpret_cast<char *>(buffer);
    hardwareSerialWrite_Parameters.size = size;

    // Return user-supplied result
    return hardwareSerialWrite_Parameters.result;
}

SoftwareSerial::operator bool (
    void
) {
    // Record invocation(s)
    ++softwareSerialOperatorBool_Parameters.invoked;

    // Return user-supplied result
    return softwareSerialOperatorBool_Parameters.result;
}

unsigned int
SoftwareSerial::available (
    void
) {
    // Record invocation(s)
    ++softwareSerialAvailable_Parameters.invoked;

    // Return user-supplied result
    return softwareSerialAvailable_Parameters.result;
}

void
SoftwareSerial::begin (
    unsigned int baud
) {
    // Record invocation(s)
    ++softwareSerialBegin_Parameters.invoked;

    // Stash parameter(s)
    softwareSerialBegin_Parameters.baud = baud;
}

void
SoftwareSerial::end (
    void
) {
    // Record invocation(s)
    ++softwareSerialEnd_Parameters.invoked;
}

void
SoftwareSerial::flush (
    void
) {
    // Record invocation(s)
    ++softwareSerialFlush_Parameters.invoked;
}

char
SoftwareSerial::read (
    void
) {
    // Record invocation(s)
    ++softwareSerialRead_Parameters.invoked;

    // Return user-supplied result
    return softwareSerialRead_Parameters.result;
}

size_t
SoftwareSerial::write (
    uint8_t * buffer,
    size_t size
) {
    // Record invocation(s)
    ++softwareSerialWrite_Parameters.invoked;

    // Stash parameter(s)
    softwareSerialWrite_Parameters.buffer = buffer;
    softwareSerialWrite_Parameters.buffer_cache = reinterpret_cast<char *>(buffer);
    softwareSerialWrite_Parameters.size = size;

    // Return user-supplied result
    return softwareSerialWrite_Parameters.result;
}

size_t
Stream::print (
    const char * str
) {
    // Record invocation(s)
    ++streamPrint_Parameters.invoked;

    // Stash parameter(s)
    streamPrint_Parameters.str = str;
    streamPrint_Parameters.str_cache = str;

    // Return user-supplied result
    return streamPrint_Parameters.result;
}

void
TwoWire::begin (
    void
) {
    // Record invocation(s)
    ++twoWireBegin_Parameters.invoked;
}

void
TwoWire::beginTransmission (
    uint8_t address
) {
    // Record invocation(s)
    ++twoWireBeginTransmission_Parameters.invoked;

    // Stash parameter(s)
    twoWireBeginTransmission_Parameters.address = address;
}

void
TwoWire::end (
    void
) {
    // Record invocation(s)
    ++twoWireEnd_Parameters.invoked;
}

uint8_t
TwoWire::endTransmission (
    void
) {
    uint8_t result;

    // Record invocation(s)
    const size_t invocation = twoWireEndTransmission_Parameters.invoked++;

    // Return user-supplied result if available
    if (twoWireEndTransmission_Parameters.results.size() > invocation) {
        result = twoWireEndTransmission_Parameters.results[invocation];
    } else {
        result = 0;
    }

    return result;
}

int
TwoWire::read (
    void
) {
    int result;

    // Record invocation(s)
    const size_t invocation = twoWireRead_Parameters.invoked++;

    // Return user-supplied result if available
    if (twoWireRead_Parameters.results.size() > invocation) {
        result = twoWireRead_Parameters.results[invocation];
    } else {
        result = 0;
    }

    return result;
}

uint8_t
TwoWire::requestFrom (
    int address,
    int quantity
) {
    // Record invocation(s)
    ++twoWireRequestFrom_Parameters.invoked;

    // Stash parameter(s)
    twoWireRequestFrom_Parameters.address = address;
    twoWireRequestFrom_Parameters.quantity = quantity;


    // Return user-supplied result
    return twoWireRequestFrom_Parameters.result;
}

size_t
TwoWire::write (
    uint8_t c
) {
    // Record invocation(s)
    ++twoWireWriteByte_Parameters.invoked;

    // Stash parameter(s)
    twoWireWriteByte_Parameters.c = c;

    // Buffer writes
    twoWireWriteByte_Parameters.write_buffer.push_back(c);

    // Return user-supplied result
    return twoWireWriteByte_Parameters.result;
}

size_t
TwoWire::write (
    uint8_t * buffer,
    size_t size
) {
    // Record invocation(s)
    ++twoWireWriteBuffer_Parameters.invoked;

    // Stash parameter(s)
    twoWireWriteBuffer_Parameters.buffer = buffer;
    twoWireWriteBuffer_Parameters.size = size;

    // Return user-supplied result
    return twoWireWriteBuffer_Parameters.result;
}
