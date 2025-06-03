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
#define NOTE_C_VERSION_PATCH 1

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
 @typedef mutexFn

 @brief The type for the various mutex (i.e. lock/unlock) hooks.
 */
typedef void (*mutexFn) (void);
/*!
 @typedef mallocFn

 @brief The type for the memory allocation hook.

 @param size The number of bytes to allocate.

 @returns A pointer to the newly allocated memory or NULL on failure.
 */
typedef void * (*mallocFn) (size_t size);
/*!
 @typedef freeFn

 @brief The type for the memory freeing hook.

 @param mem Pointer to the memory to free.
 */
typedef void (*freeFn) (void * mem);
/*!
 @typedef delayMsFn

 @brief The type for the millisecond delay hook.

 @param ms The number of milliseconds to delay for.
 */
typedef void (*delayMsFn) (uint32_t ms);
/*!
 @typedef getMsFn

 @brief The type for the millisecond counter hook.

 @returns The value of the millisecond counter.
 */
typedef uint32_t (*getMsFn) (void);
typedef size_t (*debugOutputFn) (const char *text);

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
typedef bool (*txnStartFn) (uint32_t timeoutMs);
typedef void (*txnStopFn) (void);

// External API
bool NoteReset(void);
void NoteResetRequired(void);
#define NoteNewBody JCreateObject
#define NoteAddBodyToObject(a, b) JAddItemToObject(a, "body", b)
#define NoteGetBody(a) JGetObject(a, "body")
J *NoteNewRequest(const char *request);
J *NoteNewCommand(const char *request);
J *NoteRequestResponse(J *req);
J *NoteRequestResponseWithRetry(J *req, uint32_t timeoutSeconds);
char * NoteRequestResponseJSON(const char *reqJSON);
NOTE_C_DEPRECATED void NoteSuspendTransactionDebug(void);
NOTE_C_DEPRECATED void NoteResumeTransactionDebug(void);
#define SYNCSTATUS_LEVEL_MAJOR         0
#define SYNCSTATUS_LEVEL_MINOR         1
#define SYNCSTATUS_LEVEL_DETAILED      2
#define SYNCSTATUS_LEVEL_ALGORITHMIC   3
#define SYNCSTATUS_LEVEL_ALL          -1
bool NoteDebugSyncStatus(int pollFrequencyMs, int maxLevel);
bool NoteRequest(J *req);
bool NoteRequestWithRetry(J *req, uint32_t timeoutSeconds);
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
J *NoteTransaction(J *req);
bool NoteErrorContains(const char *errstr, const char *errtype);
void NoteErrorClean(char *errbuf);
void NoteSetFnDebugOutput(debugOutputFn fn);
void NoteGetFnDebugOutput(debugOutputFn *fn);
void NoteSetFnTransaction(txnStartFn startFn, txnStopFn stopFn);
void NoteGetFnTransaction(txnStartFn *startFn, txnStopFn *stopFn);
void NoteSetFnMutex(mutexFn lockI2Cfn, mutexFn unlockI2Cfn, mutexFn lockNotefn,
                    mutexFn unlockNotefn);
void NoteGetFnMutex(mutexFn *lockI2Cfn, mutexFn *unlockI2Cfn, mutexFn *lockNotefn,
                    mutexFn *unlockNotefn);
void NoteSetFnI2CMutex(mutexFn lockI2Cfn, mutexFn unlockI2Cfn);
void NoteGetFnI2CMutex(mutexFn *lockI2Cfn, mutexFn *unlockI2Cfn);
void NoteSetFnNoteMutex(mutexFn lockFn, mutexFn unlockFn);
void NoteGetFnNoteMutex(mutexFn *lockFn, mutexFn *unlockFn);
void NoteSetFnDefault(mallocFn mallocfn, freeFn freefn, delayMsFn delayfn,
                      getMsFn millisfn);
void NoteSetFn(mallocFn mallocHook, freeFn freeHook, delayMsFn delayMsHook,
               getMsFn getMsHook);
void NoteGetFn(mallocFn *mallocHook, freeFn *freeHook, delayMsFn *delayMsHook,
               getMsFn *getMsHook);
void NoteSetFnSerial(serialResetFn resetFn, serialTransmitFn transmitFn,
                     serialAvailableFn availFn, serialReceiveFn receiveFn);
void NoteSetFnSerialDefault(serialResetFn resetFn, serialTransmitFn transmitFn,
                            serialAvailableFn availFn, serialReceiveFn receiveFn);
