/*!
 * @file _noteSerialReset_test.cpp
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

#include "n_lib.h"

extern volatile int hookActiveInterface;
extern serialResetFn hookSerialReset;

namespace
{

const bool reset_result = false;

SCENARIO("_noteSerialReset")
{
    GIVEN("hookActiveInterface is not set to NOTE_C_INTERFACE_SERIAL")
    {
        hookActiveInterface = NOTE_C_INTERFACE_NONE;

        AND_GIVEN("hookSerialReset is NULL")
        {
            hookSerialReset = NULL;

            WHEN("_noteSerialReset is called")
            {
                const bool result = _noteSerialReset();

                THEN("_noteSerialReset returns true")
                {
                    CHECK(result);
                }
            }
        }

        AND_GIVEN("hookSerialReset is not NULL")
        {
            hookSerialReset = []() { return reset_result; };

            WHEN("_noteSerialReset is called")
            {
                const bool result = _noteSerialReset();

                THEN("_noteSerialReset returns true")
                {
                    CHECK(result);
                }
            }
        }
    }

    GIVEN("hookActiveInterface is set to NOTE_C_INTERFACE_SERIAL")
    {
        hookActiveInterface = NOTE_C_INTERFACE_SERIAL;

        AND_GIVEN("hookSerialReset is NULL")
        {
            hookSerialReset = NULL;

            WHEN("_noteSerialReset is called")
            {
                const bool result = _noteSerialReset();

                THEN("_noteSerialReset returns true")
                {
                    CHECK(result);
                }
            }
        }

        AND_GIVEN("hookSerialReset is not NULL")
        {
            hookSerialReset = []() { return reset_result; };

            WHEN("_noteSerialReset is called")
            {
                const bool result = _noteSerialReset();

                THEN("_noteSerialReset returns the result of hookSerialReset")
                {
                    CHECK(result == reset_result);
                }
            }
        }
    }
}

}
