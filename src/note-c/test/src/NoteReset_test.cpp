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

#ifdef NOTE_C_TEST

#include <catch2/catch_test_macros.hpp>
#include "fff.h"

#include "n_lib.h"

DEFINE_FFF_GLOBALS
FAKE_VALUE_FUNC(bool, NoteHardReset)

namespace
{

SCENARIO("NoteReset")
{
    GIVEN("NoteReset is called") {
        NoteReset();

        THEN("NoteHardReset is invoked") {
            CHECK(NoteHardReset_fake.call_count > 0);
        }

        WHEN("`NoteHardReset` returns `false`") {
            NoteHardReset_fake.return_val = false;
            THEN("`NoteReset` also returns `false`") {
                bool result = NoteReset();
                CHECK(result == false);
            }
        }

        WHEN("`NoteHardReset` returns `true`") {
            NoteHardReset_fake.return_val = true;
            THEN("`NoteReset` also returns `true`") {
                bool result = NoteReset();
                CHECK(result == true);
            }
        }
    }

    RESET_FAKE(NoteHardReset);
}

}

#endif // NOTE_C_TEST
