#include "mock/NoteSerial_Mock.hpp"

MakeNoteSerial_Parameters<HardwareSerial> make_note_serial_Parameters;
NoteSerialAvailable_Parameters noteSerialAvailable_Parameters;
NoteSerialReceive_Parameters noteSerialReceive_Parameters;
NoteSerialReset_Parameters noteSerialReset_Parameters;
NoteSerialTransmit_Parameters noteSerialTransmit_Parameters;

NoteSerial *
make_note_serial (
    nullptr_t
) {
    // Record invocation(s)
    ++make_note_serial_Parameters.invoked;

    // Stash parameter(s)
    make_note_serial_Parameters.serial_parameters = nullptr;

    // Return user-supplied result
    return make_note_serial_Parameters.result;
}

template <typename T>
NoteSerial *
make_note_serial (
    T & serial_parameters_
)
{
    // Record invocation(s)
    ++make_note_serial_Parameters.invoked;

    // Stash parameter(s)
    make_note_serial_Parameters.serial_parameters = &serial_parameters_;

    // Return user-supplied result
    return make_note_serial_Parameters.result;
}

size_t
NoteSerial_Mock::available (
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
NoteSerial_Mock::receive (
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
NoteSerial_Mock::reset (
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
NoteSerial_Mock::transmit (
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
