/*!
 * @file NoteGetFnI2CMutex_test.cpp
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

SCENARIO("NoteGetFnI2CMutex")
{
    GIVEN("A set of I2C mutex functions") {
        NoteSetFnI2CMutex(mockLockI2C, mockUnlockI2C);

        AND_GIVEN("Valid pointers for all I2C mutex functions are provided") {
            mutexFn lockI2C = NULL;
            mutexFn unlockI2C = NULL;

            WHEN("NoteGetFnI2CMutex is called") {
                NoteGetFnI2CMutex(&lockI2C, &unlockI2C);

                THEN("It should return the mock I2C mutex functions") {
                    CHECK(lockI2C == mockLockI2C);
                    CHECK(unlockI2C == mockUnlockI2C);
                }
            }
        }

        AND_GIVEN("Only NULL pointers are provided") {
            WHEN("NoteGetFnI2CMutex is called") {
                NoteGetFnI2CMutex(NULL, NULL);

                THEN("It should not crash") {
                    SUCCEED();
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the I2C lock function is provided") {
            mutexFn lockI2C = NULL;

            WHEN("NoteGetFnI2CMutex is called") {
                NoteGetFnI2CMutex(&lockI2C, NULL);

                THEN("It should return the mock I2C lock function") {
                    CHECK(lockI2C == mockLockI2C);
                }
            }
        }

        AND_GIVEN("Only a valid pointer for the I2C unlock function is provided") {
            mutexFn unlockI2C = NULL;

            WHEN("NoteGetFnI2CMutex is called") {
                NoteGetFnI2CMutex(NULL, &unlockI2C);

                THEN("It should return the mock I2C unlock function") {
                    CHECK(unlockI2C == mockUnlockI2C);
                }
            }
        }
    }
}

}
