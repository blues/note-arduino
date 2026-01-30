/*!
 * @file _noteTransaction_calculateTimeoutMs_test.cpp
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

FAKE_VALUE_FUNC(const char *, _noteJSONTransaction, const char *, size_t, char **, uint32_t)

namespace
{

SCENARIO("_noteTransaction_calculateTimeoutMs")
{
    NoteSetFnDefault(malloc, free, NULL, NULL);

    GIVEN ("A `hub.set` command with a `seconds` parameter") {
        const int seconds = 79;
        REQUIRE(seconds != CARD_INTER_TRANSACTION_TIMEOUT_SEC);

        J *req = NoteNewCommand("hub.set");
        REQUIRE(req != NULL);
        JAddIntToObject(req, "seconds", seconds);

        WHEN("NoteTransaction is called") {
            J *resp = NoteTransaction(req);

            THEN("The seconds parameter is ignored and the timeout value is set to `CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000`") {
                CHECK(_noteJSONTransaction_fake.arg3_val == (CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000));
            }

            JDelete(resp);
        }

        JDelete(req);
    }

    GIVEN ("A `hub.set` request with a `seconds` parameter") {
        const int seconds = 79;
        REQUIRE(seconds != CARD_INTER_TRANSACTION_TIMEOUT_SEC);

        J *req = NoteNewRequest("hub.set");
        REQUIRE(req != NULL);
        JAddIntToObject(req, "seconds", seconds);

        WHEN("NoteTransaction is called") {
            J *resp = NoteTransaction(req);

            THEN("The seconds parameter is ignored and the timeout value is set to `CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000`") {
                CHECK(_noteJSONTransaction_fake.arg3_val == (CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000));
            }

            JDelete(resp);
        }

        JDelete(req);
    }

    GIVEN("A `note.add` command with a `seconds` parameter") {
        const int seconds = 917;
        REQUIRE(seconds != CARD_INTER_TRANSACTION_TIMEOUT_SEC);

        J *req = NoteNewCommand("note.add");
        REQUIRE(req != NULL);
        JAddIntToObject(req, "seconds", seconds);

        WHEN("NoteTransaction is called") {
            J *resp = NoteTransaction(req);

            THEN("The timeout value is set to `(seconds + 1) * 1000`") {
                CHECK(_noteJSONTransaction_fake.arg3_val == ((seconds + 1) * 1000));
            }

            JDelete(resp);
        }

        JDelete(req);
    }

    GIVEN("A `note.add` request with a `seconds` parameter") {
        const int seconds = 917;
        REQUIRE(seconds != CARD_INTER_TRANSACTION_TIMEOUT_SEC);

        J *req = NoteNewRequest("note.add");
        REQUIRE(req != NULL);
        JAddIntToObject(req, "seconds", seconds);

        WHEN("NoteTransaction is called") {
            J *resp = NoteTransaction(req);

            THEN("The timeout value is set to `(seconds + 1) * 1000`") {
                CHECK(_noteJSONTransaction_fake.arg3_val == ((seconds + 1) * 1000));
            }

            JDelete(resp);
        }

        JDelete(req);
    }

    GIVEN("A `note.add` command with a `milliseconds` parameter") {
        const int milliseconds = 9171979;
        REQUIRE(milliseconds != CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000);

        J *req = NoteNewCommand("note.add");
        REQUIRE(req != NULL);
        JAddIntToObject(req, "milliseconds", milliseconds);

        WHEN("NoteTransaction is called") {
            J *resp = NoteTransaction(req);

            THEN("The timeout value is set to `(milliseconds + 1000)`") {
                CHECK(_noteJSONTransaction_fake.arg3_val == (milliseconds + 1000));
            }

            JDelete(resp);
        }

        JDelete(req);
    }

    GIVEN("A `note.add` request with a `milliseconds` parameter") {
        const int milliseconds = 9171979;
        REQUIRE(milliseconds != CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000);

        J *req = NoteNewRequest("note.add");
        REQUIRE(req != NULL);
        JAddIntToObject(req, "milliseconds", milliseconds);

        WHEN("NoteTransaction is called") {
            J *resp = NoteTransaction(req);

            THEN("The timeout value is set to `(milliseconds + 1000)`") {
                CHECK(_noteJSONTransaction_fake.arg3_val == (milliseconds + 1000));
            }

            JDelete(resp);
        }

        JDelete(req);
    }

    GIVEN("A `note.add` command with both `milliseconds` and `seconds` parameters") {
        const int milliseconds = 9171979;
        const int seconds = 917;
        REQUIRE(milliseconds != CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000);
        REQUIRE(seconds != CARD_INTER_TRANSACTION_TIMEOUT_SEC);

        J *req = NoteNewCommand("note.add");
        REQUIRE(req != NULL);
        JAddIntToObject(req, "milliseconds", milliseconds);
        JAddIntToObject(req, "seconds", seconds);

        WHEN("NoteTransaction is called") {
            J *resp = NoteTransaction(req);

            THEN("The timeout value is set to `(milliseconds + 1000)`") {
                CHECK(_noteJSONTransaction_fake.arg3_val == (milliseconds + 1000));
            }

            JDelete(resp);
        }

        JDelete(req);
    }

    GIVEN("A `note.add` request with both `milliseconds` and `seconds` parameters") {
        const int milliseconds = 9171979;
        const int seconds = 917;
        REQUIRE(milliseconds != CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000);
        REQUIRE(seconds != CARD_INTER_TRANSACTION_TIMEOUT_SEC);

        J *req = NoteNewRequest("note.add");
        REQUIRE(req != NULL);
        JAddIntToObject(req, "milliseconds", milliseconds);
        JAddIntToObject(req, "seconds", seconds);

        WHEN("NoteTransaction is called") {
            J *resp = NoteTransaction(req);

            THEN("The timeout value is set to `(milliseconds + 1000)`") {
                CHECK(_noteJSONTransaction_fake.arg3_val == (milliseconds + 1000));
            }

            JDelete(resp);
        }

        JDelete(req);
    }

    GIVEN("A `note.add` command with neither `milliseconds` nor `seconds` parameters") {
        J *req = NoteNewCommand("note.add");
        REQUIRE(req != NULL);

        WHEN("NoteTransaction is called") {
            J *resp = NoteTransaction(req);

            THEN("The timeout value is set to `CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000`") {
                CHECK(_noteJSONTransaction_fake.arg3_val == (CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000));
            }

            JDelete(resp);
        }

        JDelete(req);
    }

    GIVEN("A `note.add` request with neither `milliseconds` nor `seconds` parameters") {
        J *req = NoteNewRequest("note.add");
        REQUIRE(req != NULL);

        WHEN("NoteTransaction is called") {
            J *resp = NoteTransaction(req);

            THEN("The timeout value is set to `CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000`") {
                CHECK(_noteJSONTransaction_fake.arg3_val == (CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000));
            }

            JDelete(resp);
        }

        JDelete(req);
    }

    GIVEN("A `web.post` command with a `seconds` parameter") {
        const int seconds = 917;
        REQUIRE(seconds != CARD_INTER_TRANSACTION_TIMEOUT_SEC);

        J *req = NoteNewCommand("web.post");
        REQUIRE(req != NULL);
        JAddIntToObject(req, "seconds", seconds);

        WHEN("NoteTransaction is called") {
            J *resp = NoteTransaction(req);

            THEN("The timeout value is set to `(seconds + 1) * 1000`") {
                CHECK(_noteJSONTransaction_fake.arg3_val == ((seconds + 1) * 1000));
            }

            JDelete(resp);
        }

        JDelete(req);
    }

    GIVEN("A `web.post` request with a `seconds` parameter") {
        const int seconds = 917;
        REQUIRE(seconds != CARD_INTER_TRANSACTION_TIMEOUT_SEC);

        J *req = NoteNewRequest("web.post");
        REQUIRE(req != NULL);
        JAddIntToObject(req, "seconds", seconds);

        WHEN("NoteTransaction is called") {
            J *resp = NoteTransaction(req);

            THEN("The timeout value is set to `(seconds + 1) * 1000`") {
                CHECK(_noteJSONTransaction_fake.arg3_val == ((seconds + 1) * 1000));
            }

            JDelete(resp);
        }

        JDelete(req);
    }

    GIVEN("A `web.post` command with a `milliseconds` parameter") {
        const int milliseconds = 9171979;
        REQUIRE(milliseconds != CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000);

        J *req = NoteNewCommand("web.post");
        REQUIRE(req != NULL);
        JAddIntToObject(req, "milliseconds", milliseconds);

        WHEN("NoteTransaction is called") {
            J *resp = NoteTransaction(req);

            THEN("The timeout value is set to `(milliseconds + 1000)`") {
                CHECK(_noteJSONTransaction_fake.arg3_val == (milliseconds + 1000));
            }

            JDelete(resp);
        }

        JDelete(req);
    }

    GIVEN("A `web.post` request with a `milliseconds` parameter") {
        const int milliseconds = 9171979;
        REQUIRE(milliseconds != CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000);

        J *req = NoteNewRequest("web.post");
        REQUIRE(req != NULL);
        JAddIntToObject(req, "milliseconds", milliseconds);

        WHEN("NoteTransaction is called") {
            J *resp = NoteTransaction(req);

            THEN("The timeout value is set to `(milliseconds + 1000)`") {
                CHECK(_noteJSONTransaction_fake.arg3_val == (milliseconds + 1000));
            }

            JDelete(resp);
        }

        JDelete(req);
    }

    GIVEN("A `web.post` command with both `milliseconds` and `seconds` parameters") {
        const int milliseconds = 9171979;
        const int seconds = 917;
        REQUIRE(milliseconds != CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000);
        REQUIRE(seconds != CARD_INTER_TRANSACTION_TIMEOUT_SEC);

        J *req = NoteNewCommand("web.post");
        REQUIRE(req != NULL);
        JAddIntToObject(req, "milliseconds", milliseconds);
        JAddIntToObject(req, "seconds", seconds);

        WHEN("NoteTransaction is called") {
            J *resp = NoteTransaction(req);

            THEN("The timeout value is set to `(milliseconds + 1000)`") {
                CHECK(_noteJSONTransaction_fake.arg3_val == (milliseconds + 1000));
            }

            JDelete(resp);
        }

        JDelete(req);
    }

    GIVEN("A `web.post` request with both `milliseconds` and `seconds` parameters") {
        const int milliseconds = 9171979;
        const int seconds = 917;
        REQUIRE(milliseconds != CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000);
        REQUIRE(seconds != CARD_INTER_TRANSACTION_TIMEOUT_SEC);

        J *req = NoteNewRequest("web.post");
        REQUIRE(req != NULL);
        JAddIntToObject(req, "milliseconds", milliseconds);
        JAddIntToObject(req, "seconds", seconds);

        WHEN("NoteTransaction is called") {
            J *resp = NoteTransaction(req);

            THEN("The timeout value is set to `(milliseconds + 1000)`") {
                CHECK(_noteJSONTransaction_fake.arg3_val == (milliseconds + 1000));
            }

            JDelete(resp);
        }

        JDelete(req);
    }

    GIVEN("A `web.post` command with neither `milliseconds` nor `seconds` parameters") {
        J *req = NoteNewCommand("web.post");
        REQUIRE(req != NULL);

        WHEN("NoteTransaction is called") {
            J *resp = NoteTransaction(req);

            THEN("The timeout value is set to `CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000`") {
                CHECK(_noteJSONTransaction_fake.arg3_val == (CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000));
            }

            JDelete(resp);
        }

        JDelete(req);
    }

    GIVEN("A `web.post` request with neither `milliseconds` nor `seconds` parameters") {
        J *req = NoteNewRequest("web.post");
        REQUIRE(req != NULL);

        WHEN("NoteTransaction is called") {
            J *resp = NoteTransaction(req);

            THEN("The timeout value is set to `CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000`") {
                CHECK(_noteJSONTransaction_fake.arg3_val == (CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000));
            }

            JDelete(resp);
        }

        JDelete(req);
    }

    RESET_FAKE(_noteJSONTransaction);
}

}
