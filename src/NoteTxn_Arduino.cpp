#include "NoteTxn_Arduino.hpp"

#ifndef NOTE_MOCK
  #include <Arduino.h>
#else
  #include "mock/mock-arduino.hpp"
  #include "mock/mock-parameters.hpp"
#endif

NoteTxn *
make_note_txn (
    NoteTxn::param_t txn_parameters_
)
{
    static NoteTxn * note_txn = nullptr;
    if (!txn_parameters_) {
        if (note_txn) {
            delete note_txn;
            note_txn = nullptr;
        }
    } else if (!note_txn) {
        const uint8_t * txn_pins = reinterpret_cast<uint8_t *>(txn_parameters_);
        note_txn = new NoteTxn_Arduino(txn_pins[0], txn_pins[1]);
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
