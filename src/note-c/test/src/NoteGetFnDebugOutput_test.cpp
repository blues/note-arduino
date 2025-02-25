/*!
 * @file NoteGetFnDebugOutput_test.cpp
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

size_t mockDebugOutput(const char *message)
{
    (void)message;
    return 0;
}

SCENARIO("NoteGetFnDebugOutput")
{
    GIVEN("A mock debug output function") {
        NoteSetFnDebugOutput(mockDebugOutput);

        WHEN("Get debug output function with NULL parameter") {
            THEN("It should not crash")
            NoteGetFnDebugOutput(NULL);
            {
            }

            WHEN("Get debug output function with valid parameter") {
                THEN("It should return the mock function") {
                    debugOutputFn fn = NULL;
                    NoteGetFnDebugOutput(&fn);
                    REQUIRE(fn == mockDebugOutput);
                }
            }
        }
    }

}
}
