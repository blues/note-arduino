/*!
 * @file n_hooks.c
 *
 * Hooks allow libraries dependent on note-c to provide platform- or
 * MCU-specific functions for common functions like I2C locking/unlocking,
 * memory allocation and freeing, delays, and communicating with the Notecard
 * over I2C and Serial. Using these hooks, note-c is able to manage Notecard
 * transaction logic, and defer to platform functionality, when needed.
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

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "n_lib.h"

//**************************************************************************/
/*!
  @brief  Show malloc operations for debugging in very low mem environments.
*/
/**************************************************************************/
#ifndef NOTE_C_SHOW_MALLOC
#define NOTE_C_SHOW_MALLOC  false
#endif
#if NOTE_C_SHOW_MALLOC
#include <string.h>
#endif

// Externalized Hooks
//**************************************************************************/
/*!
  @brief  Hook for the calling platform's debug interface, if any.
*/
/**************************************************************************/
debugOutputFn hookDebugOutput = NULL;
//**************************************************************************/
/*!
  @brief  Hook for the calling platform's I2C lock function.
*/
/**************************************************************************/
mutexFn hookLockI2C = NULL;
//**************************************************************************/
/*!
  @brief  Hook for the calling platform's I2C unlock function.
*/
/**************************************************************************/
mutexFn hookUnlockI2C = NULL;
//**************************************************************************/
/*!
  @brief  Hook for the calling platform's Notecard lock function.
*/
/**************************************************************************/
mutexFn hookLockNote = NULL;
//**************************************************************************/
/*!
  @brief  Hook for the calling platform's Notecard lock function.
*/
/**************************************************************************/
mutexFn hookUnlockNote = NULL;
//**************************************************************************/
/*!
  @brief  Hook for the calling platform's transaction initiation function.
*/
/**************************************************************************/
NOTE_C_STATIC txnStartFn hookTransactionStart = NULL;
//**************************************************************************/
/*!
  @brief  Hook for the calling platform's transaction completion function.
*/
/**************************************************************************/
NOTE_C_STATIC txnStopFn hookTransactionStop = NULL;
//**************************************************************************/
/*!
  @brief  Hook for the calling platform's memory allocation function.
*/
/**************************************************************************/
NOTE_C_STATIC mallocFn hookMalloc = NULL;
//**************************************************************************/
/*!
  @brief  Hook for the calling platform's memory free function.
*/
/**************************************************************************/
NOTE_C_STATIC freeFn hookFree = NULL;
//**************************************************************************/
/*!
  @brief  Hook for the calling platform's delay function.
*/
/**************************************************************************/
NOTE_C_STATIC delayMsFn hookDelayMs = NULL;
//**************************************************************************/
/*!
  @brief  Hook for the calling platform's millis timing function.
*/
/**************************************************************************/
NOTE_C_STATIC getMsFn hookGetMs = NULL;
//**************************************************************************/
/*!
  @brief  Hook for the calling platform's current active interface. Value is
  one of:
  - NOTE_C_INTERFACE_NONE (default)
  - NOTE_C_INTERFACE_SERIAL
  - NOTE_C_INTERFACE_I2C
*/
/**************************************************************************/
NOTE_C_STATIC volatile int hookActiveInterface = NOTE_C_INTERFACE_NONE;

//**************************************************************************/
/*!
  @brief  Hook for the calling platform's Serial reset function.
*/
/**************************************************************************/
NOTE_C_STATIC serialResetFn hookSerialReset = NULL;
//**************************************************************************/
/*!
  @brief  Hook for the calling platform's Serial transmit function.
*/
/**************************************************************************/
NOTE_C_STATIC serialTransmitFn hookSerialTransmit = NULL;
//**************************************************************************/
/*!
  @brief  Hook for the calling platform's Serial data available function.
*/
/**************************************************************************/
NOTE_C_STATIC serialAvailableFn hookSerialAvailable = NULL;
//**************************************************************************/
/*!
  @brief  Hook for the calling platform's Serial receive function.
*/
/**************************************************************************/
NOTE_C_STATIC serialReceiveFn hookSerialReceive = NULL;
//**************************************************************************/
/*!
  @brief  Hook for the calling platform's I2C address.
*/
/**************************************************************************/
NOTE_C_STATIC uint32_t i2cAddress = 0;
//**************************************************************************/
/*!
  @brief  Hook for the calling platform's I2C maximum segment size, in bytes.
*/
/**************************************************************************/
NOTE_C_STATIC uint32_t i2cMax = 0;
//**************************************************************************/
/*!
  @brief  Hook for the calling platform's I2C reset function.
*/
/**************************************************************************/
NOTE_C_STATIC i2cResetFn hookI2CReset = NULL;
//**************************************************************************/
/*!
  @brief  Hook for the calling platform's transmit function.
*/
/**************************************************************************/
NOTE_C_STATIC i2cTransmitFn hookI2CTransmit = NULL;
//**************************************************************************/
/*!
  @brief  Hook for the calling platform's I2C receive function.
*/
/**************************************************************************/
NOTE_C_STATIC i2cReceiveFn hookI2CReceive = NULL;
//**************************************************************************/
/*!
  @brief Variable used to determine the runtime logging level
*/
/**************************************************************************/
#ifndef NOTE_NODEBUG
NOTE_C_STATIC int noteLogLevel = NOTE_C_LOG_LEVEL;
#endif

