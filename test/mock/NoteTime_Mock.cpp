
#include "mock/NoteTime_Mock.hpp"

#include "NoteTime.h"

NoteDelay_Parameters noteDelay_Parameters;
NoteMillis_Parameters noteMillis_Parameters;

void noteDelay (
    uint32_t ms_
) {
    // Record invocation(s)
    ++noteDelay_Parameters.invoked;

    // Stash parameter(s)
    noteDelay_Parameters.ms = ms_;
}

uint32_t noteMillis (
    void
) {
    // Record invocation(s)
    ++noteMillis_Parameters.invoked;

    // Return user-supplied result
    return noteMillis_Parameters.result;
}
