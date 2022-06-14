#include "mock/NoteLog_Mock.hpp"

MakeNoteLog_Parameters make_note_log_Parameters;
NoteLogPrint_Parameters noteLogPrint_Parameters;

NoteLog *
make_note_log (
    NoteLog::param_t log_parameters_
)
{
    // Record invocation(s)
    ++make_note_log_Parameters.invoked;

    // Stash parameter(s)
    make_note_log_Parameters.log_parameters = log_parameters_;

    // Return user-supplied result
    return make_note_log_Parameters.result;
}

size_t
NoteLog_Mock::print (
    const char * message_
)
{
    // Record invocation(s)
    ++noteLogPrint_Parameters.invoked;

    // Stash parameter(s)
    noteLogPrint_Parameters.message = message_;

    // Return user-supplied result
    return noteLogPrint_Parameters.result;
}
