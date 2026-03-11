/*!
 * @file JBaseItemType_test.cpp
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

SCENARIO("JBaseItemType")
{
    GIVEN("A BOOL JSON") {
        WHEN("The value is JTYPE_BOOL_TRUE") {
            THEN("The type is JTYPE_BOOL") {
                CHECK(JBaseItemType(JTYPE_BOOL_TRUE) == JTYPE_BOOL);
            }
        }
        WHEN("The value is JTYPE_BOOL_FALSE") {
            THEN("The type is JTYPE_BOOL") {
                CHECK(JBaseItemType(JTYPE_BOOL_FALSE) == JTYPE_BOOL);
            }
        }
    }

    GIVEN("A NUMBER JSON") {
        WHEN("The value is JTYPE_NUMBER_ZERO") {
            THEN("The type is JTYPE_NUMBER") {
                CHECK(JBaseItemType(JTYPE_NUMBER_ZERO) == JTYPE_NUMBER);
            }
        }
    }

    GIVEN("A STRING JSON") {
        WHEN("The value is JTYPE_STRING_BLANK") {
            THEN("The type is JTYPE_STRING") {
                CHECK(JBaseItemType(JTYPE_STRING_BLANK) == JTYPE_STRING);
            }
        }
        WHEN("The value is JTYPE_STRING_ZERO") {
            THEN("The type is JTYPE_STRING") {
                CHECK(JBaseItemType(JTYPE_STRING_ZERO) == JTYPE_STRING);
            }
        }
        WHEN("The value is JTYPE_STRING_NUMBER") {
            THEN("The type is JTYPE_STRING") {
                CHECK(JBaseItemType(JTYPE_STRING_NUMBER) == JTYPE_STRING);
            }
        }
        WHEN("The value is JTYPE_STRING_BOOL_TRUE") {
            THEN("The type is JTYPE_STRING") {
                CHECK(JBaseItemType(JTYPE_STRING_BOOL_TRUE) == JTYPE_STRING);
            }
        }
        WHEN("The value is JTYPE_STRING_BOOL_FALSE") {
            THEN("The type is JTYPE_STRING") {
                CHECK(JBaseItemType(JTYPE_STRING_BOOL_FALSE) == JTYPE_STRING);
            }
        }
    }

    GIVEN("A BASE TYPE JSON") {
        WHEN("The value is JTYPE_BOOL") {
            THEN("The type is JTYPE_BOOL") {
                CHECK(JBaseItemType(JTYPE_BOOL) == JTYPE_BOOL);
            }
        }
        WHEN("The value is JTYPE_NUMBER") {
            THEN("The type is JTYPE_NUMBER") {
                CHECK(JBaseItemType(JTYPE_NUMBER) == JTYPE_NUMBER);
            }
        }
        WHEN("The value is JTYPE_STRING") {
            THEN("The type is JTYPE_STRING") {
                CHECK(JBaseItemType(JTYPE_STRING) == JTYPE_STRING);
            }
        }
        WHEN("The value is JTYPE_ARRAY") {
            THEN("The type is JTYPE_ARRAY") {
                CHECK(JBaseItemType(JTYPE_ARRAY) == JTYPE_ARRAY);
            }
        }
        WHEN("The value is JTYPE_OBJECT") {
            THEN("The type is JTYPE_OBJECT") {
                CHECK(JBaseItemType(JTYPE_OBJECT) == JTYPE_OBJECT);
            }
        }
    }
}

}


