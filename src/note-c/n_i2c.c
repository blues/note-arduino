/*!
 * @file n_i2c.c
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

// Forwards
NOTE_C_STATIC void _delayIO(void);
NOTE_C_STATIC const char * _i2cNoteQueryLength(uint32_t * available, uint32_t timeoutMs);

/**************************************************************************/
/*!
  @brief  We've noticed that there's an instability in some cards'
  implementations of I2C, and as a result we introduce an intentional
  delay before each and every I2C I/O.The timing was computed
  empirically based on a number of commercial devices.
*/
/**************************************************************************/
NOTE_C_STATIC void _delayIO(void)
{
    if (!cardTurboIO) {
        _DelayMs(6);
    }
}

/**************************************************************************/
/*!
  @brief  Query the Notecard for the length of cached data.

  @details  It is necessary to send a priming I2C transaction to understand
             the amount of data the Notecard is prepared to send before an
             I2C read request can be issued.
*/
/**************************************************************************/
NOTE_C_STATIC const char * _i2cNoteQueryLength(uint32_t * available,
        uint32_t timeoutMs)
{
    uint8_t dummy_buffer = 0;

    for (const uint32_t startMs = _GetMs() ; !(*available) ; _DelayMs(50)) {
        // Send a dummy I2C transaction to prime the Notecard
        const char *err = _I2CReceive(_I2CAddress(), &dummy_buffer, 0, available);
        if (err) {
            NOTE_C_LOG_ERROR(err);
            return err;
        }

        // If we've timed out, return an error
        if (timeoutMs && _GetMs() - startMs >= timeoutMs) {
            const char *err = ERRSTR("timeout: no response from Notecard {io}", c_iotimeout);
            NOTE_C_LOG_ERROR(err);
            return err;
        }
    }
    return NULL;
}

