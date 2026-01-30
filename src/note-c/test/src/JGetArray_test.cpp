/*!
 * @file JGetArray_test.cpp
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

SCENARIO("JGetArray")
{
    NoteSetFnDefault(malloc, free, NULL, NULL);

    const char field[] = "list";
    J *json = JCreateObject();
    REQUIRE(json != NULL);
    J *list = JCreateArray();
    REQUIRE(list != NULL);
    JAddItemToObject(json, field, list);

    SECTION("NULL JSON") {
        CHECK(JGetArray(NULL, "") == NULL);
    }

    SECTION("Field not present") {
        CHECK(JGetArray(json, "array") == NULL);
    }

    SECTION("Field isn't an array") {
        JAddNumberToObject(json, "num", 5);
        CHECK(JGetArray(json, "num") == NULL);
    }

    SECTION("Valid") {
        CHECK(JGetArray(json, field) != NULL);
    }

    JDelete(json);
}

}


