#ifndef MOCK_NOTE_TXN_HPP
#define MOCK_NOTE_TXN_HPP

#include <stddef.h>
#include <stdint.h>

#include "NoteTxn.hpp"

class NoteTxn_Mock final : public NoteTxn
{
public:
    bool start (uint32_t timeout_ms) override;
    void stop (void) override;
};

template <typename T>
struct MakeNoteTxn_Parameters {
    MakeNoteTxn_Parameters(
        void
    ) :
        invoked(0),
        txn_parameters{0, 0},
        result(nullptr)
    { }
    void reset (
        void
    ) {
        invoked = 0;
        txn_parameters = nullptr;
        result = nullptr;
    }
    size_t invoked;
    T txn_parameters;
    NoteTxn * result;
};

struct NoteTxnStart_Parameters {
    NoteTxnStart_Parameters(
        void
    ) :
        invoked(0),
        timeout_ms(0),
        result(false)
    { }
    void reset (
        void
    ) {
        invoked = 0;
        timeout_ms = 0;
        result = false;
    }
    size_t invoked;
    uint32_t timeout_ms;
    bool result;
};

struct NoteTxnStop_Parameters {
    NoteTxnStop_Parameters(
        void
    ) :
        invoked(0)
    { }
    void reset (
        void
    ) {
        invoked = 0;
    }
    size_t invoked;
};

extern MakeNoteTxn_Parameters<const uint8_t [2]> make_note_txn_Parameters;
extern NoteTxnStart_Parameters noteTxnStart_Parameters;
extern NoteTxnStop_Parameters noteTxnStop_Parameters;

#endif // MOCK_NOTE_TXN_HPP
