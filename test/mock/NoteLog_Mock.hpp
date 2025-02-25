#ifndef MOCK_NOTE_LOG_HPP
#define MOCK_NOTE_LOG_HPP

#include <stddef.h>
#include <stdint.h>

#include "NoteLog.hpp"
#include "mock-arduino.hpp"

class NoteLog_Mock final : public NoteLog
{
public:
    size_t print(const char * message) override;
};

template <typename T>
struct MakeNoteLog_Parameters {
    MakeNoteLog_Parameters(
        void
    ) :
        invoked(0),
        log_parameters(nullptr),
        result(nullptr)
    { }
    void reset (
        void
    ) {
        invoked = 0;
        log_parameters = nullptr;
        result = nullptr;
    }
    size_t invoked;
    T * log_parameters;
    NoteLog * result;
};

struct NoteLogPrint_Parameters {
    NoteLogPrint_Parameters(
        void
    ) :
        invoked(0),
        message(nullptr),
        result(0)
    { }
    void reset (
        void
    ) {
        invoked = 0;
        message = nullptr;
        result = 0;
    }
    size_t invoked;
    const char * message;
    size_t result;
};

extern MakeNoteLog_Parameters<HardwareSerial> make_note_log_Parameters;
extern NoteLogPrint_Parameters noteLogPrint_Parameters;

#endif // MOCK_NOTE_LOG_HPP
