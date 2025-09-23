/*!
 * @file NoteGetMs_test.cpp
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

extern getMsFn hookGetMs;

namespace
{

SCENARIO("NoteGetMs")
{
    GIVEN("hookGetMs is unset (NULL)") {
        hookGetMs = NULL;

        WHEN("NoteGetMs is called") {
            const uint32_t result = NoteGetMs();

            THEN("0 is returned") {
                CHECK(0 == result);
            }
        }
    }

    GIVEN("hookGetMs is set") {
        hookGetMs = []() -> uint32_t {
            return 1234;
        };

        WHEN("NoteGetMs is called") {
            const uint32_t result = NoteGetMs();

            THEN("the value from hookGetMs is returned") {
                CHECK(1234 == result);
            }
        }
    }
}

}
