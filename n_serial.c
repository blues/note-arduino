/*!
 * @file n_serial.c
 *
 * Written by Ray Ozzie and Blues Inc. team.
 *
 * Copyright (c) 2019 Blues Inc. MIT License. Use of this source code is
 * governed by licenses granted by the copyright holder including that found in
 * the
 * <a href="https://github.com/blues/note-c/blob/master/LICENSE">LICENSE</a>
 * file.
 *
 */

#include <stdlib.h>

#include "n_lib.h"

/**************************************************************************/
/*!
  @brief  Given a JSON string, perform a serial transaction with the Notecard.

  @param   request A string containing the JSON request object, which MUST BE
            terminated with a newline character.
  @param   reqLen the string length of the JSON request.
  @param   response [out] A c-string buffer that will contain the newline ('\n')
            terminated JSON response from the Notercard. If NULL, no response
            will be captured.
  @param   timeoutMs The maximum amount of time, in milliseconds, to wait
            for data to arrive. Passing zero (0) disables the timeout.

  @returns a c-string with an error, or `NULL` if no error occurred.
*/
/**************************************************************************/
const char *_serialNoteTransaction(const char *request, size_t reqLen, char **response, uint32_t timeoutMs)
{
    // Strip off the newline and optional carriage return characters. This
    // allows for standardized output to be reapplied.
    reqLen--;  // remove newline
    if (request[reqLen - 1] == '\r') {
        reqLen--; // remove carriage return if it exists
    }

    const char *err = _serialChunkedTransmit((uint8_t *)request, reqLen, true);
    if (err) {
        NOTE_C_LOG_ERROR(err);
        return err;
    }

    // Append the carriage return and newline to the transaction.
    _SerialTransmit((uint8_t *)c_newline, c_newline_len, true);

    // If no reply expected, we're done
    if (response == NULL) {
        return NULL;
    }

    // Wait for something to become available, processing timeout errors
    // up-front because the json parse operation immediately following is
    // subject to the serial port timeout. We'd like more flexibility in
    // max timeout and ultimately in our error handling.
    for (const uint32_t startMs = _GetMs(); !_SerialAvailable(); ) {
        if (timeoutMs && (_GetMs() - startMs) >= timeoutMs) {
            NOTE_C_LOG_DEBUG(ERRSTR("reply to request didn't arrive from module in time", c_iotimeout));
            return ERRSTR("transaction timeout {io}", c_iotimeout);
        }
        if (!cardTurboIO) {
            _DelayMs(10);
        }
    }

    // Allocate a buffer for input, noting that we always put the +1 in the
    // alloc so we can be assured that it can be null-terminated. This must be
    // the case because json parsing requires a null-terminated string.
    uint32_t available = 0;
    uint32_t jsonbufAllocLen = ALLOC_CHUNK;
    uint8_t *jsonbuf = (uint8_t *)_Malloc(jsonbufAllocLen + 1);
    if (jsonbuf == NULL) {
        const char *err = ERRSTR("transaction: jsonbuf malloc failed", c_mem);
        NOTE_C_LOG_ERROR(err);
        return err;
    }

    // Receive the Notecard response
    uint32_t jsonbufLen = 0;
    do {
        uint32_t jsonbufAvailLen = (jsonbufAllocLen - jsonbufLen);

        // Append into the json buffer
        const char *err = _serialChunkedReceive((uint8_t *)(jsonbuf + jsonbufLen), &jsonbufAvailLen, true, (CARD_INTRA_TRANSACTION_TIMEOUT_SEC * 1000), &available);
        if (err) {
            _Free(jsonbuf);
            NOTE_C_LOG_ERROR(ERRSTR("error occured during receive", c_iobad));
            return err;
        }
        jsonbufLen += jsonbufAvailLen;

        if (available) {
            // When more bytes are available than we have buffer to accommodate
            // (i.e. overflow), then we allocate blocks of size `ALLOC_CHUNK` to
            // reduce heap fragmentation.
            // NOTE: We always put the +1 in the allocation so we can be assured
            // that it can be null-terminated, because the json parser requires
            // a null-terminated string.
            jsonbufAllocLen += (ALLOC_CHUNK * ((available / ALLOC_CHUNK) + ((available % ALLOC_CHUNK) > 0)));
            uint8_t *jsonbufNew = (uint8_t *)_Malloc(jsonbufAllocLen + 1);
            if (jsonbufNew == NULL) {
                const char *err = ERRSTR("transaction: jsonbuf grow malloc failed", c_mem);
                NOTE_C_LOG_ERROR(err);
                _Free(jsonbuf);
                return err;
            }
            memcpy(jsonbufNew, jsonbuf, jsonbufLen);
            _Free(jsonbuf);
            jsonbuf = jsonbufNew;
            NOTE_C_LOG_DEBUG("additional receive buffer chunk allocated");
        }
    } while (available);

    // Null-terminate it, using the +1 space that we'd allocated in the buffer
    jsonbuf[jsonbufLen] = '\0';

    // Return it
    *response = (char *)jsonbuf;
    return NULL;
}

