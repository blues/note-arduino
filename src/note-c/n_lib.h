/*!
 * @file n_lib.h
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

#pragma once

#include <stdint.h>
#include <string.h>

#include "note.h"

// C-callable functions
#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************/
/*!
    @brief  How long to wait for the card for any given transaction.
*/
/**************************************************************************/
#define CARD_INTER_TRANSACTION_TIMEOUT_SEC 30
#define CARD_INTRA_TRANSACTION_TIMEOUT_SEC  1

// The notecard is a real-time device that has a fixed size interrupt buffer.
// We can push data at it far, far faster than it can process it, therefore we
// push it in segments with a pause between each segment.

/**************************************************************************/
/*!
    @brief  The max length, in bytes, of each request segment when using I2C.
*/
/**************************************************************************/
#define CARD_REQUEST_I2C_SEGMENT_MAX_LEN 250
/**************************************************************************/
/*!
    @brief  The delay, in miliseconds, of each request when using I2C.
*/
/**************************************************************************/
#define CARD_REQUEST_I2C_SEGMENT_DELAY_MS 250
/**************************************************************************/
/*!
    @brief  The delay, in miliseconds, between each request chunk when using I2C.
*/
/**************************************************************************/
#define CARD_REQUEST_I2C_CHUNK_DELAY_MS 20
/**************************************************************************/
/*!
    @brief  The delay, in miliseconds, to wait after receiving a NACK I2C.
*/
/**************************************************************************/
#define CARD_REQUEST_I2C_NACK_WAIT_MS 1000
/**************************************************************************/
/*!
    @brief  The max length, in bytes, of each request segment when using Serial.
*/
/**************************************************************************/
#define CARD_REQUEST_SERIAL_SEGMENT_MAX_LEN 250
/**************************************************************************/
/*!
    @brief  The delay, in miliseconds, of each request when using Serial.
*/
/**************************************************************************/
#define CARD_REQUEST_SERIAL_SEGMENT_DELAY_MS 250
/**************************************************************************/
/*!
    @brief  The time, in miliseconds, to drain incoming messages.
*/
/**************************************************************************/
#define CARD_RESET_DRAIN_MS 500
/**************************************************************************/
/*!
    @brief  The number of times we will retry a request before giving up.
*/
/**************************************************************************/
#define CARD_REQUEST_RETRIES_ALLOWED 5
/**************************************************************************/
/*!
    @brief  The number of times we will retry getting in sync before giving up.
*/
/**************************************************************************/
#define CARD_RESET_SYNC_RETRIES 10
/**************************************************************************/
/*!
    @brief  Memory allocation chunk size.
*/
/**************************************************************************/
#ifdef NOTE_C_LOW_MEM
#define ALLOC_CHUNK 64
#else
#define ALLOC_CHUNK 128
#endif

#ifdef NOTE_C_LOW_MEM
#define NOTE_DISABLE_USER_AGENT
#endif

// Transactions
J *noteTransactionShouldLock(J *req, bool lockNotecard);
const char *i2cNoteTransaction(const char *request, size_t reqLen, char **response, uint32_t timeoutMs);
bool i2cNoteReset(void);
const char *serialNoteTransaction(const char *request, size_t reqLen, char **response, uint32_t timeoutMs);
bool serialNoteReset(void);
const char *i2cChunkedReceive(uint8_t *buffer, uint32_t *size, bool delay, uint32_t timeoutMs, uint32_t *available);
const char *i2cChunkedTransmit(uint8_t *buffer, uint32_t size, bool delay);
const char *serialChunkedReceive(uint8_t *buffer, uint32_t *size, bool delay, uint32_t timeoutMs, uint32_t *available);
const char *serialChunkedTransmit(uint8_t *buffer, uint32_t size, bool delay);

