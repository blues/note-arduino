/*!
 * @file NoteErrorClean_test.cpp
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

namespace
{

SCENARIO("NoteErrorClean")
{
    SECTION("Low-memory mode") {
        char str[] = "{io}";

        NoteErrorClean(str);
        CHECK(strcmp(str, "") == 0);
    }

    SECTION("Braces without a prefixed space separator") {
        char str[] = "connected (session open){connected}";

        NoteErrorClean(str);
        CHECK(strcmp(str, "connected (session open)") == 0);
    }

    SECTION("hub.status response") {
        char str[] = "connected (session open) {connected}";

        NoteErrorClean(str);
        CHECK(strcmp(str, "connected (session open)") == 0);
    }

    SECTION("Multiple brace pairs") {
        char str[] = "connected (session open) {connected} {something else}";

        NoteErrorClean(str);
        CHECK(strcmp(str, "connected (session open)") == 0);
    }

    SECTION("No ending brace") {
        char str[] = "connected (session open) {connected";

        NoteErrorClean(str);
        // String should be unchanged if there's no ending brace.
        CHECK(strcmp(str, "connected (session open) {connected") == 0);
    }
}

}


