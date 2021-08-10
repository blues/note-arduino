#ifndef MOCK_I2C_ARDUINO_HPP
#define MOCK_I2C_ARDUINO_HPP

#include <stddef.h>
#include <stdint.h>

#include "NoteI2c.hpp"

struct MakeNoteI2c_Parameters {
    MakeNoteI2c_Parameters(
        void
    ) :
        invoked(0),
        i2c_bus(nullptr),
        result(nullptr)
    { }
    void reset (
        void
    ) {
        invoked = 0;
        i2c_bus = nullptr;
        result = nullptr;
    }
    size_t invoked;
    NoteI2c::bus_t i2c_bus;
    NoteI2c * result;
};

extern MakeNoteI2c_Parameters make_note_i2c_Parameters;

#endif // MOCK_I2C_ARDUINO_HPP
