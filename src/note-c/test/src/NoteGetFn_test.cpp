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
#include "fff.h"

#include "n_lib.h"

DEFINE_FFF_GLOBALS

namespace
{

void mockDelayMs(uint32_t ms)
{
    (void)ms;
}

void mockFree(void *mem)
{
    (void)mem;
    return;
}

uint32_t mockGetMs(void)
{
    return 0;
}

void * mockMalloc(size_t size)
{
    (void)size;
    return NULL;
}

SCENARIO("NoteGetFn")
{
    GIVEN("A set of hook functions") {
        NoteSetFn(mockMalloc, mockFree, mockDelayMs, mockGetMs);

        AND_GIVEN("Valid pointers for all platform functions are provided") {
            mallocFn mallocFunc = NULL;
            freeFn freeFunc = NULL;
            delayMsFn delayMs = NULL;
            getMsFn getMs = NULL;

            WHEN("NoteGetFn is called") {
                NoteGetFn(&mallocFunc, &freeFunc, &delayMs, &getMs);

                THEN("The hook functions are returned") {
                    CHECK(mallocFunc == mockMalloc);
                    CHECK(freeFunc == mockFree);
                    CHECK(delayMs == mockDelayMs);
                    CHECK(getMs == mockGetMs);
                }
            }
        }

        AND_GIVEN("Only NULL pointers are provided") {
            WHEN("NoteGetFn is called") {
                NoteGetFn(NULL, NULL, NULL, NULL);

                THEN("It should not crash") {
                    SUCCEED();
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the malloc function is provided") {
            mallocFn mallocFunc = NULL;

            WHEN("NoteGetFn is called") {
                NoteGetFn(&mallocFunc, NULL, NULL, NULL);

                THEN("The malloc function is returned") {
                    CHECK(mallocFunc == mockMalloc);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the free function is provided") {
            freeFn freeFunc = NULL;

            WHEN("NoteGetFn is called") {
                NoteGetFn(NULL, &freeFunc, NULL, NULL);

                THEN("The free function is returned") {
                    CHECK(freeFunc == mockFree);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the delay function is provided") {
            delayMsFn delayMs = NULL;

            WHEN("NoteGetFn is called") {
                NoteGetFn(NULL, NULL, &delayMs, NULL);

                THEN("The delay function is returned") {
                    CHECK(delayMs == mockDelayMs);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the get milliseconds function is provided") {
            getMsFn getMs = NULL;

            WHEN("NoteGetFn is called") {
                NoteGetFn(NULL, NULL, NULL, &getMs);

                THEN("The get milliseconds function is returned") {
                    CHECK(getMs == mockGetMs);
                }
            }
        }
    }
}
}
