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

#endif // NOTE_TXN_ARDUINO_HPP
