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

#ifdef NOTE_C_TEST

#include <catch2/catch_test_macros.hpp>
#include "fff.h"

#include "n_lib.h"

DEFINE_FFF_GLOBALS
FAKE_VALUE_FUNC(bool, serialNoteReset)
FAKE_VALUE_FUNC(const char *, serialNoteTransaction, const char *, size_t, char **, size_t)

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
    serialNoteReset_fake.return_val = true;
    serialNoteTransaction_fake.return_val = NULL;

    NoteSetFnSerial(serialReset, serialTransmit, serialAvailable,
                    serialReceive);

    CHECK(noteSerialReset());
    CHECK(serialResetCalled == 1);

    noteSerialTransmit((uint8_t *)req, strlen(req), false);
    CHECK(serialTransmitCalled == 1);

    CHECK(noteSerialAvailable());
    CHECK(serialAvailableCalled == 1);

    CHECK(noteSerialReceive() == 'a');
    CHECK(serialReceiveCalled == 1);

    CHECK(strcmp(noteActiveInterface(), "serial") == 0);

    CHECK(noteHardReset());
    CHECK(serialNoteReset_fake.call_count == 1);

    CHECK(noteJSONTransaction(req, strlen(req), &resp, CARD_INTER_TRANSACTION_TIMEOUT_SEC) == NULL);
    CHECK(serialNoteTransaction_fake.call_count == 1);

    // Unset the callbacks and ensure they aren't called again.
    NoteSetFnSerial(NULL, NULL, NULL, NULL);
    NoteSetFnDisabled();

    CHECK(noteSerialReset());
    CHECK(serialResetCalled == 1);

    noteSerialTransmit((uint8_t *)req, strlen(req), false);
    CHECK(serialTransmitCalled == 1);

    CHECK(!noteSerialAvailable());
    CHECK(serialAvailableCalled == 1);

    CHECK(noteSerialReceive() == 0);
    CHECK(serialReceiveCalled == 1);

    CHECK(strcmp(noteActiveInterface(), "unknown") == 0);

    CHECK(noteHardReset());
    CHECK(serialNoteReset_fake.call_count == 1);

    CHECK(noteJSONTransaction(req, strlen(req), &resp, CARD_INTER_TRANSACTION_TIMEOUT_SEC) != NULL);
    CHECK(serialNoteTransaction_fake.call_count == 1);

    RESET_FAKE(serialNoteReset);
    RESET_FAKE(serialNoteTransaction);
}

}

#endif // NOTE_C_TEST
