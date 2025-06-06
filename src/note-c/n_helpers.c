/*!
 * @file n_helpers.c
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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "n_lib.h"

// When interfacing with the Notecard, it is generally encouraged that the JSON
// object manipulation and calls to the note-arduino library are done directly
// at point of need. However, there are cases in which it's convenient to have a
// wrapper. The most common reason is when it's best to have a suppression timer
// on the actual call to the Notecard so as not to assault the I2C bus or UART
// on a continuing basis - the most common examples of this being "card.time"
// and any other Notecard state that needs to be polled for an app, such as the
// device location, its voltage level, and so on.  This file contains this kind
// of wrapper, just implemented here as a convenience to all developers.

// Time-related suppression timer and cache
static uint32_t timeBaseSetAtMs = 0;
static JTIME timeBaseSec = 0;
static bool timeBaseSetManually = false;
static uint32_t suppressionTimerSecs = 10;
static uint32_t refreshTimerSecs = 86400;
static uint32_t timeTimer = 0;
static uint32_t timeRefreshTimer = 0;
static bool zoneStillUnavailable = true;
static bool zoneForceRefresh = false;
static char curZone[10] = {0};
static char curArea[64] = {0};
static char curCountry[8] = "";
static int curZoneOffsetMins = 0;

// Location-related suppression timer and cache
static uint32_t locationTimer = 0;
static char locationLastErr[64] = {0};
static bool locationValid = false;

// Connection-related suppression timer and cache
static uint32_t connectivityTimer = 0;
static bool cardConnected = false;

// Status suppression timer
static uint32_t statusTimer = 0;

// DEPRECATED. Turbo communications mode, for special use cases and well-tested
// hardware.
bool cardTurboIO = false;

// Service config-related suppression timer and cache
static uint32_t serviceConfigTimer = 0;
static char scDevice[128] = {0};
static char scSN[128] = {0};
static char scProduct[128] = {0};
static char scService[128] = {0};

// For date conversions
#define daysByMonth(y) ((y)&03||(y)==0?normalYearDaysByMonth:leapYearDaysByMonth)
static short leapYearDaysByMonth[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
static short normalYearDaysByMonth[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
static const char *dayNames[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

// Forwards
NOTE_C_STATIC void _setTime(JTIME seconds);
NOTE_C_STATIC bool _timerExpiredSecs(uint32_t *timer, uint32_t periodSecs);
NOTE_C_STATIC int _yToDays(int year);

static const char NOTE_C_BINARY_EOP = '\n';

//**************************************************************************/
/*!
  @brief  Configure the flow control for the auxiliary serial port.
  @param   bufSize The size of the receive buffer.
  @param   delayMs The time to wait between reads.

  @returns `true` on success, `false` on failure.
*/
/**************************************************************************/
bool NoteAuxSerialFlowControl(int bufSize, int delayMs)
{
    bool result = false;

    J *req = NoteNewRequest("card.aux.serial");
    if (req != NULL) {
        JAddIntToObject(req, "max", bufSize - 1);
        JAddIntToObject(req, "ms", delayMs);
        result = NoteRequest(req);
    } else {
        NOTE_C_LOG_ERROR(ERRSTR("Failed to configure AUX serial flow control", c_mem));
    }

    return result;
}

//**************************************************************************/
/*!
  @brief  Decode binary data received from the Notecard.

  @param  encData The encoded binary data to decode.
  @param  encDataLen The length of the encoded binary data.
  @param  decBuf The target buffer for the decoded data. For in-place decoding,
                 `decBuf` can use the same address as `encData` (see note).
  @param  decBufSize The size of `decBuf`.

  @returns  The length of the decoded data, or zero on error.

  @note Use `NoteBinaryCodecMaxDecodedLength()` to calculate the required
        size for the buffer pointed to by the `decBuf` parameter, which MUST
        accommodate both the encoded data and newline terminator.
  @note This API supports in-place decoding. If you wish to utilize in-place
        decoding, then set `decBuf` to `encData` and `decBufSize` to `encLen`.
 */
/**************************************************************************/
uint32_t NoteBinaryCodecDecode(const uint8_t *encData, uint32_t encDataLen,
                               uint8_t *decBuf, uint32_t decBufSize)
{
    uint32_t result;

    // Validate parameter(s)
    if (encData == NULL || decBuf == NULL) {
        NOTE_C_LOG_ERROR(ERRSTR("NULL parameter", c_err));
        result = 0;
    } else if (decBufSize < _cobsGuaranteedFit(encDataLen)) {
        NOTE_C_LOG_ERROR(ERRSTR("output buffer too small", c_err));
        result = 0;
    } else {
        result = _cobsDecode((uint8_t *)encData, encDataLen, NOTE_C_BINARY_EOP, decBuf);
    }

    return result;
}

//**************************************************************************/
/*!

  @brief  Encode binary data to transmit to the Notecard.

  @param  decData The decoded binary data to encode.
  @param  decDataLen The length of the decoded binary data.
  @param  encBuf The target buffer for the encoded data. For in-place encoding,
                 `encBuf` can use the same buffer as `decData`, but cannot
                 share the same address (see note).
  @param  encBufSize The size of `encBuf`.

  @returns  The length of the encoded data, or zero on error.

  @note Use `NoteBinaryCodecMaxEncodedLength()` to calculate the required
        size for the buffer pointed to by the `encBuf` parameter, which MUST
        accommodate both the encoded data and newline terminator.
  @note This API supports in-place encoding. If you wish to utilize in-place
        encoding, shift the decoded data to the end of the buffer, update
        `decBuf` accordingly, and set the value of `encBuf` to the beginning
        of the buffer.
 */
/**************************************************************************/
uint32_t NoteBinaryCodecEncode(const uint8_t *decData, uint32_t decDataLen,
                               uint8_t *encBuf, uint32_t encBufSize)
{
    uint32_t result;

    // Validate parameter(s)
    if (decData == NULL || encBuf == NULL) {
        NOTE_C_LOG_ERROR(ERRSTR("NULL parameter", c_err));
        result = 0;
    } else if ((encBufSize < _cobsEncodedMaxLength(decDataLen))
               && (encBufSize < _cobsEncodedLength(decData, decDataLen))) {
        // NOTE: `_cobsEncodedMaxLength()` provides a constant time [O(1)] means
        //       of checking the buffer size. Only when it fails will the linear
        //       time [O(n)] check, `_cobsEncodedLength()`, be invoked.
        NOTE_C_LOG_ERROR(ERRSTR("output buffer too small", c_err));
        result = 0;
    } else {
        result = _cobsEncode((uint8_t *)decData, decDataLen, NOTE_C_BINARY_EOP, encBuf);
    }

    return result;
}

//**************************************************************************/
/*!
  @brief  Compute the maximum decoded data length guaranteed
          to fit into a fixed-size buffer, after being encoded.

          This API is designed for a space constrained environment, where a
          working buffer has been allocated to facilitate binary transactions.

          There are two primary use cases:

          1. When data is retrieved from the Notecard, it must be requested in
             terms of the unencoded offset and length. However, the data is
             encoded prior to transmission, and, as a result, the buffer must be
             capable of receiving the encoded (larger) data. This API returns a
             length that is safe to request from the Notecard, because the
             resulting encoded data is guaranteed to fit in the provided buffer.
          2. When data is transmitted to the Notecard, this API can be used to
             verify whether or not unencoded data of a given length will fit in
             the provided buffer after encoding.

  @param  bufferSize The size of the fixed-size buffer.

  @returns  The max length of unencoded data certain to fit in the fixed-size
            buffer, after being encoded.
 */
/**************************************************************************/
uint32_t NoteBinaryCodecMaxDecodedLength(uint32_t bufferSize)
{
    return _cobsGuaranteedFit(bufferSize);
}

//**************************************************************************/
/*!
  @brief  Compute the maximum buffer size needed to encode
          any unencoded buffer of the given length.

  @param  unencodedLength The length of an unencoded buffer.

  @returns  The max required buffer size to hold the encoded data.
 */
/**************************************************************************/
uint32_t NoteBinaryCodecMaxEncodedLength(uint32_t unencodedLength)
{
    return _cobsEncodedMaxLength(unencodedLength);
}

//**************************************************************************/
/*!
  @brief  Get the length of the data in the Notecard's binary store. If there's
          no data on the Notecard, then `*len` will return 0.

  @param  len [out] The length of the decoded contents of the Notecard's binary
          store.

  @returns An error string on error and NULL on success.
 */
/**************************************************************************/
const char * NoteBinaryStoreDecodedLength(uint32_t *len)
{
    // Validate parameter(s)
    if (!len) {
        const char *err = ERRSTR("len cannot be NULL", c_bad);
        NOTE_C_LOG_ERROR(err);
        return err;
    }

    // Issue a "card.binary" request.
    J *rsp = NoteRequestResponse(NoteNewRequest("card.binary"));
    if (!rsp) {
        const char *err = ERRSTR("unable to issue binary request", c_err);
        NOTE_C_LOG_ERROR(err);
        return err;
    }

    // Ensure the transaction doesn't return an error
    // and confirm the binary feature is available
    if (NoteResponseError(rsp)) {
        const char *jErr = JGetString(rsp, "err");
        // Swallow `{bad-bin}` errors, because we intend to overwrite the data.
        if (!NoteErrorContains(jErr, c_badbinerr)) {
            NOTE_C_LOG_ERROR(jErr);
            JDelete(rsp);
            const char *err = ERRSTR("unexpected error received during handshake", c_bad);
            NOTE_C_LOG_ERROR(err);
            return err;
        }
    }

    // Examine "length" from the response to evaluate the length of the decoded
    // data residing on the Notecard.
    *len = JGetInt(rsp, "length");
    JDelete(rsp);

    return NULL;
}

//**************************************************************************/
/*!
  @brief  Get the required buffer length to receive the entire binary object
          stored in the Notecard's binary store.

  @param  len [out] The length required to hold the entire contents of the
          Notecard's binary store. If there's no data on the Notecard, then
          `len` will return 0.

  @returns An error string on error and NULL on success.
 */
