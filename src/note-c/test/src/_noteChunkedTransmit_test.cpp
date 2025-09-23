/*!
 * @file _noteChunkedTransmit_test.cpp
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

typedef const char * (*nTransmitFn) (uint8_t *, uint32_t, bool);

extern volatile int hookActiveInterface;
extern nTransmitFn notecardChunkedTransmit;

namespace
{

const uint8_t * hookParameter_buffer;
size_t hookParameter_buffer_size;
bool hookParameter_delay;

const char * const hookResult = "random error msg";

SCENARIO("_noteChunkedTransmit")
{
    GIVEN("hookActiveInterface is set to NOTE_C_INTERFACE_NONE") {
        hookActiveInterface = NOTE_C_INTERFACE_NONE;

        AND_GIVEN("notecardChunkedTransmit is unset (NULL)") {
            notecardChunkedTransmit = NULL;

            WHEN("_noteChunkedTransmit is called") {
                const char * const result = _noteChunkedTransmit(NULL, 0, false);

                THEN("it returns \"a valid interface must be selected\"") {
                    CHECK(strcmp(result, "a valid interface must be selected") == 0);
                }
            }
        }

        AND_GIVEN("notecardChunkedTransmit is set") {
            notecardChunkedTransmit = [](uint8_t *buffer_, uint32_t size_, bool delay_) {
                hookParameter_buffer = buffer_;
                hookParameter_buffer_size = size_;
                hookParameter_delay = delay_;

                return hookResult;
            };

            WHEN("_noteChunkedTransmit is called") {
                const char * const result = _noteChunkedTransmit(NULL, 0, false);

                THEN("it returns \"a valid interface must be selected\"") {
                    CHECK(strcmp(result, "a valid interface must be selected") == 0);
                }
            }
        }
    }

    GIVEN("hookActiveInterface is set to NOTE_C_INTERFACE_I2C") {
        hookActiveInterface = NOTE_C_INTERFACE_I2C;

        AND_GIVEN("notecardChunkedTransmit is unset (NULL)") {
            notecardChunkedTransmit = NULL;

            WHEN("_noteChunkedTransmit is called") {
                const char * const result = _noteChunkedTransmit(NULL, 0, false);

                THEN("it returns \"a valid interface must be selected\"") {
                    CHECK(strcmp(result, "a valid interface must be selected") == 0);
                }
            }
        }

        AND_GIVEN("notecardChunkedTransmit is set") {
            notecardChunkedTransmit = [](uint8_t *buffer_, uint32_t size_, bool delay_) {
                hookParameter_buffer = buffer_;
                hookParameter_buffer_size = size_;
                hookParameter_delay = delay_;

                return hookResult;
            };

            AND_GIVEN("valid parameters") {
                uint8_t buffer[] = "test";
                const uint32_t size = sizeof(buffer);
                bool delay = true;

                WHEN("_noteChunkedTransmit is called") {
                    const char * const result = _noteChunkedTransmit(buffer, size, delay);

                    THEN("it calls notecardChunkedTransmit with the same parameters") {
                        CHECK(buffer == hookParameter_buffer);
                        CHECK(size == hookParameter_buffer_size);
                        CHECK(delay == hookParameter_delay);
                    }

                    THEN("it returns the results from notecardChunkedTransmit") {
                        CHECK(strcmp(result, hookResult) == 0);
                    }
                }
            }
        }
    }

    GIVEN("hookActiveInterface is set to NOTE_C_INTERFACE_SERIAL") {
        hookActiveInterface = NOTE_C_INTERFACE_SERIAL;

        AND_GIVEN("notecardChunkedTransmit is unset (NULL)") {
            notecardChunkedTransmit = NULL;

            WHEN("_noteChunkedTransmit is called") {
                const char * const result = _noteChunkedTransmit(NULL, 0, false);

                THEN("it returns \"a valid interface must be selected\"") {
                    CHECK(strcmp(result, "a valid interface must be selected") == 0);
                }
            }
        }

        AND_GIVEN("notecardChunkedTransmit is set") {
            notecardChunkedTransmit = [](uint8_t *buffer_, uint32_t size_, bool delay_) {
                hookParameter_buffer = buffer_;
                hookParameter_buffer_size = size_;
                hookParameter_delay = delay_;

                return hookResult;
            };

            AND_GIVEN("valid parameters") {
                uint8_t buffer[] = "test";
                const uint32_t size = sizeof(buffer);
                bool delay = true;

                WHEN("_noteChunkedTransmit is called") {
                    const char * const result = _noteChunkedTransmit(buffer, size, delay);

                    THEN("it calls notecardChunkedTransmit with the same parameters") {
                        CHECK(buffer == hookParameter_buffer);
                        CHECK(size == hookParameter_buffer_size);
                        CHECK(delay == hookParameter_delay);
                    }

                    THEN("it returns the results from notecardChunkedTransmit") {
                        CHECK(strcmp(result, hookResult) == 0);
                    }
                }
            }
        }
    }
}

}
