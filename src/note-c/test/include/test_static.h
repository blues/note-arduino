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
char *_crcAdd(char *json, uint16_t seqno);
bool _crcError(char *json, uint16_t shouldBeSeqno);
void _delayIO(void);
const char * _i2cNoteQueryLength(uint32_t * available, uint32_t timeoutMs);
void _noteSetActiveInterface(int interface);
void _setTime(JTIME seconds);
bool _timerExpiredSecs(uint32_t *timer, uint32_t periodSecs);
char _j_tolower(char c);

#ifdef __cplusplus
}
#endif
