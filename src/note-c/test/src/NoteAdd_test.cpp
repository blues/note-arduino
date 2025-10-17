/*!
 * @file NoteAdd_test.cpp
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
#include <fff.h>

#include "n_lib.h"

DEFINE_FFF_GLOBALS
FAKE_VALUE_FUNC(J *, NoteNewRequest, const char *)
FAKE_VALUE_FUNC(bool, NoteRequest, J *)

namespace
{

SCENARIO("NoteAdd")
{
    NoteSetFnDefault(malloc, free, NULL, NULL);

    const char file[] = "data.qo";
    J *body = JCreateObject();
    REQUIRE(body != NULL);

    SECTION("NoteNewRequest fails") {
        NoteNewRequest_fake.return_val = NULL;

        CHECK(!NoteAdd(file, body, false));
    }

    SECTION("NoteRequest") {
        J *req = JCreateObject();
        NoteNewRequest_fake.return_val = req;

        SECTION("Fails") {
            NoteRequest_fake.return_val = false;

            CHECK(!NoteAdd(file, body, false));
        }

        SECTION("Succeeds") {
            NoteRequest_fake.return_val = true;

            SECTION("Not urgent") {
                CHECK(NoteAdd(file, body, false));
                CHECK(!JGetBool(req, "start"));
            }

            SECTION("Urgent") {
                CHECK(NoteAdd(file, body, true));
                CHECK(JGetBool(req, "start"));
            }
        }

        JDelete(req);
    }

    RESET_FAKE(NoteNewRequest);
    RESET_FAKE(NoteRequest);
}

}