// Internal hooks
typedef bool (*nNoteResetFn) (void);
typedef const char * (*nTransactionFn) (const char *, size_t, char **, uint32_t);
typedef const char * (*nReceiveFn) (uint8_t *, uint32_t *, bool, uint32_t, uint32_t *);
typedef const char * (*nTransmitFn) (uint8_t *, uint32_t, bool);
NOTE_C_STATIC nNoteResetFn notecardReset = NULL;
NOTE_C_STATIC nTransactionFn notecardTransaction = NULL;
NOTE_C_STATIC nReceiveFn notecardChunkedReceive = NULL;
NOTE_C_STATIC nTransmitFn notecardChunkedTransmit = NULL;

//**************************************************************************/
/*!
 @brief  Get the active, platform-specific communications method

 @returns  The active interface. One of:
   - NOTE_C_INTERFACE_NONE (default)
   - NOTE_C_INTERFACE_SERIAL
   - NOTE_C_INTERFACE_I2C
 */
/**************************************************************************/
int NoteGetActiveInterface(void)
{
    return hookActiveInterface;
}

NOTE_C_STATIC void _noteSetActiveInterface(int interface)
{
    hookActiveInterface = interface;

    switch (interface) {
    case NOTE_C_INTERFACE_SERIAL:
        notecardReset = _serialNoteReset;
        notecardTransaction = _serialNoteTransaction;
        notecardChunkedReceive = _serialChunkedReceive;
        notecardChunkedTransmit = _serialChunkedTransmit;
        break;
    case NOTE_C_INTERFACE_I2C:
        notecardReset = _i2cNoteReset;
        notecardTransaction = _i2cNoteTransaction;
        notecardChunkedReceive = _i2cNoteChunkedReceive;
        notecardChunkedTransmit = _i2cNoteChunkedTransmit;
        break;
    default:
        hookActiveInterface = NOTE_C_INTERFACE_NONE; // unrecognized interfaces are disabled
        notecardReset = NULL;
        notecardTransaction = NULL;
        notecardChunkedReceive = NULL;
        notecardChunkedTransmit = NULL;
        break;
    }
}

//**************************************************************************/
/*!
 @brief  Set the desired, platform-specific communications method
 @param   interface  The desired interface to use. One of:
   - NOTE_C_INTERFACE_NONE (default)
   - NOTE_C_INTERFACE_SERIAL
   - NOTE_C_INTERFACE_I2C
 */
/**************************************************************************/
void NoteSetActiveInterface(int interface)
{
    _LockNote();
    _noteSetActiveInterface(interface);
    _UnlockNote();
}

//**************************************************************************/
/*!
 @brief  Set the default memory and timing hooks if they aren't already set
 @param   mallocfn  The default memory allocation `malloc`
 function to use.
 @param   freefn  The default memory free
 function to use.
 @param   delayfn  The default delay function to use.
 @param   millisfn  The default 'millis' function to use.
 */
//**************************************************************************/
void NoteSetFnDefault(mallocFn mallocfn, freeFn freefn, delayMsFn delayfn, getMsFn millisfn)
{
    _LockNote();
    if (hookMalloc == NULL) {
        hookMalloc = mallocfn;
    }
    if (hookFree == NULL) {
        hookFree = freefn;
    }
    if (hookDelayMs == NULL) {
        hookDelayMs = delayfn;
    }
    if (hookGetMs == NULL) {
        hookGetMs = millisfn;
    }
    _UnlockNote();
}

//**************************************************************************/
/*!
 @brief Set the platform-specific memory and timing hooks.

 @param mallocHook The platform-specific memory allocation function (i.e.
        `malloc`).
 @param freeHook The platform-specific memory free function (i.e. `free`).
 @param delayMsHook The platform-specific millisecond delay function.
 @param getMsHook The platform-specific millisecond counter function.
*/
//**************************************************************************/
void NoteSetFn(mallocFn mallocHook, freeFn freeHook, delayMsFn delayMsHook,
               getMsFn getMsHook)
{
    _LockNote();
    hookMalloc = mallocHook;
    hookFree = freeHook;
    hookDelayMs = delayMsHook;
    hookGetMs = getMsHook;
    _UnlockNote();
}

