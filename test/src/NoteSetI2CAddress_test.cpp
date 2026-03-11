/*!
 * @file NoteSetI2CAddress_test.cpp
 *
 * Written by the Blues Inc. team.
 *
 * Copyright (c) 2024 Blues Inc. MIT License. Use of this source code is
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

SCENARIO("NoteSetI2CAddress")
{
    GIVEN("A valid I2C Address") {
        uint32_t notecardAddress = 0x09;

        WHEN("NoteSetI2CAddress is called") {
            NoteSetI2CAddress(notecardAddress);

            THEN("It should set i2cAddress to the value provided") {
                CHECK(i2cAddress == notecardAddress);
            }
        }
    }

    GIVEN("An I2C Address of zero") {
        uint32_t notecardAddress = 0;

        WHEN("NoteSetI2CAddress is called") {
            NoteSetI2CAddress(notecardAddress);

            THEN("It should set i2cAddress to zero") {
                CHECK(i2cAddress == notecardAddress);
            }
        }
    }
}

}
