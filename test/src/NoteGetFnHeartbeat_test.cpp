/*!
 * @file NoteGetFnHeartbeat_test.cpp
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

#include "n_lib.h"

#ifdef NOTE_C_HEARTBEAT_CALLBACK

namespace
{

static int testContext = 42;

bool mockHeartbeatFn(const char *heartbeatJson, void *context)
{
    (void)heartbeatJson;
    (void)context;
    return false;
}

SCENARIO("NoteGetFnHeartbeat")
{
    GIVEN("A mock heartbeat function and context are set") {
        NoteSetFnHeartbeat(mockHeartbeatFn, &testContext);

        WHEN("NoteGetFnHeartbeat is called with NULL parameters") {
            THEN("It should not crash") {
                NoteGetFnHeartbeat(NULL, NULL);
            }
        }

        WHEN("NoteGetFnHeartbeat is called with NULL function pointer") {
            void *retrievedContext = nullptr;

            THEN("It should not crash and retrieve context") {
                NoteGetFnHeartbeat(NULL, &retrievedContext);
                REQUIRE(retrievedContext == &testContext);
            }
        }

        WHEN("NoteGetFnHeartbeat is called with NULL context pointer") {
            heartbeatFn retrievedFn = nullptr;

            THEN("It should not crash and retrieve function") {
                NoteGetFnHeartbeat(&retrievedFn, NULL);
                REQUIRE(retrievedFn == mockHeartbeatFn);
            }
        }

        WHEN("NoteGetFnHeartbeat is called with valid parameters") {
            heartbeatFn retrievedFn = nullptr;
            void *retrievedContext = nullptr;

            THEN("It should return the mock function and context") {
                NoteGetFnHeartbeat(&retrievedFn, &retrievedContext);
                REQUIRE(retrievedFn == mockHeartbeatFn);
                REQUIRE(retrievedContext == &testContext);
            }
        }
    }

    GIVEN("No heartbeat function is set") {
        NoteSetFnHeartbeat(NULL, NULL);

        WHEN("NoteGetFnHeartbeat is called") {
            heartbeatFn retrievedFn = mockHeartbeatFn; // Initialize to non-NULL
            void *retrievedContext = &testContext; // Initialize to non-NULL

            THEN("It should return NULL for both function and context") {
                NoteGetFnHeartbeat(&retrievedFn, &retrievedContext);
                REQUIRE(retrievedFn == NULL);
                REQUIRE(retrievedContext == NULL);
            }
        }
    }
}

}

#endif // NOTE_C_HEARTBEAT_CALLBACK
