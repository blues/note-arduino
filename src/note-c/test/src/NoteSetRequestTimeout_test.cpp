/*!
 * @file NoteSetRequestTimeout_test.cpp
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

#include <catch2/catch_test_macros.hpp>
#include <fff.h>

#include "n_lib.h"

DEFINE_FFF_GLOBALS

namespace
{

static const int default_timeout_sec = CARD_INTER_TRANSACTION_TIMEOUT_SEC;

SCENARIO("NoteSetRequestTimeout")
{
    GIVEN("The default timeout is in use") {
        cardTransactionTimeoutOverrideSecs = 0;

        WHEN("NoteSetRequestTimeout is called with a new timeout value") {
            const uint32_t newTimeout = 60;
            const uint32_t result = NoteSetRequestTimeout(newTimeout);

            THEN("The previous value (30 seconds default) is returned") {
                CHECK(result == default_timeout_sec);
            }

            THEN("The new timeout value is set") {
                CHECK(CARD_INTER_TRANSACTION_TIMEOUT_SEC == newTimeout);
            }
        }

        WHEN("NoteSetRequestTimeout is called with zero to restore default") {
            const uint32_t result = NoteSetRequestTimeout(0);

            THEN("The previous override value is returned") {
                CHECK(result == default_timeout_sec);  // Returns the previous effective value
            }

            THEN("The timeout is reset to use the default") {
                CHECK(CARD_INTER_TRANSACTION_TIMEOUT_SEC == default_timeout_sec);
            }
        }
    }

    GIVEN("A custom timeout is already set") {
        const uint32_t customTimeout = 120;
        cardTransactionTimeoutOverrideSecs = customTimeout;

        WHEN("NoteSetRequestTimeout is called with a different timeout value") {
            const uint32_t newTimeout = 90;
            const uint32_t result = NoteSetRequestTimeout(newTimeout);

            THEN("The previous value (the custom timeout) is returned") {
                CHECK(result == customTimeout);
            }

            THEN("The new timeout value is set") {
                CHECK(CARD_INTER_TRANSACTION_TIMEOUT_SEC == newTimeout);
            }
        }

        WHEN("NoteSetRequestTimeout is called with zero to restore default") {
            const uint32_t result = NoteSetRequestTimeout(0);

            THEN("The previous value (the custom timeout) is returned") {
                CHECK(result == customTimeout);
            }

            THEN("The timeout is reset to use the default") {
                CHECK(CARD_INTER_TRANSACTION_TIMEOUT_SEC == default_timeout_sec);
            }
        }
    }

    SECTION("Edge cases") {
        WHEN("NoteSetRequestTimeout is called with maximum uint32_t value") {
            const uint32_t maxTimeout = UINT32_MAX;
            const uint32_t result = NoteSetRequestTimeout(maxTimeout);

            THEN("The previous value (30 seconds default) is returned") {
                CHECK(result == default_timeout_sec);
            }

            THEN("The function handles the maximum value correctly") {
                CHECK(CARD_INTER_TRANSACTION_TIMEOUT_SEC == maxTimeout);
            }
        }

        WHEN("NoteSetRequestTimeout is called with 1 second") {
            const uint32_t minTimeout = 1;
            const uint32_t result = NoteSetRequestTimeout(minTimeout);

            THEN("The previous value (30 seconds default) is returned") {
                CHECK(result == default_timeout_sec);
            }

            THEN("The function handles the minimum non-zero value correctly") {
                CHECK(CARD_INTER_TRANSACTION_TIMEOUT_SEC == minTimeout);
            }
        }
    }

    // Restore original timeout value for other tests
    cardTransactionTimeoutOverrideSecs = default_timeout_sec;
}

}
