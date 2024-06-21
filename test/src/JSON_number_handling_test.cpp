/*!
 * @file JSON_number_handling_test.cpp
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

// We treat most of the JSON code as "tested" in the sense that it comes from a
// tested third party library. However, we have made some changes to the
// underlying code. For example, we've tweaked the number parsing code (see
// parse_number and print_number in n_cjson.c).

namespace
{

#define FIELD "num"

#define JINTEGER_MAX_STR "9223372036854775807"
// "Integers between 2^(n) and 2^(n+1) round to a multiple of 2^(n−52)."
// (https://en.wikipedia.org/wiki/Double-precision_floating-point_format)
// 9223372036854775807 == 2^63 - 1, so its JNUMBER representation should be rounded
// up to the nearest multiple of 2^10 (1024).
#define JINTEGER_MAX_TO_FLOAT ((JNUMBER)9223372036854775808UL)
#define JINTEGER_MAX_TO_FLOAT_STR "9223372036854775808"

// 9223372036854779903 is between 2^63 and 2^64, so conversion to floating point
// should round to the nearest multiple of 2^11 (2048).
#define JINTEGER_MAX_PLUS_4096 9223372036854779903UL
#define JINTEGER_MAX_PLUS_4096_STR "9223372036854779903"
// The nearest multiple of 2048.
#define JINTEGER_MAX_PLUS_4096_TO_FLOAT ((JNUMBER)9223372036854780000UL)
#define JINTEGER_MAX_PLUS_4096_TO_FLOAT_STR "9.22337203685478e+18"

#define JINTEGER_MIN_STR "-9223372036854775808"
// -9223372036854775808 is already a multiple of 1024, so it can be represented
// exactly as a floating point number.
#define JINTEGER_MIN_TO_FLOAT -((JNUMBER)9223372036854775808UL)
#define JINTEGER_MIN_TO_FLOAT_STR "-9223372036854775808"

#define JINTEGER_MIN_MINUS_4096 -((JNUMBER)9223372036854779904UL)
#define JINTEGER_MIN_MINUS_4096_STR "-9223372036854779904"
// The nearest multiple of 2048.
#define JINTEGER_MIN_MINUS_4096_TO_FLOAT -((JNUMBER)9223372036854780000UL)
#define JINTEGER_MIN_MINUS_4096_TO_FLOAT_STR "-9.22337203685478e+18"

#define SMALL_FLOAT 0.0009765625
#define SMALL_FLOAT_STR "0.0009765625"

#define UNIX_TIMESTAMP 1705699768
#define UNIX_TIMESTAMP_STR "1705699768"

SCENARIO("Unmarshalling")
{
    J *obj;
    NoteSetFnDefault(malloc, free, NULL, NULL);

    GIVEN("A JSON string with a numeric field with value 0") {
        const char json[] = "{\"" FIELD "\":0}";

        WHEN("JParse is called on that string") {
            obj = JParse(json);

            REQUIRE(obj != NULL);

            THEN("JGetInt on the object returns 0") {
                CHECK(JGetInt(obj, FIELD) == 0);
            }

            THEN("JGetNumber on the object returns 0") {
                CHECK(JGetNumber(obj, FIELD) == 0);
            }

            JDelete(obj);
        }
    }

    GIVEN("A JSON string with a numeric field with the max value of JINTEGER") {
        const char json[] = "{\"" FIELD "\":" JINTEGER_MAX_STR "}";

        WHEN("JParse is called on that string") {
            obj = JParse(json);

            REQUIRE(obj != NULL);

            THEN("JGetInt on the object returns the max value of JINTEGER") {
                CHECK(JGetInt(obj, FIELD) == JINTEGER_MAX);
            }

            THEN("JGetNumber on the object returns the max value of JINTEGER "
                 "rounded to the expected power of 2") {
                CHECK(JGetNumber(obj, FIELD) == JINTEGER_MAX_TO_FLOAT);
            }

            JDelete(obj);
        }
    }

    GIVEN("A JSON string with a numeric field with the max value of JINTEGER "
          "plus 4096") {
        const char json[] = "{\"" FIELD "\":" JINTEGER_MAX_PLUS_4096_STR "}";

        WHEN("JParse is called on that string") {
            obj = JParse(json);

            REQUIRE(obj != NULL);

            THEN("JGetInt on the object returns the max value of JINTEGER "
                 "(saturation)") {
                CHECK(JGetInt(obj, FIELD) == JINTEGER_MAX);
            }

            THEN("JGetNumber on the object returns the max value of JINTEGER "
                 "plus 4096 rounded to the expected power of 2") {
                CHECK(JGetNumber(obj, FIELD) == JINTEGER_MAX_PLUS_4096_TO_FLOAT);
            }

            JDelete(obj);
        }
    }

    GIVEN("A JSON string with a numeric field with the min value of JINTEGER") {
        const char json[] = "{\"" FIELD "\":" JINTEGER_MIN_STR "}";

        WHEN("JParse is called on that string") {
            obj = JParse(json);

            REQUIRE(obj != NULL);

            THEN("JGetInt on the object returns the min value of JINTEGER") {
                CHECK(JGetInt(obj, FIELD) == JINTEGER_MIN);
            }

            THEN("JGetNumber on the object returns the min value of JINTEGER "
                 "rounded to the expected power of 2") {
                CHECK(JGetNumber(obj, FIELD) == JINTEGER_MIN_TO_FLOAT);
            }

            JDelete(obj);
        }
    }

    GIVEN("A JSON string with a numeric field with the min value of JINTEGER "
          "minus 4096") {
        const char json[] = "{\"" FIELD "\":" JINTEGER_MIN_MINUS_4096_STR "}";

        WHEN("JParse is called on that string") {
            obj = JParse(json);

            REQUIRE(obj != NULL);

            THEN("JGetInt on the object returns the min value of JINTEGER "
                 "(saturation)") {
                CHECK(JGetInt(obj, FIELD) == JINTEGER_MIN);
            }

            THEN("JGetNumber on the object returns the min value of JINTEGER "
                 "rounded to the expected power of 2") {
                CHECK(JGetNumber(obj, FIELD) ==
                      JINTEGER_MIN_MINUS_4096_TO_FLOAT);
            }

            JDelete(obj);
        }
    }

    GIVEN("A JSON string with a numeric field with a small floating point "
          "value") {
        const char json[] = "{\"" FIELD "\":" SMALL_FLOAT_STR "}";

        WHEN("JParse is called on that string") {
            obj = JParse(json);

            REQUIRE(obj != NULL);

            THEN("JGetNumber on the object returns the small floating point "
                 "value") {
                CHECK(JGetNumber(obj, FIELD) == SMALL_FLOAT);
            }

            JDelete(obj);
        }
    }

    GIVEN("A JSON string with a numeric field with a number that's accurately "
          "represented by a 32-bit int but not a single precision float (e.g. a"
          " Unix timestamp") {
        const char json[] = "{\"" FIELD "\":" UNIX_TIMESTAMP_STR "}";

        WHEN("JParse is called on that string") {
            obj = JParse(json);

            REQUIRE(obj != NULL);

            THEN("JGetNumber on the object returns the small floating point "
                 "value") {
                CHECK(JGetNumber(obj, FIELD) == UNIX_TIMESTAMP);
            }

            JDelete(obj);
        }
    }
}

SCENARIO("Marshalling")
{
    J *obj;
    NoteSetFnDefault(malloc, free, NULL, NULL);

    GIVEN("A JSON object with a numeric field with value 0") {
        const char expected[] = "{\"" FIELD "\":0}";
        obj = JCreateObject();
        REQUIRE(obj != NULL);
        REQUIRE(JAddNumberToObject(obj, FIELD, 0) != NULL);

        WHEN("JPrintUnformatted is called on that object") {
            char *out = JPrintUnformatted(obj);
            REQUIRE(out != NULL);

            THEN("0 is printed accurately") {
                CHECK(strcmp(expected, out) == 0);
            }

            JFree(out);
        }

        JDelete(obj);
    }

    GIVEN("A JSON object with a numeric field with the max value of JINTEGER") {
        const char expected[] = "{\"" FIELD "\":" JINTEGER_MAX_STR "}";
        obj = JCreateObject();
        REQUIRE(obj != NULL);
        REQUIRE(JAddIntToObject(obj, FIELD, JINTEGER_MAX) != NULL);

        WHEN("JPrintUnformatted is called on that object") {
            char *out = JPrintUnformatted(obj);
            REQUIRE(out != NULL);

            THEN("The value is printed accurately") {
                CHECK(strcmp(expected, out) == 0);
            }

            JFree(out);
        }

        JDelete(obj);
    }

    GIVEN("A JSON object with a numeric field with the max value of JINTEGER "
          "plus 4096") {
#ifdef NOTE_C_TEST_SINGLE_PRECISION
        // In the NOTE_C_TEST_SINGLE_PRECISION case, where JNUMBER is a
        // single-precision float, JINTEGER_MAX_PLUS_4096 is indistinguishable
        // from JINTEGER_MAX.
        const char expected[] = "{\"" FIELD "\":" JINTEGER_MAX_STR "}";
#else
        const char expected[] = "{\"" FIELD "\":" \
                                JINTEGER_MAX_PLUS_4096_TO_FLOAT_STR "}";
#endif
        obj = JCreateObject();
        REQUIRE(obj != NULL);
        REQUIRE(JAddNumberToObject(obj, FIELD, JINTEGER_MAX_PLUS_4096) != NULL);

        WHEN("JPrintUnformatted is called on that object") {
            char *out = JPrintUnformatted(obj);
            REQUIRE(out != NULL);

            THEN("The value printed is the max value of JINTEGER plus 4096 "
                 "rounded to the expected power of 2") {
                CHECK(strcmp(expected, out) == 0);
            }

            JFree(out);
        }

        JDelete(obj);
    }

    GIVEN("A JSON object with a numeric field with the min value of JINTEGER") {
        const char expected[] = "{\"" FIELD "\":" JINTEGER_MIN_TO_FLOAT_STR "}";
        obj = JCreateObject();
        REQUIRE(obj != NULL);
        REQUIRE(JAddNumberToObject(obj, FIELD, JINTEGER_MIN) != NULL);


        WHEN("JPrintUnformatted is called on that object") {
            char *out = JPrintUnformatted(obj);
            REQUIRE(out != NULL);

            THEN("The value printed is the min value of JINTEGER") {
                CHECK(strcmp(expected, out) == 0);
            }

            JFree(out);
        }

        JDelete(obj);
    }

    GIVEN("A JSON object with a numeric field with the min value of JINTEGER "
          "minus one") {
        const char expected[] = "{\"" FIELD "\":" JINTEGER_MIN_STR "}";
        obj = JCreateObject();
        REQUIRE(obj != NULL);
        REQUIRE(JAddNumberToObject(obj, FIELD, JINTEGER_MIN) != NULL);


        WHEN("JPrintUnformatted is called on that object") {
            char *out = JPrintUnformatted(obj);
            REQUIRE(out != NULL);

            THEN("The value printed is the min value of JINTEGER "
                 "(saturation)") {
                CHECK(strcmp(expected, out) == 0);
            }

            JFree(out);
        }

        JDelete(obj);
    }

    GIVEN("A J object with a numeric field with the min value of JINTEGER minus"
          " 4096") {
#ifdef NOTE_C_TEST_SINGLE_PRECISION
        // In the NOTE_C_TEST_SINGLE_PRECISION case, where JNUMBER is a
        // single-precision float, JINTEGER_MIN_MINUS_4096 is indistinguishable
        // from JINTEGER_MIN.
        const char expected[] = "{\"" FIELD "\":" JINTEGER_MIN_STR "}";
#else
        const char expected[] = "{\"" FIELD "\":" \
                                JINTEGER_MIN_MINUS_4096_TO_FLOAT_STR "}";
#endif
        obj = JCreateObject();
        REQUIRE(obj != NULL);
        REQUIRE(JAddNumberToObject(obj, FIELD, JINTEGER_MIN_MINUS_4096) != NULL);

        WHEN("JPrintUnformatted is called on that object") {
            char *out = JPrintUnformatted(obj);
            REQUIRE(out != NULL);

            THEN("The value printed is the value rounded to the expected power "
                 "of 2") {
                CHECK(strcmp(expected, out) == 0);
            }

            JFree(out);
        }

        JDelete(obj);
    }

    GIVEN("A J object with a numeric field with a small floating point value") {
        const char expected[] = "{\"" FIELD "\":" SMALL_FLOAT_STR "}";
        obj = JCreateObject();
        REQUIRE(obj != NULL);
        REQUIRE(JAddNumberToObject(obj, FIELD, SMALL_FLOAT) != NULL);

        WHEN("JPrintUnformatted is called on that object") {
            char *out = JPrintUnformatted(obj);
            REQUIRE(out != NULL);
            // Replace closing '}' with null-terminator so we only pick out the
            // number when using sccanf.
            out[strlen(out) - 1] = '\0';
            const char prefix[] = "\"num\":";
            const char *numStart = strstr(out, prefix);
            numStart += strlen(prefix);
            double numValue;
            sscanf(numStart, "%lf", &numValue);

            THEN("The value printed is (approximately) the small floating point"
                 "value") {
                // If the two values are within 1e-11 of each other, call that
                // equal.
                CHECK((numValue - SMALL_FLOAT) < 1e-11);
            }

            JFree(out);
        }

        JDelete(obj);
    }

    GIVEN("A J object with a numeric field with a number that's accurately "
          "represented by an 32-bit int but not a single precision float (e.g."
          "a Unix timestamp") {
        const char expected[] = "{\"" FIELD "\":" UNIX_TIMESTAMP_STR "}";
        obj = JCreateObject();
        REQUIRE(obj != NULL);
        REQUIRE(JAddIntToObject(obj, FIELD, UNIX_TIMESTAMP) != NULL);

        WHEN("JPrintUnformatted is called on that object") {
            char *out = JPrintUnformatted(obj);
            REQUIRE(out != NULL);

            THEN("The timestamp is printed accurately") {
                CHECK(strcmp(expected, out) == 0);
            }

            JFree(out);
        }

        JDelete(obj);
    }
}

}
