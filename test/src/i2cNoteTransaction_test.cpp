/*!
 * @file i2cNoteTransaction_test.cpp
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

#ifdef TEST

#include <catch2/catch_test_macros.hpp>
#include "fff.h"

#include "n_lib.h"
#include "i2c_mocks.h"

DEFINE_FFF_GLOBALS
FAKE_VALUE_FUNC(void *, NoteMalloc, size_t)
FAKE_VALUE_FUNC(const char *, NoteI2CTransmit, uint16_t, uint8_t *, uint16_t)
FAKE_VALUE_FUNC(const char *, NoteI2CReceive, uint16_t, uint8_t *, uint16_t,
                uint32_t *)
FAKE_VALUE_FUNC(long unsigned int, NoteGetMs)
FAKE_VOID_FUNC(NoteLockI2C)
FAKE_VOID_FUNC(NoteUnlockI2C)

namespace
{

#define I2C_MULTI_CHUNK_RECV_BYTES (ALLOC_CHUNK * 2)

TEST_CASE("i2cNoteTransaction")
{
    NoteSetFnDefault(NULL, free, NULL, NULL);

    RESET_FAKE(NoteMalloc);
    RESET_FAKE(NoteI2CTransmit);
    RESET_FAKE(NoteI2CReceive);
    RESET_FAKE(NoteGetMs);
    RESET_FAKE(NoteLockI2C);
    RESET_FAKE(NoteUnlockI2C);

    char noteAddReq[] = "{\"req\": \"note.add\"}";

    SECTION("Transmit buffer allocation fails") {
        NoteMalloc_fake.return_val = NULL;

        REQUIRE(i2cNoteTransaction(noteAddReq, NULL) != NULL);
        REQUIRE(NoteMalloc_fake.call_count == 1);
    }

    SECTION("No response expected") {
        NoteMalloc_fake.custom_fake = malloc;
        NoteI2CTransmit_fake.return_val = NULL;
        char *request = NULL;
        uint32_t reqLen;

        SECTION("NoteI2CTransmit fails") {
            NoteI2CTransmit_fake.return_val = "an error";

            REQUIRE(i2cNoteTransaction(noteAddReq, NULL) != NULL);
            REQUIRE(NoteI2CTransmit_fake.call_count == 1);
        }

        SECTION("One transmission") {
            reqLen = NoteI2CMax() - 1;
            request = (char*)malloc(reqLen);
            REQUIRE(request != NULL);
            memset(request, 1, reqLen - 1);
            request[reqLen - 1] = '\0';
            REQUIRE(i2cNoteTransaction(request, NULL) == NULL);
            // The request length is less than NoteI2CMax(), so it should all be
            // sent in one call to NoteI2CTransmit.
            REQUIRE(NoteI2CTransmit_fake.call_count == 1);
        }

        SECTION("Multiple transmissions") {
            reqLen = NoteI2CMax() + 1;
            request = (char*)malloc(reqLen);
            REQUIRE(request != NULL);
            memset(request, 1, reqLen - 1);
            request[reqLen - 1] = '\0';
            REQUIRE(i2cNoteTransaction(request, NULL) == NULL);
            // The request is 1 byte greater than NoteI2CMax(), so it should
            // require two calls to NoteI2CTransmit.
            REQUIRE(NoteI2CTransmit_fake.call_count == 2);
        }

        // We should have locked and unlocked the i2c bus exactly once.
        REQUIRE(NoteLockI2C_fake.call_count == 1);
        REQUIRE(NoteUnlockI2C_fake.call_count == 1);
        REQUIRE(NoteI2CReceive_fake.call_count == 0);

        free(request);
    }

    SECTION("Response expected") {
        char* resp = NULL;

        SECTION("Response buffer allocation fails") {
            uint8_t *transmitBuf = (uint8_t *)malloc(strlen(noteAddReq) + 1);
            REQUIRE(transmitBuf != NULL);
            void* mallocReturnVals[2] = {transmitBuf, NULL};
            SET_RETURN_SEQ(NoteMalloc, mallocReturnVals, 2);

            REQUIRE(i2cNoteTransaction(noteAddReq, &resp) != NULL);
            REQUIRE(NoteI2CTransmit_fake.call_count == 1);
            REQUIRE(NoteI2CReceive_fake.call_count == 0);
            REQUIRE(NoteMalloc_fake.call_count == 2);
        }

        SECTION("NoteI2CReceive fails") {
            NoteMalloc_fake.custom_fake = malloc;
            NoteI2CReceive_fake.return_val = "an error";

            REQUIRE(i2cNoteTransaction(noteAddReq, &resp) != NULL);
            REQUIRE(NoteI2CTransmit_fake.call_count == 1);
            REQUIRE(NoteI2CReceive_fake.call_count == 1);
            REQUIRE(NoteMalloc_fake.call_count == 2);
        }

        SECTION("Force timeout") {
            NoteMalloc_fake.custom_fake = malloc;
            NoteI2CReceive_fake.custom_fake = NoteI2CReceiveNothing;
            long unsigned int getMsReturnVals[2] = {
                0, NOTECARD_TRANSACTION_TIMEOUT_SEC * 1000 + 1
            };
            SET_RETURN_SEQ(NoteGetMs, getMsReturnVals, 2);

            REQUIRE(i2cNoteTransaction(noteAddReq, &resp) != NULL);
            REQUIRE(NoteI2CTransmit_fake.call_count == 1);
            REQUIRE(NoteI2CReceive_fake.call_count == 1);
            REQUIRE(NoteMalloc_fake.call_count == 2);
        }

        SECTION("Check response") {
            SECTION("One receipt") {
                NoteMalloc_fake.custom_fake = malloc;
                NoteI2CReceive_fake.custom_fake = NoteI2CReceiveOne;

                REQUIRE(i2cNoteTransaction(noteAddReq, &resp) == NULL);

                REQUIRE(NoteI2CReceive_fake.call_count == 2);
                REQUIRE(NoteMalloc_fake.call_count == 2);
            }

            SECTION("Multiple chunks") {
                NoteMalloc_fake.custom_fake = malloc;
                NoteI2CReceive_fake.custom_fake = NoteI2CReceiveMultiChunk;

                REQUIRE(i2cNoteTransaction(noteAddReq, &resp) == NULL);

                // 1 call to get available data to read, plus however many I2C
                // transactions it takes to read that data.
                const uint32_t numRecvCalls = 1 + (I2C_MULTI_CHUNK_RECV_BYTES +
                                                   (NoteI2CMax() - 1)) / NoteI2CMax();
                REQUIRE(NoteI2CReceive_fake.call_count == numRecvCalls);
                // 1 malloc for rx buffer, 1 for tx buffer, 1 to grow tx buffer.
                REQUIRE(NoteMalloc_fake.call_count == 3);
            }

            // The response should be all 1s followed by a newline.
            size_t respSz = strlen(resp);
            for (size_t i = 0; i < respSz; ++i) {
                if (i != respSz - 1) {
                    REQUIRE(resp[i] == 1);
                } else {
                    REQUIRE(resp[i] == '\n');
                }
            }
        }

        REQUIRE(NoteLockI2C_fake.call_count == 1);
        REQUIRE(NoteUnlockI2C_fake.call_count == 1);

        free(resp);
    }
}

}

#endif // TEST
