/*!
 * @file NoteSetI2CMtu_test.cpp
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

SCENARIO("NoteSetI2CMtu")
{
    GIVEN("A valid I2C MTU") {
        uint32_t notecardMtu = 17;

        WHEN("NoteSetI2CMtu is called") {
            NoteSetI2CMtu(notecardMtu);

            THEN("It should set i2cMax to the value provided") {
                CHECK(i2cMax == notecardMtu);
            }
        }
    }

    GIVEN("An I2C MTU of zero") {
        uint32_t notecardMtu = 0;

        WHEN("NoteSetI2CMtu is called") {
            NoteSetI2CMtu(notecardMtu);

            THEN("It should set i2cMax to zero") {
                CHECK(i2cMax == notecardMtu);
            }
        }
    }

    GIVEN("An I2C MTU larger than the maximum") {
        uint32_t tooLargeMtu = (NOTE_I2C_MTU_MAX + 1);

        WHEN("NoteSetI2CMtu is called") {
            NoteSetI2CMtu(tooLargeMtu);

            THEN("It should set i2cMax to the value provided") {
                CHECK(i2cMax == tooLargeMtu);
            }
        }
    }
}

}
