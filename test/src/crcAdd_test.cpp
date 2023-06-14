/*!
 * @file crcAdd_test.cpp
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

#ifdef NOTE_C_TEST

#include <catch2/catch_test_macros.hpp>
#include "fff.h"

#include "n_lib.h"
#include "test_static.h"

DEFINE_FFF_GLOBALS
FAKE_VALUE_FUNC(void *, NoteMalloc, size_t)

namespace
{

TEST_CASE("crcAdd")
{
    NoteSetFnDefault(malloc, free, NULL, NULL);

    RESET_FAKE(NoteMalloc);

    char validReq[] = "{\"req\": \"hub.sync\"}";
    uint16_t seqNo = 1;

    SECTION("NoteMalloc fails") {
        NoteMalloc_fake.return_val = NULL;

        CHECK(crcAdd(validReq, seqNo) == NULL);
    }

    SECTION("NoteMalloc succeeds") {
        NoteMalloc_fake.custom_fake = malloc;

        char emptyStringReq[] = "";
        char invalidJsonReq[] = "{\"req\":";

        SECTION("Empty string") {
            CHECK(crcAdd(emptyStringReq, seqNo) == NULL);
        }

        SECTION("Invalid JSON") {
            CHECK(crcAdd(invalidJsonReq, seqNo) == NULL);
        }

        SECTION("Valid JSON") {
            const char expectedNewJson[] = "{\"req\": \"hub.sync\",\"crc\":\"0001:DF2B9115\"}";
            char *newJson = crcAdd(validReq, seqNo);

            REQUIRE(newJson != NULL);
            CHECK(strcmp(expectedNewJson, newJson) == 0);

            NoteFree(newJson);
        }
    }
}

}

#endif // TEST
