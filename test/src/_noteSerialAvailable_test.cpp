/*!
 * @file _noteSerialAvailable_test.cpp
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
extern serialAvailableFn hookSerialAvailable;

namespace
{

const bool available_result = true;

SCENARIO("_noteSerialAvailable")
{
    GIVEN("hookActiveInterface is not set to NOTE_C_INTERFACE_SERIAL")
    {
        hookActiveInterface = NOTE_C_INTERFACE_NONE;

        AND_GIVEN("hookSerialAvailable is NULL")
        {
            hookSerialAvailable = NULL;

            WHEN("_noteSerialAvailable is called")
            {
                const bool result = _noteSerialAvailable();

                THEN("_noteSerialAvailable returns false")
                {
                    CHECK_FALSE(result);
                }
            }
        }

        AND_GIVEN("hookSerialAvailable is not NULL")
        {
            hookSerialAvailable = []() { return available_result; };

            WHEN("_noteSerialAvailable is called")
            {
                const bool result = _noteSerialAvailable();

                THEN("_noteSerialAvailable returns false")
                {
                    CHECK_FALSE(result);
                }
            }
        }
    }

    GIVEN("hookActiveInterface is set to NOTE_C_INTERFACE_SERIAL")
    {
        hookActiveInterface = NOTE_C_INTERFACE_SERIAL;

        AND_GIVEN("hookSerialAvailable is NULL")
        {
            hookSerialAvailable = NULL;

            WHEN("_noteSerialAvailable is called")
            {
                const bool result = _noteSerialAvailable();

                THEN("_noteSerialAvailable returns false")
                {
                    CHECK_FALSE(result);
                }
            }
        }

        AND_GIVEN("hookSerialAvailable is not NULL")
        {
            hookSerialAvailable = []() { return available_result; };

            WHEN("_noteSerialAvailable is called")
            {
                const bool result = _noteSerialAvailable();

                THEN("_noteSerialAvailable returns the result of hookSerialAvailable")
                {
                    CHECK(result == available_result);
                }
            }
        }
    }
}

}
