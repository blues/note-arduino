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

    // The following tests verify the fix for the ceil(u/254) vs floor(u/254)+1
    // bug. At bufLen = 255k+1, the old formula overestimated by 1, promising a
    // fit that would actually overflow the buffer after COBS encoding.

    GIVEN("A buffer length of 256 bytes (255*1 + 1, boundary case)") {
        // BUG FIX: Old code returned 254. But encoding 254 non-zero bytes
        // produces 254 + 2 code bytes = 256, which with +1 EOP = 257 > 256.
        // Correct answer: 253 (encodes to 253 + 1 code + 1 EOP = 255 <= 256).
        const uint32_t bufLen = 256;

        WHEN("The max unencoded length is calculated") {
            const uint32_t unencodedLen = _cobsGuaranteedFit(bufLen);

            THEN("The unencoded length is 253, not 254") {
                CHECK(unencodedLen == 253);
            }
        }
    }

    GIVEN("A buffer length of 511 bytes (255*2 + 1, boundary case)") {
        // BUG FIX: Old code returned 508. But encoding 508 non-zero bytes
        // produces 508 + 3 code bytes = 511, plus 1 EOP = 512 > 511.
        // Correct answer: 507.
        const uint32_t bufLen = 511;

        WHEN("The max unencoded length is calculated") {
            const uint32_t unencodedLen = _cobsGuaranteedFit(bufLen);

            THEN("The unencoded length is 507, not 508") {
                CHECK(unencodedLen == 507);
            }
        }
    }

    GIVEN("A buffer length of 766 bytes (255*3 + 1, boundary case)") {
        const uint32_t bufLen = 766;

        WHEN("The max unencoded length is calculated") {
            const uint32_t unencodedLen = _cobsGuaranteedFit(bufLen);

            THEN("The unencoded length is 761") {
                CHECK(unencodedLen == 761);
            }
        }
    }

    GIVEN("A buffer length of 257 bytes (non-boundary, one past 256)") {
        const uint32_t bufLen = 257;

        WHEN("The max unencoded length is calculated") {
            const uint32_t unencodedLen = _cobsGuaranteedFit(bufLen);

            THEN("The unencoded length is 254") {
                CHECK(unencodedLen == 254);
            }
        }
    }

    GIVEN("A buffer length of 262144 bytes (customer-reported capacity)") {
        const uint32_t bufLen = 262144;

        WHEN("The max unencoded length is calculated") {
            const uint32_t unencodedLen = _cobsGuaranteedFit(bufLen);

            THEN("The unencoded length is 261114") {
                CHECK(unencodedLen == 261114);
            }
        }
    }
}

SCENARIO("_cobsGuaranteedFit round-trip with _cobsEncodedMaxLength")
{
    GIVEN("Buffer sizes at every 255k+1 boundary from 1 to 2551") {
        WHEN("_cobsGuaranteedFit is computed and then _cobsEncodedMaxLength is applied") {
            THEN("The encoded max length never exceeds the original buffer") {
                for (uint32_t k = 0; k <= 10; k++) {
                    uint32_t bufLen = 255 * k + 1;
                    if (bufLen <= 2) continue;
                    uint32_t maxUnencoded = _cobsGuaranteedFit(bufLen);
                    uint32_t encodedLen = _cobsEncodedMaxLength(maxUnencoded);
                    CHECK(encodedLen <= bufLen);
                }
            }
        }
    }

    GIVEN("Buffer sizes at every 255k boundary from 255 to 2550") {
        WHEN("The result is checked against _cobsEncodedMaxLength") {
            THEN("The encoded max length never exceeds the original buffer") {
                for (uint32_t k = 1; k <= 10; k++) {
                    uint32_t bufLen = 255 * k;
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
