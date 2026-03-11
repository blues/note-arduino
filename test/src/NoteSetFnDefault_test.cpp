/*!
 * @file NoteSetFnDefault_test.cpp
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
#include <fff.h>

#include "n_lib.h"

DEFINE_FFF_GLOBALS
FAKE_VOID_FUNC(_noteLockNote)
FAKE_VOID_FUNC(_noteUnlockNote)

extern mallocFn hookMalloc;
extern freeFn hookFree;
extern delayMsFn hookDelayMs;
extern getMsFn hookGetMs;

namespace
{

void* MyMalloc(size_t)
{
    return NULL;
}

void MyFree(void *)
{
}

void MyDelayMs(uint32_t)
{
}

uint32_t MyGetMs()
{
    return 0;
}

SCENARIO("NoteSetFnDefault")
{
    GIVEN("Hooks have not been set (i.e. NULL)") {
        hookMalloc = NULL;
        hookFree = NULL;
        hookDelayMs = NULL;
        hookGetMs = NULL;

        WHEN("NoteSetFnDefault is called") {
            NoteSetFnDefault(MyMalloc, MyFree, MyDelayMs, MyGetMs);

            THEN("The hooks are set") {
                CHECK(hookMalloc == MyMalloc);
                CHECK(hookFree == MyFree);
                CHECK(hookDelayMs == MyDelayMs);
                CHECK(hookGetMs == MyGetMs);
            }

            THEN("The Notecard lock is taken and released") {
                CHECK(1 == _noteLockNote_fake.call_count);
                CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
            }
        }
    }

    GIVEN("Hooks have been set with non-NULL values") {
        hookMalloc = MyMalloc;
        hookFree = MyFree;
        hookDelayMs = MyDelayMs;
        hookGetMs = MyGetMs;

        WHEN("NoteSetFnDefault is called") {
            NoteSetFnDefault(NULL, NULL, NULL, NULL);

            THEN("The existing hooks not overridden") {
                CHECK(hookMalloc == MyMalloc);
                CHECK(hookFree == MyFree);
                CHECK(hookDelayMs == MyDelayMs);
                CHECK(hookGetMs == MyGetMs);
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
