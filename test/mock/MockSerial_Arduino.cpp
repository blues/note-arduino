#include "mock/MockSerial_Arduino.hpp"

#include "NoteSerial_Arduino.hpp"

MakeNoteSerial_Parameters make_note_serial_Parameters;

NoteSerial *
make_note_serial (
    NoteSerial::channel_t serial_channel_,
    size_t baud_rate_
)
{
    // Record invocation(s)
    ++make_note_serial_Parameters.invoked;

    // Stash parameter(s)
    make_note_serial_Parameters.serial_channel = serial_channel_;
    make_note_serial_Parameters.baud_rate = baud_rate_;

    // Return user-supplied result
    return make_note_serial_Parameters.result;
}

NoteSerial_Arduino::NoteSerial_Arduino
(
    HardwareSerial & hw_serial_,
    size_t baud_rate_
) :
    _notecardSerial(hw_serial_),
    _notecardSerialSpeed(baud_rate_)
{

}

size_t
NoteSerial_Arduino::available (
    void
)
{
    size_t result = 0;

    return result;
}

char
NoteSerial_Arduino::receive (
    void
)
{
    char result = 'z';

    return result;
}

bool
NoteSerial_Arduino::reset (
    void
)
{
    bool result = true;

    return result;
}

size_t
NoteSerial_Arduino::transmit (
    uint8_t *buffer,
    size_t size,
    bool flush
)
{
    (void)buffer;
    (void)size;
    (void)flush;

    size_t result = 0;

    return result;
}