//**************************************************************************/
/*!
    @brief  Initialize or re-initialize the Serial bus, returning false if
            anything fails.

    @returns a boolean. `true` if the reset was successful, `false`, if not.
*/
/**************************************************************************/
bool _serialNoteReset(void)
{
    NOTE_C_LOG_DEBUG("resetting Serial interface...");

    // Reset the Serial subsystem and exit if failure
    _DelayMs(CARD_REQUEST_SERIAL_SEGMENT_DELAY_MS);
    if (!_SerialReset()) {
        NOTE_C_LOG_ERROR(ERRSTR("unable to reset Serial interface.", c_err));
        return false;
    }

    // The guaranteed behavior for robust resyncing is to send two newlines
    // and  wait for two echoed blank lines in return.
    bool notecardReady = false;
    for (size_t retries = 0; retries < CARD_RESET_SYNC_RETRIES ; ++retries) {

        // Send a newline to the module to clean out request/response processing
        // NOTE: This MUST always be `\n` and not `\r\n`, because there are some
        //       versions of the Notecard firmware will not respond to `\r\n`
        //       after communicating over I2C.
        _SerialTransmit((uint8_t *)"\n", 1, true);

        // Drain all communications for 500ms
        bool somethingFound = false;
        bool nonControlCharFound = false;

        // Read Serial data for at least CARD_RESET_DRAIN_MS continously
        for (uint32_t startMs = _GetMs() ; (_GetMs() - startMs) < CARD_RESET_DRAIN_MS ;) {
            // Determine if Serial data is available
            while (_SerialAvailable()) {
                somethingFound = true;
                // The Notecard responds to a bare `\n` with `\r\n`. If we get
                // any other characters back, it means the host and Notecard
                // aren't synced up yet and we need to transmit `\n` again.
                char ch = _SerialReceive();
                if (ch != '\n' && ch != '\r') {
                    nonControlCharFound = true;
                    // Reset the timer with each non-control character
                    startMs = _GetMs();
                }
            }
            _DelayMs(1);
        }

        // If all we got back is newlines, we're ready
        if (somethingFound && !nonControlCharFound) {
            notecardReady = true;
            break;
        }

        NOTE_C_LOG_ERROR(somethingFound ? ERRSTR("unrecognized data from notecard", c_iobad) : ERRSTR("notecard not responding", c_iobad));

        _DelayMs(CARD_RESET_DRAIN_MS);
        if (!_SerialReset()) {
            NOTE_C_LOG_ERROR(ERRSTR("unable to reset Serial interface.", c_err));
            return false;
        }

        NOTE_C_LOG_DEBUG("retrying Serial interface reset.");
    }

    // Done
    return notecardReady;
}

