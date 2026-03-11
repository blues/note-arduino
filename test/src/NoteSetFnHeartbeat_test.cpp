/*!
 * @file NoteSetFnHeartbeat_test.cpp
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
FAKE_VOID_FUNC(_noteLockNote)
FAKE_VOID_FUNC(_noteUnlockNote)

extern heartbeatFn hookHeartbeat;
extern void *hookHeartbeatContext;

namespace
{

static int testContext = 42;
static const char *lastHeartbeat = nullptr;
static void *lastContext = nullptr;

bool mockHeartbeatFn(const char *heartbeatJson, void *context)
{
    lastHeartbeat = heartbeatJson;
    lastContext = context;
    return false;
}

SCENARIO("NoteSetFnHeartbeat")
{
    GIVEN("Heartbeat hook has not been set (i.e. NULL)") {
        hookHeartbeat = NULL;
        hookHeartbeatContext = NULL;

        WHEN("NoteSetFnHeartbeat is called with valid parameters") {
            NoteSetFnHeartbeat(mockHeartbeatFn, &testContext);

            THEN("The hooks are set") {
                CHECK(hookHeartbeat == mockHeartbeatFn);
                CHECK(hookHeartbeatContext == &testContext);
            }

            THEN("The Notecard lock is taken and released") {
                CHECK(1 == _noteLockNote_fake.call_count);
                CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
            }
        }

        WHEN("NoteSetFnHeartbeat is called with NULL function") {
            NoteSetFnHeartbeat(NULL, &testContext);

            THEN("The hooks are set to NULL/context") {
                CHECK(hookHeartbeat == NULL);
                CHECK(hookHeartbeatContext == &testContext);
            }
        }

        WHEN("NoteSetFnHeartbeat is called with NULL context") {
            NoteSetFnHeartbeat(mockHeartbeatFn, NULL);

            THEN("The hooks are set") {
                CHECK(hookHeartbeat == mockHeartbeatFn);
                CHECK(hookHeartbeatContext == NULL);
            }
        }
    }

    GIVEN("Heartbeat hook has been set with a non-NULL value") {
        hookHeartbeat = mockHeartbeatFn;
        hookHeartbeatContext = &testContext;

        WHEN("NoteSetFnHeartbeat is called with different parameters") {
            static int newContext = 99;
            NoteSetFnHeartbeat(NULL, &newContext);

            THEN("The existing hook is overridden") {
                CHECK(hookHeartbeat == NULL);
                CHECK(hookHeartbeatContext == &newContext);
            }

            THEN("The Notecard lock is taken and released") {
                CHECK(1 == _noteLockNote_fake.call_count);
                CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
            }
        }
    }

    RESET_FAKE(_noteLockNote);
    RESET_FAKE(_noteUnlockNote);
}

}

#endif // NOTE_C_HEARTBEAT_CALLBACK
