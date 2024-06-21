/*!
 * @file NoteRequestResponseWithRetry_test.cpp
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
FAKE_VALUE_FUNC(J *, NoteTransaction, J *)
FAKE_VALUE_FUNC(uint32_t, NoteGetMs)

namespace
{

J *NoteTransactionIOError(J *)
{
    J *resp = JCreateObject();
    assert(resp != NULL);
    JAddStringToObject(resp, c_err, c_ioerr);

    return resp;
}

J *NoteTransactionNotSupportedError(J *)
{
    J *resp = JCreateObject();
    assert(resp != NULL);
    JAddStringToObject(resp, c_err, c_unsupported);

    return resp;
}

J *NoteTransactionIOAndNotSupportedErrors(J *)
{
    J *resp = JCreateObject();
    assert(resp != NULL);
    JAddStringToObject(resp, c_err, "{io} {not-supported}");

    return resp;
}

J *NoteTransactionOtherError(J *)
{
    J *resp = JCreateObject();
    assert(resp != NULL);
    JAddStringToObject(resp, c_err, c_bad);

    return resp;
}

TEST_CASE("NoteRequestResponseWithRetry")
{
    NoteSetFnDefault(malloc, free, NULL, NULL);

    J *rsp = NULL;

    GIVEN("A NULL request") {
        J *req = NULL;

        WHEN("NoteRequestResponseWithRetry is called") {
            rsp = NoteRequestResponseWithRetry(req, 0);

            THEN("The returned response is NULL") {
                CHECK(rsp == NULL);
            }
        }
    }

    GIVEN("A valid request") {
        J *req = NoteNewRequest("note.add");
        REQUIRE(req != NULL);

        AND_GIVEN("A timeout of 5 seconds") {
            const uint32_t timeoutSec = 5;

            AND_GIVEN("The timeout will trigger after 1 retry") {
                uint32_t getMsReturnVals[3] = {0, 3000, 6000};
                SET_RETURN_SEQ(NoteGetMs, getMsReturnVals, 3);

                AND_GIVEN("The response to NoteTransaction is NULL") {
                    NoteTransaction_fake.return_val = NULL;

                    WHEN("NoteRequestResponseWithRetry is called") {
                        rsp = NoteRequestResponseWithRetry(req, timeoutSec);

                        THEN("The returned response is NULL") {
                            CHECK(rsp == NULL);
                        }

                        THEN("NoteTransaction will be called twice (1 retry)")  {
                            CHECK(NoteTransaction_fake.call_count == 2);
                        }
                    }
                }

                AND_GIVEN("The response to NoteTransaction contains an I/O "
                          "error") {
                    NoteTransaction_fake.custom_fake = NoteTransactionIOError;

                    WHEN("NoteRequestResponseWithRetry is called") {
                        rsp = NoteRequestResponseWithRetry(req, timeoutSec);

                        THEN("The returned response is NULL") {
                            CHECK(rsp == NULL);
                        }

                        THEN("NoteTransaction will be called twice (1 "
                             "retry)")  {
                            CHECK(NoteTransaction_fake.call_count == 2);
                        }
                    }
                }

                AND_GIVEN("The response to NoteTransaction contains a not "
                          "supported error") {
                    NoteTransaction_fake.custom_fake = NoteTransactionNotSupportedError;

                    WHEN("NoteRequestResponseWithRetry is called") {
                        rsp = NoteRequestResponseWithRetry(req, timeoutSec);

                        THEN("The returned response has a not-supported error") {
                            CHECK(JContainsString(rsp, c_err, c_unsupported));
                        }

                        THEN("NoteTransaction is only called once (no retries)")  {
                            CHECK(NoteTransaction_fake.call_count == 1);
                        }
                    }
                }

                AND_GIVEN("The response to NoteTransaction contains a not "
                          "supported error AND an I/O error") {
                    NoteTransaction_fake.custom_fake = NoteTransactionIOAndNotSupportedErrors;

                    WHEN("NoteRequestResponseWithRetry is called") {
                        rsp = NoteRequestResponseWithRetry(req, timeoutSec);

                        THEN("The returned response has a not-supported "
                             "error") {
                            CHECK(JContainsString(rsp, c_err, c_unsupported));
                        }

                        THEN("The returned response has an I/O error") {
                            CHECK(JContainsString(rsp, c_err, c_ioerr));
                        }

                        THEN("NoteTransaction is only called once (no "
                             "retries)")  {
                            CHECK(NoteTransaction_fake.call_count == 1);
                        }
                    }
                }

                AND_GIVEN("The response to NoteTransaction contains an error "
                          "that isn't I/O or \"not supported\"") {
                    NoteTransaction_fake.custom_fake = NoteTransactionOtherError;

                    WHEN("NoteRequestResponseWithRetry is called") {
                        rsp = NoteRequestResponseWithRetry(req, timeoutSec);

                        THEN("The returned response has the error") {
                            CHECK(JContainsString(rsp, c_err, c_bad));
                        }

                        THEN("NoteTransaction is only called once (no "
                             "retries)")  {
                            CHECK(NoteTransaction_fake.call_count == 1);
                        }
                    }
                }

                AND_GIVEN("There's a valid response on the first "
                          " NoteTransaction attempt") {
                    NoteTransaction_fake.return_val = JCreateObject();

                    WHEN("NoteRequestResponseWithRetry is called") {
                        rsp = NoteRequestResponseWithRetry(req, timeoutSec);

                        THEN("The returned response is non-NULL") {
                            CHECK(rsp != NULL);
                        }

                        THEN("NoteTranaction is only called once (no "
                             "retries)") {
                            CHECK(NoteTransaction_fake.call_count == 1);
                        }
                    }
                }

                AND_GIVEN("There's a valid response on the second "
                          "NoteTransaction attempt") {
                    J *noteTransactionReturnVals[2] = {NULL, JCreateObject()};
                    SET_RETURN_SEQ(NoteTransaction, noteTransactionReturnVals,
                                   2);

                    WHEN("NoteRequestResponseWithRetry is called") {
                        rsp = NoteRequestResponseWithRetry(req, timeoutSec);

                        THEN("The returned response is non-NULL") {
                            CHECK(rsp != NULL);
                        }

                        THEN("NoteTranaction is only called twice (1 retry)") {
                            CHECK(NoteTransaction_fake.call_count == 2);
                        }
                    }
                }
            }
        }
    }

    JDelete(rsp);

    RESET_FAKE(NoteTransaction);
    RESET_FAKE(NoteGetMs);
}

}