//**************************************************************************/
/*!
  @brief  Set the platform-specific debug output function.
  @param   fn  A function pointer to call for debug output.
*/
/**************************************************************************/
void NoteSetFnDebugOutput(debugOutputFn fn)
{
    _LockNote();
    hookDebugOutput = fn;
    _UnlockNote();
}

//**************************************************************************/
/*!
  @brief  Determine if a debug output function has been set.
  @returns  A boolean indicating whether a debug ouput function was
  provided.
*/
/**************************************************************************/
bool _noteIsDebugOutputActive(void)
{
    return hookDebugOutput != NULL;
}

//**************************************************************************/
/*!
  @brief  Set the platform-specific transaction initiation/completion fn's
  @param   startFn  The platform-specific transaction initiation function to use.
  @param   stopFn  The platform-specific transaction completion function to use.
  to use.
*/
/**************************************************************************/
void NoteSetFnTransaction(txnStartFn startFn, txnStopFn stopFn)
{
    _LockNote();
    hookTransactionStart = startFn;
    hookTransactionStop = stopFn;
    _UnlockNote();
}

//**************************************************************************/
/*!
  @brief  Set the platform-specific mutex functions for I2C and the
  Notecard.
  @param   lockI2Cfn  The platform-specific I2C lock function to use.
  @param   unlockI2Cfn  The platform-specific I2C unlock function to use.
  @param   lockNotefn  The platform-specific Notecard lock function to use.
  @param   unlockNotefn  The platform-specific Notecard unlock function
  to use.
*/
/**************************************************************************/
void NoteSetFnMutex(mutexFn lockI2Cfn, mutexFn unlockI2Cfn, mutexFn lockNotefn, mutexFn unlockNotefn)
{
    hookLockI2C = lockI2Cfn;
    hookUnlockI2C = unlockI2Cfn;
    hookLockNote = lockNotefn;
    hookUnlockNote = unlockNotefn;
}

/*!
 @brief Set the platform-specific mutex functions for I2C.

 @param lockI2Cfn The platform-specific I2C lock function.
 @param unlockI2Cfn The platform-specific I2C unlock function.
 */
void NoteSetFnI2CMutex(mutexFn lockI2Cfn, mutexFn unlockI2Cfn)
{
    hookLockI2C = lockI2Cfn;
    hookUnlockI2C = unlockI2Cfn;
}

/*!
 @brief  Set the platform-specific mutex functions for the Notecard.

 @param lockFn The platform-specific Notecard lock function.
 @param unlockFn The platform-specific Notecard unlock function.
 */
void NoteSetFnNoteMutex(mutexFn lockFn, mutexFn unlockFn)
{
    hookLockNote = lockFn;
    hookUnlockNote = unlockFn;
}

/*!
 @brief Set the platform-specific hooks for communicating with the Notecard over
        serial.

 @param resetFn The platform-specific serial reset function.
 @param transmitFn The platform-specific serial transmit function.
 @param availFn The platform-specific serial available function.
 @param receiveFn The platform-specific serial receive function.
*/
void NoteSetFnSerial(serialResetFn resetFn, serialTransmitFn transmitFn,
                     serialAvailableFn availFn, serialReceiveFn receiveFn)
{
    _LockNote();

    hookSerialReset = resetFn;
    hookSerialTransmit = transmitFn;
    hookSerialAvailable = availFn;
    hookSerialReceive = receiveFn;

    _noteSetActiveInterface(NOTE_C_INTERFACE_SERIAL);

    _UnlockNote();
}

/*!
 * @brief Set the default Serial hooks if they aren't already set
 * @param   resetFn The platform-specific serial reset function.
 * @param   transmitFn The platform-specific serial transmit function.
 * @param   availFn The platform-specific serial available function.
 * @param   receiveFn The platform-specific serial receive function.
 */
void NoteSetFnSerialDefault(serialResetFn resetFn, serialTransmitFn transmitFn,
                            serialAvailableFn availFn, serialReceiveFn receiveFn)
{
    _LockNote();

    if (hookSerialReset == NULL) {
        hookSerialReset = resetFn;
    }
    if (hookSerialTransmit == NULL) {
        hookSerialTransmit = transmitFn;
    }
    if (hookSerialAvailable == NULL) {
        hookSerialAvailable = availFn;
    }
    if (hookSerialReceive == NULL) {
        hookSerialReceive = receiveFn;
    }

    if (hookActiveInterface == NOTE_C_INTERFACE_NONE) {
        _noteSetActiveInterface(NOTE_C_INTERFACE_SERIAL);
    }

    _UnlockNote();
}

