/*!
 * @file NoteSetFnSerialDefault_test.cpp
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
extern volatile int hookActiveInterface;

namespace
{

bool MySerialReset()
{
    return true;
}

void MySerialTransmit(uint8_t* buffer, size_t size, bool flush)
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

SCENARIO("NoteSetFnSerialDefault")
{
    GIVEN("Serial hooks have not been set (i.e. NULL)") {
        hookSerialReset = NULL;
        hookSerialTransmit = NULL;
        hookSerialAvailable = NULL;
        hookSerialReceive = NULL;
        hookActiveInterface = NOTE_C_INTERFACE_NONE;

        WHEN("NoteSetFnSerialDefault is called") {
            NoteSetFnSerialDefault(MySerialReset, MySerialTransmit, MySerialAvailable,
                                   MySerialReceive);

            THEN("The serial hooks are set") {
                CHECK(MySerialReset == hookSerialReset);
                CHECK(MySerialTransmit == hookSerialTransmit);
                CHECK(MySerialAvailable == hookSerialAvailable);
                CHECK(MySerialReceive == hookSerialReceive);
            }

            THEN("_noteSetActiveInterface is called with the serial interface") {
                CHECK(1 == _noteSetActiveInterface_fake.call_count);
                CHECK(NOTE_C_INTERFACE_SERIAL == _noteSetActiveInterface_fake.arg0_val);
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
        hookActiveInterface = NOTE_C_INTERFACE_SERIAL;

        WHEN("NoteSetFnSerialDefault is called with different values") {
            auto otherReset = []() -> bool { return false; };
            auto otherTransmit = [](uint8_t* buffer, size_t size, bool flush) -> void { };
            auto otherAvailable = []() -> bool { return false; };
            auto otherReceive = []() -> char { return 'a'; };

            NoteSetFnSerialDefault(otherReset, otherTransmit, otherAvailable, otherReceive);

            THEN("The serial hooks are not changed") {
                CHECK(MySerialReset == hookSerialReset);
                CHECK(MySerialTransmit == hookSerialTransmit);
                CHECK(MySerialAvailable == hookSerialAvailable);
                CHECK(MySerialReceive == hookSerialReceive);
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
