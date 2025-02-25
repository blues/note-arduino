/*!
 * @file NoteSetFnSerial_test.cpp
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
FAKE_VALUE_FUNC(bool, _serialNoteReset)
FAKE_VALUE_FUNC(const char *, _serialNoteTransaction, const char *, size_t, char **, uint32_t)

namespace
{

uint8_t serialResetCalled = 0;
uint8_t serialTransmitCalled = 0;
uint8_t serialAvailableCalled = 0;
uint8_t serialReceiveCalled = 0;

bool serialReset()
{
    ++serialResetCalled;
    return true;
}

void serialTransmit(uint8_t *, size_t, bool)
{
    ++serialTransmitCalled;
}

bool serialAvailable()
{
    ++serialAvailableCalled;
    return true;
}

char serialReceive()
{
    ++serialReceiveCalled;
    return 'a';
}

SCENARIO("NoteSetFnSerial")
{
    char req[] = "{ \"req\": \"note.add\" }\n";
    char *resp = NULL;
    _serialNoteReset_fake.return_val = true;
    _serialNoteTransaction_fake.return_val = NULL;

    NoteSetFnSerial(serialReset, serialTransmit, serialAvailable,
                    serialReceive);

    CHECK(_noteSerialReset());
    CHECK(serialResetCalled == 1);

    _noteSerialTransmit((uint8_t *)req, strlen(req), false);
    CHECK(serialTransmitCalled == 1);

    CHECK(_noteSerialAvailable());
    CHECK(serialAvailableCalled == 1);

    CHECK(_noteSerialReceive() == 'a');
    CHECK(serialReceiveCalled == 1);

    CHECK(strcmp(_noteActiveInterface(), "serial") == 0);

    CHECK(_noteHardReset());
    CHECK(_serialNoteReset_fake.call_count == 1);

    CHECK(_noteJSONTransaction(req, strlen(req), &resp, CARD_INTER_TRANSACTION_TIMEOUT_SEC) == NULL);
    CHECK(_serialNoteTransaction_fake.call_count == 1);

    // Unset the callbacks and ensure they aren't called again.
    NoteSetFnSerial(NULL, NULL, NULL, NULL);
    NoteSetFnDisabled();

    CHECK(_noteSerialReset());
    CHECK(serialResetCalled == 1);

    _noteSerialTransmit((uint8_t *)req, strlen(req), false);
    CHECK(serialTransmitCalled == 1);

    CHECK(!_noteSerialAvailable());
    CHECK(serialAvailableCalled == 1);

    CHECK(_noteSerialReceive() == 0);
    CHECK(serialReceiveCalled == 1);

    CHECK(strcmp(_noteActiveInterface(), "unknown") == 0);

    CHECK(_noteHardReset());
    CHECK(_serialNoteReset_fake.call_count == 1);

    CHECK(_noteJSONTransaction(req, strlen(req), &resp, CARD_INTER_TRANSACTION_TIMEOUT_SEC) != NULL);
    CHECK(_serialNoteTransaction_fake.call_count == 1);

    RESET_FAKE(_serialNoteReset);
    RESET_FAKE(_serialNoteTransaction);
}

}
