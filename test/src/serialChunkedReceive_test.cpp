/*!
 * @file serialChunkedReceive_test.cpp
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
#include "time_mocks.h"

#include "n_lib.h"

DEFINE_FFF_GLOBALS
FAKE_VALUE_FUNC(bool, noteSerialAvailable)
FAKE_VALUE_FUNC(char, noteSerialReceive)
FAKE_VALUE_FUNC(long unsigned int, NoteGetMs)
FAKE_VOID_FUNC(NoteDelayMs, uint32_t)

namespace
{

// Fill the serial receive buffer with size - 1 a's, then a newline.
char *populateRecvBuf(size_t size)
{
    char *buf = (char *)malloc(size);
    memset(buf, 'a', size - 1);
    buf[size - 1] = '\n';
    SET_RETURN_SEQ(noteSerialReceive, buf, size);

    return buf;
}

SCENARIO("serialChunkedReceive")
{
    NoteSetFnDefault(malloc, free, NULL, NULL);

    NoteGetMs_fake.custom_fake = []() -> long unsigned int {
        static long unsigned int count = 0;

        // increment by 1 second
        count += 750;
        // return count pre-increment
        return count - 750;
    };
    uint8_t buf[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00};
    uint32_t size = sizeof(buf);
    bool delay = false;
    const size_t timeoutMs = 3000;
    // 37 is not significant. serialChunkedReceive will return either a 1 or 0
    // in the "available" parameter. 37 is simply "not 1 or 0" -- that means we
    // can validate that available is changed to the correct value where
    // appropriate.
    uint32_t available = 37;

    GIVEN("noteSerialAvailable indicates nothing is available to read") {
        noteSerialAvailable_fake.return_val = false;

        WHEN("serialChunkedReceive is called") {
            const char *err = serialChunkedReceive(buf, &size, delay, timeoutMs,
                                                   &available);

            THEN("An error is returned") {
                CHECK(err != NULL);

                AND_THEN("That error contains the substring {io}") {
                    CHECK(strstr(err, "{io}") != NULL);
                }
            }

            THEN("The output buffer size is 0") {
                CHECK(size == 0);
            }
        }
    }

    GIVEN("The Notecard has 1 fewer bytes than the size of the output buffer") {
        noteSerialAvailable_fake.return_val = true;
        char *recvBuf = populateRecvBuf(sizeof(buf) - 1);

        WHEN("serialChunkedReceive is called") {
            const char *err = serialChunkedReceive(buf, &size, delay, timeoutMs,
                                                   &available);

            THEN("No error is returned") {
                CHECK(err == NULL);
            }

            THEN("The returned size is exactly the number of bytes received "
                 "with noteSerialReceive") {
                CHECK(size == sizeof(buf) - 1);
            }

            THEN("The output buffer has exactly the bytes returned by "
                 "noteSerialReceive") {
                CHECK(memcmp(buf, recvBuf, size) == 0);
            }

            THEN("The returned available bytes is zero") {
                CHECK(available == 0);
            }
        }

        free(recvBuf);
    }

    GIVEN("The Notecard has exactly the same number of bytes as the output "
          "buffer") {
        noteSerialAvailable_fake.return_val = true;
        char *recvBuf = populateRecvBuf(sizeof(buf));

        WHEN("serialChunkedReceive is called") {
            const char *err = serialChunkedReceive(buf, &size, delay, timeoutMs,
                                                   &available);

            THEN("No error is returned") {
                CHECK(err == NULL);
            }

            THEN("The returned size is exactly the number of bytes received "
                 "with noteSerialReceive") {
                CHECK(size == sizeof(buf));
            }

            THEN("The output buffer has exactly the bytes returned by "
                 "noteSerialReceive") {
                CHECK(memcmp(buf, recvBuf, size) == 0);
            }

            THEN("The returned available bytes is zero") {
                CHECK(available == 0);
            }
        }

        free(recvBuf);
    }

    GIVEN("The Notecard has 3 more bytes than the size of the output buffer") {
        noteSerialAvailable_fake.return_val = true;
        char *recvBuf = populateRecvBuf(sizeof(buf) + 3);

        WHEN("serialChunkedReceive is called") {
            const char *err = serialChunkedReceive(buf, &size, delay, timeoutMs,
                                                   &available);

            THEN("No error is returned") {
                CHECK(err == NULL);
            }

            THEN("The returned size is exactly the number of bytes received "
                 "with noteSerialReceive") {
                CHECK(size == sizeof(buf));
            }

            THEN("The output buffer has exactly the bytes returned by "
                 "noteSerialReceive") {
                CHECK(memcmp(buf, recvBuf, size) == 0);
            }

            THEN("The returned \"available\" bytes is 1") {
                CHECK(available == 1);
            }
        }

        free(recvBuf);
    }

    GIVEN("A 0-length output buffer is provided") {
        WHEN("serialChunkedReceive is called") {
            uint32_t originalAvailable = available;
            uint32_t zeroSize = 0;
            const char *err = serialChunkedReceive(buf, &zeroSize, delay,
                                                   timeoutMs, &available);

            THEN("No error is returned") {
                CHECK(err == NULL);
            }

            THEN("Size is 0") {
                CHECK(zeroSize == 0);
            }

            THEN("Available is unchanged") {
                CHECK(available == originalAvailable);
            }
        }
    }

    GIVEN("No \\n indicating end of packet is received and a timeout occurs") {
        bool serialAvailableReturnVals[4] = {0};
        size_t numAvailableBytes = 3;
        memset(serialAvailableReturnVals, true, numAvailableBytes);
        size_t numAvailRetVals = sizeof(serialAvailableReturnVals) /
                                 sizeof(*serialAvailableReturnVals);
        SET_RETURN_SEQ(noteSerialAvailable, serialAvailableReturnVals,
                       numAvailRetVals);
        char *recvBuf = populateRecvBuf(sizeof(buf));

        WHEN("serialChunkedReceive is called") {
            const char *err = serialChunkedReceive(buf, &size, delay, timeoutMs,
                                                   &available);

            THEN("An error is returned") {
                CHECK(err != NULL);

                AND_THEN("That error contains the substring {io}") {
                    CHECK(strstr(err, "{io}") != NULL);
                }
            }

            THEN("The returned size is the size of the partial packet") {
                CHECK(size == numAvailableBytes);

                AND_THEN("The output buffer contains the bytes comprising the "
                         "partial packet") {
                    CHECK(memcmp(buf, recvBuf, size) == 0);
                }
            }
        }

        free(recvBuf);
    }

    GIVEN("Bytes are intermittently unavailable to read but the full packet "
          "eventually comes in") {
        bool serialAvailableReturnVals[] = {false, false, false, true, true, false, true, true};
        size_t numAvailRetVals = sizeof(serialAvailableReturnVals) /
                                 sizeof(*serialAvailableReturnVals);
        size_t numAvailableBytes = 4;
        SET_RETURN_SEQ(noteSerialAvailable, serialAvailableReturnVals,
                       numAvailRetVals);
        char *recvBuf = populateRecvBuf(numAvailableBytes);

        WHEN("serialChunkedReceive is called") {
            const char *err = serialChunkedReceive(buf, &size, delay, timeoutMs,
                                                   &available);

            THEN("No error is returned") {
                CHECK(err == NULL);
            }

            THEN("The returned size is exactly the number of bytes received "
                 "with noteSerialReceive") {
                CHECK(size == numAvailableBytes);
            }

            THEN("The output buffer has exactly the bytes returned by "
                 "noteSerialReceive") {
                CHECK(memcmp(buf, recvBuf, size) == 0);
            }

            THEN("The returned available bytes is 0") {
                CHECK(available == 0);
            }
        }

        free(recvBuf);
    }

    GIVEN("The delay parameter is false") {
        noteSerialAvailable_fake.return_val = false;

        WHEN("serialChunkedReceive is called") {
            serialChunkedReceive(buf, &size, false, timeoutMs, &available);

            THEN("NoteDelayMs is never called while waiting for bytes to become"
                 " available to read") {
                CHECK(NoteDelayMs_fake.call_count == 0);
            }
        }
    }

    GIVEN("The delay parameter is true") {
        noteSerialAvailable_fake.return_val = false;

        WHEN("serialChunkedReceive is called") {
            serialChunkedReceive(buf, &size, true, timeoutMs, &available);

            THEN("NoteDelayMs is called while waiting for bytes to become"
                 " available to read") {
                CHECK(NoteDelayMs_fake.call_count > 0);
            }
        }
    }

    RESET_FAKE(noteSerialAvailable);
    RESET_FAKE(noteSerialReceive);
    RESET_FAKE(NoteGetMs);
    RESET_FAKE(NoteDelayMs);
}

}

#endif // NOTE_C_TEST
