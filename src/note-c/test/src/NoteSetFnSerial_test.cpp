/*!
 * @file NoteSetFnSerial_test.cpp
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

extern serialResetFn hookSerialReset;
extern serialTransmitFn hookSerialTransmit;
extern serialAvailableFn hookSerialAvailable;
extern serialReceiveFn hookSerialReceive;

namespace
{

bool MySerialReset()
{
    return true;
}

void MySerialTransmit(uint8_t *, size_t, bool)
{
}

bool MySerialAvailable()
{
    return true;
}

char MySerialReceive()
{
    return 'z';
}

SCENARIO("NoteSetFnSerial")
{
    GIVEN("Serial hooks have not been set (i.e. NULL)") {
        hookSerialReset = NULL;
        hookSerialTransmit = NULL;
        hookSerialAvailable = NULL;
        hookSerialReceive = NULL;

        WHEN("NoteSetFnSerial is called") {
            NoteSetFnSerial(MySerialReset, MySerialTransmit, MySerialAvailable,
                MySerialReceive);

            THEN("_noteSetActiveInterface is called with the serial interface") {
                CHECK(NOTE_C_INTERFACE_SERIAL == _noteSetActiveInterface_fake.arg0_val);
            }

            THEN("The serial hooks are set") {
                CHECK(MySerialReset == hookSerialReset);
                CHECK(MySerialTransmit == hookSerialTransmit);
                CHECK(MySerialAvailable == hookSerialAvailable);
                CHECK(MySerialReceive == hookSerialReceive);
            }

            THEN("The Notecard lock is taken and released") {
                CHECK(1 == _noteLockNote_fake.call_count);
                CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
            }
        }
    }

    GIVEN("Serial hooks have previously been set") {
        hookSerialReset = MySerialReset;
        hookSerialTransmit = MySerialTransmit;
        hookSerialAvailable = MySerialAvailable;
        hookSerialReceive = MySerialReceive;

        WHEN("NoteSetFnSerial is called") {
            NoteSetFnSerial(NULL, NULL, NULL, NULL);

            THEN("_noteSetActiveInterface is called with the serial interface") {
                CHECK(NOTE_C_INTERFACE_SERIAL == _noteSetActiveInterface_fake.arg0_val);
            }

            THEN("The serial hooks are set") {
                CHECK(NULL == hookSerialReset);
                CHECK(NULL == hookSerialTransmit);
                CHECK(NULL == hookSerialAvailable);
                CHECK(NULL == hookSerialReceive);
            }

            THEN("The Notecard lock is taken and released") {
                CHECK(1 == _noteLockNote_fake.call_count);
                CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
            }
        }
    }

    RESET_FAKE(_noteLockNote);
    RESET_FAKE(_noteUnlockNote);
}

}
