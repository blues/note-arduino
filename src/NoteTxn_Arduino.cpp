#include "NoteTxn_Arduino.hpp"

#ifndef NOTE_MOCK
  #include <Arduino.h>
#else
  #include "mock/mock-arduino.hpp"
  #include "mock/mock-parameters.hpp"
#endif

// Singleton instance of the NoteTxn_Arduino class
namespace instance {
    inline NoteTxn* & note_txn (void) {
        static NoteTxn* note_txn = nullptr;
        return note_txn;
    }
};

NoteTxn *
make_note_txn (
    nullptr_t
) {
    NoteTxn* & note_txn = instance::note_txn();
    if (note_txn) {
        delete note_txn;
        note_txn = nullptr;
    }
    return note_txn;
}

template <typename T>
NoteTxn *
make_note_txn (
    T & txn_pins_
) {
    NoteTxn* & note_txn = instance::note_txn();

    if (txn_pins_[0] == txn_pins_[1]) {
        // Invalid tuple invokes deletion
        if (note_txn) {
            delete note_txn;
            note_txn = nullptr;
        }
    } else if (!note_txn) {
        note_txn = new NoteTxn_Arduino(txn_pins_[0], txn_pins_[1]);
    }

    return note_txn;
}

NoteTxn_Arduino::NoteTxn_Arduino
(
    uint8_t ctx_pin_,
    uint8_t rtx_pin_
) :
    _ctx_pin(ctx_pin_),
    _rtx_pin(rtx_pin_)
{
    // Float CTX/RTX to conserve energy
    ::pinMode(_ctx_pin, INPUT);
    ::pinMode(_rtx_pin, INPUT);
}

bool
NoteTxn_Arduino::start (
    uint32_t timeout_ms_
)
{
    bool result = false;

    // Signal Request To Transact
    ::pinMode(_rtx_pin, OUTPUT);
    ::digitalWrite(_rtx_pin, HIGH);

    // Await Clear To Transact Signal
    ::pinMode(_ctx_pin, INPUT_PULLUP);
    for (uint32_t timeout = (::millis() + timeout_ms_)
       ; ::millis() < timeout
       ; ::delay(1)
    ) {
        if (HIGH == ::digitalRead(_ctx_pin)) {
            result = true;
            break;
        }
    }

    // Float CTX to conserve energy
    ::pinMode(_ctx_pin, INPUT);

    // Abandon request on timeout
    if (!result) {
        stop();
    }

    return result;
}

void
NoteTxn_Arduino::stop (
    void
)
{
    // Float RTX pin
    ::pinMode(_rtx_pin, INPUT);
}

// Explicitly instantiate the template function for array types
template NoteTxn * make_note_txn<uint8_t[2]>(uint8_t(&)[2]);
template NoteTxn * make_note_txn<const uint8_t[2]>(const uint8_t(&)[2]);