void NoteGetFnSerial(serialResetFn *resetFn, serialTransmitFn *transmitFn,
                     serialAvailableFn *availFn, serialReceiveFn *receiveFn);
void NoteSetFnI2C(uint32_t notecardAddr, uint32_t maxTransmitSize,
                  i2cResetFn resetFn, i2cTransmitFn transmitFn,
                  i2cReceiveFn receiveFn);
void NoteSetFnI2cDefault(uint32_t notecardAddr, uint32_t maxTransmitSize,
                         i2cResetFn resetFn, i2cTransmitFn transmitFn,
                         i2cReceiveFn receiveFn);
void NoteGetFnI2C(uint32_t *notecardAddr, uint32_t *maxTransmitSize,
                  i2cResetFn *resetFn, i2cTransmitFn *transmitFn,
                  i2cReceiveFn *receiveFn);
void NoteSetActiveInterface(int interface);
int NoteGetActiveInterface(void);
void NoteSetFnDisabled(void);
void NoteSetI2CAddress(uint32_t i2cAddr);
void NoteGetI2CAddress(uint32_t *i2cAddr);

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
#define NOTE_I2C_HEADER_SIZE 2
#endif

// Maximum bytes capable of being transmitted in a single read/write operation
#ifndef NOTE_I2C_MAX_MAX
#define NOTE_I2C_MAX_MAX (UCHAR_MAX - NOTE_I2C_HEADER_SIZE)
#endif

// In ARDUINO implementations, which to date is the largest use of this library,
// the Wire package is implemented in a broad variety of ways by different
// vendors.  The default implementation has a mere 32-byte static I2C buffer,
// which means that the maximum to be transmitted in a single I/O (given our
// 2-byte serial-to-i2c protocol header) is 30 bytes.  However, if we know
// the specific platform (such as STM32DUINO) we can relax this restriction.
#if defined(NOTE_I2C_MAX_DEFAULT)
// user is overriding it at compile time
#elif defined(ARDUINO_ARCH_STM32)
// we know that stm32duino dynamically allocates I/O buffer
#define NOTE_I2C_MAX_DEFAULT NOTE_I2C_MAX_MAX
#else
// default to what's known to be safe for all Arduino implementations
/*!
 @brief The maximum number of bytes to request from or transmit to the Notecard
        in a single chunk.
 */
#define NOTE_I2C_MAX_DEFAULT	30
#endif

// User agent
J *NoteUserAgent(void);
void NoteUserAgentUpdate(J *ua);
void NoteSetUserAgent(char *agent);
void NoteSetUserAgentOS(char *os_name, char *os_platform, char *os_family, char *os_version);
void NoteSetUserAgentCPU(int cpu_mem, int cpu_mhz, int cpu_cores, char *cpu_vendor, char *cpu_name);

// Calls to the functions set above
void NoteDebug(const char *message);
void NoteDebugln(const char *message);
void NoteDebugIntln(const char *message, int n);
void NoteDebugf(const char *format, ...);

#define NOTE_C_LOG_LEVEL_ERROR  0
#define NOTE_C_LOG_LEVEL_WARN   1
#define NOTE_C_LOG_LEVEL_INFO   2
#define NOTE_C_LOG_LEVEL_DEBUG  3

void NoteDebugWithLevel(uint8_t level, const char *msg);
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
void NoteSetLogLevel(int level);

void *NoteMalloc(size_t size);
void NoteFree(void *);
uint32_t NoteGetMs(void);
void NoteDelayMs(uint32_t ms);
void NoteLockI2C(void);
void NoteUnlockI2C(void);
uint32_t NoteI2CAddress(void);
uint32_t NoteI2CMax(void);
uint32_t NoteMemAvailable(void);
bool NotePrint(const char *text);
void NotePrintln(const char *line);
bool NotePrintf(const char *format, ...);

// String helpers to help encourage the world to abandon the horribly-error-prone strn*
size_t strlcpy(char *dst, const char *src, size_t siz);
size_t strlcat(char *dst, const char *src, size_t siz);

