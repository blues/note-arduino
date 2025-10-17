/*!
 * @file _errDoc_test.cpp
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

#include <cstddef>

DEFINE_FFF_GLOBALS

FAKE_VALUE_FUNC(J *, JCreateObject)
FAKE_VOID_FUNC(NoteDebugWithLevel, uint8_t, const char *)

namespace
{

SCENARIO("_errDoc")
{
    NoteSetFnDefault(malloc, free, NULL, NULL);

    GIVEN("Memory allocation failure") {
        JCreateObject_fake.return_val = nullptr;
        WHEN("_errDoc is called") {
            J *result = _errDoc(0, "error");
            THEN("it returns null") {
                CHECK(result == nullptr);
            }

            THEN("NoteDebugWithLevel is called with NOTE_C_LOG_LEVEL_ERROR") {
                CHECK(NoteDebugWithLevel_fake.call_count > 0);
                CHECK(NoteDebugWithLevel_fake.arg0_val == NOTE_C_LOG_LEVEL_ERROR);
            }
        }
    }

    GIVEN("Memory allocation success") {
        JCreateObject_fake.return_val = (J *)NoteMalloc(sizeof(J));
        REQUIRE(JCreateObject_fake.return_val != nullptr);
        memset(JCreateObject_fake.return_val, 0, sizeof(J));
        JCreateObject_fake.return_val->type = JObject;

        WHEN("Transactions have been suppressed") {
            _noteSuspendTransactionDebug();

            AND_WHEN("_errDoc is called") {
                J *result = _errDoc(123, "some error");

                THEN("No errors are logged to the console") {
                    REQUIRE(result != nullptr);

                    CHECK(NoteDebugWithLevel_fake.call_count == 0);
                }

                JDelete(result);
            }

            _noteResumeTransactionDebug();
        }

        WHEN("_errDoc is called") {
            J *result = _errDoc(123, "some error");

            THEN("it returns an object with a source field of `note-c`") {
                REQUIRE(result != nullptr);

                CHECK(strcmp(JGetString(result, "src"), "note-c") == 0);
            }

#ifndef NOTE_NODEBUG
            THEN("The error is logged to the console") {
                CHECK(NoteDebugWithLevel_fake.call_count > 0);
                CHECK(NoteDebugWithLevel_fake.arg0_val == NOTE_C_LOG_LEVEL_ERROR);
            }
#else
            THEN("No errors are logged to the console") {
                CHECK(NoteDebugWithLevel_fake.call_count == 0);
            }
#endif // NOTE_NODEBUG

            JDelete(result);
        }

        AND_GIVEN("A specific error message is provided") {
            const char *errorMessage = "a specific error message";

            WHEN("_errDoc is called") {
                J *result = _errDoc(123, errorMessage);

                THEN("it returns an object with the same error message") {
                    REQUIRE(result != nullptr);

                    CHECK(strcmp(JGetString(result, "err"), errorMessage) == 0);
                }

                JDelete(result);
            }

            AND_GIVEN("ID parameter value is zero") {
                const int id = 0;

                WHEN("_errDoc is called") {
                    J *result = _errDoc(id, errorMessage);

                    THEN("it returns a object without an id field") {
                        REQUIRE(result != nullptr);

                        CHECK(!JIsPresent(result, "id"));
                    }

                    JDelete(result);
                }
            }

            AND_GIVEN("ID parameter value is non-zero") {
                const int id = 79;

                WHEN("_errDoc is called") {
                    J *result = _errDoc(id, errorMessage);

                    THEN("it returns a object with an id field") {
                        REQUIRE(result != nullptr);

                        CHECK(JIsPresent(result, "id"));
                    }

                    JDelete(result);
                }
            }
        }
    }

    RESET_FAKE(JCreateObject);
    RESET_FAKE(NoteDebugWithLevel);
}

}
