/*!
 * @file i2cChunkedTransmit_test.cpp
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

#ifdef NOTE_C_TEST

#include <catch2/catch_test_macros.hpp>
#include "fff.h"

#include "n_lib.h"
#include "test_static.h"

DEFINE_FFF_GLOBALS
FAKE_VALUE_FUNC(const char *, NoteI2CTransmit, uint16_t, uint8_t *, uint16_t)
FAKE_VALUE_FUNC(bool, NoteI2CReset, uint16_t)
FAKE_VOID_FUNC(NoteDelayMs, uint32_t)

namespace
{

uint8_t transmitBuf[CARD_REQUEST_I2C_SEGMENT_MAX_LEN * 2];
size_t transmitBufIdx = 0;

const char *NoteI2CTransmitCapture(uint16_t, uint8_t *buf, uint16_t size)
{
    memcpy(transmitBuf + transmitBufIdx, buf, size);

    return NULL;
}

SCENARIO("i2cChunkedTransmit")
{
    bool delay = true;
    transmitBufIdx = 0;

    GIVEN("0 is specified for the input buffer length") {
        uint8_t buf[] = {0xAB};
        size_t size = 0;

        WHEN("i2cChunkedTransmit is called") {
            const char *err = i2cChunkedTransmit(buf, size, delay);

            THEN("Nothing is transmitted") {
                CHECK(NoteI2CTransmit_fake.call_count == 0);
            }

            THEN("No error is returned") {
                CHECK(err == NULL);
            }
        }
    }

    GIVEN("NoteI2CTransmit returns an error") {
        uint8_t buf[] = {0xAB};
        size_t size = sizeof(buf);
        NoteI2CTransmit_fake.return_val = "some error";

        WHEN("i2cChunkedTransmit is called") {
            const char *err = i2cChunkedTransmit(buf, size, delay);

            THEN("An error is returned") {
                CHECK(err != NULL);
            }

            THEN("NoteI2CReset is called") {
                CHECK(NoteI2CReset_fake.call_count > 0);
            }
        }
    }

    GIVEN("A valid, non-zero length buffer to transmit") {
        uint8_t *buf = NULL;
        size_t size = 0;
        const char *err = NULL;
        NoteI2CTransmit_fake.custom_fake = NoteI2CTransmitCapture;

        AND_GIVEN("The input buffer can be sent in one chunk") {
            uint8_t oneChunkBuf[NOTE_I2C_MAX_DEFAULT];
            buf = oneChunkBuf;
            size = sizeof(oneChunkBuf);
            memset(oneChunkBuf, 1, size);

            WHEN("i2cChunkedTransmit is called") {
                err = i2cChunkedTransmit(buf, size, delay);

                THEN("The data is sent in 1 call to NoteI2CTransmit") {
                    CHECK(NoteI2CTransmit_fake.call_count == 1);
                }
            }
        }

        AND_GIVEN("The input buffer doesn't fit into a single chunk") {
            uint8_t multiChunkBuf[NOTE_I2C_MAX_DEFAULT * 2];
            buf = multiChunkBuf;
            size = sizeof(multiChunkBuf);
            memset(multiChunkBuf, 2, size);

            WHEN("i2cChunkedTransmit is called") {
                err = i2cChunkedTransmit(buf, size, delay);

                THEN("The data is sent in > 1 call to NoteI2CTransmit") {
                    CHECK(NoteI2CTransmit_fake.call_count > 1);
                }
            }
        }

        AND_GIVEN("The input buffer is larger than the maximum I2C segment "
                  "length") {
            uint8_t greaterThanSegmentBuf[CARD_REQUEST_I2C_SEGMENT_MAX_LEN + 1];
            buf = greaterThanSegmentBuf;
            size = sizeof(greaterThanSegmentBuf);
            memset(greaterThanSegmentBuf, 3, size);

            WHEN("i2cChunkedTransmit is called") {
                err = i2cChunkedTransmit(buf, size, delay);
            }
        }

        THEN("No error is returned") {
            CHECK(err == NULL);
        }

        THEN("The data passed to NoteI2CTransmit is exactly the same as "
             "what was passed to i2cChunkedTransmit") {
            memcmp(buf, transmitBuf, sizeof(buf) == 0);
        }
    }

    RESET_FAKE(NoteI2CTransmit);
    RESET_FAKE(NoteI2CReset);
    RESET_FAKE(NoteDelayMs);
}

}

#endif // NOTE_C_TEST
