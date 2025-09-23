/*!
 * @file _noteI2CReset_test.cpp
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

extern volatile int hookActiveInterface;
extern i2cResetFn hookI2CReset;

namespace
{

uint16_t hookParameter_address;

const bool hookResult = false;

SCENARIO("_noteI2CReset")
{
    GIVEN("hookActiveInterface is not set to NOTE_C_INTERFACE_I2C") {
        hookActiveInterface = NOTE_C_INTERFACE_NONE;

        AND_GIVEN("hookI2CReset is NULL") {
            hookI2CReset = NULL;

            WHEN("_noteI2CReset is called") {
                const bool result = _noteI2CReset(0);

                THEN("_noteI2CReset returns true") {
                    CHECK(result);
                }
            }
        }

        AND_GIVEN("hookI2CReset is not NULL") {
            hookI2CReset = [](uint16_t address_) {
                hookParameter_address = address_;
                return hookResult;
            };

            AND_GIVEN("valid parameters") {
                const uint16_t address = NOTE_I2C_ADDR_DEFAULT;

                WHEN("_noteI2CReset is called") {
                    const bool result = _noteI2CReset(address);

                    THEN("_noteI2CReset returns true") {
                        CHECK(result);
                    }
                }
            }
        }
    }

    GIVEN("hookActiveInterface is set to NOTE_C_INTERFACE_I2C") {
        hookActiveInterface = NOTE_C_INTERFACE_I2C;

        AND_GIVEN("hookI2CReset is NULL") {
            hookI2CReset = NULL;

            WHEN("_noteI2CReset is called") {
                const bool result = _noteI2CReset(0);

                THEN("_noteI2CReset returns true") {
                    CHECK(result);
                }
            }
        }

        AND_GIVEN("hookI2CReset is not NULL") {
            hookI2CReset = [](uint16_t address_) {
                hookParameter_address = address_;
                return hookResult;
            };

            AND_GIVEN("valid parameters") {
                const uint16_t address = NOTE_I2C_ADDR_DEFAULT;

                WHEN("_noteI2CReset is called") {
                    const bool result = _noteI2CReset(address);

                    THEN("_noteI2CReset calls hookI2CReset with the same parameters") {
                        CHECK(address == hookParameter_address);
                    }

                    THEN("_noteI2CReset returns the result of hookI2CReset") {
                        CHECK(result == hookResult);
                    }
                }
            }
        }
    }
}

}
