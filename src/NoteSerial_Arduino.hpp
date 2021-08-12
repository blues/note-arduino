#ifndef NOTE_SERIAL_ARDUINO_HPP
#define NOTE_SERIAL_ARDUINO_HPP

#include "NoteSerial.hpp"

#ifndef NOTE_MOCK
#include <Arduino.h>
#else
#include "mock/mock-arduino.hpp"
#endif

class NoteSerial_Arduino final : public NoteSerial
{
public:
    NoteSerial_Arduino(HardwareSerial & hw_serial_, size_t baud_rate_);
    size_t available(void) override;
    char receive(void) override;
    bool reset(void) override;
    size_t transmit(uint8_t * buffer, size_t size, bool flush) override;

private:
    HardwareSerial & _notecardSerial;
    const int _notecardSerialSpeed;
};

#endif // NOTE_SERIAL_ARDUINO_HPP
