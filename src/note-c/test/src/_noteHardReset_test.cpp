/*!
 * @file _noteHardReset_test.cpp
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

typedef bool (*nNoteResetFn) (void); // MUST BE IDENTICAL TO THE FUNCTION POINTER TYPE IN `n_hooks.c`

extern nNoteResetFn notecardReset;

namespace
{

const bool hookResult = false;

SCENARIO("_noteHardReset")
{
    GIVEN("notecardReset is unset (NULL)") {
        notecardReset = NULL;

        WHEN("_noteHardReset is called") {
            const bool result = _noteHardReset();

            THEN("it returns true") {
                CHECK(result);
            }
        }
    }

    GIVEN("notecardReset is set") {
        notecardReset = []() -> bool {
            return hookResult;
        };

        WHEN("_noteHardReset is called") {
            const bool result = _noteHardReset();

            THEN("it returns the value returned by notecardReset") {
                CHECK(result == hookResult);
            }
        }
    }
}

}
