// Copyright 2018 Inca Roads LLC.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "n_lib.h"

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
void NoteFnDebugMsg(const char *line) {
#ifndef NOTE_NODEBUG
    if (hookDebugOutput != NULL)
        hookDebugOutput(line);
#endif
}
void NoteFnDebug(const char *format, ...) {
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
long unsigned int NoteFnGetMs() {
    if (hookGetMs == NULL)
        return 0;
    return hookGetMs();
}
void NoteFnDelayMs(uint32_t ms) {
    if (hookDelayMs != NULL)
        hookDelayMs(ms);
}
void *NoteFnMalloc(size_t size) {
    if (hookMalloc == NULL)
        return NULL;
    return hookMalloc(size);
}
void NoteFnFree(void *p) {
    if (hookFree != NULL)
        hookFree(p);
}
void NoteFnLockI2C() {
    if (hookLockI2C != NULL)
        hookLockI2C();
}
void NoteFnUnlockI2C() {
    if (hookUnlockI2C != NULL)
        hookUnlockI2C();
}
void NoteFnLockNote() {
    if (hookLockNote != NULL)
        hookLockNote();
}
void NoteFnUnlockNote() {
    if (hookUnlockNote != NULL)
        hookUnlockNote();
}
void NoteFnSerialReset() {
    if (hookActiveInterface == interfaceSerial && hookSerialReset != NULL)
        hookSerialReset();
}
void NoteFnSerialTransmit(uint8_t *text, size_t len, bool flush) {
    if (hookActiveInterface == interfaceSerial && hookSerialTransmit != NULL)
        hookSerialTransmit(text, len, flush);
}
bool NoteFnSerialAvailable() {
    if (hookActiveInterface == interfaceSerial && hookSerialAvailable != NULL)
        return hookSerialAvailable();
    return false;
}
char NoteFnSerialReceive() {
    if (hookActiveInterface == interfaceSerial && hookSerialReceive != NULL)
        return hookSerialReceive();
    return 0;
}
void NoteFnI2CReset() {
    if (hookActiveInterface == interfaceI2C && hookI2CReset != NULL)
        hookI2CReset();
}
const char *NoteFnI2CTransmit(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size) {
    if (hookActiveInterface == interfaceI2C && hookI2CTransmit != NULL)
        return hookI2CTransmit(DevAddress, pBuffer, Size);
    return "i2c not active";
}
const char *NoteFnI2CReceive(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size, uint32_t *available) {
    if (hookActiveInterface == interfaceI2C && hookI2CReceive != NULL)
        return hookI2CReceive(DevAddress, pBuffer, Size, available);
    return "i2c not active";
}
uint32_t NoteFnI2CAddress() {
    if (i2cAddress == NOTE_I2C_MAX_DEFAULT)
        return 0x17;
    return i2cAddress;
}
uint32_t NoteFnI2CMax() {
    // Many Arduino libraries (such as ESP32) have a limit less than 32, so if the max isn't specified
    // we must assume the worst and segment the I2C messages into very tiny chunks.
    if (i2cMax == NOTE_I2C_MAX_DEFAULT)
        return 30;
    // Note design specs
    if (i2cMax > 127)
        i2cMax = 127;
    return i2cMax;
}

bool NoteFnNoteReset() {
    if (notecardReset == NULL)
        return "notecard not initialized";
    return notecardReset();
}
const char *NoteFnTransaction(char *json, char **jsonResponse) {
    if (notecardTransaction == NULL)
        return "notecard not initialized";
    return notecardTransaction(json, jsonResponse);
}
