/*!
 * @file NoteSetActiveInterface_test.cpp
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

namespace
{

SCENARIO("NoteSetActiveInterface")
{
    WHEN("NoteSetActiveInterface is called with NOTE_C_INTERFACE_NONE") {
        NoteSetActiveInterface(NOTE_C_INTERFACE_NONE);

        THEN("_noteSetActiveInterface is called with `NOTE_C_INTERFACE_NONE`") {
            CHECK(NOTE_C_INTERFACE_NONE == _noteSetActiveInterface_fake.arg0_val);
        }

        THEN("The Notecard lock is taken and released") {
            CHECK(1 == _noteLockNote_fake.call_count);
            CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
        }
    }

    WHEN("NoteSetActiveInterface is called with NOTE_C_INTERFACE_I2C") {
        NoteSetActiveInterface(NOTE_C_INTERFACE_I2C);

        THEN("_noteSetActiveInterface is called with the I2C interface") {
            CHECK(NOTE_C_INTERFACE_I2C == _noteSetActiveInterface_fake.arg0_val);
        }

        THEN("The Notecard lock is taken and released") {
            CHECK(1 == _noteLockNote_fake.call_count);
            CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
        }
    }

    WHEN("NoteSetActiveInterface is called with NOTE_C_INTERFACE_SERIAL") {
        NoteSetActiveInterface(NOTE_C_INTERFACE_SERIAL);

        THEN("_noteSetActiveInterface is called with the serial interface") {
            CHECK(NOTE_C_INTERFACE_SERIAL == _noteSetActiveInterface_fake.arg0_val);
        }

        THEN("The Notecard lock is taken and released") {
            CHECK(1 == _noteLockNote_fake.call_count);
            CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
        }
    }

    RESET_FAKE(_noteLockNote);
    RESET_FAKE(_noteSetActiveInterface);
    RESET_FAKE(_noteUnlockNote);
}

}
