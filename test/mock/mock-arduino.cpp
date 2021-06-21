#include "mock-arduino.hpp"

HardwareSerialAvailable_Parameters hardwareSerialAvailable_Parameters;
HardwareSerialBegin_Parameters hardwareSerialBegin_Parameters;
HardwareSerialFlush_Parameters hardwareSerialFlush_Parameters;
HardwareSerialRead_Parameters hardwareSerialRead_Parameters;
HardwareSerialWrite_Parameters hardwareSerialWrite_Parameters;
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
TwoWire Wire;

void
delay (
    unsigned int millis
) {
    (void)millis;
}

size_t
millis (
    void
) {
    return 0;
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
