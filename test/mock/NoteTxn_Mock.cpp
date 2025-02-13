#include "mock/NoteTxn_Mock.hpp"

MakeNoteTxn_Parameters<const uint8_t [2]> make_note_txn_Parameters;
NoteTxnStart_Parameters noteTxnStart_Parameters;
NoteTxnStop_Parameters noteTxnStop_Parameters;

NoteTxn *
make_note_txn (
    nullptr_t
) {
    // Record invocation(s)
    ++make_note_txn_Parameters.invoked;

    // Stash parameter(s)

    // Return user-supplied result
    return make_note_txn_Parameters.result;
}

template <typename T>
NoteTxn *
make_note_txn (
    T & txn_parameters_
)
{
    // Record invocation(s)
    ++make_note_txn_Parameters.invoked;

    // Stash parameter(s)
    make_note_txn_Parameters.txn_parameters = txn_parameters_;

    // Return user-supplied result
    return make_note_txn_Parameters.result;
}

bool
NoteTxn_Mock::start (
    uint32_t timeout_ms_
) {
    // Record invocation(s)
    ++noteTxnStart_Parameters.invoked;

    // Stash parameter(s)
    noteTxnStart_Parameters.timeout_ms = timeout_ms_;

    // Return user-supplied result
    return noteTxnStart_Parameters.result;
}

void
NoteTxn_Mock::stop (
    void
) {
    // Record invocation(s)
    ++noteTxnStop_Parameters.invoked;

    // Stash parameter(s)

    // Return user-supplied result
}
