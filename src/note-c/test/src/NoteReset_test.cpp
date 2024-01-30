/*!
 * @file NoteReset_test.cpp
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
#include "fff.h"

#include "n_lib.h"

DEFINE_FFF_GLOBALS
FAKE_VALUE_FUNC(bool, noteHardReset)

namespace
{

SCENARIO("NoteReset")
{
    GIVEN("NoteReset is called") {
        NoteReset();

        THEN("noteHardReset is invoked") {
            CHECK(noteHardReset_fake.call_count > 0);
        }

        WHEN("`noteHardReset` returns `false`") {
            noteHardReset_fake.return_val = false;
            THEN("`NoteReset` also returns `false`") {
                bool result = NoteReset();
                CHECK(result == false);
            }
        }

        WHEN("`noteHardReset` returns `true`") {
            noteHardReset_fake.return_val = true;
            THEN("`NoteReset` also returns `true`") {
                bool result = NoteReset();
                CHECK(result == true);
            }
        }
    }

    RESET_FAKE(noteHardReset);
}

}


