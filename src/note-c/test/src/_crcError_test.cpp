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
                CHECK(!_crcError(json, seqNo));
            }
        }

        AND_GIVEN("Invalid JSON") {
            char json[] = "{\"req\":";

            THEN("A CRC error SHALL NOT be reported") {
                CHECK(!_crcError(json, seqNo));
            }
        }

        AND_GIVEN("The Notecard returns an error message") {
            char json[] = "{\"err\":\"cannot interpret JSON: bool being placed into a non-bool field {io}\"}";

            THEN("A CRC error SHALL NOT be reported") {
                CHECK(!_crcError(json, seqNo));
            }
        }

        AND_GIVEN("No CRC field") {
            char json[] = "{\"req\": \"hub.sync\"}";

            THEN("A CRC error SHALL NOT be reported") {
                CHECK(!_crcError(json, seqNo));
            }
        }

        AND_GIVEN("The CRC field exists, but is not at the tail of the response") {
            char json[] = "{\"crc\":\"0009:10BAC79A\",\"req\": \"hub.sync\"}";

            THEN("A CRC error SHALL NOT be reported") {
                CHECK(!_crcError(json, seqNo));
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
                CHECK(!_crcError(json, seqNo));
            }
        }

        AND_GIVEN("Invalid JSON") {
            char json[] = "{\"req\":";

            THEN("A CRC error SHALL NOT be reported") {
                CHECK(!_crcError(json, seqNo));
            }
        }

        AND_GIVEN("The Notecard returns an error message") {
            char json[] = "{\"err\":\"cannot interpret JSON: bool being placed into a non-bool field {io}\"}";

            THEN("A CRC error SHALL NOT be reported") {
                CHECK(!_crcError(json, seqNo));
            }
        }

        AND_GIVEN("No CRC field") {
            char json[] = "{\"req\": \"hub.sync\"}";

            THEN("A CRC error SHALL NOT be reported") {
                CHECK(!_crcError(json, seqNo));
            }
        }

        AND_GIVEN("The CRC field exists, but is not at the tail of the response") {
            char json[] = "{\"crc\":\"0009:10BAC79A\",\"req\": \"hub.sync\"}";

            THEN("A CRC error SHALL be reported") {
                CHECK(_crcError(json, seqNo));
            }
        }

        AND_GIVEN("Valid JSON and CRC field present") {
            WHEN("CRC doesn't match") {
                char json[] = "{\"req\":\"hub.sync\",\"crc\":\"0001:DEADBEEF\"}";

                THEN("A CRC error SHALL be reported") {
                    CHECK(_crcError(json, seqNo));
                }
            }

            WHEN("Sequence number doesn't match") {
                char json[] = "{\"req\":\"hub.sync\",\"crc\":\"0009:10BAC79A\"}";

                THEN("A CRC error SHALL be reported") {
                    CHECK(_crcError(json, seqNo));
                }
            }

            WHEN("Everything matches") {
                char json[] = "{\"req\":\"hub.sync\"}";
                char *jsonWithCrc = _crcAdd(json, seqNo);
                REQUIRE(jsonWithCrc != NULL);

                THEN("A CRC error SHALL NOT be reported") {
                    CHECK(!_crcError(json, seqNo));
                }

                NoteFree(jsonWithCrc);
            }

            AND_GIVEN("a trailing CRLF") {
                char json[] = "{\"req\":\"hub.sync\",\"crc\":\"0001:10BAC79A\"}\r\n";

                THEN("A CRC error SHALL NOT be reported") {
                    // Trailing \r\n should be ignored.
                    CHECK(!_crcError(json, seqNo));
                }
            }
        }
    }
}
}

#endif // !NOTE_C_LOW_MEM
