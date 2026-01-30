/*!
 @file note.h

 Written by Ray Ozzie and Blues Inc. team.

 Copyright (c) 2019 Blues Inc. MIT License. Use of this source code is
 governed by licenses granted by the copyright holder including that found in
 the
 <a href="https://github.com/blues/note-c/blob/master/LICENSE">LICENSE</a>
 file.

 This library bundles the <a href="https://github.com/DaveGamble/cJSON">cJSON
 JSON parser library</a>.
 */

#pragma once

#if defined(__GNUC__) || defined(__clang__)
#define NOTE_C_DEPRECATED __attribute__((__deprecated__))
#elif defined(_MSC_VER)
#define NOTE_C_DEPRECATED __declspec(deprecated)
#else
#define NOTE_C_DEPRECATED
#define NOTE_C_NO_DEPRECATED_ATTR
#endif // __GNUC__ || __clang__

// In case they're not yet defined
#include <float.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

enum {
    NOTE_C_INTERFACE_NONE = 0,
    NOTE_C_INTERFACE_SERIAL,
    NOTE_C_INTERFACE_I2C,
};

#define _NOTE_C_STRINGIZE(x) #x
#define NOTE_C_STRINGIZE(x) _NOTE_C_STRINGIZE(x)

#define NOTE_C_VERSION_MAJOR 2
#define NOTE_C_VERSION_MINOR 5
#define NOTE_C_VERSION_PATCH 4

#define NOTE_C_VERSION NOTE_C_STRINGIZE(NOTE_C_VERSION_MAJOR) "." NOTE_C_STRINGIZE(NOTE_C_VERSION_MINOR) "." NOTE_C_STRINGIZE(NOTE_C_VERSION_PATCH)

// If double and float are the same size, then we must be on a small MCU. Turn
// on NOTE_C_LOW_MEM to conserve memory.
#if defined(FLT_MAX_EXP) && defined(DBL_MAX_EXP)
#if (FLT_MAX_EXP == DBL_MAX_EXP)
#define NOTE_C_LOW_MEM
#define NOTE_C_SINGLE_PRECISION
#endif
#elif defined(__FLT_MAX_EXP__) && defined(__DBL_MAX_EXP__)
#if (__FLT_MAX_EXP__ == __DBL_MAX_EXP__)
#define NOTE_C_LOW_MEM
#define NOTE_C_SINGLE_PRECISION
#endif
#else
// cppcheck-suppress preprocessorErrorDirective
#error What are floating point exponent length symbols for this compiler?
#endif

// NOTE_LOWMEM is the old name of NOTE_C_LOW_MEM. If NOTE_LOWMEM is defined,
// we define NOTE_C_LOW_MEM as well, for backwards compatibility. NOTE_FLOAT is
// also no longer used internally, but used to be defined when NOTE_LOWMEM was
// defined. It's also preserved here for backwards compatibility.
#ifdef NOTE_LOWMEM
#define NOTE_C_LOW_MEM
#define NOTE_FLOAT
#endif

#ifdef NOTE_C_LOW_MEM
#define ERRSTR(x,y) (y)
#define NOTE_C_LOG_DEBUG(msg) do { } while (0)
#else
#define ERRSTR(x,y) (x)
#endif // NOTE_C_LOW_MEM

/*!
 @brief The floating point type used for JSON handling in note-c.
 */
#ifdef NOTE_C_SINGLE_PRECISION
typedef float JNUMBER;
#else
typedef double JNUMBER;
#endif

/*!
 @brief The signed integer type used for JSON handling in note-c.
 */
typedef int64_t JINTEGER;
#define JINTEGER_MIN INT64_MIN
#define JINTEGER_MAX INT64_MAX

/*!
 @brief The unsigned integer type used for JSON handling in note-c.
 */
typedef uint64_t JUINTEGER;

// UNIX Epoch time (also known as POSIX time) is the  number of seconds that have elapsed since
// 00:00:00 Thursday, 1 January 1970, Coordinated Universal Time (UTC).  In this project, it always
// originates from the Notecard, which synchronizes the time from both the cell network and GPS.
typedef JUINTEGER JTIME;

