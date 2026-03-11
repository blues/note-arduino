/*!
 * @file NoteDelayMs_test.cpp
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

extern delayMsFn hookDelayMs;

namespace
{

uint32_t delayMs = 0;

SCENARIO("NoteDelayMs")
{
    GIVEN("hookDelayMs is unset (NULL)") {
        hookDelayMs = NULL;

        WHEN("NoteDelayMs is called") {
            NoteDelayMs(1234);

            THEN("nothing happens") {
                // Nothing to check
                SUCCEED();
            }
        }
    }

    GIVEN("hookDelayMs is set") {
        hookDelayMs = [](uint32_t ms) {
            delayMs = ms;
        };

        WHEN("NoteDelayMs is called") {
            NoteDelayMs(1234);

            THEN("the value passed to NoteDelayMs is passed to hookDelayMs") {
                CHECK(1234 == delayMs);
            }
        }
    }
}

}
