/*!
 * @file NoteGetFnTransaction_test.cpp
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

bool mockTxnStart(uint32_t timeoutMs)
{
    (void)timeoutMs;
    return false;
}

void mockTxnStop(void)
{
    return;
}

SCENARIO("NoteGetFnTransaction")
{
    GIVEN("A set of transaction functions") {
        NoteSetFnTransaction(mockTxnStart, mockTxnStop);

        AND_GIVEN("Valid pointers for all transaction functions are provided") {
            txnStartFn startFn = NULL;
            txnStopFn stopFn = NULL;

            WHEN("NoteGetFnTransaction is called") {
                NoteGetFnTransaction(&startFn, &stopFn);

                THEN("It should return the mock transaction functions") {
                    CHECK(startFn == mockTxnStart);
                    CHECK(stopFn == mockTxnStop);
                }
            }
        }

        AND_GIVEN("Only NULL pointers are provided") {
            WHEN("NoteGetFnTransaction is called") {
                NoteGetFnTransaction(NULL, NULL);

                THEN("It should not crash") {
                    SUCCEED();
                }
            }
        }

        AND_GIVEN("Only a valid pointer for transaction start function is provided") {
            txnStartFn startFn = NULL;

            WHEN("NoteGetFnTransaction is called") {
                NoteGetFnTransaction(&startFn, NULL);

                THEN("It should return the mock start function") {
                    REQUIRE(startFn == mockTxnStart);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for transaction stop function is provided") {
            txnStopFn stopFn = NULL;

            WHEN("NoteGetFnTransaction is called") {
                NoteGetFnTransaction(NULL, &stopFn);

                THEN("It should return the mock stop function") {
                    REQUIRE(stopFn == mockTxnStop);
                }
            }
        }
    }
}

}
