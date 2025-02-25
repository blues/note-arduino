/*!
 * @file JNtoA_test.cpp
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

#ifdef NOTE_C_SINGLE_PRECISION
const JNUMBER nums[] = {
    3.40282e+38, // Approximate largest single-precision float value
    1.17549e-38, // Approximate smallest single-precision float value
};

const char *expectedNumStrs[] = {
    "3.40282e+38",
    "1.17549e-38"
};
#else
const JNUMBER nums[] = {
    1.79769e+308, // Approximate largest double-precision float value
    2.22507e-308, // Approximate smallest double-precision float value
};

const char *expectedNumStrs[] = {
    "1.79769e+308",
    "2.22507e-308"
};
#endif // NOTE_C_SINGLE_PRECISION

const JNUMBER TOLERANCE = 1e-8;

const size_t NUM_TESTS = sizeof(nums) / sizeof(nums[0]);

SCENARIO("JNtoA")
{
    char numStr[JNTOA_MAX] = {0};

    for (size_t i = 0; i < NUM_TESTS; ++i) {
        GIVEN(std::string("The number to convert to a string is ") + expectedNumStrs[i]) {
            WHEN("JNtoA is called on that number") {
                JNtoA(nums[i], numStr, -1);

                THEN(std::string("The string returned is (approximately)") + expectedNumStrs[i]) {
                    double extractedNum;
                    sscanf(expectedNumStrs[i], "%lf", &extractedNum);
                    JNUMBER diff = fabs(extractedNum - nums[i]);
                    JNUMBER err = diff / nums[i];
                    CHECK(err < TOLERANCE);
                }
            }
        }
    }
}

}
