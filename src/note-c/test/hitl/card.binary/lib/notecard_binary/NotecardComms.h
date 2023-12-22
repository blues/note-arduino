#ifdef PLATFORMIO

#pragma once

#include <Notecard.h>

enum NotecardInterface {
    NOTECARD_IF_I2C=1,
    NOTECARD_IF_SERIAL,
    NOTECARD_IF_AUX_SERIAL
};

#ifndef NOTECARD_IF_SERIAL_PORT
#define NOTECARD_IF_SERIAL_PORT Serial3
#endif

#ifndef NOTECARD_IF_SERIAL_BAUDRATE
#define NOTECARD_IF_SERIAL_BAUDRATE (9600)
#endif

#ifndef NOTECARD_IF_AUX_SERIAL_PORT
#define NOTECARD_IF_AUX_SERIAL_PORT Serial1
#endif

#ifndef NOTECARD_IF_AUX_SERIAL_BAUDRATE
#define NOTECARD_IF_AUX_SERIAL_BAUDRATE 115200
#endif

#ifndef NOTECARD_IF_I2C_ADDRESS
#define NOTECARD_IF_I2C_ADDRESS NOTE_I2C_ADDR_DEFAULT
#endif



bool initialize_notecard_interface(NotecardInterface iface);
size_t readDataUntilTimeout(Stream& serial, size_t timeout, uint8_t* buf, size_t bufLen, size_t dataLen, size_t& duration);

/**
 * @brief Sets the baudrate of the aux serial interface and the mode to "req".
 *
 * @param baudrate The desired baudrate to set
 * @param nif The notecard interface to use to send the request.
 * @return true The baudrate was set
 * @return false An error occurred. See the Notecard debug stream for details.
 */
bool set_aux_serial_baudrate(size_t baudrate=NOTECARD_IF_AUX_SERIAL_BAUDRATE, NotecardInterface nif=NOTECARD_IF_I2C);


extern Notecard notecard;

#endif // PLATFORMIO
