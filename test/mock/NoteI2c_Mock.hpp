#ifndef MOCK_NOTE_I2C_HPP
#define MOCK_NOTE_I2C_HPP

#include <stddef.h>
#include <stdint.h>

#include <string>

#include "NoteI2c.hpp"
#include "mock/mock-arduino.hpp"

class NoteI2c_Mock final : public NoteI2c
{
public:
    const char * receive(uint16_t device_address, uint8_t * buffer, uint16_t requested_byte_count, uint32_t * available) override;
    bool reset(uint16_t device_address) override;
    const char * transmit(uint16_t device_address, uint8_t * buffer, uint16_t size) override;
};

template <typename T>
struct MakeNoteI2c_Parameters {
    MakeNoteI2c_Parameters(
        void
    ) :
        invoked(0),
        i2c_parameters(nullptr),
        result(nullptr)
    { }
    void reset (
        void
    ) {
        invoked = 0;
        i2c_parameters = nullptr;
        result = nullptr;
    }
    size_t invoked;
    T * i2c_parameters;
    NoteI2c * result;
};

struct NoteI2cReceive_Parameters {
    NoteI2cReceive_Parameters(
        void
    ) :
        invoked(0),
        device_address(0),
        buffer(nullptr),
        requested_byte_count(0),
        available(nullptr),
        result(nullptr)
    { }
    void reset (
        void
    ) {
        invoked = 0;
        device_address = 0;
        buffer = nullptr;
        buffer_cache.clear();
        requested_byte_count = 0;
        available = nullptr;
        result = nullptr;
    }
    size_t invoked;
    uint16_t device_address;
    uint8_t * buffer;
    std::string buffer_cache;
    uint16_t requested_byte_count;
    uint32_t * available;
    const char * result;
};

struct NoteI2cReset_Parameters {
    NoteI2cReset_Parameters(
        void
    ) :
        invoked(0),
        device_address(0),
        result(false)
    { }
    void reset (
        void
    ) {
        invoked = 0;
        device_address = 0;
        result = false;
    }
    size_t invoked;
    uint16_t device_address;
    bool result;
};

struct NoteI2cTransmit_Parameters {
    NoteI2cTransmit_Parameters(
        void
    ) :
        invoked(0),
        device_address(0),
        buffer(nullptr),
        size(0),
        result(nullptr)
    { }
    void reset (
        void
    ) {
        invoked = 0;
        device_address = 0;
        buffer = nullptr;
        buffer_cache.clear();
        size = 0;
        result = nullptr;
    }
    size_t invoked;
    uint16_t device_address;
    uint8_t * buffer;
    std::string buffer_cache;
    uint16_t size;
    const char * result;
};

extern MakeNoteI2c_Parameters<TwoWire> make_note_i2c_Parameters;
extern NoteI2cReceive_Parameters noteI2cReceive_Parameters;
extern NoteI2cReset_Parameters noteI2cReset_Parameters;
extern NoteI2cTransmit_Parameters noteI2cTransmit_Parameters;

#endif // MOCK_NOTE_I2C_HPP
