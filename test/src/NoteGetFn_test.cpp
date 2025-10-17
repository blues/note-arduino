/*!
 * @file NoteGetFn_test.cpp
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

void MyDelayMs(uint32_t)
{
}

void MyFree(void *)
{
    return;
}

uint32_t MyGetMs(void)
{
    return 0;
}

void * MyMalloc(size_t)
{
    return NULL;
}

SCENARIO("NoteGetFn")
{
    GIVEN("Hooks have been set with non-NULL values") {
        hookMalloc = MyMalloc;
        hookFree = MyFree;
        hookDelayMs = MyDelayMs;
        hookGetMs = MyGetMs;

        WHEN("NoteGetFn is called") {
            NoteGetFn(NULL, NULL, NULL, NULL);

            THEN("It should not crash") {
                SUCCEED();
            }

            THEN("The Notecard lock is taken and released") {
                CHECK(1 == _noteLockNote_fake.call_count);
                CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
            }
        }

        AND_GIVEN("Valid empty pointers for all platform functions") {
            mallocFn mallocFunc = NULL;
            freeFn freeFunc = NULL;
            delayMsFn delayMs = NULL;
            getMsFn getMs = NULL;

            WHEN("NoteGetFn is called") {
                NoteGetFn(&mallocFunc, &freeFunc, &delayMs, &getMs);

                THEN("The hook functions are returned") {
                    CHECK(mallocFunc == MyMalloc);
                    CHECK(freeFunc == MyFree);
                    CHECK(delayMs == MyDelayMs);
                    CHECK(getMs == MyGetMs);
                }

                THEN("The Notecard lock is taken and released") {
                    CHECK(1 == _noteLockNote_fake.call_count);
                    CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the malloc function is provided") {
            mallocFn mallocFunc = NULL;

            WHEN("NoteGetFn is called") {
                NoteGetFn(&mallocFunc, NULL, NULL, NULL);

                THEN("The malloc function is returned") {
                    CHECK(mallocFunc == MyMalloc);
                }

                THEN("The Notecard lock is taken and released") {
                    CHECK(1 == _noteLockNote_fake.call_count);
                    CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the free function is provided") {
            freeFn freeFunc = NULL;

            WHEN("NoteGetFn is called") {
                NoteGetFn(NULL, &freeFunc, NULL, NULL);

                THEN("The free function is returned") {
                    CHECK(freeFunc == MyFree);
                }

                THEN("The Notecard lock is taken and released") {
                    CHECK(1 == _noteLockNote_fake.call_count);
                    CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the delay function is provided") {
            delayMsFn delayMs = NULL;

            WHEN("NoteGetFn is called") {
                NoteGetFn(NULL, NULL, &delayMs, NULL);

                THEN("The delay function is returned") {
                    CHECK(delayMs == MyDelayMs);
                }

                THEN("The Notecard lock is taken and released") {
                    CHECK(1 == _noteLockNote_fake.call_count);
                    CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the get milliseconds function is provided") {
            getMsFn getMs = NULL;

            WHEN("NoteGetFn is called") {
                NoteGetFn(NULL, NULL, NULL, &getMs);

                THEN("The get milliseconds function is returned") {
                    CHECK(getMs == MyGetMs);
                }

                THEN("The Notecard lock is taken and released") {
                    CHECK(1 == _noteLockNote_fake.call_count);
                    CHECK(_noteUnlockNote_fake.call_count == _noteLockNote_fake.call_count);
                }
            }
        }
    }

    RESET_FAKE(_noteLockNote);
    RESET_FAKE(_noteUnlockNote);
}

}
