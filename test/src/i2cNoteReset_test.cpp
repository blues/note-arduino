/*!
 * @file i2cNoteReset_test.cpp
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
FAKE_VALUE_FUNC(bool, NoteI2CReset, uint16_t)
FAKE_VALUE_FUNC(const char *, NoteI2CTransmit, uint16_t, uint8_t *, uint16_t)
FAKE_VALUE_FUNC(const char *, NoteI2CReceive, uint16_t, uint8_t *, uint16_t,
                uint32_t *)
FAKE_VOID_FUNC(NoteLockI2C)
FAKE_VOID_FUNC(NoteUnlockI2C)

namespace
{

TEST_CASE("i2cNoteReset")
{
    RESET_FAKE(NoteI2CReset);
    RESET_FAKE(NoteI2CTransmit);
    RESET_FAKE(NoteI2CReceive);
    RESET_FAKE(NoteLockI2C);
    RESET_FAKE(NoteUnlockI2C);

    SECTION("NoteI2CReset fails") {
        NoteI2CReset_fake.return_val = false;

        REQUIRE(!i2cNoteReset());
        REQUIRE(NoteI2CReset_fake.call_count == 1);
    }

    SECTION("NoteI2CTransmit fails") {
        NoteI2CReset_fake.return_val = true;
        NoteI2CTransmit_fake.return_val = "an error";

        REQUIRE(!i2cNoteReset());
        // Make sure I2C bus is reset after failed transmit.
        REQUIRE(NoteI2CReset_fake.call_count == 2);
        REQUIRE(NoteI2CTransmit_fake.call_count == 1);
    }

    SECTION("NoteI2CReceive fails") {
        NoteI2CReset_fake.return_val = true;
        NoteI2CReceive_fake.return_val = "an error";

        REQUIRE(!i2cNoteReset());
        // Make sure I2C bus is reset after failed receive.
        REQUIRE(NoteI2CReset_fake.call_count == 2);
        REQUIRE(NoteI2CTransmit_fake.call_count == 1);
        // Receive will be retried 3 times.
        REQUIRE(NoteI2CReceive_fake.call_count == 3);
    }

    SECTION("Nothing to read") {
        NoteI2CReset_fake.return_val = true;
        NoteI2CReceive_fake.custom_fake = NoteI2CReceiveNothing;

        REQUIRE(i2cNoteReset());
        REQUIRE(NoteI2CReset_fake.call_count == 1);
        REQUIRE(NoteI2CTransmit_fake.call_count == 1);
        REQUIRE(NoteI2CReceive_fake.call_count == 1);
    }

    SECTION("Drain chunks") {
        NoteI2CReset_fake.return_val = true;
        NoteI2CReceive_fake.custom_fake = NoteI2CReceiveMultiChunk;

        REQUIRE(i2cNoteReset());
        REQUIRE(NoteI2CReset_fake.call_count == 1);
        REQUIRE(NoteI2CTransmit_fake.call_count == 1);
        // 1 call to get available data to read, plus however many I2C
        // transactions it takes to read that data.
        const uint32_t numRecvCalls = 1 + (I2C_MULTI_CHUNK_RECV_BYTES +
                                           (NoteI2CMax() - 1)) / NoteI2CMax();
        REQUIRE(NoteI2CReceive_fake.call_count == numRecvCalls);
    }

    REQUIRE(NoteLockI2C_fake.call_count == 1);
    REQUIRE(NoteUnlockI2C_fake.call_count == 1);
}

}

#endif // TEST