/**************************************************************************/
/*!
  @brief  Receive bytes over Serial from the Notecard.

  @param   buffer A buffer to receive bytes into.
  @param   size (in/out)
            - (in) The size of the buffer in bytes.
            - (out) The length of the received data in bytes.
  @param   delay Respect standard processing delays.
  @param   timeoutMs The maximum amount of time, in milliseconds, to wait for
            serial data to arrive. Passing zero (0) disables the timeout.
  @param   available (out) The amount of bytes unable to fit into the provided buffer.

  @returns  A c-string with an error, or `NULL` if no error ocurred.
*/
/**************************************************************************/
const char *_serialChunkedReceive(uint8_t *buffer, uint32_t *size, bool delay, uint32_t timeoutMs, uint32_t *available)
{
    size_t received = 0;
    bool overflow = (received >= *size);
    uint32_t startMs = _GetMs();
    for (bool eop = false ; !overflow && !eop ;) {
        while (!_SerialAvailable()) {
            if (timeoutMs && (_GetMs() - startMs >= timeoutMs)) {
                *size = received;
                if (received) {
                    NOTE_C_LOG_ERROR(ERRSTR("received only partial reply before timeout", c_iobad));
                }
                return ERRSTR("timeout: transaction incomplete {io}",c_iotimeout);
            }
            // Yield while awaiting the first byte (lazy). After the first byte,
            // start to spin for the remaining bytes (greedy).
            if (delay && received == 0) {
                _DelayMs(1);
            }
        }

        // Once we've received any character, we will no longer wait patiently
        timeoutMs = (CARD_INTRA_TRANSACTION_TIMEOUT_SEC * 1000);
        startMs = _GetMs();

        // Receive the next character
        char ch = _SerialReceive();

        // Place into the buffer
        buffer[received++] = ch;

        // Look for end-of-packet marker
        eop = (ch == '\n');

        // Check overflow condition
        overflow = ((received >= *size) && !eop);
        if (overflow) {
            // We haven't received a newline, so we're not done with this
            // packet. If the newline never comes, for whatever reason, when
            // this function is called again, we'll timeout. We don't just
            // use _SerialAvailable to set *available here because we're
            // typically reading faster than the serial buffer fills, and so
            // _SerialAvailable may return 0.
            *available = 1;
            break;
        } else {
            *available = 0;
        }
    }

    // Return it
    *size = received;
    return NULL;
}

/**************************************************************************/
/*!
  @brief  Transmit bytes over serial to the Notecard.

  @param   buffer A buffer of bytes to transmit.
  @param   size The count of bytes in the buffer to send.
  @param   delay Respect standard processing delays.

  @returns  A c-string with an error, or `NULL` if no error ocurred.
*/
/**************************************************************************/
const char *_serialChunkedTransmit(uint8_t *buffer, uint32_t size, bool delay)
{
    // Transmit the request in segments so as not to overwhelm the Notecard's
    // interrupt buffers
    uint32_t segOff = 0;
    uint32_t segLeft = size;

    if (sizeof(size_t) != 4) { // Give the compiler a hint to eliminate the code
        // Ensure truncation does not occur on 16-bit microcontrollers
        const size_t castSize = (size_t)size;
        if (castSize != size) {
            const char *err = ERRSTR("Cannot transmit provided size; limit to `size_t`", c_iobad);
            NOTE_C_LOG_ERROR(err);
            return err;
        }
    }

    while (true) {
        size_t segLen = segLeft;
        if (segLen > CARD_REQUEST_SERIAL_SEGMENT_MAX_LEN) {
            segLen = CARD_REQUEST_SERIAL_SEGMENT_MAX_LEN;
        }
        _SerialTransmit(&buffer[segOff], segLen, false);
        segOff += segLen;
        segLeft -= segLen;
        if (segLeft == 0) {
            break;
        }
        if (delay) {
            _DelayMs(CARD_REQUEST_SERIAL_SEGMENT_DELAY_MS);
        }
    }

    return NULL;
}