/**************************************************************************/
const char * NoteBinaryStoreEncodedLength(uint32_t *len)
{
    // Validate parameter(s)
    if (!len) {
        const char *err = ERRSTR("size cannot be NULL", c_err);
        NOTE_C_LOG_ERROR(err);
        return err;
    }

    // Issue a "card.binary" request.
    J *rsp = NoteRequestResponse(NoteNewRequest("card.binary"));
    if (!rsp) {
        const char *err = ERRSTR("unable to issue binary request", c_err);
        NOTE_C_LOG_ERROR(err);
        return err;
    }

    // Ensure the transaction doesn't return an error
    // and confirm the binary feature is available
    if (NoteResponseError(rsp)) {
        const char *jErr = JGetString(rsp, "err");
        // Swallow `{bad-bin}` errors, because we intend to overwrite the data.
        if (!NoteErrorContains(jErr, c_badbinerr)) {
            NOTE_C_LOG_ERROR(jErr);
            JDelete(rsp);
            const char *err = ERRSTR("unexpected error received during handshake", c_bad);
            NOTE_C_LOG_ERROR(err);
            return err;
        }
    }

    // Examine "cobs" from the response to evaluate the space required to hold
    // the encoded data to be received from the Notecard.
    long int cobs = JGetInt(rsp, "cobs");
    JDelete(rsp);
    *len = cobs;

    return NULL;
}

//**************************************************************************/
/*!
  @brief  Receive a large binary object from the Notecard's binary store.

  @param  buffer A buffer to hold the binary range.
  @param  bufLen The total length of the provided buffer.
  @param  decodedOffset The offset to the decoded binary data residing
                        in the Notecard's binary store.
  @param  decodedLen The length of the decoded data to fetch from the Notecard.

  @returns  NULL on success, else an error string pointer.

  @note  The buffer must be large enough to hold the encoded value of the
         data store contents from the requested offset for the specified length.
         To determine the necessary buffer size for a given data length, use
         `NoteBinaryCodecMaxEncodedLength()`, or if you wish to consume the
         entire buffer use `(NoteBinaryStoreEncodedLength() + 1)` instead.
 */
/**************************************************************************/
const char * NoteBinaryStoreReceive(uint8_t *buffer, uint32_t bufLen,
                                    uint32_t decodedOffset, uint32_t decodedLen)
{
    // Validate parameter(s)
    if (!buffer) {
        const char *err = ERRSTR("NULL buffer", c_bad);
        NOTE_C_LOG_ERROR(err);
        return err;
    }
    if (bufLen < _cobsEncodedMaxLength(decodedLen)) {
        const char *err = ERRSTR("insufficient buffer size", c_bad);
        NOTE_C_LOG_ERROR(err);
        return err;
    }
    if (decodedLen == 0) {
        const char *err = ERRSTR("decodedLen cannot be zero (0)", c_bad);
        NOTE_C_LOG_ERROR(err);
        return err;
    }

    // Claim Notecard Mutex
    _LockNote();

    // Issue `card.binary.get` and capture `"status"` from response
    char status[NOTE_MD5_HASH_STRING_SIZE] = {0};
    J *req = NoteNewRequest("card.binary.get");
    if (req) {
        JAddIntToObject(req, "offset", decodedOffset);
        JAddIntToObject(req, "length", decodedLen);

        // We already have the Notecard lock, so call
        // _noteTransactionShouldLock with `lockNotecard` set to false so we
        // don't try to lock again.
        J *rsp = _noteTransactionShouldLock(req, false);
        JDelete(req);
        // Ensure the transaction doesn't return an error.
        if (!rsp || NoteResponseError(rsp)) {
            if (rsp) {
                NOTE_C_LOG_ERROR(JGetString(rsp,"err"));
                JDelete(rsp);
            }

            const char *err = ERRSTR("failed to initialize binary transaction", c_err);
            NOTE_C_LOG_ERROR(err);
            _UnlockNote();
            return err;
        }

        // Examine "status" from the response to evaluate the MD5 checksum.
        strlcpy(status, JGetString(rsp,"status"), NOTE_MD5_HASH_STRING_SIZE);
        JDelete(rsp);
    } else {
        const char *err = ERRSTR("unable to allocate request", c_mem);
        NOTE_C_LOG_ERROR(err);
        _UnlockNote();
        return err;
    }

    // Read raw bytes from the active interface into a predefined buffer
    uint32_t available = 0;
    NOTE_C_LOG_DEBUG("receiving binary data...");
    const char *err = _ChunkedReceive(buffer, &bufLen, false, (CARD_INTRA_TRANSACTION_TIMEOUT_SEC * 1000), &available);
    NOTE_C_LOG_DEBUG("binary receive complete.");

    // Release Notecard Mutex
    _UnlockNote();

    // Ensure transaction was successful
    if (err) {
        // Queue a reset when a problem is detected, otherwise `note-c` will
        // attempt to allocate memory to receive the response.
        NoteResetRequired();
        return ERRSTR(err, c_err);
    }

    // Check buffer overflow condition
    if (available) {
        const char *err = ERRSTR("unexpected data available", c_err);
        NOTE_C_LOG_ERROR(err);
        // Queue a reset when a problem is detected, otherwise `note-c` will
        // attempt to allocate memory to receive the response.
        NoteResetRequired();
        return err;
    }

    // _ChunkedReceive returns the raw bytes that came off the wire, which
    // includes a terminating newline that ends the packet. This newline isn't
    // part of the binary payload, so we decrement the length by 1 to remove it.
    --bufLen;

    // Decode it in place, which is safe because decoding shrinks
    const uint32_t decLen = NoteBinaryCodecDecode(buffer, bufLen, buffer, bufLen);

    // Ensure the decoded length matches the caller's expectations.
    if (decodedLen != decLen) {
        const char *err = ERRSTR("length mismatch after decoding", c_err);
        NOTE_C_LOG_ERROR(err);
        // Queue a reset when a problem is detected, otherwise `note-c` will
        // attempt to allocate memory to receive the response.
        NoteResetRequired();
        return err;
    }

    // Put a hard marker at the end of the decoded portion of the buffer. This
    // enables easier human reasoning when interrogating the buffer, if the
    // buffer holds a string.
    buffer[decLen] = '\0';

    // Verify MD5
    char hashString[NOTE_MD5_HASH_STRING_SIZE] = {0};
    NoteMD5HashString(buffer, decLen, hashString, NOTE_MD5_HASH_STRING_SIZE);
    if (strncmp(hashString, status, NOTE_MD5_HASH_STRING_SIZE)) {
        const char *err = ERRSTR("computed MD5 does not match received MD5", c_err);
        NOTE_C_LOG_ERROR(err);
        return err;
    }

    // Return `NULL` if success, else error string pointer
    return NULL;
}

//**************************************************************************/
/*!
  @brief  Reset the Notecard's binary store.

  @returns  NULL on success, else an error string pointer.

  @note  This operation is necessary to clear the Notecard's binary buffer after
         a binary object is received from the Notecard, or if the Notecard's
         binary store has been left in an unknown state due to an error arising
         from a binary transfer to the Notecard.
 */
/**************************************************************************/
const char * NoteBinaryStoreReset(void)
{
    J *req = NoteNewRequest("card.binary");
    if (req) {
        JAddBoolToObject(req, "delete", true);

        // Ensure the transaction doesn't return an error.
        J *rsp = NoteRequestResponse(req);
        if (NoteResponseError(rsp)) {
            NOTE_C_LOG_ERROR(JGetString(rsp,"err"));
            JDelete(rsp);
            const char *err = ERRSTR("failed to reset binary buffer", c_err);
            NOTE_C_LOG_ERROR(err);
            return err;
        }

        JDelete(rsp);
    } else {
        const char *err = ERRSTR("unable to allocate request", c_mem);
        NOTE_C_LOG_ERROR(err);
        return err;
    }

    return NULL;
}

//**************************************************************************/
/*!
  @brief  Transmit a large binary object to the Notecard's binary store.

  @param  unencodedData  A buffer with data to encode in place.
  @param  unencodedLen   The length of the data in the buffer.
  @param  bufLen         The total length of the buffer (see notes).
  @param  notecardOffset The offset where the data buffer should be appended
                         to the decoded binary data residing in the Notecard's
                         binary store. This does not provide random access, but
                         rather ensures alignment across sequential writes.

  @returns  NULL on success, else an error string pointer.

  @note  Buffers are encoded in place, the buffer _MUST_ be larger than the data
         to be encoded. The original contents of the buffer will be modified.
         Use `NoteBinaryCodecMaxEncodedLength()` to calculate the required size
         for the buffer pointed to by the `unencodedData` parameter, which MUST
         accommodate both the encoded data and newline terminator.
 */
