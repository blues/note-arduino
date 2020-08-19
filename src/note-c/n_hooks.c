// Copyright 2018 Inca Roads LLC.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "n_lib.h"

// Show mallocs for debugging in very low mem environments
#define	NOTE_SHOW_MALLOC	false

// Which I/O port to use
#define interfaceNone       0
#define interfaceSerial     1
#define interfaceI2C        2

// Externalized Hooks
debugOutputFn hookDebugOutput = NULL;
mutexFn hookLockI2C = NULL;
mutexFn hookUnlockI2C = NULL;
mutexFn hookLockNote = NULL;
mutexFn hookUnlockNote = NULL;
mallocFn hookMalloc = NULL;
freeFn hookFree = NULL;
delayMsFn hookDelayMs = NULL;
getMsFn hookGetMs = NULL;
uint32_t hookActiveInterface = interfaceNone;

serialResetFn hookSerialReset = NULL;
serialTransmitFn hookSerialTransmit = NULL;
serialAvailableFn hookSerialAvailable = NULL;
serialReceiveFn hookSerialReceive = NULL;

uint32_t i2cAddress = 0;
uint32_t i2cMax = 0;
i2cResetFn hookI2CReset = NULL;
i2cTransmitFn hookI2CTransmit = NULL;
i2cReceiveFn hookI2CReceive = NULL;

// Internal hooks
typedef bool (*nNoteResetFn) (void);
typedef const char * (*nTransactionFn) (char *, char **);
static nNoteResetFn notecardReset = NULL;
static nTransactionFn notecardTransaction = NULL;

// Set the debug output hooks if they aren't already set
void NoteSetFnDefault(mallocFn mallocfn, freeFn freefn, delayMsFn delayfn, getMsFn millisfn) {
    if (hookMalloc == NULL)
        hookMalloc = mallocfn;
    if (hookFree == NULL)
        hookFree = freefn;
    if (hookDelayMs == NULL)
        hookDelayMs = delayfn;
    if (hookGetMs == NULL)
        hookGetMs = millisfn;
}
// Set the debug output hook
void NoteSetFn(mallocFn mallocfn, freeFn freefn, delayMsFn delayfn, getMsFn millisfn) {
    hookMalloc = mallocfn;
    hookFree = freefn;
    hookDelayMs = delayfn;
    hookGetMs = millisfn;
}
void NoteSetFnDebugOutput(debugOutputFn fn) {
    hookDebugOutput = fn;
}
bool NoteIsDebugOutputActive() {
    return hookDebugOutput != NULL;
}
void NoteSetFnMutex(mutexFn lockI2Cfn, mutexFn unlockI2Cfn, mutexFn lockNotefn, mutexFn unlockNotefn) {
    hookLockI2C = lockI2Cfn;
    hookUnlockI2C = unlockI2Cfn;
    hookLockNote = lockNotefn;
    hookUnlockNote = unlockNotefn;
}
void NoteSetFnSerial(serialResetFn resetfn, serialTransmitFn transmitfn, serialAvailableFn availfn, serialReceiveFn receivefn) {
    hookActiveInterface = interfaceSerial;

    hookSerialReset = resetfn;
    hookSerialTransmit = transmitfn;
    hookSerialAvailable = availfn;
    hookSerialReceive = receivefn;

    notecardReset = serialNoteReset;
    notecardTransaction = serialNoteTransaction;
}

void NoteSetFnI2C(uint32_t i2caddress, uint32_t i2cmax, i2cResetFn resetfn, i2cTransmitFn transmitfn, i2cReceiveFn receivefn) {
    i2cAddress = i2caddress;
    i2cMax = i2cmax;

    hookActiveInterface = interfaceI2C;

    hookI2CReset = resetfn;
    hookI2CTransmit = transmitfn;
    hookI2CReceive = receivefn;

    notecardReset = i2cNoteReset;
    notecardTransaction = i2cNoteTransaction;
}

// Runtime hook wrappers
void NoteDebugln(const char *line) {
	NoteDebug(line);
	NoteDebug(c_newline);
}

