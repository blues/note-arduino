/*!
 * @file Notecard.cpp
 *
 * @mainpage Arduino Library for the Notecard
 *
 * @section intro_sec Introduction
 *
 * The note-arduino Arduino library for communicating with the
 * <a href="https://blues.com">Blues</a>
 * Notecard via serial or I2C.
 *
 * This library allows you to control a Notecard by writing an Arduino sketch in
 * C or C++. Your sketch may programmatically configure Notecard and send Notes
 * to <a href="https://notehub.io">Notehub.io</a>.
 *
 * @section dependencies Dependencies
 *
 * This library is a wrapper around and depends upon the
 * <a href="https://github.com/blues/note-c">note-c library</a>, which it
 * includes as a git subtree.
 *
 * In addition, this library requires a physical
 * connection to a Notecard over I2C or Serial to be functional.
 *
 * @section author Author
 *
 * Written by Ray Ozzie and Brandon Satrom for Blues Inc.
 *
 * @section license License
 *
 * Copyright (c) 2019 Blues Inc. MIT License. Use of this source code is
 * governed by licenses granted by the copyright holder including that found in
 * the
 * <a href="https://github.com/blues/note-arduino/blob/master/LICENSE">LICENSE</a>
 * file.
 *
 */

#include "Notecard.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "NoteTime.h"

/***************************************************************************
 SINGLETON ABSTRACTION (REQUIRED BY NOTE-C)
 ***************************************************************************/

namespace
{
NoteI2c *noteI2c(nullptr);

const char *noteI2cReceive(uint16_t device_address_, uint8_t *buffer_, uint16_t size_, uint32_t *available_)
{
    const char *result;
    if (noteI2c) {
        result = noteI2c->receive(device_address_, buffer_, size_, available_);
    } else {
        result = "i2c: A call to Notecard::begin() is required. {io}";
    }
    return result;
}

bool noteI2cReset(uint16_t device_address_)
{
    bool result;
    if (noteI2c) {
        result = noteI2c->reset(device_address_);
    } else {
        result = false;
    }
    return result;
}

const char *noteI2cTransmit(uint16_t device_address_, uint8_t *buffer_, uint16_t size_)
{
    const char *result;
    if (noteI2c) {
        result = noteI2c->transmit(device_address_, buffer_, size_);
    } else {
        result = "i2c: A call to Notecard::begin() is required. {io}";
    }
    return result;
}

NoteLog *noteLog(nullptr);

size_t noteLogPrint(const char * message_)
{
    size_t result;
    if (noteLog) {
        result = noteLog->print(message_);
    } else {
        result = 0;
    }
    return result;
}

NoteSerial *noteSerial(nullptr);

bool noteSerialAvailable(void)
{
    bool result;
    if (noteSerial) {
        result = noteSerial->available();
    } else {
        result = false;
    }
    return result;
}

char noteSerialReceive(void)
{
    char result;
    if (noteSerial) {
        result = noteSerial->receive();
    } else {
        result = '\0';
    }
    return result;
}

bool noteSerialReset(void)
{
    bool result;
    if (noteSerial) {
        result = noteSerial->reset();
    } else {
        result = false;
    }
    return result;
}

void noteSerialTransmit(uint8_t *text_, size_t len_, bool flush_)
{
    if (noteSerial) {
        noteSerial->transmit(text_, len_, flush_);
    }
}

NoteTxn *noteTxn(nullptr);

bool noteTransactionStart (uint32_t timeout_ms_) {
    bool result;
    if (noteTxn) {
        result = noteTxn->start(timeout_ms_);
    } else {
        // NoteTransaction not set, assume unnecessary
        result = true;
    }
    return result;
}

void noteTransactionStop (void) {
    if (noteTxn) {
        noteTxn->stop();
    }
}

}

