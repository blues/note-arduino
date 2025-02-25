/*!
 * @file _serialNoteReset_test.cpp
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
FAKE_VALUE_FUNC(bool, _noteSerialReset)
FAKE_VOID_FUNC(_noteSerialTransmit, uint8_t *, size_t, bool)
FAKE_VALUE_FUNC(bool, _noteSerialAvailable)
FAKE_VALUE_FUNC(char, _noteSerialReceive)
FAKE_VALUE_FUNC(uint32_t, NoteGetMs)

namespace
{

uint32_t NoteGetMsMock()
{
    static uint32_t ret = 500;

    // Cycle through returning 0, 1, and 500. 500 ms is the timeout of the
    // receive loop in _serialNoteReset.
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

SCENARIO("_serialNoteReset")
{
    SECTION("_noteSerialReset fails") {
        _noteSerialReset_fake.return_val = false;

        CHECK(!_serialNoteReset());
        CHECK(_noteSerialReset_fake.call_count == 1);
        CHECK(_noteSerialTransmit_fake.call_count == 0);
    }

    SECTION("Serial never available") {
        _noteSerialReset_fake.return_val = true;
        _noteSerialAvailable_fake.return_val = false;
        NoteGetMs_fake.custom_fake = NoteGetMsMock;

        CHECK(!_serialNoteReset());
        // _serialNoteReset has retry logic, so we expect retries.
        CHECK(_noteSerialTransmit_fake.call_count > 1);
        CHECK(_noteSerialReceive_fake.call_count == 0);
    }

    SECTION("Character received") {
        NoteGetMs_fake.custom_fake = NoteGetMsMock;
        bool serialAvailRetVals[] = {true, false};
        SET_RETURN_SEQ(_noteSerialAvailable, serialAvailRetVals, 2);

        SECTION("Non-control character received") {
            _noteSerialReceive_fake.return_val = 'a';

            SECTION("Retry") {
                _noteSerialReset_fake.return_val = true;

                CHECK(!_serialNoteReset());
                // Expect retries.
                CHECK(_noteSerialTransmit_fake.call_count > 1);
            }

            SECTION("_noteSerialReset fails before retry possible") {
                bool _noteSerialResetRetVals[] = {true, false};
                SET_RETURN_SEQ(_noteSerialReset, _noteSerialResetRetVals, 2);

                CHECK(!_serialNoteReset());
                // No retries.
                CHECK(_noteSerialTransmit_fake.call_count == 1);
            }
        }

        SECTION("Only control character received") {
            _noteSerialReset_fake.return_val = true;
            _noteSerialReceive_fake.return_val = '\n';

            CHECK(_serialNoteReset());
            // There should be no retrying.
            CHECK(_noteSerialTransmit_fake.call_count == 1);
        }

        CHECK(_noteSerialReceive_fake.call_count > 0);
    }

    SECTION("NoteGetMs overflow") {
        _noteSerialReset_fake.return_val = true;
        _noteSerialReceive_fake.return_val = '\n';
        bool serialAvailRetVals[] = {true, false};
        SET_RETURN_SEQ(_noteSerialAvailable, serialAvailRetVals, 2);
        uint32_t getMsReturnVals[] = {
            UINT32_MAX - 500,
            UINT32_MAX - 400,
            0
        };
        SET_RETURN_SEQ(NoteGetMs, getMsReturnVals, 3);

        CHECK(_serialNoteReset());
    }

    RESET_FAKE(_noteSerialReset);
    RESET_FAKE(_noteSerialTransmit);
    RESET_FAKE(_noteSerialAvailable);
    RESET_FAKE(_noteSerialReceive);
    RESET_FAKE(NoteGetMs);
}

}