/**************************************************************************/
const char * NoteBinaryStoreTransmit(uint8_t *unencodedData, uint32_t unencodedLen,
                                     uint32_t bufLen, uint32_t notecardOffset)
{
    // Validate parameter(s)
    if (!unencodedData) {
        const char *err = ERRSTR("unencodedData cannot be NULL", c_err);
        NOTE_C_LOG_ERROR(err);
        return err;
    } else if ((bufLen < _cobsEncodedMaxLength(unencodedLen))
               && (bufLen < (_cobsEncodedLength(unencodedData, unencodedLen) + 1))) {
        // NOTE: `_cobsEncodedMaxLength()` provides a constant time [O(1)] means
        //       of checking the buffer size. Only when it fails will the linear
        //       time [O(n)] check, `_cobsEncodedLength()`, be invoked.
        const char *err = ERRSTR("insufficient buffer size", c_bad);
        NOTE_C_LOG_ERROR(err);
        return err;
    }

    // Issue a "card.binary" request. The length is reset if this is the
    // first segment, clearing out any error that might potentially be
    // pending from a previous use of the binary store.
    J *req = NoteNewRequest("card.binary");
    if (notecardOffset == 0) {
        JAddBoolToObject(req, "reset", true);
    }
    J *rsp = NoteRequestResponse(req);
    if (!rsp) {
        const char *err = ERRSTR("unable to issue binary request", c_err);
        NOTE_C_LOG_ERROR(err);
        return err;
    }

    // Ensure the transaction doesn't return an error
    // and confirm the binary feature is available
    if (NoteResponseError(rsp)) {
        const char *jErr = JGetString(rsp, "err");
        // Swallow `{bad-bin}` errors, because we intend to overwrite the data.
        if (!NoteErrorContains(jErr, c_badbinerr)) {
            NOTE_C_LOG_ERROR(jErr);
            JDelete(rsp);
            const char *err = ERRSTR("unexpected error received during handshake", c_bad);
            NOTE_C_LOG_ERROR(err);
            return err;
        }
    }

    // Examine "length" and "max" from the response to evaluate the unencoded
    // space available to "card.binary.put" on the Notecard.
    const long len = JGetInt(rsp,"length");
    const long max = JGetInt(rsp,"max");
    JDelete(rsp);
    if (!max) {
        const char *err = ERRSTR("unexpected response: max is zero or not present", c_err);
        NOTE_C_LOG_ERROR(err);
        return err;
    }

    // Validate the index provided by the caller, against the `length` value
    // returned from the Notecard to ensure the caller and Notecard agree on
    // how much data is residing on the Notecard.
    if ((long)notecardOffset != len) {
        const char *err = ERRSTR("notecard data length is misaligned with offset", c_mem);
        NOTE_C_LOG_ERROR(err);
        return err;
    }

    // When offset is zero, the Notecard's entire binary buffer is available
    const uint32_t remaining = (notecardOffset ? (max - len) : max);
    if (unencodedLen > remaining) {
        const char *err = ERRSTR("buffer size exceeds available memory", c_mem);
        NOTE_C_LOG_ERROR(err);
        return err;
    }

    // Calculate MD5
    char hashString[NOTE_MD5_HASH_STRING_SIZE] = {0};
    NoteMD5HashString(unencodedData, unencodedLen, hashString, NOTE_MD5_HASH_STRING_SIZE);

    // Shift the data to the end of the buffer. Next, we'll encode the data,
    // outputting the encoded data to the front of the buffer.
    const uint32_t dataShift = (bufLen - unencodedLen);
    memmove(unencodedData + dataShift, unencodedData, unencodedLen);

    // Create an alias to help reason about the buffer after in-place encoding.
    uint8_t * const encodedData = unencodedData;

    // Update unencoded data pointer
    unencodedData += dataShift;

    // Capture encoded length
    // NOTE: `(bufLen - 1)` accounts for one byte of space we need to save for a
    //       newline to mark the end of the packet.
    const uint32_t encLen = NoteBinaryCodecEncode(unencodedData, unencodedLen, encodedData, (bufLen - 1));

    // Append the \n, which marks the end of a packet.
    encodedData[encLen] = '\n';

    const size_t NOTE_C_BINARY_RETRIES = 3;
    for (size_t i = 0 ; i < NOTE_C_BINARY_RETRIES ; ++i) {
        // Claim Notecard Mutex
        _LockNote();

        // Issue a "card.binary.put"
        J *req = NoteNewRequest("card.binary.put");
        if (req) {
            JAddIntToObject(req, "cobs", encLen);
            if (notecardOffset) {
                JAddIntToObject(req, "offset", notecardOffset);
            }
            JAddStringToObject(req, "status", hashString);

            // We already have the Notecard lock, so call
            // _noteTransactionShouldLock with `lockNotecard` set to false so we
            // don't try to lock again.
            rsp = _noteTransactionShouldLock(req, false);
            JDelete(req);
            // Ensure the transaction doesn't return an error.
            if (!rsp || NoteResponseError(rsp)) {
                if (rsp) {
                    NOTE_C_LOG_ERROR(JGetString(rsp,"err"));
                    JDelete(rsp);
                }

                const char *err = ERRSTR("failed to initialize binary transaction", c_err);
                NOTE_C_LOG_ERROR(err);
                _UnlockNote();
                // On errors, we restore the caller's input buffer by decoding
                // it. The caller is then able to retry transmission with their
                // original pointer to this buffer.
                NoteBinaryCodecDecode(encodedData, encLen, encodedData, bufLen);
                return err;
            }

            JDelete(rsp);
        } else {
            const char *err = ERRSTR("unable to allocate request", c_mem);
            NOTE_C_LOG_ERROR(err);
            _UnlockNote();
            NoteBinaryCodecDecode(encodedData, encLen, encodedData, bufLen);
            return err;
        }

        // Immediately send the encoded binary.
        NOTE_C_LOG_DEBUG("transmitting binary data...");
        const char *err = _ChunkedTransmit(encodedData, (encLen + 1), false);
        NOTE_C_LOG_DEBUG("binary transmission complete.");

        // Release Notecard Mutex
        _UnlockNote();

        // Ensure transaction was successful
        if (err) {
            NoteBinaryCodecDecode(encodedData, encLen, encodedData, bufLen);
            return ERRSTR(err, c_err);
        }

        // Issue a `"card.binary"` request.
        rsp = NoteRequestResponse(NoteNewRequest("card.binary"));
        if (!rsp) {
            const char *err = ERRSTR("unable to validate request", c_err);
            NOTE_C_LOG_ERROR(err);
            NoteBinaryCodecDecode(encodedData, encLen, encodedData, bufLen);
            return err;
        }

        // Ensure the transaction doesn't return an error
        // to confirm the binary was received
        if (NoteResponseError(rsp)) {
            const char *jErr = JGetString(rsp, "err");
            if (NoteErrorContains(jErr, c_badbinerr)) {
                NOTE_C_LOG_WARN(jErr);
                JDelete(rsp);
                if ( i < (NOTE_C_BINARY_RETRIES - 1) ) {
                    NOTE_C_LOG_WARN("retrying binary transmission...");
                    continue;
                }
                const char *err = ERRSTR("binary data invalid", c_bad);
                NOTE_C_LOG_ERROR(err);
                NoteBinaryCodecDecode(encodedData, encLen, encodedData, bufLen);
                return err;
            } else {
                NOTE_C_LOG_ERROR(jErr);
                JDelete(rsp);
                const char *err = ERRSTR("unexpected error received during confirmation", c_bad);
                NOTE_C_LOG_ERROR(err);
                NoteBinaryCodecDecode(encodedData, encLen, encodedData, bufLen);
                return err;
            }
        }
        JDelete(rsp);
        break;
    }

    // Return `NULL` on success
    return NULL;
}

//**************************************************************************/
/*!
  @brief  Determine if the card time is "real" calendar/clock time, or if
  it is simply the millisecond clock since boot.
  @returns  A boolean indicating whether the current time is valid.
*/
/**************************************************************************/
bool NoteTimeValid(void)
{
    timeTimer = 0;
    return NoteTimeValidST();
}

//**************************************************************************/
/*!
  @brief  Determine if if the suppression-timer card time is valid.
  @returns  A boolean indicating whether the current time is valid.
*/
/**************************************************************************/
bool NoteTimeValidST(void)
{
    NoteTimeST();
    return (timeBaseSec != 0);
}

//**************************************************************************/
/*!
  @brief  Get the current epoch time, unsuppressed.
  @returns  The current time.
*/
/**************************************************************************/
JTIME NoteTime(void)
{
    timeTimer = 0;
    return NoteTimeST();
}

/*!
  @brief Set the number of minutes between refreshes of the time from the
         Notecard, to help minimize clock drift on this host. Set this to 0 for
         no auto-refresh; it defaults to daily.

  @param mins Minutes between time refreshes.
 */
void NoteTimeRefreshMins(uint32_t mins)
{
    refreshTimerSecs = mins * 60;
}

//**************************************************************************/
/*!
  @brief  Set the time.
  @param   seconds The UNIX Epoch time.
*/
/**************************************************************************/
NOTE_C_STATIC void _setTime(JTIME seconds)
{
    timeBaseSec = seconds;
    timeBaseSetAtMs = _GetMs();
}

/*!
  @brief  Set the time from a source that is NOT the Notecard
  @param  secondsUTC The UNIX Epoch time, or 0 for automatic Notecard time
  @param  offset The local time zone offset, in minutes, to adjust UTC
  @param  zone The optional local time zone name (3 character c-string). Note
                that this isn't used in any time calculations. To compute
                accurate local time, only the offset is used. See
                https://www.iana.org/time-zones for a time zone database.
  @param   country The optional country
  @param   area The optional region
 */
void NoteTimeSet(JTIME secondsUTC, int offset, char *zone, char *country, char *area)
{
    if (secondsUTC == 0) {
        timeBaseSec = 0;
        timeBaseSetAtMs = 0;
        timeBaseSetManually = false;
        zoneStillUnavailable = true;
        zoneForceRefresh = false;
    } else {
        timeBaseSec = secondsUTC;
        timeBaseSetAtMs = _GetMs();
        timeBaseSetManually = true;
        zoneStillUnavailable = false;
        curZoneOffsetMins = offset;
        strlcpy(curZone, zone == NULL ? "UTC" : zone, sizeof(curZone));
        strlcpy(curArea, area == NULL ? "" : area, sizeof(curArea));
        strlcpy(curCountry, country == NULL ? "" : country, sizeof(curCountry));
    }
}

//**************************************************************************/
/*!
  @brief  Print a full, newline-terminated line.
  @param   line The c-string line to print.
*/
/**************************************************************************/
void NotePrintln(const char *line)
{
    NotePrint(line);
    NotePrint(c_newline);
}

//**************************************************************************/
/*!
  @brief  Log text "raw" to either the active debug console or to the
  Notecard's USB console.
  @param   text  A debug string for output.
*/
/**************************************************************************/
bool NotePrint(const char *text)
{
    bool success = false;

    if (_noteIsDebugOutputActive()) {
        _Debug(text);
        return true;
    }

    J *req = NoteNewRequest("card.log");
    JAddStringToObject(req, "text", text);
    success = NoteRequest(req);

    return success;
}