/*!
 @brief Set the platform-specific hooks for communicating with the Notecard over
        I2C, as well as the I2C address of the Notecard and maximum transmission
        size.

  @param notecardAddr The I2C address of the Notecard. Pass 0 to use the default
         address.
  @param maxTransmitSize The max number of bytes to send to the Notecard in a
         single I2C segment. Pass 0 to use the default maximum transmission
         size.
  @param resetFn The platform-specific I2C reset function.
  @param transmitFn The platform-specific I2C transmit function.
  @param receiveFn The platform-specific I2C receive function.
 */
void NoteSetFnI2C(uint32_t notecardAddr, uint32_t maxTransmitSize,
                  i2cResetFn resetFn, i2cTransmitFn transmitFn,
                  i2cReceiveFn receiveFn)
{
    _LockNote();

    i2cAddress = notecardAddr;
    i2cMax = maxTransmitSize;

    hookI2CReset = resetFn;
    hookI2CTransmit = transmitFn;
    hookI2CReceive = receiveFn;

    _noteSetActiveInterface(NOTE_C_INTERFACE_I2C);

    _UnlockNote();
}

/*!
 * @brief Set the default I2C hooks if they aren't already set
 * @param   notecardAddr The I2C address of the Notecard. Pass 0 to use the default
 *         address.
 * @param   maxTransmitSize The max number of bytes to send to the Notecard in a
 *         single I2C segment. Pass 0 to use the default maximum transmission
 *         size.
 * @param   resetFn The platform-specific I2C reset function.
 * @param   transmitFn The platform-specific I2C transmit function.
 * @param   receiveFn The platform-specific I2C receive function.
 */
void NoteSetFnI2cDefault(uint32_t notecardAddr, uint32_t maxTransmitSize,
                         i2cResetFn resetFn, i2cTransmitFn transmitFn,
                         i2cReceiveFn receiveFn)
{
    _LockNote();

    if (i2cAddress == 0) {
        i2cAddress = notecardAddr;
    }
    if (i2cMax == 0) {
        i2cMax = maxTransmitSize;
    }
    if (hookI2CReset == NULL) {
        hookI2CReset = resetFn;
    }
    if (hookI2CTransmit == NULL) {
        hookI2CTransmit = transmitFn;
    }
    if (hookI2CReceive == NULL) {
        hookI2CReceive = receiveFn;
    }

    if (hookActiveInterface == NOTE_C_INTERFACE_NONE) {
        _noteSetActiveInterface(NOTE_C_INTERFACE_I2C);
    }

    _UnlockNote();
}

//**************************************************************************/
/*!
  @brief  Set the platform-specific communications method to be disabled
*/
/**************************************************************************/
void NoteSetFnDisabled(void)
{

    _LockNote();
    _noteSetActiveInterface(NOTE_C_INTERFACE_NONE);
    _UnlockNote();

}

// Runtime hook wrappers

//**************************************************************************/
/*!
  @brief  Set the log level for the _DebugWithLevel function.
  @param   level  The log level to set.
*/
/**************************************************************************/
void NoteSetLogLevel(int level)
{
#ifndef NOTE_NODEBUG
    noteLogLevel = level;
#else
    (void)level;
#endif
}

//**************************************************************************/
/*!
  @brief  Write a number to the debug stream and output a newline.
  @param   line  A debug string for output.
  @param n The number to write.
*/
/**************************************************************************/
void NoteDebugIntln(const char *line, int n)
{
    if (line != NULL) {
        _Debug(line);
    }
    char str[16];
    JItoA(n, str);
    _Debug(str);
    _Debug(c_newline);
}

//**************************************************************************/
/*!
  @brief  Write text to the debug stream and output a newline.
  @param   line  A debug string for output.
*/
/**************************************************************************/
void NoteDebugln(const char *line)
{
    _Debug(line);
    _Debug(c_newline);
}

//**************************************************************************/
/*!
  @brief  Write to the debug stream.
  @param   line  A debug string for output.
*/
/**************************************************************************/
void NoteDebug(const char *line)
{
#ifndef NOTE_NODEBUG
    if (_noteIsDebugOutputActive()) {
        hookDebugOutput(line);
    }
#else
    (void)line;
#endif // !NOTE_NODEBUG
}

//**************************************************************************/
/*!
  @brief       Write the message to the debug stream, if the level is less than
               or equal to NOTE_C_LOG_LEVEL. Otherwise, the message is dropped.
  @param level The log level of the message. See the NOTE_C_LOG_LEVEL_* macros
               in note.h for possible values.
  @param msg   The debug message.
*/
/**************************************************************************/
void NoteDebugWithLevel(uint8_t level, const char *msg)
{
#ifndef NOTE_NODEBUG
    if (level > noteLogLevel) {
        return;
    }

    _Debug(msg);
#else
    (void)level;
    (void)msg;
#endif // !NOTE_NODEBUG
}

