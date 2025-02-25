/*!
 * @file _serialNoteTransaction_test.cpp
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

#include "n_lib.h"

DEFINE_FFF_GLOBALS
FAKE_VALUE_FUNC(void *, NoteMalloc, size_t)
FAKE_VALUE_FUNC(bool, _noteSerialAvailable)
FAKE_VALUE_FUNC(char, _noteSerialReceive)
FAKE_VALUE_FUNC(uint32_t, NoteGetMs)
FAKE_VOID_FUNC(_noteSerialTransmit, uint8_t *, size_t, bool)
FAKE_VALUE_FUNC(const char *, _serialChunkedTransmit, uint8_t *, uint32_t, bool);
FAKE_VALUE_FUNC(const char *, _serialChunkedReceive, uint8_t *, uint32_t *, bool, uint32_t, uint32_t *)

namespace
{

char transmitBuf[CARD_REQUEST_SERIAL_SEGMENT_MAX_LEN * 2];
size_t transmitBufLen = 0;
bool resetTransmitBufLen = false;

void _noteSerialTransmitAppend(uint8_t *buf, size_t len, bool)
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

const char *_serialChunkedTransmitAppend(uint8_t *buf, uint32_t len, bool)
{
    _noteSerialTransmitAppend(buf, len, true);

    return NULL;
}

const char *_serialChunkedReceiveNothing(uint8_t *, uint32_t *size, bool,
        uint32_t, uint32_t *available)
{
    *size = 0;
    *available = 0;

    return NULL;
}

const char *_serialChunkedReceiveOneAndDone(uint8_t *buf, uint32_t *size, bool,
        uint32_t, uint32_t *available)
{
    *buf = '\n';
    *size = 1;
    *available = 0;

    return NULL;
}

#define SERIAL_CHUNKED_RECEIVE_MULTIPLE_SIZE (2 * ALLOC_CHUNK)

size_t _serialChunkedReceiveMultipleLeft = SERIAL_CHUNKED_RECEIVE_MULTIPLE_SIZE;

const char *_serialChunkedReceiveMultiple(uint8_t *buf, uint32_t *size, bool,
        uint32_t, uint32_t *available)
{
    memset(buf, 1, *size);
    _serialChunkedReceiveMultipleLeft -= *size;
    *available = _serialChunkedReceiveMultipleLeft;

    return NULL;
}

void *MallocNull(size_t)
{
    return NULL;
}

SCENARIO("_serialNoteTransaction")
{
    NoteSetFnDefault(NULL, free, NULL, NULL);

    char req[] = "{\"req\": \"note.add\"}\n";
    const uint32_t timeoutMs = CARD_INTER_TRANSACTION_TIMEOUT_SEC;

    GIVEN("A valid JSON request C-string and a NULL response pointer") {
        _noteSerialTransmit_fake.custom_fake = _noteSerialTransmitAppend;
        _serialChunkedTransmit_fake.custom_fake = _serialChunkedTransmitAppend;

        WHEN("_serialNoteTransaction is called") {
            const char *err = _serialNoteTransaction(req, strlen(req), NULL, timeoutMs);

            THEN("_serialNoteTransaction returns NULL (no error)") {
                CHECK(err == NULL);
            }

            THEN("The request is given to the transmission function followed by"
                 " \\r\\n") {
                size_t rawReqLen = (strlen(req) - 1);
                CHECK(!memcmp(transmitBuf, req, rawReqLen));
                CHECK(!memcmp(transmitBuf + rawReqLen, c_newline, c_newline_len));
            }

            THEN("_noteSerialReceive is not called") {
                CHECK(_noteSerialReceive_fake.call_count == 0);
            }
        }

        AND_GIVEN("_serialChunkedTransmit returns an error") {
            _serialChunkedTransmit_fake.custom_fake = NULL;
            _serialChunkedTransmit_fake.return_val = "some error";

            WHEN("_serialNoteTransaction is called") {
                const char *err = _serialNoteTransaction(req, strlen(req), NULL, timeoutMs);

                THEN("_serialNoteTransaction returns an error") {
                    CHECK(err != NULL);
                }
            }
        }
    }

    GIVEN("A valid JSON request C-string and a valid response pointer") {
        char* rsp = NULL;

        AND_GIVEN("Allocating a buffer to hold the response fails") {
            NoteMalloc_fake.return_val = NULL;
            _noteSerialAvailable_fake.return_val = true;

            WHEN("_serialNoteTransaction is called") {
                const char *err = _serialNoteTransaction(req, strlen(req), &rsp, timeoutMs);

                THEN("NoteMalloc is called") {
                    REQUIRE(NoteMalloc_fake.call_count > 0);
                }

                THEN("_serialNoteTransaction returns an error") {
                    CHECK(err != NULL);
                }

                THEN("_noteSerialReceive is not called (no bytes received from "
                     "the Notecard)") {
                    CHECK(_noteSerialReceive_fake.call_count == 0);
                }

                THEN("The response pointer is unchanged") {
                    CHECK(rsp == NULL);
                }
            }
        }

        AND_GIVEN("_noteSerialAvailable never indicates bytes are available from"
                  " the Notecard") {
            _noteSerialAvailable_fake.return_val = false;
            NoteMalloc_fake.custom_fake = malloc;
            uint32_t getMsReturnVals[3];

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

                WHEN("_serialNoteTransaction is called") {
                    const char* err = _serialNoteTransaction(req, strlen(req), &rsp, timeoutMs);

                    THEN("_noteSerialReceive is not called (no bytes received "
                         "from the Notecard)") {
                        CHECK(_noteSerialReceive_fake.call_count == 0);
                    }

                    THEN("_serialNoteTransaction returns an error") {
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

        AND_GIVEN("_serialChunkedReceive returns an error") {
            _noteSerialAvailable_fake.return_val = true;
            NoteMalloc_fake.custom_fake = malloc;
            _serialChunkedReceive_fake.return_val = "some error";

            WHEN("_serialNoteTransaction is called") {
                const char *err = _serialNoteTransaction(req, strlen(req), &rsp, timeoutMs);

                THEN("_serialNoteTransaction returns an error") {
                    CHECK(err != NULL);
                }

                THEN("The response pointer is unchanged") {
                    CHECK(rsp == NULL);
                }
            }
        }

        AND_GIVEN("_serialChunkedReceive indicates there's nothing to receive") {
            _noteSerialAvailable_fake.return_val = true;
            NoteMalloc_fake.custom_fake = malloc;
            _serialChunkedReceive_fake.custom_fake = _serialChunkedReceiveNothing;

            WHEN("_serialNoteTransaction is called") {
                const char *err = _serialNoteTransaction(req, strlen(req), &rsp, timeoutMs);

                THEN("_serialNoteTransaction returns NULL (no error)") {
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
                  "_serialChunkedReceive") {
            _noteSerialAvailable_fake.return_val = true;
            NoteMalloc_fake.custom_fake = malloc;
            _serialChunkedReceive_fake.custom_fake =
                _serialChunkedReceiveOneAndDone;

            WHEN("_serialNoteTransaction is called") {
                const char *err = _serialNoteTransaction(req, strlen(req), &rsp, timeoutMs);

                THEN("_serialNoteTransaction returns NULL (no error)") {
                    CHECK(err == NULL);
                }

                THEN("_serialChunkedReceive is called exactly once") {
                    CHECK(_serialChunkedReceive_fake.call_count == 1);
                }

                THEN("The response pointer is not NULL") {
                    CHECK(rsp != NULL);

                    AND_THEN("The response is exactly what was received by "
                             "_serialChunkedReceive") {
                        CHECK(strcmp(rsp, "\n") == 0);
                    }
                }
            }

            free(rsp);
        }

        AND_GIVEN("The response is too big for one call to "
                  "_serialChunkedReceive") {
            _noteSerialAvailable_fake.return_val = true;
            NoteMalloc_fake.custom_fake = malloc;
            _serialChunkedReceive_fake.custom_fake =
                _serialChunkedReceiveMultiple;
            // TODO: Explain what we're doing here.
            _serialChunkedReceiveMultipleLeft =
                SERIAL_CHUNKED_RECEIVE_MULTIPLE_SIZE;

            WHEN("_serialNoteTransaction is called") {
                const char *err = _serialNoteTransaction(req, strlen(req), &rsp, timeoutMs);

                THEN("_serialNoteTransaction returns NULL (no error)") {
                    CHECK(err == NULL);
                }

                THEN("_serialChunkedReceive is called more than once") {
                    CHECK(_serialChunkedReceive_fake.call_count > 1);
                }

                THEN("The response pointer is not NULL") {
                    CHECK(rsp != NULL);

                    char expectedRsp[SERIAL_CHUNKED_RECEIVE_MULTIPLE_SIZE + 1];
                    memset(expectedRsp, 1,
                           SERIAL_CHUNKED_RECEIVE_MULTIPLE_SIZE);
                    expectedRsp[SERIAL_CHUNKED_RECEIVE_MULTIPLE_SIZE] = '\0';
                    AND_THEN("The response is exactly what was received by "
                             "_serialChunkedReceive") {
                        CHECK(strcmp(rsp, expectedRsp) == 0);
                    }
                }

                free(rsp);
            }

            AND_GIVEN("Growing the response buffer fails") {
                void *(*mallocFns[])(size_t) = {malloc, MallocNull};
                SET_CUSTOM_FAKE_SEQ(NoteMalloc, mallocFns, 2);

                WHEN("_serialNoteTransaction is called") {
                    const char *err = _serialNoteTransaction(req, strlen(req), &rsp, timeoutMs);

                    THEN("_serialNoteTransaction returns an error") {
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
    RESET_FAKE(_noteSerialAvailable);
    RESET_FAKE(_noteSerialTransmit);
    RESET_FAKE(_noteSerialReceive);
    RESET_FAKE(NoteGetMs);
    RESET_FAKE(_serialChunkedTransmit);
    RESET_FAKE(_serialChunkedReceive);
}

}


