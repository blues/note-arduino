#pragma once

#include "n_lib.h"

// If we're building the tests, NOTE_C_STATIC is defined to nothing. This allows
// the tests to access the static functions in note-c. Among other things, this
// let's us mock these normally static functions.
#define NOTE_C_STATIC

#ifdef __cplusplus
extern "C" {
#endif

// Make these normally static functions externally visible if building tests.
char *crcAdd(char *json, uint16_t seqno);
bool crcError(char *json, uint16_t shouldBeSeqno);
void delayIO(void);
const char * i2cNoteQueryLength(uint32_t * available, uint32_t timeoutMs);
void setTime(JTIME seconds);
bool timerExpiredSecs(uint32_t *timer, uint32_t periodSecs);
char Jtolower(char c);

#ifdef __cplusplus
}
#endif
