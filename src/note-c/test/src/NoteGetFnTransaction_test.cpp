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
#include <fff.h>

#include "n_lib.h"

DEFINE_FFF_GLOBALS
FAKE_VOID_FUNC(_noteLockNote)
FAKE_VOID_FUNC(_noteUnlockNote)

extern txnStartFn hookTransactionStart;
extern txnStopFn hookTransactionStop;

namespace
{

bool MyTxnStart(uint32_t)
{
    return false;
}

void MyTxnStop(void)
{
    return;
}

SCENARIO("NoteGetFnTransaction")
{
    GIVEN("Transaction hooks have been set with non-NULL values") {
        hookTransactionStart = MyTxnStart;
        hookTransactionStop = MyTxnStop;

        WHEN("NoteGetFnTransaction is called") {
            NoteGetFnTransaction(NULL, NULL);

            THEN("It should not crash") {
                SUCCEED();
            }

            THEN("The Notecard lock is taken and released") {
                CHECK(1 == _noteLockNote_fake.call_count);
                CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
            }
        }

        AND_GIVEN("Valid empty pointers for both transaction functions") {
            txnStartFn startFn = NULL;
            txnStopFn stopFn = NULL;

            WHEN("NoteGetFnTransaction is called") {
                NoteGetFnTransaction(&startFn, &stopFn);

                THEN("The hook functions are returned") {
                    CHECK(startFn == MyTxnStart);
                    CHECK(stopFn == MyTxnStop);
                }

                THEN("The Notecard lock is taken and released") {
                    CHECK(1 == _noteLockNote_fake.call_count);
                    CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for transaction start function is provided") {
            txnStartFn startFn = NULL;

            WHEN("NoteGetFnTransaction is called") {
                NoteGetFnTransaction(&startFn, NULL);

                THEN("It should return the mock start function") {
                    REQUIRE(startFn == MyTxnStart);
                }

                THEN("The Notecard lock is taken and released") {
                    CHECK(1 == _noteLockNote_fake.call_count);
                    CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for transaction stop function is provided") {
            txnStopFn stopFn = NULL;

            WHEN("NoteGetFnTransaction is called") {
                NoteGetFnTransaction(NULL, &stopFn);

                THEN("It should return the mock stop function") {
                    REQUIRE(stopFn == MyTxnStop);
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
