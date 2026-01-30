/*!
 * @file NoteSetFnDebugOutput_test.cpp
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
FAKE_VOID_FUNC(_noteUnlockNote)

extern debugOutputFn hookDebugOutput;

namespace
{

size_t MyDebug(const char *)
{
    return 0;
}

SCENARIO("NoteSetFnDebugOutput")
{
    GIVEN("Debug hook has not been set (i.e. NULL)") {
        hookDebugOutput = NULL;

        WHEN("NoteSetFnDebugOutput is called") {
            NoteSetFnDebugOutput(MyDebug);

            THEN("The hooks are set") {
                CHECK(hookDebugOutput == MyDebug);
            }

            THEN("The Notecard lock is taken and released") {
                CHECK(1 == _noteLockNote_fake.call_count);
                CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
            }
        }
    }

    GIVEN("Debug hook has been set with a non-NULL value") {
        hookDebugOutput = MyDebug;

        WHEN("NoteSetFnDebugOutput is called") {
            NoteSetFnDebugOutput(NULL);

            THEN("The existing hook is overridden") {
                CHECK(hookDebugOutput == NULL);
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
