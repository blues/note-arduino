/*!
 * @file NoteTransaction_test.cpp
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
FAKE_VALUE_FUNC(char *, _crcAdd, char *, uint16_t)
FAKE_VALUE_FUNC(bool, _crcError, char *, uint16_t)
FAKE_VALUE_FUNC(bool, _noteHardReset)
FAKE_VALUE_FUNC(const char *, _noteJSONTransaction, const char *, size_t, char **, uint32_t)
FAKE_VALUE_FUNC(bool, _noteTransactionStart, uint32_t)
FAKE_VOID_FUNC(NoteDebugWithLevel, uint8_t, const char *)
FAKE_VOID_FUNC(NoteDelayMs, uint32_t)
FAKE_VALUE_FUNC(bool, NoteReset)
FAKE_VALUE_FUNC(J *, NoteUserAgent)

namespace
{

const char *_noteJSONTransactionValid(const char *, size_t, char **resp, uint32_t)
{
    static char respString[] = "{ \"total\": 1 }";

    if (resp) {
        char* respBuf = reinterpret_cast<char *>(malloc(sizeof(respString)));
        memcpy(respBuf, respString, sizeof(respString));
        *resp = respBuf;
    }

    return NULL;
}

const char *_noteJSONTransactionBadJSON(const char *, size_t, char **resp, uint32_t)
{
    static char respString[] = "Bad JSON";

    if (resp) {
        char* respBuf = reinterpret_cast<char *>(malloc(sizeof(respString)));
        memcpy(respBuf, respString, sizeof(respString));
        *resp = respBuf;
    }

    return NULL;
}

const char *_noteJSONTransactionIOError(const char *, size_t, char **resp, uint32_t)
{
    static char respString[] = "{\"err\": \"{io}\"}";

    if (resp) {
        char* respBuf = reinterpret_cast<char *>(malloc(sizeof(respString)));
        memcpy(respBuf, respString, sizeof(respString));
        *resp = respBuf;
    }

    return NULL;
}

const char *_noteJSONTransactionNotSupportedError(const char *, size_t, char **resp, uint32_t)
{
    static char respString[] = "{\"err\": \"{not-supported}\"}";

    if (resp) {
        char* respBuf = reinterpret_cast<char *>(malloc(sizeof(respString)));
        memcpy(respBuf, respString, sizeof(respString));
        *resp = respBuf;
    }

    return NULL;
}

char * _crcAdd_customFake(char *json, uint16_t seqno)
{
    // Custom fake implementation for _crcAdd
    return strdup(json);
}

SCENARIO("NoteTransaction")
{
    NoteSetFnDefault(malloc, free, NULL, NULL);
    _crcAdd_fake.custom_fake = _crcAdd_customFake;

    // NoteReset's mock should succeed unless the test explicitly instructs
    // it to fail.
    NoteReset_fake.return_val = true;
    _noteTransactionStart_fake.return_val = true;
    _crcError_fake.return_val = false;

    SECTION("Passing a NULL request returns NULL") {
        CHECK(NoteTransaction(NULL) == NULL);
    }

    SECTION("Passing a request with neither a command nor a request returns NULL") {
        const char malformedRequest[] = "{\"foo\":\"bar\"}";
        J *req = JParse(malformedRequest);

        CHECK(NoteTransaction(req) == NULL);

        JDelete(req);
    }

    SECTION("Passing a request with both a command and a request returns NULL") {
        const char malformedRequest[] = "{\"cmd\":\"foo\",\"req\":\"bar\"}";
        J *req = JParse(malformedRequest);

        CHECK(NoteTransaction(req) == NULL);

        JDelete(req);
    }

    SECTION("NULL returned when _noteTransactionStart fails on a command") {
        _noteTransactionStart_fake.return_val = false;
        J *req = NoteNewCommand("note.add");
        REQUIRE(req != NULL);

        CHECK(NoteTransaction(req) == NULL);

        JDelete(req);
    }

    SECTION("Error returned when _noteTransactionStart fails on a request") {
        _noteTransactionStart_fake.return_val = false;
        J *req = NoteNewRequest("note.add");
        REQUIRE(req != NULL);

        J* rsp = NoteTransaction(req);
        CHECK(rsp != NULL);
        CHECK(NoteResponseError(rsp));

        JDelete(req);
        JDelete(rsp);
    }

    SECTION("When transactions are suppressed, NoteDebugWithLevel is not called so long as a reset is not required") {
        resetRequired = false; // Ensure resetRequired is false

        J *req = NoteNewRequest("note.add");
        REQUIRE(req != NULL);
        _noteJSONTransaction_fake.custom_fake = _noteJSONTransactionValid;
        NoteSuspendTransactionDebug();

        J *resp = _noteTransactionShouldLock(req, false);

        NoteResumeTransactionDebug();
        CHECK(_noteJSONTransaction_fake.call_count == 1);
        CHECK(resp != NULL);

        // Ensure there's no error in the response.
        CHECK(!NoteResponseError(resp));

        // Ensure NoteDebugWithLevel was not called
        CHECK(NoteDebugWithLevel_fake.call_count == 0);

        JDelete(req);
        JDelete(resp);
    }

    SECTION("A response is expected and the response is valid with the Notecard unlocked") {
        J *req = NoteNewRequest("note.add");
        REQUIRE(req != NULL);
        _noteJSONTransaction_fake.custom_fake = _noteJSONTransactionValid;

        J *resp = _noteTransactionShouldLock(req, false);

        CHECK(_noteJSONTransaction_fake.call_count == 1);
        CHECK(resp != NULL);
        // Ensure there's no error in the response.
        CHECK(!NoteResponseError(resp));

        JDelete(req);
        JDelete(resp);
    }

    SECTION("A response is expected and the response is valid with the Notecard locked") {
        J *req = NoteNewRequest("note.add");
        REQUIRE(req != NULL);
        _noteJSONTransaction_fake.custom_fake = _noteJSONTransactionValid;

        J *resp = NoteTransaction(req);

        CHECK(_noteJSONTransaction_fake.call_count == 1);
        CHECK(resp != NULL);
        // Ensure there's no error in the response.
        CHECK(!NoteResponseError(resp));

        JDelete(req);
        JDelete(resp);
    }

    SECTION("A response is expected and the response has an I/O error") {
        J *req = NoteNewRequest("note.add");
        REQUIRE(req != NULL);
        _noteJSONTransaction_fake.return_val = "This is an I/O error. {io}";

        J *resp = NoteTransaction(req);

        // Ensure the mock is called at least once
        // Here the error causes multiple invocations by retries
        CHECK(_noteJSONTransaction_fake.call_count >= 1);

        // Ensure there's an error in the response.
        CHECK(resp != NULL);
        CHECK(NoteResponseError(resp));

        // Ensure the error is an I/O error.
        CHECK(NoteResponseErrorContains(resp, "{io}"));

        // NoteDelayMs called to allow for each retry
        CHECK(NoteDelayMs_fake.call_count > CARD_REQUEST_RETRIES_ALLOWED);

        // _noteHardReset called to allow for each retry
        CHECK(_noteHardReset_fake.call_count > CARD_REQUEST_RETRIES_ALLOWED);

        JDelete(req);
        JDelete(resp);
    }

    SECTION("A response is expected and the response has an error") {
        J *req = NoteNewRequest("note.add");
        REQUIRE(req != NULL);
        _noteJSONTransaction_fake.return_val = "This is an error.";

        J *resp = NoteTransaction(req);

        // Ensure the mock is called at least once
        // Here the error causes multiple invocations by retries
        CHECK(_noteJSONTransaction_fake.call_count >= 1);

        // Ensure there's an error in the response.
        CHECK(resp != NULL);
        CHECK(NoteResponseError(resp));

        JDelete(req);
        JDelete(resp);
    }

    WHEN("The transaction is successful and the response has a {bad-bin} error") {
        J *req = NoteNewRequest("note.add");
        REQUIRE(req != NULL);
        _noteJSONTransaction_fake.custom_fake = [](const char *, size_t, char **response, uint32_t) -> const char * {
            const char rsp_str[] = "{\"err\":\"{bad-bin}\"}";
            *response = (char *)malloc(sizeof(rsp_str));
            strncpy(*response, rsp_str, sizeof(rsp_str));
            return nullptr;
        };

        J *resp = NoteTransaction(req);

        // Ensure the mock is called at least once
        // Here the error causes multiple invocations by retries
        REQUIRE(_noteJSONTransaction_fake.call_count >= 1);

        THEN("An error is returned") {
            CHECK(resp != NULL);
        }

        THEN("The transaction is not retried") {
            CHECK(_noteJSONTransaction_fake.call_count == 1);
        }

        JDelete(req);
        JDelete(resp);
    }

    WHEN("The transaction is successful and the response contains invalid JSON") {
        J *req = NoteNewRequest("note.add");
        REQUIRE(req != NULL);
        _noteJSONTransaction_fake.custom_fake = [](const char *, size_t, char **response, uint32_t) -> const char * {
            const char rsp_str[] = "{Looks like JSON, but won't parse}";
            *response = (char *)malloc(sizeof(rsp_str));
            strncpy(*response, rsp_str, sizeof(rsp_str));
            return nullptr;
        };

        J *resp = NoteTransaction(req);

        // Ensure the mock is called at least once
        // Here the error causes multiple invocations by retries
        REQUIRE(_noteJSONTransaction_fake.call_count >= 1);

        THEN("An error is returned") {
            CHECK(resp != NULL);
        }

        THEN("The transaction is retried") {
            CHECK(_noteJSONTransaction_fake.call_count == (1 + CARD_REQUEST_RETRIES_ALLOWED));
        }

        JDelete(req);
        JDelete(resp);
    }

    WHEN("The transaction is successful, and contains UTF-8 encoded characters in valid JSON") {
        J* req = NoteNewRequest("card.time");
        REQUIRE(req != NULL);
        _noteJSONTransaction_fake.custom_fake = [] (const char *, size_t, char **response, uint32_t) -> const char * {
            const char rsp_str[] = "{\"minutes\":-300,\"lat\":19.432608,\"lon\":-99.133122,\"area\":\"Ciudad de México\",\"country\":\"MX\",\"zone\":\"CDT,America/Mexico_City\",\"time\":1726006340}";
            *response = (char *)malloc(sizeof(rsp_str));
            strncpy(*response, rsp_str, sizeof(rsp_str));
            return nullptr;
        };
        J* resp = NoteTransaction(req);
        THEN("An error is not returned") {
            CHECK(resp != NULL);
            CHECK(!NoteResponseError(resp));
        }

        JDelete(req);
        JDelete(resp);
    }

    WHEN("The CRC addition fails, but the transaction is still successful") {
        J* req = NoteNewRequest("card.time");
        REQUIRE(req != NULL);
        _noteJSONTransaction_fake.custom_fake = [] (const char *, size_t, char **response, uint32_t) -> const char * {
            const char rsp_str[] = "{\"minutes\":-300,\"lat\":19.432608,\"lon\":-99.133122,\"area\":\"Ciudad de México\",\"country\":\"MX\",\"zone\":\"CDT,America/Mexico_City\",\"time\":1726006340}";
            *response = (char *)malloc(sizeof(rsp_str));
            strncpy(*response, rsp_str, sizeof(rsp_str));
            return nullptr;
        };
        _crcAdd_fake.custom_fake = nullptr;
        _crcAdd_fake.return_val = nullptr; // Simulate CRC failure by not adding it

        J* resp = NoteTransaction(req);
        THEN("An error is not returned") {
            CHECK(resp != NULL);
            CHECK(!NoteResponseError(resp));
        }

        JDelete(req);
        JDelete(resp);
    }

    WHEN("The transaction is successful, and contains UTF-16 encoded characters in valid JSON") {
        J* req = NoteNewRequest("card.time");
        REQUIRE(req != NULL);
        _noteJSONTransaction_fake.custom_fake = [] (const char *, size_t, char **response, uint32_t) -> const char * {
            const char16_t rsp_str[] = u"{\"minutes\":-300,\"lat\":19.432608,\"lon\":-99.133122,\"area\":\"Ciudad de México\",\"country\":\"MX\",\"zone\":\"CDT,America/Mexico_City\",\"time\":1726006340}";
            *response = (char *)malloc(sizeof(rsp_str));
            memcpy(*response, rsp_str, sizeof(rsp_str));
            return nullptr;
        };
        J* resp = NoteTransaction(req);
        THEN("An {io} error is returned") {
            CHECK(resp != NULL);
            CHECK(NoteResponseError(resp));
            CHECK(JContainsString(resp, "err", "{io}"));
        }

        JDelete(req);
        JDelete(resp);
    }

#ifndef NOTE_C_LOW_MEM
    SECTION("Bad CRC") {
        J *req = NoteNewRequest("note.add");
        REQUIRE(req != NULL);
        _noteJSONTransaction_fake.custom_fake = _noteJSONTransactionValid;
        _crcError_fake.return_val = true;

        J *resp = NoteTransaction(req);

        CHECK(resp != NULL);
        CHECK(NoteResponseError(resp));

        JDelete(req);
        JDelete(resp);
    }
#endif // !NOTE_C_LOW_MEM

    SECTION("I/O error") {
        J *req = NoteNewRequest("note.add");
        REQUIRE(req != NULL);
        _noteJSONTransaction_fake.custom_fake = _noteJSONTransactionIOError;

        J *resp = NoteTransaction(req);

        CHECK(resp != NULL);
        CHECK(NoteResponseError(resp));

        JDelete(req);
        JDelete(resp);
    }

    GIVEN("A valid request") {
        J *req = NoteNewRequest("note.add");
        REQUIRE(req != NULL);

        AND_GIVEN("_noteJSONTransaction returns a response with a \"not "
                  "supported\" error") {
            _noteJSONTransaction_fake.custom_fake = _noteJSONTransactionNotSupportedError;

            WHEN("NoteTransaction is called") {
                J *rsp = NoteTransaction(req);

                THEN("The response contains the \"not supported\" error") {
                    CHECK(JContainsString(rsp, c_err, c_unsupported));
                }

                THEN("_noteJSONTransaction is only called once (no retries)") {
                    CHECK(_noteJSONTransaction_fake.call_count == 1);
                }

                JDelete(rsp);
            }
        }

        JDelete(req);
    }

    SECTION("A reset is required and it fails") {
        J *req = NoteNewCommand("note.add");
        REQUIRE(req != NULL);
        NoteResetRequired();
        // Force NoteReset failure.
        NoteReset_fake.return_val = false;

        J *resp = NoteTransaction(req);
        REQUIRE(NoteReset_fake.call_count == 1);

        // The transaction shouldn't be attempted if reset failed.
        CHECK(_noteJSONTransaction_fake.call_count == 0);

        // The response should be null if reset failed.
        CHECK(resp == NULL);

        JDelete(req);
    }

    SECTION("A reset is required and it fails") {
        J *req = NoteNewRequest("note.add");
        REQUIRE(req != NULL);
        NoteResetRequired();
        // Force NoteReset failure.
        NoteReset_fake.return_val = false;

        J *resp = NoteTransaction(req);
        REQUIRE(NoteReset_fake.call_count == 1);

        // The transaction shouldn't be attempted if reset failed.
        CHECK(_noteJSONTransaction_fake.call_count == 0);

        // The response should be null if reset failed.
        CHECK(resp != NULL);
        CHECK(NoteResponseError(resp));

        JDelete(req);
        JDelete(resp);
    }

    SECTION("Serializing the JSON request fails when Notecard is unlocked") {
        // Create an invalid J object.
        J *req = reinterpret_cast<J *>(malloc(sizeof(J)));
        REQUIRE(req != NULL);
        memset(req, 0, sizeof(J));

        J *resp = _noteTransactionShouldLock(req, false);

        // The transaction shouldn't be attempted if the request couldn't be
        // serialized.
        CHECK(_noteJSONTransaction_fake.call_count == 0);

        // Ensure there's no error in the response.
        CHECK(resp == NULL);

        JDelete(req);
        JDelete(resp);
    }

    SECTION("Serializing the JSON request fails when Notecard is locked") {
        // Create an invalid J object.
        J *req = reinterpret_cast<J *>(malloc(sizeof(J)));
        REQUIRE(req != NULL);
        memset(req, 0, sizeof(J));

        J *resp = NoteTransaction(req);

        // The transaction shouldn't be attempted if the request couldn't be
        // serialized.
        CHECK(_noteJSONTransaction_fake.call_count == 0);

        // Ensure there's no error in the response.
        CHECK(resp == NULL);

        JDelete(req);
        JDelete(resp);
    }

    SECTION("No response is expected") {
        J *req = NoteNewCommand("note.add");
        REQUIRE(req != NULL);
        _noteJSONTransaction_fake.custom_fake = _noteJSONTransactionValid;

        J *resp = NoteTransaction(req);

        CHECK(_noteJSONTransaction_fake.call_count == 1);
        CHECK(resp != NULL);
        // Ensure there's no error in the response.
        CHECK(!NoteResponseError(resp));
        // Ensure a blank object was returned.
        J *expectedResp = JCreateObject();
        CHECK(JCompare(resp, expectedResp, true));

        JDelete(req);
        JDelete(resp);
        JDelete(expectedResp);
    }

    SECTION("Parsing the JSON response fails") {
        J *req = NoteNewRequest("note.add");
        REQUIRE(req != NULL);
        _noteJSONTransaction_fake.custom_fake = _noteJSONTransactionBadJSON;

        J *resp = NoteTransaction(req);

        CHECK(_noteJSONTransaction_fake.call_count >= 1);
        CHECK(resp != NULL);

        // Ensure there's an error in the response.
        CHECK(NoteResponseError(resp));

        JDelete(req);
        JDelete(resp);
    }

#ifndef NOTE_DISABLE_USER_AGENT
    SECTION("hub.set command with product adds user agent information") {
        J *req = NoteNewCommand("hub.set");
        REQUIRE(req != NULL);
        JAddStringToObject(req, "product", "a.b.c:d");
        _noteJSONTransaction_fake.custom_fake = _noteJSONTransactionValid;
        NoteUserAgent_fake.return_val = JCreateObject();

        J *resp = NoteTransaction(req);
        CHECK(resp != NULL);
        CHECK(NoteUserAgent_fake.call_count > 0);

        JDelete(req);
        JDelete(resp);
    }

    SECTION("hub.set request with product adds user agent information") {
        J *req = NoteNewRequest("hub.set");
        REQUIRE(req != NULL);
        JAddStringToObject(req, "product", "a.b.c:d");
        _noteJSONTransaction_fake.custom_fake = _noteJSONTransactionValid;
        NoteUserAgent_fake.return_val = JCreateObject();

        J *resp = NoteTransaction(req);
        CHECK(resp != NULL);
        CHECK(NoteUserAgent_fake.call_count > 0);

        JDelete(req);
        JDelete(resp);
    }

    SECTION("NoteUserAgent returns NULL during hub.set request with product") {
        J *req = NoteNewRequest("hub.set");
        REQUIRE(req != NULL);
        JAddStringToObject(req, "product", "a.b.c:d");
        _noteJSONTransaction_fake.custom_fake = _noteJSONTransactionValid;
        NoteUserAgent_fake.return_val = NULL;

        J *resp = NoteTransaction(req);
        REQUIRE(NoteUserAgent_fake.call_count > 0);

        CHECK(resp != NULL);

        JDelete(req);
        JDelete(resp);
    }

    SECTION("hub.set command without product does not add user agent information") {
        J *req = NoteNewCommand("hub.set");
        REQUIRE(req != NULL);
        _noteJSONTransaction_fake.custom_fake = _noteJSONTransactionValid;

        J *resp = NoteTransaction(req);
        CHECK(resp != NULL);
        CHECK(NoteUserAgent_fake.call_count == 0);

        JDelete(req);
        JDelete(resp);
    }

    SECTION("hub.set request without product does not add user agent information") {
        J *req = NoteNewRequest("hub.set");
        REQUIRE(req != NULL);
        _noteJSONTransaction_fake.custom_fake = _noteJSONTransactionValid;

        J *resp = NoteTransaction(req);
        CHECK(resp != NULL);
        CHECK(NoteUserAgent_fake.call_count == 0);

        JDelete(req);
        JDelete(resp);
    }
#endif // !NOTE_DISABLE_USER_AGENT

    RESET_FAKE(_crcAdd);
    RESET_FAKE(_crcError);
    RESET_FAKE(_noteHardReset);
    RESET_FAKE(_noteJSONTransaction);
    RESET_FAKE(_noteTransactionStart);
    RESET_FAKE(NoteDebugWithLevel);
    RESET_FAKE(NoteDelayMs);
    RESET_FAKE(NoteReset);
    RESET_FAKE(NoteUserAgent);
}

}
