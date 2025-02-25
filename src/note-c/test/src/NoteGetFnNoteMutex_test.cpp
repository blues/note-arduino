/*!
 * @file NoteGetFnNoteMutex_test.cpp
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

void mockLockNote(void)
{
    return;
}

void mockUnlockNote(void)
{
    return;
}

SCENARIO("NoteGetFnNoteMutex")
{
    GIVEN("A set of Note mutex functions") {
        NoteSetFnNoteMutex(mockLockNote, mockUnlockNote);

        AND_GIVEN("Valid pointers for all Note mutex functions are provided") {
            mutexFn lockNote = NULL;
            mutexFn unlockNote = NULL;

            WHEN("NoteGetFnNoteMutex is called") {
                NoteGetFnNoteMutex(&lockNote, &unlockNote);

                THEN("It should return the mock Note mutex functions") {
                    CHECK(lockNote == mockLockNote);
                    CHECK(unlockNote == mockUnlockNote);
                }
            }
        }

        AND_GIVEN("Only NULL pointers are provided") {
            WHEN("NoteGetFnNoteMutex is called") {
                NoteGetFnNoteMutex(NULL, NULL);
                THEN("It should not crash") {
                    SUCCEED();
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the Note lock function is provided") {
            mutexFn lockNote = NULL;

            WHEN("NoteGetFnNoteMutex is called") {
                NoteGetFnNoteMutex(&lockNote, NULL);

                THEN("It should return the mock lock function") {
                    CHECK(lockNote == mockLockNote);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the Note unlock function is provided") {
            mutexFn unlockNote = NULL;

            WHEN("NoteGetFnNoteMutex is called") {
                NoteGetFnNoteMutex(NULL, &unlockNote);

                THEN("It should return the mock unlock function") {
                    CHECK(unlockNote == mockUnlockNote);
                }
            }
        }
    }
}

}
