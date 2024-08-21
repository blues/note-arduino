/*!
 * @file Notecard.h
 *
 * The note-arduino Arduino library for communicating with the
 * <a href="https://blues.com">Blues</a>
 * Notecard via serial or I2C.
 *
 * This library allows you to control a Notecard by writing an Arduino sketch in
 * C or C++. Your sketch may programmatically configure Notecard and send Notes
 * to <a href="https://notehub.io">Notehub.io</a>.
 *
 * This library is a wrapper around the
 * <a href="https://github.com/blues/note-c">note-c library</a>, which it
 * includes as a git subtree.
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

#include "NoteI2c.hpp"
#include "NoteLog.hpp"
#include "NoteSerial.hpp"
#include "NoteTxn.hpp"

#ifdef ARDUINO
#include <Arduino.h>
#include <Wire.h>
#include "NoteSerial_Arduino.hpp"
#endif

#ifndef NOTE_MOCK
#include <note-c/note.h>
#else
#include "mock/mock-arduino.hpp"
#include "mock/mock-parameters.hpp"
#endif

#define NOTE_ARDUINO_VERSION_MAJOR 1
#define NOTE_ARDUINO_VERSION_MINOR 6
#define NOTE_ARDUINO_VERSION_PATCH 3

#define NOTE_ARDUINO_VERSION NOTE_C_STRINGIZE(NOTE_ARDUINO_VERSION_MAJOR) "." NOTE_C_STRINGIZE(NOTE_ARDUINO_VERSION_MINOR) "." NOTE_C_STRINGIZE(NOTE_ARDUINO_VERSION_PATCH)

#if defined(__GNUC__) | defined(__clang__)
    #define NOTE_ARDUINO_DEPRECATED __attribute__((__deprecated__))
#elif defined(_MSC_VER)
    #define NOTE_ARDUINO_DEPRECATED __declspec(deprecated)
#else
    #define NOTE_ARDUINO_DEPRECATED
    #define NOTE_ARDUINO_NO_DEPRECATED_ATTR
#endif // __GNUC__ || __clang__

/**************************************************************************/
/*!
    @brief  Class that stores state and functions for interacting with the
    Blues Notecard.
*/
/**************************************************************************/
class Notecard
{
public:
    ~Notecard(void);
#ifdef ARDUINO
    inline void begin(uint32_t i2cAddress = NOTE_I2C_ADDR_DEFAULT,
                      uint32_t i2cMax = NOTE_I2C_MAX_DEFAULT,
                      TwoWire &wirePort = Wire) {
        begin(make_note_i2c(&wirePort), i2cAddress, i2cMax);
    }
    inline void begin(HardwareSerial &serial, uint32_t speed = 9600) {
        MakeNoteSerial_ArduinoParameters arduino_parameters(serial, speed);
        begin(make_note_serial(&arduino_parameters));
    }
    inline void setDebugOutputStream(Stream &dbgserial) {
        setDebugOutputStream(make_note_log(&dbgserial));
    }
    inline void setTransactionPins(uint8_t ctx_pin, uint8_t rtx_pin) {
        uint8_t txn_pins[2] = {ctx_pin, rtx_pin};
        setTransactionPins(make_note_txn(txn_pins));
    }
#endif
    void begin(NoteI2c * noteI2c,
               uint32_t i2cAddress = NOTE_I2C_ADDR_DEFAULT,
               uint32_t i2cMax = NOTE_I2C_MAX_DEFAULT);
    void begin(NoteSerial * noteSerial);
    inline void clearDebugOutputStream(void) {
        setDebugOutputStream(nullptr);
    }
    inline void clearTransactionPins(void) {
        setTransactionPins(nullptr);
    }
    bool debugSyncStatus (int pollFrequencyMs, int maxLevel);
    void deleteResponse(J *rsp) const;
    void end(void);
    NOTE_ARDUINO_DEPRECATED void logDebug(const char *message) const;
    NOTE_ARDUINO_DEPRECATED void logDebugf(const char *format, ...) const;
    J *newCommand(const char *request) const;
    J *newRequest(const char *request) const;
    J *requestAndResponse(J *req) const;
    J *requestAndResponseWithRetry(J *req, uint32_t timeoutSeconds) const;
    bool responseError(J *rsp) const;
    bool sendRequest(J *req) const;
    bool sendRequestWithRetry(J *req, uint32_t timeoutSeconds) const;
    void setDebugOutputStream(NoteLog * noteLog);
    void setFnI2cMutex(mutexFn lockI2cFn, mutexFn unlockI2cFn);
    void setFnNoteMutex(mutexFn lockNoteFn, mutexFn unlockNoteFn);
    void setTransactionPins(NoteTxn * noteTxn);

private:
    void platformInit (bool assignCallbacks);
};

#endif
