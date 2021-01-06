#include "mock-arduino.hpp"

HardwareSerialBegin_Parameters hardwareSerialBegin_Parameters;

void
delay (
    unsigned int millis
) {
    // Validate parameter(s)

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
    return 0;
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

}

char
HardwareSerial::read (
    void
) {
    return 'z';
}

void
HardwareSerial::write (
    unsigned char * text,
    unsigned int len
) {

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

}

void
TwoWire::beginTransmission (
    int
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
    return 0;
}

void
TwoWire::write (
    unsigned char c
) {

}

long unsigned int
TwoWire::write (
    unsigned char * msg,
    long unsigned int len
) {
    return 0;
}

HardwareSerial Serial;
TwoWire Wire;
Stream dbgserial;
