/*!
 * @file NoteBinaryStoreEncodedLength_test.cpp
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
FAKE_VALUE_FUNC(J *, NoteRequestResponse, J *)

const uint32_t cobsLen = 10;

namespace
{

SCENARIO("NoteBinaryStoreEncodedLength")
{
    NoteSetFnDefault(malloc, free, NULL, NULL);

    uint32_t size = 1;

    GIVEN("Bad parameters are supplied") {
        WHEN("Length is NULL") {
            const char *err = NoteBinaryStoreEncodedLength(NULL);

            THEN("An error is returned") {
                CHECK(err != NULL);
            }
        }
    }

    GIVEN("The card.binary request fails") {
        NoteRequestResponse_fake.custom_fake = [](J *req) -> J * {
            JDelete(req);

            return NULL;
        };

        WHEN("NoteBinaryStoreEncodedLength is called") {
            const char *err = NoteBinaryStoreEncodedLength(&size);

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

        WHEN("NoteBinaryStoreEncodedLength is called") {
            const char *err = NoteBinaryStoreEncodedLength(&size);

            THEN("An error is returned") {
                CHECK(err != NULL);
            }
        }
    }

    GIVEN("The response to the card.binary request indicates there's nothing to"
          " read") {
        NoteRequestResponse_fake.custom_fake = [](J *req) -> J * {
            JDelete(req);
            J *rsp = JCreateObject();
            JAddIntToObject(rsp, "cobs", 0);

            return rsp;
        };

        WHEN("NoteBinaryStoreEncodedLength is called") {
            const char *err = NoteBinaryStoreEncodedLength(&size);

            THEN("An error is not returned") {
                CHECK(err == NULL);
            }

            THEN("The size out parameter is 0") {
                CHECK(size == 0);
            }
        }
    }

    GIVEN("The response to the card.binary request indicates there's data "
          "to read") {
        NoteRequestResponse_fake.custom_fake = [](J *req) -> J * {
            JDelete(req);
            J *rsp = JCreateObject();
            JAddIntToObject(rsp, "cobs", cobsLen);

            return rsp;
        };

        WHEN("NoteBinaryStoreEncodedLength is called") {
            const char *err = NoteBinaryStoreEncodedLength(&size);

            THEN("An error is not returned") {
                CHECK(err == NULL);
            }

            THEN("The size out parameter is the cobs value in the card.binary "
                 "response") {
                CHECK(size == cobsLen);
            }
        }
    }

    RESET_FAKE(NoteRequestResponse);
}

}


