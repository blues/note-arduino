#ifndef MOCK_NOTE_SERIAL_HPP
#define MOCK_NOTE_SERIAL_HPP

#include <stddef.h>
#include <stdint.h>

#include "NoteSerial.hpp"
#include "mock/mock-arduino.hpp"

class NoteSerial_Mock final : public NoteSerial
{
public:
    size_t available(void) override;
    char receive(void) override;
    bool reset(void) override;
    size_t transmit(uint8_t * buffer, size_t size, bool flush) override;
    bool setBaudRate(uint32_t rate) override;
    uint32_t getBaudRate(void) const override;
};

template <typename T>
struct MakeNoteSerial_Parameters {
    MakeNoteSerial_Parameters(
        void
    ) :
        invoked(0),
        serial_parameters(nullptr),
        result(nullptr)
    { }
    void reset (
        void
    ) {
        invoked = 0;
        serial_parameters = nullptr;
        result = nullptr;
    }
    size_t invoked;
    T * serial_parameters;
    NoteSerial * result;
};

struct NoteSerialAvailable_Parameters {
    NoteSerialAvailable_Parameters(
        void
    ) :
        invoked(0),
        result(0)
    { }
    void reset (
        void
    ) {
        invoked = 0;
        result = 0;
    }
    size_t invoked;
    size_t result;
};

struct NoteSerialReceive_Parameters {
    NoteSerialReceive_Parameters(
        void
    ) :
        invoked(0),
        result('\0')
    { }
    void reset (
        void
    ) {
        invoked = 0;
        result = '\0';
    }
    size_t invoked;
    char result;
};

struct NoteSerialReset_Parameters {
    NoteSerialReset_Parameters(
        void
    ) :
        invoked(0),
        result(false)
    { }
    void reset (
        void
    ) {
        invoked = 0;
        result = false;
    }
    size_t invoked;
    bool result;
};

struct NoteSerialTransmit_Parameters {
    NoteSerialTransmit_Parameters(
        void
    ) :
        invoked(0),
        buffer(nullptr),
        size(0),
        flush(false),
        result(0)
    { }
    void reset (
        void
    ) {
        invoked = 0;
        buffer = nullptr;
        size = 0;
        flush = false;
        result = 0;
    }
    size_t invoked;
    uint8_t * buffer;
    size_t size;
    bool flush;
    size_t result;
};

struct NoteSerialSetBaudRate_Parameters {
    NoteSerialSetBaudRate_Parameters(
        void
    ) :
        invoked(0),
        rate(0),
        result(false)
    { }
    void reset (
        void
    ) {
        invoked = 0;
        rate = 0;
        result = false;
    }
    size_t invoked;
    uint32_t rate;
    bool result;
};

struct NoteSerialGetBaudRate_Parameters {
    NoteSerialGetBaudRate_Parameters(
        void
    ) :
        invoked(0),
        result(0)
    { }
    void reset (
        void
    ) {
        invoked = 0;
        result = 0;
    }
    mutable size_t invoked;
    uint32_t result;
};

extern MakeNoteSerial_Parameters<HardwareSerial> make_note_serial_Parameters;
extern NoteSerialAvailable_Parameters noteSerialAvailable_Parameters;
extern NoteSerialReceive_Parameters noteSerialReceive_Parameters;
extern NoteSerialReset_Parameters noteSerialReset_Parameters;
extern NoteSerialTransmit_Parameters noteSerialTransmit_Parameters;
extern NoteSerialSetBaudRate_Parameters noteSerialSetBaudRate_Parameters;
extern NoteSerialGetBaudRate_Parameters noteSerialGetBaudRate_Parameters;

#endif // MOCK_NOTE_SERIAL_HPP
