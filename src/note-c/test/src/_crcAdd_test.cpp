/*!
 * @file _crcAdd_test.cpp
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

#ifndef NOTE_C_LOW_MEM

#include <catch2/catch_test_macros.hpp>
#include <fff.h>

#include "n_lib.h"

DEFINE_FFF_GLOBALS
FAKE_VALUE_FUNC(void *, NoteMalloc, size_t)

namespace
{

SCENARIO("_crcAdd")
{
    NoteSetFnDefault(malloc, free, NULL, NULL);

    char validReq[] = "{\"req\": \"hub.sync\"}";
    uint16_t seqNo = 1;

    SECTION("NoteMalloc fails") {
        NoteMalloc_fake.return_val = NULL;

        CHECK(_crcAdd(validReq, seqNo) == NULL);
    }

    SECTION("NoteMalloc succeeds") {
        NoteMalloc_fake.custom_fake = malloc;

        char emptyStringReq[] = "";
        char emptyObjectReq[] = "{}";
        char invalidJsonReq[] = "{\"req\":";

        SECTION("Empty string") {
            CHECK(_crcAdd(emptyStringReq, seqNo) == NULL);
        }

        SECTION("Empty object") {
            const char expectedNewJson[] = "{ \"crc\":\"0001:A3A6BF43\"}";
            char *newJson = _crcAdd(emptyObjectReq, seqNo);

            REQUIRE(newJson != NULL);
            CHECK(strcmp(expectedNewJson, newJson) == 0);

            NoteFree(newJson);
        }

        SECTION("Invalid JSON") {
            CHECK(_crcAdd(invalidJsonReq, seqNo) == NULL);
        }

        SECTION("Valid JSON") {
            const char expectedNewJson[] = "{\"req\": \"hub.sync\",\"crc\":\"0001:DF2B9115\"}";
            char *newJson = _crcAdd(validReq, seqNo);

            REQUIRE(newJson != NULL);
            CHECK(strcmp(expectedNewJson, newJson) == 0);

            NoteFree(newJson);
        }
    }

    RESET_FAKE(NoteMalloc);
}

}

#endif // !NOTE_C_LOW_MEM
