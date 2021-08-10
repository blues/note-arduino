#ifndef MOCK_SERIAL_ARDUINO_HPP
#define MOCK_SERIAL_ARDUINO_HPP

#include <stddef.h>
#include <stdint.h>

#include "NoteSerial.hpp"

struct MakeNoteSerial_Parameters {
    MakeNoteSerial_Parameters(
        void
    ) :
        invoked(0),
        serial_channel(nullptr),
        baud_rate(0),
        result(nullptr)
    { }
    void reset (
        void
    ) {
        invoked = 0;
        serial_channel = nullptr;
        baud_rate = 0;
        result = nullptr;
    }
    size_t invoked;
    NoteSerial::channel_t serial_channel;
    size_t baud_rate;
    NoteSerial * result;
};

extern MakeNoteSerial_Parameters make_note_serial_Parameters;

#endif // MOCK_SERIAL_ARDUINO_HPP
