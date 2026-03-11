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

    // The following tests verify the fix for the ceil(u/254) vs floor(u/254)+1
    // bug. At exact multiples of 254, the encoder emits an extra code byte
    // that ceil() misses. Example: 254 non-zero bytes encode as [0xFF, data
    // x254, 0x01] -- that's 2 code bytes, not ceil(254/254)=1.

    GIVEN("A buffer length of exactly 253 bytes (just under one full COBS block)") {
        const uint32_t bufLen = 253;

        WHEN("The max encoded length is calculated") {
            const uint32_t encodedLen = _cobsEncodedMaxLength(bufLen);

            THEN("The encoded length is 253 + 1 code byte + 1 EOP = 255") {
                CHECK(encodedLen == 255);
            }
        }
    }

    GIVEN("A buffer length of exactly 254 bytes (one full COBS block)") {
        // BUG FIX: Old code used ceil(254/254)=1 code byte, but the encoder
        // actually emits 2: one 0xFF at the start and one 0x01 at the end.
        const uint32_t bufLen = 254;

        WHEN("The max encoded length is calculated") {
            const uint32_t encodedLen = _cobsEncodedMaxLength(bufLen);

            THEN("The encoded length is 254 + 2 code bytes + 1 EOP = 257") {
                CHECK(encodedLen == 257);
            }
        }
    }

    GIVEN("A buffer length of exactly 508 bytes (two full COBS blocks)") {
        // BUG FIX: Old code used ceil(508/254)=2 code bytes, but the encoder
        // emits 3: one per 254-byte block plus one final.
        const uint32_t bufLen = 508;

        WHEN("The max encoded length is calculated") {
            const uint32_t encodedLen = _cobsEncodedMaxLength(bufLen);

            THEN("The encoded length is 508 + 3 code bytes + 1 EOP = 512") {
                CHECK(encodedLen == 512);
            }
        }
    }

    GIVEN("A buffer length of 255 bytes (one byte past a full COBS block)") {
        const uint32_t bufLen = 255;

        WHEN("The max encoded length is calculated") {
            const uint32_t encodedLen = _cobsEncodedMaxLength(bufLen);

            THEN("The encoded length is 255 + 2 code bytes + 1 EOP = 258") {
                CHECK(encodedLen == 258);
            }
        }
    }
}

SCENARIO("_cobsEncodedMaxLength round-trip with _cobsGuaranteedFit")
{
    GIVEN("Various buffer sizes") {
        // For any buffer size B, encoding cobsGuaranteedFit(B) bytes must
        // produce at most B bytes (including the EOP byte).
        uint32_t testSizes[] = {3, 8, 255, 256, 257, 510, 511, 512, 65536, 65796, 262144};

        WHEN("_cobsGuaranteedFit is computed and then _cobsEncodedMaxLength is applied") {
            THEN("The encoded max length never exceeds the original buffer") {
                for (size_t i = 0; i < sizeof(testSizes)/sizeof(testSizes[0]); i++) {
                    uint32_t bufLen = testSizes[i];
                    uint32_t maxUnencoded = _cobsGuaranteedFit(bufLen);
                    uint32_t encodedLen = _cobsEncodedMaxLength(maxUnencoded);
                    CHECK(encodedLen <= bufLen);
                }
            }
        }
    }
}
}

#endif // !NOTE_C_LOW_MEM
