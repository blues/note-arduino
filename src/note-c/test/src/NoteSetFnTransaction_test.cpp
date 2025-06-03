/*!
 * @file NoteSetFnTransaction_test.cpp
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
FAKE_VOID_FUNC(_noteLockNote)
FAKE_VOID_FUNC(_noteUnlockNote)

extern txnStartFn hookTransactionStart;
extern txnStopFn hookTransactionStop;

namespace
{

bool MyTransactionStart(uint32_t)
{
    return false;
}

void MyTransactionStop(void)
{
}

SCENARIO("NoteSetFnTransaction")
{
    GIVEN("Transaction hooks have not been set (i.e. NULL)") {
        hookTransactionStart = NULL;
        hookTransactionStop = NULL;

        WHEN("NoteSetFnTransaction is called") {
            NoteSetFnTransaction(MyTransactionStart, MyTransactionStop);

            THEN("The hooks are set") {
                CHECK(hookTransactionStart == MyTransactionStart);
                CHECK(hookTransactionStop == MyTransactionStop);
            }

            THEN("The Notecard lock is taken and released") {
                CHECK(1 == _noteLockNote_fake.call_count);
                CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
            }
        }
    }

    GIVEN("Transaction hooks have been set with non-NULL values") {
        hookTransactionStart = MyTransactionStart;
        hookTransactionStop = MyTransactionStop;

        WHEN("NoteSetFnTransaction is called") {
            NoteSetFnTransaction(NULL, NULL);

            THEN("The existing hooks overridden") {
                CHECK(hookTransactionStart == NULL);
                CHECK(hookTransactionStop == NULL);
            }
        }
    }

    RESET_FAKE(_noteLockNote);
    RESET_FAKE(_noteUnlockNote);
}

}
