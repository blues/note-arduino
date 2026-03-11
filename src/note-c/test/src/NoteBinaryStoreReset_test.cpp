/*!
 * @file NoteBinaryStoreReset_test.cpp
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
FAKE_VALUE_FUNC(J *, NoteRequestResponse, J *)

namespace
{

SCENARIO("NoteBinaryStoreReset")
{
    NoteSetFnDefault(malloc, free, NULL, NULL);

    NoteNewRequest_fake.custom_fake = [](const char *req) -> J * {
        return JCreateObject();
    };
    NoteRequestResponse_fake.custom_fake = [](J *req) -> J * {
        JDelete(req);
        return JCreateObject();
    };

    GIVEN("NoteNewRequest fails to allocate the card.binary request") {
        NoteNewRequest_fake.custom_fake = NULL;
        NoteNewRequest_fake.return_val = NULL;

        WHEN("NoteBinaryStoreReset is called") {
            const char *err = NoteBinaryStoreReset();

            THEN("An error is returned") {
                CHECK(err != NULL);
            }
        }
    }

    GIVEN("The response to the card.binary request has an error") {
        NoteRequestResponse_fake.custom_fake = [](J *req) -> J * {
            JDelete(req);
            J *rsp = JCreateObject();
            JAddStringToObject(rsp, "err", "some error");

            return rsp;
        };

        WHEN("NoteBinaryStoreReset is called") {
            const char *err = NoteBinaryStoreReset();

            THEN("An error is returned") {
                CHECK(err != NULL);
            }
        }
    }

    GIVEN("The response to the card.binary indicates success") {
        WHEN("NoteBinaryStoreReset is called") {
            const char *err = NoteBinaryStoreReset();

            THEN("An error is not returned") {
                CHECK(err == NULL);
            }
        }
    }

    RESET_FAKE(NoteNewRequest);
    RESET_FAKE(NoteRequestResponse);
}

}