/**************************************************************************/
/*!
    @brief  Platform Initialization for the Notecard.
            This function configures system functions to be used
            by the Notecard.
    @param    assignCallbacks
              When `true` the system callbacks will be assigned,
              when `false` the system callbacks will be cleared.
    @param    i2cmax
              The max length of each message to send from the host to
              the Notecard. Used to ensure the messages are sized appropriately
              for the host.
    @param    wirePort
              The TwoWire implementation to use for I2C communication.
*/
/**************************************************************************/
void Notecard::platformInit (bool assignCallbacks)
{
    NoteSetUserAgent((char *) ("note-arduino " NOTE_ARDUINO_VERSION));
    if (assignCallbacks) {
        NoteSetFnDefault(malloc, free, noteDelay, noteMillis);
    } else {
        NoteSetFnDefault(nullptr, nullptr, nullptr, nullptr);
    }
}

/***************************************************************************
 PUBLIC FUNCTIONS
 ***************************************************************************/

Notecard::~Notecard (void)
{
    // Delete Singleton(s)
    noteI2c = make_note_i2c(nullptr);
    noteLog = make_note_log(nullptr);
    noteSerial = make_note_serial(nullptr);
    noteTxn = make_note_txn(nullptr);
}

/**************************************************************************/
/*!
    @brief  Initialize the Notecard for I2C.
            This function configures the Notecard to use the I2C
            bus for communication with the host.
    @param    noteI2c
              A platform specific I2C implementation to use for
              communicating with the Notecard from the host.
    @param    i2cAddress
              The I2C Address to use for the Notecard.
    @param    i2cMax
              The max length of each message to send from the host
              to the Notecard. Used to ensure the messages are sized
              appropriately for the host.
*/
/**************************************************************************/
void Notecard::begin(NoteI2c * noteI2c_, uint32_t i2cAddress_, uint32_t i2cMax_)
{
    noteI2c = noteI2c_;
    platformInit(noteI2c);
    if (noteI2c) {
        NoteSetFnI2C(i2cAddress_, i2cMax_, noteI2cReset,
                    noteI2cTransmit, noteI2cReceive);
    } else {
        NoteSetFnI2C(0, 0, nullptr, nullptr, nullptr);
    }
}

/**************************************************************************/
/*!
    @brief  Initialize the Notecard for Serial communication.
            This function configures the Notecard to use Serial
            for communication with the host.
    @param    noteSerial
              A platform specific serial implementation to use for
              communicating with the Notecard from the host.
*/
/**************************************************************************/
void Notecard::begin(NoteSerial * noteSerial_)
{
    noteSerial = noteSerial_;
    platformInit(noteSerial);
    if (noteSerial) {
        NoteSetFnSerial(noteSerialReset, noteSerialTransmit,
                        noteSerialAvailable, noteSerialReceive);
    } else {
        NoteSetFnSerial(nullptr, nullptr, nullptr, nullptr);
    }
}

/**************************************************************************/
/*!
    @brief  Periodically show Notecard sync status, returning `TRUE`
            if something was displayed to the debug stream.
    @param    pollFrequencyMs
              The frequency to poll the Notecard for sync status.
    @param    maxLevel
              The maximum log level to output to the debug console. Pass
              -1 for all.
    @return `true` if a pending response was displayed to the debug stream.
*/
/**************************************************************************/
bool Notecard::debugSyncStatus(int pollFrequencyMs, int maxLevel)
{
    return NoteDebugSyncStatus(pollFrequencyMs, maxLevel);
}

/**************************************************************************/
/*!
    @brief  Deletes a `J` JSON response object from memory.
    @param    rsp
              A `J` JSON response object.
*/
/**************************************************************************/
void Notecard::deleteResponse(J *rsp) const
{
    NoteDeleteResponse(rsp);
}

/**************************************************************************/
/*!
    @brief  Deinitialize the Notecard object communication.
            This function clears the Notecard object's communication
            interfaces, and frees all associated memory.
*/
/**************************************************************************/
void Notecard::end(void)
{
    // Clear I2C Interface
    i2cTransmitFn i2c_is_set = nullptr;
    NoteGetFnI2C(nullptr, nullptr, nullptr, &i2c_is_set, nullptr);
    if (i2c_is_set) {
        // Delete Singletons
        noteI2c = make_note_i2c(nullptr);
        NoteSetFnI2C(0, 0, nullptr, nullptr, nullptr);
    }

    // Clear Serial Interface
    serialTransmitFn serial_is_set = nullptr;
    NoteGetFnSerial(nullptr, &serial_is_set, nullptr, nullptr);
    if (serial_is_set) {
        // Delete Singletons
        noteSerial = make_note_serial(nullptr);
        NoteSetFnSerial(nullptr, nullptr, nullptr, nullptr);
    }

    // Clear Platform Callbacks
    platformInit(false);
}