//**************************************************************************/
/*!
  @brief  Get the current epoch time as known by the module. If it isn't known
  by the module, just return the time since boot as indicated by the
  millisecond clock.
  @returns  The current time, or the time since boot.
*/
/**************************************************************************/
JTIME NoteTimeST(void)
{

    // Handle timer tick wrap by resetting the base
    uint32_t nowMs = _GetMs();
    if (timeBaseSec != 0 && nowMs < timeBaseSetAtMs) {
        int64_t actualTimeMs = 0x100000000LL + (int64_t) nowMs;
        int64_t elapsedTimeMs = actualTimeMs - (int64_t) timeBaseSetAtMs;
        uint32_t elapsedTimeSecs = (uint32_t) (elapsedTimeMs / 1000);
        timeBaseSec += elapsedTimeSecs;
        timeBaseSetAtMs = nowMs;
    }

    // If it's time to refresh the time, do so
    if (refreshTimerSecs != 0 && _timerExpiredSecs(&timeRefreshTimer, refreshTimerSecs)) {
        timeTimer = 0;
    }

    // If we haven't yet fetched the time, or if we still need the timezone, do
    // so with a suppression timer so that we don't hammer the module before
    // it's had a chance to connect to the network to fetch time.
    if (!timeBaseSetManually && (timeTimer == 0 || timeBaseSec == 0 || zoneStillUnavailable || zoneForceRefresh)) {
        if (_timerExpiredSecs(&timeTimer, suppressionTimerSecs)) {

            // Request time and zone info from the card
            J *rsp = NoteRequestResponse(NoteNewRequest("card.time"));
            if (rsp != NULL) {
                if (!NoteResponseError(rsp)) {
                    JTIME seconds = JGetInt(rsp, "time");
                    if (seconds != 0) {

                        // Set the time
                        _setTime(seconds);

                        // Get the zone
                        char *z = JGetString(rsp, "zone");
                        if (z[0] != '\0') {
                            char zone[64];
                            strlcpy(zone, z, sizeof(zone));
                            // Only use the 3-letter abbrev
                            char *sep = strchr(zone, ',');
                            if (sep == NULL) {
                                zone[0] = '\0';
                            } else {
                                *sep = '\0';
                            }
                            zoneStillUnavailable = (memcmp(zone, "UTC", 3) == 0);
                            zoneForceRefresh = false;
                            strlcpy(curZone, zone, sizeof(curZone));
                            curZoneOffsetMins = JGetInt(rsp, "minutes");
                            strlcpy(curCountry, JGetString(rsp, "country"), sizeof(curCountry));
                            strlcpy(curArea, JGetString(rsp, "area"), sizeof(curArea));
                        }

                    }
                }
                NoteDeleteResponse(rsp);
            }
        }
    }

    // Adjust the base time by the number of seconds that have elapsed since
    // the base.
    JTIME adjustedTime = timeBaseSec + (int32_t) (((int64_t) nowMs - (int64_t) timeBaseSetAtMs) / 1000);

    // Done
    return adjustedTime;

}

/*!
  @brief Set suppression timer secs, returning the previous value.

  @param secs New suppression timer seconds.

  @returns Previous suppression timer seconds
 */
uint32_t NoteSetSTSecs(uint32_t secs)
{
    uint32_t prev = suppressionTimerSecs;
    suppressionTimerSecs = secs;
    return prev;
}

//**************************************************************************/
/*!
  @brief  Return local region info, if known. Returns true if valid.
  @param   retCountry (out) The country.
  @param   retArea (out) The area value.
  @param   retZone (out) The timezone value.
  @param   retZoneOffset (out) The timezone offset.
  @returns boolean indicating if the region information is valid.
*/
/**************************************************************************/
bool NoteRegion(char **retCountry, char **retArea, char **retZone, int *retZoneOffset)
{
    NoteTimeST();
    if (zoneStillUnavailable) {
        if (retCountry != NULL) {
            *retCountry = (char *) "";
        }
        if (retArea != NULL) {
            *retArea = (char *) "";
        }
        if (retZone != NULL) {
            *retZone = (char *) "UTC";
        }
        if (retZoneOffset != NULL) {
            *retZoneOffset = 0;
        }
        return false;
    }
    if (retCountry != NULL) {
        *retCountry = curCountry;
    }
    if (retArea != NULL) {
        *retArea = curArea;
    }
    if (retZone != NULL) {
        *retZone = curZone;
    }
    if (retZoneOffset != NULL) {
        *retZoneOffset = curZoneOffsetMins;
    }
    return true;
}

/*!
  @brief Return local time info, if known. Returns true if valid.

  @param retYear [out] Pointer to return year value.
  @param retMonth [out] Pointer to return month value.
  @param retDay [out] Pointer to return day value.
  @param retHour [out] Pointer to return hour value.
  @param retMinute [out] Pointer to return minute value.
  @param retSecond [out] Pointer to return seconds value.
  @param retWeekday [out] Pointer to return weekday string.
  @param retZone [in,out] If NULL, local time will be returned in UTC, else
         returns pointer to zone string

  @returns True if either the zone or DST have changed since last call, false
           otherwise.

  @note Only call this if time is valid.
 */
bool NoteLocalTimeST(uint16_t *retYear, uint8_t *retMonth, uint8_t *retDay,
                     uint8_t *retHour, uint8_t *retMinute, uint8_t *retSecond, char **retWeekday,
                     char **retZone)
{

    // Preset
    if (retYear != NULL) {
        *retYear = 0;
    }
    if (retMonth != NULL) {
        *retMonth = 0;
    }
    if (retDay != NULL) {
        *retDay = 0;
    }
    if (retHour != NULL) {
        *retHour = 0;
    }
    if (retMinute != NULL) {
        *retMinute = 0;
    }
    if (retSecond != NULL) {
        *retSecond = 0;
    }
    if (retWeekday != NULL) {
        *retWeekday = (char *) "";
    }
    if (retZone != NULL) {
        *retZone = (char *) "";
    }

    // Exit if time isn't yet valid
    if (!NoteTimeValidST()) {
        return false;
    }

    // Get the current time and region, and compute local time
    char  *currentZone;
    int currentOffsetMins;
    JTIME currentEpochTime = NoteTimeST();
    bool regionAvailable = NoteRegion(NULL, NULL, &currentZone, &currentOffsetMins);

    // Offset the epoch time by time zone offset
    currentEpochTime += currentOffsetMins * 60;

    // Convert from unix epoch time to local date/time
    int i, year, mon;
    int32_t days;
    uint32_t secs;
    secs = (uint32_t) currentEpochTime + ((70*365L+17)*86400LU);
    days = secs / 86400;
    if (retWeekday != NULL) {
        *retWeekday = (char *) dayNames[(days + 1) % 7];
    }
    for (year = days / 365; days < (i = _yToDays(year) + 365L * year); ) {
        --year;
    }
    days -= i;
    if (retYear != NULL) {
        *retYear = (uint16_t) year+1900;
    }
    short *pm = daysByMonth(year);
    for (mon = 12; days < pm[--mon]; ) ;
    if (retMonth != NULL) {
        *retMonth = (uint8_t) mon+1;
    }
    if (retDay != NULL) {
        *retDay = days - pm[mon] + 1;
    }
    secs = secs % 86400;
    uint8_t currentHour = (uint8_t) (secs / 3600);
    if (retHour != NULL) {
        *retHour = currentHour;
    }
    secs %= 3600;
    if (retMinute != NULL) {
        *retMinute = (uint8_t) (secs/60);
    }
    if (retSecond != NULL) {
        *retSecond = (uint8_t) (secs % 60);
    }
    if (retZone != NULL) {
        *retZone = currentZone;
    }

    // Determine whether or not we should refresh to check whether DST offset has changed, which
    // is between midnight and 3am local time (if available).
    static uint8_t lastHour;
    static bool lastInfoIsKnown = false;
    if (regionAvailable && lastInfoIsKnown) {
        if ((lastHour != 1 && currentHour == 1) || (lastHour != 2 && currentHour == 2) || (lastHour != 3 && currentHour == 3)) {
            zoneForceRefresh = true;
        }
    }
    lastInfoIsKnown = true;
    lastHour = currentHour;

    // Done
    return regionAvailable;

}

// Figure out how many days at start of the year
NOTE_C_STATIC int _yToDays(int year)
{
    int days = 0;
    if (0 < year) {
        days = (year - 1) / 4;
    } else if (year <= -4) {
        days = year / 4;
    }
    return days + daysByMonth(year)[0];
}

//**************************************************************************/
/*!
  @brief  See if the card location is valid.  If it is OFF, it is
  treated as valid.
  @param   errbuf (out) A buffer with any error information to return.
  @param   errbuflen The length of the error buffer.
  @returns boolean indicating if the location information is valid.
*/
/**************************************************************************/
bool NoteLocationValid(char *errbuf, uint32_t errbuflen)
{
    locationTimer = 0;
    locationValid = false;
    locationLastErr[0] = '\0';
    return NoteLocationValidST(errbuf, errbuflen);
}

//**************************************************************************/
/*!
  @brief  See if the card location is valid, time-suppressed.  If it is OFF,
  it is treated as valid.
  @param   errbuf (out) A buffer with any error information to return.
  @param   errbuflen The length of the error buffer.
  @returns boolean indicating if the location information is valid.
*/
/**************************************************************************/
bool NoteLocationValidST(char *errbuf, uint32_t errbuflen)
{

    // Preset
    if (errbuf != NULL) {
        strlcpy(errbuf, locationLastErr, errbuflen);
    }

    // If it was ever valid, return true
    if (locationValid) {
        locationLastErr[0] = '\0';
        if (errbuf != NULL) {
            *errbuf = '\0';
        }
        return true;
    }

    // If we haven't yet fetched the location, do so with a suppression
    // timer so that we don't hammer the module before it's had a chance to
    // connect to the gps to fetch location.
    if (!_timerExpiredSecs(&locationTimer, suppressionTimerSecs)) {
        return false;
    }

    // Request location from the card
    J *rsp = NoteRequestResponse(NoteNewRequest("card.location"));
    if (rsp == NULL) {
        return false;
    }

    // If valid, or the location mode is OFF, we're done
    if (!NoteResponseError(rsp) || strcmp(JGetString(rsp, "mode"), "off") == 0) {
        NoteDeleteResponse(rsp);
        locationValid = true;
        locationLastErr[0] = '\0';
        if (errbuf != NULL) {
            *errbuf = '\0';
        }
        return true;
    }

    // Remember the error for next iteration
    strlcpy(locationLastErr, JGetString(rsp, "err"), sizeof(locationLastErr));
    if (errbuf != NULL) {
        strlcpy(errbuf, locationLastErr, errbuflen);
    }
    NoteDeleteResponse(rsp);
    return false;

}

//**************************************************************************/
/*!
  @brief  Set a service environment variable default string.
  @param   variable The variable key.
  @param   buf The variable value.
  @returns boolean indicating if variable was set.
*/
/**************************************************************************/
bool NoteSetEnvDefault(const char *variable, char *buf)
{
    bool success = false;
    J *req = NoteNewRequest("env.default");
    if (req != NULL) {
        JAddStringToObject(req, "name", variable);
        JAddStringToObject(req, "text", buf);
        success = NoteRequest(req);
    }
    return success;
}

//**************************************************************************/
/*!
  @brief  Set a service environment variable default integer.
  @param   variable The variable key.
  @param   defaultVal The variable value.
  @returns boolean indicating if variable was set.
*/
/**************************************************************************/
bool NoteSetEnvDefaultInt(const char *variable, JINTEGER defaultVal)
{
    char buf[32];
    JItoA(defaultVal, buf);
    return NoteSetEnvDefault(variable, buf);
}