// JSON helpers
void JInit(void);
void JCheck(void);
bool JIsPresent(J *json, const char *field);
char *JGetString(J *json, const char *field);
JNUMBER JGetNumber(J *json, const char *field);
J *JGetArray(J *json, const char *field);
J *JGetObject(J *json, const char *field);
JINTEGER JGetInt(J *json, const char *field);
bool JGetBool(J *json, const char *field);
JNUMBER JNumberValue(J *item);
char *JStringValue(J *item);
bool JBoolValue(J *item);
JINTEGER JIntValue(J *item);
bool JIsNullString(J *json, const char *field);
bool JIsExactString(J *json, const char *field, const char *teststr);
bool JContainsString(J *json, const char *field, const char *substr);
bool JAddBinaryToObject(J *json, const char *fieldName, const void *binaryData, uint32_t binaryDataLen);
bool JGetBinaryFromObject(J *json, const char *fieldName, uint8_t **retBinaryData, uint32_t *retBinaryDataLen);
const char *JGetItemName(const J * item);
char *JAllocString(uint8_t *buffer, uint32_t len);
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
int JGetType(J *json, const char *field);
int JGetItemType(J *item);
int JBaseItemType(int type);
#define JGetObjectItemName(j) (j->string)

// Helper functions for apps that wish to limit their C library dependencies
#define JNTOA_PRECISION (16)
#define JNTOA_MAX       (44)
char * JNtoA(JNUMBER f, char * buf, int precision);
JNUMBER JAtoN(const char *string, char **endPtr);
void JItoA(JINTEGER n, char *s);
JINTEGER JAtoI(const char *s);
int JB64EncodeLen(int len);
int JB64Encode(char * coded_dst, const char *plain_src,int len_plain_src);
int JB64DecodeLen(const char * coded_src);
int JB64Decode(char * plain_dst, const char *coded_src);

// MD5 Helper functions
typedef struct {
    unsigned long buf[4];
    unsigned long bits[2];
    unsigned char in[64];
} NoteMD5Context;
#define NOTE_MD5_HASH_SIZE 16
#define NOTE_MD5_HASH_STRING_SIZE (((NOTE_MD5_HASH_SIZE)*2)+1)
void NoteMD5Init(NoteMD5Context *ctx);
void NoteMD5Update(NoteMD5Context *ctx, unsigned char const *buf, unsigned long len);
void NoteMD5Final(unsigned char *digest, NoteMD5Context *ctx);
void NoteMD5Transform(unsigned long buf[4], const unsigned char inraw[64]);
void NoteMD5Hash(unsigned char* data, unsigned long len, unsigned char *retHash);
void NoteMD5HashString(unsigned char *data, unsigned long len, char *strbuf, unsigned long buflen);
void NoteMD5HashToString(unsigned char *hash, char *strbuf, unsigned long buflen);

// High-level helper functions that are both useful and serve to show developers
// how to call the API
bool NoteAuxSerialFlowControl(int bufSize, int delayMs);
uint32_t NoteBinaryCodecDecode(const uint8_t *encData, uint32_t encDataLen,
                               uint8_t *decBuf, uint32_t decBufSize);
uint32_t NoteBinaryCodecEncode(const uint8_t *decData, uint32_t decDataLen,
                               uint8_t *encBuf, uint32_t encBufSize);
uint32_t NoteBinaryCodecMaxDecodedLength(uint32_t bufferSize);
uint32_t NoteBinaryCodecMaxEncodedLength(uint32_t unencodedLength);
const char * NoteBinaryStoreDecodedLength(uint32_t *len);
const char * NoteBinaryStoreEncodedLength(uint32_t *len);
const char * NoteBinaryStoreReceive(uint8_t *buffer, uint32_t bufLen,
                                    uint32_t decodedOffset, uint32_t decodedLen);
const char * NoteBinaryStoreReset(void);
const char * NoteBinaryStoreTransmit(uint8_t *unencodedData, uint32_t unencodedLen,
                                     uint32_t bufLen, uint32_t notecardOffset);
