/*!
 * @file _crcError_test.cpp
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

#ifndef NOTE_C_LOW_MEM

#include <catch2/catch_test_macros.hpp>

#include "n_lib.h"

extern bool notecardFirmwareSupportsCrc;

namespace
{

SCENARIO("_crcError")
{
    GIVEN("The Notecard firmware does NOT support CRC") {
        notecardFirmwareSupportsCrc = false;

        NoteSetFnDefault(malloc, free, NULL, NULL);

        uint16_t seqNo = 1;

        AND_GIVEN("An empty string") {
            char json[] = "";

            THEN("A CRC error SHALL NOT be reported") {
                REQUIRE(notecardFirmwareSupportsCrc == false);
                CHECK(!_crcError(json, seqNo));
            }
        }

        AND_GIVEN("Invalid JSON") {
            char json[] = "{\"req\":\"hub.sync\",\"crc\":\"0009:10BAC79A\""; // Missing closing brace

            THEN("A CRC error SHALL NOT be reported") {
                REQUIRE(notecardFirmwareSupportsCrc == false);
                CHECK(!_crcError(json, seqNo));
            }
        }

        AND_GIVEN("The Notecard returns an error message") {
            char json[] = "{\"err\":\"cannot interpret JSON: bool being placed into a non-bool field {io}\"}";

            THEN("A CRC error SHALL NOT be reported") {
                REQUIRE(notecardFirmwareSupportsCrc == false);
                CHECK(!_crcError(json, seqNo));
            }
        }

        AND_GIVEN("No CRC field") {
            char json[] = "{\"req\": \"hub.sync\"}";

            THEN("A CRC error SHALL NOT be reported") {
                REQUIRE(notecardFirmwareSupportsCrc == false);
                CHECK(!_crcError(json, seqNo));
            }
        }

        AND_GIVEN("The CRC field exists, but is not at the tail of the response") {
            char json[] = "{\"crc\":\"0009:10BAC79A\",\"req\": \"hub.sync\"}";

            THEN("A CRC error SHALL NOT be reported") {
                REQUIRE(notecardFirmwareSupportsCrc == false);
                CHECK(!_crcError(json, seqNo));
            }
        }

        AND_GIVEN("A hub.status response with a quote at the CRC check offset") {
            // The test string below is carefully crafted to be long enough to
            // trigger the memcmp bug if we are not correctly checking for the
            // presence of the CRC field when checking for CRC support. This
            // JSON string happens to have a quotation mark at the exact
            // position where the CRC field would begin.
            // This test ensures the memcmp size bug (i.e. comparing 1 byte
            // instead of 7), does not interpret the '"' at position 19 as the
            // start of a CRC field, causing a false detection of CRC support
            // and permanently flipping notecardFirmwareSupportsCrc to true,
            // breaking all subsequent non-CRC communication.
            char json[] = "{\"connected\":false,\"status\":\"connecting\"}";

            THEN("A CRC error SHALL NOT be reported") {
                REQUIRE(notecardFirmwareSupportsCrc == false);
                CHECK(!_crcError(json, seqNo));
            }

            THEN("notecardFirmwareSupportsCrc SHALL remain false") {
                REQUIRE(notecardFirmwareSupportsCrc == false);
                _crcError(json, seqNo);
                CHECK(notecardFirmwareSupportsCrc == false);
            }
        }

        AND_GIVEN("Valid JSON and CRC field present") {
            WHEN("Everything matches") {
                char json[] = "{\"req\":\"hub.sync\"}";
                char *jsonWithCrc = _crcAdd(json, seqNo);
                REQUIRE(jsonWithCrc != NULL);

                THEN("A CRC error SHALL NOT be reported") {
                    REQUIRE(notecardFirmwareSupportsCrc == false);
                    CHECK(!_crcError(jsonWithCrc, seqNo));
                }

                THEN("notecardFirmwareSupportsCrc SHALL become true") {
                    REQUIRE(notecardFirmwareSupportsCrc == false);
                    _crcError(jsonWithCrc, seqNo);
                    CHECK(notecardFirmwareSupportsCrc == true);
                }

                NoteFree(jsonWithCrc);
            }
        }
    }

    GIVEN("The Notecard firmware supports CRC") {
        notecardFirmwareSupportsCrc = true;

        NoteSetFnDefault(malloc, free, NULL, NULL);

        uint16_t seqNo = 1;

        AND_GIVEN("An empty string") {
            char json[] = "";

            THEN("A CRC error SHALL NOT be reported") {
                REQUIRE(notecardFirmwareSupportsCrc == true);
                CHECK(!_crcError(json, seqNo));
            }
        }

        AND_GIVEN("Invalid JSON") {
            char json[] = "{\"req\":";

            THEN("A CRC error SHALL NOT be reported") {
                REQUIRE(notecardFirmwareSupportsCrc == true);
                CHECK(!_crcError(json, seqNo));
            }
        }

        AND_GIVEN("The Notecard returns an error message") {
            char json[] = "{\"err\":\"cannot interpret JSON: bool being placed into a non-bool field {io}\"}";

            THEN("A CRC error SHALL NOT be reported") {
                REQUIRE(notecardFirmwareSupportsCrc == true);
                CHECK(!_crcError(json, seqNo));
            }
        }

        AND_GIVEN("Valid JSON but no CRC field") {
            // The test string below is carefully crafted to be long enough to
            // trigger the memcmp bug if we are not correctly checking for the
            // presence of the CRC field when checking for CRC support. This
            // JSON string happens to have a quotation mark at the exact
            // position where the CRC field would begin.
            // This test ensures the memcmp size bug (i.e. comparing 1 byte
            // instead of 7), does not interpret the '"' at position 19 as the
            // start of a CRC field, causing a false detection of CRC support
            // and not recognizing a break in the protocol.
            char json[] = "{\"connected\":false,\"status\":\"connecting\"}";

            THEN("A CRC error SHALL be reported") {
                REQUIRE(notecardFirmwareSupportsCrc == true);
                CHECK(_crcError(json, seqNo));
            }

            THEN("notecardFirmwareSupportsCrc SHALL remain true") {
                REQUIRE(notecardFirmwareSupportsCrc == true);
                _crcError(json, seqNo);
                CHECK(notecardFirmwareSupportsCrc == true);
            }
        }

        AND_GIVEN("The CRC field exists, but is not at the tail of the response") {
            char json[] = "{\"crc\":\"0009:10BAC79A\",\"req\": \"hub.sync\"}";

            THEN("A CRC error SHALL be reported") {
                REQUIRE(notecardFirmwareSupportsCrc == true);
                CHECK(_crcError(json, seqNo));
            }
        }

        AND_GIVEN("Valid JSON and CRC field present") {
            WHEN("CRC doesn't match") {
                char json[] = "{\"req\":\"hub.sync\",\"crc\":\"0001:DEADBEEF\"}";

                THEN("A CRC error SHALL be reported") {
                    REQUIRE(notecardFirmwareSupportsCrc == true);
                    CHECK(_crcError(json, seqNo));
                }
            }

            WHEN("Sequence number doesn't match") {
                char json[] = "{\"req\":\"hub.sync\",\"crc\":\"0009:10BAC79A\"}";

                THEN("A CRC error SHALL be reported") {
                    REQUIRE(notecardFirmwareSupportsCrc == true);
                    CHECK(_crcError(json, seqNo));
                }
            }

            WHEN("Everything matches") {
                char json[] = "{\"req\":\"hub.sync\"}";
                char *jsonWithCrc = _crcAdd(json, seqNo);
                REQUIRE(jsonWithCrc != NULL);

                THEN("A CRC error SHALL NOT be reported") {
                    REQUIRE(notecardFirmwareSupportsCrc == true);
                    CHECK(!_crcError(jsonWithCrc, seqNo));
                }

                NoteFree(jsonWithCrc);
            }

            AND_GIVEN("a trailing CRLF") {
                char json[] = "{\"req\":\"hub.sync\",\"crc\":\"0001:10BAC79A\"}\r\n";

                THEN("A CRC error SHALL NOT be reported") {
                    // Trailing \r\n should be ignored.
                    REQUIRE(notecardFirmwareSupportsCrc == true);
                    CHECK(!_crcError(json, seqNo));
                }
            }
        }
    }
}
}

#endif // !NOTE_C_LOW_MEM
