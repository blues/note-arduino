#include "mock/MockI2c_Arduino.hpp"

#include "NoteI2c_Arduino.hpp"

MakeNoteI2c_Parameters make_note_i2c_Parameters;

NoteI2c *
make_note_i2c (
    NoteI2c::bus_t i2c_bus_
)
{
    // Record invocation(s)
    ++make_note_i2c_Parameters.invoked;

    // Stash parameter(s)
    make_note_i2c_Parameters.i2c_bus = i2c_bus_;

    // Return user-supplied result
    return make_note_i2c_Parameters.result;
}

NoteI2c_Arduino::NoteI2c_Arduino
(
    TwoWire & i2c_bus_
) :
    _i2cPort(i2c_bus_)
{

}

const char *
NoteI2c_Arduino::receive (
    uint16_t device_address_,
    uint8_t * buffer_,
    uint16_t requested_byte_count_,
    uint32_t * available_
) {
    (void)device_address_;
    (void)buffer_;
    (void)requested_byte_count_;
    (void)available_;

    const char * result = nullptr;

    return result;
}

bool
NoteI2c_Arduino::reset (
    uint16_t device_address_
) {
    (void)device_address_;

    return true;
}

const char *
NoteI2c_Arduino::transmit (
    uint16_t device_address_,
    uint8_t * buffer_,
    uint16_t size_
) {
    (void)device_address_;
    (void)buffer_;
    (void)size_;

    const char * result = nullptr;

    return result;
}
