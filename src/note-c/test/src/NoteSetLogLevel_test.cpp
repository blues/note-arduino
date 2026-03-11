/*!
 * @file NoteSetLogLevel_test.cpp
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

extern int noteLogLevel;

namespace
{

SCENARIO("NoteSetLogLevel")
{
    GIVEN("noteLogLevel is set to the default value") {
#ifndef NOTE_NODEBUG
        noteLogLevel = NOTE_C_LOG_LEVEL_DEFAULT;
#endif

        WHEN("NoteSetLogLevel is called with a new value") {
            REQUIRE(NOTE_C_LOG_LEVEL_DEFAULT != NOTE_C_LOG_LEVEL_WARN);
            NoteSetLogLevel(NOTE_C_LOG_LEVEL_WARN);

#ifndef NOTE_NODEBUG
            THEN("noteLogLevel is set to the new value") {
                CHECK(noteLogLevel == NOTE_C_LOG_LEVEL_WARN);
            }
#else
            THEN("no errors occur") {
                SUCCEED();
            }
#endif
        }
    }
}

}