//**************************************************************************/
/*!
  @brief       Same as NoteDebugWithLevel, but add a newline at the end.
  @param level The log level of the message. See the NOTE_C_LOG_LEVEL_* macros
               in note.h for possible values.
  @param msg   The debug message.
*/
/**************************************************************************/
void NoteDebugWithLevelLn(uint8_t level, const char *msg)
{
    _DebugWithLevel(level, msg);
    _DebugWithLevel(level, c_newline);
}

//**************************************************************************/
/*!
  @brief  Get the current milliseconds value from the platform-specific
  hook.
  @returns  The current milliseconds value.
*/
/**************************************************************************/
uint32_t NoteGetMs(void)
{
    if (hookGetMs == NULL) {
        return 0;
    }
    return hookGetMs();
}

//**************************************************************************/
/*!
  @brief  Delay milliseconds using the platform-specific hook.
  @param   ms the milliseconds delay value.
*/
/**************************************************************************/
void NoteDelayMs(uint32_t ms)
{
    if (hookDelayMs != NULL) {
        hookDelayMs(ms);
    }
}

#ifndef NOTE_C_LOW_MEM

//**************************************************************************/
/*!
  @brief  Convert number to a hex string
  @param  n the number
  @param  p the buffer to return it into
  */
/**************************************************************************/
void _n_htoa32(uint32_t n, char *p)
{
    for (int i=0; i<8; i++) {
        uint32_t nibble = (n >> 28) & 0xff;
        n = n << 4;
        if (nibble >= 10) {
            *p++ = 'A' + (nibble-10);
        } else {
            *p++ = '0' + nibble;
        }
    }
    *p = '\0';
}

#if NOTE_C_SHOW_MALLOC
static_assert(sizeof(void *) == sizeof(uint32_t), "Pointer size mismatch");

NOTE_C_STATIC void _n_ptoa32(void *ptr, char *str)
{
    _n_htoa32((uint32_t)ptr, str);
}

#endif  // NOTE_C_SHOW_MALLOC
#endif  // !NOTE_C_LOW_MEM

//**************************************************************************/
/*!
  @brief  Allocate a memory chunk using the platform-specific hook.
  @param   size the number of bytes to allocate.
*/
/**************************************************************************/
void *NoteMalloc(size_t size)
{
    if (hookMalloc == NULL) {
        return NULL;
    }
    void *p = hookMalloc(size);
#if NOTE_C_SHOW_MALLOC && !defined(NOTE_C_LOW_MEM)
    if (_noteIsDebugOutputActive()) {
        hookDebugOutput("malloc ");
        // Convert the size to a string and print
        char str[16];
        JItoA(size, str);
        hookDebugOutput(str);
        if (p == NULL) {
            hookDebugOutput(" FAIL");
        } else {
            hookDebugOutput(" ");
            // Convert the pointer to a string and print
            _n_ptoa32(p, str);
            hookDebugOutput(str);
        }
    }
#endif // NOTE_C_SHOW_MALLOC && !defined(NOTE_C_LOW_MEM)
    return p;
}

//**************************************************************************/
/*!
  @brief  Free memory using the platform-specific hook.
  @param   p A pointer to the memory address to free.
*/
/**************************************************************************/
void NoteFree(void *p)
{
    if (hookFree != NULL) {
#if NOTE_C_SHOW_MALLOC && !defined(NOTE_C_LOW_MEM)
        if (_noteIsDebugOutputActive()) {
            hookDebugOutput("free ");
            // Convert the pointer to a string and print
            char str[16];
            _n_ptoa32(p, str);
            hookDebugOutput(str);
        }
#endif // NOTE_C_SHOW_MALLOC && !defined(NOTE_C_LOW_MEM)
        hookFree(p);
    }
}

//**************************************************************************/
/*!
  @brief  Lock the I2C bus using the platform-specific hook.
*/
/**************************************************************************/
void NoteLockI2C(void)
{
    if (hookLockI2C != NULL) {
        hookLockI2C();
    }
}

//**************************************************************************/
/*!
  @brief  Unlock the I2C bus using the platform-specific hook.
*/
/**************************************************************************/
void NoteUnlockI2C(void)
{
    if (hookUnlockI2C != NULL) {
        hookUnlockI2C();
    }
}

//**************************************************************************/
/*!
  @brief  Lock the Notecard using the platform-specific hook.
*/
/**************************************************************************/
void _noteLockNote(void)
{
    if (hookLockNote != NULL) {
        hookLockNote();
    }
}

//**************************************************************************/
/*!
  @brief  Unlock the Notecard using the platform-specific hook.
*/
/**************************************************************************/
void _noteUnlockNote(void)
{
    if (hookUnlockNote != NULL) {
        hookUnlockNote();
    }
}

//**************************************************************************/
/*!
  @brief  Indicate that we're initiating a transaction using the platform-specific hook.
*/
/**************************************************************************/
bool _noteTransactionStart(uint32_t timeoutMs)
{
    if (hookTransactionStart != NULL) {
        return hookTransactionStart(timeoutMs);
    }
    return true;
}