// C-callable functions
#ifdef __cplusplus
extern "C" {
#endif

// cJSON wrappers
#include "n_cjson.h"

// Notecard hook functions

/*!
 @typedef delayMsFn

 @brief The type for the millisecond delay hook.

 @param ms The number of milliseconds to delay for.
 */
typedef void (*delayMsFn) (uint32_t ms);

/*!
 @typedef freeFn

 @brief The type for the memory freeing hook.

 @param mem Pointer to the memory to free.
 */
typedef void (*freeFn) (void * mem);

/*!
 @typedef getMsFn

 @brief The type for the millisecond counter hook.

 @returns The value of the millisecond counter.
 */
typedef uint32_t (*getMsFn) (void);

/*!
 @typedef debugOutputFn

 @brief The type for the debug output hook.

 @param text The debug text to output.

 @returns Number of characters written, or 0 on failure.
 */
typedef size_t (*debugOutputFn) (const char *text);

#ifdef NOTE_C_HEARTBEAT_CALLBACK
/*!
 @typedef heartbeatFn

 @brief The type for a heartbeat notification

 @param heartbeatJson The heartbeat status received
 @param context User context passed to the heartbeat function

 @returns `true` if the heartbeat processing should abandon the transaction.
 */
typedef bool (*heartbeatFn) (const char *heartbeatJson, void *context);
#endif

/*!
 @typedef i2cReceiveFn

 @brief The type for the I2C receive hook.

 This hook is used to receive a buffer of bytes from the Notecard.

 @param address The I2C address of the Notecard sending the data to receive.
 @param rxBuf A buffer to hold the data received from the Notecard.
 @param rxBufSize The size, in bytes, of rxBuf.
 @param available The number of bytes remaining to be received, if any.

 @returns NULL on success and an error string on failure.
 */
typedef const char * (*i2cReceiveFn) (uint16_t address, uint8_t* rxBuf,
                                      uint16_t rxBufSize, uint32_t *available);

/*!
 @typedef i2cResetFn

 @brief The type for the I2C reset hook.

 This hook is used to reset the I2C peripheral used to communicate with the
 Notecard.

 @param address The I2C address of the Notecard.
 */
typedef bool (*i2cResetFn) (uint16_t address);

/*!
 @typedef i2cTransmitFn

 @brief The type for the I2C transmit hook.

 This hook is used to send a buffer of bytes to the Notecard.

 @param address The I2C address of the Notecard to transmit the data to.
 @param txBuf A buffer of bytes to transmit to the Notecard.
 @param txBufSize The size, in bytes, of `txBuf`.

 @returns NULL on success and an error string on failure.
 */
typedef const char * (*i2cTransmitFn) (uint16_t address, uint8_t* txBuf,
                                       uint16_t txBufSize);

/*!
 @typedef mallocFn

 @brief The type for the memory allocation hook.

 @param size The number of bytes to allocate.

 @returns A pointer to the newly allocated memory or NULL on failure.
 */
typedef void * (*mallocFn) (size_t size);

/*!
 @typedef mutexFn

 @brief The type for the various mutex (i.e. lock/unlock) hooks.
 */
typedef void (*mutexFn) (void);

/*!
 @typedef serialAvailableFn

 @brief The type for the serial available hook.

 @return `true` if there's data to read and `false` otherwise.
 */
typedef bool (*serialAvailableFn) (void);

/*!
 @typedef serialReceiveFn

 @brief The type for the serial receive hook.

 @return The received byte.
 */
typedef char (*serialReceiveFn) (void);

/*!
 @typedef serialResetFn

 @brief The type for the serial reset hook.

 This hook is used to reset the serial peripheral used to communicate with the
 Notecard.

 @returns `true` on success and `false` on failure.
 */
typedef bool (*serialResetFn) (void);

/*!
 @typedef serialTransmitFn

 @brief The type for the serial transmit hook.

 @param txBuf A buffer of bytes to transmit to the Notecard.
 @param txBufSize The size, in bytes, of `txBuf`.
 @param flush If true, flush the serial peripheral's transmit buffer.
 */
typedef void (*serialTransmitFn) (uint8_t *txBuf, size_t txBufSize, bool flush);

/*!
 @typedef txnStartFn

 @brief The type for the transaction start hook.

 @param timeoutMs Transaction timeout in milliseconds.

 @returns `true` if the transaction was started successfully, `false` otherwise.
 */
typedef bool (*txnStartFn) (uint32_t timeoutMs);

/*!
 @typedef txnStopFn

 @brief The type for the transaction stop hook.
 */
typedef void (*txnStopFn) (void);

// External API

/*!
 @brief Reset the Notecard, clearing any error state.

 @returns `true` if the reset was successful, `false` otherwise.
 */
bool NoteReset(void);
/*!
 @brief Mark that a Notecard reset is required on the next transaction.

 This function sets an internal flag indicating that the Notecard should be
 reset before the next communication attempt.
 */
void NoteResetRequired(void);
#define NoteNewBody JCreateObject
#define NoteAddBodyToObject(a, b) JAddItemToObject(a, "body", b)
#define NoteGetBody(a) JGetObject(a, "body")
/*!
 @brief Create a new request JSON object.

 Creates a dynamically allocated `J` object with one field `"req"` whose value
 is the passed in request string.

 @param request The name of the Notecard API request (e.g., "card.version").

 @returns A pointer to a newly allocated JSON object with the "req" field
          populated, or NULL on failure.
 */
J *NoteNewRequest(const char *request);
/*!
 @brief Create a new command JSON object.

 Create a dynamically allocated `J` object with one field `"cmd"` whose value is
 the passed in request string. Commands are similar to requests, but the
 Notecard does not send a response.

 @param request The name of the Notecard API command (e.g., "card.restart").

 @returns A pointer to a newly allocated JSON object with the "cmd" field
          populated, or NULL on failure.
 */
J *NoteNewCommand(const char *request);
/*!
 @brief Send a request to the Notecard and return the response.

 The passed in request object is always freed, regardless of if the request was
 successful or not.

 @param req Pointer to a `J` request object.

 @returns A `J` object with the response or NULL if there was an error sending
          the request.

 @see NoteResponseError to check the response for errors.
 */
J *NoteRequestResponse(J *req);
/*!
 @brief Send a request to the Notecard, retrying until it succeeds or times out,
        then return the response.

 The passed in request object is always freed, regardless of if the request was
 successful or not.

 @param req Pointer to a `J` request object.
 @param timeoutSeconds Time limit for retries, in seconds.

 @returns A `J` object with the response or NULL if there was an error sending
          the request.

 @note Timeouts may occur when either there is no response, or if the response
       contains an I/O error.

 @see NoteResponseError to check the response for errors.
 */
J *NoteRequestResponseWithRetry(J *req, uint32_t timeoutSeconds);
/*!
 @brief Send a request to the Notecard and return the response as JSON string.

 Unlike NoteRequestResponse, this function expects the request to be a valid
 JSON C-string, rather than a `J` object. The string is expected to be
 newline-terminated, otherwise the call produces undefined behavior. The
 response is returned as a dynamically allocated JSON C-string. The response
 is newline-terminated, just like the request. The caller is responsible for
 freeing the response string. If the request was a command (i.e. it uses "cmd"
 instead of "req"), this function returns NULL, since commands do not have
 a response.

 @param reqJSON A valid newline-terminated JSON C-string containing the request.

 @returns A newline-terminated JSON C-string with the response, or NULL
          if there was no response or if there was an error.

 @note When a "cmd" is sent, it is not possible to determine if an error occurred.

 @note Unlike the `NoteRequest*` functions, this function does not automatically
       free the request JSON string. It is not possible to know if the parameter
       is a string literal. As such, it is the caller's responsibility to manage
       the memory associated with the request string.
 */
char * NoteRequestResponseJSON(const char *reqJSON);
NOTE_C_DEPRECATED void NoteSuspendTransactionDebug(void);
NOTE_C_DEPRECATED void NoteResumeTransactionDebug(void);
#define SYNCSTATUS_LEVEL_MAJOR         0
#define SYNCSTATUS_LEVEL_MINOR         1
#define SYNCSTATUS_LEVEL_DETAILED      2
#define SYNCSTATUS_LEVEL_ALGORITHMIC   3
#define SYNCSTATUS_LEVEL_ALL          -1
/*!
 @brief Enable debug output for sync status monitoring.

 @param pollFrequencyMs How often to poll for sync status, in milliseconds.
 @param maxLevel Maximum detail level for sync status output.

 @returns `true` if debugging was enabled successfully, `false` otherwise.
 */
bool NoteDebugSyncStatus(int pollFrequencyMs, int maxLevel);
/*!
 @brief Send a request to the Notecard.

 The passed in request object is always freed, regardless of if the request was
 successful or not. The response from the Notecard, if any, is freed and not
 returned to the caller.

 @param req Pointer to a `J` request object.

 @returns `true` if successful and `false` if an error occurs (e.g. out of
          memory or the response from the Notecard has an "err" field). If req
          is a command rather than a request, a `true` return value indicates
          that the command was sent without error. However, since the Notecard
          sends no response to commands, it does not guarantee that the
          command was received and processed by the Notecard.

 @see NoteRequestResponse if you need to work with the response.
 */
bool NoteRequest(J *req);
/*!
 @brief Send a request to the Notecard, retrying until it succeeds or times out.

 The passed in request object is always freed, regardless of if the request was
 successful or not. The response from the Notecard, if any, is freed and not
 returned to the caller.

 @param req Pointer to a `J` request object.
 @param timeoutSeconds Time limit for retries, in seconds, if there is no
        response, or if the response contains an I/O error.

 @returns `true` if successful and `false` if an error occurs (e.g. out of
          memory or the response from the Notecard has an "err" field).

 @see NoteRequestResponseWithRetry if you need to work with the response.
 */
bool NoteRequestWithRetry(J *req, uint32_t timeoutSeconds);
/*!
 @brief Set the request timeout for Notecard transactions.

 Provides control for transactions which must be tailored based upon the nature
 of the transaction being performed.

 @param overrideSecs The timeout in seconds (0 for default).

 @returns The previous timeout value that was overridden.
 */
uint32_t NoteSetRequestTimeout(uint32_t overrideSecs);

/*!
 @brief Check if the Notecard response contains an error.

 @param rsp The response to check.

 @returns `true` if there's an error and `false` if there's not.
 */
#define NoteResponseError(rsp) (!JIsNullString(rsp, "err"))
#define NoteResponseErrorContains(rsp, errstr) (JContainsString(rsp, "err", errstr))
/*!
 @brief Free a response from the Notecard.

 @param rsp The response to free.
 */
#define NoteDeleteResponse(rsp) JDelete(rsp)
/*!
 @brief Send a request to the Notecard and return the response.

 This function doesn't free the passed in request object. The caller is
 responsible for freeing it.

 @param req Pointer to a `J` request object.

 @returns A `J` object with the response or NULL if there was an error sending
          the request.

 @see NoteResponseError to check the response for errors.
 */
J *NoteTransaction(J *req);
/*!
 @brief Check if an error string contains a specific error type.

 @param errstr The error string to check.
 @param errtype The error type to search for in errstr.

 @returns `true` if the error string contains the specified error type,
          `false` otherwise.

 @note Only Notecard errors enclosed in `{}` (e.g. `{io}` for an I/O error)
       are guaranteed by the API.
 */
bool NoteErrorContains(const char *errstr, const char *errtype);
/*!
 @brief Clean up an error string by removing all bracketed tags.

 Notecard errors are enclosed in `{}` (e.g. `{io}` for an I/O error). This
 function takes the input string and removes all bracketed errors from it,
 meaning it removes any substrings matching the pattern `{some error string}`,
 including the braces.

 @param errbuf A C-string to to clean of errors.
 */
void NoteErrorClean(char *errbuf);
/*!
 @brief Set the debug output function hook.

 @param fn Pointer to the debug output function.

 @note This operation will lock Notecard access while in progress, if Notecard
       mutex functions have been set.
 */
void NoteSetFnDebugOutput(debugOutputFn fn);
/*!
 @brief Get the currently set debug output function hook.

 @param fn Pointer to store the current debug output function pointer.
 */
void NoteGetFnDebugOutput(debugOutputFn *fn);
#ifdef NOTE_C_HEARTBEAT_CALLBACK
/*!
 @brief Set the heartbeat callback function.

 @param fn Pointer to the heartbeat callback function.
 @param context User context to pass to the heartbeat function.
 */
void NoteSetFnHeartbeat(heartbeatFn fn, void *context);
/*!
 @brief Get the currently set heartbeat callback function.

 @param fn Pointer to store the current heartbeat function pointer.
 @param context Pointer to store the current heartbeat context pointer.
 */
void NoteGetFnHeartbeat(heartbeatFn *fn, void **context);
#endif
/*!
 @brief Set the transaction hook functions.

 @param startFn Function to call when starting a transaction.
 @param stopFn Function to call when stopping a transaction.

 @note This operation will lock Notecard access while in progress, if Notecard
       mutex functions have been set.
 */
void NoteSetFnTransaction(txnStartFn startFn, txnStopFn stopFn);
/*!
 @brief Get the platform-specific transaction hook functions.

 @param startFn Pointer to store the current transaction start function.
 @param stopFn Pointer to store the current transaction stop function.

 @note Any of the passed in pointers can be NULL if the caller is not
       interested in that particular function pointer.
 */
void NoteGetFnTransaction(txnStartFn *startFn, txnStopFn *stopFn);
/*!
 @brief Set the mutex functions for I2C and Notecard access protection.

 @param lockI2Cfn Function to lock I2C access.
 @param unlockI2Cfn Function to unlock I2C access.
 @param lockNotefn Function to lock Notecard access.
 @param unlockNotefn Function to unlock Notecard access.
 */
void NoteSetFnMutex(mutexFn lockI2Cfn, mutexFn unlockI2Cfn, mutexFn lockNotefn,
                    mutexFn unlockNotefn);
/*!
 @brief Get the currently set mutex functions for I2C and Notecard.

 @param lockI2Cfn Pointer to store the current I2C lock function.
 @param unlockI2Cfn Pointer to store the current I2C unlock function.
 @param lockNotefn Pointer to store the current Note lock function.
 @param unlockNotefn Pointer to store the current Note unlock function.

 @note Any of the passed in pointers can be NULL if the caller is not
       interested in that particular function pointer.
 */
void NoteGetFnMutex(mutexFn *lockI2Cfn, mutexFn *unlockI2Cfn, mutexFn *lockNotefn,
                    mutexFn *unlockNotefn);
/*!
 @brief Set the I2C mutex functions.

 @param lockI2Cfn Function to lock I2C access.
 @param unlockI2Cfn Function to unlock I2C access.
 */
void NoteSetFnI2CMutex(mutexFn lockI2Cfn, mutexFn unlockI2Cfn);
/*!
 @brief Get the currently set I2C mutex functions.

 @param lockI2Cfn Pointer to store the current I2C lock function.
 @param unlockI2Cfn Pointer to store the current I2C unlock function.

 @note Any of the passed in pointers can be NULL if the caller is not
       interested in that particular function pointer.
 */
void NoteGetFnI2CMutex(mutexFn *lockI2Cfn, mutexFn *unlockI2Cfn);
/*!
 @brief Set the Notecard mutex functions.

 @param lockFn Function to lock Notecard access.
 @param unlockFn Function to unlock Notecard access.
 */
void NoteSetFnNoteMutex(mutexFn lockFn, mutexFn unlockFn);
/*!
 @brief Get the currently set Notecard mutex functions.

 @param lockFn Pointer to store the current Notecard lock function.
 @param unlockFn Pointer to store the current Notecard unlock function.

 @note Any of the passed in pointers can be NULL if the caller is not
       interested in that particular function pointer.
 */
void NoteGetFnNoteMutex(mutexFn *lockFn, mutexFn *unlockFn);
/*!
 @brief Set the default system functions (memory allocation, delay, timing).

 @param mallocfn The platform-specific function for memory allocation.
 @param freefn The platform-specific function for memory deallocation.
 @param delayfn The platform-specific function for millisecond delays.
 @param millisfn The platform-specific function to get millisecond counter.

 @note These functions are used if no other system functions are set.
 @note This operation will lock Notecard access while in progress, if Notecard
       mutex functions have been set.
 */
void NoteSetFnDefault(mallocFn mallocfn, freeFn freefn, delayMsFn delayfn,
                      getMsFn millisfn);
/*!
 @brief Set the system hook functions (memory allocation, delay, timing).

 @param mallocHook The platform-specific function for memory allocation.
 @param freeHook The platform-specific function for memory deallocation.
 @param delayMsHook The platform-specific function for millisecond delays.
 @param getMsHook The platform-specific function to get millisecond counter.

 @note This function overrides any previously set system functions.
 @note This operation will lock Notecard access while in progress, if Notecard
       mutex functions have been set.
 */
void NoteSetFn(mallocFn mallocHook, freeFn freeHook, delayMsFn delayMsHook,
               getMsFn getMsHook);
/*!
 @brief Get the platform-specific memory and timing hook functions.

 @param mallocHook Pointer to store the current memory allocation function.
 @param freeHook Pointer to store the current memory deallocation function.
 @param delayMsHook Pointer to store the current delay function.
 @param getMsHook Pointer to store the current millisecond counter function.

 @note Any of the passed in pointers can be NULL if the caller is not
       interested in that particular function pointer.
 */
void NoteGetFn(mallocFn *mallocHook, freeFn *freeHook, delayMsFn *delayMsHook,
               getMsFn *getMsHook);
/*!
 @brief Set the platform-specific serial communication hook functions.

 @param resetFn The platform-specific function to reset the serial peripheral.
 @param transmitFn The platform-specific function to transmit data via serial.
 @param availFn The platform-specific function to check if serial data is available.
 @param receiveFn The platform-specific function to receive serial data.

 @note This function overrides any previously set serial functions.
 @note This operation will set the active interface to serial.
 @note This operation will lock Notecard access while in progress, if Notecard
       mutex functions have been set.
 */
void NoteSetFnSerial(serialResetFn resetFn, serialTransmitFn transmitFn,
                     serialAvailableFn availFn, serialReceiveFn receiveFn);
/*!
 @brief Set the default serial communication hook functions.

 @param resetFn The platform-specific function to reset the serial peripheral.
 @param transmitFn The platform-specific function to transmit data via serial.
 @param availFn The platform-specific function to check if serial data is available.
 @param receiveFn The platform-specific function to receive serial data.

 @note These functions are used if no other serial functions are set.
 @note This operation will set the active interface to serial, if unset.
 @note This operation will lock Notecard access while in progress, if Notecard
       mutex functions have been set.
 */
void NoteSetFnSerialDefault(serialResetFn resetFn, serialTransmitFn transmitFn,
                            serialAvailableFn availFn, serialReceiveFn receiveFn);
/*!
 @brief Get the platform-specific serial communication hook functions.

 @param resetFn Pointer to store the current serial reset function.
 @param transmitFn Pointer to store the current serial transmit function.
 @param availFn Pointer to store the current serial available function.
 @param receiveFn Pointer to store the current serial receive function.

 @note Any of the passed in pointers can be NULL if the caller is not
       interested in that particular function pointer.
 */
void NoteGetFnSerial(serialResetFn *resetFn, serialTransmitFn *transmitFn,
                     serialAvailableFn *availFn, serialReceiveFn *receiveFn);
/*!
 @brief Set the platform-specific I2C communication hook functions, address and MTU.

 @param notecardAddr I2C address of the Notecard (0 for default).
 @param maxTransmitSize Maximum number of bytes to transmit in a single I2C segment (0 for default).
 @param resetFn The platform-specific function to reset the I2C peripheral.
 @param transmitFn The platform-specific function to transmit data via I2C.
 @param receiveFn The platform-specific function to receive data via I2C.

 @note This function overrides any previously set I2C functions.
 @note This operation will set the active interface to I2C.
 @note This operation will lock Notecard access while in progress, if Notecard
       mutex functions have been set.
 */
void NoteSetFnI2C(uint32_t notecardAddr, uint32_t maxTransmitSize,
                  i2cResetFn resetFn, i2cTransmitFn transmitFn,
                  i2cReceiveFn receiveFn);
/*!
 @brief Set the default I2C communication hook functions.

 @param notecardAddr I2C address of the Notecard (0 for default).
 @param maxTransmitSize Maximum number of bytes to transmit in a single I2C segment (0 for default).
 @param resetFn The platform-specific function to reset the I2C peripheral.
 @param transmitFn The platform-specific function to transmit data via I2C.
 @param receiveFn The platform-specific function to receive data via I2C.

 @note These functions are used if no other I2C functions are set.
 @note This operation will set the active interface to I2C, if unset.
 @note This operation will lock Notecard access while in progress, if Notecard
       mutex functions have been set.
 */
void NoteSetFnI2CDefault(uint32_t notecardAddr, uint32_t maxTransmitSize,
                         i2cResetFn resetFn, i2cTransmitFn transmitFn,
                         i2cReceiveFn receiveFn);
/*!
 @brief Get the platform-specific I2C communication hook functions.

 @param notecardAddr Pointer to store the current Notecard I2C address.
 @param maxTransmitSize Pointer to store the current maximum transmit size.
 @param resetFn Pointer to store the current I2C reset function.
 @param transmitFn Pointer to store the current I2C transmit function.
 @param receiveFn Pointer to store the current I2C receive function.

 @note Any of the passed in pointers can be NULL if the caller is not
       interested in that particular function pointer.
 */
void NoteGetFnI2C(uint32_t *notecardAddr, uint32_t *maxTransmitSize,
                  i2cResetFn *resetFn, i2cTransmitFn *transmitFn,
                  i2cReceiveFn *receiveFn);
/*!
 @brief Set the active communication interface.

 @param   interface  The desired interface to use. One of:
   - NOTE_C_INTERFACE_NONE (default)
   - NOTE_C_INTERFACE_SERIAL
   - NOTE_C_INTERFACE_I2C

 @note This operation will lock Notecard access while in progress, if Notecard
       mutex functions have been set.
 */
void NoteSetActiveInterface(int interface);
/*!
 @brief Get the currently active communication interface.

 @returns The active interface (NOTE_C_INTERFACE_SERIAL, NOTE_C_INTERFACE_I2C, or NOTE_C_INTERFACE_NONE).
 */
int NoteGetActiveInterface(void);
/*!
 @brief Disable all Notecard communication functions.

 This function sets all hook functions to disabled/stub implementations.
 */
void NoteSetFnDisabled(void);
/*!
 @brief Set the I2C address for Notecard communication.

 @param i2cAddr The I2C address to use for the Notecard.
 */
void NoteSetI2CAddress(uint32_t i2cAddr);
/*!
 @brief Set the MTU for I2C communication with the Notecard.

 The MTU is the maximum number of bytes that can be sent to or received
 from the Notecard in a single I2C transaction.

 @param i2cMtu The maximum transmission unit (MTU) for I2C communication.
 */
void NoteSetI2CMtu(uint32_t i2cMtu);
/*!
 @brief Get the current I2C address for Notecard communication.

 @param [out] i2cAddr Pointer to store the current I2C address.
 */
void NoteGetI2CAddress(uint32_t *i2cAddr);
/*!
 @brief Get the current MTU for I2C communication with the Notecard.

 The MTU is the maximum number of bytes that can be sent to or received
 from the Notecard in a single I2C transaction.

 @param [out] i2cMtu Pointer to store the current I2C MTU.
 */
void NoteGetI2CMtu(uint32_t *i2cMtu);

// The Notecard, whose default I2C address is below, uses a serial-to-i2c
// protocol whose "byte count" must fit into a single byte and which must not
// include a 2-byte header field.  This is why the maximum that can be
// transmitted by note-c in a single I2C I/O is 255 - 2 = 253 bytes.

/*!
 @brief The default I2C address of the Notecard.
 */
#define NOTE_I2C_ADDR_DEFAULT	0x17

// Serial-to-i2c protocol header size in bytes
#ifndef NOTE_I2C_HEADER_SIZE
#  define NOTE_I2C_HEADER_SIZE 2
#endif

// Maximum bytes capable of being transmitted in a single read/write operation
#ifdef NOTE_I2C_MAX_MAX
#  ifdef NOTE_I2C_MTU_MAX
#    undef NOTE_I2C_MTU_MAX
#  endif
#else
#  ifdef NOTE_I2C_MTU_MAX
#    define NOTE_I2C_MAX_MAX NOTE_I2C_MTU_MAX
#  else
#    define NOTE_I2C_MAX_MAX (UCHAR_MAX - NOTE_I2C_HEADER_SIZE)
#  endif
#endif
#ifndef NOTE_I2C_MTU_MAX
#  define NOTE_I2C_MTU_MAX NOTE_I2C_MAX_MAX
#endif

// In ARDUINO implementations, which to date is the largest use of this library,
// the Wire package is implemented in a broad variety of ways by different
// vendors.  The default implementation has a mere 32-byte static I2C buffer,
// which means that the maximum to be transmitted in a single I/O (given our
// 2-byte serial-to-i2c protocol header) is 30 bytes.  However, if we know
// the specific platform (such as STM32DUINO) we can relax this restriction.
#ifdef NOTE_I2C_MAX_DEFAULT
// user is overriding it at compile time
#  ifdef NOTE_I2C_MTU_DEFAULT
#    undef NOTE_I2C_MTU_DEFAULT
#  endif
#else
#  if defined(NOTE_I2C_MTU_DEFAULT)
// user is overriding it at compile time
#    define NOTE_I2C_MAX_DEFAULT NOTE_I2C_MTU_DEFAULT
#  elif defined(ARDUINO_ARCH_STM32)
// we know that stm32duino dynamically allocates I/O buffer
#    define NOTE_I2C_MAX_DEFAULT NOTE_I2C_MTU_MAX
#  else
// default to what's known to be safe for all Arduino implementations
/*!
 @brief The maximum number of bytes to request from or transmit to the Notecard
        in a single chunk.
 */
#    define NOTE_I2C_MAX_DEFAULT	30
#  endif
#endif
#ifndef NOTE_I2C_MTU_DEFAULT
#  define NOTE_I2C_MTU_DEFAULT NOTE_I2C_MAX_DEFAULT
#endif

// User agent

/*!
 @brief Get the current user agent JSON object.

 @returns A JSON object containing user agent information, or NULL on failure.
 */
J *NoteUserAgent(void);
/*!
 @brief Update the user agent with additional information.

 @param ua JSON object containing user agent updates to merge.
 */
void NoteUserAgentUpdate(J *ua);
/*!
 @brief Set the user agent string.

 @param agent The user agent string to set.
 */
void NoteSetUserAgent(char *agent);
/*!
 @brief Set the operating system information in the user agent.

 @param os_name The operating system name.
 @param os_platform The operating system platform.
 @param os_family The operating system family.
 @param os_version The operating system version.
 */
void NoteSetUserAgentOS(char *os_name, char *os_platform, char *os_family, char *os_version);
/*!
 @brief Set the CPU information in the user agent.

 @param cpu_mem Available CPU memory in kilobytes.
 @param cpu_mhz CPU speed in MHz.
 @param cpu_cores Number of CPU cores.
 @param cpu_vendor CPU vendor name.
 @param cpu_name CPU model name.
 */
void NoteSetUserAgentCPU(int cpu_mem, int cpu_mhz, int cpu_cores, char *cpu_vendor, char *cpu_name);

// Calls to the functions set above

/*!
 @brief Output a debug message.

 @param msg The debug message to output.
 */
void NoteDebug(const char *msg);
/*!
 @brief Output a debug message followed by a newline.

 @param msg The debug message to output.
 */
void NoteDebugln(const char *msg);
/*!
 @brief Output a debug message with an integer value appended, terminated by
        a newline.

 @param msg The debug message to output.
 @param n The integer value to append to the message.
 */
void NoteDebugIntln(const char *msg, int n);
/*!
 @brief Output a formatted debug message.

 @param format The printf-style format string.
 @param ... Variable arguments for the format string.
 */
void NoteDebugf(const char *format, ...);

#define NOTE_C_LOG_LEVEL_ERROR  0
#define NOTE_C_LOG_LEVEL_WARN   1
#define NOTE_C_LOG_LEVEL_INFO   2
#define NOTE_C_LOG_LEVEL_DEBUG  3

/*!
 @brief Output a debug message with a specified log level.

 @param level The log level for this message.
 @param msg The debug message to output.

 @note The message will be dropped if the specified level is less than
       or equal to the current log level.
 @note See the NOTE_C_LOG_LEVEL_* macros for possible values.
 */
void NoteDebugWithLevel(uint8_t level, const char *msg);
/*!
 @brief Output a debug message with a specified log level, followed by a newline.

 @param level The log level for this message.
 @param msg The debug message to output.

 @note The message will be dropped if the specified level is less than
       or equal to the current log level.
 @note See the NOTE_C_LOG_LEVEL_* macros for possible values.
 */
void NoteDebugWithLevelLn(uint8_t level, const char *msg);

#ifdef NOTE_C_LOG_SHOW_FILE_AND_LINE
#define NOTE_C_LOG_FILE_AND_LINE(lvl) do { \
  NoteDebugWithLevel(lvl, __FILE__ ":" NOTE_C_STRINGIZE(__LINE__) " "); \
} while (0);
#else
#define NOTE_C_LOG_FILE_AND_LINE(lvl)
#endif

