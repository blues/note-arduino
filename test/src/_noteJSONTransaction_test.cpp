/*!
 * @file _noteJSONTransaction_test.cpp
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

typedef const char * (*nTransactionFn) (const char *, size_t, char **, uint32_t); // MUST BE IDENTICAL TO THE FUNCTION POINTER TYPE IN `n_hooks.c`

extern volatile int hookActiveInterface;
extern nTransactionFn notecardTransaction;

namespace
{

const char * hookParameter_request;
size_t hookParameter_request_length;
const char ** hookParameter_response;
uint32_t hookParameter_timeout;

const char * const hookResult_response = "test";
const char * const hookResult = "random error msg";

SCENARIO("_noteJSONTransaction")
{
    GIVEN("hookActiveInterface is set to NOTE_C_INTERFACE_NONE") {
        hookActiveInterface = NOTE_C_INTERFACE_NONE;

        AND_GIVEN("notecardTransaction is unset (NULL)") {
            notecardTransaction = NULL;

            WHEN("_noteJSONTransaction is called") {
                const char * const result = _noteJSONTransaction(NULL, 0, NULL, 0);

                THEN("it returns \"a valid interface must be selected\"") {
                    CHECK(strcmp(result, "a valid interface must be selected") == 0);
                }
            }
        }

        AND_GIVEN("notecardTransaction is set") {
            notecardTransaction = [](const char *request_, size_t request_length_, char **response_, uint32_t timeout_) {
                hookParameter_request = request_;
                hookParameter_request_length = request_length_;
                hookParameter_response = const_cast<const char **>(response_);
                hookParameter_timeout = timeout_;

                *response_ = const_cast<char *>(hookResult_response);
                return hookResult;
            };

            WHEN("_noteJSONTransaction is called") {
                const char * const result = _noteJSONTransaction(NULL, 0, NULL, 0);

                THEN("it returns \"a valid interface must be selected\"") {
                    CHECK(strcmp(result, "a valid interface must be selected") == 0);
                }
            }
        }
    }

    GIVEN("hookActiveInterface is set to NOTE_C_INTERFACE_I2C") {
        hookActiveInterface = NOTE_C_INTERFACE_I2C;

        AND_GIVEN("notecardTransaction is unset (NULL)") {
            notecardTransaction = NULL;

            WHEN("_noteJSONTransaction is called") {
                const char * const result = _noteJSONTransaction(NULL, 0, NULL, 0);

                THEN("it returns \"a valid interface must be selected\"") {
                    CHECK(strcmp(result, "a valid interface must be selected") == 0);
                }
            }
        }

        AND_GIVEN("notecardTransaction is set") {
            notecardTransaction = [](const char *request_, size_t request_length_, char **response_, uint32_t timeout_) {
                hookParameter_request = request_;
                hookParameter_request_length = request_length_;
                hookParameter_response = const_cast<const char **>(response_);
                hookParameter_timeout = timeout_;

                *response_ = const_cast<char *>(hookResult_response);
                return hookResult;
            };

            AND_GIVEN("valid parameters") {
                const char * const request = "test";
                const size_t request_length = strlen(request);
                char * response = NULL;
                const uint32_t timeout = 17;

                WHEN("_noteJSONTransaction is called") {
                    const char * const result = _noteJSONTransaction(request, request_length, &response, timeout);

                    THEN("it calls notecardTransaction with the same parameters") {
                        CHECK(strcmp(request, hookParameter_request) == 0);
                        CHECK(request_length == hookParameter_request_length);
                        CHECK(&response == hookParameter_response);
                        CHECK(timeout == hookParameter_timeout);
                    }

                    THEN("it returns the results from notecardTransaction") {
                        CHECK(strcmp(result, hookResult) == 0);
                        CHECK(strcmp(response, hookResult_response) == 0);
                    }
                }
            }
        }
    }

    GIVEN("hookActiveInterface is set to NOTE_C_INTERFACE_SERIAL") {
        hookActiveInterface = NOTE_C_INTERFACE_SERIAL;

        AND_GIVEN("notecardTransaction is unset (NULL)") {
            notecardTransaction = NULL;

            WHEN("_noteJSONTransaction is called") {
                const char * const result = _noteJSONTransaction(NULL, 0, NULL, 0);

                THEN("it returns \"a valid interface must be selected\"") {
                    CHECK(strcmp(result, "a valid interface must be selected") == 0);
                }
            }
        }

        AND_GIVEN("notecardTransaction is set") {
            notecardTransaction = [](const char *request_, size_t request_length_, char **response_, uint32_t timeout_) {
                hookParameter_request = request_;
                hookParameter_request_length = request_length_;
                hookParameter_response = const_cast<const char **>(response_);
                hookParameter_timeout = timeout_;

                *response_ = const_cast<char *>(hookResult_response);
                return hookResult;
            };

            AND_GIVEN("valid parameters") {
                const char * const request = "test";
                const size_t request_length = strlen(request);
                char * response = NULL;
                const uint32_t timeout = 17;

                WHEN("_noteJSONTransaction is called") {
                    const char * const result = _noteJSONTransaction(request, request_length, &response, timeout);

                    THEN("it calls notecardTransaction with the same parameters") {
                        CHECK(strcmp(request, hookParameter_request) == 0);
                        CHECK(request_length == hookParameter_request_length);
                        CHECK(&response == hookParameter_response);
                        CHECK(timeout == hookParameter_timeout);
                    }

                    THEN("it returns the results from notecardTransaction") {
                        CHECK(strcmp(result, hookResult) == 0);
                        CHECK(strcmp(response, hookResult_response) == 0);
                    }
                }
            }
        }
    }
}

}
