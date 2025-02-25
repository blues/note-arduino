/*!
 * @file NoteGetFnI2C_test.cpp
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
uint32_t maxTransmitSize = 0xF0;

bool mockI2CReset(uint16_t address)
{
    (void)address;
    return false;
}

const char * mockI2CTransmit(uint16_t address, uint8_t* txBuf, uint16_t txBufSize)
{
    (void)address;
    (void)txBuf;
    (void)txBufSize;
    return NULL;
}

const char * mockI2CReceive(uint16_t address, uint8_t* rxBuf, uint16_t rxBufSize, uint32_t *available)
{
    (void)address;
    (void)rxBuf;
    (void)rxBufSize;
    (void)available;
    return NULL;
}

SCENARIO("NoteGetFnI2C")
{
    GIVEN("A set of I2C parameters") {
        NoteSetFnI2C(notecardAddr, maxTransmitSize, mockI2CReset, mockI2CTransmit, mockI2CReceive);

        AND_GIVEN("Valid pointers for all I2C functions are provided") {
            uint32_t notecardAddr = 0;
            uint32_t maxTransmitSize = 0;
            i2cResetFn resetFunc = NULL;
            i2cTransmitFn transmitFunc = NULL;
            i2cReceiveFn receiveFunc = NULL;

            WHEN("NoteGetFnI2C is called") {
                THEN("It should return the mock functions") {
                    NoteGetFnI2C(&notecardAddr, &maxTransmitSize, &resetFunc, &transmitFunc, &receiveFunc);
                    CHECK(notecardAddr == 0x09);
                    CHECK(maxTransmitSize == 0xF0);
                    CHECK(resetFunc == mockI2CReset);
                    CHECK(transmitFunc == mockI2CTransmit);
                    CHECK(receiveFunc == mockI2CReceive);
                }
            }
        }

        AND_GIVEN("Only NULL pointers are provided") {
            WHEN("NoteGetFnI2C is called") {
                NoteGetFnI2C(NULL, NULL, NULL, NULL, NULL);

                THEN("It should not crash") {
                    SUCCEED();
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the I2C address is provided") {
            uint32_t notecardAddr = 0;

            WHEN("NoteGetFnI2C is called") {
                NoteGetFnI2C(&notecardAddr, NULL, NULL, NULL, NULL);

                THEN("It should return the mock address") {
                    CHECK(notecardAddr == 0x09);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the I2C max transmit size is provided") {
            uint32_t maxTransmitSize = 0;

            WHEN("NoteGetFnI2C is called") {
                NoteGetFnI2C(NULL, &maxTransmitSize, NULL, NULL, NULL);

                THEN("It should return the mock max transmit size") {
                    CHECK(maxTransmitSize == 0xF0);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the I2C reset function is provided") {
            i2cResetFn resetFunc = NULL;

            WHEN("NoteGetFnI2C is called") {
                NoteGetFnI2C(NULL, NULL, &resetFunc, NULL, NULL);

                THEN("It should return the mock reset function") {
                    CHECK(resetFunc == mockI2CReset);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the I2C transmit function is provided") {
            i2cTransmitFn transmitFunc = NULL;

            WHEN("NoteGetFnI2C is called") {
                NoteGetFnI2C(NULL, NULL, NULL, &transmitFunc, NULL);

                THEN("It should return the mock transmit function") {
                    CHECK(transmitFunc == mockI2CTransmit);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the I2C receive function is provided") {
            i2cReceiveFn receiveFunc = NULL;

            WHEN("NoteGetFnI2C is called") {
                NoteGetFnI2C(NULL, NULL, NULL, NULL, &receiveFunc);

                THEN("It should return the mock receive function") {
                    CHECK(receiveFunc == mockI2CReceive);
                }
            }
        }
    }
}

}
