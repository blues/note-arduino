/*!
 * @file _noteI2CTransmit_test.cpp
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
extern i2cTransmitFn hookI2CTransmit;

namespace
{

uint16_t hookParameter_address;
uint8_t * hookParameter_buffer;
uint16_t hookParameter_buffer_size;

const char * const hookResult = "test";

SCENARIO("_noteI2CTransmit")
{
    GIVEN("hookActiveInterface is not set to NOTE_C_INTERFACE_I2C")
    {
        hookActiveInterface = NOTE_C_INTERFACE_NONE;

        AND_GIVEN("hookI2CTransmit is NULL")
        {
            hookI2CTransmit = NULL;

            WHEN("_noteI2CTransmit is called")
            {
                const char * const result = _noteI2CTransmit(0, NULL, 0);

                THEN("_noteI2CTransmit returns \"i2c not active\"")
                {
                    CHECK(strcmp(result, "i2c not active") == 0);
                }
            }
        }

        AND_GIVEN("hookI2CTransmit is not NULL")
        {
            hookI2CTransmit = [](uint16_t address_, uint8_t *buffer_, uint16_t buffer_size_) {
                hookParameter_address = address_;
                hookParameter_buffer = buffer_;
                hookParameter_buffer_size = buffer_size_;
                return hookResult;
            };

            AND_GIVEN("valid parameters") {
                const uint16_t address = 0;
                uint8_t buffer[] = "test";
                const uint16_t buffer_size = sizeof(buffer);

                WHEN("_noteI2CTransmit is called")
                {
                    const char * result = _noteI2CTransmit(address, buffer, buffer_size);

                    THEN("_noteI2CTransmit returns \"i2c not active\"")
                    {
                        CHECK(strcmp(result, "i2c not active") == 0);
                    }
                }
            }
        }
    }

    GIVEN("hookActiveInterface is set to NOTE_C_INTERFACE_I2C")
    {
        hookActiveInterface = NOTE_C_INTERFACE_I2C;

        AND_GIVEN("hookI2CTransmit is NULL")
        {
            hookI2CTransmit = NULL;

            WHEN("_noteI2CTransmit is called")
            {
                const char * const result = _noteI2CTransmit(0, NULL, 0);

                THEN("_noteI2CTransmit returns \"i2c not active\"")
                {
                    CHECK(strcmp(result, "i2c not active") == 0);
                }
            }
        }

        AND_GIVEN("hookI2CTransmit is not NULL")
        {
            hookI2CTransmit = [](uint16_t address_, uint8_t *buffer_, uint16_t buffer_size_) {
                hookParameter_address = address_;
                hookParameter_buffer = buffer_;
                hookParameter_buffer_size = buffer_size_;
                return hookResult;
            };

            AND_GIVEN("valid parameters") {
                const uint16_t address = NOTE_I2C_ADDR_DEFAULT;
                uint8_t buffer[] = "test";
                const uint16_t buffer_size = sizeof(buffer);

                WHEN("_noteI2CTransmit is called")
                {
                    const char * const result = _noteI2CTransmit(address, buffer, buffer_size);

                    THEN("_noteI2CTransmit calls hookI2CTransmit with the same parameters")
                    {
                        CHECK(address == hookParameter_address);
                        CHECK(buffer == hookParameter_buffer);
                        CHECK(buffer_size == hookParameter_buffer_size);
                        CHECK(strcmp(result, hookResult) == 0);
                    }
                }
            }
        }
    }
}

}
