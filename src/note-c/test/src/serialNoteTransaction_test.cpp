/*!
 * @file serialNoteTransaction_test.cpp
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

DEFINE_FFF_GLOBALS
FAKE_VALUE_FUNC(void *, NoteMalloc, size_t)
FAKE_VALUE_FUNC(bool, NoteSerialAvailable)
FAKE_VALUE_FUNC(char, NoteSerialReceive)
FAKE_VALUE_FUNC(long unsigned int, NoteGetMs)
FAKE_VOID_FUNC(NoteSerialTransmit, uint8_t *, size_t, bool)
FAKE_VALUE_FUNC(const char *, serialChunkedTransmit, uint8_t *, uint32_t, bool);
FAKE_VALUE_FUNC(const char *, serialChunkedReceive, uint8_t *, uint32_t *, bool, size_t, uint32_t *)

namespace
{

char transmitBuf[CARD_REQUEST_SERIAL_SEGMENT_MAX_LEN * 2];
size_t transmitBufLen = 0;
bool resetTransmitBufLen = false;

void NoteSerialTransmitAppend(uint8_t *buf, size_t len, bool)
{
    if (resetTransmitBufLen) {
        transmitBufLen = 0;
        resetTransmitBufLen = false;
    }

    if (buf[len - 1] == '\n') {
        resetTransmitBufLen = true;
    }

    if (transmitBufLen + len > sizeof(transmitBuf)) {
        return;
    }

    memcpy(transmitBuf + transmitBufLen, buf, len);
    transmitBufLen += len;
}

const char *serialChunkedTransmitAppend(uint8_t *buf, uint32_t len, bool)
{
    NoteSerialTransmitAppend(buf, len, true);

    return NULL;
}

const char *serialChunkedReceiveNothing(uint8_t *, uint32_t *size, bool, size_t,
                                        uint32_t *available)
{
    *size = 0;
    *available = 0;

    return NULL;
}

const char *serialChunkedReceiveOneAndDone(uint8_t *buf, uint32_t *size, bool,
        size_t, uint32_t *available)
{
    *buf = '\n';
    *size = 1;
    *available = 0;

    return NULL;
}

#define SERIAL_CHUNKED_RECEIVE_MULTIPLE_SIZE (2 * ALLOC_CHUNK)

size_t serialChunkedReceiveMultipleLeft = SERIAL_CHUNKED_RECEIVE_MULTIPLE_SIZE;

const char *serialChunkedReceiveMultiple(uint8_t *buf, uint32_t *size, bool,
        size_t, uint32_t *available)
{
    memset(buf, 1, *size);
    serialChunkedReceiveMultipleLeft -= *size;
    *available = serialChunkedReceiveMultipleLeft;

    return NULL;
}

void *MallocNull(size_t)
{
    return NULL;
}

SCENARIO("serialNoteTransaction")
{
    NoteSetFnDefault(NULL, free, NULL, NULL);

    char req[] = "{\"req\": \"note.add\"}";
    const size_t timeoutMs = CARD_INTER_TRANSACTION_TIMEOUT_SEC;

    GIVEN("A valid JSON request C-string and a NULL response pointer") {
        size_t reqLen = strlen(req);
        NoteSerialTransmit_fake.custom_fake = NoteSerialTransmitAppend;
        serialChunkedTransmit_fake.custom_fake = serialChunkedTransmitAppend;

        WHEN("serialNoteTransaction is called") {
            const char *err = serialNoteTransaction(req, NULL, timeoutMs);

            THEN("serialNoteTransaction returns NULL (no error)") {
                CHECK(err == NULL);
            }

            THEN("The request is given to the transmission function followed by"
                 " \\r\\n") {
                CHECK(!memcmp(transmitBuf, req, reqLen - 2));
                CHECK(!memcmp(transmitBuf + reqLen, c_newline, c_newline_len));
            }

            THEN("NoteSerialReceive is not called") {
                CHECK(NoteSerialReceive_fake.call_count == 0);
            }
        }

        AND_GIVEN("serialChunkedTransmit returns an error") {
            serialChunkedTransmit_fake.custom_fake = NULL;
            serialChunkedTransmit_fake.return_val = "some error";

            WHEN("serialNoteTransaction is called") {
                const char *err = serialNoteTransaction(req, NULL, timeoutMs);

                THEN("serialNoteTransaction returns an error") {
                    CHECK(err != NULL);
                }
            }
        }
    }

    GIVEN("A valid JSON request C-string and a valid response pointer") {
        char* rsp = NULL;

        AND_GIVEN("Allocating a buffer to hold the response fails") {
            NoteMalloc_fake.return_val = NULL;
            NoteSerialAvailable_fake.return_val = true;

            WHEN("serialNoteTransaction is called") {
                const char *err = serialNoteTransaction(req, &rsp, timeoutMs);

                THEN("NoteMalloc is called") {
                    REQUIRE(NoteMalloc_fake.call_count > 0);
                }

                THEN("serialNoteTransaction returns an error") {
                    CHECK(err != NULL);
                }

                THEN("NoteSerialReceive is not called (no bytes received from "
                     "the Notecard)") {
                    CHECK(NoteSerialReceive_fake.call_count == 0);
                }

                THEN("The response pointer is unchanged") {
                    CHECK(rsp == NULL);
                }
            }
        }

        AND_GIVEN("NoteSerialAvailable never indicates bytes are available from"
                  " the Notecard") {
            NoteSerialAvailable_fake.return_val = false;
            NoteMalloc_fake.custom_fake = malloc;
            long unsigned int getMsReturnVals[3];

            AND_GIVEN("There's a timeout after waiting for "
                      "CARD_INTER_TRANSACTION_TIMEOUT_SEC seconds") {
                getMsReturnVals[0] = UINT32_MAX -
                                     CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000;
                getMsReturnVals[1] = UINT32_MAX -
                                     (CARD_INTER_TRANSACTION_TIMEOUT_SEC - 1) *
                                     1000;
                // Force the millisecond counter, which is used to determine if
                // we've timed out, to roll over to cover that edge case.
                getMsReturnVals[2] = 0;

                SET_RETURN_SEQ(NoteGetMs, getMsReturnVals, 3);

                WHEN("serialNoteTransaction is called") {
                    const char* err = serialNoteTransaction(req, &rsp, timeoutMs);

                    THEN("NoteSerialReceive is not called (no bytes received "
                         "from the Notecard)") {
                        CHECK(NoteSerialReceive_fake.call_count == 0);
                    }

                    THEN("serialNoteTransaction returns an error") {
                        REQUIRE(err != NULL);
                    }

                    THEN("The error message indicates there was a timeout") {
                        CHECK(strstr(err, "timeout") != NULL);
                    }

                    THEN("The response pointer is unchanged") {
                        CHECK(rsp == NULL);
                    }
                }
            }
        }

        AND_GIVEN("serialChunkedReceive returns an error") {
            NoteSerialAvailable_fake.return_val = true;
            NoteMalloc_fake.custom_fake = malloc;
            serialChunkedReceive_fake.return_val = "some error";

            WHEN("serialNoteTransaction is called") {
                const char *err = serialNoteTransaction(req, &rsp, timeoutMs);

                THEN("serialNoteTransaction returns an error") {
                    CHECK(err != NULL);
                }

                THEN("The response pointer is unchanged") {
                    CHECK(rsp == NULL);
                }
            }
        }

        AND_GIVEN("serialChunkedReceive indicates there's nothing to receive") {
            NoteSerialAvailable_fake.return_val = true;
            NoteMalloc_fake.custom_fake = malloc;
            serialChunkedReceive_fake.custom_fake = serialChunkedReceiveNothing;

            WHEN("serialNoteTransaction is called") {
                const char *err = serialNoteTransaction(req, &rsp, timeoutMs);

                THEN("serialNoteTransaction returns NULL (no error)") {
                    CHECK(err == NULL);
                }

                THEN("The response pointer is not NULL") {
                    CHECK(rsp != NULL);

                    AND_THEN("The response is an empty string") {
                        CHECK(strlen(rsp) == 0);
                    }
                }
            }

            free(rsp);
        }

        AND_GIVEN("The response is small enough for one call to "
                  "serialChunkedReceive") {
            NoteSerialAvailable_fake.return_val = true;
            NoteMalloc_fake.custom_fake = malloc;
            serialChunkedReceive_fake.custom_fake =
                serialChunkedReceiveOneAndDone;

            WHEN("serialNoteTransaction is called") {
                const char *err = serialNoteTransaction(req, &rsp, timeoutMs);

                THEN("serialNoteTransaction returns NULL (no error)") {
                    CHECK(err == NULL);
                }

                THEN("serialChunkedReceive is called exactly once") {
                    CHECK(serialChunkedReceive_fake.call_count == 1);
                }

                THEN("The response pointer is not NULL") {
                    CHECK(rsp != NULL);

                    AND_THEN("The response is exactly what was received by "
                             "serialChunkedReceive") {
                        CHECK(strcmp(rsp, "\n") == 0);
                    }
                }
            }

            free(rsp);
        }

        AND_GIVEN("The response is too big for one call to "
                  "serialChunkedReceive") {
            NoteSerialAvailable_fake.return_val = true;
            NoteMalloc_fake.custom_fake = malloc;
            serialChunkedReceive_fake.custom_fake =
                serialChunkedReceiveMultiple;
            // TODO: Explain what we're doing here.
            serialChunkedReceiveMultipleLeft =
                SERIAL_CHUNKED_RECEIVE_MULTIPLE_SIZE;

            WHEN("serialNoteTransaction is called") {
                const char *err = serialNoteTransaction(req, &rsp, timeoutMs);

                THEN("serialNoteTransaction returns NULL (no error)") {
                    CHECK(err == NULL);
                }

                THEN("serialChunkedReceive is called more than once") {
                    CHECK(serialChunkedReceive_fake.call_count > 1);
                }

                THEN("The response pointer is not NULL") {
                    CHECK(rsp != NULL);

                    char expectedRsp[SERIAL_CHUNKED_RECEIVE_MULTIPLE_SIZE + 1];
                    memset(expectedRsp, 1,
                           SERIAL_CHUNKED_RECEIVE_MULTIPLE_SIZE);
                    expectedRsp[SERIAL_CHUNKED_RECEIVE_MULTIPLE_SIZE] = '\0';
                    AND_THEN("The response is exactly what was received by "
                             "serialChunkedReceive") {
                        CHECK(strcmp(rsp, expectedRsp) == 0);
                    }
                }

                free(rsp);
            }

            AND_GIVEN("Growing the response buffer fails") {
                void *(*mallocFns[])(size_t) = {malloc, MallocNull};
                SET_CUSTOM_FAKE_SEQ(NoteMalloc, mallocFns, 2);

                WHEN("serialNoteTransaction is called") {
                    const char *err = serialNoteTransaction(req, &rsp, timeoutMs);

                    THEN("serialNoteTransaction returns an error") {
                        CHECK(err != NULL);
                    }

                    THEN("The response pointer is unchanged") {
                        CHECK(rsp == NULL);
                    }
                }
            }
        }
    }

    RESET_FAKE(NoteMalloc);
    RESET_FAKE(NoteSerialAvailable);
    RESET_FAKE(NoteSerialTransmit);
    RESET_FAKE(NoteSerialReceive);
    RESET_FAKE(NoteGetMs);
    RESET_FAKE(serialChunkedTransmit);
    RESET_FAKE(serialChunkedReceive);
}

}

#endif // NOTE_C_TEST