#ifndef NOTE_C_LOG_ERROR
#define NOTE_C_LOG_ERROR(msg) do { \
  NOTE_C_LOG_FILE_AND_LINE(NOTE_C_LOG_LEVEL_ERROR); \
  NoteDebugWithLevel(NOTE_C_LOG_LEVEL_ERROR, "[ERROR] "); \
  NoteDebugWithLevelLn(NOTE_C_LOG_LEVEL_ERROR, msg); \
} while (0)
#endif

#ifndef NOTE_C_LOG_WARN
#define NOTE_C_LOG_WARN(msg) do { \
  NOTE_C_LOG_FILE_AND_LINE(NOTE_C_LOG_LEVEL_WARN); \
  NoteDebugWithLevel(NOTE_C_LOG_LEVEL_WARN, "[WARN] "); \
  NoteDebugWithLevelLn(NOTE_C_LOG_LEVEL_WARN, msg); \
} while (0)
#endif

#ifndef NOTE_C_LOG_INFO
#define NOTE_C_LOG_INFO(msg) do { \
  NOTE_C_LOG_FILE_AND_LINE(NOTE_C_LOG_LEVEL_INFO); \
  NoteDebugWithLevel(NOTE_C_LOG_LEVEL_INFO, "[INFO] "); \
  NoteDebugWithLevelLn(NOTE_C_LOG_LEVEL_INFO, msg); \
} while (0)
#endif

