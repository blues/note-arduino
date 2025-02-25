/*!
 * @file _i2cChunkedReceive_test.cpp
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
#include "test_static.h"
#include "time_mocks.h"

DEFINE_FFF_GLOBALS
FAKE_VALUE_FUNC(const char *, _noteI2CReceive, uint16_t, uint8_t *, uint16_t,
                uint32_t *)
FAKE_VALUE_FUNC(uint32_t, NoteGetMs)
FAKE_VOID_FUNC(NoteDelayMs, uint32_t)

namespace
{

const char *_noteI2CReceiveInfinite(uint16_t, uint8_t *buf, uint16_t size,
                                    uint32_t *available)
{
    memset(buf, 'a', size);
    *available = NOTE_I2C_MAX_DEFAULT;

    return NULL;
}

SCENARIO("_i2cChunkedReceive")
{
    NoteGetMs_fake.custom_fake = NoteGetMsIncrement;

    uint32_t available = 0;
    uint32_t timeoutMs = 5000;

    GIVEN("0 is specified for the output buffer length") {
        uint8_t buf[] = {0xAB};
        uint32_t zeroSize = 0;

        AND_GIVEN("_noteI2CReceive reports that here are bytes available from "
                  "the Notecard") {
            _noteI2CReceive_fake.custom_fake = _noteI2CReceiveInfinite;

            WHEN("_i2cChunkedReceive is called") {
                uint32_t originalAvailable = available;
                const char *err = _i2cChunkedReceive(buf, &zeroSize, true,
                                                     timeoutMs, &available);

                THEN("No error is returned") {
                    CHECK(err == NULL);
                }

                THEN("Size is 0") {
                    CHECK(zeroSize == 0);
                }

                THEN("available is exactly the number of bytes reported "
                     "available by _noteI2CReceive") {
                    CHECK(available == NOTE_I2C_MAX_DEFAULT);
                }
            }
        }
    }

    GIVEN("_noteI2CReceive returns an error") {
        uint8_t buf[] = {0xAB};
        uint32_t size = sizeof(buf);
        _noteI2CReceive_fake.return_val = "some error";

        WHEN("_i2cChunkedReceive is called") {
            const char *err = _i2cChunkedReceive(buf, &size, true, timeoutMs,
                                                 &available);

            THEN("An error is returned") {
                CHECK(err != NULL);
            }
        }
    }

    GIVEN("The output buffer is too small") {
        uint8_t buf[NOTE_I2C_MAX_DEFAULT] = {0};
        uint32_t size = sizeof(buf);
        _noteI2CReceive_fake.custom_fake = _noteI2CReceiveInfinite;

        WHEN("_i2cChunkedReceive is called") {
            const char *err = _i2cChunkedReceive(buf, &size, true, timeoutMs,
                                                 &available);

            THEN("No error is returned") {
                CHECK(err == NULL);
            }

            THEN("The output size is exactly the number of bytes returned by "
                 "_noteI2CReceive") {
                CHECK(size == NOTE_I2C_MAX_DEFAULT);
            }

            THEN("available is exactly the number of bytes reported available "
                 "by _noteI2CReceive") {
                CHECK(available == NOTE_I2C_MAX_DEFAULT);
            }

            THEN("The output buffer contains exactly the bytes returned by "
                 "_noteI2CReceive") {
                uint8_t expectedBuf[sizeof(buf)];
                memset(expectedBuf, 'a', sizeof(expectedBuf));

                CHECK(memcmp(buf, expectedBuf, sizeof(expectedBuf)) == 0);
            }
        }
    }

    GIVEN("The output buffer is sufficiently large") {
        uint8_t buf[NOTE_I2C_MAX_DEFAULT * 3] = {0};
        uint32_t size = sizeof(buf);

        _noteI2CReceive_fake.custom_fake = [](uint16_t, uint8_t *buf,
        uint16_t size, uint32_t *available) -> const char* {
            // If _noteI2CReceive is called with size 0, the caller is querying
            // the Notecard for how many bytes are available. Here, we report
            // back that there are NOTE_I2C_MAX_DEFAULT * 2 bytes available.
            if (size == 0)
            {
                *available = NOTE_I2C_MAX_DEFAULT * 2;
            }
            // If the user has requested all the available bytes, return a
            // sequence of a's with a newline at the end.
            else if (*available == size)
            {
                memset(buf, 'a', size - 1);
                buf[size - 1] = '\n';
                *available = 0;
            }
            // If the user has requested less than the available bytes, just
            // return as many a's as they requested, and deduct that amount
            // from what's available.
            else
            {
                memset(buf, 'a', size);
                *available -= size;
            }

            return NULL;
        };

        // TODO: See if this and the AND_GIVEN below can be refactored to reduce
        // repetition.
        AND_GIVEN("available is non-zero to start") {
            size_t numBytesExpected = 10;
            available = 10;

            WHEN("_i2cChunkedReceive is called") {
                const char *err = _i2cChunkedReceive(buf, &size, true, timeoutMs,
                                                     &available);

                THEN("No error is returned") {
                    CHECK(err == NULL);
                }

                THEN("The output size is exactly the number of bytes returned"
                     "by _noteI2CReceive") {
                    CHECK(size == numBytesExpected);
                }

                THEN("available is 0") {
                    CHECK(available == 0);
                }

                THEN("The output buffer contains exactly the bytes returned by "
                     "_noteI2CReceive") {
                    uint8_t expectedBuf[10];
                    memset(expectedBuf, 'a', sizeof(expectedBuf) - 1);
                    expectedBuf[sizeof(expectedBuf) - 1] = '\n';

                    CHECK(memcmp(buf, expectedBuf, sizeof(expectedBuf)) == 0);
                }
            }
        }

        AND_GIVEN("available is zero to start") {
            size_t numBytesExpected = NOTE_I2C_MAX_DEFAULT * 2;
            available = 0;

            WHEN("_i2cChunkedReceive is called") {
                const char *err = _i2cChunkedReceive(buf, &size, true, timeoutMs,
                                                     &available);

                THEN("No error is returned") {
                    CHECK(err == NULL);
                }

                THEN("The output size is exactly the number of bytes returned"
                     "by _noteI2CReceive") {
                    CHECK(size == numBytesExpected);
                }

                THEN("available is 0") {
                    CHECK(available == 0);
                }

                THEN("The output buffer contains exactly the bytes returned by "
                     "_noteI2CReceive") {
                    uint8_t expectedBuf[NOTE_I2C_MAX_DEFAULT * 2];
                    memset(expectedBuf, 'a', sizeof(expectedBuf) - 1);
                    expectedBuf[sizeof(expectedBuf) - 1] = '\n';

                    CHECK(memcmp(buf, expectedBuf, sizeof(expectedBuf)) == 0);
                }
            }
        }
    }

    GIVEN("End-of-packet (\\n) is received, but _noteI2CReceive indicates more "
          "is still available to read") {
        uint8_t buf[NOTE_I2C_MAX_DEFAULT * 3] = {0};
        uint32_t size = sizeof(buf);
        size_t numBytesExpected = NOTE_I2C_MAX_DEFAULT * 2;

        // On the first call, _noteI2CReceive reports back that
        // NOTE_I2C_MAX_DEFAULT are available to read.
        auto bytesAvailable = [](uint16_t, uint8_t *buf, uint16_t size,
        uint32_t *available) -> const char* {
            *available = NOTE_I2C_MAX_DEFAULT;

            return NULL;
        };
        // On the next call, NOTE_I2C_MAX_DEFAULT-1 a's are written to the
        // output buffer followed by a newline. We report back that
        // NOTE_I2C_MAX_DEFAULT bytes are still available to read.
        auto fullPacketButMoreAvailable = [](uint16_t, uint8_t *buf,
        uint16_t size, uint32_t *available) -> const char* {
            *available = NOTE_I2C_MAX_DEFAULT;
            memset(buf, 'a', size - 1);
            buf[size - 1] = '\n';

            return NULL;
        };
        // On the final call, NOTE_I2C_MAX_DEFAULT a's are written to the
        // output buffer, and we report back that 0 bytes are available to read.
        auto excessData = [](uint16_t, uint8_t *buf, uint16_t size,
        uint32_t *available) -> const char* {
            memset(buf, 'a', size);
            *available -= size;

            return NULL;
        };
        const char *(*recvFakeSequence[])(uint16_t, uint8_t *, uint16_t,
                                          uint32_t *) = {
            bytesAvailable,
            fullPacketButMoreAvailable,
            excessData
        };
        SET_CUSTOM_FAKE_SEQ(_noteI2CReceive, recvFakeSequence, 3);

        WHEN("_i2cChunkedReceive is called") {
            const char *err = _i2cChunkedReceive(buf, &size, true, timeoutMs,
                                                 &available);

            THEN("No error is returned") {
                CHECK(err == NULL);
            }

            THEN("The output size is exactly the number of bytes returned by "
                 "_noteI2CReceive") {
                CHECK(size == numBytesExpected);
            }

            THEN("available is 0") {
                CHECK(available == 0);
            }

            THEN("The output buffer contains exactly the bytes returned by "
                 "_noteI2CReceive") {
                uint8_t expectedBuf[NOTE_I2C_MAX_DEFAULT * 2];
                memset(expectedBuf, 'a', NOTE_I2C_MAX_DEFAULT - 1);
                expectedBuf[NOTE_I2C_MAX_DEFAULT - 1] = '\n';
                memset(expectedBuf + NOTE_I2C_MAX_DEFAULT, 'a',
                       NOTE_I2C_MAX_DEFAULT);

                CHECK(memcmp(buf, expectedBuf, sizeof(expectedBuf)) == 0);
            }
        }
    }

    GIVEN("There's nothing to read from the Notecard") {
        _noteI2CReceive_fake.custom_fake = [](uint16_t, uint8_t *buf,
        uint16_t size, uint32_t *available) -> const char* {
            *available = 0;

            return NULL;
        };
        uint8_t buf[NOTE_I2C_MAX_DEFAULT] = {0};
        uint32_t size = sizeof(buf);

        WHEN("_i2cChunkedReceive is called") {
            const char *err = _i2cChunkedReceive(buf, &size, true, timeoutMs,
                                                 &available);

            THEN("An error is returned") {
                CHECK(err != NULL);

                AND_THEN("That error contains the substring {io}") {
                    CHECK(strstr(err, "{io}") != NULL);
                }
            }

            THEN("Size is 0") {
                CHECK(size == 0);
            }

            THEN("Available is 0") {
                CHECK(available == 0);
            }
        }

        AND_GIVEN("The delay parameter is false") {
            WHEN("_i2cChunkedReceive is called") {
                _i2cChunkedReceive(buf, &size, false, timeoutMs, &available);

                THEN("NoteDelayMs is never called") {
                    CHECK(NoteDelayMs_fake.call_count == 0);
                }
            }
        }

        AND_GIVEN("The delay parameter is true") {
            WHEN("_i2cChunkedReceive is called") {
                _i2cChunkedReceive(buf, &size, true, timeoutMs, &available);

                THEN("NoteDelayMs is called") {
                    CHECK(NoteDelayMs_fake.call_count > 0);
                }
            }
        }
    }

    GIVEN("There's initially data to read from the Notecard, but then there's "
          "nothing available and we never receive the \\n") {
        // First, _noteI2CReceive will report that NOTE_I2C_MAX_DEFAULT bytes are
        // available to read.
        auto bytesAvailable = [](uint16_t, uint8_t *buf, uint16_t size,
        uint32_t *available) -> const char* {
            *available = NOTE_I2C_MAX_DEFAULT;

            return NULL;
        };
        // On the next call, those bytes will be written (without a trailing
        // newline), and available will be set to 0 to indicate there's nothing
        // left to read.
        auto partialPacket = [](uint16_t, uint8_t *buf, uint16_t size,
        uint32_t *available) -> const char* {
            memset(buf, 'a', size);
            *available = 0;

            return NULL;
        };
        // Finally, any subsequent calls will just set available to 0 and do
        // nothing else. This will eventually lead to a timeout in
        // _i2cChunkedReceive because it's still waiting for the newline.
        auto nothingAvailable = [](uint16_t, uint8_t *buf, uint16_t size,
        uint32_t *available) -> const char* {
            *available = 0;

            return NULL;
        };
        const char *(*recvFakeSequence[])(uint16_t, uint8_t *, uint16_t,
                                          uint32_t *) = {
            bytesAvailable,
            partialPacket,
            nothingAvailable
        };
        SET_CUSTOM_FAKE_SEQ(_noteI2CReceive, recvFakeSequence, 3);

        uint8_t buf[NOTE_I2C_MAX_DEFAULT] = {0};
        uint32_t size = sizeof(buf);

        WHEN("_i2cChunkedReceive is called") {
            const char *err = _i2cChunkedReceive(buf, &size, true, timeoutMs,
                                                 &available);

            THEN("An error is returned") {
                CHECK(err != NULL);

                AND_THEN("That error contains the substring {io}") {
                    CHECK(strstr(err, "{io}") != NULL);
                }
            }

            THEN("Size is the size of the incomplete packet") {
                CHECK(size == NOTE_I2C_MAX_DEFAULT);
            }

            THEN("Available is 0") {
                CHECK(available == 0);
            }
        }
    }

    RESET_FAKE(_noteI2CReceive);
    RESET_FAKE(NoteGetMs);
    RESET_FAKE(NoteDelayMs);
}

}


