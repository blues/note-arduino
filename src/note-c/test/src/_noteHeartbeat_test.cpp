/*!
 * @file _noteHeartbeat_test.cpp
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

#ifdef NOTE_C_HEARTBEAT_CALLBACK

DEFINE_FFF_GLOBALS

namespace
{

static const char *receivedHeartbeat = nullptr;
static void *receivedContext = nullptr;
static int callbackCallCount = 0;
static bool callbackReturnValue = false;

bool mockHeartbeatCallback(const char *heartbeatJson, void *context)
{
    receivedHeartbeat = heartbeatJson;
    receivedContext = context;
    callbackCallCount++;
    return callbackReturnValue;
}

void resetCallbackState()
{
    receivedHeartbeat = nullptr;
    receivedContext = nullptr;
    callbackCallCount = 0;
    callbackReturnValue = false;
}

SCENARIO("_noteHeartbeat")
{

    GIVEN("No heartbeat callback is registered") {
        NoteSetFnHeartbeat(NULL, NULL);
        resetCallbackState();

        WHEN("_noteHeartbeat is called with valid JSON") {
            const char *testHeartbeat = "testing stsafe";
            _noteHeartbeat(testHeartbeat);

            THEN("No callback should be invoked") {
                CHECK(callbackCallCount == 0);
                CHECK(receivedHeartbeat == nullptr);
                CHECK(receivedContext == nullptr);
            }
        }

        WHEN("_noteHeartbeat is called with NULL") {
            _noteHeartbeat(NULL);

            THEN("No callback should be invoked") {
                CHECK(callbackCallCount == 0);
                CHECK(receivedHeartbeat == nullptr);
                CHECK(receivedContext == nullptr);
            }
        }
    }

    GIVEN("A heartbeat callback is registered with context") {
        static int testContext = 42;
        NoteSetFnHeartbeat(mockHeartbeatCallback, &testContext);
        resetCallbackState();

        WHEN("_noteHeartbeat is called with valid heartbeat status") {
            const char *testHeartbeat = "testing stsafe";
            _noteHeartbeat(testHeartbeat);

            THEN("The callback should be invoked with correct parameters") {
                CHECK(callbackCallCount == 1);
                CHECK(receivedHeartbeat == testHeartbeat);
                CHECK(receivedContext == &testContext);
            }
        }

        WHEN("_noteHeartbeat is called with NULL heartbeat") {
            _noteHeartbeat(NULL);

            THEN("The callback should be invoked with NULL heartbeat") {
                CHECK(callbackCallCount == 1);
                CHECK(receivedHeartbeat == NULL);
                CHECK(receivedContext == &testContext);
            }
        }

        WHEN("_noteHeartbeat is called multiple times") {
            const char *heartbeat1 = "first heartbeat";
            const char *heartbeat2 = "second heartbeat";

            _noteHeartbeat(heartbeat1);
            _noteHeartbeat(heartbeat2);

            THEN("The callback should be invoked each time") {
                CHECK(callbackCallCount == 2);
                CHECK(receivedHeartbeat == heartbeat2); // Last call
                CHECK(receivedContext == &testContext);
            }
        }
    }

    GIVEN("A heartbeat callback is registered without context") {
        NoteSetFnHeartbeat(mockHeartbeatCallback, NULL);
        resetCallbackState();

        WHEN("_noteHeartbeat is called") {
            const char *testHeartbeat = "no context test";
            _noteHeartbeat(testHeartbeat);

            THEN("The callback should be invoked with NULL context") {
                CHECK(callbackCallCount == 1);
                CHECK(receivedHeartbeat == testHeartbeat);
                CHECK(receivedContext == NULL);
            }
        }
    }
}

}

#endif // NOTE_C_HEARTBEAT_CALLBACK