/**************************************************************************/
/*!
    @deprecated NoteDebug, which this function wraps, should be treated as an
    internal Notecard logging function, used only by the library itself
    (note-arduino and note-c) and not its users.
    @brief  Write a message to the serial debug stream.
    @param    message
              A string to log to the serial debug stream.
*/
/**************************************************************************/
NOTE_ARDUINO_DEPRECATED void Notecard::logDebug(const char *message) const
{
#ifdef NOTE_ARDUINO_NO_DEPRECATED_ATTR
    NOTE_C_LOG_WARN("logDebug is deprecated.")
#endif
    NoteDebug(message);
}

/**************************************************************************/
/*!
    @deprecated NoteDebug, which this function wraps, should be treated as an
    internal Notecard logging function, used only by the library itself
    (note-arduino and note-c) and not its users.
    @brief  Write a formatted message to the serial debug stream.
    @param    format
              A format string to log to the serial debug stream.
    @param    ... one or more values to interpolate into the format string.
*/
/**************************************************************************/
NOTE_ARDUINO_DEPRECATED void Notecard::logDebugf(const char *format, ...) const
{
    char message[256];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

#ifdef NOTE_ARDUINO_NO_DEPRECATED_ATTR
    NOTE_C_LOG_WARN("logDebugf is deprecated.")
#endif
    NoteDebug(message);
}

/**************************************************************************/
/*!
    @brief  Creates a new command object for population by the host.
            This function accepts a command string (for example, `"note.add"`)
            and initializes a JSON Object to return to the host.
    @param    request
              The command name, for example, `note.add`.
    @return A `J` JSON Object populated with the request name.
*/
/**************************************************************************/
J *Notecard::newCommand(const char *request) const
{
    return NoteNewCommand(request);
}

/**************************************************************************/
/*!
    @brief  Creates a new request object for population by the host.
            This function accepts a request string (for example, `"note.add"`)
            and initializes a JSON Object to return to the host.
    @param    request
              The request name, for example, `note.add`.
    @return A `J` JSON Object populated with the request name.
*/
/**************************************************************************/
J *Notecard::newRequest(const char *request) const
{
    return NoteNewRequest(request);
}

/**************************************************************************/
/*!
    @brief  Sends a request to the Notecard and returns the JSON Response.
            This function takes a populated `J` JSON request object
            and sends it to the Notecard.
    @param    req
              A `J` JSON request object.
    @return `J` JSON Object with the response from the Notecard.
*/
/**************************************************************************/
J *Notecard::requestAndResponse(J *req) const
{
    return NoteRequestResponse(req);
}

/**************************************************************************/
/*!
    @brief  Sends a request to the Notecard, retrying it on failure until the
            provided timeout interval lapses, and returns the JSON response.
    @param    req
              A `J` JSON request object.
    @param    timeoutSeconds
              The timeout interval, in seconds.
    @return `J` JSON Object with the response from the Notecard.
*/
/**************************************************************************/
J *Notecard::requestAndResponseWithRetry(J *req, uint32_t timeoutSeconds) const
{
    return NoteRequestResponseWithRetry(req, timeoutSeconds);
}

/**************************************************************************/
/*!
    @brief  Determines if there is an error string present in a response object.
    @param    rsp
              A `J` JSON Response object.
    @return `true` if the response object contains an error.
*/
/**************************************************************************/
bool Notecard::responseError(J *rsp) const
{
    return NoteResponseError(rsp);
}

/**************************************************************************/
/*!
    @brief  Sends a request to the Notecard.
            This function takes a populated `J` JSON request object
            and sends it to the Notecard.
    @param    req
              A `J` JSON request object.
    @return `True` if the message was successfully sent to the Notecard,
            `False` if there was an error.
*/
/**************************************************************************/
bool Notecard::sendRequest(J *req) const
{
    return NoteRequest(req);
}

