/*!
 * @file NoteTransaction_test.cpp
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
//#include <fff.h>

#include "n_lib.h"

// DEFINE_FFF_GLOBALS
// FAKE_VALUE_FUNC(bool, NoteReset)
// FAKE_VALUE_FUNC(const char *, _noteJSONTransaction, const char *, size_t, char **, uint32_t)
// FAKE_VALUE_FUNC(bool, _noteTransactionStart, uint32_t)
// FAKE_VALUE_FUNC(J *, NoteUserAgent)
// FAKE_VALUE_FUNC(bool, _crcError, char *, uint16_t)

SCENARIO("NoteResponseError")
{
    NoteSetFnDefault(malloc, free, NULL, NULL);

    // // NoteReset's mock should succeed unless the test explicitly instructs
    // // it to fail.
    // NoteReset_fake.return_val = true;
    // _noteTransactionStart_fake.return_val = true;
    // _crcError_fake.return_val = false;

    SECTION("No err field, returns false") {
        J* rsp = JParse("{\"test\":\"me\"}");
        //CHECK(rsp == NULL);
        CHECK(!(NoteResponseError(rsp)));
        JDelete(rsp);
    }

    SECTION("Supplying a response with err returns True") {
        J* rsp = JParse("{\"err\":\"DFU mode is not yet fully active: starting communications {wait-module} {connecting} {dfu-not-ready}\"}\n");
        bool hasError = NoteResponseError(rsp);
        CHECK((hasError ==true && rsp != NULL));
        JDelete(rsp);
    }
}