/**************************************************************************/
/*!
  @brief  Given a JSON string, perform an I2C transaction with the Notecard.

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
const char *_i2cNoteTransaction(const char *request, size_t reqLen, char **response, uint32_t timeoutMs)
{
    const char *err = NULL;

    // Lock over the entire transaction
    _LockI2C();

    err = _i2cChunkedTransmit((uint8_t *)request, reqLen, true);
    if (err) {
        _UnlockI2C();
        return err;
    }

    // If no reply expected, we're done
    if (response == NULL) {
        _UnlockI2C();
        return NULL;
    }

    _delayIO();

    // Allocate a buffer for input, noting that we always put the +1 in the
    // alloc so we can be assured that it can be null-terminated. This must be
    // the case because json parsing requires a null-terminated string.
    uint32_t available = 0;
    err = _i2cNoteQueryLength(&available, timeoutMs);
    if (err) {
        NOTE_C_LOG_ERROR(ERRSTR("failed to query Notecard", c_err));
        _UnlockI2C();
        return err;
    }
    size_t jsonbufAllocLen = (ALLOC_CHUNK * ((available / ALLOC_CHUNK) + ((available % ALLOC_CHUNK) > 0)));
    uint8_t *jsonbuf = NULL;
    if (jsonbufAllocLen) {
        jsonbuf = (uint8_t *)_Malloc(jsonbufAllocLen + 1);
        if (jsonbuf == NULL) {
            const char *err = ERRSTR("transaction: jsonbuf malloc failed", c_mem);
            NOTE_C_LOG_ERROR(err);
            _UnlockI2C();
            return err;
        }
    }

    // Receive the Notecard response
    uint32_t jsonbufLen = 0;
    do {
        uint32_t jsonbufAvailLen = (jsonbufAllocLen - jsonbufLen);

        // Append into the json buffer
        const char *err = _i2cChunkedReceive((uint8_t *)(jsonbuf + jsonbufLen), &jsonbufAvailLen, true, (CARD_INTRA_TRANSACTION_TIMEOUT_SEC * 1000), &available);
        if (err) {
            if (jsonbuf) {
                _Free(jsonbuf);
            }
            NOTE_C_LOG_ERROR(ERRSTR("error occured during receive", c_iobad));
            _UnlockI2C();
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
                if (jsonbuf) {
                    _Free(jsonbuf);
                }
                _UnlockI2C();
                return err;
            }
            if (jsonbuf) {
                memcpy(jsonbufNew, jsonbuf, jsonbufLen);
                _Free(jsonbuf);
            }
            jsonbuf = jsonbufNew;
            NOTE_C_LOG_DEBUG("additional receive buffer chunk allocated");
        }
    } while (available);

    // Done with the bus
    _UnlockI2C();

    // Null-terminate it, using the +1 space that we'd allocated in the buffer
    if (jsonbuf) {
        jsonbuf[jsonbufLen] = '\0';
    }

    // Return it
    *response = (char *)jsonbuf;
    return NULL;
}

//**************************************************************************/
/*!
  @brief  Initialize or re-initialize the I2C subsystem, returning false if
  anything fails.

  @returns a boolean. `true` if the reset was successful, `false`, if not.
*/
/**************************************************************************/
bool _i2cNoteReset(void)
{
    bool notecardReady = false;

    // Claim the I2C bus
    _LockI2C();
    NOTE_C_LOG_DEBUG("resetting I2C interface...");

    // Reset the I2C subsystem and exit if failure
    _DelayMs(CARD_REQUEST_I2C_SEGMENT_DELAY_MS);
    notecardReady = _I2CReset(_I2CAddress());
    if (!notecardReady) {
        NOTE_C_LOG_ERROR(ERRSTR("error encountered during I2C reset hook execution", c_err));
        _UnlockI2C();
        return false;
    }
    _delayIO();

    // The guaranteed behavior for robust resyncing is to send two newlines
    // and  wait for two echoed blank lines in return.
    for (size_t retries = 0; retries < CARD_RESET_SYNC_RETRIES ; ++retries) {
        // Send a newline to the module to clean out request/response processing
        // NOTE: This MUST always be `\n` and not `\r\n`, because there are some
        //       versions of the Notecard firmware will not respond to `\r\n`
        //       after communicating over I2C.
        const char *transmitErr = _I2CTransmit(_I2CAddress(), (uint8_t *)"\n", 1);
        // If we get a failure on transmitting the `\n`, it means that the
        // Notecard isn't present.
        if (transmitErr) {
            NOTE_C_LOG_ERROR(transmitErr);
            NOTE_C_LOG_ERROR(ERRSTR("error encountered during I2C transmit hook execution", c_err));
            _DelayMs(CARD_REQUEST_I2C_NACK_WAIT_MS);
            notecardReady = false;
            continue;
        }

        // Wait for the Notecard to respond with a carriage return and newline
        _DelayMs(CARD_REQUEST_I2C_SEGMENT_DELAY_MS);

        // Determine if I2C data is available
        // set initial state of variable to perform query
        uint16_t chunkLen = 0;

        // Content flags to determine if reset conditions are met.
        bool somethingFound = false;
        bool nonControlCharFound = false;

        // Read I2C data for at least `CARD_RESET_DRAIN_MS` continuously
        for (uint32_t startMs = _GetMs() ; (_GetMs() - startMs) < CARD_RESET_DRAIN_MS ;) {

            // Read the next chunk of available data
            uint32_t available = 0;
            uint8_t buffer[ALLOC_CHUNK] = {0};
            chunkLen = (chunkLen > sizeof(buffer)) ? sizeof(buffer) : chunkLen;
            chunkLen = (chunkLen > _I2CMax()) ? _I2CMax() : chunkLen;
            const char *err = _I2CReceive(_I2CAddress(), buffer, chunkLen, &available);
            if (err) {
                // We have received a hardware or protocol level error.
                // Introduce delay to relieve system stress.
                NOTE_C_LOG_ERROR(err);
                NOTE_C_LOG_ERROR(ERRSTR("error encountered during I2C receive hook execution", c_err));
                _DelayMs(CARD_REQUEST_I2C_SEGMENT_DELAY_MS);
                notecardReady = false;
                continue;
            }

            // Set content flags
            if (chunkLen) {
                somethingFound = true;
                // The Notecard responds to a bare `\n` with `\r\n`. If we get
                // any other characters back, it means the host and Notecard
                // aren't synced up yet and we need to transmit `\n` again.
                for (size_t i = 0; i < chunkLen ; ++i) {
                    char ch = buffer[i];
                    if (ch != '\n' && ch != '\r') {
                        nonControlCharFound = true;
                        // Reset the timer with each non-control character
                        startMs = _GetMs();
                    }
                }
            }

            // Read the minimum of the available bytes left to read and what
            // will fit into a 16-bit unsigned value (_I2CReceive takes the
            // buffer size as a uint16_t).
            chunkLen = (available > 0xFFFF) ? 0xFFFF : available;

            _DelayMs(CARD_REQUEST_I2C_CHUNK_DELAY_MS);
        }

        // If characters were received and they were ONLY `\r` or `\n`,
        // then the Notecard has been successfully reset.
        if (!somethingFound || nonControlCharFound) {
            notecardReady = false;
            if (somethingFound) {
                NOTE_C_LOG_WARN(ERRSTR("unrecognized data from notecard", c_iobad));
            } else {
                NOTE_C_LOG_ERROR(ERRSTR("notecard not responding", c_iobad));

                // Reset the I2C subsystem and exit if failure
                if (!_I2CReset(_I2CAddress())) {
                    NOTE_C_LOG_ERROR(ERRSTR("error encountered during I2C reset hook execution", c_err));
                    break;
                }
                _delayIO();
            }
        } else {
            notecardReady = true;
            break;
        }

        NOTE_C_LOG_DEBUG("retrying I2C interface reset...");
    }

    // Done with the I2C bus
    _UnlockI2C();

    // Done
    return notecardReady;
}

