#ifndef MOCK_NOTE_TIME_HPP
#define MOCK_NOTE_TIME_HPP

#include <stddef.h>
#include <stdint.h>

struct NoteDelay_Parameters {
    NoteDelay_Parameters(
        void
    ) :
        invoked(0),
        ms(0)
    { }
    void reset (
        void
    ) {
        invoked = 0;
        ms = 0;
    }
    size_t invoked;
    uint32_t ms;
};

struct NoteMillis_Parameters {
    NoteMillis_Parameters(
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
    uint32_t result;
};

extern NoteDelay_Parameters noteDelay_Parameters;
extern NoteMillis_Parameters noteMillis_Parameters;

#endif // MOCK_NOTE_TIME_HPP
