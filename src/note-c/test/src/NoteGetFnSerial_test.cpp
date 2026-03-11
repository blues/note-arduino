/*!
 * @file NoteGetFnSerial_test.cpp
 *
 * Written by the Blues Inc. team.
 *
 * Copyright (c) 2024 Blues Inc. MIT License. Use of this source code is
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
FAKE_VOID_FUNC(_noteLockNote)
FAKE_VOID_FUNC(_noteUnlockNote)

extern serialResetFn hookSerialReset;
extern serialTransmitFn hookSerialTransmit;
extern serialAvailableFn hookSerialAvailable;
extern serialReceiveFn hookSerialReceive;

namespace
{

bool MySerialReset(void)
{
    return false;
}

void MySerialTransmit(uint8_t *, size_t, bool)
{
}

bool MySerialAvailable(void)
{
    return false;
}

char MySerialReceive(void)
{
    return 0;
}

SCENARIO("NoteGetFnSerial")
{
    GIVEN("Serial hooks have previously been set") {
        hookSerialReset = MySerialReset;
        hookSerialTransmit = MySerialTransmit;
        hookSerialAvailable = MySerialAvailable;
        hookSerialReceive = MySerialReceive;

        WHEN("NoteGetFnSerial is called") {
            NoteGetFnSerial(NULL, NULL, NULL, NULL);

            THEN("It should not crash") {
                SUCCEED();
            }

            THEN("The Notecard lock is taken and released") {
                CHECK(1 == _noteLockNote_fake.call_count);
                CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
            }
        }

        AND_GIVEN("Valid empty pointers for all serial functions") {
            serialResetFn resetFunc = NULL;
            serialTransmitFn transmitFunc = NULL;
            serialAvailableFn availableFunc = NULL;
            serialReceiveFn receiveFunc = NULL;

            WHEN("NoteGetFnSerial is called") {
                NoteGetFnSerial(&resetFunc, &transmitFunc, &availableFunc, &receiveFunc);

                THEN("It should return the mock serial functions") {
                    CHECK(resetFunc == MySerialReset);
                    CHECK(transmitFunc == MySerialTransmit);
                    CHECK(availableFunc == MySerialAvailable);
                    CHECK(receiveFunc == MySerialReceive);
                }

                THEN("The Notecard lock is taken and released") {
                    CHECK(1 == _noteLockNote_fake.call_count);
                    CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
                }
            }
        }

        AND_GIVEN("Only a vaild pointer for the serial reset function is provided") {
            serialResetFn resetFunc = NULL;

            WHEN("NoteGetFnSerial is called") {
                NoteGetFnSerial(&resetFunc, NULL, NULL, NULL);

                THEN("It should return the mock serial reset function") {
                    CHECK(resetFunc == MySerialReset);
                }

                THEN("The Notecard lock is taken and released") {
                    CHECK(1 == _noteLockNote_fake.call_count);
                    CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the serial transmit function is provided") {
            serialTransmitFn transmitFunc = NULL;

            WHEN("NoteGetFnSerial is called") {
                NoteGetFnSerial(NULL, &transmitFunc, NULL, NULL);

                THEN("It should return the mock serial transmit function") {
                    CHECK(transmitFunc == MySerialTransmit);
                }

                THEN("The Notecard lock is taken and released") {
                    CHECK(1 == _noteLockNote_fake.call_count);
                    CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the serial available function is provided") {
            serialAvailableFn availableFunc = NULL;

            WHEN("NoteGetFnSerial is called") {
                NoteGetFnSerial(NULL, NULL, &availableFunc, NULL);

                THEN("It should return the mock serial available function") {
                    CHECK(availableFunc == MySerialAvailable);
                }

                THEN("The Notecard lock is taken and released") {
                    CHECK(1 == _noteLockNote_fake.call_count);
                    CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the serial receive function is provided") {
            serialReceiveFn receiveFunc = NULL;

            WHEN("NoteGetFnSerial is called") {
                NoteGetFnSerial(NULL, NULL, NULL, &receiveFunc);

                THEN("It should return the mock serial receive function") {
                    CHECK(receiveFunc == MySerialReceive);
                }

                THEN("The Notecard lock is taken and released") {
                    CHECK(1 == _noteLockNote_fake.call_count);
                    CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
                }
            }
        }
    }

    RESET_FAKE(_noteLockNote);
    RESET_FAKE(_noteUnlockNote);
}

}