/**************************************************************************/
/*!
    @brief  Sends a request to the Notecard, retrying it on failure until the
            provided timeout interval lapses.
    @param    req
              A `J` JSON request object.
    @param    timeoutSeconds
              The timeout interval, in seconds.
    @return `True` if the message was successfully sent to the Notecard,
            `False` if the message couldn't be sent.
*/
/**************************************************************************/
bool Notecard::sendRequestWithRetry(J *req, uint32_t timeoutSeconds) const
{
    return NoteRequestWithRetry(req, timeoutSeconds);
}

/**************************************************************************/
/*!
    @brief  Set the debug output source.
            A NoteLog object will be constructed via `make_note_log()`
            using a platform specific logging channel (for example, `Serial`
            on Arduino). The specified channel will be configured as the
            source for debug messages provided to `notecard.logDebug()`.
    @param    noteLog
              A platform specific log implementation to be used for
              debug output.
*/
/**************************************************************************/
void Notecard::setDebugOutputStream(NoteLog * noteLog_)
{
    noteLog = noteLog_;
    if (noteLog) {
        NoteSetFnDebugOutput(noteLogPrint);
    } else {
        make_note_log(nullptr);  // Clear singleton
        NoteSetFnDebugOutput(nullptr);
    }
}

/**************************************************************************/
/*!
    @brief  Override default memory management and timing functions.
    @param    mallocHook
              A memory allocation hook.
    @param    freeHook
              A memory deallocation hook.
    @param    delayMsHook
              A delay execution hook.
    @param    getMsHook
              A get current time hook.
*/
/**************************************************************************/
void Notecard::setFn(mallocFn mallocHook, freeFn freeHook, delayMsFn delayMsHook, getMsFn getMsHook) {
    NoteSetFn(mallocHook, freeHook, delayMsHook, getMsHook);
}

/**************************************************************************/
/*!
    @brief  Set the lock/unlock functions the Notecard uses for I2C access.
    @param    lockI2cFn
              A user-defined callback that blocks until access to the I2C
              bus has become available, then returns with ownership of the
              I2C bus.
    @param    unlockI2cFn
              A user-defined callback that releases ownership of the
              I2C bus taken during the call to `lockI2cFn()`.
*/
/**************************************************************************/
void Notecard::setFnI2cMutex(mutexFn lockI2cFn_, mutexFn unlockI2cFn_) {
    NoteSetFnI2CMutex(lockI2cFn_, unlockI2cFn_);
}

/**************************************************************************/
/*!
    @brief  Set the lock/unlock functions the host MCU uses to ensure
            a complete transaction with the Notecard.
    @param    lockNoteFn
              A user-defined callback that blocks until the Notecard has
              completed any previous transactions, then returns with
              ownership of the next Notecard transaction.
    @param    unlockNoteFn
              A user-defined callback that releases ownership of the
              Notecard transaction taken during the call to `lockNoteFn()`.
*/
/**************************************************************************/
void Notecard::setFnNoteMutex(mutexFn lockNoteFn_, mutexFn unlockNoteFn_) {
    NoteSetFnNoteMutex(lockNoteFn_, unlockNoteFn_);
}

/**************************************************************************/
/*!
    @brief  Set the transaction pins.
            A NoteTxn object will be constructed via `make_note_txn()`
            using a platform specific tuple of digital I/O pins. The
            pins are used to send a request to transact and to listen
            for the clear to transact signal. Transaction pins are not
            necessary on any legacy Notecards, and are only necessary
            for certain Notecard SKUs. The pins allow the Notecard to
            inform the host it has had time to awaken from deep sleep
            and is ready to process commands.
    @param    noteTxn
              A platform specific tuple of digital I/O pins.
*/
/**************************************************************************/
void Notecard::setTransactionPins(NoteTxn * noteTxn_) {
    noteTxn = noteTxn_;  // Set global interface
    if (noteTxn_) {
        NoteSetFnTransaction(noteTransactionStart, noteTransactionStop);
    } else {
        make_note_txn(nullptr);  // Clear singleton
        NoteSetFnTransaction(nullptr, nullptr);
    }
}
