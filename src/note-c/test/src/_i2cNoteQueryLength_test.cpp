/*!
 * @file _i2cNoteQueryLength_test.cpp
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
#include <fff.h>

#include "n_lib.h"
#include "time_mocks.h"

DEFINE_FFF_GLOBALS
FAKE_VALUE_FUNC(const char *, _noteI2CReceive, uint16_t, uint8_t *, uint16_t,
                uint32_t *)
FAKE_VALUE_FUNC(uint32_t, NoteGetMs)

namespace
{

// Something's immediately available.
// Nothing available at the start, then something becomes available.

const char * _noteI2CReceiveNotAvailable(uint16_t, uint8_t *, uint16_t,
        uint32_t *available)
{
    *available = 0;

    return NULL;
}

enum {
    NUM_BYTES_AVAILABLE = 3
};

const char * _noteI2CReceiveAvailable(uint16_t, uint8_t *, uint16_t,
                                      uint32_t *available)
{
    *available = NUM_BYTES_AVAILABLE;

    return NULL;
}

SCENARIO("_i2cNoteQueryLength")
{
    NoteGetMs_fake.custom_fake = NoteGetMsIncrement;
    uint32_t available = 0;
    uint32_t timeoutMs = 5000;

    GIVEN("_noteI2CReceive returns an error") {
        _noteI2CReceive_fake.return_val = "some error";

        WHEN("_i2cNoteQueryLength is called") {
            const char *err = _i2cNoteQueryLength(&available, timeoutMs);

            THEN("An error is returned") {
                CHECK(err != NULL);
            }
        }
    }

    GIVEN("Bytes are never available to read") {
        _noteI2CReceive_fake.custom_fake = _noteI2CReceiveNotAvailable;

        WHEN("_i2cNoteQueryLength is called") {
            const char *err = _i2cNoteQueryLength(&available, timeoutMs);

            THEN("An error is returned") {
                REQUIRE(err != NULL);

                AND_THEN("That error contains the substring {io}") {
                    CHECK(strstr(err, "{io}") != NULL);
                }
            }
        }
    }

    GIVEN("Bytes are immediately available to read") {
        _noteI2CReceive_fake.custom_fake = _noteI2CReceiveAvailable;

        WHEN("_i2cNoteQueryLength is called") {
            const char *err = _i2cNoteQueryLength(&available, timeoutMs);

            THEN("No error is returned") {
                CHECK(err == NULL);
            }

            THEN("The available out parameter reports the same number of "
                 "available bytes as _noteI2CReceive reported") {
                CHECK(available == NUM_BYTES_AVAILABLE);
            }
        }
    }

    GIVEN("Bytes are immediately available to read") {
        _noteI2CReceive_fake.custom_fake = _noteI2CReceiveAvailable;

        WHEN("_i2cNoteQueryLength is called") {
            const char *err = _i2cNoteQueryLength(&available, timeoutMs);

            THEN("No error is returned") {
                CHECK(err == NULL);
            }

            THEN("The available out parameter reports the same number of "
                 "available bytes as _noteI2CReceive reported") {
                CHECK(available == NUM_BYTES_AVAILABLE);
            }
        }
    }

    GIVEN("Bytes aren't available to read at first, but then become "
          "available") {
        const char * (*customI2CReciveFakes[])
        (uint16_t, uint8_t *, uint16_t, uint32_t *available) = {
            _noteI2CReceiveNotAvailable,
            _noteI2CReceiveNotAvailable,
            _noteI2CReceiveAvailable
        };
        SET_CUSTOM_FAKE_SEQ(_noteI2CReceive, customI2CReciveFakes, 3);

        WHEN("_i2cNoteQueryLength is called") {
            const char *err = _i2cNoteQueryLength(&available, timeoutMs);

            THEN("No error is returned") {
                CHECK(err == NULL);
            }

            THEN("The available out parameter reports the same number of "
                 "available bytes as _noteI2CReceive reported") {
                CHECK(available == NUM_BYTES_AVAILABLE);
            }
        }
    }

    RESET_FAKE(_noteI2CReceive);
    RESET_FAKE(NoteGetMs);
}

}


