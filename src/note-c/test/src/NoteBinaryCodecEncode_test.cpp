/*!
 * @file NoteBinaryCodecEncode_test.cpp
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



#include <string.h>

#include <catch2/catch_test_macros.hpp>
#include "fff.h"

#include "n_lib.h"

DEFINE_FFF_GLOBALS
FAKE_VALUE_FUNC(uint32_t, _cobsEncode, uint8_t *, uint32_t, uint8_t, uint8_t *)

uint8_t decData[10] = "Hi Blues!";
uint32_t decDataLen = strlen((const char *)decData);
uint8_t encBuf[12];
uint32_t encBufLen = sizeof(encBuf);
uint32_t encLen;

namespace
{

SCENARIO("NoteBinaryCodecEncode")
{
    // Reset variables(s)
    encLen = 0;

    GIVEN("Bad parameters are supplied") {
        WHEN("decData is NULL") {
            encLen = NoteBinaryCodecEncode(NULL, decDataLen, encBuf, encBufLen);

            THEN("The encoded length is zero") {
                CHECK(encLen == 0);
            }
        }
        WHEN("encBuf is NULL") {
            encLen = NoteBinaryCodecEncode(decData, decDataLen, NULL, encBufLen);

            THEN("The encoded length is zero") {
                CHECK(encLen == 0);
            }
        }
        WHEN("encBufLen is less than the size required for in place encoding") {
            uint32_t badOutLen = (_cobsEncodedLength(decData, decDataLen) - 1);
            encLen = NoteBinaryCodecEncode(decData, decDataLen, encBuf, badOutLen);

            THEN("The encoded length is zero") {
                CHECK(encLen == 0);
            }
        }
    }

    GIVEN("Parameters are in order") {
        const uint32_t EXPECTED_RESULT = 79;
        _cobsEncode_fake.return_val = EXPECTED_RESULT;
        encLen = NoteBinaryCodecEncode(decData, decDataLen, encBuf, encBufLen);

        THEN("_cobsEncode is invoked") {
            CHECK(_cobsEncode_fake.call_count > 0);
        }

        WHEN("_cobsEncode is invoked") {
            THEN("The parameters are passed without modification") {
                CHECK(_cobsEncode_fake.arg0_val == decData);
                CHECK(_cobsEncode_fake.arg1_val == decDataLen);
                CHECK(_cobsEncode_fake.arg3_val == encBuf);
            }

            THEN("The result is returned without modification") {
                CHECK(EXPECTED_RESULT == encLen);
            }
        }
    }

    RESET_FAKE(_cobsEncode);
}

}


