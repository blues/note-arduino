#include "NoteLog_Arduino.hpp"

// Singleton instance of the NoteLog_Arduino class
namespace instance {
    inline NoteLog* & note_log (void) {
        static NoteLog* note_log = nullptr;
        return note_log;
    }
};

NoteLog *
make_note_log (
    nullptr_t
) {
    NoteLog* & note_log = instance::note_log();
    if (note_log) {
        delete note_log;
        note_log = nullptr;
    }
    return note_log;
}

template <typename T>
NoteLog *
make_note_log (
    T & log_parameters_
) {
    NoteLog* & note_log = instance::note_log();
    if (!note_log) {
        note_log = new NoteLog_Arduino(reinterpret_cast<T *>(&log_parameters_));
    }

    return note_log;
}

NoteLog_Arduino::NoteLog_Arduino
(
    Stream * log_stream_
) :
    _notecardLog(log_stream_)
{ }

size_t
NoteLog_Arduino::print (
    const char * str_
)
{
    size_t result;

    result = _notecardLog->print(str_);

    return result;
}

// Explicitly instantiate the template function for the supported types
template NoteLog * make_note_log<Stream>(Stream &);
