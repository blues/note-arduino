#ifndef NOTE_GPIO_H
#define NOTE_GPIO_H

#include <stdint.h>

extern const uint8_t NOTE_GPIO_MODE_INPUT;
extern const uint8_t NOTE_GPIO_MODE_INPUT_PULLUP;
extern const uint8_t NOTE_GPIO_MODE_OUTPUT;
extern const uint8_t NOTE_GPIO_STATE_HIGH;
extern const uint8_t NOTE_GPIO_STATE_LOW;

extern "C" {

/**************************************************************************/
/*!
    @brief  Inspects the state of the specified digital input pin.
    @param[in]  pin The pin to inspect
    @return The current state of the pin
    @par
    - @c NOTE_GPIO_STATE_HIGH
    - @c NOTE_GPIO_STATE_LOW
*/
/**************************************************************************/
uint8_t noteDigitalRead(uint8_t pin);

/**************************************************************************/
/*!
    @brief  Modifies the state of the specified digital output pin.
    @param[in]  pin The pin to modify
    @param[in]  val The deisred state
    @par
    - @c NOTE_GPIO_STATE_HIGH
    - @c NOTE_GPIO_STATE_LOW
*/
/**************************************************************************/
void noteDigitalWrite(uint8_t pin, uint8_t val);

/**************************************************************************/
/*!
    @brief  Modifies the operating mode of a pin.
    @param[in]  pin The pin to modify
    @param[in]  mode The deisred operating mode
    @par
    - @c NOTE_GPIO_MODE_INPUT
    - @c NOTE_GPIO_MODE_INPUT_PULLUP
    - @c NOTE_GPIO_MODE_OUTPUT
*/
/**************************************************************************/
void notePinMode(uint8_t pin, uint8_t mode);

}

#endif // NOTE_GPIO_H