//**************************************************************************/
/*!
  @brief  Indicate that we've completed a transaction using the platform-specific hook.
*/
/**************************************************************************/
void _noteTransactionStop(void)
{
    if (hookTransactionStop != NULL) {
        hookTransactionStop();
    }
}

/*!
 @brief Get the platform-specific debug output function.
 @param fn Pointer to store the debug output function pointer.
*/
void NoteGetFnDebugOutput(debugOutputFn *fn)
{
    if (fn != NULL) {
        *fn = hookDebugOutput;
    }
}

/*!
 @brief Get the platform-specific transaction functions.
 @param startFn Pointer to store the transaction start function pointer.
 @param stopFn Pointer to store the transaction stop function pointer.
*/
void NoteGetFnTransaction(txnStartFn *startFn, txnStopFn *stopFn)
{
    _LockNote();
    if (startFn != NULL) {
        *startFn = hookTransactionStart;
    }
    if (stopFn != NULL) {
        *stopFn = hookTransactionStop;
    }
    _UnlockNote();
}

/*!
 @brief Get the platform-specific mutex functions for I2C and Notecard.
 @param lockI2Cfn Pointer to store the I2C lock function pointer.
 @param unlockI2Cfn Pointer to store the I2C unlock function pointer.
 @param lockNotefn Pointer to store the Notecard lock function pointer.
 @param unlockNotefn Pointer to store the Notecard unlock function pointer.
*/
void NoteGetFnMutex(mutexFn *lockI2Cfn, mutexFn *unlockI2Cfn, mutexFn *lockNotefn,
                    mutexFn *unlockNotefn)
{
    if (lockI2Cfn != NULL) {
        *lockI2Cfn = hookLockI2C;
    }
    if (unlockI2Cfn != NULL) {
        *unlockI2Cfn = hookUnlockI2C;
    }
    if (lockNotefn != NULL) {
        *lockNotefn = hookLockNote;
    }
    if (unlockNotefn != NULL) {
        *unlockNotefn = hookUnlockNote;
    }
}

/*!
 @brief Get the platform-specific mutex functions for I2C.
 @param lockI2Cfn Pointer to store the I2C lock function pointer.
 @param unlockI2Cfn Pointer to store the I2C unlock function pointer.
*/
void NoteGetFnI2CMutex(mutexFn *lockI2Cfn, mutexFn *unlockI2Cfn)
{
    if (lockI2Cfn != NULL) {
        *lockI2Cfn = hookLockI2C;
    }
    if (unlockI2Cfn != NULL) {
        *unlockI2Cfn = hookUnlockI2C;
    }
}

/*!
 @brief Get the platform-specific mutex functions for the Notecard.
 @param lockFn Pointer to store the Notecard lock function pointer.
 @param unlockFn Pointer to store the Notecard unlock function pointer.
*/
void NoteGetFnNoteMutex(mutexFn *lockFn, mutexFn *unlockFn)
{
    if (lockFn != NULL) {
        *lockFn = hookLockNote;
    }
    if (unlockFn != NULL) {
        *unlockFn = hookUnlockNote;
    }
}

/*!
 @brief Get the platform-specific memory and timing hooks.
 @param mallocHook Pointer to store the memory allocation function pointer.
 @param freeHook Pointer to store the memory free function pointer.
 @param delayMsHook Pointer to store the delay function pointer.
 @param getMsHook Pointer to store the millis function pointer.
*/
void NoteGetFn(mallocFn *mallocHook, freeFn *freeHook, delayMsFn *delayMsHook,
               getMsFn *getMsHook)
{
    _LockNote();
    if (mallocHook != NULL) {
        *mallocHook = hookMalloc;
    }
    if (freeHook != NULL) {
        *freeHook = hookFree;
    }
    if (delayMsHook != NULL) {
        *delayMsHook = hookDelayMs;
    }
    if (getMsHook != NULL) {
        *getMsHook = hookGetMs;
    }
    _UnlockNote();
}

/*!
 @brief Get the platform-specific hooks for serial communication.
 @param resetFn Pointer to store the serial reset function pointer.
 @param transmitFn Pointer to store the serial transmit function pointer.
 @param availFn Pointer to store the serial available function pointer.
 @param receiveFn Pointer to store the serial receive function pointer.
*/
void NoteGetFnSerial(serialResetFn *resetFn, serialTransmitFn *transmitFn,
                     serialAvailableFn *availFn, serialReceiveFn *receiveFn)
{
    _LockNote();
    if (resetFn != NULL) {
        *resetFn = hookSerialReset;
    }
    if (transmitFn != NULL) {
        *transmitFn = hookSerialTransmit;
    }
    if (availFn != NULL) {
        *availFn = hookSerialAvailable;
    }
    if (receiveFn != NULL) {
        *receiveFn = hookSerialReceive;
    }
    _UnlockNote();
}

