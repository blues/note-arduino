/*!
 * @file NoteBinaryCodecDecode_test.cpp
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
#include <fff.h>

#include "n_lib.h"

DEFINE_FFF_GLOBALS
FAKE_VALUE_FUNC(uint32_t, _cobsDecode, uint8_t *, uint32_t, uint8_t, uint8_t *)

uint8_t encData[12];
const uint32_t encDataLen = sizeof(encData);
uint8_t decBuf[10];
const uint32_t decBufLen = sizeof(decBuf);
uint32_t decLen;

namespace
{

SCENARIO("NoteBinaryCodecDecode")
{
    // Reset variables(s)
    decLen = 0;

    GIVEN("Bad parameters are supplied") {
        decLen = 79;
        WHEN("encData is NULL") {
            decLen = NoteBinaryCodecDecode(NULL, encDataLen, decBuf, decBufLen);

            THEN("The decoded length is zero") {
                CHECK(decLen == 0);
            }
        }
        WHEN("decBuf is NULL") {
            decLen = NoteBinaryCodecDecode(encData, encDataLen, NULL, decBufLen);

            THEN("The decoded length is zero") {
                CHECK(decLen == 0);
            }
        }
        WHEN("decBufLen is less than the size required for the worst-case decoding") {
            uint32_t badDecLen = (_cobsGuaranteedFit(encDataLen) - 1);
            decLen = NoteBinaryCodecDecode(encData, encDataLen, decBuf, badDecLen);

            THEN("The decoded length is zero") {
                CHECK(decLen == 0);
            }
        }
    }

    GIVEN("Parameters are in order") {
        const uint32_t EXPECTED_RESULT = 79;
        _cobsDecode_fake.return_val = EXPECTED_RESULT;
        decLen = NoteBinaryCodecDecode(encData, encDataLen, decBuf, decBufLen);

        THEN("_cobsDecode is invoked") {
            CHECK(_cobsDecode_fake.call_count > 0);
        }

        WHEN("_cobsDecode is invoked") {
            THEN("The parameters are passed without modification") {
                CHECK(_cobsDecode_fake.arg0_val == encData);
                CHECK(_cobsDecode_fake.arg1_val == encDataLen);
                CHECK(_cobsDecode_fake.arg3_val == decBuf);
            }

            THEN("The result is returned without modification") {
                CHECK(EXPECTED_RESULT == decLen);
            }
        }
    }

    RESET_FAKE(_cobsDecode);
}

}


