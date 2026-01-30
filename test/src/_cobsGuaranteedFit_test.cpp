/*!
 * @file _cobsGuaranteedFit_test.cpp
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

SCENARIO("_cobsGuaranteedFit")
{
    GIVEN("A buffer length less than the COBS max packet size of 254 bytes") {
        const uint32_t bufLen = 8;

        WHEN("The max unencoded length is calculated") {
            const uint32_t unencodedLen = _cobsGuaranteedFit(bufLen);

            THEN("The unencoded length is correct") {
                const uint32_t expectedLen = 6;
                CHECK(unencodedLen == expectedLen);
            }
        }
    }

    GIVEN("A buffer length greater than the COBS max packet size of 254 bytes") {
        // https://discuss.blues.com/t/cobs-off-by-one-error-with-encoding-decoding-lengths/3435
        const uint32_t bufLen = 65796;

        WHEN("The max unencoded length is calculated") {
            const uint32_t unencodedLen = _cobsGuaranteedFit(bufLen);

            THEN("The unencoded length is correct") {
                const uint32_t expectedLen = 65536;
                CHECK(unencodedLen == expectedLen);
            }
        }
    }

    GIVEN("A buffer length of 0 bytes") {
        const uint32_t bufLen = 0;

        WHEN("The max unencoded length is calculated") {
            const uint32_t unencodedLen = _cobsGuaranteedFit(bufLen);

            THEN("The unencoded length is correct") {
                const uint32_t expectedLen = 0;
                CHECK(unencodedLen == expectedLen);
            }
        }
    }

    GIVEN("A buffer length of 1 byte") {
        const uint32_t bufLen = 1;

        WHEN("The max unencoded length is calculated") {
            const uint32_t unencodedLen = _cobsGuaranteedFit(bufLen);

            THEN("The unencoded length is correct") {
                const uint32_t expectedLen = 0;
                CHECK(unencodedLen == expectedLen);
            }
        }
    }

    GIVEN("A buffer length of 2 bytes") {
        const uint32_t bufLen = 2;

        WHEN("The max unencoded length is calculated") {
            const uint32_t unencodedLen = _cobsGuaranteedFit(bufLen);

            THEN("The unencoded length is correct") {
                const uint32_t expectedLen = 0;
                CHECK(unencodedLen == expectedLen);
            }
        }
    }

    GIVEN("A buffer length of 3 bytes") {
        const uint32_t bufLen = 3;

        WHEN("The max unencoded length is calculated") {
            const uint32_t unencodedLen = _cobsGuaranteedFit(bufLen);

            THEN("The unencoded length is correct") {
                const uint32_t expectedLen = 1;
                CHECK(unencodedLen == expectedLen);
            }
        }
    }
}
}

#endif // !NOTE_C_LOW_MEM
