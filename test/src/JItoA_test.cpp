/*!
 * @file JItoA_test.cpp
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

#if JINTEGER_MIN == -2147483648
const char JINTEGER_MIN_STR[] = "-2147483648";
// The weird form of the RHS of this conditional resolves this warning:
// "warning: integer constant is so large that it is unsigned". See:
// https://stackoverflow.com/a/65008305
#elif JINTEGER_MIN == (-9223372036854775807 - 1)
const char JINTEGER_MIN_STR[] = "-9223372036854775808";
#else
#error "Couldn't determine value of JINTEGER_MIN."
#endif

#if JINTEGER_MAX == 2147483647
const char JINTEGER_MAX_STR[] = "2147483647";
#elif JINTEGER_MAX == 9223372036854775807
const char JINTEGER_MAX_STR[] = "9223372036854775807";
#else
#error "Couldn't determine value of JINTEGER_MAX."
#endif

SCENARIO("JItoA")
{
    JINTEGER n;
    char text[16];

    GIVEN("The number to convert to a string is 0") {
        n = 0;

        WHEN("JItoA is called on it") {
            JItoA(n, text);

            THEN("The correct string is produced") {
                CHECK(strcmp(text, "0") == 0);
            }
        }
    }

    GIVEN("The number to convert to a string is non-zero") {
        n = 1;

        WHEN("JItoA is called on it") {
            JItoA(n, text);

            THEN("The correct string is produced") {
                CHECK(strcmp(text, "1") == 0);
            }
        }
    }

    GIVEN("The number to convert to a string is positive with trailing zeros") {
        n = 1234500;

        WHEN("JItoA is called on it") {
            JItoA(n, text);

            THEN("The correct string is produced") {
                CHECK(strcmp(text, "1234500") == 0);
            }
        }
    }

    GIVEN("The number to convert to a string is negative with trailing zeros") {
        n = -1234500;

        WHEN("JItoA is called on it") {
            JItoA(n, text);

            THEN("The correct string is produced") {
                CHECK(strcmp(text, "-1234500") == 0);
            }
        }
    }

    GIVEN("The number to convert to a string is JINTEGER_MIN") {
        n = JINTEGER_MIN;

        WHEN("JItoA is called on it") {
            JItoA(n, text);

            THEN("The correct string is produced") {
                CHECK(strcmp(text, JINTEGER_MIN_STR) == 0);
            }
        }
    }

    GIVEN("The number to convert to a string is JINTEGER_MAX") {
        n = JINTEGER_MAX;

        WHEN("JItoA is called on it") {
            JItoA(n, text);

            THEN("The correct string is produced") {
                CHECK(strcmp(text, JINTEGER_MAX_STR) == 0);
            }
        }
    }
}

}