/*!
 @brief Get the platform-specific hooks for I2C communication.
 @param notecardAddr Pointer to store the I2C address.
 @param maxTransmitSize Pointer to store the I2C maximum segment size.
 @param resetFn Pointer to store the I2C reset function pointer.
 @param transmitFn Pointer to store the I2C transmit function pointer.
 @param receiveFn Pointer to store the I2C receive function pointer.
*/
void NoteGetFnI2C(uint32_t *notecardAddr, uint32_t *maxTransmitSize,
                  i2cResetFn *resetFn, i2cTransmitFn *transmitFn,
                  i2cReceiveFn *receiveFn)
{
    _LockNote();
    if (notecardAddr != NULL) {
        *notecardAddr = i2cAddress;
    }
    if (maxTransmitSize != NULL) {
        *maxTransmitSize = i2cMax;
    }
    if (resetFn != NULL) {
        *resetFn = hookI2CReset;
    }
    if (transmitFn != NULL) {
        *transmitFn = hookI2CTransmit;
    }
    if (receiveFn != NULL) {
        *receiveFn = hookI2CReceive;
    }
    _UnlockNote();
}

/*!
 @brief Get the I2C address of the Notecard.
 @param i2cAddr Pointer to store the I2C address.
*/
void NoteGetI2CAddress(uint32_t *i2cAddr)
{
    if (i2cAddr != NULL) {
        *i2cAddr = i2cAddress;
    }
}

//**************************************************************************/
/*!
  @brief  Reset the Serial bus using the platform-specific hook.
  @returns A boolean indicating whether the Serial bus was reset successfully.
*/
/**************************************************************************/
bool _noteSerialReset(void)
{
    if (hookActiveInterface == NOTE_C_INTERFACE_SERIAL && hookSerialReset != NULL) {
        return hookSerialReset();
    }
    return true;
}

//**************************************************************************/
/*!
  @brief  Transmit bytes over Serial using the platform-specific hook.
  @param   text The bytes to transmit.
  @param   len The length of bytes.
  @param   flush `true` to flush the bytes upon transmit.
*/
/**************************************************************************/
void _noteSerialTransmit(uint8_t *text, size_t len, bool flush)
{
    if (hookActiveInterface == NOTE_C_INTERFACE_SERIAL && hookSerialTransmit != NULL) {
        hookSerialTransmit(text, len, flush);
    }
}

//**************************************************************************/
/*!
  @brief  Determine if Serial bus is available using the platform-specific
  hook.
  @returns A boolean indicating whether the Serial bus is available to read.
*/
/**************************************************************************/
bool _noteSerialAvailable(void)
{
    if (hookActiveInterface == NOTE_C_INTERFACE_SERIAL && hookSerialAvailable != NULL) {
        return hookSerialAvailable();
    }
    return false;
}

//**************************************************************************/
/*!
  @brief  Obtain a character from the Serial bus using the platform-specific
  hook.
  @returns A character from the Serial bus.
*/
/**************************************************************************/
char _noteSerialReceive(void)
{
    if (hookActiveInterface == NOTE_C_INTERFACE_SERIAL && hookSerialReceive != NULL) {
        return hookSerialReceive();
    }
    return '\0';
}

//**************************************************************************/
/*!
  @brief  Reset the I2C bus using the platform-specific hook.
  @returns A boolean indicating whether the I2C bus was reset successfully.
*/
/**************************************************************************/
bool _noteI2CReset(uint16_t DevAddress)
{
    if (hookActiveInterface == NOTE_C_INTERFACE_I2C && hookI2CReset != NULL) {
        return hookI2CReset(DevAddress);
    }
    return true;
}

//**************************************************************************/
/*!
  @brief  Transmit bytes over I2C using the platform-specific hook.
  @param   DevAddress the I2C address for transmission.
  @param   pBuffer The bytes to transmit.
  @param   Size The length of bytes.
  @returns A c-string from the platform-specific hook, or an error string
  if the bus is not active.
*/
/**************************************************************************/
const char *_noteI2CTransmit(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size)
{
    if (hookActiveInterface == NOTE_C_INTERFACE_I2C && hookI2CTransmit != NULL) {
        return hookI2CTransmit(DevAddress, pBuffer, Size);
    }
    return "i2c not active";
}

//**************************************************************************/
/*!
  @brief  Receive bytes from I2C using the platform-specific hook.
  @param   DevAddress the I2C address for transmission.
  @param   pBuffer (out) A buffer in which to place received bytes.
  @param   Size The length of bytes.
  @param   available (out) The number of bytes left to read.
  @returns A c-string from the platform-specific hook, or an error string
  if the bus is not active.
*/
/**************************************************************************/
const char *_noteI2CReceive(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size, uint32_t *available)
{
    if (hookActiveInterface == NOTE_C_INTERFACE_I2C && hookI2CReceive != NULL) {
        return hookI2CReceive(DevAddress, pBuffer, Size, available);
    }
    return "i2c not active";
}

