/*!
 * @file NoteI2CAddress_test.cpp
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

extern uint32_t i2cAddress;

namespace
{

SCENARIO("NoteI2CAddress")
{
    GIVEN("i2cAddress is unset (0)") {
        i2cAddress = 0;

        WHEN("NoteI2CAddress is called") {
            const uint32_t result = NoteI2CAddress();

            THEN("NOTE_I2C_ADDR_DEFAULT is returned") {
                CHECK(result == NOTE_I2C_ADDR_DEFAULT);
            }
        }
    }

    GIVEN("i2cAddress is set") {
        i2cAddress = 0x79;

        WHEN("NoteI2CAddress is called") {
            const uint32_t result = NoteI2CAddress();

            THEN("the value from i2cAddress is returned") {
                CHECK(result == i2cAddress);
            }
        }
    }
}

}