uint32_t NoteSetSTSecs(uint32_t secs);
bool NoteTimeValid(void);
bool NoteTimeValidST(void);
JTIME NoteTime(void);
JTIME NoteTimeST(void);
void NoteTimeRefreshMins(uint32_t mins);
void NoteTimeSet(JTIME secondsUTC, int offset, char *zone, char *country, char *area);
bool NoteLocalTimeST(uint16_t *retYear, uint8_t *retMonth, uint8_t *retDay, uint8_t *retHour, uint8_t *retMinute, uint8_t *retSecond, char **retWeekday, char **retZone);
bool NoteRegion(char **retCountry, char **retArea, char **retZone, int *retZoneOffset);
bool NoteLocationValid(char *errbuf, uint32_t errbuflen);
bool NoteLocationValidST(char *errbuf, uint32_t errbuflen);
NOTE_C_DEPRECATED void NoteTurboIO(bool enable);
JINTEGER NoteGetEnvInt(const char *variable, JINTEGER defaultVal);
JNUMBER NoteGetEnvNumber(const char *variable, JNUMBER defaultVal);
bool NoteGetEnv(const char *variable, const char *defaultVal, char *buf, uint32_t buflen);
bool NoteSetEnvDefault(const char *variable, char *buf);
bool NoteSetEnvDefaultNumber(const char *variable, JNUMBER defaultVal);
bool NoteSetEnvDefaultInt(const char *variable, JINTEGER defaultVal);
bool NoteIsConnected(void);
bool NoteIsConnectedST(void);
bool NoteGetNetStatus(char *statusBuf, int statusBufLen);
bool NoteGetVersion(char *versionBuf, int versionBufLen);
bool NoteGetLocation(JNUMBER *retLat, JNUMBER *retLon, JTIME *time, char *statusBuf, int statusBufLen);
bool NoteSetLocation(JNUMBER lat, JNUMBER lon);
bool NoteClearLocation(void);
bool NoteGetLocationMode(char *modeBuf, int modeBufLen);
bool NoteSetLocationMode(const char *mode, uint32_t seconds);
bool NoteGetServiceConfig(char *productBuf, int productBufLen, char *serviceBuf, int serviceBufLen, char *deviceBuf, int deviceBufLen, char *snBuf, int snBufLen);
bool NoteGetServiceConfigST(char *productBuf, int productBufLen, char *serviceBuf, int serviceBufLen, char *deviceBuf, int deviceBufLen, char *snBuf, int snBufLen);
bool NoteGetStatus(char *statusBuf, int statusBufLen, JTIME *bootTime, bool *retUSB, bool *retSignals);
bool NoteGetStatusST(char *statusBuf, int statusBufLen, JTIME *bootTime, bool *retUSB, bool *retSignals);
bool NoteSleep(char *stateb64, uint32_t seconds, const char *modes);
bool NoteWake(int stateLen, void *state);
bool NoteFactoryReset(bool deleteConfigSettings);
bool NoteSetSerialNumber(const char *sn);
bool NoteSetProductID(const char *productID);
bool NoteSetUploadMode(const char *uploadMode, int uploadMinutes, bool align);
bool NoteSetSyncMode(const char *uploadMode, int uploadMinutes, int downloadMinutes, bool align, bool sync);
#define NoteSend NoteAdd
bool NoteAdd(const char *target, J *body, bool urgent);
bool NoteSendToRoute(const char *method, const char *routeAlias, char *notefile, J *body);
bool NoteGetVoltage(JNUMBER *voltage);
bool NoteGetTemperature(JNUMBER *temp);
bool NoteGetContact(char *nameBuf, int nameBufLen, char *orgBuf, int orgBufLen, char *roleBuf, int roleBufLen, char *emailBuf, int emailBufLen);
bool NoteSetContact(char *nameBuf, char *orgBuf, char *roleBuf, char *emailBuf);

// Definitions necessary for payload descriptor
#define NP_SEGTYPE_LEN 4
#define NP_SEGLEN_LEN sizeof(uint32_t)
#define NP_SEGHDR_LEN (NP_SEGTYPE_LEN + NP_SEGLEN_LEN)
typedef struct {
    uint8_t *data;
    uint32_t alloc;
    uint32_t length;
} NotePayloadDesc;
bool NotePayloadSaveAndSleep(NotePayloadDesc *desc, uint32_t seconds, const char *modes);
bool NotePayloadRetrieveAfterSleep(NotePayloadDesc *desc);
void NotePayloadSet(NotePayloadDesc *desc, uint8_t *buf, uint32_t buflen);
void NotePayloadFree(NotePayloadDesc *desc);
bool NotePayloadAddSegment(NotePayloadDesc *desc, const char segtype[NP_SEGTYPE_LEN], void *pdata, uint32_t plen);
bool NotePayloadFindSegment(NotePayloadDesc *desc, const char segtype[NP_SEGTYPE_LEN], void *pdata, uint32_t *plen);
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
bool NoteTemplate(const char *notefileID, J *templateBody);

// End of C-callable functions
#ifdef __cplusplus
}
#endif