#ifndef NOTE_C_LOG_DEBUG
#define NOTE_C_LOG_DEBUG(msg) do { \
  NOTE_C_LOG_FILE_AND_LINE(NOTE_C_LOG_LEVEL_DEBUG); \
  NoteDebugWithLevel(NOTE_C_LOG_LEVEL_DEBUG, "[DEBUG] "); \
  NoteDebugWithLevelLn(NOTE_C_LOG_LEVEL_DEBUG, msg); \
} while (0)
#endif

// The default log level
#define NOTE_C_LOG_LEVEL_DEFAULT NOTE_C_LOG_LEVEL_INFO

// The max log level for NoteDebugWithLevel may be configured at
// compile-time, via NOTE_C_LOG_LEVEL, otherwise it defaults to
// NOTE_C_LOG_LEVEL_DEFAULT.
#ifndef NOTE_C_LOG_LEVEL
#define NOTE_C_LOG_LEVEL NOTE_C_LOG_LEVEL_DEFAULT
#endif

// Log level may also be set at runtime, via NoteSetLogLevel.

/*!
 @brief Set the current log level for debug output filtering.

 @param level The log level to set:
              - NOTE_C_LOG_LEVEL_ERROR
              - NOTE_C_LOG_LEVEL_WARN
              - NOTE_C_LOG_LEVEL_INFO
              - NOTE_C_LOG_LEVEL_DEBUG
 */
void NoteSetLogLevel(int level);

/*!
 @brief Allocate memory chunk using the platform-specific hook.

 @param size Minimum number of bytes to allocate.

 @returns Pointer to allocated memory, or NULL if allocation failed.
 */
void *NoteMalloc(size_t size);
/*!
 @brief Free memory using the platform-specific hook.

 @param ptr Pointer to memory to free.
 */
void NoteFree(void *ptr);
/*!
 @brief Get the current millisecond counter value.

 @returns Current millisecond counter value.
 */
uint32_t NoteGetMs(void);
/*!
 @brief Use platform hook to delay for the specified number of milliseconds.

 @param ms Number of milliseconds to delay.
 */
void NoteDelayMs(uint32_t ms);
/*!
 @brief Lock I2C access for thread safety.
 */
void NoteLockI2C(void);
/*!
 @brief Unlock I2C access.
 */
void NoteUnlockI2C(void);
/*!
 @brief Get the current I2C address being used for Notecard communication.

 @returns The I2C address of the Notecard.

 @note If the address remains unset or is set to 0x00 using `NoteSetI2CAddress`,
       the address will be returned as `NOTE_I2C_ADDRESS_DEFAULT`.

 @see NoteGetI2CAddress
 */
