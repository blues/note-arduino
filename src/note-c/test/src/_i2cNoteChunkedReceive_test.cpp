/*!
 * @file _i2cNoteChunkedReceive_test.cpp
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
#include <fff.h>

#include "n_lib.h"

DEFINE_FFF_GLOBALS
FAKE_VALUE_FUNC(const char *, _i2cChunkedReceive, uint8_t *, uint32_t *, bool, uint32_t, uint32_t *)
FAKE_VOID_FUNC(NoteLockI2C)
FAKE_VOID_FUNC(NoteUnlockI2C)

namespace
{

const char * const result_val = "zak";
const uint32_t result_size = 19;
const uint32_t result_available = 79;

SCENARIO("_i2cNoteChunkedReceive")
{
    GIVEN("Valid I/O and mocked _i2cChunkedReceive") {
        const uint8_t * const buffer = (uint8_t *)"zjf";
        uint32_t size = 0;
        const bool delay = true;
        const uint32_t timeoutMs = 0;
        uint32_t available = 0;

        _i2cChunkedReceive_fake.custom_fake = [](uint8_t *, uint32_t *size, bool, uint32_t, uint32_t *available) -> const char* {
            *size = result_size;
            *available = result_available;
            return result_val;
        };

        WHEN("_i2cNoteChunkedReceive is called") {
            const char *err = _i2cNoteChunkedReceive((uint8_t *)buffer, &size, delay, timeoutMs, &available);

            THEN("_i2cChunkedReceive is called with the same parameters") {
                CHECK(_i2cChunkedReceive_fake.arg0_val == buffer);
                CHECK(_i2cChunkedReceive_fake.arg1_val == &size);
                CHECK(_i2cChunkedReceive_fake.arg2_val == delay);
                CHECK(_i2cChunkedReceive_fake.arg3_val == timeoutMs);
                CHECK(_i2cChunkedReceive_fake.arg4_val == &available);
            }

            THEN("The results of _i2cChunkReceive is returned unmodified") {
                CHECK(strcmp(err, result_val) == 0);
                CHECK(size == result_size);
                CHECK(available == result_available);
            }

            THEN("The Notecard is locked and unlocked") {
                CHECK(1 == NoteLockI2C_fake.call_count);
                CHECK(NoteUnlockI2C_fake.call_count == NoteLockI2C_fake.call_count);
            }
        }
    }

    RESET_FAKE(_i2cChunkedReceive);
    RESET_FAKE(NoteLockI2C);
    RESET_FAKE(NoteUnlockI2C);
}

}


