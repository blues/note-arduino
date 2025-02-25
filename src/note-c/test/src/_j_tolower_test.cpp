/*!
 * @file _j_tolower_test.cpp
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
#include "test_static.h"

namespace
{

SCENARIO("_j_tolower")
{
    const char alphabetUpper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char alphabetLower[] = "abcdefghijklmnopqrstuvwxyz";
    char result[sizeof(alphabetUpper)] = {0};

    GIVEN("The uppercase characters of the alphabet") {
        WHEN("_j_tolower is called on each letter") {
            for (size_t i = 0; i < sizeof(alphabetUpper); ++i) {
                result[i] = _j_tolower(alphabetUpper[i]);
            }

            THEN("The corresponding lowercase letter is returned") {
                for (size_t i = 0; i < sizeof(alphabetUpper); ++i) {
                    CHECK(result[i] == alphabetLower[i]);
                }
            }
        }
    }

    GIVEN("The lowercase characters of the alphabet") {
        WHEN("_j_tolower is called on each letter") {
            for (size_t i = 0; i < sizeof(alphabetLower); ++i) {
                result[i] = _j_tolower(alphabetLower[i]);
            }

            THEN("The same lowercase letter is returned") {
                for (size_t i = 0; i < sizeof(alphabetLower); ++i) {
                    CHECK(result[i] == alphabetLower[i]);
                }
            }
        }
    }

    GIVEN("A non-letter char") {
        char invalid = '.';

        WHEN("_j_tolower is called on that char") {
            char invalidResult = _j_tolower(invalid);

            THEN("The non-letter char is returned") {
                CHECK(invalidResult == invalid);
            }
        }
    }
}

}