uint32_t NoteI2CAddress(void);
/*!
 @brief Get the current MTU for I2C communication with the Notecard.

 The MTU is the maximum number of bytes that can be sent to or received
 from the Notecard in a single I2C transaction.

 @returns Maximum number of bytes that can be transferred in a single I2C transaction.

 @note If the MTU remains unset or is set to 0 using NoteSetI2CMtu, the
       MTU will be returned as NOTE_I2C_MTU_DEFAULT.
 @note The MTU value returned will always be less than or equal to
       `NOTE_I2C_MTU_MAX`. If a value larger than `NOTE_I2C_MTU_MAX` was set using
       `NoteSetI2CMtu`, the MTU will be silently reduced to `NOTE_I2C_MTU_MAX`.

 @see NoteGetI2CMtu
 */
uint32_t NoteI2CMax(void);
/*!
 @brief Get the amount of available memory.

 @returns Amount of available memory in bytes, or 0 if unknown.
 */
uint32_t NoteMemAvailable(void);
/*!
 @brief Print text using the configured debug output function.

 @param text The text to print.

 @returns `true` if the text was printed successfully, `false` otherwise.
 */
bool NotePrint(const char *text);
/*!
 @brief Print a line of text followed by a newline.

 @param line The line of text to print.
 */
void NotePrintln(const char *line);
/*!
 @brief Print formatted text using printf-style formatting.

 @param format The printf-style format string.
 @param ... Variable arguments for the format string.

 @returns `true` if the text was printed successfully, `false` otherwise.
 */
bool NotePrintf(const char *format, ...);

// String helpers to help encourage the world to abandon the horribly-error-prone strn*

#ifndef HAVE_STRLCPY
/*!
 @brief Safe string copy function.

 Copies up to siz-1 characters from src to dst, ensuring dst is always
 null-terminated.

 @param dst Destination buffer.
 @param src Source string.
 @param siz Size of destination buffer.

 @returns Length of src string.
 */
#ifdef __cplusplus
size_t strlcpy(char *dst, const char *src, size_t siz) noexcept;
#else
size_t strlcpy(char *dst, const char *src, size_t siz);
#endif
#endif

#ifndef HAVE_STRLCAT
/*!
 @brief Safe string concatenation function.

 Appends src to dst, ensuring the result is null-terminated and does not
 exceed siz characters total.

 @param dst Destination buffer containing existing string.
 @param src Source string to append.
 @param siz Total size of destination buffer.

 @returns Total length of the string that would result from the concatenation.
 */
#ifdef __cplusplus
size_t strlcat(char *dst, const char *src, size_t siz) noexcept;
#else
size_t strlcat(char *dst, const char *src, size_t siz);
#endif
#endif

// JSON helpers

/*!
 @brief Initialize the JSON subsystem.
 */
void JInit(void);
/*!
 @brief Check the JSON subsystem for memory leaks or other issues.
 */
void JCheck(void);
/*!
 @brief Check if a field is present in a JSON object.

 @param json The JSON object to check.
 @param field The field name to look for.

 @returns `true` if the field is present, `false` otherwise.
 */
bool JIsPresent(J *json, const char *field);
/*!
 @brief Get a string value from a JSON object field.

 @param json The JSON object to query.
 @param field The field name to retrieve.

 @returns Pointer to the string value, or an empty string ("") if the field
          doesn't exist or isn't a string.

 @note The returned string is a pointer to the string contained in the JSON
       object. It is not a copy of the string, so once the JSON object is freed,
       the pointer is no longer valid.
 */
char *JGetString(J *json, const char *field);
/*!
 @brief Get the number field from a JSON object.

 @param json The JSON object to query.
 @param field The field name to retrieve.

 @returns The numeric value, or 0.0f if the field doesn't exist or isn't a number.

 @note The returned value is the floating point representation of the number.

 @see `JGetInt`
 @see `JGetBool`
 */
JNUMBER JGetNumber(J *json, const char *field);
/*!
 @brief Get an array object from a JSON object field.

 @param json The JSON object to query.
 @param field The field name to retrieve.

 @returns Pointer to the JSON array (`J *`), or `NULL` if the field doesn't
          exist or isn't an array.

 @note The returned JSON object is a pointer to the array contained in the
       parent JSON object. It is not a copy, so once the parent JSON object is
       freed, the pointer is no longer valid.
 */
J *JGetArray(J *json, const char *field);
/*!
 @brief Get a JSON object from a JSON object field.

 @param json The JSON object to query.
 @param field The field name to retrieve.

 @returns Pointer to the nested JSON object, or NULL if the field doesn't exist
          or isn't an object.

 @note The returned JSON object is a pointer to the object contained in the
       parent JSON object. It is not a copy, so once the parent JSON object is
       freed, the pointer is no longer valid.
 */
J *JGetObject(J *json, const char *field);
/*!
 @brief Get the integer value of a number field from a JSON object.

 @param json The JSON object to query.
 @param field The field name to retrieve.

 @returns The integer value, or 0 if the field doesn't exist or isn't a number.

 @note The returned value is the integer representation of the number.

 @see `JGetBool`
 @see `JGetNumber`
 */
JINTEGER JGetInt(J *json, const char *field);
/*!
 @brief Get the value of a boolean field from a JSON object.

 @param json The JSON object to query.
 @param field The field name to retrieve.

 @returns The boolean value, or `false` if the field doesn't exist or isn't a boolean.

 @see `JGetInt`
 @see `JGetNumber`
 */
bool JGetBool(J *json, const char *field);
/*!
 @brief Get the numeric value from a JSON item.

 @param item The JSON item to query.

 @returns The numeric value, or 0.0f if the item isn't a number.
 */
JNUMBER JNumberValue(J *item);
/*!
 @brief Get the string value from a JSON item.

 @param item The JSON item to query.

 @returns Pointer to the string value, or empty string if the item isn't a string.
 */
char *JStringValue(J *item);
/*!
 @brief Get the boolean value from a JSON item.

 @param item The JSON item to query.

 @returns The boolean value, or false if the item isn't a boolean.
 */
bool JBoolValue(J *item);
/*!
 @brief Get the integer value from a JSON item.

 @param item The JSON item to query.

 @returns The integer value, or 0 if the item isn't an integer.
 */
JINTEGER JIntValue(J *item);
/*!
 @brief Check if a JSON object field is NULL or an empty string.

 @param json The JSON object to query.
 @param field The field name to check.

 @returns `true` if the field is null, missing, or an empty string; `false` otherwise.
 */
bool JIsNullString(J *json, const char *field);
/*!
 @brief Check if a JSON object field exactly matches a test string.

 @param json The JSON object to query.
 @param field The field name to check.
 @param teststr The string to compare against.

 @returns `true` if the field value exactly matches teststr, `false` otherwise.
 */
bool JIsExactString(J *json, const char *field, const char *teststr);
/*!
 @brief Check if a JSON object field contains a substring.

 @param json The JSON object to query.
 @param field The field name to check.
 @param substr The substring to search for.

 @returns `true` if the field exists and contains the substring, `false` otherwise.
 */
bool JContainsString(J *json, const char *field, const char *substr);
/*!
 @brief Add binary data to a JSON object as a base64-encoded string.

 @param json The JSON object to modify.
 @param fieldName The field name to add.
 @param binaryData A buffer of binary data to encode.
 @param binaryDataLen The length of the binary data in bytes.

 @returns True if the string was successfully encoded and added to the object,
          otherwise false.
 */
bool JAddBinaryToObject(J *json, const char *fieldName, const void *binaryData, uint32_t binaryDataLen);
/*!
 @brief Decode a Base64-encoded string field in a JSON object and return the
        decoded bytes.

 @param json The JSON object to query.
 @param fieldName The field name to retrieve.
 @param retBinaryData A pointer to a pointer used to store the decoded binary
          data (caller must free).
 @param retBinaryDataLen A pointer to an unsigned integer used to store the
          length of the decoded binary data.

 @returns `true` if the binary data was successfully decoded, `false` otherwise.

 @note The returned binary buffer must be freed by the user with `JFree` when it
       is no longer needed.

 @note On error, the returned binary buffer and data length shall be set to
       `NULL` and zero (0), respectively.
 */
bool JGetBinaryFromObject(J *json, const char *fieldName, uint8_t **retBinaryData, uint32_t *retBinaryDataLen);
/*!
 @brief Get the name/key of a JSON item.

 @param item The JSON item to query.

 @returns Pointer to the item's name/key, or the empty string, if the item has no name.
 */
const char *JGetItemName(const J * item);
/*!
 @brief Allocate a new JSON string and copy contents of the buffer.

 @param buffer The buffer containing the string data.
 @param len The length of buffer in bytes.

 @returns If buffer is NULL or length 0, the empty string. If allocation
          fails, NULL. On success, the converted c-string. The returned
          string must be freed with JFree.
 */
char *JAllocString(uint8_t *buffer, uint32_t len);
/*!
 @brief Get a string representation of a JSON item's type.

 @param item The JSON item to query.

 @returns String representation of the item's type (e.g., "number", "string", "object").
 */
const char *JType(J *item);

