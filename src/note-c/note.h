// Copyright 2018 Inca Roads LLC.  All rights reserved. 
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#pragma once

// In case they're not yet defined
#include <stdbool.h>
#include <stdint.h>

// Define our basic floating data type.  In most cases "double" is the right answer, however for
// very small microcontrollers these libraries are simply too large.
#ifdef NOTE_FLOAT
#define JNUMBER float
#else
#define JNUMBER double
#endif

// UNIX Epoch time (also known as POSIX time) is the  number of seconds that have elapsed since
// 00:00:00 Thursday, 1 January 1970, Coordinated Universal Time (UTC).  In this project, it always
// originates from the Notecard, which synchronizes the time from both the cell network and GPS.
typedef unsigned long int JTIME;

// C-callable functions
#ifdef __cplusplus
extern "C" {
#endif

// cJSON wrappers
#include "n_cjson.h"

// Card callback functions
typedef void (*mutexFn) (void);
typedef void * (*mallocFn) (size_t size);
typedef void (*freeFn) (void *);
typedef void (*delayMsFn) (uint32_t ms);
typedef long unsigned int (*getMsFn) (void);
typedef size_t (*debugOutputFn) (const char *text);
typedef void (*serialResetFn) (void);
typedef void (*serialTransmitFn) (uint8_t *data, size_t len, bool flush);
typedef bool (*serialAvailableFn) (void);
typedef char (*serialReceiveFn) (void);
typedef void (*i2cResetFn) (void);
typedef const char * (*i2cTransmitFn) (uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size);
typedef const char * (*i2cReceiveFn) (uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size, uint32_t *avail);

// External API
bool NoteReset(void);
void NoteResetRequired(void);
#define NoteNewBody JCreateObject
J *NoteNewRequest(const char *request);
J *NoteRequestResponse(J *req);
bool NoteRequest(J *req);
#define NoteResponseError(rsp) (!JIsNullString(rsp, "err"))
#define NoteResponseErrorContains(rsp, errstr) (JContainsString(rsp, "err", errstr))
#define NoteDeleteResponse(rsp) JDelete(rsp)
J *NoteTransaction(J *req);
bool NoteErrorContains(const char *errstr, const char *errtype);
void NoteErrorClean(char *errbuf);
void NoteSetFnDebugOutput(debugOutputFn fn);
void NoteSetFnMutex(mutexFn lockI2Cfn, mutexFn unlockI2Cfn, mutexFn lockNotefn, mutexFn unlockNotefn);
void NoteSetFnDefault(mallocFn mallocfn, freeFn freefn, delayMsFn delayfn, getMsFn millisfn);
void NoteSetFn(mallocFn mallocfn, freeFn freefn, delayMsFn delayfn, getMsFn millisfn);
void NoteSetFnSerial(serialResetFn resetfn, serialTransmitFn writefn, serialAvailableFn availfn, serialReceiveFn readfn);
#define NOTE_I2C_ADDR_DEFAULT	0
#define NOTE_I2C_MAX_DEFAULT	0
void NoteSetFnI2C(uint32_t i2caddr, uint32_t i2cmax, i2cResetFn resetfn, i2cTransmitFn transmitfn, i2cReceiveFn receivefn);

// Calls to the functions set above
#define NotePrintf NoteFnDebug
void NoteFnDebugMsg(const char *message);
void NoteFnDebug(const char *format, ...);
void *NoteFnMalloc(size_t size);
void NoteFnFree(void *);
long unsigned int NoteFnGetMs(void);
void NoteFnDelayMs(uint32_t ms);
void NoteFnLockI2C(void);
void NoteFnUnlockI2C(void);
uint32_t NoteFnI2CAddress(void);
uint32_t NoteFnI2CMax(void);

// String helpers to help encourage the world to abandon the horribly-error-prone strn*
size_t strlcpy(char *dst, const char *src, size_t siz);
size_t strlcat(char *dst, const char *src, size_t siz);

// JSON helpers
void JInit(void);
void JCheck(void);
void *JMalloc(size_t size);
void JFree(void *p);
bool JIsPresent(J *rsp, const char *field);
char *JGetString(J *rsp, const char *field);
JNUMBER JGetNumber(J *rsp, const char *field);
J *JGetObject(J *rsp, const char *field);
int JGetInt(J *rsp, const char *field);
bool JGetBool(J *rsp, const char *field);
bool JIsNullString(J *rsp, const char *field);
bool JIsExactString(J *rsp, const char *field, const char *teststr);
bool JContainsString(J *rsp, const char *field, const char *substr);
bool JAddBinaryToObject(J *req, const char *fieldName, const void *binaryData, uint32_t binaryDataLen);

// Helper functions for apps that wish to limit their C library dependencies
#define JNRound(x,NUMDIGITS) ((round((x) * 1E##NUMDIGITS)) / 1E##NUMDIGITS)
#define JNTOA_PRECISION (10)
#define JNTOA_MAX       ((2*JNTOA_PRECISION)+1+1)
char * JNtoA(JNUMBER f, char * buf, int precision);
JNUMBER JAtoN(const char *string, char **endPtr);
int JB64EncodeLen(int len);
int JB64Encode(char * coded_dst, const char *plain_src,int len_plain_src);
int JB64DecodeLen(const char * coded_src);
int JB64Decode(char * plain_dst, const char *coded_src);

// High-level helper functions that are both useful and serve to show developers how to call the API
bool NoteTimeValid(void);
bool NoteTimeValidST(void);
JTIME NoteTime(void);
JTIME NoteTimeST(void);
bool NoteRegion(char **retCountry, char **retArea, char **retZone, int *retZoneOffset);
bool NoteLocationValid(char *errbuf, uint32_t errbuflen);
bool NoteLocationValidST(char *errbuf, uint32_t errbuflen);
int NoteGetEnvInt(const char *variable, int defaultVal);
void NoteGetEnv(const char *variable, const char *defaultVal, char *buf, uint32_t buflen);
bool NoteGetEnvAll(char *statusBuf, int statusBufLen);
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
bool NoteTemplate(const char *target, J *body);
bool NoteSend(const char *target, J *body, bool urgent);
bool NoteSendToRoute(const char *method, const char *routeAlias, char *notefile, J *body);
bool NoteGetVoltage(JNUMBER *voltage);
bool NoteGetTemperature(JNUMBER *temp);
bool NoteGetContact(char *nameBuf, int nameBufLen, char *orgBuf, int orgBufLen, char *roleBuf, int roleBufLen, char *emailBuf, int emailBufLen);
bool NoteSetContact(char *nameBuf, char *orgBuf, char *roleBuf, char *emailBuf);

// End of C-callable functions
#ifdef __cplusplus
}
#endif
