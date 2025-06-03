/*!
 * @file NoteFree_test.cpp
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

#include "n_lib.h"

extern freeFn hookFree;

namespace
{

void * free_ptr = NULL;

SCENARIO("NoteFree")
{
    GIVEN("hookFree is unset (NULL)") {
        hookFree = NULL;

        WHEN("NoteFree is called") {
            NoteFree(reinterpret_cast<void *>(0x1234));

            THEN("nothing happens") {
                SUCCEED();
            }
        }
    }

    GIVEN("hookFree is set") {
        hookFree = [](void * ptr_) {
            free_ptr = ptr_;
        };

        AND_GIVEN("a pointer") {
            void * ptr = reinterpret_cast<void *>(0x1234);

            WHEN("NoteFree is called") {
                NoteFree(ptr);

                THEN("the pointer is passed to hookFree") {
                    CHECK(free_ptr == ptr);
                }
            }
        }
    }
}

}
