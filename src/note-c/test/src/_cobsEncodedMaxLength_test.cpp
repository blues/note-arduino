/*!
 * @file _cobsEncodedMaxLength_test.cpp
 *
 * Written by the Blues Inc. team.
 *
 * Copyright (c) 2025 Blues Inc. MIT License. Use of this source code is
 * governed by licenses granted by the copyright holder including that found in
 * the
 * <a href="https://github.com/blues/note-c/blob/master/LICENSE">LICENSE</a>
 * file.
 *
 */

#ifndef NOTE_C_LOW_MEM

#include <catch2/catch_test_macros.hpp>

#include "n_lib.h"

namespace
{

SCENARIO("_cobsEncodedMaxLength")
{
    GIVEN("A buffer length less than the COBS max packet size of 254 bytes") {
        const uint32_t bufLen = 6;

        WHEN("The max encoded length is calculated") {
            const uint32_t encodedLen = _cobsEncodedMaxLength(bufLen);

            THEN("The encoded length is correct") {
                const uint32_t expectedLen = 8;
                CHECK(encodedLen == expectedLen);
            }
        }
    }

    GIVEN("A buffer length greater than the COBS max packet size of 254 bytes") {
        // https://discuss.blues.com/t/cobs-off-by-one-error-with-encoding-decoding-lengths/3435
        const uint32_t bufLen = 65536;

        WHEN("The max encoded length is calculated") {
            const uint32_t encodedLen = _cobsEncodedMaxLength(bufLen);

            THEN("The encoded length is correct") {
                const uint32_t expectedLen = 65796;
                CHECK(encodedLen == expectedLen);
            }
        }
    }

    GIVEN("A buffer length of 0 bytes") {
        const uint32_t bufLen = 0;

        WHEN("The max encoded length is calculated") {
            const uint32_t encodedLen = _cobsEncodedMaxLength(bufLen);

            THEN("The encoded length is correct") {
                const uint32_t expectedLen = 2;
                CHECK(encodedLen == expectedLen);
            }
        }
    }

    GIVEN("A buffer length of 1 byte") {
        const uint32_t bufLen = 1;

        WHEN("The max encoded length is calculated") {
            const uint32_t encodedLen = _cobsEncodedMaxLength(bufLen);

            THEN("The encoded length is correct") {
                const uint32_t expectedLen = 3;
                CHECK(encodedLen == expectedLen);
            }
        }
    }
}
}

#endif // !NOTE_C_LOW_MEM
