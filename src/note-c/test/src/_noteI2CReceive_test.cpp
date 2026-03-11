/*!
 * @file _noteI2CReceive_test.cpp
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
extern i2cReceiveFn hookI2CReceive;

namespace
{

uint16_t hookParameter_address;
uint8_t * hookParameter_buffer;
uint16_t hookParameter_buffer_size;
uint32_t * hookParameter_available;

const char * const hookResult = "test";
const uint32_t hookResult_Available = 1234;

SCENARIO("_noteI2CReceive")
{
    GIVEN("hookActiveInterface is not set to NOTE_C_INTERFACE_I2C") {
        hookActiveInterface = NOTE_C_INTERFACE_NONE;

        AND_GIVEN("hookI2CReceive is NULL") {
            hookI2CReceive = NULL;

            WHEN("_noteI2CReceive is called") {
                const char * const result = _noteI2CReceive(0, NULL, 0, NULL);

                THEN("_noteI2CReceive returns \"i2c not active\"") {
                    CHECK(strcmp(result, "i2c not active") == 0);
                }
            }
        }

        AND_GIVEN("hookI2CReceive is not NULL") {
            hookI2CReceive = [](uint16_t address_, uint8_t *buffer_, uint16_t buffer_size_, uint32_t *available_) {
                hookParameter_address = address_;
                hookParameter_buffer = buffer_;
                hookParameter_buffer_size = buffer_size_;
                hookParameter_available = available_;
                return hookResult;
            };

            AND_GIVEN("valid parameters") {
                const uint16_t address = 0;
                uint8_t buffer[] = "test";
                const uint16_t buffer_size = sizeof(buffer);
                uint32_t available = 0;

                WHEN("_noteI2CReceive is called") {
                    const char * const result = _noteI2CReceive(address, buffer, buffer_size, &available);

                    THEN("_noteI2CReceive returns \"i2c not active\"") {
                        CHECK(strcmp(result, "i2c not active") == 0);
                    }
                }
            }
        }
    }

    GIVEN("hookActiveInterface is set to NOTE_C_INTERFACE_I2C") {
        hookActiveInterface = NOTE_C_INTERFACE_I2C;

        AND_GIVEN("hookI2CReceive is NULL") {
            hookI2CReceive = NULL;

            WHEN("_noteI2CReceive is called") {
                const char * const result = _noteI2CReceive(0, NULL, 0, NULL);

                THEN("_noteI2CReceive returns \"i2c not active\"") {
                    CHECK(strcmp(result, "i2c not active") == 0);
                }
            }
        }

        AND_GIVEN("hookI2CReceive is not NULL") {
            hookI2CReceive = [](uint16_t address_, uint8_t *buffer_, uint16_t buffer_size_, uint32_t *available_) {
                hookParameter_address = address_;
                hookParameter_buffer = buffer_;
                hookParameter_buffer_size = buffer_size_;
                hookParameter_available = available_;
                *available_ = hookResult_Available;
                return hookResult;
            };

            AND_GIVEN("valid parameters") {
                const uint16_t address = NOTE_I2C_ADDR_DEFAULT;
                uint8_t buffer[] = "test";
                const uint16_t buffer_size = sizeof(buffer);
                uint32_t available = 0;

                WHEN("_noteI2CReceive is called") {
                    const char * const result = _noteI2CReceive(address, buffer, buffer_size, &available);

                    THEN("_noteI2CReceive calls hookI2CReceive with the same parameters") {
                        CHECK(address == hookParameter_address);
                        CHECK(buffer == hookParameter_buffer);
                        CHECK(buffer_size == hookParameter_buffer_size);
                        CHECK(&available == hookParameter_available);
                    }

                    THEN("_noteI2CReceive returns the values from hookI2CReceive") {
                        CHECK(strcmp(result, hookResult) == 0);
                        CHECK(available == hookResult_Available);
                    }
                }
            }
        }
    }
}

}
