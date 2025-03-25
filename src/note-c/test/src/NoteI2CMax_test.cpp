/*!
 * @file NoteI2CMax_test.cpp
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

extern uint32_t i2cMax;

namespace
{

SCENARIO("NoteI2CMax")
{
    GIVEN("i2cMax is unset (0)") {
        i2cMax = 0;

        WHEN("NoteI2CMax is called") {
            const uint32_t result = NoteI2CMax();

            THEN("NOTE_I2C_MAX_DEFAULT is returned") {
                CHECK(result == NOTE_I2C_MAX_DEFAULT);
            }
        }
    }

    GIVEN("i2cMax is greater than NOTE_I2C_MAX_MAX") {
        i2cMax = NOTE_I2C_MAX_MAX + 1;

        WHEN("NoteI2CMax is called") {
            const uint32_t result = NoteI2CMax();

            THEN("NOTE_I2C_MAX_MAX is returned") {
                CHECK(result == NOTE_I2C_MAX_MAX);
            }
        }
    }

    GIVEN("i2cMax is set") {
        i2cMax = 17;

        WHEN("NoteI2CMax is called") {
            const uint32_t result = NoteI2CMax();

            THEN("the value from i2cMax is returned") {
                CHECK(result == i2cMax);
            }
        }
    }
}

}
