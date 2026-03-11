/*!
 * @file JPrintUnformatted_test.cpp
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

// We typically don't write unit tests for cJSON functions. This is an exception
// because we modified some of the internal cJSON printing code to use strlcpy
// instead of strcpy, and we want to make sure we didn't introduce a bug.
SCENARIO("JPrintUnformatted")
{
    NoteSetFnDefault(malloc, free, NULL, NULL);

    GIVEN("A valid JSON object") {
        J *jsonObj = JParse("{"
                            "\"string\": \"Hello, World!\","
                            "\"number\": 42,"
                            "\"boolean\": true,"
                            "\"nullValue\": null,"
                            "\"array\": [1, \"two\", false, 3.14],"
                            "\"nestedObject\": {"
                            "\"key1\": \"value1\","
                            "\"key2\": 123,"
                            "\"key3\": {"
                            "\"subKey1\": true,"
                            "\"subKey2\": [\"apple\", \"banana\", \"cherry\"]"
                            "}"
                            "}"
                            "}");

        REQUIRE(jsonObj != NULL);

        WHEN("JPrintUnformatted is called on that object") {
            char *result = JPrintUnformatted(jsonObj);

            THEN("A non-NULL, non-zero length string is returned") {
                REQUIRE(result != NULL);
                CHECK(strlen(result) > 0);
            }

            JFree(result);
        }

        JDelete(jsonObj);
    }
}

}


