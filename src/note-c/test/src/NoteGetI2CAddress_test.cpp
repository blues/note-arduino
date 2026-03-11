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

#include "n_lib.h"

extern uint32_t i2cAddress;

namespace
{

SCENARIO("NoteGetI2CAddress")
{
    GIVEN("i2cAddress is unset (0x00)") {
        i2cAddress = 0x00;

        AND_GIVEN("A valid pointer for the I2C address is provided") {
            uint32_t i2cAddr = 0x01;

            WHEN("NoteGetI2CAddress is called") {
                NoteGetI2CAddress(&i2cAddr);

                THEN("It should return zero") {
                    CHECK(i2cAddr == i2cAddress);
                }
            }
        }
    }

    GIVEN("A valid I2C address has been set") {
        i2cAddress = 0x79;

        AND_GIVEN("A valid pointer for the I2C address is provided") {
            uint32_t i2cAddr = 0x00;

            WHEN("NoteGetI2CAddress is called") {
                NoteGetI2CAddress(&i2cAddr);

                THEN("It should return the address") {
                    CHECK(i2cAddr == i2cAddress);
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