void NoteDebug(const char *line) {
#ifndef NOTE_NODEBUG
    if (hookDebugOutput != NULL)
        hookDebugOutput(line);
#endif
}
void NoteDebugf(const char *format, ...) {
#ifndef NOTE_NODEBUG
    if (hookDebugOutput != NULL) {
        char line[256];
        va_list args;
        va_start(args, format);
        vsnprintf(line, sizeof(line), format, args);
        va_end(args);
        hookDebugOutput(line);
    }
#endif
}
long unsigned int NoteGetMs() {
    if (hookGetMs == NULL)
        return 0;
    return hookGetMs();
}
void NoteDelayMs(uint32_t ms) {
    if (hookDelayMs != NULL)
        hookDelayMs(ms);
}
#if NOTE_SHOW_MALLOC
void *malloc_show(size_t len) {
	char str[10];
	itoa(len, str, 10);
    hookDebugOutput(str);
    hookDebugOutput("\r\n");
    return hookMalloc(len);
}
#endif
void *NoteMalloc(size_t size) {
    if (hookMalloc == NULL)
        return NULL;
#if NOTE_SHOW_MALLOC
	return malloc_show(size);
#else
    return hookMalloc(size);
#endif
}
void NoteFree(void *p) {
    if (hookFree != NULL)
        hookFree(p);
}
void NoteLockI2C() {
    if (hookLockI2C != NULL)
        hookLockI2C();
}
void NoteUnlockI2C() {
    if (hookUnlockI2C != NULL)
        hookUnlockI2C();
}
void NoteLockNote() {
    if (hookLockNote != NULL)
        hookLockNote();
}
void NoteUnlockNote() {
    if (hookUnlockNote != NULL)
        hookUnlockNote();
}
bool NoteSerialReset() {
    if (hookActiveInterface == interfaceSerial && hookSerialReset != NULL) {
        return hookSerialReset();
	}
	return false;
}
void NoteSerialTransmit(uint8_t *text, size_t len, bool flush) {
    if (hookActiveInterface == interfaceSerial && hookSerialTransmit != NULL) {
        hookSerialTransmit(text, len, flush);
	}
}
bool NoteSerialAvailable() {
    if (hookActiveInterface == interfaceSerial && hookSerialAvailable != NULL) {
        return hookSerialAvailable();
	}
    return false;
}
char NoteSerialReceive() {
    if (hookActiveInterface == interfaceSerial && hookSerialReceive != NULL) {
        return hookSerialReceive();
	}
    return 0;
}
bool NoteI2CReset() {
    if (hookActiveInterface == interfaceI2C && hookI2CReset != NULL) {
        return hookI2CReset();
	}
	return false;
}
const char *NoteI2CTransmit(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size) {
    if (hookActiveInterface == interfaceI2C && hookI2CTransmit != NULL) {
        return hookI2CTransmit(DevAddress, pBuffer, Size);
	}
    return "i2c not active";
}
const char *NoteI2CReceive(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size, uint32_t *available) {
    if (hookActiveInterface == interfaceI2C && hookI2CReceive != NULL) {
        return hookI2CReceive(DevAddress, pBuffer, Size, available);
	}
    return "i2c not active";
}
uint32_t NoteI2CAddress() {
    if (i2cAddress == NOTE_I2C_MAX_DEFAULT)
        return 0x17;
    return i2cAddress;
}
void NoteSetI2CAddress(uint32_t i2caddress) {
	i2cAddress = i2caddress;
}
uint32_t NoteI2CMax() {
    // Many Arduino libraries (such as ESP32) have a limit less than 32, so if the max isn't specified
    // we must assume the worst and segment the I2C messages into very tiny chunks.
    if (i2cMax == NOTE_I2C_MAX_DEFAULT)
        return 30;
    // Note design specs
    if (i2cMax > 127)
        i2cMax = 127;
    return i2cMax;
}

bool NoteHardReset() {
    if (notecardReset == NULL)
        return false;
    return notecardReset();
}
const char *NoteJSONTransaction(char *json, char **jsonResponse) {
    if (notecardTransaction == NULL)
        return "notecard not initialized";
    return notecardTransaction(json, jsonResponse);
}
