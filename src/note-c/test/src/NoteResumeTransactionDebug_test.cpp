/*!
 * @file NoteResumeTransactionDebug_test.cpp
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
#include <fff.h>

#include "n_lib.h"

#include <cstddef>

DEFINE_FFF_GLOBALS

FAKE_VOID_FUNC(_noteResumeTransactionDebug)

namespace
{

SCENARIO("NoteResumeTransactionDebug")
{
    WHEN("NoteResumeTransactionDebug is called") {
        NoteResumeTransactionDebug();

        THEN("_noteResumeTransactionDebug is called once") {
            CHECK(_noteResumeTransactionDebug_fake.call_count == 1);
        }
    }

    RESET_FAKE(_noteResumeTransactionDebug);
}

}
