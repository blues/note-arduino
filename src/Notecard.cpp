/*!
 * @file Notecard.cpp
 *
 * @mainpage Arduino Library for the Notecard
 *
 * @section intro_sec Introduction
 *
 * The note-arduino Arduino library for communicating with the
 * <a href="https://blues.io">Blues Wireless</a>
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
 * includes as a git submodule.
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

#include "NoteLog.hpp"
#include "NoteI2c.hpp"
#include "NoteSerial.hpp"

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
}


/***************************************************************************
 PRIVATE FUNCTIONS
 ***************************************************************************/

/* These functions are necessary because some platforms define types
   for delay and millis differently */

extern "C" {

    static uint32_t platform_millis(void);
    static uint32_t platform_millis(void)
    {
        return (uint32_t) millis();
    }

    static void platform_delay(uint32_t ms);
    static void platform_delay(uint32_t ms)
    {
        delay((unsigned long int) ms);
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
    noteSerial = make_note_serial(nullptr, 0);
}

/**************************************************************************/
/*!
    @brief  Initialize the Notecard for I2C.
            This function configures the Notecard to use the I2C bus
            for communication with the host.
    @param    i2caddress
              The I2C Address to use for the Notecard.
    @param    i2cmax
              The max length of each message to send from the host to
              the Notecard. Used to ensure the messages are sized appropriately
              for the host.
    @param    wirePort
              The TwoWire implementation to use for I2C communication.
*/
/**************************************************************************/
void Notecard::begin(uint32_t i2caddress, uint32_t i2cmax, TwoWire &wirePort)
{
    NoteSetUserAgent((char *)"note-arduino");
    NoteSetFnDefault(malloc, free, platform_delay, platform_millis);
    noteI2c = make_note_i2c(&wirePort);

    NoteSetFnI2C(i2caddress, i2cmax, noteI2cReset,
                 noteI2cTransmit, noteI2cReceive);
}

/**************************************************************************/
/*!
    @brief  Initialize the Notecard for Serial communication.
            This function configures the Notecard to use Serial
            for communication with the host.
    @param    selectedSerialPort
              The HardwareSerial bus to use.
    @param    selectedSpeed
              The baud rate to use for communicating with the Notecard
              from the host.
*/
/**************************************************************************/
void Notecard::begin(HardwareSerial &selectedSerialPort, int selectedSpeed)
{
    NoteSetUserAgent((char *)"note-arduino");
    NoteSetFnDefault(malloc, free, platform_delay, platform_millis);
    noteSerial = make_note_serial(&selectedSerialPort, selectedSpeed);

    NoteSetFnSerial(noteSerialReset, noteSerialTransmit,
                    noteSerialAvailable, noteSerialReceive);
}

/**************************************************************************/
/*!
    @brief  Set the debug output source.
            This function takes a Stream object (for example, `Serial`)
            and configures it as a source for writing debug messages
            during development.
    @param    dbgserial
              The Stream object to use for debug output.
*/
/**************************************************************************/
void Notecard::setDebugOutputStream(Stream &dbgserial)
{
    noteLog = make_note_log(&dbgserial);
    NoteSetFnDebugOutput(noteLogPrint);
}

/**************************************************************************/
/*!
    @brief  Clear the debug output source.
*/
/**************************************************************************/
void Notecard::clearDebugOutputStream()
{
    NoteSetFnDebugOutput(nullptr);
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
J *Notecard::newRequest(const char *request)
{
    return NoteNewRequest(request);
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
J *Notecard::newCommand(const char *request)
{
    return NoteNewCommand(request);
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
bool Notecard::sendRequest(J *req)
{
    return NoteRequest(req);
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
J *Notecard::requestAndResponse(J *req)
{
    return NoteRequestResponse(req);
}

/**************************************************************************/
/*!
    @brief  Deletes a `J` JSON response object from memory.
    @param    rsp
              A `J` JSON response object.
*/
/**************************************************************************/
void Notecard::deleteResponse(J *rsp)
{
    NoteDeleteResponse(rsp);
}

/**************************************************************************/
/*!
    @brief  Write a message to the serial debug stream.
    @param    message
              A string to log to the serial debug stream.
*/
/**************************************************************************/
void Notecard::logDebug(const char *message)
{
    NoteDebug(message);
}

/**************************************************************************/
/*!
    @brief  Write a formatted message to the serial debug stream.
    @param    format
              A format string to log to the serial debug stream.
    @param    ... one or more values to interpolate into the format string.
*/
/**************************************************************************/
void Notecard::logDebugf(const char *format, ...)
{
    char message[256];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    NoteDebug(message);
}

/**************************************************************************/
/*!
    @brief  Periodically show Notecard sync status,
      returning TRUE if something was displayed to the debug stream.
    @param    pollFrequencyMs
              The frequency to poll the Notecard for sync status.
  @param    maxLevel
        The maximum log level to output to the debug console. Pass
        -1 for all.
    @return `True` if a pending response was displayed to the debug stream.
*/
/**************************************************************************/
bool Notecard::debugSyncStatus(int pollFrequencyMs, int maxLevel)
{
    return NoteDebugSyncStatus(pollFrequencyMs, maxLevel);
}

/**************************************************************************/
/*!
    @brief  Determines if there is an error string present in a response object.
    @param    rsp
              A `J` JSON Response object.
    @return `True` if the response object contains an error.
*/
/**************************************************************************/
bool Notecard::responseError(J *rsp)
{
    return NoteResponseError(rsp);
}
