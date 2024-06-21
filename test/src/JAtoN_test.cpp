/*!
 * @file JAtoN_test.cpp
 *
 * Written by the Blues Inc. team.
 *
 * Copyright (c) 2024 Blues Inc. MIT License. Use of this source code is
 * governed by licenses granted by the copyright holder including that found in
 * the
 * <a href="https://github.com/blues/note-c/blob/master/LICENSE">LICENSE</a>
 * file.
 *
 */

#include <catch2/catch_test_macros.hpp>
#include <cmath>

#include "n_lib.h"

namespace
{

#ifdef NOTE_C_LOW_MEM
const JNUMBER expectedNums[] = {
    3.40282e+38, // Approximate largest single-precision float value
    1.17549e-38, // Approximate smallest single-precision float value
};

const char *numStrs[] = {
    "3.40282e+38",
    "1.17549e-38"
};
#else
const JNUMBER expectedNums[] = {
    1.79769e+308, // Approximate largest double-precision float value
    2.22507e-308, // Approximate smallest double-precision float value
};

const char *numStrs[] = {
    "1.79769e+308",
    "2.22507e-308"
};
#endif // NOTE_C_LOW_MEM

const size_t NUM_TESTS = sizeof(expectedNums) / sizeof(expectedNums[0]);
const JNUMBER TOLERANCE = 1e-15;

SCENARIO("JAtoN")
{
    for (size_t i = 0; i < NUM_TESTS; ++i) {
        GIVEN(std::string("The string to convert to a JNUMBER is ") + numStrs[i]) {
            WHEN("JAtoN is called on that string") {
                JNUMBER num = JAtoN(numStrs[i], NULL);

                THEN(std::string("The number returned is (approximately) ") + numStrs[i]) {
                    JNUMBER diff = fabs(num - expectedNums[i]);
                    JNUMBER err = diff / expectedNums[i];
                    CHECK(err < TOLERANCE);
                }
            }
        }
    }
}

}
