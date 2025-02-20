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
        size_t baud_rate_
    ) :
        serial(serial_),
        baud_rate(baud_rate_)
    { }
    T & serial;
    size_t baud_rate;
};

template <typename T>
class NoteSerial_Arduino final : public NoteSerial
{
public:
    NoteSerial_Arduino(T & serial_, size_t baud_rate_);
    ~NoteSerial_Arduino(void);
    size_t available(void) override;
    char receive(void) override;
    bool reset(void) override;
    size_t transmit(uint8_t * buffer, size_t size, bool flush) override;

private:
    T & _notecardSerial;
    const int _notecardSerialSpeed;
};

#endif // NOTE_SERIAL_ARDUINO_HPP
