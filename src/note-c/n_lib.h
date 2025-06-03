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

//**************************************************************************/
/*!
  @brief  `NOTE_C_TEST` enables testing of static functions.
*/
/**************************************************************************/
#ifdef NOTE_C_TEST
#include "test/include/test_static.h"
#define NOTE_C_STATIC
#else
#define NOTE_C_STATIC static
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
#endif // NOTE_C_LOW_MEM

#ifdef NOTE_C_LOW_MEM
#define NOTE_DISABLE_USER_AGENT
#endif // NOTE_C_LOW_MEM

// Transactions
void _noteResumeTransactionDebug(void);
void _noteSuspendTransactionDebug(void);
J *_noteTransactionShouldLock(J *req, bool lockNotecard);
const char *_i2cNoteTransaction(const char *request, size_t reqLen, char **response, uint32_t timeoutMs);
bool _i2cNoteReset(void);
const char *_serialNoteTransaction(const char *request, size_t reqLen, char **response, uint32_t timeoutMs);
bool _serialNoteReset(void);
const char *_i2cChunkedReceive(uint8_t *buffer, uint32_t *size, bool delay, uint32_t timeoutMs, uint32_t *available);
const char *_i2cChunkedTransmit(uint8_t *buffer, uint32_t size, bool delay);
const char *_i2cNoteChunkedReceive(uint8_t *buffer, uint32_t *size, bool delay, uint32_t timeoutMs, uint32_t *available);
const char *_i2cNoteChunkedTransmit(uint8_t *buffer, uint32_t size, bool delay);
const char *_serialChunkedReceive(uint8_t *buffer, uint32_t *size, bool delay, uint32_t timeoutMs, uint32_t *available);
const char *_serialChunkedTransmit(uint8_t *buffer, uint32_t size, bool delay);

// Hooks
void _noteLockNote(void);
void _noteUnlockNote(void);
bool _noteTransactionStart(uint32_t timeoutMs);
void _noteTransactionStop(void);
const char *_noteActiveInterface(void);
bool _noteSerialReset(void);
void _noteSerialTransmit(uint8_t *, size_t, bool);
bool _noteSerialAvailable(void);
char _noteSerialReceive(void);
bool _noteI2CReset(uint16_t DevAddress);
const char *_noteI2CTransmit(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size);
const char *_noteI2CReceive(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size, uint32_t *avail);
bool _noteHardReset(void);
const char *_noteJSONTransaction(const char *request, size_t reqLen, char **response, uint32_t timeoutMs);
const char *_noteChunkedReceive(uint8_t *buffer, uint32_t *size, bool delay, uint32_t timeoutMs, uint32_t *available);
const char *_noteChunkedTransmit(uint8_t *buffer, uint32_t size, bool delay);
bool _noteIsDebugOutputActive(void);

// Utilities
void _n_htoa32(uint32_t n, char *p);
void _n_htoa16(uint16_t n, unsigned char *p);
uint64_t _n_atoh(char *p, int maxLen);

// COBS Helpers
uint32_t _cobsDecode(uint8_t *ptr, uint32_t length, uint8_t eop, uint8_t *dst);
uint32_t _cobsEncode(uint8_t *ptr, uint32_t length, uint8_t eop, uint8_t *dst);
uint32_t _cobsEncodedLength(const uint8_t *ptr, uint32_t length);
uint32_t _cobsEncodedMaxLength(uint32_t length);
uint32_t _cobsGuaranteedFit(uint32_t bufLen);

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
#define _LockNote _noteLockNote
#define _UnlockNote _noteUnlockNote
#define _TransactionStart _noteTransactionStart
#define _TransactionStop _noteTransactionStop
#define _SerialReset _noteSerialReset
#define _SerialTransmit _noteSerialTransmit
#define _SerialAvailable _noteSerialAvailable
#define _SerialReceive _noteSerialReceive
#define _I2CReset _noteI2CReset
#define _I2CTransmit _noteI2CTransmit
#define _I2CReceive _noteI2CReceive
#define _Reset _noteHardReset
#define _Transaction _noteJSONTransaction
#define _ChunkedReceive _noteChunkedReceive
#define _ChunkedTransmit _noteChunkedTransmit
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
