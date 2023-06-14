/*!
 * @file crcError_test.cpp
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

#include "test_static.h"

namespace
{

TEST_CASE("crcError")
{
    NoteSetFnDefault(malloc, free, NULL, NULL);

    uint16_t seqNo = 1;

    SECTION("Empty string") {
        char json[] = "";

        CHECK(!crcError(json, seqNo));
    }

    SECTION("Invalid JSON") {
        char json[] = "{\"req\":";

        CHECK(!crcError(json, seqNo));
    }

    SECTION("No CRC field") {
        char json[] = "{\"req\": \"hub.sync\"}";

        CHECK(!crcError(json, seqNo));
    }

    SECTION("CRC field at unexpected position") {
        char json[] = "{\"crc\":\"0009:10BAC79A\",\"req\": \"hub.sync\"}";

        CHECK(!crcError(json, seqNo));
    }

    SECTION("Valid JSON and CRC field present") {

        SECTION("CRC doesn't match") {
            char json[] = "{\"req\":\"hub.sync\",\"crc\":\"0001:DEADBEEF\"}";

            CHECK(crcError(json, seqNo));
        }

        SECTION("Sequence number doesn't match") {
            char json[] = "{\"req\":\"hub.sync\",\"crc\":\"0009:10BAC79A\"}";

            CHECK(crcError(json, seqNo));
        }

        SECTION("Everything matches") {
            char json[] = "{\"req\":\"hub.sync\"}";
            char *jsonWithCrc = crcAdd(json, seqNo);
            REQUIRE(jsonWithCrc != NULL);

            CHECK(!crcError(jsonWithCrc, seqNo));

            NoteFree(jsonWithCrc);
        }
    }
}

}

#endif // TEST
