/*!
 * @file NoteSerialHooks_test.cpp
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

#include "n_lib.h"

namespace
{

bool txnStartCalled = false;
bool txnStopCalled = false;

bool MyTxnStart(uint32_t)
{
    txnStartCalled = true;
    return true;
}

void MyTxnStop()
{
    txnStopCalled = true;
    return;
}

SCENARIO("NoteSerialHooks")
{
    SECTION("Hooks not set") {
        noteTransactionStart(0);
        noteTransactionStop();

        CHECK(!txnStartCalled);
        CHECK(!txnStopCalled);
    }

    SECTION("Hooks set") {
        NoteSetFnTransaction(MyTxnStart, MyTxnStop);
        noteTransactionStart(0);
        noteTransactionStop();

        CHECK(txnStartCalled);
        CHECK(txnStopCalled);
    }

    txnStartCalled = false;
    txnStopCalled = false;
}

}

#endif // NOTE_C_TEST
