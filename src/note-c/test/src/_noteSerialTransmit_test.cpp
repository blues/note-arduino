/*!
 * @file _noteSerialTransmit_test.cpp
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
extern serialTransmitFn hookSerialTransmit;

namespace
{

uint8_t * hookParameter_buffer;
size_t hookParameter_buffer_size;
bool hookParameter_flush;

SCENARIO("_noteSerialTransmit")
{
    GIVEN("hookActiveInterface is not set to NOTE_C_INTERFACE_SERIAL")
    {
        hookActiveInterface = NOTE_C_INTERFACE_NONE;

        AND_GIVEN("hookSerialTransmit is NULL")
        {
            hookSerialTransmit = NULL;

            WHEN("_noteSerialTransmit is called")
            {
                _noteSerialTransmit(NULL, 0, false);

                THEN("_noteSerialTransmit does nothing")
                {
                    SUCCEED();
                }
            }
        }

        AND_GIVEN("hookSerialTransmit is not NULL")
        {
            hookSerialTransmit = [](uint8_t *, size_t, bool) {};

            WHEN("_noteSerialTransmit is called")
            {
                _noteSerialTransmit(NULL, 0, false);

                THEN("_noteSerialTransmit does nothing")
                {
                    SUCCEED();
                }
            }
        }
    }

    GIVEN("hookActiveInterface is set to NOTE_C_INTERFACE_SERIAL")
    {
        hookActiveInterface = NOTE_C_INTERFACE_SERIAL;

        AND_GIVEN("hookSerialTransmit is NULL")
        {
            hookSerialTransmit = NULL;

            WHEN("_noteSerialTransmit is called")
            {
                _noteSerialTransmit(NULL, 0, false);

                THEN("_noteSerialTransmit does nothing")
                {
                    SUCCEED();
                }
            }
        }

        AND_GIVEN("hookSerialTransmit is not NULL")
        {
            hookSerialTransmit = [](uint8_t *buffer_, size_t buffer_size_, bool flush_) {
                hookParameter_buffer = buffer_;
                hookParameter_buffer_size = buffer_size_;
                hookParameter_flush = flush_;
            };

            AND_GIVEN("valid parameters") {
                uint8_t buffer[] = "test";
                const size_t buffer_size = sizeof(buffer);
                const bool flush = true;

                WHEN("_noteSerialTransmit is called")
                {
                    _noteSerialTransmit(buffer, buffer_size, flush);

                    THEN("_noteSerialTransmit calls hookSerialTransmit with the same parameters")
                    {
                        CHECK(buffer == hookParameter_buffer);
                        CHECK(buffer_size == hookParameter_buffer_size);
                        CHECK(flush == hookParameter_flush);
                    }
                }
            }
        }
    }
}

}
