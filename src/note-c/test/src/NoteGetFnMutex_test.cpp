/*!
 * @file NoteGetFnMutex_test.cpp
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

#include "n_lib.h"

namespace
{

void mockLockI2C(void)
{
    return;
}

void mockUnlockI2C(void)
{
    return;
}

void mockLockNote(void)
{
    return;
}

void mockUnlockNote(void)
{
    return;
}

SCENARIO("NoteGetFnMutex")
{
    GIVEN("A set of mutex functions") {
        NoteSetFnMutex(mockLockI2C, mockUnlockI2C, mockLockNote, mockUnlockNote);

        AND_GIVEN("Valid pointers for all mutex functions are provided") {
            mutexFn lockI2C = NULL;
            mutexFn unlockI2C = NULL;
            mutexFn lockNote = NULL;
            mutexFn unlockNote = NULL;

            WHEN("NoteGetFnMutex is called") {
                NoteGetFnMutex(&lockI2C, &unlockI2C, &lockNote, &unlockNote);

                THEN("It should return the mock mutex functions") {
                    CHECK(lockI2C == mockLockI2C);
                    CHECK(unlockI2C == mockUnlockI2C);
                    CHECK(lockNote == mockLockNote);
                    CHECK(unlockNote == mockUnlockNote);
                }
            }
        }

        AND_GIVEN("Only NULL pointers are provided") {
            WHEN("NoteGetFnMutex is called") {
                NoteGetFnMutex(NULL, NULL, NULL, NULL);

                THEN("It should not crash") {
                    SUCCEED();
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the I2C lock function is provided") {
            mutexFn lockI2C = NULL;

            WHEN("NoteGetFnMutex is called") {
                NoteGetFnMutex(&lockI2C, NULL, NULL, NULL);

                THEN("It should return the mock lock function") {
                    CHECK(lockI2C == mockLockI2C);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the I2C unlock function is provided") {
            mutexFn unlockI2C = NULL;

            WHEN("NoteGetFnMutex is called") {
                NoteGetFnMutex(NULL, &unlockI2C, NULL, NULL);

                THEN("It should return the mock unlock function") {
                    CHECK(unlockI2C == mockUnlockI2C);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the Note lock function is provided") {
            mutexFn lockNote = NULL;

            WHEN("NoteGetFnMutex is called") {
                NoteGetFnMutex(NULL, NULL, &lockNote, NULL);

                THEN("It should return the mock lock function") {
                    CHECK(lockNote == mockLockNote);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the Note unlock function is provided") {
            mutexFn unlockNote = NULL;

            WHEN("NoteGetFnMutex is called") {
                NoteGetFnMutex(NULL, NULL, NULL, &unlockNote);

                THEN("It should return the mock unlock function") {
                    CHECK(unlockNote == mockUnlockNote);
                }
            }
        }
    }
}

}
