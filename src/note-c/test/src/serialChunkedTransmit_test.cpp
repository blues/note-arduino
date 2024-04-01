/*!
 * @file serialChunkedTransmit_test.cpp
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
#include "fff.h"
#include "time_mocks.h"

#include "n_lib.h"

DEFINE_FFF_GLOBALS
FAKE_VOID_FUNC(noteSerialTransmit, uint8_t *, size_t, bool)
FAKE_VOID_FUNC(NoteDelayMs, uint32_t)

namespace
{

// This buffer will hold the bytes passed to noteSerialTransmit. We can then
// check that the bytes passed to serialChunkedTransmit are passed to
// noteSerialTransmit without alteration.
uint8_t accumulatedBuf[CARD_REQUEST_SERIAL_SEGMENT_MAX_LEN * 2];
size_t accumulatedIdx = 0;

void noteSerialTransmitAccumulate(uint8_t *buf, size_t size, bool)
{
    memcpy(accumulatedBuf + accumulatedIdx, buf, size);
    accumulatedIdx += size;
}

SCENARIO("serialChunkedTransmit")
{
    memset(accumulatedBuf, 0, sizeof(accumulatedBuf));
    accumulatedIdx = 0;

    GIVEN("A 0-length transmit buffer") {
        WHEN("serialChunkedTransmit is called") {
            uint8_t buf[] = {0x01};
            const char *err = serialChunkedTransmit(buf, 0, true);

            THEN("No error is returned") {
                CHECK(err == NULL);
            }
        }
    }

    GIVEN("A buffer with size less than the maximum serial segment length") {
        noteSerialTransmit_fake.custom_fake = noteSerialTransmitAccumulate;
        uint8_t buf[CARD_REQUEST_SERIAL_SEGMENT_MAX_LEN - 1];
        size_t size = sizeof(buf);
        memset(buf, 1, size);

        WHEN("serialChunkedTransmit is called") {
            const char *err = serialChunkedTransmit(buf, size, true);

            THEN("No error is returned") {
                CHECK(err == NULL);
            }

            THEN("The buffer is passed verbatim to noteSerialTransmit") {
                CHECK(memcmp(buf, accumulatedBuf, size) == 0);
            }

            THEN("noteSerialTransmit is only called once, since the buffer fits"
                 " in a single segment") {
                CHECK(noteSerialTransmit_fake.call_count == 1);
            }
        }
    }

    // TODO: Come up with a way to reduce duplication here.
    GIVEN("A buffer with size equal to the maximum serial segment length") {
        noteSerialTransmit_fake.custom_fake = noteSerialTransmitAccumulate;
        uint8_t buf[CARD_REQUEST_SERIAL_SEGMENT_MAX_LEN];
        size_t size = sizeof(buf);
        memset(buf, 1, size);

        WHEN("serialChunkedTransmit is called") {
            const char *err = serialChunkedTransmit(buf, size, true);

            THEN("No error is returned") {
                CHECK(err == NULL);
            }

            THEN("The buffer is passed verbatim to noteSerialTransmit") {
                CHECK(memcmp(buf, accumulatedBuf, size) == 0);
            }

            THEN("noteSerialTransmit is only called once, since the buffer fits"
                 " in a single segment") {
                CHECK(noteSerialTransmit_fake.call_count == 1);
            }
        }
    }

    GIVEN("A buffer with size greater than the maximum serial segment length") {
        noteSerialTransmit_fake.custom_fake = noteSerialTransmitAccumulate;
        uint8_t buf[CARD_REQUEST_SERIAL_SEGMENT_MAX_LEN + 10];
        size_t size = sizeof(buf);
        memset(buf, 1, size);

        WHEN("serialChunkedTransmit is called") {
            const char *err = serialChunkedTransmit(buf, size, true);

            THEN("No error is returned") {
                CHECK(err == NULL);
            }

            THEN("The buffer is passed verbatim to noteSerialTransmit") {
                CHECK(memcmp(buf, accumulatedBuf, size) == 0);
            }

            THEN("noteSerialTransmit is called more than once, since the buffer"
                 " doesn't fit in a single segment") {
                CHECK(noteSerialTransmit_fake.call_count > 1);
            }
        }

        AND_GIVEN("The delay parameter is false") {
            WHEN("serialChunkedTransmit is called") {
                const char *err = serialChunkedTransmit(buf, size, false);

                THEN("NoteDelayMs is never called") {
                    CHECK(NoteDelayMs_fake.call_count == 0);
                }
            }
        }

        AND_GIVEN("The delay parameter is true") {
            WHEN("serialChunkedTransmit is called") {
                const char *err = serialChunkedTransmit(buf, size, true);

                THEN("NoteDelayMs is called") {
                    CHECK(NoteDelayMs_fake.call_count > 0);
                }
            }
        }
    }

    RESET_FAKE(noteSerialTransmit);
    RESET_FAKE(NoteDelayMs);
}

}