//**************************************************************************/
/*!
  @brief  Get the I2C address of the Notecard.
  @returns The current I2C address.
*/
/**************************************************************************/
uint32_t NoteI2CAddress(void)
{
    if (i2cAddress == 0) {
        return NOTE_I2C_ADDR_DEFAULT;
    }
    return i2cAddress;
}

//**************************************************************************/
/*!
  @brief  Set the I2C address for communication with the Notecard.
  @param   i2cAddr the I2C address to use for the Notecard.
*/
/**************************************************************************/
void NoteSetI2CAddress(uint32_t i2cAddr)
{
    i2cAddress = i2cAddr;
}

//**************************************************************************/
/*!
  @brief  Determine the maximum number of bytes for each segment of
  data sent to the Notecard over I2C.
  @returns A 32-bit integer of the maximum number of bytes per I2C segment.
*/
/**************************************************************************/
uint32_t NoteI2CMax(void)
{
    // Many Arduino libraries (such as ESP32) have a limit less than 32, so if the max isn't specified
    // we must assume the worst and segment the I2C messages into very tiny chunks.
    if (i2cMax == 0) {
        return NOTE_I2C_MAX_DEFAULT;
    }
    // Note design specs
    if (i2cMax > NOTE_I2C_MAX_MAX) {
        i2cMax = NOTE_I2C_MAX_MAX;
    }
    return i2cMax;
}

//**************************************************************************/
/*!
  @brief  Perform a hard reset on the Notecard using the platform-specific
  hook.
  @returns A boolean indicating whether the Notecard has been reset successfully.
*/
/**************************************************************************/
bool _noteHardReset(void)
{
    if (notecardReset == NULL) {
        return true;
    }
    return notecardReset();
}


//**************************************************************************/
/*!
  @brief  Perform a JSON request to the Notecard using the currently-set
  platform hook.

  @param   request A string containing the JSON request object, which MUST BE
            terminated with a newline character.
  @param   reqLen the string length of the JSON request.
  @param   response [out] A c-string buffer that will contain the newline ('\n')
            terminated JSON response from the Notercard. If NULL, no response
            will be captured.
  @param   timeoutMs The maximum amount of time, in milliseconds, to wait
            for data to arrive. Passing zero (0) disables the timeout.

  @returns NULL if successful, or an error string if the transaction failed
  or the hook has not been set.
*/
/**************************************************************************/
const char *_noteJSONTransaction(const char *request, size_t reqLen, char **response, uint32_t timeoutMs)
{
    if (notecardTransaction == NULL || hookActiveInterface == NOTE_C_INTERFACE_NONE) {
        return "a valid interface must be selected";
    }
    return notecardTransaction(request, reqLen, response, timeoutMs);
}

/**************************************************************************/
/*!
  @brief  Receive bytes over from the Notecard using the currently-set
  platform hook.
  @param   buffer A buffer to receive bytes into.
  @param   size (in/out)
            - (in) The size of the buffer in bytes.
            - (out) The length of the received data in bytes.
  @param   delay Respect standard processing delays.
  @param   timeoutMs The maximum amount of time, in milliseconds, to wait
            for data to arrive. Passing zero (0) disables the timeout.
  @param   available (in/out)
            - (in) The amount of bytes to request. Sending zero (0) will
                   initiate a priming query when using the I2C interface.
            - (out) The amount of bytes unable to fit into the provided buffer.
  @returns  A c-string with an error, or `NULL` if no error ocurred.
*/
/**************************************************************************/
const char *_noteChunkedReceive(uint8_t *buffer, uint32_t *size, bool delay,
                                uint32_t timeoutMs, uint32_t *available)
{
    if (notecardChunkedReceive == NULL || hookActiveInterface == NOTE_C_INTERFACE_NONE) {
        return "a valid interface must be selected";
    }
    return notecardChunkedReceive(buffer, size, delay, timeoutMs, available);
}

/**************************************************************************/
/*!
  @brief  Transmit bytes over to the Notecard using the currently-set
  platform hook.
  @param   buffer A buffer of bytes to transmit.
  @param   size The count of bytes in the buffer to send
  @param   delay Respect standard processing delays.
  @returns  A c-string with an error, or `NULL` if no error ocurred.
*/
/**************************************************************************/
const char *_noteChunkedTransmit(uint8_t *buffer, uint32_t size, bool delay)
{
    if (notecardChunkedTransmit == NULL || hookActiveInterface == NOTE_C_INTERFACE_NONE) {
        return "a valid interface must be selected";
    }
    return notecardChunkedTransmit(buffer, size, delay);
}