#define JTYPE_NOT_PRESENT		0
#define JTYPE_BOOL_TRUE			1
#define JTYPE_BOOL_FALSE		2
#define JTYPE_BOOL              JTYPE_BOOL_TRUE
#define JTYPE_NULL				3
#define JTYPE_NUMBER_ZERO		4
#define JTYPE_NUMBER			5
#define JTYPE_STRING_BLANK		6
#define JTYPE_STRING_ZERO		7
#define JTYPE_STRING_NUMBER		8
#define JTYPE_STRING_BOOL_TRUE 	9
#define JTYPE_STRING_BOOL_FALSE	10
#define JTYPE_STRING			11
#define JTYPE_OBJECT			12
#define JTYPE_ARRAY				13
/*!
 @brief  Get the type of a field, as an int usable in a switch statement.

 @param json The JSON object to query.
 @param field The field name to check.

 @returns Type code (JTYPE_* constants), or JTYPE_NOT_PRESENT if the field doesn't exist.
 */
int JGetType(J *json, const char *field);
/*!
 @brief Get the type code of a JSON item.

 @param item The JSON item to query.

 @returns Type code (JTYPE_* constants).
 */
int JGetItemType(J *item);
/*!
 @brief Get the base type from a detailed type code.

 @param type The detailed type code to convert.

 @returns The base type code.
 */
int JBaseItemType(int type);
#define JGetObjectItemName(j) (j->string)

// Helper functions for apps that wish to limit their C library dependencies
#define JNTOA_PRECISION (16)
#define JNTOA_MAX       (44)

/*!
 @brief Convert a number to a string.

 @param f The number to convert.
 @param buf Buffer to store the string result.
 @param precision Number of decimal places for floating point numbers.

 @returns Pointer to the string representation.
 */
char * JNtoA(JNUMBER f, char * buf, int precision);
/*!
 @brief Convert a string to a number.

 @param string The string to convert.
 @param endPtr Pointer to store the position where conversion stopped (optional).

 @returns The numeric value.
 */
JNUMBER JAtoN(const char *string, char **endPtr);
/*!
 @brief Convert an integer to a string.

 @param n The integer to convert.
 @param s Buffer to store the NULL-terminated string result.

 @note The buffer must be large enough because no bounds checking is performed.
 */
void JItoA(JINTEGER n, char *s);
/*!
 @brief Convert a string to an integer.

 @param s A null-terminated text buffer.

 @returns The integer value (0 if invalid)
 */
JINTEGER JAtoI(const char *s);
/*!
 @brief Calculate the length needed for base64 encoding.

 @param len Length of the data to be encoded.

 @returns Length of the base64-encoded string (including null terminator).
 */
int JB64EncodeLen(int len);
/*!
 @brief Encode data to base64.

 @param coded_dst Buffer to store the base64-encoded result.
 @param plain_src Source data to encode.
 @param len_plain_src Length of the source data.

 @returns Length of the encoded string.
 */
int JB64Encode(char * coded_dst, const char *plain_src,int len_plain_src);
/*!
 @brief Calculate the length of data after base64 decoding.

 @param coded_src The base64-encoded string.

 @returns Length of the decoded data.
 */
int JB64DecodeLen(const char * coded_src);
/*!
 @brief Decode base64 data.

 @param plain_dst Buffer to store the decoded result.
 @param coded_src The base64-encoded string to decode.

 @returns Length of the decoded data.
 */
int JB64Decode(char * plain_dst, const char *coded_src);

// MD5 Helper functions

/*!
 @brief MD5 context structure for incremental MD5 hashing.
 */
typedef struct {
    unsigned long buf[4];
    unsigned long bits[2];
    unsigned char in[64];
} NoteMD5Context;
#define NOTE_MD5_HASH_SIZE 16
#define NOTE_MD5_HASH_STRING_SIZE (((NOTE_MD5_HASH_SIZE)*2)+1)

/*!
 @brief Initialize an MD5 context for hashing.

 @param ctx The MD5 context to initialize.
 */
void NoteMD5Init(NoteMD5Context *ctx);
/*!
 @brief Update an MD5 context with additional data.

 @param ctx The MD5 context to update.
 @param buf Buffer containing data to hash.
 @param len Length of the data in bytes.
 */
void NoteMD5Update(NoteMD5Context *ctx, unsigned char const *buf, unsigned long len);
/*!
 @brief Finalize an MD5 hash and get the digest.

 @param digest Buffer to store the 16-byte MD5 digest.
 @param ctx The MD5 context to finalize.
 */
void NoteMD5Final(unsigned char *digest, NoteMD5Context *ctx);
/*!
 @brief Internal MD5 transformation function.

 @param buf MD5 state buffer.
 @param inraw 64-byte input block.
 */
void NoteMD5Transform(unsigned long buf[4], const unsigned char inraw[64]);
/*!
 @brief Compute MD5 hash of data in a single call.

 @param data Data to hash.
 @param len Length of the data in bytes.
 @param retHash Buffer to store the 16-byte MD5 digest.
 */
void NoteMD5Hash(unsigned char* data, unsigned long len, unsigned char *retHash);
/*!
 @brief Compute MD5 hash of data and return it as a hex string.

 @param data Data to hash.
 @param len Length of the data in bytes.
 @param strbuf Buffer to store the hex string representation.
 @param buflen Length of the string buffer.
 */
void NoteMD5HashString(unsigned char *data, unsigned long len, char *strbuf, unsigned long buflen);
/*!
 @brief Convert an MD5 hash to a hex string.

 @param hash 16-byte MD5 digest to convert.
 @param strbuf Buffer to store the hex string representation.
 @param buflen Length of the string buffer.
 */
void NoteMD5HashToString(unsigned char *hash, char *strbuf, unsigned long buflen);

// High-level helper functions that are both useful and serve to show developers
// how to call the API

/*!
 @brief Configure the flow control for the auxiliary serial port.

 @param bufSize The size of the receive buffer.
 @param delayMs The time to wait between reads.

 @returns `true` on success, `false` on failure.
 */
bool NoteAuxSerialFlowControl(int bufSize, int delayMs);
/*!
 @brief Decode binary data using the Notecard's binary encoding.

 @param encData Pointer to the encoded data.
 @param encDataLen Length of the encoded data.
 @param decBuf Buffer to store the decoded data.
 @param decBufSize Size of the decode buffer.

 @returns Number of bytes decoded, or 0 on error.
 */
uint32_t NoteBinaryCodecDecode(const uint8_t *encData, uint32_t encDataLen,
                               uint8_t *decBuf, uint32_t decBufSize);
/*!
 @brief Encode binary data using the Notecard's binary encoding.

 @param decData Pointer to the data to encode.
 @param decDataLen Length of the data to encode.
 @param encBuf Buffer to store the encoded data.
 @param encBufSize Size of the encode buffer.

 @returns Number of bytes encoded, or 0 on error.
 */
uint32_t NoteBinaryCodecEncode(const uint8_t *decData, uint32_t decDataLen,
                               uint8_t *encBuf, uint32_t encBufSize);
/*!
 @brief Calculate the maximum decoded length for a given buffer size.

 @param bufferSize The size of the encoded buffer.

 @returns Maximum number of bytes that can be decoded from the buffer.
 */
uint32_t NoteBinaryCodecMaxDecodedLength(uint32_t bufferSize);
/*!
 @brief Calculate the maximum encoded length for given data.

 @param unencodedLength The length of the data to be encoded.

 @returns Maximum number of bytes needed to encode the data.
 */
uint32_t NoteBinaryCodecMaxEncodedLength(uint32_t unencodedLength);
/*!
 @brief Get the decoded length of data in the binary store.

 @param len Pointer to store the decoded length.

 @returns NULL on success, error string on failure.
 */
const char * NoteBinaryStoreDecodedLength(uint32_t *len);
/*!
 @brief Get the encoded length of data in the binary store.

 @param len Pointer to store the encoded length.

 @returns NULL on success, error string on failure.
 */
const char * NoteBinaryStoreEncodedLength(uint32_t *len);
/*!
 @brief Receive data from the binary store.

 @param buffer Buffer to store the received data.
 @param bufLen Size of the buffer.
 @param decodedOffset Offset in the decoded data to start from.
 @param decodedLen Number of decoded bytes to receive.

 @returns NULL on success, error string on failure.
 */
const char * NoteBinaryStoreReceive(uint8_t *buffer, uint32_t bufLen,
                                    uint32_t decodedOffset, uint32_t decodedLen);
/*!
 @brief Reset the binary store.

 @returns NULL on success, error string on failure.
 */
const char * NoteBinaryStoreReset(void);
/*!
 @brief Transmit data to the binary store.

 @param unencodedData Pointer to the data to transmit.
 @param unencodedLen Length of the data to transmit.
 @param bufLen Size of the transmission buffer.
 @param notecardOffset Offset in the Notecard's storage.

 @returns NULL on success, error string on failure.
 */
const char * NoteBinaryStoreTransmit(uint8_t *unencodedData, uint32_t unencodedLen,
                                     uint32_t bufLen, uint32_t notecardOffset);
/*!
 @brief Set the session time in seconds.

 @param secs The session time to set in seconds.

 @returns The previous session time value.
 */
uint32_t NoteSetSTSecs(uint32_t secs);
/*!
 @brief Check if the Notecard's time is valid.

 @returns `true` if the time is valid, `false` otherwise.
 */
bool NoteTimeValid(void);
/*!
 @brief Check if the Notecard's session time is valid.

 @returns `true` if the session time is valid, `false` otherwise.
 */
bool NoteTimeValidST(void);
/*!
 @brief Get the current time from the Notecard.

 @returns Current time as seconds since Unix epoch, or 0 if time is invalid.
 */
JTIME NoteTime(void);
/*!
 @brief Get the current session time from the Notecard.

 @returns Current session time as seconds since Unix epoch, or 0 if time is invalid.
 */