//**************************************************************************/
/*!
  @brief  Set a service environment variable default number.
  @param   variable The variable key.
  @param   defaultVal The variable value.
  @returns boolean indicating if variable was set.
*/
/**************************************************************************/
bool NoteSetEnvDefaultNumber(const char *variable, JNUMBER defaultVal)
{
    char buf[JNTOA_MAX];
    JNtoA(defaultVal, buf, -1);
    return NoteSetEnvDefault(variable, buf);
}

//**************************************************************************/
/*!
  @brief  Get a service environment variable number.
  @param   variable The variable key.
  @param   defaultVal The default variable value.
  @returns environment variable value.
*/
/**************************************************************************/
JNUMBER NoteGetEnvNumber(const char *variable, JNUMBER defaultVal)
{
    char buf[JNTOA_MAX], buf2[JNTOA_MAX];
    JNtoA(defaultVal, buf2, -1);
    NoteGetEnv(variable, buf2, buf, sizeof(buf));
    return JAtoN((const char*)buf, NULL);
}

//**************************************************************************/
/*!
  @brief  Get a service environment variable integer.
  @param   variable The variable key.
  @param   defaultVal The default variable value.
  @returns environment variable value.
*/
/**************************************************************************/
JINTEGER NoteGetEnvInt(const char *variable, JINTEGER defaultVal)
{
    char buf[32], buf2[32];
    JItoA(defaultVal, buf2);
    NoteGetEnv(variable, buf2, buf, sizeof(buf));
    return atoi(buf);
}

//**************************************************************************/
/*!
  @brief  Get a service environment variable.
  @param   variable The variable key.
  @param   defaultVal The default variable value, if the environment variable
           hasn't been set.
  @param   buf (out) The buffer in which to place the variable value. If the
           environment variable hasn't been set, defaultVal will be copied into
           buf.
  @param   buflen The length of the output buffer.
  @returns true if there is no error (JSON response with no explicit error)
*/
/**************************************************************************/
bool NoteGetEnv(const char *variable, const char *defaultVal, char *buf, uint32_t buflen)
{
    bool success = false;
    if (defaultVal == NULL) {
        buf[0] = '\0';
    } else {
        strlcpy(buf, defaultVal, buflen);
    }
    J *req = NoteNewRequest("env.get");
    if (req != NULL) {
        JAddStringToObject(req, "name", variable);
        J *rsp = NoteRequestResponse(req);
        if (rsp != NULL) {
            if (!NoteResponseError(rsp)) {
                success = true;
                char *val = JGetString(rsp, "text");
                if (val[0] != '\0') {
                    strlcpy(buf, val, buflen);
                }
            }
            NoteDeleteResponse(rsp);
        }
    }
    return success;
}

//**************************************************************************/
/*!
  @brief  Determine if the Notecard is connected to the network.
  @returns boolean. `true` if connected.
*/
/**************************************************************************/
bool NoteIsConnected(void)
{
    connectivityTimer = 0;
    return NoteIsConnectedST();
}

//**************************************************************************/
/*!
  @brief  Determine if the Notecard is connected to the network.
  @returns boolean. `true` if connected.
*/
/**************************************************************************/
bool NoteIsConnectedST(void)
{
    if (_timerExpiredSecs(&connectivityTimer, suppressionTimerSecs)) {
        J *rsp = NoteRequestResponse(NoteNewRequest("hub.status"));
        if (rsp != NULL) {
            if (!NoteResponseError(rsp)) {
                cardConnected = JGetBool(rsp, "connected");
            }
            NoteDeleteResponse(rsp);
        }
    }
    return cardConnected;
}

//**************************************************************************/
/*!
  @brief  Get Full Network Status via `hub.status`.
  @param  statusBuf (out) a buffer to populate with the status response.
  @param  statusBufLen The length of the status buffer
  @returns boolean. `true` if the status was retrieved.
*/
/**************************************************************************/
bool NoteGetNetStatus(char *statusBuf, int statusBufLen)
{
    bool success = false;
    statusBuf[0] = '\0';
    J *rsp = NoteRequestResponse(NoteNewRequest("hub.status"));
    if (rsp != NULL) {
        success = !NoteResponseError(rsp);
        if (success) {
            strlcpy(statusBuf, JGetString(rsp, "status"), statusBufLen);
        }
        NoteDeleteResponse(rsp);
    }
    return success;
}

//**************************************************************************/
/*!
  @brief  Make a `card.version` request
  @param  versionBuf (out) a buffer to populate with the version response.
  @param  versionBufLen The length of the version buffer
  @returns boolean. `true` if the version information was retrieved.
*/
/**************************************************************************/
bool NoteGetVersion(char *versionBuf, int versionBufLen)
{
    bool success = false;
    versionBuf[0] = '\0';
    J *rsp = NoteRequestResponse(NoteNewRequest("card.version"));
    if (rsp != NULL) {
        success = !NoteResponseError(rsp);
        if (success) {
            strlcpy(versionBuf, JGetString(rsp, "version"), versionBufLen);
        }
        NoteDeleteResponse(rsp);
    }
    return success;
}

//**************************************************************************/
/*!
  @brief  Make a `card.location` request
  @param  retLat (out) The retrieved latitude value.
  @param  retLon (out) The retrieved longitude value.
  @param  time (out) The retrieved time.
  @param  statusBuf (out) a buffer to populate with the location response.
  @param  statusBufLen The length of the location buffer
  @returns boolean. `true` if the location information was retrieved.
*/
/**************************************************************************/
bool NoteGetLocation(JNUMBER *retLat, JNUMBER *retLon, JTIME *time, char *statusBuf, int statusBufLen)
{
    bool locValid = false;
    if (statusBuf != NULL) {
        *statusBuf = '\0';
    }
    if (retLat != NULL) {
        *retLat = 0.0;
    }
    if (retLon != NULL) {
        *retLon = 0.0;
    }
    if (time != NULL) {
        *time = 0;
    }
    J *rsp = NoteRequestResponse(NoteNewRequest("card.location"));
    if (rsp != NULL) {
        if (statusBuf != NULL) {
            strlcpy(statusBuf, JGetString(rsp, "status"), statusBufLen);
        }
        if (JIsPresent(rsp, "lat") && JIsPresent(rsp, "lon")) {
            if (retLat != NULL) {
                *retLat = JGetNumber(rsp, "lat");
            }
            if (retLon != NULL) {
                *retLon = JGetNumber(rsp, "lon");
            }
            locValid = true;
        }
        JTIME seconds = JGetInt(rsp, "time");
        if (seconds != 0 && time != NULL) {
            *time = seconds;
        }
        NoteDeleteResponse(rsp);
    }
    return locValid;
}

//**************************************************************************/
/*!
  @brief  Set the card location to a static value.
  @param  lat The latitude value to set on the Notecard.
  @param  lon The longitude value to set on the Notecard.
  @returns boolean. `true` if the location information was set.
*/
/**************************************************************************/
bool NoteSetLocation(JNUMBER lat, JNUMBER lon)
{
    bool success = false;
    J *req = NoteNewRequest("card.location.mode");
    if (req != NULL) {
        JAddStringToObject(req, "mode", "fixed");
        JAddNumberToObject(req, "lat", lat);
        JAddNumberToObject(req, "lon", lon);
        success = NoteRequest(req);
    }
    return success;
}

//**************************************************************************/
/*!
  @brief  Clear last known Location.
  @returns boolean. `true` if the location information was cleared.
*/
/**************************************************************************/
bool NoteClearLocation(void)
{
    bool success = false;
    J *req = NoteNewRequest("card.location.mode");
    if (req != NULL) {
        JAddBoolToObject(req, "delete", true);
        success = NoteRequest(req);
    }
    return success;
}

//**************************************************************************/
/*!
  @brief  Get the current location Mode.
  @param  modeBuf (out) a buffer to populate with the location mode response.
  @param  modeBufLen The length of the mode buffer
  @returns boolean. `true` if the mode information was retrieved.
*/
/**************************************************************************/
bool NoteGetLocationMode(char *modeBuf, int modeBufLen)
{
    bool success = false;
    modeBuf[0] = '\0';
    J *rsp = NoteRequestResponse(NoteNewRequest("card.location.mode"));
    if (rsp != NULL) {
        success = !NoteResponseError(rsp);
        if (success) {
            strlcpy(modeBuf, JGetString(rsp, "mode"), modeBufLen);
        }
        NoteDeleteResponse(rsp);
    }
    return success;
}

//**************************************************************************/
/*!
  @brief  Make a `card.location.mode` request.
  @param  mode the Notecard location mode to set.
  @param  seconds The value of the `seconds` field for the request.
  @returns boolean. `true` if the mode was set.
*/
/**************************************************************************/
bool NoteSetLocationMode(const char *mode, uint32_t seconds)
{
    bool success = false;
    J *req = NoteNewRequest("card.location.mode");
    if (req != NULL) {
        if (mode[0] == '\0') {
            mode = "-";
        }
        JAddStringToObject(req, "mode", mode);
        JAddNumberToObject(req, "seconds", seconds);
        success = NoteRequest(req);
    }
    return success;
}

//**************************************************************************/
/*!
  @brief  Get the current service configuration information, unsuppressed.
  @param  productBuf (out) a buffer to populate with the ProductUID from the
  response.
  @param  productBufLen The length of the ProductUID buffer.
  @param  serviceBuf (out) a buffer to populate with the host url from the
  response.
  @param  serviceBufLen The length of the host buffer.
  @param  deviceBuf (out) a buffer to populate with the DeviceUID from the
  response.
  @param  deviceBufLen The length of the DeviceUID buffer.
  @param  snBuf (out) a buffer to populate with the Product Serial Number
  from the response.
  @param  snBufLen The length of the Serial Number buffer.
  @returns boolean. `true` if the service configuration was obtained.
*/
/**************************************************************************/
bool NoteGetServiceConfig(char *productBuf, int productBufLen, char *serviceBuf, int serviceBufLen, char *deviceBuf, int deviceBufLen, char *snBuf, int snBufLen)
{
    serviceConfigTimer = 0;
    return NoteGetServiceConfigST(productBuf, productBufLen, serviceBuf, serviceBufLen, deviceBuf, deviceBufLen, snBuf, snBufLen);
}