/**************************************************************************/
/*!
  @brief  Receive bytes over I2C from the Notecard.

  @param   buffer A buffer to receive bytes into.
  @param   size (in/out)
            - (in) The size of the buffer in bytes.
            - (out) The length of the received data in bytes.
  @param   delay Respect standard processing delays.
  @param   timeoutMs The maximum amount of time, in milliseconds, to wait for
            serial data to arrive. Passing zero (0) disables the timeout.
  @param   available (in/out)
            - (in) The amount of bytes to request. Sending zero (0) will
                   initiate a priming query when using the I2C interface.
            - (out) The amount of bytes unable to fit into the provided buffer.

  @returns  A c-string with an error, or `NULL` if no error ocurred.
*/
/**************************************************************************/
const char *_i2cNoteChunkedReceive(uint8_t *buffer, uint32_t *size, bool delay, uint32_t timeoutMs, uint32_t *available)
{
    _LockI2C();
    const char *errstr = _i2cChunkedReceive(buffer, size, delay, timeoutMs, available);
    _UnlockI2C();
    return errstr;
}

/**************************************************************************/
/*!
  @brief  Receive bytes over I2C from the Notecard.

  @param   buffer A buffer to receive bytes into.
  @param   size (in/out)
            - (in) The size of the buffer in bytes.
            - (out) The length of the received data in bytes.
  @param   delay Respect standard processing delays.
  @param   timeoutMs The maximum amount of time, in milliseconds, to wait for
            serial data to arrive. Passing zero (0) disables the timeout.
  @param   available (in/out)
            - (in) The amount of bytes to request. Sending zero (0) will
                   initiate a priming query when using the I2C interface.
            - (out) The amount of bytes unable to fit into the provided buffer.

  @returns  A c-string with an error, or `NULL` if no error ocurred.
*/
/**************************************************************************/
const char *_i2cChunkedReceive(uint8_t *buffer, uint32_t *size, bool delay, uint32_t timeoutMs, uint32_t *available)
{
    // Load buffer with chunked I2C values
    size_t received = 0;
    uint16_t requested = 0;
    bool overflow = false;
    uint32_t startMs = _GetMs();

    // Request all available bytes, up to the maximum request size
    requested = (*available > 0xFFFF) ? 0xFFFF : *available;
    requested = (requested > _I2CMax()) ? _I2CMax() : requested;

    for (bool eop = false ; !overflow ; overflow = ((received + requested) > *size)) {

        // Read a chunk of data from I2C
        // The first read will request zero bytes to query the amount of data
        // available to receive from the Notecard.
        const char *err = _I2CReceive(_I2CAddress(), (buffer + received), requested, available);
        if (err) {
            *size = received;
            NOTE_C_LOG_ERROR(err);
            return err;
        }

        // Add requested bytes to received total
        received += requested;

        // Once we've received any character, we will no longer wait patiently
        if (requested != 0) {
            timeoutMs = (CARD_INTRA_TRANSACTION_TIMEOUT_SEC * 1000);
            startMs = _GetMs();
        }

        // Request all available bytes, up to the maximum request size
        requested = (*available > 0xFFFF) ? 0xFFFF : *available;
        requested = (requested > _I2CMax()) ? _I2CMax() : requested;

        // Look for end-of-packet marker
        if (received > 0 && !eop) {
            eop = (buffer[received-1] == '\n');
        }

        // If the last byte of the chunk is `\n`, then we have received a
        // complete message. However, everything pending from the Notecard must
        // be pulled. This loop will only exit when a newline is received AND
        // there are no more bytes available from the Notecard, OR if the buffer
        // is full and cannot receive more bytes (i.e. overflow condition).
        if (*available && eop) {
            NOTE_C_LOG_WARN(ERRSTR("received newline before all data was received", c_iobad));
        };

        // If there's something available on the Notecard for us to receive, do it
        if (*available > 0) {
            continue;
        }

        // If there's nothing available AND we've received a newline, we're done
        if (eop) {
            break;
        }

        // Exit on timeout
        if (timeoutMs && (_GetMs() - startMs >= timeoutMs)) {
            *size = received;
            if (received) {
                NOTE_C_LOG_ERROR(ERRSTR("received only partial reply before timeout", c_iobad));
            }
            return ERRSTR("timeout: transaction incomplete {io}", c_iotimeout);
        }

        // Delay, simply waiting for the Note to process the request
        if (delay) {
            _DelayMs(50);
        }
    }

    *size = received;
    return NULL;
}

