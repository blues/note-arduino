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
#include <fff.h>

#include "n_lib.h"

DEFINE_FFF_GLOBALS
FAKE_VOID_FUNC(_noteLockNote)
FAKE_VOID_FUNC(_noteUnlockNote)

extern uint32_t i2cAddress;
extern uint32_t i2cMax;
extern i2cResetFn hookI2CReset;
extern i2cTransmitFn hookI2CTransmit;
extern i2cReceiveFn hookI2CReceive;

namespace
{

const uint32_t MyI2cAddress = 0x19790917;
const uint32_t MyI2cMax = 79;

bool MyI2cReset(uint16_t)
{
    return false;
}

const char * MyI2cTransmit(uint16_t, uint8_t*, uint16_t)
{
    return NULL;
}

const char * MyI2cReceive(uint16_t, uint8_t*, uint16_t, uint32_t *)
{
    return NULL;
}

SCENARIO("NoteGetFnI2C")
{
    GIVEN("I2C hooks and static variables have previously been set") {
        i2cAddress = MyI2cAddress;
        i2cMax = MyI2cMax;
        hookI2CReset = MyI2cReset;
        hookI2CTransmit = MyI2cTransmit;
        hookI2CReceive = MyI2cReceive;

        WHEN("NoteGetFnI2C is called") {
            NoteGetFnI2C(NULL, NULL, NULL, NULL, NULL);

            THEN("It should not crash") {
                SUCCEED();
            }

            THEN("The Notecard lock is taken and released") {
                CHECK(1 == _noteLockNote_fake.call_count);
                CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
            }
        }

        AND_GIVEN("Valid empty pointers for all I2C functions") {
            uint32_t notecardAddr = 0;
            uint32_t maxTransmitSize = 0;
            i2cResetFn resetFunc = NULL;
            i2cTransmitFn transmitFunc = NULL;
            i2cReceiveFn receiveFunc = NULL;

            WHEN("NoteGetFnI2C is called") {
                NoteGetFnI2C(&notecardAddr, &maxTransmitSize, &resetFunc, &transmitFunc, &receiveFunc);

                THEN("It should return the mock functions") {
                    CHECK(notecardAddr == MyI2cAddress);
                    CHECK(maxTransmitSize == MyI2cMax);
                    CHECK(resetFunc == MyI2cReset);
                    CHECK(transmitFunc == MyI2cTransmit);
                    CHECK(receiveFunc == MyI2cReceive);
                }

                THEN("The Notecard lock is taken and released") {
                    CHECK(1 == _noteLockNote_fake.call_count);
                    CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the I2C address is provided") {
            uint32_t notecardAddr = 0;

            WHEN("NoteGetFnI2C is called") {
                NoteGetFnI2C(&notecardAddr, NULL, NULL, NULL, NULL);

                THEN("It should return the mock address") {
                    CHECK(notecardAddr == MyI2cAddress);
                }

                THEN("The Notecard lock is taken and released") {
                    CHECK(1 == _noteLockNote_fake.call_count);
                    CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the I2C max transmit size is provided") {
            uint32_t maxTransmitSize = 0;

            WHEN("NoteGetFnI2C is called") {
                NoteGetFnI2C(NULL, &maxTransmitSize, NULL, NULL, NULL);

                THEN("It should return the mock max transmit size") {
                    CHECK(maxTransmitSize == MyI2cMax);
                }

                THEN("The Notecard lock is taken and released") {
                    CHECK(1 == _noteLockNote_fake.call_count);
                    CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the I2C reset function is provided") {
            i2cResetFn resetFunc = NULL;

            WHEN("NoteGetFnI2C is called") {
                NoteGetFnI2C(NULL, NULL, &resetFunc, NULL, NULL);

                THEN("It should return the mock reset function") {
                    CHECK(resetFunc == MyI2cReset);
                }

                THEN("The Notecard lock is taken and released") {
                    CHECK(1 == _noteLockNote_fake.call_count);
                    CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the I2C transmit function is provided") {
            i2cTransmitFn transmitFunc = NULL;

            WHEN("NoteGetFnI2C is called") {
                NoteGetFnI2C(NULL, NULL, NULL, &transmitFunc, NULL);

                THEN("It should return the mock transmit function") {
                    CHECK(transmitFunc == MyI2cTransmit);
                }

                THEN("The Notecard lock is taken and released") {
                    CHECK(1 == _noteLockNote_fake.call_count);
                    CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the I2C receive function is provided") {
            i2cReceiveFn receiveFunc = NULL;

            WHEN("NoteGetFnI2C is called") {
                NoteGetFnI2C(NULL, NULL, NULL, NULL, &receiveFunc);

                THEN("It should return the mock receive function") {
                    CHECK(receiveFunc == MyI2cReceive);
                }

                THEN("The Notecard lock is taken and released") {
                    CHECK(1 == _noteLockNote_fake.call_count);
                    CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
                }
            }
        }
    }

    RESET_FAKE(_noteLockNote);
    RESET_FAKE(_noteUnlockNote);
}

}
