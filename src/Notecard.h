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

#include "NoteDefines.h"
#include "NoteI2c.hpp"
#include "NoteLog.hpp"
#include "NoteSerial.hpp"
#include "NoteTxn.hpp"

#ifdef ARDUINO
#include <Arduino.h>
#ifdef NOTE_ARDUINO_SOFTWARE_SERIAL_SUPPORT
#include <SoftwareSerial.h>
#endif
#include <Wire.h>
#include "NoteSerial_Arduino.hpp"
#endif

#ifndef NOTE_MOCK
#include <note-c/note.h>
#else
#include "mock/mock-arduino.hpp"
#include "mock/mock-parameters.hpp"
#endif

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
    /**************************************************************************/
    /*!
        @brief  Initialize the Notecard for I2C.

        This function configures the Notecard to use the I2C
        bus for communication with the host.

        @param [in] i2cAddress
                The I2C Address to use for the Notecard.
        @param [in] i2cMtu
                The max length of each message to send from the host
                to the Notecard. Used to ensure the messages are sized
                appropriately for the host.
        @param [in] wirePort
                The TwoWire port to use for I2C communication.
    */
    /**************************************************************************/
    inline void begin(uint32_t i2cAddress = NOTE_I2C_ADDR_DEFAULT,
                      uint32_t i2cMtu = NOTE_I2C_MTU_DEFAULT,
                      TwoWire &wirePort = Wire) {
        begin(make_note_i2c(wirePort), i2cAddress, i2cMtu);
    }

    /**************************************************************************/
    /*!
        @brief  Initialize the Notecard for Serial communication.

        This function configures the Notecard to use hardware Serial
        for communication with the host.

        @param [in] serial
                The HardwareSerial port to use for Serial communication.
        @param [in] speed
                The baud rate to use for Serial communication.
    */
    /**************************************************************************/
    inline void begin(HardwareSerial &serial, uint32_t speed = 9600) {
        MakeNoteSerial_ArduinoParameters<HardwareSerial> arduino_parameters(serial, speed);
        begin(make_note_serial<MakeNoteSerial_ArduinoParameters<HardwareSerial>>(arduino_parameters));
    }

#ifdef NOTE_ARDUINO_SOFTWARE_SERIAL_SUPPORT
    /**************************************************************************/
    /*!
        @brief  Initialize the Notecard for Serial communication.

        This function configures the Notecard to use software defined Serial
        for communication with the host.

        @param [in] serial
                The SoftwareSerial port to use for Serial communication.
        @param [in] speed
                The baud rate to use for Serial communication.
    */
    /**************************************************************************/
    inline void begin(SoftwareSerial &serial, uint32_t speed = 9600) {
        MakeNoteSerial_ArduinoParameters<SoftwareSerial> arduino_parameters(serial, speed);
        begin(make_note_serial<MakeNoteSerial_ArduinoParameters<SoftwareSerial>>(arduino_parameters));
    }
#endif

    /**************************************************************************/
    /*!
        @brief  Set the debug output source

        The specified channel will be configured as the source for debug
        messages provided to `notecard.logDebug()`.

        @param [in] dbgserial
                A Stream object to be used for debug output.
    */
    /**************************************************************************/
    inline void setDebugOutputStream(Stream &dbgserial) {
        setDebugOutputStream(make_note_log(dbgserial));
    }

    /**************************************************************************/
    /*!
        @brief  Set the transaction pins.

        The pins are used to send a request to transact and to listen for the
        clear to transact signal. The pins allow the Notecard to inform the
        host it has had time to awaken from deep sleep and is ready to process
        commands.

        @param [in] ctx_pin
                The pin used to listen for the clear to transact signal.
        @param [in] rtx_pin
                The pin used to signal a request to transact.

        @note Transaction pins are not necessary on any legacy Notecards, and
              are only necessary for certain Notecard SKUs.
    */
    /**************************************************************************/
    inline void setTransactionPins(uint8_t ctx_pin, uint8_t rtx_pin) {
        uint8_t txn_pins[2] = {ctx_pin, rtx_pin};
        setTransactionPins(make_note_txn(txn_pins));
    }
