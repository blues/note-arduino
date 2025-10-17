/*!
 * @file _i2cNoteReset_test.cpp
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

DEFINE_FFF_GLOBALS
FAKE_VOID_FUNC(_delayIO)
FAKE_VOID_FUNC(NoteDelayMs, uint32_t)
FAKE_VALUE_FUNC(uint32_t, NoteGetMs)
FAKE_VALUE_FUNC(bool, _noteI2CReset, uint16_t)
FAKE_VALUE_FUNC(const char *, _noteI2CTransmit, uint16_t, uint8_t *, uint16_t)
FAKE_VALUE_FUNC(const char *, _noteI2CReceive, uint16_t, uint8_t *, uint16_t,
                uint32_t *)
FAKE_VOID_FUNC(NoteLockI2C)
FAKE_VOID_FUNC(NoteUnlockI2C)

namespace
{

static uint32_t rtc_ms = 0;

void NoteDelayMs_mock(uint32_t delayMs)
{
    rtc_ms += delayMs;
}

uint32_t NoteGetMs_mock(void)
{
    return rtc_ms++;
}

const char * _noteI2CReceive_CleanResetSeq0(uint16_t, uint8_t *buffer, uint16_t, uint32_t *available)
{
    if (available) {
        *available = 2;
    }
    return nullptr;
}

const char * _noteI2CReceive_CleanResetSeq1(uint16_t, uint8_t *buffer, uint16_t, uint32_t *available)
{
    if (available) {
        *available = 0;
    }
    if (buffer) {
        buffer[0] = '\r';
        buffer[1] = '\n';
    }
    return nullptr;
}

const char * _noteI2CReceive_AvailGTBufferMessageSeq0(uint16_t, uint8_t *buffer, uint16_t, uint32_t *available)
{
    if (available) {
        *available = (ALLOC_CHUNK + 1);
    }
    return nullptr;
}

const char * _noteI2CReceive_AvailGTBufferMessageSeq1(uint16_t, uint8_t *buffer, uint16_t, uint32_t *available)
{
    if (available) {
        *available = 0;
    }
    return nullptr;
}

const char * _noteI2CReceive_AvailGTFFFFMessageSeq0(uint16_t, uint8_t *buffer, uint16_t, uint32_t *available)
{
    if (available) {
        *available = 19790917;
    }
    return nullptr;
}

const char * _noteI2CReceive_AvailGTFFFFMessageSeq1(uint16_t, uint8_t *buffer, uint16_t, uint32_t *available)
{
    if (available) {
        *available = 0;
    }
    return nullptr;
}

const char * _noteI2CReceive_AvailGTMaxMessageSeq0(uint16_t, uint8_t *buffer, uint16_t, uint32_t *available)
{
    if (available) {
        *available = NoteI2CMax();
    }
    return nullptr;
}

const char * _noteI2CReceive_AvailGTMaxMessageSeq1(uint16_t, uint8_t *buffer, uint16_t, uint32_t *available)
{
    if (available) {
        *available = 0;
    }
    return nullptr;
}

const char * _noteI2CReceive_ClearMessageSeq0(uint16_t, uint8_t *buffer, uint16_t, uint32_t *available)
{
    if (available) {
        *available = 4;
    }
    return nullptr;
}

const char * _noteI2CReceive_ClearMessageSeq1(uint16_t, uint8_t *buffer, uint16_t, uint32_t *available)
{
    if (available) {
        *available = 0;
    }
    if (buffer) {
        buffer[0] = '{';
        buffer[1] = '}';
        buffer[2] = '\r';
        buffer[3] = '\n';
    }
    return nullptr;
}

const char * _noteI2CReceive_ClearMessageSeq2(uint16_t, uint8_t *buffer, uint16_t, uint32_t *available)
{
    if (available) {
        *available = 2;
    }
    return nullptr;
}

const char * _noteI2CReceive_ClearMessageSeq3(uint16_t, uint8_t *buffer, uint16_t, uint32_t *available)
{
    if (available) {
        *available = 0;
    }
    if (buffer) {
        buffer[0] = '\r';
        buffer[1] = '\n';
    }
    return nullptr;
}

SCENARIO("_i2cNoteReset")
{
    //TODO: Refactor GIVEN/WHEN/THEN block grouping and names
    WHEN("Invoked") {
        NoteDelayMs_fake.custom_fake = NoteDelayMs_mock;
        NoteGetMs_fake.custom_fake = NoteGetMs_mock;
        _i2cNoteReset();

        THEN("Delay for `CARD_REQUEST_I2C_SEGMENT_DELAY_MS`") {
            CHECK(NoteDelayMs_fake.call_count > 0);
            CHECK(NoteDelayMs_fake.arg0_history[0] == CARD_REQUEST_I2C_SEGMENT_DELAY_MS);
        }

        THEN("The I2C mutex will be taken") {
            CHECK(NoteLockI2C_fake.call_count > 0);
        }

        THEN("`_noteI2CReset()` is called") {
            CHECK(_noteI2CReset_fake.call_count > 0);
        }
        AND_THEN("The first parameter is the Notecard address") {
            CHECK(_noteI2CReset_fake.arg0_history[0] == NoteI2CAddress());
        }
    }

    GIVEN("`_noteI2CReset()` is called") {
        NoteDelayMs_fake.custom_fake = NoteDelayMs_mock;
        NoteGetMs_fake.custom_fake = NoteGetMs_mock;

        WHEN("`_noteI2CReset()` fails") {
            _noteI2CReset_fake.return_val = false;
            const bool success = _i2cNoteReset();

            THEN("The I2C mutex will be released") {
                CHECK(NoteUnlockI2C_fake.call_count == NoteLockI2C_fake.call_count);
            }
            AND_THEN("`_i2cNoteReset()` fails") {
                CHECK(success == false);
            }

            THEN("Exit immediately without retrying") {
                CHECK(_noteI2CReset_fake.call_count == 1);
            }
        }

        WHEN("`_noteI2CReset()` succeeds") {
            _noteI2CReset_fake.return_val = true;
            _i2cNoteReset();

            THEN("Then the I/O delay is respected") {
                CHECK(_delayIO_fake.call_count > 0);
            }
        }
    }

    GIVEN("`_noteI2CReset()` succeeds") {
        NoteDelayMs_fake.custom_fake = NoteDelayMs_mock;
        NoteGetMs_fake.custom_fake = NoteGetMs_mock;
        _noteI2CReset_fake.return_val = true;
        const bool success = _i2cNoteReset();

        THEN("`_noteI2CTransmit()` is called") {
            CHECK(_noteI2CTransmit_fake.call_count > 0);
        }
        THEN("The first parameter is the Notecard address") {
            CHECK(_noteI2CTransmit_fake.arg0_history[0] == NoteI2CAddress());
        }
        THEN("The second parameter is a string containing a single newline") {
            CHECK(_noteI2CTransmit_fake.arg1_history[0][0] == '\n');
        }
        THEN("The third parameter is the string length of 1") {
            CHECK(_noteI2CTransmit_fake.arg2_history[0] == sizeof(char));
        }
    }

    GIVEN("`_noteI2CTransmit()` is called") {
        NoteDelayMs_fake.custom_fake = NoteDelayMs_mock;
        NoteGetMs_fake.custom_fake = NoteGetMs_mock;
        _noteI2CReset_fake.return_val = true;

        WHEN("`_noteI2CTransmit()` fails") {
            _noteI2CTransmit_fake.return_val = "most likely a NACK has occurred";
            const bool success = _i2cNoteReset();

            THEN("Delay for `CARD_REQUEST_I2C_NACK_WAIT_MS` milliseconds") {
                CHECK(NoteDelayMs_fake.call_count > 1);
                CHECK(NoteDelayMs_fake.arg0_history[1] == CARD_REQUEST_I2C_NACK_WAIT_MS);
            }

            THEN("`_noteI2CReceive()` is not called") {
                CHECK(_noteI2CReceive_fake.call_count == 0);
            }

            THEN("`_noteI2CTransmit()` will retry `CARD_RESET_SYNC_RETRIES` times") {
                CHECK(_noteI2CTransmit_fake.call_count == CARD_RESET_SYNC_RETRIES);
            }

            THEN("The I2C mutex will be released") {
                CHECK(NoteUnlockI2C_fake.call_count == NoteLockI2C_fake.call_count);
            }
            AND_THEN("`_i2cNoteReset()` fails") {
                CHECK(success == false);
            }
        }

        WHEN("`_noteI2CTransmit()` succeeds") {
            _noteI2CTransmit_fake.return_val = nullptr;
            const bool success = _i2cNoteReset();

            THEN("Delay for `CARD_REQUEST_I2C_SEGMENT_DELAY_MS`") {
                CHECK(NoteDelayMs_fake.call_count > 1);
                CHECK(NoteDelayMs_fake.arg0_history[1] == CARD_REQUEST_I2C_SEGMENT_DELAY_MS);
            }

            THEN("`_noteI2CReceive()` is called to query the response") {
                CHECK(_noteI2CReceive_fake.call_count > 0);
            }
            AND_THEN("The first parameter is the Notecard address") {
                CHECK(_noteI2CReceive_fake.arg0_history[0] == NoteI2CAddress());
            }
            AND_THEN("The second parameter is a non-NULL static buffer used to drain bytes") {
                CHECK(_noteI2CReceive_fake.arg1_history[0] != nullptr);
            }
            AND_THEN("The third parameter is zero to indicate a query request") {
                CHECK(_noteI2CReceive_fake.arg2_history[0] == 0);
            }
            AND_THEN("The fourth parameter is a non-NULL unsigned integer pointer used for the query response") {
                CHECK(_noteI2CReceive_fake.arg3_history[0] != nullptr);
            }
        }
    }

    GIVEN("`_noteI2CReceive()` is called") {
        NoteDelayMs_fake.custom_fake = NoteDelayMs_mock;
        NoteGetMs_fake.custom_fake = NoteGetMs_mock;
        _noteI2CReset_fake.return_val = true;
        _noteI2CTransmit_fake.return_val = nullptr;

        WHEN("`_noteI2CReceive()` fails") {
            _noteI2CReceive_fake.return_val = "the Notecard ESP commonly generates protocol errors";
            const bool success = _i2cNoteReset();

            THEN("Delay for `CARD_REQUEST_I2C_SEGMENT_DELAY_MS`") {
                CHECK(NoteDelayMs_fake.call_count > 2);
                CHECK(NoteDelayMs_fake.arg0_history[2] == CARD_REQUEST_I2C_SEGMENT_DELAY_MS);
            }
            AND_THEN("Will retry `_noteI2CReceive()`") {
                CHECK(_noteI2CReceive_fake.call_count > CARD_RESET_SYNC_RETRIES);
            }

            THEN("The I2C mutex will be released") {
                CHECK(NoteUnlockI2C_fake.call_count == NoteLockI2C_fake.call_count);
            }
            AND_THEN("`_i2cNoteReset()` fails") {
                CHECK(success == false);
            }
        }
        WHEN("`_noteI2CReceive()` succeeds") {
            const char * (*ClearMessageSeq[])(uint16_t, uint8_t *, uint16_t, uint32_t *) = {
                _noteI2CReceive_ClearMessageSeq0,
                _noteI2CReceive_ClearMessageSeq1,
                _noteI2CReceive_ClearMessageSeq2,
                _noteI2CReceive_ClearMessageSeq3,
            };
            SET_CUSTOM_FAKE_SEQ(_noteI2CReceive, ClearMessageSeq, 4);
            const bool success = _i2cNoteReset();

            THEN("Delay for `CARD_REQUEST_I2C_CHUNK_DELAY_MS`") {
                CHECK(NoteDelayMs_fake.call_count > 2);
                CHECK(NoteDelayMs_fake.arg0_history[2] == CARD_REQUEST_I2C_CHUNK_DELAY_MS);
            }

            THEN("`_noteI2CReceive()` is called to receive the response") {
                CHECK(_noteI2CReceive_fake.call_count > 1);
            }
            AND_THEN("The first parameter is the Notecard address") {
                CHECK(_noteI2CReceive_fake.arg0_val == NoteI2CAddress());
            }
            AND_THEN("The second parameter is a non-NULL static buffer used to drain bytes") {
                CHECK(_noteI2CReceive_fake.arg1_val != nullptr);
            }
            AND_THEN("The third parameter is populated with the available value from the previous query") {
                REQUIRE(_noteI2CReceive_fake.call_count >= 4);
                CHECK(_noteI2CReceive_fake.arg2_history[0] == 0);
                CHECK(_noteI2CReceive_fake.arg2_history[1] == 4);
                CHECK(_noteI2CReceive_fake.arg2_history[2] == 0);
                CHECK(_noteI2CReceive_fake.arg2_history[3] == 2);
            }
            AND_THEN("The fourth parameter is a non-NULL unsigned integer pointer used for the query response") {
                CHECK(_noteI2CReceive_fake.arg3_val != nullptr);
            }
        }
    }

    GIVEN("`_noteI2CReceive()` succeeds") {
        NoteDelayMs_fake.custom_fake = NoteDelayMs_mock;
        NoteGetMs_fake.custom_fake = NoteGetMs_mock;
        _noteI2CReset_fake.return_val = true;
        _noteI2CTransmit_fake.return_val = nullptr;

        WHEN("`_noteI2CReceive()` available returns a value greater than that which can be contained by uint16_t") {
            const char * (*AvailGTFFFFMessageSeq[])(uint16_t, uint8_t *, uint16_t, uint32_t *) = {
                _noteI2CReceive_AvailGTFFFFMessageSeq0,
                _noteI2CReceive_AvailGTFFFFMessageSeq1,
            };
            SET_CUSTOM_FAKE_SEQ(_noteI2CReceive, AvailGTFFFFMessageSeq, 2);
            const bool success = _i2cNoteReset();

            THEN("The third parameter is populated with the available value from the previous query") {
                REQUIRE(_noteI2CReceive_fake.call_count >= 2);
                CHECK(_noteI2CReceive_fake.arg2_history[0] == 0);
                CHECK(_noteI2CReceive_fake.arg2_history[1] == NoteI2CMax());
            }
        }

        WHEN("`_noteI2CReceive()` available returns a value greater than the buffer size") {
            const char * (*AvailGTBufferMessageSeq[])(uint16_t, uint8_t *, uint16_t, uint32_t *) = {
                _noteI2CReceive_AvailGTBufferMessageSeq0,
                _noteI2CReceive_AvailGTBufferMessageSeq1,
            };
            SET_CUSTOM_FAKE_SEQ(_noteI2CReceive, AvailGTBufferMessageSeq, 2);
            const bool success = _i2cNoteReset();

            THEN("The third parameter is populated with a value capped at the buffer size") {
                REQUIRE(_noteI2CReceive_fake.call_count >= 2);
                CHECK(_noteI2CReceive_fake.arg2_val <= ALLOC_CHUNK);
            }
        }

        WHEN("`_noteI2CReceive()` available returns a value greater than the maximum size allowed by the Serial-over-I2C protocol") {
            const char * (*AvailGTMaxMessageSeq[])(uint16_t, uint8_t *, uint16_t, uint32_t *) = {
                _noteI2CReceive_AvailGTMaxMessageSeq0,
                _noteI2CReceive_AvailGTMaxMessageSeq1,
            };
            SET_CUSTOM_FAKE_SEQ(_noteI2CReceive, AvailGTMaxMessageSeq, 2);
            const bool success = _i2cNoteReset();

            THEN("The third parameter is populated with a value capped at NoteI2CMax()") {
                REQUIRE(_noteI2CReceive_fake.call_count >= 2);
                CHECK(_noteI2CReceive_fake.arg2_val <= NoteI2CMax());
            }
        }
    }

    GIVEN("`_noteI2CReceive()` will drain the Notecard I2C interface for `CARD_RESET_DRAIN_MS`") {
        NoteDelayMs_fake.custom_fake = NoteDelayMs_mock;
        NoteGetMs_fake.custom_fake = NoteGetMs_mock;
        _noteI2CReset_fake.return_val = true;
        _noteI2CTransmit_fake.return_val = nullptr;

        WHEN("`_noteI2CReceive()` does not fail") {
            rtc_ms = 0;
            _noteI2CReceive_fake.return_val = nullptr;
            const bool success = _i2cNoteReset();

            THEN("`_noteI2CReceive()` will be called for `CARD_RESET_DRAIN_MS`") {
                // Must account for both the priming delay and the post transmit delay
                CHECK(rtc_ms >= CARD_RESET_DRAIN_MS + (CARD_REQUEST_I2C_SEGMENT_DELAY_MS * 2));
            }
        }

        WHEN("`_noteI2CReceive()` receives any char that is neither \\r nor \\n") {
            const char * (*ClearMessageSeq[])(uint16_t, uint8_t *, uint16_t, uint32_t *) = {
                _noteI2CReceive_ClearMessageSeq0,
                _noteI2CReceive_ClearMessageSeq1,
                _noteI2CReceive_ClearMessageSeq2,
                _noteI2CReceive_ClearMessageSeq3,
            };
            SET_CUSTOM_FAKE_SEQ(_noteI2CReceive, ClearMessageSeq, 4);
            const bool success = _i2cNoteReset();

            THEN("`_i2cNoteReset` fails") {
                CHECK(success == false);
            }
        }

        WHEN("`_noteI2CReceive()` receives nothing") {
            _noteI2CReceive_fake.return_val = nullptr;
            const bool success = _i2cNoteReset();

            THEN("`_i2cNoteReset` fails") {
                CHECK(success == false);
            }
            THEN("`_noteI2CReset()` is called") {
                CHECK(_noteI2CReset_fake.call_count > 1);
            }
        }

        AND_GIVEN("`_noteI2CReceive()` receives nothing") {
            _noteI2CReceive_fake.return_val = nullptr;

            WHEN("`_noteI2CReset()` fails") {
                bool retSeq[] = { true, false };
                SET_RETURN_SEQ(_noteI2CReset, retSeq, 2);
                const bool success = _i2cNoteReset();
                THEN("The I2C mutex will be released") {
                    CHECK(NoteUnlockI2C_fake.call_count == NoteLockI2C_fake.call_count);
                }
                THEN("Exit immediately without retrying") {
                    CHECK(_noteI2CReset_fake.call_count == 2);
                }
            }

            WHEN("`_noteI2CReset()` succeeds") {
                _i2cNoteReset();

                THEN("Then the I/O delay is respected") {
                    // 1 + pre loop
                    // 10 retries (simple _noteI2CReceive_fake returns zero --> nothing was found 10x)
                    CHECK(_delayIO_fake.call_count > 10);
                }
            }
        }

        WHEN("`_noteI2CReceive()` receives only `\\r` and/or `\\n`") {
            const char * (*CleanResetSeq[])(uint16_t, uint8_t *, uint16_t, uint32_t *) = {
                _noteI2CReceive_CleanResetSeq0,
                _noteI2CReceive_CleanResetSeq1,
            };
            SET_CUSTOM_FAKE_SEQ(_noteI2CReceive, CleanResetSeq, 2);
            const bool success = _i2cNoteReset();

            THEN("Exit retry loop immediately") {
                CHECK(_noteI2CTransmit_fake.call_count == 1);
            }

            THEN("`_i2cNoteReset` succeeds") {
                CHECK(success);
            }
            AND_THEN("The I2C mutex will be released") {
                CHECK(NoteUnlockI2C_fake.call_count == NoteLockI2C_fake.call_count);
            }
        }
    }

    // Sanity check against stray return paths
    CHECK(NoteLockI2C_fake.call_count == 1);
    CHECK(NoteUnlockI2C_fake.call_count == 1);

    RESET_FAKE(_delayIO);
    RESET_FAKE(NoteDelayMs);
    RESET_FAKE(NoteGetMs);
    RESET_FAKE(_noteI2CReset);
    RESET_FAKE(_noteI2CTransmit);
    RESET_FAKE(_noteI2CReceive);
    RESET_FAKE(NoteLockI2C);
    RESET_FAKE(NoteUnlockI2C);
}

}