//**************************************************************************/
/*!
  @brief  Get the current service configuration information, with
  a supression timer.
  @param  productBuf (out) a buffer to populate with the ProductUID from the
  response.
  @param  productBufLen The length of the ProductUID buffer.
  @param  serviceBuf (out) a buffer to populate with the host url from the
  response.
  @param  serviceBufLen The length of the host buffer.
  @param  deviceBuf (out) a buffer to populate with the DeviceUID from the
  response.
  @param  deviceBufLen The length of the DeviceUID buffer.
  @param  snBuf (out) a buffer to populate with the Product Serial Number
  from the response.
  @param  snBufLen The length of the Serial Number buffer.
  @returns boolean. `true` if the service configuration was obtained.
*/
/**************************************************************************/
bool NoteGetServiceConfigST(char *productBuf, int productBufLen, char *serviceBuf, int serviceBufLen, char *deviceBuf, int deviceBufLen, char *snBuf, int snBufLen)
{
    bool success = false;

    // Use cache except for a rare refresh
    if (scProduct[0] == '\0' || scDevice[0] == '\0' || _timerExpiredSecs(&serviceConfigTimer, 4*60*60)) {
        J *rsp = NoteRequestResponse(NoteNewRequest("hub.get"));
        if (rsp != NULL) {
            success = !NoteResponseError(rsp);
            if (success) {
                strlcpy(scProduct, JGetString(rsp, "product"), sizeof(scProduct));
                strlcpy(scService, JGetString(rsp, "host"), sizeof(scService));
                strlcpy(scDevice, JGetString(rsp, "device"), sizeof(scDevice));
                strlcpy(scSN, JGetString(rsp, "sn"), sizeof(scSN));
            }
            NoteDeleteResponse(rsp);
        }
    } else {
        success = true;
    }

    // Done
    if (productBuf != NULL) {
        strlcpy(productBuf, scProduct, productBufLen);
    }
    if (serviceBuf != NULL) {
        strlcpy(serviceBuf, scService, serviceBufLen);
    }
    if (deviceBuf != NULL) {
        strlcpy(deviceBuf, scDevice, deviceBufLen);
    }
    if (snBuf != NULL) {
        strlcpy(snBuf, scSN, snBufLen);
    }
    return success;
}

//**************************************************************************/
/*!
  @brief  Get Status of the Notecard, unsuppressed.
  @param  statusBuf (out) a buffer to populate with the Notecard status
  from the response.
  @param  statusBufLen The length of the status buffer.
  @param  bootTime (out) The Notecard boot time.
  @param  retUSB (out) Whether the Notecard is powered over USB.
  @param  retSignals (out) Whether the Notecard has a network signal.
  @returns boolean. `true` if the card status was obtained.
*/
/**************************************************************************/
bool NoteGetStatus(char *statusBuf, int statusBufLen, JTIME *bootTime, bool *retUSB, bool *retSignals)
{
    statusTimer = 0;
    return NoteGetStatusST(statusBuf, statusBufLen, bootTime, retUSB, retSignals);
}

//**************************************************************************/
/*!
  @brief  Get Status of the Notecard, with a suppression timer.
  @param  statusBuf (out) a buffer to populate with the Notecard status
  from the response.
  @param  statusBufLen The length of the status buffer.
  @param  bootTime (out) The Notecard boot time.
  @param  retUSB (out) Whether the Notecard is powered over USB.
  @param  retSignals (out) Whether the Notecard has a network signal.
  @returns boolean. `true` if the card status was obtained.
*/
/**************************************************************************/
bool NoteGetStatusST(char *statusBuf, int statusBufLen, JTIME *bootTime, bool *retUSB, bool *retSignals)
{
    bool success = false;
    static char lastStatus[128] = {0};
    static JTIME lastBootTime = 0;
    static bool lastUSB = false;
    static bool lastSignals = false;

    // Refresh if it's time to do so
    if (_timerExpiredSecs(&statusTimer, suppressionTimerSecs)) {
        J *rsp = NoteRequestResponse(NoteNewRequest("card.status"));
        if (rsp != NULL) {
            success = !NoteResponseError(rsp);
            if (success) {
                strlcpy(lastStatus, JGetString(rsp, "status"), sizeof(lastStatus));
                lastBootTime = JGetInt(rsp, "time");
                lastUSB = JGetBool(rsp, "usb");
                if (JGetBool(rsp, "connected")) {
                    lastSignals = (JGetInt(rsp, "signals") > 0);
                } else {
                    lastSignals = false;
                }
            }
            NoteDeleteResponse(rsp);
        }
    } else {
        success = true;
    }

    // Done
    if (statusBuf != NULL) {
        strlcpy(statusBuf, lastStatus, statusBufLen);
    }
    if (bootTime != NULL) {
        *bootTime = lastBootTime;
    }
    if (retUSB != NULL) {
        *retUSB = lastUSB;
    }
    if (retSignals != NULL) {
        *retSignals = lastSignals;
    }

    return success;
}

/*!
  @brief Save the current state and use `card.attn` to set a host
         sleep interval.

  @param desc An optional binary payload to keep in memory while the host
         sleeps.
  @param seconds The duration to sleep.
  @param modes Optional list of additional `card.attn` modes.

  @returns `true` if the cmd is sent without error. The Notecard does not reply
            to commands so a `true` return value does not guarantee
            that the sleep request was received and processed by the Notecard.
*/
bool NotePayloadSaveAndSleep(NotePayloadDesc *desc, uint32_t seconds, const char *modes)
{
    char *stateB64 = NULL;

    // If specified, encode
    if (desc->data != NULL && desc->length != 0) {

        // Get length that it will be when converted to base64
        unsigned b64Len = JB64EncodeLen(desc->length);

        // Convert the state to base64
        stateB64 = _Malloc(b64Len);
        if (stateB64 == NULL) {
            return false;
        }

        // Encode the state buffer into a string
        JB64Encode(stateB64, (const char *) desc->data, desc->length);

    }

    // Sleep
    bool success = NoteSleep(stateB64, seconds, modes);

    // Free the temp buffer
    if (stateB64 != NULL) {
        _Free(stateB64);
    }

    // Done
    return success;

}

//**************************************************************************/
/*!
  @brief  Save the current state and use `card.attn` to set a host
  sleep interval.
  @param  stateb64 A base64 payload to keep in memory while the host sleeps.
  @param  seconds The duration to sleep.
  @param  modes Optional list of additional `card.attn` modes.
  @returns boolean. `true` if the cmd is sent without error. The Notecard
           does not reply to `cmd` so a `true` return value does not guarantee
           that the sleep request was received and processed by the Notecard.
*/
/**************************************************************************/
bool NoteSleep(char *stateb64, uint32_t seconds, const char *modes)
{
    bool success = false;

    // Trace
    NOTE_C_LOG_INFO("ABOUT TO SLEEP\n");

    // Use a Command rather than a Request so that the Notecard doesn't try to
    // send a response back to us (host), which would cause a communications
    // error on the Notecard end.
    NOTE_C_LOG_INFO("requesting sleep\n");
    J *req = NoteNewCommand("card.attn");
    if (req != NULL) {
        // Add the base64 item in a wonderful way that doesn't strdup the
        // huge string
        if (stateb64 != NULL) {
            J *stringReferenceItem = JCreateStringReference(stateb64);
            if (stringReferenceItem != NULL) {
                JAddItemToObject(req, "payload", stringReferenceItem);
            }
        }
        char modestr[64];
        strlcpy(modestr, "sleep", sizeof(modestr));
        if (modes != NULL) {
            strlcat(modestr, ",", sizeof(modestr));
            strlcat(modestr, modes, sizeof(modestr));
        }
        JAddStringToObject(req, "mode", modestr);
        JAddNumberToObject(req, "seconds", seconds);

        // Note that since we use cmd and not req a true return value from
        // NoteRequest means only that the cmd was sent without error.
        success = NoteRequest(req);
    }

    // Trace
    NOTE_C_LOG_ERROR("DIDN'T SLEEP\n");

    // Done
    return success;
}

//**************************************************************************/
/*!
  @brief  Wake the module by restoring state into a state buffer of a
  specified length, and fail if it isn't available or isn't that length.
  @param  stateLen A length of the state payload buffer to return to the host.
  @param  state (out) The in-memory payload to return to the host.
  @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
bool NoteWake(int stateLen, void *state)
{
    NotePayloadDesc desc;
    bool success = NotePayloadRetrieveAfterSleep(&desc);
    if (!success) {
        return false;
    }
    if (desc.length != (uint32_t)stateLen) {
        NotePayloadFree(&desc);
        return false;
    }
    memcpy(state, desc.data, stateLen);
    NotePayloadFree(&desc);
    return true;
}

/*!
  @brief  Wake the module by restoring state into a state buffer, returning
          its length, and fail if it isn't available.

  @param  desc [out] Payload descriptor to hold the retrieved.
          payload.

  @returns True if the request was successful, false otherwise.
 */
bool NotePayloadRetrieveAfterSleep(NotePayloadDesc *desc)
{

    // Initialize payload descriptor
    if (desc != NULL) {
        memset(desc, 0, sizeof(NotePayloadDesc));
    }

    // Send the Notecard a request to retrieve the saved state
    J *req = NoteNewRequest("card.attn");
    if (req == NULL) {
        return false;
    }
    JAddBoolToObject(req, "start", true);
    J *rsp = NoteRequestResponse(req);
    if (rsp == NULL) {
        return false;
    }
    if (NoteResponseError(rsp)) {
        NoteDeleteResponse(rsp);
        return false;
    }

    // Note the current time, if the field is present
    JTIME seconds = JGetInt(rsp, "time");
    if (seconds != 0) {
        _setTime(seconds);
    }

    // If we didn't expect any state to be restored, we're done
    if (desc == NULL) {
        NoteDeleteResponse(rsp);
        return true;
    }

    // Exit if no payload, knowing that we expected one
    char *payload = JGetString(rsp, "payload");
    if (payload[0] == '\0') {
        NoteDeleteResponse(rsp);
        return false;
    }

    // Allocate a buffer for the payload. (We can't decode in place because we
    // can't risk overwriting memory if the actual payload is even slightly
    // different.)
    uint32_t allocLen = JB64DecodeLen(payload);
    uint8_t *p = (uint8_t *) _Malloc(allocLen);
    if (p == NULL) {
        NoteDeleteResponse(rsp);
        return false;
    }
    uint32_t actualLen = (uint32_t) JB64Decode((char *)p, payload);

    // Fill out the payload descriptor
    desc->data = p;
    desc->alloc = allocLen;
    desc->length = actualLen;

    // State restored
    NOTE_C_LOG_INFO("AWAKENED SUCCESSFULLY\n");
    NoteDeleteResponse(rsp);
    return true;
}

