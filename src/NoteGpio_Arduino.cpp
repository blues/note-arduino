
#include "NoteGpio.h"

#include <stdint.h>

#ifndef NOTE_MOCK
#include <Arduino.h>
#else
#include "mock/mock-arduino.hpp"
#endif

const uint8_t NOTE_GPIO_MODE_INPUT = INPUT;
const uint8_t NOTE_GPIO_MODE_INPUT_PULLUP = INPUT_PULLUP;
const uint8_t NOTE_GPIO_MODE_OUTPUT = OUTPUT;
const uint8_t NOTE_GPIO_STATE_HIGH = HIGH;
const uint8_t NOTE_GPIO_STATE_LOW = LOW;

uint8_t noteDigitalRead(uint8_t pin_)
{
    return ::digitalRead(pin_);
}

void noteDigitalWrite(uint8_t pin_, uint8_t val_)
{
    ::digitalWrite(pin_,val_);
}

void notePinMode(uint8_t pin_, uint8_t mode_)
{
    ::pinMode(pin_,mode_);
}
