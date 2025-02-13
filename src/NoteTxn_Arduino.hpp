#ifndef NOTE_TXN_ARDUINO_HPP
#define NOTE_TXN_ARDUINO_HPP

#include "NoteTxn.hpp"

class NoteTxn_Arduino final : public NoteTxn
{
public:
    NoteTxn_Arduino (uint8_t ctx_pin, uint8_t rtx_pin);
    bool start (uint32_t timeout_ms) override;
    void stop (void) override;

private:
    uint8_t _ctx_pin;
    uint8_t _rtx_pin;
};

template <> NoteTxn * make_note_txn <std::nullptr_t> (const std::nullptr_t & txn_pins_);
template <> NoteTxn * make_note_txn <uint8_t [2]> (uint8_t (&txn_pins_)[2]);
template <> NoteTxn * make_note_txn <const uint8_t [2]> (const uint8_t (&txn_pins_)[2]);

#endif // NOTE_TXN_ARDUINO_HPP