//**************************************************************************/
/*!
  @brief  Restore the module to factory settings.
  @param  deleteConfigSettings Whether to delete current configuration
  settings on the Notecard during the reset.
  @returns boolean. `true` if reset was successful.
*/
/**************************************************************************/
bool NoteFactoryReset(bool deleteConfigSettings)
{
    bool success = false;

    // Perform the restore-to-factor-settings transaction
    J *req = NoteNewRequest("card.restore");
    if (req != NULL) {
        JAddBoolToObject(req, "delete", deleteConfigSettings);
        success = NoteRequest(req);
    }

    // Exit if it didn't work
    if (!success) {
        return false;
    }

    // Wait for serial to stabilize after it reboots
    _DelayMs(5000);
    NOTE_C_LOG_INFO("CARD RESTORED\n");

    // Reset the Notecard
    while (!NoteReset()) {
        _DelayMs(5000);
    }

    // Success
    return true;

}

//**************************************************************************/
/*!
  @brief  Set the Notecard Product UID.
  @param  productID The ProductUID to set on the Notecard.
  @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
bool NoteSetProductID(const char *productID)
{
    bool success = false;
    J *req = NoteNewRequest("hub.set");
    if (req != NULL) {
        if (productID[0] == '\0') {
            JAddStringToObject(req, "product", "-");
        } else {
            JAddStringToObject(req, "product", productID);
        }
        success = NoteRequest(req);
    }
    // Flush cache so that service config is re-fetched
    serviceConfigTimer = 0;
    return success;
}

//**************************************************************************/
/*!
  @brief  Set the Notecard Serial Number.
  @param  sn The Serial Number to set on the Notecard.
  @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
bool NoteSetSerialNumber(const char *sn)
{
    bool success = false;
    J *req = NoteNewRequest("hub.set");
    if (req != NULL) {
        if (sn[0] == '\0') {
            JAddStringToObject(req, "sn", "-");
        } else {
            JAddStringToObject(req, "sn", sn);
        }
        success = NoteRequest(req);
    }
    // Flush cache so that service config is re-fetched
    serviceConfigTimer = 0;
    return success;
}

//**************************************************************************/
/*!
  @brief  Set the Notecard upload mode and interval.
  @param   uploadMode The upload mode (for instance, `continuous`,
  or `periodic`).
  @param   uploadMinutes The max number of minutes to wait between Notehub
  uploads.
  @param   align Flag to specify that uploads should be grouped within the
  specified period, rather than counting the number of minutes
  from first modified.
  @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
bool NoteSetUploadMode(const char *uploadMode, int uploadMinutes, bool align)
{
    bool success = false;
    J *req = NoteNewRequest("hub.set");
    if (req != NULL) {
        JAddStringToObject(req, "mode", uploadMode);
        if (uploadMinutes != 0) {
            JAddNumberToObject(req, "outbound", uploadMinutes);
            // Setting this flag aligns uploads to be grouped within the period,
            // rather than counting the number of minutes from "first modified".
            JAddBoolToObject(req, "align", align);
        }
        success = NoteRequest(req);
    }
    return success;

}

//**************************************************************************/
/*!
  @brief  Set the Notecard upload mode, download mode and interval.
  @param   uploadMode The upload mode (for instance, `continuous`,
  or `periodic`).
  @param   uploadMinutes The max number of minutes to wait between Notehub
  uploads.
  @param   downloadMinutes The max number of minutes to wait between Notehub
  downloads.
  @param   align Flag to specify that uploads should be grouped within the
  specified period, rather than counting the number of minutes
  from first modified.
  @param   sync Setting this flag when mode is `continuous` causes an
  immediate sync when a file is modified on the service side.
  @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
bool NoteSetSyncMode(const char *uploadMode, int uploadMinutes, int downloadMinutes, bool align, bool sync)
{
    bool success = false;
    J *req = NoteNewRequest("hub.set");
    if (req != NULL) {
        JAddStringToObject(req, "mode", uploadMode);
        if (uploadMinutes != 0) {
            JAddNumberToObject(req, "outbound", uploadMinutes);
            // Setting this flag aligns uploads to be grouped within the period,
            // rather than counting the number of minutes from "first modified".
            JAddBoolToObject(req, "align", align);
        }
        if (downloadMinutes != 0) {
            JAddNumberToObject(req, "inbound", downloadMinutes);
        }
        // Setting this flag when mode is "continuous" causes an immediate sync
        // when a file is modified on the service side via HTTP
        JAddBoolToObject(req, "sync", sync);
        success = NoteRequest(req);
    }
    return success;

}

//**************************************************************************/
/*!
  @brief  Set a template for a Notefile.
  @param   target The Notefile on which to set a template.
  @param   body The template body.
  @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
bool NoteTemplate(const char *target, J *body)
{
    J *req = NoteNewRequest("note.template");
    if (req == NULL) {
        JDelete(body);
        return false;
    }
    JAddStringToObject(req, "file", target);
    JAddItemToObject(req, "body", body);
    return NoteRequest(req);
}

//**************************************************************************/
/*!
  @brief  Add a Note to a Notefile with `note.add`. Body is freed, regardless
          of success.
  @param   target The Notefile on which to set a template.
  @param   body The template body.
  @param   urgent Whether to perform an immediate sync after the Note
  is added.
  @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
bool NoteAdd(const char *target, J *body, bool urgent)
{

    // Initiate the request
    J *req = NoteNewRequest("note.add");
    if (req == NULL) {
        JDelete(body);
        return false;
    }

    // Add the target notefile and body to the request.  Note that
    // JAddItemToObject passes ownership of the object to req
    JAddStringToObject(req, "file", target);
    JAddItemToObject(req, "body", body);

    // Initiate sync NOW if it's urgent
    if (urgent) {
        JAddBoolToObject(req, "start", true);
    }

    // Perform the transaction
    return NoteRequest(req);

}

//**************************************************************************/
/*!
  @brief  Send a body to a route using an HTTP request.
  Body is freed, regardless of success.
  @param   method HTTP method to use, `get`, `post` or `put`.
  @param   routeAlias The Notehub Route alias.
  @param   notefile The Notefile name.
  @param   body The request JSON body.
  @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
bool NoteSendToRoute(const char *method, const char *routeAlias, char *notefile, J *body)
{

    // Create the new event
    J *req = NoteNewRequest("note.event");
    if (req == NULL) {
        JDelete(body);
        return false;
    }

    // Add the body item and the Notefile name
    JAddItemToObject(req, "body", body);
    JAddStringToObject(req, "file", notefile);

    // Perform the transaction to convert it to an event
    J *rsp = NoteRequestResponse(req);
    if (rsp == NULL) {
        return false;
    }

    // Exit if error
    if (NoteResponseError(rsp)) {
        NoteDeleteResponse(rsp);
        return false;
    }

    // Extract the event, which we'll use as the body for the next transaction
    body = JDetachItemFromObject(rsp, "body");
    NoteDeleteResponse(rsp);

    // Create the web transaction
    char request[32];
    strlcpy(request, "web.", sizeof(request));
    strlcat(request, method, sizeof(request));
    req = NoteNewRequest(request);
    if (req == NULL) {
        JDelete(body);
        return false;
    }

    // Add the body, and the alias of the route on the notehub, hard-wired here
    JAddItemToObject(req, "body", body);
    JAddStringToObject(req, "route", routeAlias);

    // Perform the transaction
    return NoteRequest(req);

}

//**************************************************************************/
/*!
  @brief  Get the voltage of the Notecard.
  @param   voltage (out) The Notecard voltage.
  @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
bool NoteGetVoltage(JNUMBER *voltage)
{
    bool success = false;
    *voltage = 0.0;
    J *rsp = NoteRequestResponse(NoteNewRequest("card.voltage"));
    if (rsp != NULL) {
        if (!NoteResponseError(rsp)) {
            *voltage = JGetNumber(rsp, "value");
            success = true;
        }
        NoteDeleteResponse(rsp);
    }
    return success;
}

//**************************************************************************/
/*!
  @brief  Get the temperature of the Notecard.
  @param   temp (out) The Notecard temperature.
  @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
bool NoteGetTemperature(JNUMBER *temp)
{
    bool success = false;
    *temp = 0.0;
    J *rsp = NoteRequestResponse(NoteNewRequest("card.temp"));
    if (rsp != NULL) {
        if (!NoteResponseError(rsp)) {
            *temp = JGetNumber(rsp, "value");
            success = true;
        }
        NoteDeleteResponse(rsp);
    }
    return success;
}

//**************************************************************************/
/*!
  @brief  Get the Notecard contact info.
  @param   nameBuf (out) The contact name buffer.
  @param   nameBufLen The length of the contact name buffer.
  @param   orgBuf (out) The contact organization buffer.
  @param   orgBufLen The length of the contact organization buffer.
  @param   roleBuf (out) The contact role buffer.
  @param   roleBufLen The length of the contact role buffer.
  @param   emailBuf (out) The contact email buffer.
  @param   emailBufLen The length of the contact email buffer.
  @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
bool NoteGetContact(char *nameBuf, int nameBufLen, char *orgBuf, int orgBufLen, char *roleBuf, int roleBufLen, char *emailBuf, int emailBufLen)
{
    bool success = false;

    if (nameBuf != NULL) {
        *nameBuf = '\0';
    }
    if (orgBuf != NULL) {
        *orgBuf = '\0';
    }
    if (roleBuf != NULL) {
        *roleBuf = '\0';
    }
    if (emailBuf != NULL) {
        *emailBuf = '\0';
    }

    J *rsp = NoteRequestResponse(NoteNewRequest("card.contact"));
    if (rsp != NULL) {
        success = !NoteResponseError(rsp);
        if (success) {
            if (nameBuf != NULL) {
                strlcpy(nameBuf, JGetString(rsp, "name"), nameBufLen);
            }
            if (orgBuf != NULL) {
                strlcpy(orgBuf, JGetString(rsp, "org"), orgBufLen);
            }
            if (roleBuf != NULL) {
                strlcpy(roleBuf, JGetString(rsp, "role"), roleBufLen);
            }
            if (emailBuf != NULL) {
                strlcpy(emailBuf, JGetString(rsp, "email"), emailBufLen);
            }
        }
        NoteDeleteResponse(rsp);
    }

    return success;
}

//**************************************************************************/
/*!
  @brief  Set the Notecard contact info.
  @param   nameBuf (in) The contact name buffer.
  @param   orgBuf (in) The contact organization buffer.
  @param   roleBuf (in) The contact role buffer.
  @param   emailBuf (in) The contact email buffer.
  @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
bool NoteSetContact(char *nameBuf, char *orgBuf, char *roleBuf, char *emailBuf)
{
    J *req = NoteNewRequest("card.contact");
    if (req == NULL) {
        return false;
    }
    if (nameBuf != NULL) {
        JAddStringToObject(req, "name", nameBuf);
    }
    if (orgBuf != NULL) {
        JAddStringToObject(req, "org", orgBuf);
    }
    if (roleBuf != NULL) {
        JAddStringToObject(req, "role", roleBuf);
    }
    if (emailBuf != NULL) {
        JAddStringToObject(req, "email", emailBuf);
    }
    return NoteRequest(req);
}

// A simple suppression timer based on a millisecond system clock. This clock is
// reset to 0 after boot and every wake. This returns true if the specified
// interval has elapsed, in seconds, and it updates the timer if it expires so
// that we will go another period.
NOTE_C_STATIC bool _timerExpiredSecs(uint32_t *timer, uint32_t periodSecs)
{
    bool expired = false;

    // If the timer went backward, we've expired regardless of the interval
    uint32_t prev = *timer;
    uint32_t now = _GetMs();
    if (now < prev) {
        prev = 0;
    }

    // If never initialized, it's expired
    if (prev == 0 || now >= prev+(periodSecs*1000)) {
        expired = true;
        *timer = now;
    }

    return expired;

}

//**************************************************************************/
/*!
  @brief  Periodically show Notecard sync status, returning `true` if
  something was displayed
  @param   pollFrequencyMs The time to wait, in milliseconds, between
  polling for sync status.
  @param   maxLevel The Synclog level to monitor.
  @returns boolean. `true` if a sync status message was displayed.
*/
/**************************************************************************/
bool NoteDebugSyncStatus(int pollFrequencyMs, int maxLevel)
{

    // Suppress polls so as to not overwhelm the notecard
    static uint32_t lastCommStatusPollMs = 0;
    if (lastCommStatusPollMs != 0 && (_GetMs() - lastCommStatusPollMs) < (uint32_t) pollFrequencyMs) {
        return false;
    }

    // Get the next queued status note
    J *req = NoteNewRequest("note.get");
    if (req == NULL) {
        return false;
    }
    JAddStringToObject(req, "file", "_synclog.qi");
    JAddBoolToObject(req, "delete", true);
    _noteSuspendTransactionDebug();
    J *rsp = NoteRequestResponse(req);
    _noteResumeTransactionDebug();
    if (rsp != NULL) {

        // If an error is returned, this means that no response is pending. Note
        // that it's expected that this might return either a "note does not
        // exist" error if there are no pending inbound notes, or a "file does
        // not exist" error if the inbound queue hasn't yet been created on the
        // service.
        if (NoteResponseError(rsp)) {
            // Only stop polling quickly if we don't receive anything
            lastCommStatusPollMs = _GetMs();
            NoteDeleteResponse(rsp);
            return false;
        }

        // Get the note's body
        J *body = JGetObject(rsp, "body");
        if (body != NULL) {
            if (maxLevel < 0 || JGetInt(body, "level") <= maxLevel) {
                _Debug("[SYNC] ");
                _Debug(JGetString(body, "subsystem"));
                _Debug(" ");
                _Debugln(JGetString(body, "text"));
            }
        }

        // Done with this response
        NoteDeleteResponse(rsp);
        return true;
    }

    return false;

}