JTIME NoteTimeST(void);
/*!
 @brief Set the time refresh interval.

 @param mins Refresh interval in minutes.
 */
void NoteTimeRefreshMins(uint32_t mins);
/*!
 @brief Set the time and timezone information.

 @param secondsUTC Time in seconds since Unix epoch.
 @param offset Timezone offset in minutes from UTC.
 @param zone Timezone name (e.g., "PST").
 @param country Country name.
 @param area Area/city name.
 */
void NoteTimeSet(JTIME secondsUTC, int offset, char *zone, char *country, char *area);
/*!
 @brief Get the local time in a structured format (session time).

 @param retYear Pointer to store the year.
 @param retMonth Pointer to store the month (1-12).
 @param retDay Pointer to store the day (1-31).
 @param retHour Pointer to store the hour (0-23).
 @param retMinute Pointer to store the minute (0-59).
 @param retSecond Pointer to store the second (0-59).
 @param retWeekday Pointer to store the weekday name.
 @param retZone Pointer to store the timezone name.

 @returns `true` if the time was successfully retrieved, `false` otherwise.
 */
bool NoteLocalTimeST(uint16_t *retYear, uint8_t *retMonth, uint8_t *retDay, uint8_t *retHour, uint8_t *retMinute, uint8_t *retSecond, char **retWeekday, char **retZone);
/*!
 @brief Get the current region/timezone information.

 @param retCountry Pointer to store the country name.
 @param retArea Pointer to store the area/city name.
 @param retZone Pointer to store the timezone name.
 @param retZoneOffset Pointer to store the timezone offset in minutes from UTC.

 @returns `true` if the region information was successfully retrieved, `false` otherwise.
 */
bool NoteRegion(char **retCountry, char **retArea, char **retZone, int *retZoneOffset);
/*!
 @brief Check if the Notecard's location is valid.

 @param errbuf Buffer to store error message if location is invalid.
 @param errbuflen Size of the error buffer.

 @returns `true` if the location is valid, `false` otherwise.
 */
bool NoteLocationValid(char *errbuf, uint32_t errbuflen);
/*!
 @brief Check if the Notecard's session-time location is valid.

 @param errbuf Buffer to store error message if location is invalid.
 @param errbuflen Size of the error buffer.

 @returns `true` if the location is valid, `false` otherwise.
 */
bool NoteLocationValidST(char *errbuf, uint32_t errbuflen);
/*!
 @brief Enable or disable turbo I/O mode (deprecated).

 @param enable True to enable turbo mode, false to disable.

 @deprecated This function is deprecated and should not be used.
 */
NOTE_C_DEPRECATED void NoteTurboIO(bool enable);
/*!
 @brief Get an integer environment variable from the Notecard.

 @param variable The name of the environment variable.
 @param defaultVal Default value to return if the variable doesn't exist.

 @returns The integer value of the environment variable, or defaultVal if not found.
 */
JINTEGER NoteGetEnvInt(const char *variable, JINTEGER defaultVal);
/*!
 @brief Get a numeric environment variable from the Notecard.

 @param variable The name of the environment variable.
 @param defaultVal Default value to return if the variable doesn't exist.

 @returns The numeric value of the environment variable, or defaultVal if not found.
 */
JNUMBER NoteGetEnvNumber(const char *variable, JNUMBER defaultVal);
/*!
 @brief Get a string environment variable from the Notecard.

 @param variable The name of the environment variable.
 @param defaultVal Default value to use if the variable doesn't exist.
 @param buf Buffer to store the result.
 @param buflen Size of the buffer.

 @returns `true` if the variable was successfully retrieved, `false` otherwise.
 */
bool NoteGetEnv(const char *variable, const char *defaultVal, char *buf, uint32_t buflen);
/*!
 @brief Set a default string environment variable on the Notecard.

 @param variable The name of the environment variable to set.
 @param buf The default value to set.

 @returns `true` if the variable was successfully set, `false` otherwise.
 */
bool NoteSetEnvDefault(const char *variable, char *buf);
/*!
 @brief Set a default numeric environment variable on the Notecard.

 @param variable The name of the environment variable to set.
 @param defaultVal The default numeric value to set.

 @returns `true` if the variable was successfully set, `false` otherwise.
 */
bool NoteSetEnvDefaultNumber(const char *variable, JNUMBER defaultVal);
/*!
 @brief Set a default integer environment variable on the Notecard.

 @param variable The name of the environment variable to set.
 @param defaultVal The default integer value to set.

 @returns `true` if the variable was successfully set, `false` otherwise.
 */
bool NoteSetEnvDefaultInt(const char *variable, JINTEGER defaultVal);
/*!
 @brief Check if the Notecard is connected to the network.

 @returns `true` if connected, `false` otherwise.
 */
bool NoteIsConnected(void);
/*!
 @brief Check if the Notecard is connected to the network (session time).

 @returns `true` if connected, `false` otherwise.
 */
bool NoteIsConnectedST(void);
/*!
 @brief Get the network status from the Notecard.

 @param statusBuf Buffer to store the status string.
 @param statusBufLen Size of the status buffer.

 @returns `true` if the status was successfully retrieved, `false` otherwise.
 */
bool NoteGetNetStatus(char *statusBuf, int statusBufLen);
/*!
 @brief Get the firmware version from the Notecard.

 @param versionBuf Buffer to store the version string.
 @param versionBufLen Size of the version buffer.

 @returns `true` if the version was successfully retrieved, `false` otherwise.
 */
bool NoteGetVersion(char *versionBuf, int versionBufLen);
/*!
 @brief Get the current location from the Notecard.

 @param retLat Pointer to store the latitude.
 @param retLon Pointer to store the longitude.
 @param time Pointer to store the time when location was acquired.
 @param statusBuf Buffer to store any status message.
 @param statusBufLen Size of the status buffer.

 @returns `true` if the location was successfully retrieved, `false` otherwise.
 */
bool NoteGetLocation(JNUMBER *retLat, JNUMBER *retLon, JTIME *time, char *statusBuf, int statusBufLen);
/*!
 @brief Set the location on the Notecard.

 @param lat Latitude to set.
 @param lon Longitude to set.

 @returns `true` if the location was successfully set, `false` otherwise.
 */
bool NoteSetLocation(JNUMBER lat, JNUMBER lon);
/*!
 @brief Clear the stored location on the Notecard.

 @returns `true` if the location was successfully cleared, `false` otherwise.
 */
bool NoteClearLocation(void);
/*!
 @brief Get the current location mode from the Notecard.

 @param modeBuf Buffer to store the location mode string.
 @param modeBufLen Size of the mode buffer.

 @returns `true` if the location mode was successfully retrieved, `false` otherwise.
 */
bool NoteGetLocationMode(char *modeBuf, int modeBufLen);
/*!
 @brief Set the location mode on the Notecard.

 @param mode The location mode to set (e.g., "periodic", "continuous").
 @param seconds Update interval in seconds (for periodic mode).

 @returns `true` if the location mode was successfully set, `false` otherwise.
 */
bool NoteSetLocationMode(const char *mode, uint32_t seconds);
/*!
 @brief Get the service configuration from the Notecard.

 @param productBuf Buffer to store the product ID.
 @param productBufLen Size of the product buffer.
 @param serviceBuf Buffer to store the service name.
 @param serviceBufLen Size of the service buffer.
 @param deviceBuf Buffer to store the device ID.
 @param deviceBufLen Size of the device buffer.
 @param snBuf Buffer to store the serial number.
 @param snBufLen Size of the serial number buffer.

 @returns `true` if the configuration was successfully retrieved, `false` otherwise.
 */
bool NoteGetServiceConfig(char *productBuf, int productBufLen, char *serviceBuf, int serviceBufLen, char *deviceBuf, int deviceBufLen, char *snBuf, int snBufLen);
/*!
 @brief Get the service configuration from the Notecard (session time).

 @param productBuf Buffer to store the product ID.
 @param productBufLen Size of the product buffer.
 @param serviceBuf Buffer to store the service name.
 @param serviceBufLen Size of the service buffer.
 @param deviceBuf Buffer to store the device ID.
 @param deviceBufLen Size of the device buffer.
 @param snBuf Buffer to store the serial number.
 @param snBufLen Size of the serial number buffer.

 @returns `true` if the configuration was successfully retrieved, `false` otherwise.
 */
bool NoteGetServiceConfigST(char *productBuf, int productBufLen, char *serviceBuf, int serviceBufLen, char *deviceBuf, int deviceBufLen, char *snBuf, int snBufLen);
/*!
 @brief Get the status information from the Notecard.

 @param statusBuf Buffer to store the status string.
 @param statusBufLen Size of the status buffer.
 @param bootTime Pointer to store the boot time.
 @param retUSB Pointer to store the USB connection status.
 @param retSignals Pointer to store the signal information status.

 @returns `true` if the status was successfully retrieved, `false` otherwise.
 */
bool NoteGetStatus(char *statusBuf, int statusBufLen, JTIME *bootTime, bool *retUSB, bool *retSignals);
/*!
 @brief Get the status information from the Notecard (session time).

 @param statusBuf Buffer to store the status string.
 @param statusBufLen Size of the status buffer.
 @param bootTime Pointer to store the boot time.
 @param retUSB Pointer to store the USB connection status.
 @param retSignals Pointer to store the signal information status.

 @returns `true` if the status was successfully retrieved, `false` otherwise.
 */
