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
#include "fff.h"

#include "n_lib.h"

DEFINE_FFF_GLOBALS

namespace
{

bool mockSerialReset(void)
{
    return false;
}

void mockSerialTransmit(uint8_t *txBuf, size_t txBufSize, bool flush)
{
    (void)txBuf;
    (void)txBufSize;
    (void)flush;
}

bool mockSerialAvailable(void)
{
    return false;
}

char mockSerialReceive(void)
{
    return 0;
}

SCENARIO("NoteGetFnSerial")
{
    GIVEN("A set of serial functions") {
        NoteSetFnSerial(mockSerialReset, mockSerialTransmit, mockSerialAvailable, mockSerialReceive);

        AND_GIVEN("Valid pointers for all serial functions are provided") {
            serialResetFn resetFunc = NULL;
            serialTransmitFn transmitFunc = NULL;
            serialAvailableFn availableFunc = NULL;
            serialReceiveFn receiveFunc = NULL;

            WHEN("NoteGetFnSerial is called") {
                NoteGetFnSerial(&resetFunc, &transmitFunc, &availableFunc, &receiveFunc);

                THEN("It should return the mock serial functions") {
                    CHECK(resetFunc == mockSerialReset);
                    CHECK(transmitFunc == mockSerialTransmit);
                    CHECK(availableFunc == mockSerialAvailable);
                    CHECK(receiveFunc == mockSerialReceive);
                }
            }
        }

        AND_GIVEN("Only NULL pointers are provided") {
            WHEN("NoteGetFnSerial is called") {
                NoteGetFnSerial(NULL, NULL, NULL, NULL);

                THEN("It should not crash") {
                    SUCCEED();
                }
            }
        }

        AND_GIVEN("Only a vaild pointer for the serial reset function is provided") {
            serialResetFn resetFunc = NULL;

            WHEN("NoteGetFnSerial is called") {
                NoteGetFnSerial(&resetFunc, NULL, NULL, NULL);

                THEN("It should return the mock serial reset function") {
                    CHECK(resetFunc == mockSerialReset);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the serial transmit function is provided") {
            serialTransmitFn transmitFunc = NULL;

            WHEN("NoteGetFnSerial is called") {
                NoteGetFnSerial(NULL, &transmitFunc, NULL, NULL);

                THEN("It should return the mock serial transmit function") {
                    CHECK(transmitFunc == mockSerialTransmit);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the serial available function is provided") {
            serialAvailableFn availableFunc = NULL;

            WHEN("NoteGetFnSerial is called") {
                NoteGetFnSerial(NULL, NULL, &availableFunc, NULL);

                THEN("It should return the mock serial available function") {
                    CHECK(availableFunc == mockSerialAvailable);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the serial receive function is provided") {
            serialReceiveFn receiveFunc = NULL;

            WHEN("NoteGetFnSerial is called") {
                NoteGetFnSerial(NULL, NULL, NULL, &receiveFunc);

                THEN("It should return the mock serial receive function") {
                    CHECK(receiveFunc == mockSerialReceive);
                }
            }
        }
    }
}

}
