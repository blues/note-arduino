/*!
 * @file NoteGetActiveInterface_test.cpp
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

extern volatile int hookActiveInterface;

namespace
{

SCENARIO("NoteGetActiveInterface")
{
    WHEN("NoteGetActiveInterface is called") {
        const int interface = NoteGetActiveInterface();

        THEN("the value of hookActiveInterface is returned") {
            CHECK(interface == hookActiveInterface);
        }
    }
}

}
