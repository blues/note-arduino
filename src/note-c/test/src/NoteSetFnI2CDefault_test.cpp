/*!
 * @file NoteSetFnI2CDefault_test.cpp
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
#include <fff.h>

#include "n_lib.h"

DEFINE_FFF_GLOBALS
FAKE_VOID_FUNC(_noteLockNote)
FAKE_VOID_FUNC(_noteSetActiveInterface, int)
FAKE_VOID_FUNC(_noteUnlockNote)

extern uint32_t i2cAddress;
extern uint32_t i2cMax;
extern i2cResetFn hookI2CReset;
extern i2cTransmitFn hookI2CTransmit;
extern i2cReceiveFn hookI2CReceive;
extern volatile int hookActiveInterface;

namespace
{

const uint32_t MyI2cAddress = 0x19790917;
const uint32_t MyI2cMax = 79;

bool MyI2cReset(uint16_t addr)
{
    return true;
}

const char *MyI2cTransmit(uint16_t addr, uint8_t* buffer, uint16_t size)
{
    return NULL;
}

const char *MyI2cReceive(uint16_t addr, uint8_t* buffer, uint16_t size, uint32_t* available)
{
    return NULL;
}

SCENARIO("NoteSetFnI2CDefault")
{
    GIVEN("I2C hooks and static variables have not been set (i.e. NULL)") {
        i2cAddress = 0;
        i2cMax = 0;
        hookI2CReset = NULL;
        hookI2CTransmit = NULL;
        hookI2CReceive = NULL;
        hookActiveInterface = NOTE_C_INTERFACE_NONE;

        WHEN("NoteSetFnI2CDefault is called") {
            NoteSetFnI2CDefault(MyI2cAddress, MyI2cMax, MyI2cReset, MyI2cTransmit, MyI2cReceive);

            THEN("The static variables are set") {
                CHECK(MyI2cAddress == i2cAddress);
                CHECK(MyI2cMax == i2cMax);
            }

            THEN("The I2C hooks are set") {
                CHECK(MyI2cReset == hookI2CReset);
                CHECK(MyI2cTransmit == hookI2CTransmit);
                CHECK(MyI2cReceive == hookI2CReceive);
            }

            THEN("_noteSetActiveInterface is called with the I2C interface") {
                CHECK(1 == _noteSetActiveInterface_fake.call_count);
                CHECK(NOTE_C_INTERFACE_I2C == _noteSetActiveInterface_fake.arg0_val);
            }

            THEN("The Notecard lock is taken and released") {
                CHECK(1 == _noteLockNote_fake.call_count);
                CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
            }
        }
    }

    GIVEN("I2C hooks and static variables have previously been set") {
        i2cAddress = MyI2cAddress;
        i2cMax = MyI2cMax;
        hookI2CReset = MyI2cReset;
        hookI2CTransmit = MyI2cTransmit;
        hookI2CReceive = MyI2cReceive;
        hookActiveInterface = NOTE_C_INTERFACE_I2C;

        WHEN("NoteSetFnI2CDefault is called with different values") {
            const uint32_t otherAddress = 0x42;
            const uint32_t otherMax = 42;

            auto otherReset = [](uint16_t addr) -> bool { return false; };
            auto otherTransmit = [](uint16_t addr, uint8_t* buffer, uint16_t size) -> const char* { return "test"; };
            auto otherReceive = [](uint16_t addr, uint8_t* buffer, uint16_t size, uint32_t* available) -> const char* { return "test"; };

            NoteSetFnI2CDefault(otherAddress, otherMax, otherReset, otherTransmit, otherReceive);

            THEN("The static variables are not changed") {
                CHECK(MyI2cAddress == i2cAddress);
                CHECK(MyI2cMax == i2cMax);
            }

            THEN("The I2C hooks are not changed") {
                CHECK(MyI2cReset == hookI2CReset);
                CHECK(MyI2cTransmit == hookI2CTransmit);
                CHECK(MyI2cReceive == hookI2CReceive);
            }

            THEN("_noteSetActiveInterface is not called") {
                CHECK(0 == _noteSetActiveInterface_fake.call_count);
            }

            THEN("The Notecard lock is taken and released") {
                CHECK(1 == _noteLockNote_fake.call_count);
                CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
            }
        }
    }

    RESET_FAKE(_noteLockNote);
    RESET_FAKE(_noteSetActiveInterface);
    RESET_FAKE(_noteUnlockNote);
}

}
