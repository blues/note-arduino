#ifndef MOCK_LOG_ARDUINO_HPP
#define MOCK_LOG_ARDUINO_HPP

#include <stddef.h>
#include <stdint.h>

#include "NoteLog.hpp"

struct MakeNoteLog_Parameters {
    MakeNoteLog_Parameters(
        void
    ) :
        invoked(0),
        log_channel(nullptr),
        result(nullptr)
    { }
    void reset (
        void
    ) {
        invoked = 0;
        log_channel = nullptr;
        result = nullptr;
    }
    size_t invoked;
    NoteLog::channel_t log_channel;
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

extern MakeNoteLog_Parameters make_note_log_Parameters;
extern NoteLogPrint_Parameters noteLogPrint_Parameters;

#endif // MOCK_LOG_ARDUINO_HPP
