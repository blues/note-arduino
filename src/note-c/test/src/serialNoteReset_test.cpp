/*!
 * @file serialNoteReset_test.cpp
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
FAKE_VALUE_FUNC(bool, noteSerialReset)
FAKE_VOID_FUNC(noteSerialTransmit, uint8_t *, size_t, bool)
FAKE_VALUE_FUNC(bool, noteSerialAvailable)
FAKE_VALUE_FUNC(char, noteSerialReceive)
FAKE_VALUE_FUNC(long unsigned int, NoteGetMs)

namespace
{

long unsigned int NoteGetMsMock()
{
    static long unsigned int ret = 500;

    // Cycle through returning 0, 1, and 500. 500 ms is the timeout of the
    // receive loop in serialNoteReset.
    switch (ret) {
    case 0:
        ret = 1;
        break;
    case 1:
        ret = 500;
        break;
    case 500:
        ret = 0;
        break;
    }

    return ret;
}

SCENARIO("serialNoteReset")
{
    SECTION("noteSerialReset fails") {
        noteSerialReset_fake.return_val = false;

        CHECK(!serialNoteReset());
        CHECK(noteSerialReset_fake.call_count == 1);
        CHECK(noteSerialTransmit_fake.call_count == 0);
    }

    SECTION("Serial never available") {
        noteSerialReset_fake.return_val = true;
        noteSerialAvailable_fake.return_val = false;
        NoteGetMs_fake.custom_fake = NoteGetMsMock;

        CHECK(!serialNoteReset());
        // serialNoteReset has retry logic, so we expect retries.
        CHECK(noteSerialTransmit_fake.call_count > 1);
        CHECK(noteSerialReceive_fake.call_count == 0);
    }

    SECTION("Character received") {
        NoteGetMs_fake.custom_fake = NoteGetMsMock;
        bool serialAvailRetVals[] = {true, false};
        SET_RETURN_SEQ(noteSerialAvailable, serialAvailRetVals, 2);

        SECTION("Non-control character received") {
            noteSerialReceive_fake.return_val = 'a';

            SECTION("Retry") {
                noteSerialReset_fake.return_val = true;

                CHECK(!serialNoteReset());
                // Expect retries.
                CHECK(noteSerialTransmit_fake.call_count > 1);
            }

            SECTION("noteSerialReset fails before retry possible") {
                bool noteSerialResetRetVals[] = {true, false};
                SET_RETURN_SEQ(noteSerialReset, noteSerialResetRetVals, 2);

                CHECK(!serialNoteReset());
                // No retries.
                CHECK(noteSerialTransmit_fake.call_count == 1);
            }
        }

        SECTION("Only control character received") {
            noteSerialReset_fake.return_val = true;
            noteSerialReceive_fake.return_val = '\n';

            CHECK(serialNoteReset());
            // There should be no retrying.
            CHECK(noteSerialTransmit_fake.call_count == 1);
        }

        CHECK(noteSerialReceive_fake.call_count > 0);
    }

    SECTION("NoteGetMs overflow") {
        noteSerialReset_fake.return_val = true;
        noteSerialReceive_fake.return_val = '\n';
        bool serialAvailRetVals[] = {true, false};
        SET_RETURN_SEQ(noteSerialAvailable, serialAvailRetVals, 2);
        long unsigned int getMsReturnVals[] = {
            UINT32_MAX - 500,
            UINT32_MAX - 400,
            0
        };
        SET_RETURN_SEQ(NoteGetMs, getMsReturnVals, 3);

        CHECK(serialNoteReset());
    }

    RESET_FAKE(noteSerialReset);
    RESET_FAKE(noteSerialTransmit);
    RESET_FAKE(noteSerialAvailable);
    RESET_FAKE(noteSerialReceive);
    RESET_FAKE(NoteGetMs);
}

}

#endif // NOTE_C_TEST
