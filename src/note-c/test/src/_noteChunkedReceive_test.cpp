/*!
 * @file _noteChunkedReceive_test.cpp
 *
 * Written by the Blues Inc. team.
 *
 * Copyright (c) 2023 Blues Inc. MIT License. Use of this source code is
 * governed by licenses granted by the copyright holder including that found in
 * the
 * <a href="https://github.com/blues/note-c/blob/master/LICENSE">LICENSE</a>
 * file.
 *
 */

#include <catch2/catch_test_macros.hpp>

#include "n_lib.h"

typedef const char * (*nReceiveFn) (uint8_t *, uint32_t *, bool, uint32_t, uint32_t *);

extern volatile int hookActiveInterface;
extern nReceiveFn notecardChunkedReceive;

namespace
{

const uint8_t * hookParameter_buffer;
uint32_t * hookParameter_buffer_size;
bool hookParameter_delay;
uint32_t hookParameter_timeoutMs;
uint32_t * hookParameter_available;

const char * const hookResult = "random error msg";
const uint32_t hookResult_size = 9;
const uint32_t hookResult_available = 17;

SCENARIO("_noteChunkedReceive")
{
    GIVEN("hookActiveInterface is set to NOTE_C_INTERFACE_NONE") {
        hookActiveInterface = NOTE_C_INTERFACE_NONE;

        AND_GIVEN("notecardChunkedReceive is unset (NULL)") {
            notecardChunkedReceive = NULL;

            WHEN("_noteChunkedReceive is called") {
                const char * const result = _noteChunkedReceive(NULL, NULL, false, 0, NULL);

                THEN("it returns \"a valid interface must be selected\"") {
                    CHECK(strcmp(result, "a valid interface must be selected") == 0);
                }
            }
        }

        AND_GIVEN("notecardChunkedReceive is set") {
            notecardChunkedReceive = [](uint8_t *buffer_, uint32_t *size_, bool delay_, uint32_t timeoutMs_, uint32_t *available_) {
                hookParameter_buffer = buffer_;
                hookParameter_buffer_size = size_;
                hookParameter_delay = delay_;
                hookParameter_timeoutMs = timeoutMs_;
                hookParameter_available = available_;

                *size_ = hookResult_size;
                *available_ = hookResult_available;
                return hookResult;
            };

            WHEN("_noteChunkedReceive is called") {
                const char * const result = _noteChunkedReceive(NULL, NULL, false, 0, NULL);

                THEN("it returns \"a valid interface must be selected\"") {
                    CHECK(strcmp(result, "a valid interface must be selected") == 0);
                }
            }
        }
    }

    GIVEN("hookActiveInterface is set to NOTE_C_INTERFACE_I2C") {
        hookActiveInterface = NOTE_C_INTERFACE_I2C;

        AND_GIVEN("notecardChunkedReceive is unset (NULL)") {
            notecardChunkedReceive = NULL;

            WHEN("_noteChunkedReceive is called") {
                const char * const result = _noteChunkedReceive(NULL, NULL, false, 0, NULL);

                THEN("it returns \"a valid interface must be selected\"") {
                    CHECK(strcmp(result, "a valid interface must be selected") == 0);
                }
            }
        }

        AND_GIVEN("notecardChunkedReceive is set") {
            notecardChunkedReceive = [](uint8_t *buffer_, uint32_t *size_, bool delay_, uint32_t timeoutMs_, uint32_t *available_) {
                hookParameter_buffer = buffer_;
                hookParameter_buffer_size = size_;
                hookParameter_delay = delay_;
                hookParameter_timeoutMs = timeoutMs_;
                hookParameter_available = available_;

                *size_ = hookResult_size;
                *available_ = hookResult_available;
                return hookResult;
            };

            AND_GIVEN("valid parameters") {
                uint8_t buffer[] = "test";
                uint32_t size = sizeof(buffer);
                bool delay = true;
                const uint32_t timeoutMs = 17;
                uint32_t available = 0;

                WHEN("_noteChunkedReceive is called") {
                    const char * const result = _noteChunkedReceive(buffer, &size, delay, timeoutMs, &available);

                    THEN("it calls notecardChunkedReceive with the same parameters") {
                        CHECK(buffer == hookParameter_buffer);
                        CHECK(&size == hookParameter_buffer_size);
                        CHECK(delay == hookParameter_delay);
                        CHECK(timeoutMs == hookParameter_timeoutMs);
                        CHECK(&available == hookParameter_available);
                    }

                    THEN("it returns the results from notecardChunkedReceive") {
                        CHECK(strcmp(result, hookResult) == 0);
                        CHECK(size == hookResult_size);
                        CHECK(available == hookResult_available);
                    }
                }
            }
        }
    }

    GIVEN("hookActiveInterface is set to NOTE_C_INTERFACE_SERIAL") {
        hookActiveInterface = NOTE_C_INTERFACE_SERIAL;

        AND_GIVEN("notecardChunkedReceive is unset (NULL)") {
            notecardChunkedReceive = NULL;

            WHEN("_noteChunkedReceive is called") {
                const char * const result = _noteChunkedReceive(NULL, NULL, false, 0, NULL);

                THEN("it returns \"a valid interface must be selected\"") {
                    CHECK(strcmp(result, "a valid interface must be selected") == 0);
                }
            }
        }

        AND_GIVEN("notecardChunkedReceive is set") {
            notecardChunkedReceive = [](uint8_t *buffer_, uint32_t *size_, bool delay_, uint32_t timeoutMs_, uint32_t *available_) {
                hookParameter_buffer = buffer_;
                hookParameter_buffer_size = size_;
                hookParameter_delay = delay_;
                hookParameter_timeoutMs = timeoutMs_;
                hookParameter_available = available_;

                *size_ = hookResult_size;
                *available_ = hookResult_available;
                return hookResult;
            };

            AND_GIVEN("valid parameters") {
                uint8_t buffer[] = "test";
                uint32_t size = sizeof(buffer);
                bool delay = true;
                const uint32_t timeoutMs = 17;
                uint32_t available = 0;

                WHEN("_noteChunkedReceive is called") {
                    const char * const result = _noteChunkedReceive(buffer, &size, delay, timeoutMs, &available);

                    THEN("it calls notecardChunkedReceive with the same parameters") {
                        CHECK(buffer == hookParameter_buffer);
                        CHECK(&size == hookParameter_buffer_size);
                        CHECK(delay == hookParameter_delay);
                        CHECK(timeoutMs == hookParameter_timeoutMs);
                        CHECK(&available == hookParameter_available);
                    }

                    THEN("it returns the results from notecardChunkedReceive") {
                        CHECK(strcmp(result, hookResult) == 0);
                        CHECK(size == hookResult_size);
                        CHECK(available == hookResult_available);
                    }
                }
            }
        }
    }
}

}
