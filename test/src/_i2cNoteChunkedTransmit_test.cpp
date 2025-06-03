/*!
 * @file _i2cNoteChunkedTransmit_test.cpp
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
FAKE_VALUE_FUNC(const char *, _i2cChunkedTransmit, uint8_t *, uint32_t, bool)
FAKE_VOID_FUNC(NoteLockI2C)
FAKE_VOID_FUNC(NoteUnlockI2C)

namespace
{

const char * const result_val = "zak";

SCENARIO("_i2cNoteChunkedTransmit")
{
    GIVEN("Valid I/O and mocked _i2cChunkedTransmit") {
        const uint8_t * const buffer = (uint8_t *)"zjf";
        uint32_t size = 79;
        const bool delay = true;

        _i2cChunkedTransmit_fake.custom_fake = [](uint8_t *, uint32_t, bool) -> const char* {
            return result_val;
        };

        WHEN("_i2cNoteChunkedTransmit is called") {
            const char *err = _i2cNoteChunkedTransmit((uint8_t *)buffer, size, delay);

            THEN("_i2cChunkedTransmit is called with the same parameters") {
                CHECK(_i2cChunkedTransmit_fake.arg0_val == buffer);
                CHECK(_i2cChunkedTransmit_fake.arg1_val == size);
                CHECK(_i2cChunkedTransmit_fake.arg2_val == delay);
            }

            THEN("The results of _i2cChunkedTransmit is returned unmodified") {
                CHECK(strcmp(err, result_val) == 0);
            }

            THEN("The Notecard is locked and unlocked") {
                CHECK(1 == NoteLockI2C_fake.call_count);
                CHECK(NoteUnlockI2C_fake.call_count == NoteLockI2C_fake.call_count);
            }
        }
    }

    RESET_FAKE(_i2cChunkedTransmit);
    RESET_FAKE(NoteLockI2C);
    RESET_FAKE(NoteUnlockI2C);
}

}


