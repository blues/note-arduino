/*!
 * @file NoteGetI2CAddress_test.cpp
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
#include "fff.h"

#include "n_lib.h"

DEFINE_FFF_GLOBALS

namespace
{

uint32_t notecardAddr = 0x09;

SCENARIO("NoteGetI2CAddress")
{
    GIVEN("An I2C address") {
        NoteSetI2CAddress(notecardAddr);

        AND_GIVEN("A valid pointer for the I2C address is provided") {
            uint32_t i2cAddress = 0;
            WHEN("NoteGetI2CAddress is called") {
                NoteGetI2CAddress(&i2cAddress);

                THEN("It should return the mock address") {
                    CHECK(i2cAddress == notecardAddr);
                }
            }
        }

        AND_GIVEN("A NULL pointer for the I2C address is provided") {
            WHEN("NoteGetI2CAddress is called") {
                NoteGetI2CAddress(NULL);

                THEN("It should not crash") {
                    SUCCEED();
                }
            }
        }
    }
}

}