/**************************************************************************/
/*!
  @brief  Transmit bytes over I2C to the Notecard.

  @param   buffer A buffer of bytes to transmit.
  @param   size The count of bytes in the buffer to send
  @param   delay Respect standard processing delays.

  @returns  A c-string with an error, or `NULL` if no error ocurred.
*/
/**************************************************************************/
const char *_i2cNoteChunkedTransmit(uint8_t *buffer, uint32_t size, bool delay)
{
    _LockI2C();
    const char *errstr = _i2cChunkedTransmit(buffer, size, delay);
    _UnlockI2C();
    return errstr;
}

/**************************************************************************/
/*!
  @brief  Transmit bytes over I2C to the Notecard.

  @param   buffer A buffer of bytes to transmit.
  @param   size The count of bytes in the buffer to send
  @param   delay Respect standard processing delays.

  @returns  A c-string with an error, or `NULL` if no error ocurred.
*/
/**************************************************************************/
const char *_i2cChunkedTransmit(uint8_t *buffer, uint32_t size, bool delay)
{
    // Transmit the request in chunks, but also in segments so as not to
    // overwhelm the notecard's interrupt buffers
    const char *estr;
    uint8_t *chunk = buffer;
    uint16_t sentInSegment = 0;
    while (size > 0) {
        // Constrain chunkLen to fit into 16 bits (_I2CTransmit takes the buffer
        // size as a uint16_t).
        uint16_t chunkLen = (size > 0xFFFF) ? 0xFFFF : size;
        // Constrain chunkLen to be <= _I2CMax().
        chunkLen = (chunkLen > _I2CMax()) ? _I2CMax() : chunkLen;

        if (delay) {
            _delayIO();
        }
        estr = _I2CTransmit(_I2CAddress(), chunk, chunkLen);
        if (estr != NULL) {
            _I2CReset(_I2CAddress());
            NOTE_C_LOG_ERROR(estr);
            return estr;
        }
        chunk += chunkLen;
        size -= chunkLen;
        sentInSegment += chunkLen;
        if (sentInSegment > CARD_REQUEST_I2C_SEGMENT_MAX_LEN) {
            sentInSegment = 0;
            if (delay) {
                _DelayMs(CARD_REQUEST_I2C_SEGMENT_DELAY_MS);
            }
        }
        if (delay) {
            _DelayMs(CARD_REQUEST_I2C_CHUNK_DELAY_MS);
        }
    }

    return NULL;
}
