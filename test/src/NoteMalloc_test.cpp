/*!
 * @file NoteMalloc_test.cpp
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

extern mallocFn hookMalloc;

namespace
{

SCENARIO("NoteMalloc")
{
    GIVEN("hookMalloc is unset (NULL)") {
        hookMalloc = NULL;

        WHEN("NoteMalloc is called") {
            void *const result = NoteMalloc(1234);

            THEN("NULL is returned") {
                CHECK(NULL == result);
            }
        }
    }

    GIVEN("hookMalloc is set") {
        hookMalloc = [](size_t) -> void * {
            return reinterpret_cast<void *>(0x1234);
        };

        WHEN("NoteMalloc is called") {
            void *const result = NoteMalloc(1234);

            THEN("the value from hookMalloc is returned") {
                CHECK(reinterpret_cast<void *>(0x1234) == result);
            }
        }
    }
}

}
