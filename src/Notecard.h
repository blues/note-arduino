/*!
 * @file Notecard.h
 *
 * The note-arduino Arduino library for communicating with the
 * <a href="https://blues.io">Blues Wireless</a>
 * Notecard via serial or I2C.
 *
 * This library allows you to control a Notecard by writing an Arduino sketch in
 * C or C++. Your sketch may programmatically configure Notecard and send Notes
 * to <a href="https://notehub.io">Notehub.io</a>.
 *
 * This library is a wrapper around the
 * <a href="https://github.com/blues/note-c">note-c library</a>, which it
 * includes as a git submodule.
 *
 * Written by Ray Ozzie and Brandon Satrom for Blues Inc.
 *
 * MIT License. Use of this source code is governed by licenses granted by the
 * copyright holder including that found in the
 * <a href="https://github.com/blues/note-arduino/blob/master/LICENSE">LICENSE</a>
 * file.
 *
 */
#ifndef Notecard_h
#define Notecard_h

#include <stddef.h>
#include <stdint.h>

#ifndef NOTE_MOCK
#include <Arduino.h>
#include <Wire.h>
#include <note-c/note.h>
#else
#include "mock/mock-arduino.hpp"
#include "mock/mock-parameters.hpp"
#endif

#ifdef ARDUINO_ARCH_ESP32

/*
 * \brief Adafruit Feather-Specific Definitions
 *
 * The Adafruit feather specification defines standard pinout names for digital
 * and analog pins. Unfortunately, the Adafruit Huzzah32 does not comply with
 * this specification. As such, we provide mappings for the pins that are
 * technically incorrect but which make it easier to write code that works
 * across different feathers.
 *
 * On the ESP32 for instance, the pinout for Digital Pin 14 is located where
 * D5 is usually located, so our symbol maps D5 to Digital Pin 14 on that
 * board, etc...
 *
 * \see https://learn.adafruit.com/adafruit-feather/feather-specification
 */

#ifdef B0
#undef B0
#endif
#define B0 21

#ifdef D5
#undef D5
#endif
#define D5 14

#ifdef D6
#undef D6
#endif
#define D6 32

#ifdef D9
#undef D9
#endif
#define D9 15

#ifdef D10
#undef D10
#endif
#define D10 33

#ifdef D11
#undef D11
#endif
#define D11 27

#ifdef D12
#undef D12
#endif
#define D12 12

#ifdef D13
#undef D13
#endif
#define D13 13

#elif defined(ARDUINO_SWAN_R5)

#ifdef CS
#undef CS
#endif
#define CS PD0

#ifdef B0
#undef B0
#endif
#define B0 CS

#else

#ifndef D5
#define D5 5
#endif

#ifndef D6
#define D6 6
#endif

#ifndef D9
#define D9 9
#endif

#ifndef D10
#define D10 10
#endif

#ifndef D11
#define D11 11
#endif

#ifndef D12
#define D12 12
#endif

#ifndef D13
#define D13 13
#endif

#endif

/**************************************************************************/
/*!
    @brief  Class that stores state and functions for interacting with the
    Blues Wireless Notecard.
*/
/**************************************************************************/
class Notecard
{
public:
    ~Notecard(void);
    void begin(uint32_t i2cAddress = NOTE_I2C_ADDR_DEFAULT,
               uint32_t i2cMax = NOTE_I2C_MAX_DEFAULT,
               TwoWire &wirePort = Wire);
    void begin(HardwareSerial &serial, int speed = 9600);
    void setDebugOutputStream(Stream &dbgserial);
    void clearDebugOutputStream(void);
    J *newRequest(const char *request);
    J *newCommand(const char *request);
    bool sendRequest(J *req);
    J *requestAndResponse(J *req);
    void deleteResponse(J *rsp);
    void logDebug(const char *message);
    void logDebugf(const char *format, ...);
    bool debugSyncStatus(int pollFrequencyMs, int maxLevel);
    bool responseError(J *rsp);
};

#endif