// Hooks
void noteLockNote(void);
void noteUnlockNote(void);
bool noteTransactionStart(uint32_t timeoutMs);
void noteTransactionStop(void);
const char *noteActiveInterface(void);
bool noteSerialReset(void);
void noteSerialTransmit(uint8_t *, size_t, bool);
bool noteSerialAvailable(void);
char noteSerialReceive(void);
bool noteI2CReset(uint16_t DevAddress);
const char *noteI2CTransmit(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size);
const char *noteI2CReceive(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size, uint32_t *avail);
bool noteHardReset(void);
const char *noteJSONTransaction(const char *request, size_t reqLen, char **response, uint32_t timeoutMs);
const char *noteChunkedReceive(uint8_t *buffer, uint32_t *size, bool delay, uint32_t timeoutMs, uint32_t *available);
const char *noteChunkedTransmit(uint8_t *buffer, uint32_t size, bool delay);
bool noteIsDebugOutputActive(void);

// Utilities
void n_htoa32(uint32_t n, char *p);
void n_htoa16(uint16_t n, unsigned char *p);
uint64_t n_atoh(char *p, int maxLen);

// COBS Helpers
uint32_t cobsDecode(uint8_t *ptr, uint32_t length, uint8_t eop, uint8_t *dst);
uint32_t cobsEncode(uint8_t *ptr, uint32_t length, uint8_t eop, uint8_t *dst);
uint32_t cobsEncodedLength(const uint8_t *ptr, uint32_t length);
uint32_t cobsEncodedMaxLength(uint32_t length);
uint32_t cobsGuaranteedFit(uint32_t bufLen);

// Turbo I/O mode
extern bool cardTurboIO;

// Constants, a global optimization to save static string memory
extern const char *c_null;
#define c_null_len 4

extern const char *c_false;
#define c_false_len 5

extern const char *c_true;
#define c_true_len 4

extern const char *c_nullstring;
#define c_nullstring_len 0

extern const char *c_newline;
#define c_newline_len 2

extern const char *c_mem;
#define c_mem_len 3

extern const char *c_iotimeout;
#define c_iotimeout_len 12

extern const char *c_err;
#define c_err_len 3

extern const char *c_req;
#define c_req_len 3

extern const char *c_cmd;
#define c_cmd_len 3

extern const char *c_bad;
#define c_bad_len 3

extern const char *c_iobad;
#define c_iobad_len 8

extern const char *c_ioerr;
#define c_ioerr_len 4

extern const char *c_unsupported;
#define c_unsupported_len 15

extern const char *c_badbinerr;
#define c_badbinerr_len 9

// Readability wrappers.  Anything starting with _ is simply calling the wrapper
// function.
#define _LockNote noteLockNote
#define _UnlockNote noteUnlockNote
#define _TransactionStart noteTransactionStart
#define _TransactionStop noteTransactionStop
#define _SerialReset noteSerialReset
#define _SerialTransmit noteSerialTransmit
#define _SerialAvailable noteSerialAvailable
#define _SerialReceive noteSerialReceive
#define _I2CReset noteI2CReset
#define _I2CTransmit noteI2CTransmit
#define _I2CReceive noteI2CReceive
#define _Reset noteHardReset
#define _Transaction noteJSONTransaction
#define _ChunkedReceive noteChunkedReceive
#define _ChunkedTransmit noteChunkedTransmit
#define _Malloc NoteMalloc
#define _Free NoteFree
#define _GetMs NoteGetMs
#define _DelayMs NoteDelayMs
#define _LockI2C NoteLockI2C
#define _UnlockI2C NoteUnlockI2C
#define _I2CAddress NoteI2CAddress
#define _I2CMax NoteI2CMax
#ifdef NOTE_NODEBUG
#define _Debug(x)
#define _Debugln(x)
#define _DebugIntln(x, y)
#define _DebugWithLevel(x, y)
#define _DebugWithLevelLn(x, y)
#else
#define _Debug(x) NoteDebug(x)
#define _Debugln(x) NoteDebugln(x)
#define _DebugIntln(x, y) NoteDebugIntln(x, y)
#define _DebugWithLevel(x, y) NoteDebugWithLevel(x, y)
#define _DebugWithLevelLn(x, y) NoteDebugWithLevelLn(x, y)
#endif

// End of C-callable functions
#ifdef __cplusplus
}
#endif