bool NoteGetStatusST(char *statusBuf, int statusBufLen, JTIME *bootTime, bool *retUSB, bool *retSignals);
/*!
 @brief Put the Notecard into sleep mode.

 @param stateb64 Base64-encoded state to preserve during sleep.
 @param seconds Number of seconds to sleep.
 @param modes Sleep mode configuration string.

 @returns `true` if the sleep command was successful, `false` otherwise.
 */
bool NoteSleep(char *stateb64, uint32_t seconds, const char *modes);
/*!
 @brief Wake the Notecard from sleep mode and retrieve preserved state.

 @param stateLen Length of the state buffer.
 @param state Buffer to store the preserved state.

 @returns `true` if the wake operation was successful, `false` otherwise.
 */
bool NoteWake(int stateLen, void *state);
/*!
 @brief Perform a factory reset of the Notecard.

 @param deleteConfigSettings Whether to delete configuration settings.

 @returns `true` if the reset was successful, `false` otherwise.
 */
bool NoteFactoryReset(bool deleteConfigSettings);
/*!
 @brief Set the serial number on the Notecard.

 @param sn The serial number to set.

 @returns `true` if the serial number was successfully set, `false` otherwise.
 */
bool NoteSetSerialNumber(const char *sn);
/*!
 @brief Set the product ID on the Notecard.

 @param productID The product ID to set.

 @returns `true` if the product ID was successfully set, `false` otherwise.
 */
bool NoteSetProductID(const char *productID);
/*!
 @brief Set the upload mode for the Notecard.

 @param uploadMode The upload mode to set (e.g., "periodic", "continuous").
 @param uploadMinutes Upload interval in minutes.
 @param align Whether to align upload times.

 @returns `true` if the upload mode was successfully set, `false` otherwise.
 */
bool NoteSetUploadMode(const char *uploadMode, int uploadMinutes, bool align);
/*!
 @brief Set the synchronization mode for the Notecard.

 @param uploadMode The upload mode to set.
 @param uploadMinutes Upload interval in minutes.
 @param downloadMinutes Download interval in minutes.
 @param align Whether to align sync times.
 @param sync Whether to perform an immediate sync.

 @returns `true` if the sync mode was successfully set, `false` otherwise.
 */
bool NoteSetSyncMode(const char *uploadMode, int uploadMinutes, int downloadMinutes, bool align, bool sync);
#define NoteSend NoteAdd

/*!
 @brief Add a note to a notefile for transmission.

 @param target The target notefile name.
 @param body JSON object containing the note body (will be consumed/freed).
 @param urgent Whether this is an urgent note that should be sent immediately.

 @returns `true` if the note was successfully added, `false` otherwise.
 */
bool NoteAdd(const char *target, J *body, bool urgent);
/*!
 @brief Send a body to a route using an HTTP request.

 Body is freed, regardless of success.

 @param method HTTP method to use, "get", "post" or "put".
 @param routeAlias The Notehub Route alias.
 @param notefile The Notefile name.
 @param body The request JSON body (will be consumed/freed).

 @returns `true` if request was successful, `false` otherwise.
 */
bool NoteSendToRoute(const char *method, const char *routeAlias, char *notefile, J *body);
/*!
 @brief Get the current voltage reading from the Notecard.

 @param voltage Pointer to store the voltage value.

 @returns `true` if the voltage was successfully retrieved, `false` otherwise.
 */
bool NoteGetVoltage(JNUMBER *voltage);
/*!
 @brief Get the current temperature reading from the Notecard.

 @param temp Pointer to store the temperature value in Celsius.

 @returns `true` if the temperature was successfully retrieved, `false` otherwise.
 */
bool NoteGetTemperature(JNUMBER *temp);
/*!
 @brief Get the contact information from the Notecard.

 @param nameBuf Buffer to store the contact name.
 @param nameBufLen Size of the name buffer.
 @param orgBuf Buffer to store the organization.
 @param orgBufLen Size of the organization buffer.
 @param roleBuf Buffer to store the role.
 @param roleBufLen Size of the role buffer.
 @param emailBuf Buffer to store the email address.
 @param emailBufLen Size of the email buffer.

 @returns `true` if the contact information was successfully retrieved, `false` otherwise.
 */
bool NoteGetContact(char *nameBuf, int nameBufLen, char *orgBuf, int orgBufLen, char *roleBuf, int roleBufLen, char *emailBuf, int emailBufLen);
/*!
 @brief Set the contact information on the Notecard.

 @param nameBuf The contact name to set.
 @param orgBuf The organization to set.
 @param roleBuf The role to set.
 @param emailBuf The email address to set.

 @returns `true` if the contact information was successfully set, `false` otherwise.
 */
bool NoteSetContact(char *nameBuf, char *orgBuf, char *roleBuf, char *emailBuf);

// Definitions necessary for payload descriptor
#define NP_SEGTYPE_LEN 4
#define NP_SEGLEN_LEN sizeof(uint32_t)
#define NP_SEGHDR_LEN (NP_SEGTYPE_LEN + NP_SEGLEN_LEN)

/*!
 @brief Structure for managing payload data with segments.
 */
typedef struct {
    uint8_t *data;    /*!< Pointer to the payload data */
    uint32_t alloc;   /*!< Allocated size of the data buffer */
    uint32_t length;  /*!< Current length of data in the buffer */
} NotePayloadDesc;

/*!
 @brief Save payload data and put the Notecard to sleep.

 @param desc The payload descriptor containing data to save.
 @param seconds Number of seconds to sleep.
 @param modes Sleep mode configuration string.

 @returns `true` if the operation was successful, `false` otherwise.
 */
bool NotePayloadSaveAndSleep(NotePayloadDesc *desc, uint32_t seconds, const char *modes);
/*!
 @brief Retrieve payload data after waking from sleep.

 @param desc The payload descriptor to populate with retrieved data.

 @returns `true` if the payload was successfully retrieved, `false` otherwise.
 */
bool NotePayloadRetrieveAfterSleep(NotePayloadDesc *desc);
/*!
 @brief Set the buffer for a payload descriptor.

 @param desc The payload descriptor to configure.
 @param buf Pointer to the buffer to use.
 @param buflen Size of the buffer.
 */
void NotePayloadSet(NotePayloadDesc *desc, uint8_t *buf, uint32_t buflen);
/*!
 @brief Free the memory allocated for a payload descriptor.

 @param desc The payload descriptor to free.
 */
void NotePayloadFree(NotePayloadDesc *desc);
/*!
 @brief Add a data segment to a payload.

 @param desc The payload descriptor to modify.
 @param segtype 4-character segment type identifier.
 @param pdata Pointer to the segment data.
 @param plen Length of the segment data.

 @returns `true` if the segment was successfully added, `false` otherwise.
 */
bool NotePayloadAddSegment(NotePayloadDesc *desc, const char segtype[NP_SEGTYPE_LEN], void *pdata, uint32_t plen);
/*!
 @brief Find a data segment in a payload.

 @param desc The payload descriptor to search.
 @param segtype 4-character segment type identifier to find.
 @param pdata Buffer to copy the segment data to.
 @param plen Pointer to the length of the segment data.

 @returns `true` if the segment was found, `false` otherwise.
 */
bool NotePayloadFindSegment(NotePayloadDesc *desc, const char segtype[NP_SEGTYPE_LEN], void *pdata, uint32_t *plen);
/*!
 @brief Get a data segment from a payload.

 @param desc The payload descriptor to search.
 @param segtype 4-character segment type identifier to get.
 @param pdata Buffer to copy the segment data to.
 @param len Maximum length of data to copy.

 @returns `true` if the segment was found and copied, `false` otherwise.
 */
bool NotePayloadGetSegment(NotePayloadDesc *desc, const char segtype[NP_SEGTYPE_LEN], void *pdata, uint32_t len);

// Hard-wired constants used to specify field types when creating note templates
#define TBOOL           true                 // bool
#define TINT8           11                   // 1-byte signed integer
#define TINT16          12                   // 2-byte signed integer
#define TINT24          13                   // 3-byte signed integer
#define TINT32          14                   // 4-byte signed integer
#define TINT64          18                   // 8-byte signed integer (note-c support depends upon platform)
#define TUINT8          21                   // 1-byte unsigned integer (requires notecard firmware >= build 14444)
#define TUINT16         22                   // 2-byte unsigned integer (requires notecard firmware >= build 14444)
#define TUINT24         23                   // 3-byte unsigned integer (requires notecard firmware >= build 14444)
#define TUINT32         24                   // 4-byte unsigned integer (requires notecard firmware >= build 14444)
#define TFLOAT16        12.1                 // 2-byte IEEE 754 floating point
#define TFLOAT32        14.1                 // 4-byte IEEE 754 floating point (a.k.a. "float")
#define TFLOAT64        18.1                 // 8-byte IEEE 754 floating point (a.k.a. "double")
#define TSTRING(N)      _NOTE_C_STRINGIZE(N) // UTF-8 text of N bytes maximum (fixed-length reserved buffer)
#define TSTRINGV        _NOTE_C_STRINGIZE(0) // variable-length string

/*!
 @brief Set a template for a notefile to optimize transmission.

 Templates allow the Notecard to send only changed values, reducing
 bandwidth usage.

 @param notefileID The name of the notefile to create a template for.
 @param templateBody JSON object describing the template structure (will be consumed/freed).

 @returns `true` if the template was successfully set, `false` otherwise.
 */
bool NoteTemplate(const char *notefileID, J *templateBody);

// End of C-callable functions
#ifdef __cplusplus
}
#endif
