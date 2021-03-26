#include "mock-arduino.hpp"

HardwareSerialAvailable_Parameters hardwareSerialAvailable_Parameters;
HardwareSerialBegin_Parameters hardwareSerialBegin_Parameters;
HardwareSerialFlush_Parameters hardwareSerialFlush_Parameters;
HardwareSerialRead_Parameters hardwareSerialRead_Parameters;
HardwareSerialWrite_Parameters hardwareSerialWrite_Parameters;
TwoWireBegin_Parameters twoWireBegin_Parameters;

// Global Arduino Objects (Singletons)
HardwareSerial Serial;
TwoWire Wire;
Stream dbgserial;

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

    return hardwareSerialAvailable_Parameters.result;
}

void
HardwareSerial::begin (
    unsigned int baud
) {
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

    return hardwareSerialRead_Parameters.result;
}

size_t
HardwareSerial::write (
    uint8_t * buffer,
    size_t size
) {
    // Stash parameter(s)
    hardwareSerialWrite_Parameters.buffer = buffer;
    hardwareSerialWrite_Parameters.size = size;

    return hardwareSerialWrite_Parameters.result;
}

long unsigned int
Stream::print (
    const char *
) {
    return 0;
}

void
TwoWire::begin (
    void
) {
  // Capture call
  twoWireBegin_Parameters.called = true;
}

void
TwoWire::beginTransmission (
    int
) {

}

void
TwoWire::end (
    void
) {

}

int
TwoWire::endTransmission (
    void
) {
    return 0;
}

unsigned char
TwoWire::read (
    void
) {
    return 'z';
}

int
TwoWire::requestFrom (
    int dev_addr,
    unsigned int read_len
) {
    (void)dev_addr;
    (void)read_len;
    return 0;
}

void
TwoWire::write (
    unsigned char c
) {
    (void)c;
}

long unsigned int
TwoWire::write (
    unsigned char * msg,
    long unsigned int len
) {
    (void)msg;
    (void)len;
    return 0;
}
