/*!
 * @file _noteSerialReceive_test.cpp
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
extern serialReceiveFn hookSerialReceive;

namespace
{

const char receive_result = 'z';

SCENARIO("_noteSerialReceive")
{
    GIVEN("hookActiveInterface is not set to NOTE_C_INTERFACE_SERIAL")
    {
        hookActiveInterface = NOTE_C_INTERFACE_NONE;

        AND_GIVEN("hookSerialReceive is NULL")
        {
            hookSerialReceive = NULL;

            WHEN("_noteSerialReceive is called")
            {
                const char result = _noteSerialReceive();

                THEN("_noteSerialReceive returns '\0'")
                {
                    CHECK(result == '\0');
                }
            }
        }

        AND_GIVEN("hookSerialReceive is not NULL")
        {
            hookSerialReceive = []() -> char { return receive_result; };

            WHEN("_noteSerialReceive is called")
            {
                const char result = _noteSerialReceive();

                THEN("_noteSerialReceive returns '\0'")
                {
                    CHECK(result == '\0');
                }
            }
        }
    }

    GIVEN("hookActiveInterface is set to NOTE_C_INTERFACE_SERIAL")
    {
        hookActiveInterface = NOTE_C_INTERFACE_SERIAL;

        AND_GIVEN("hookSerialReceive is NULL")
        {
            hookSerialReceive = NULL;

            WHEN("_noteSerialReceive is called")
            {
                const char result = _noteSerialReceive();

                THEN("_noteSerialReceive returns '\0'")
                {
                    CHECK(result == '\0');
                }
            }
        }

        AND_GIVEN("hookSerialReceive is not NULL")
        {
            hookSerialReceive = []() -> char { return receive_result; };

            WHEN("_noteSerialReceive is called")
            {
                const char result = _noteSerialReceive();

                THEN("_noteSerialReceive returns the result of hookSerialReceive")
                {
                    CHECK(result == receive_result);
                }
            }
        }
    }
}

}
