#include "mock/NoteSerial_Mock.hpp"

#include "NoteSerial_Arduino.hpp"

MakeNoteSerial_Parameters make_note_serial_Parameters;
NoteSerialAvailable_Parameters noteSerialAvailable_Parameters;
NoteSerialReceive_Parameters noteSerialReceive_Parameters;
NoteSerialReset_Parameters noteSerialReset_Parameters;
NoteSerialTransmit_Parameters noteSerialTransmit_Parameters;

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

NoteSerial_Arduino::NoteSerial_Arduino (
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
    // Record invocation(s)
    ++noteSerialAvailable_Parameters.invoked;

    // Stash parameter(s)

    // Return user-supplied result
    return noteSerialAvailable_Parameters.result;
}

char
NoteSerial_Arduino::receive (
    void
)
{
    // Record invocation(s)
    ++noteSerialReceive_Parameters.invoked;

    // Stash parameter(s)

    // Return user-supplied result
    return noteSerialReceive_Parameters.result;
}

bool
NoteSerial_Arduino::reset (
    void
)
{
    // Record invocation(s)
    ++noteSerialReset_Parameters.invoked;

    // Stash parameter(s)

    // Return user-supplied result
    return noteSerialReset_Parameters.result;
}

size_t
NoteSerial_Arduino::transmit (
    uint8_t *buffer_,
    size_t size_,
    bool flush_
)
{
    // Record invocation(s)
    ++noteSerialTransmit_Parameters.invoked;

    // Stash parameter(s)
    noteSerialTransmit_Parameters.buffer = buffer_;
    noteSerialTransmit_Parameters.size = size_;
    noteSerialTransmit_Parameters.flush = flush_;

    // Return user-supplied result
    return noteSerialTransmit_Parameters.result;
}