#endif
    /**************************************************************************/
    /*!
        @brief  Initialize the Notecard for I2C.

        This function configures the Notecard to use the I2C
        bus for communication with the host.

        @param [in] noteI2c
                A platform specific I2C implementation to use for
                communicating with the Notecard from the host.
        @param [in] i2cAddress
                The I2C Address to use for the Notecard.
        @param [in] i2cMtu
                The max length of each message to send from the host
                to the Notecard. Used to ensure the messages are sized
                appropriately for the host.
    */
    /**************************************************************************/
    void begin(NoteI2c * noteI2c,
               uint32_t i2cAddress = NOTE_I2C_ADDR_DEFAULT,
               uint32_t i2cMtu = NOTE_I2C_MTU_DEFAULT);

    /**************************************************************************/
    /*!
        @brief  Initialize the Notecard for Serial communication.

        This function configures the Notecard to use Serial
        for communication with the host.

        @param [in] noteSerial
                A platform specific serial implementation to use for
                communicating with the Notecard from the host.
    */
    /**************************************************************************/
    void begin(NoteSerial * noteSerial);

    /**************************************************************************/
    /*!
        @brief  Clear the debug output source

        Detach the previously specified debug channel.
    */
    /**************************************************************************/
    inline void clearDebugOutputStream(void) {
        setDebugOutputStream(nullptr);
    }

    /**************************************************************************/
    /*!
        @brief  Clear the transaction pins.

        Detach the previously specified transaction pins.
    */
    /**************************************************************************/
    inline void clearTransactionPins(void) {
        setTransactionPins(nullptr);
    }

    /**************************************************************************/
    /*!
        @brief Poll for Notecard sync status.

        @param [in] pollFrequencyMs
                The minimum frequency to poll the Notecard for sync status.
        @param [in] maxLevel
                The maximum log level to output to the debug console. Pass
                -1 for all.

        @return `true` if a pending response was displayed to the debug stream.
    */
    /**************************************************************************/
    bool debugSyncStatus (int pollFrequencyMs, int maxLevel);

    /**************************************************************************/
    /*!
        @brief  Frees a `J` JSON response.

        A `J` JSON response object, along with all nested objects, shall
        be freed from memory.

        @param [in] rsp
                A `J` JSON response object.
    */
    /**************************************************************************/
    void deleteResponse(J *rsp) const;

    /**************************************************************************/
    /*!
        @brief  Deinitialize the Notecard object

        This function clears the Notecard object's communication
        interfaces, and frees all associated memory.
    */
    /**************************************************************************/
    void end(void);

    /**************************************************************************/
    /*!
        @deprecated NoteDebug, which this function wraps, should be treated
        as an internal Notecard logging function, used only by the library
        itself (note-arduino and note-c) and not its users.

        @brief  Write a message to the serial debug stream.

        @param [in] message
                A string to log to the serial debug stream.
    */
    /**************************************************************************/
    NOTE_ARDUINO_DEPRECATED void logDebug(const char *message) const;

    /**************************************************************************/
    /*!
        @deprecated NoteDebug, which this function wraps, should be treated
        as an internal Notecard logging function, used only by the library
        itself (note-arduino and note-c) and not its users.

        @brief  Write a formatted message to the serial debug stream.

        @param [in] format
                A format string to log to the serial debug stream.
        @param [in] ...
                One or more values to interpolate into the format string.
    */
    /**************************************************************************/
    NOTE_ARDUINO_DEPRECATED void logDebugf(const char *format, ...) const;

    /**************************************************************************/
    /*!
        @brief  Creates a new command object for population by the host.

        This function accepts a command string and initializes a JSON object
        to return to the host. A command is a JSON request that DOES NOT
        expect a response from the Notecard.

        @param [in] request
                The command name (e.g. `card.attn`).

        @return A `J` JSON object populated with the command name.
    */
    /**************************************************************************/
    J *newCommand(const char *request) const;

    /**************************************************************************/
    /*!
        @brief  Creates a new request object for population by the host.

        This function accepts a request string and initializes a JSON object
        to return to the host. A request is a JSON object that DOES expect
        a response from the Notecard.

        @param [in] request
                The request name (e.g. `note.add`).

        @return A `J` JSON object populated with the request name.
    */
    /**************************************************************************/
    J *newRequest(const char *request) const;

    /**************************************************************************/
    /*!
        @brief  Sends a request to the Notecard and returns the JSON response.

        This function takes a populated `J` JSON request object
        and sends it to the Notecard.

        @param [in] req
                A `J` JSON request object.

        @return `J` JSON object with the response from the Notecard.

        @note The request object is deleted by this function; do not use it
              after calling this function.
    */
    /**************************************************************************/
    J *requestAndResponse(J *req) const;

    /**************************************************************************/
    /*!
        @brief  Sends a request to the Notecard and returns the JSON response;
                retrying on failure until the provided timeout interval lapses.

        @param [in] req
                A `J` JSON request object.
        @param [in] timeoutSeconds
                The timeout interval, in seconds.

        @return `J` JSON object with the response from the Notecard.

        @note The request object is deleted by this function; do not use it
              after calling this function.
    */
    /**************************************************************************/
    J *requestAndResponseWithRetry(J *req, uint32_t timeoutSeconds) const;

    /**************************************************************************/
    /*!
        @brief  Checks a response object for the presence of an error string.

        @param [in] rsp
                A `J` JSON Response object.

        @return `true` if the response object contains an error.
    */
    /**************************************************************************/
    bool responseError(J *rsp) const;

    /**************************************************************************/
    /*!
        @brief  Sends a request to the Notecard.

        This function takes a populated `J` JSON request object and sends
        it to the Notecard.

        @param [in] req
                A `J` JSON request object.

        @return `true` if the message was successfully sent to the Notecard,
                otherwise `false` if there was an error.

        @note The request object is deleted by this function; do not use it
              after calling this function.
    */
    /**************************************************************************/
    bool sendRequest(J *req) const;

    /**************************************************************************/
    /*!
        @brief  Sends a request to the Notecard; retrying on failure until
        the provided timeout interval lapses.

        @param [in] req
                A `J` JSON request object.
        @param [in] timeoutSeconds
                The timeout interval, in seconds.

        @return `true` if the message was successfully sent to the Notecard,
                otherwise `false` if there was an error.

        @note The request object is deleted by this function; do not use it
              after calling this function.
    */
    /**************************************************************************/
    bool sendRequestWithRetry(J *req, uint32_t timeoutSeconds) const;

    /**************************************************************************/
    /*!
        @brief  Set the debug output source

        The specified channel will be configured as the source for debug
        messages provided to `notecard.logDebug()`.

        @param [in] noteLog
                A platform specific log implementation to be used for
                debug output.
    */
    /**************************************************************************/
    void setDebugOutputStream(NoteLog * noteLog);

    /**************************************************************************/
    /*!
        @brief  Override default memory management and timing functions.

        @param [in] mallocHook
                A memory allocation hook.
        @param [in] freeHook
                A memory deallocation hook.
        @param [in] delayMsHook
                A delay execution hook.
        @param [in] getMsHook
                A get current time in milliseconds hook.
    */
    /**************************************************************************/
    void setFn(mallocFn mallocHook, freeFn freeHook, delayMsFn delayMsHook, getMsFn getMsHook);

    /**************************************************************************/
    /*!
        @brief  Set the lock/unlock functions the Notecard uses for I2C access.

        The mutex acquisition and release functions must share the same mutex
        as the all other I2C operations in the host application. They will be
        used to minimize bus contention and ensure exclusive access to the I2C
        bus during Notecard transactions.

        @param [in] lockI2cFn
                A user-defined callback that blocks until access to the I2C
                bus has become available, then returns with ownership of the
                I2C bus.
        @param [in] unlockI2cFn
                A user-defined callback that releases ownership of the
                I2C bus taken during the call to `lockI2cFn()`.
    */
    /**************************************************************************/
    void setFnI2cMutex(mutexFn lockI2cFn, mutexFn unlockI2cFn);

    /**************************************************************************/
    /*!
        @brief  Set the lock/unlock functions the host MCU uses to ensure each
                task has exclusive access while transacting with the Notecard.

        @param [in] lockNoteFn
                A user-defined callback that blocks until the Notecard has
                completed any previous transactions, then returns with
                ownership of the next Notecard transaction.
        @param [in] unlockNoteFn
                A user-defined callback that releases ownership of the
                Notecard transaction taken during the call to `lockNoteFn()`.
    */
    /**************************************************************************/
    void setFnNoteMutex(mutexFn lockNoteFn, mutexFn unlockNoteFn);

    /**************************************************************************/
    /*!
        @brief  Set the transaction pins.

        The pins are used to send a request to transact and to listen for the
        clear to transact signal. The pins allow the Notecard to inform the
        host it has had time to awaken from deep sleep and is ready to process
        commands.

        @param [in] noteTxn
                A platform specific tuple of digital I/O pins.

        @note Transaction pins are not necessary on any legacy Notecards, and
              are only necessary for certain Notecard SKUs.
    */
    /**************************************************************************/
    void setTransactionPins(NoteTxn * noteTxn);

private:
    void platformInit (bool assignCallbacks);
};

#endif
