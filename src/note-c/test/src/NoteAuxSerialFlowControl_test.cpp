/*!
 * @file NoteAuxSerialFlowControl_test.cpp
 *
 * Written by the Blues Inc. team.
 *
 * Copyright (c) 2025 Blues Inc. MIT License. Use of this source code is
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

FAKE_VOID_FUNC(NoteDebugWithLevel, uint8_t, const char *)
FAKE_VALUE_FUNC(J *, NoteNewRequest, const char *)
FAKE_VALUE_FUNC(bool, NoteRequest, J *)

namespace
{

SCENARIO("NoteAuxSerialFlowControl")
{
    GIVEN("Alloc and free functions have been set") {
        NoteSetFnDefault(malloc, free, NULL, NULL);

        AND_GIVEN("Values for buffer size and delay") {
            int bufSize = 79;
            int delayMs = 17;

            AND_GIVEN("NoteRequest succeeds") {
                // Ensure the fake is set up to return true
                NoteRequest_fake.return_val = true;

                AND_GIVEN("NoteNewRequest succeeds") {
                    // Ensure the fake is set up to return a valid request object
                    NoteNewRequest_fake.custom_fake = [](const char *reqType) -> J * {
                        // Create a new JSON object for the request
                        J *request = JCreateObject();
                        if (request != NULL)
                        {
                            JAddStringToObject(request, "req", reqType);
                        }
                        return request;
                    };

                    WHEN("NoteAuxSerialFlowControl is called") {
                        bool result = NoteAuxSerialFlowControl(bufSize, delayMs);

                        THEN("A request JSON object is created") {
                            REQUIRE(NoteRequest_fake.call_count == 1);
                            REQUIRE(NoteRequest_fake.arg0_history[0] != NULL);
                        }

                        THEN("The request type is 'card.aux.serial'") {
                            const char *type = JGetString(NoteRequest_fake.arg0_history[0], "req");
                            CHECK(strcmp(type, "card.aux.serial") == 0);
                        }

                        THEN("Buffer size minus one is set in request") {
                            const int max = JGetInt(NoteRequest_fake.arg0_history[0], "max");
                            CHECK(max == bufSize - 1);
                        }

                        THEN("Delay in milliseconds is set in request") {
                            const int ms = JGetInt(NoteRequest_fake.arg0_history[0], "ms");
                            CHECK(ms == delayMs);
                        }

                        THEN("The function returns true") {
                            CHECK(result == true);
                        }

                        // Free the JSON object created by NoteAuxSerialFlowControl()
                        JDelete(NoteRequest_fake.arg0_history[0]);
                    }
                }
            }

            AND_GIVEN("NoteNewRequest fails") {
                // Ensure the fake is set up to return false
                NoteNewRequest_fake.return_val = nullptr;

                WHEN("NoteAuxSerialFlowControl is called") {
                    bool result = NoteAuxSerialFlowControl(bufSize, delayMs);
                    REQUIRE(NoteNewRequest_fake.call_count > 0);

                    THEN("An error is logged") {
                        CHECK(NoteDebugWithLevel_fake.call_count > 0);
                        CHECK(NoteDebugWithLevel_fake.arg0_history[0] == NOTE_C_LOG_LEVEL_ERROR);
                    }

                    THEN("The function returns false") {
                        CHECK(result == false);
                    }

                    // Free the JSON object created by NoteAuxSerialFlowControl()
                    JDelete(NoteRequest_fake.arg0_history[0]);
                }
            }
        }
    }

    RESET_FAKE(NoteDebugWithLevel);
    RESET_FAKE(NoteNewRequest);
    RESET_FAKE(NoteRequest);
}

}
