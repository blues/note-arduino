/*!
 * @file NoteGetI2CMtu_test.cpp
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

extern uint32_t i2cMax;

namespace
{

SCENARIO("NoteGetI2CMtu")
{
    GIVEN("i2cMax is unset (0)") {
        i2cMax = 0;

        AND_GIVEN("A valid pointer for the I2C MTU is provided") {
            uint32_t i2cMtu = 1;

            WHEN("NoteGetI2CMtu is called") {
                NoteGetI2CMtu(&i2cMtu);

                THEN("It should return zero") {
                    CHECK(i2cMax == i2cMtu);
                }
            }
        }
    }

    GIVEN("i2cMax is greater than NOTE_I2C_MTU_MAX") {
        i2cMax = (NOTE_I2C_MTU_MAX + 1);

        AND_GIVEN("A valid pointer for the I2C MTU is provided") {
            uint32_t i2cMtu = 0;

            WHEN("NoteGetI2CMtu is called") {
                NoteGetI2CMtu(&i2cMtu);

                THEN("It should return the i2cMax value") {
                    CHECK(i2cMax == i2cMtu);
                }
            }
        }
    }

    GIVEN("A valid I2C MTU has been set") {
        i2cMax = 17;

        AND_GIVEN("A valid pointer for the I2C MTU is provided") {
            uint32_t i2cMtu = 0;

            WHEN("NoteGetI2CMtu is called") {
                NoteGetI2CMtu(&i2cMtu);

                THEN("It should return the MTU") {
                    CHECK(i2cMtu == i2cMax);
                }
            }
        }

        AND_GIVEN("A NULL pointer for the I2C MTU is provided") {

            WHEN("NoteGetI2CMtu is called") {
                NoteGetI2CMtu(NULL);

                THEN("It should not crash") {
                    SUCCEED();
                }
            }
        }
    }
}

}
