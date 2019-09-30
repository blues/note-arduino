// Copyright 2018 Inca Roads LLC.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#pragma once

#include <string.h>
#include "note.h"

// C-callable functions
#ifdef __cplusplus
extern "C" {
#endif

// How long to wait for the card for any given transaction
#define NOTECARD_TRANSACTION_TIMEOUT_SEC     10

// The notecard is a real-time device that has a fixed size interrupt buffer.  We can push data
// at it far, far faster than it can process it, therefore we push it in segments with a pause
// between each segment.
#define CARD_REQUEST_SEGMENT_MAX_LEN 1000
#define CARD_REQUEST_SEGMENT_DELAY_MS 250

// Transactions
const char *i2cNoteTransaction(char *json, char **jsonResponse);
bool i2cNoteReset(void);
const char *serialNoteTransaction(char *json, char **jsonResponse);
bool serialNoteReset(void);

// Hooks
void NoteFnLockNote(void);
void NoteFnUnlockNote(void);
void NoteFnSerialReset(void);
void NoteFnSerialTransmit(uint8_t *, size_t, bool);
bool NoteFnSerialAvailable(void);
char NoteFnSerialReceive(void);
void NoteFnI2CReset(void);
const char *NoteFnI2CTransmit(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size);
const char *NoteFnI2CReceive(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size, uint32_t *avail);
bool NoteFnNoteReset(void);
const char *NoteFnTransaction(char *json, char **jsonResponse);

// Readability wrappers.  Anything starting with _ is simply calling the wrapper function
#define _LockNote NoteFnLockNote
#define _UnlockNote NoteFnUnlockNote
#define _SerialReset NoteFnSerialReset
#define _SerialTransmit NoteFnSerialTransmit
#define _SerialAvailable NoteFnSerialAvailable
#define _SerialReceive NoteFnSerialReceive
#define _I2CReset NoteFnI2CReset
#define _I2CTransmit NoteFnI2CTransmit
#define _I2CReceive NoteFnI2CReceive
#define _NoteReset NoteFnNoteReset
#define _Transaction NoteFnTransaction
#define _Debug NoteFnDebug
#define _Malloc NoteFnMalloc
#define _Free NoteFnFree
#define _GetMs NoteFnGetMs
#define _DelayMs NoteFnDelayMs
#define _LockI2C NoteFnLockI2C
#define _UnlockI2C NoteFnUnlockI2C
#define _I2CAddress NoteFnI2CAddress
#define _I2CMax NoteFnI2CMax

// End of C-callable functions
#ifdef __cplusplus
}
#endif
