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

    This function configures the user agent information and optionally
    the default system functions to be used by the Notecard.

    @param    assignCallbacks
              When `true` the system callbacks will be assigned,
              when `false` the system callbacks will be cleared.
*/
/**************************************************************************/
void Notecard::platformInit (bool assignCallbacks)
{
    NoteSetUserAgent((char *) ("note-arduino " NOTE_ARDUINO_VERSION));
    if (assignCallbacks) {
        NoteSetFnDefault(malloc, free, noteDelay, noteMillis);
    } else {
        NoteSetFn(nullptr, nullptr, nullptr, nullptr);  // Force clear
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

void Notecard::begin(NoteI2c * noteI2c_, uint32_t i2cAddress_, uint32_t i2cMtu_)
{
    noteI2c = noteI2c_;
    platformInit(noteI2c);
    if (noteI2c) {
        NoteSetI2CAddress(i2cAddress_); // Force set user supplied address
        NoteSetI2CMtu(i2cMtu_);         // Force set user supplied MTU
        NoteSetFnI2CDefault(i2cAddress_, i2cMtu_, noteI2cReset,
                            noteI2cTransmit, noteI2cReceive);
    } else {
        NoteSetFnI2C(0, 0, nullptr, nullptr, nullptr); // Force clear
    }
}

void Notecard::begin(NoteSerial * noteSerial_)
{
    noteSerial = noteSerial_;
    platformInit(noteSerial);
    if (noteSerial) {
        NoteSetFnSerialDefault(noteSerialReset, noteSerialTransmit,
                               noteSerialAvailable, noteSerialReceive);
    } else {
        NoteSetFnSerial(nullptr, nullptr, nullptr, nullptr); // Force clear
    }
}

bool Notecard::debugSyncStatus(int pollFrequencyMs, int maxLevel)
{
    return NoteDebugSyncStatus(pollFrequencyMs, maxLevel);
}

void Notecard::deleteResponse(J *rsp) const
{
    NoteDeleteResponse(rsp);
}

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

NOTE_ARDUINO_DEPRECATED void Notecard::logDebug(const char *message) const
{
#ifdef NOTE_ARDUINO_NO_DEPRECATED_ATTR
    NOTE_C_LOG_WARN("logDebug is deprecated.")
#endif
    NoteDebug(message);
}

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

J *Notecard::newCommand(const char *request) const
{
    return NoteNewCommand(request);
}

J *Notecard::newRequest(const char *request) const
{
    return NoteNewRequest(request);
}

J *Notecard::requestAndResponse(J *req) const
{
    return NoteRequestResponse(req);
}

J *Notecard::requestAndResponseWithRetry(J *req, uint32_t timeoutSeconds) const
{
    return NoteRequestResponseWithRetry(req, timeoutSeconds);
}

bool Notecard::responseError(J *rsp) const
{
    return NoteResponseError(rsp);
}

bool Notecard::sendRequest(J *req) const
{
    return NoteRequest(req);
}

bool Notecard::sendRequestWithRetry(J *req, uint32_t timeoutSeconds) const
{
    return NoteRequestWithRetry(req, timeoutSeconds);
}

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

void Notecard::setFn(mallocFn mallocHook, freeFn freeHook, delayMsFn delayMsHook, getMsFn getMsHook) {
    NoteSetFn(mallocHook, freeHook, delayMsHook, getMsHook);
}

void Notecard::setFnI2cMutex(mutexFn lockI2cFn_, mutexFn unlockI2cFn_) {
    NoteSetFnI2CMutex(lockI2cFn_, unlockI2cFn_);
}

void Notecard::setFnNoteMutex(mutexFn lockNoteFn_, mutexFn unlockNoteFn_) {
    NoteSetFnNoteMutex(lockNoteFn_, unlockNoteFn_);
}

void Notecard::setTransactionPins(NoteTxn * noteTxn_) {
    noteTxn = noteTxn_;  // Set global interface
    if (noteTxn_) {
        NoteSetFnTransaction(noteTransactionStart, noteTransactionStop);
    } else {
        make_note_txn(nullptr);  // Clear singleton
        NoteSetFnTransaction(nullptr, nullptr);
    }
}
