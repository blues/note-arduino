#ifndef NOTE_SERIAL_ARDUINO_HPP
#define NOTE_SERIAL_ARDUINO_HPP

#include "NoteSerial.hpp"

#ifndef NOTE_MOCK
#include <Arduino.h>
#else
#include "mock/mock-arduino.hpp"
#endif

template <typename T>
struct MakeNoteSerial_ArduinoParameters {
    MakeNoteSerial_ArduinoParameters (
        T & serial_,
        uint32_t baud_rate_
    ) :
        serial(serial_),
        baud_rate(baud_rate_)
    { }
    T & serial;
    uint32_t baud_rate;
};

template <typename T>
class NoteSerial_Arduino final : public NoteSerial
{
public:
    NoteSerial_Arduino(T & serial_, uint32_t baud_rate_);
    ~NoteSerial_Arduino(void);
    size_t available(void) override;
    char receive(void) override;
    bool reset(void) override;
    size_t transmit(uint8_t * buffer, size_t size, bool flush) override;
    bool setBaudRate(uint32_t rate) override;
    uint32_t getBaudRate(void) const override;

private:
    T & _notecardSerial;
    // Not `const`: `setBaudRate()` mutates this so `reset()` restores to the
    // currently-active rate rather than whatever was passed at construction.
    uint32_t _notecardSerialSpeed;
};

#endif // NOTE_SERIAL_ARDUINO_HPP