// A general purpose, super nonperformant, but accurate way of figuring out how
// much memory is available, while exercising the allocator to ensure that it
// competently deals with adjacent block coalescing on free.
typedef struct objHeader_s {
    struct objHeader_s *prev;
    int length;
} objHeader;

//**************************************************************************/
/*!
  @brief  Obtain the amount of free memory available on the Notecard.
  @returns The number of bytes of memory available on the Notecard.
*/
/**************************************************************************/
uint32_t NoteMemAvailable(void)
{

    // Allocate progressively smaller and smaller chunks
    objHeader *lastObj = NULL;
    static long int maxsize = 35000;
    for (long int i=maxsize; i>=(long int)sizeof(objHeader); i=i-sizeof(objHeader)) {
        for (long int j=0;; j++) {
            objHeader *thisObj;
            thisObj = (objHeader *) _Malloc(i);
            if (thisObj == NULL) {
                break;
            }
            thisObj->prev = lastObj;
            thisObj->length = i;
            lastObj = thisObj;
        }
    }

    // Free the objects backwards
    long int lastLength = 0;
    long int lastLengthCount = 0;
    uint32_t total = 0;
    while (lastObj != NULL) {
        if (lastObj->length != lastLength) {
            lastLength = lastObj->length;
            lastLengthCount = 1;
        } else {
            lastLengthCount++;
        }
        objHeader *thisObj = lastObj;
        lastObj = lastObj->prev;
        total += thisObj->length;
        _Free(thisObj);
    }

    return total;

}

//**************************************************************************/
/*!
  @brief  Create a desc from a buffer, or initialize a new to-be-allocated desc
		  if the buf is null
  @param   desc Pointer to the payload descriptor
  @param   buf Pointer to the buffer to initialize the desc with (or NULL)
  @param   buflen Length of the buffer to initialize the desc with (or 0)
*/
/**************************************************************************/
void NotePayloadSet(NotePayloadDesc *desc, uint8_t *buf, uint32_t buflen)
{
    desc->data = buf;
    desc->alloc = buflen;
    desc->length = buflen;
}

//**************************************************************************/
/*!
  @brief  Free the payload pointed to by the descriptor
  @param   desc Pointer to the payload descriptor
*/
/**************************************************************************/
void NotePayloadFree(NotePayloadDesc *desc)
{
    if (desc->data != NULL) {
        _Free(desc->data);
    }
    desc->data = NULL;
    desc->alloc = 0;
    desc->length = 0;
}

//**************************************************************************/
/*!
  @brief  Add a segment to the specified binary with 4-character type code
  @param   desc Pointer to the payload descriptor
  @param   segtype Pointer to the 4-character payload identifier
  @param   data Pointer to the data segment to be appended
  @param   len Length of data segment
  @returns boolean. `true` if named segment is appended successfully
*/
/**************************************************************************/
bool NotePayloadAddSegment(NotePayloadDesc *desc, const char segtype[NP_SEGTYPE_LEN], void *data, uint32_t len)
{
    uint32_t alloc = 512;
    uint32_t hlen = len + NP_SEGHDR_LEN;
    if (hlen > alloc) {
        alloc += hlen;
    }
    if (desc->data == NULL) {
        uint8_t *base = _Malloc(alloc);
        if (base == NULL) {
            return false;
        }
        uint8_t *p = base;
        memcpy(p, segtype, NP_SEGTYPE_LEN);
        p += NP_SEGTYPE_LEN;
        memcpy(p, &len, NP_SEGLEN_LEN);
        p += NP_SEGLEN_LEN;
        memcpy(p, data, len);
        desc->data = base;
        desc->alloc = alloc;
        desc->length = hlen;
    } else if ((desc->alloc - desc->length) < hlen) {
        uint8_t *base = _Malloc(desc->alloc + alloc);
        if (base == NULL) {
            return false;
        }
        uint8_t *p = base;
        memcpy(p, desc->data, desc->length);
        p += desc->length;
        memcpy(p, segtype, NP_SEGTYPE_LEN);
        p += NP_SEGTYPE_LEN;
        memcpy(p, &len, NP_SEGLEN_LEN);
        p += NP_SEGLEN_LEN;
        memcpy(p, data, len);
        _Free(desc->data);
        desc->data = base;
        desc->alloc = desc->alloc + alloc;
        desc->length += hlen;
    } else {
        uint8_t *p = desc->data + desc->length;
        memcpy(p, segtype, NP_SEGTYPE_LEN);
        p += NP_SEGTYPE_LEN;
        memcpy(p, &len, NP_SEGLEN_LEN);
        p += NP_SEGLEN_LEN;
        memcpy(p, data, len);
        desc->length += hlen;
    }
    return true;
}

//**************************************************************************/
/*!
  @brief  Find and copy a named segment from a segmented payload
  @param   desc Pointer to the payload descriptor
  @param   segtype Pointer to the 4-character payload identifier
  @param   pdata Buffer to copy found segment to if return is true
  @param   len The expected length of the returned segment
  @returns boolean. `true` if named segment is restored successfully
*/
/**************************************************************************/
bool NotePayloadGetSegment(NotePayloadDesc *desc, const char segtype[NP_SEGTYPE_LEN], void *pdata, uint32_t len)
{
    uint8_t *data;
    uint32_t datalen;
    bool success = NotePayloadFindSegment(desc, segtype, &data, &datalen);
    if (success && datalen == len) {
        memcpy(pdata, data, len);
        return true;
    }
    return false;
}

//**************************************************************************/
/*!
  @brief  Find a named segment within a segmented payload
  @param   desc Pointer to the payload descriptor
  @param   segtype Pointer to the 4-character payload identifier
  @param   pdata Pointer to the found segment if return is true
  @param   plen Pointer to the returned segment length if return is true
  @returns boolean. `true` if named segment is found
*/
/**************************************************************************/
bool NotePayloadFindSegment(NotePayloadDesc *desc, const char segtype[NP_SEGTYPE_LEN], void *pdata, uint32_t *plen)
{

    // Preset returns
    * (uint8_t **) pdata = NULL;
    *plen = 0;

    // Locate the segment
    uint8_t *p = desc->data;
    uint32_t left = desc->length;
    if (p == NULL) {
        return false;
    }
    while (left >= NP_SEGHDR_LEN) {
        uint32_t len;
        memcpy(&len, p + NP_SEGTYPE_LEN, sizeof(len));
        if (memcmp(p, segtype, NP_SEGTYPE_LEN) == 0) {
            *plen = len;
            * (uint8_t **) pdata = p + NP_SEGHDR_LEN;
            return true;
        }
        len += NP_SEGHDR_LEN;
        p += len;
        if (len > left) {
            left = 0;
        } else {
            left -= len;
        }
    }
    return false;
}

void NoteTurboIO(bool enable)
{
    (void)enable;

    NOTE_C_LOG_WARN("NoteTurboIO is deprecated and has no effect.");
}
