/*!
 * @file NoteDebug_test.cpp
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

namespace
{

typedef struct {
    char debugBuf[256];
    size_t debugBufIdx;
    bool debugOutputCalled;
} TestState;

TestState state;

size_t MyDebugOutput(const char *line)
{
    size_t len = 0;
    state.debugOutputCalled = true;

    if (line != NULL) {
        len = strlcpy(state.debugBuf + state.debugBufIdx, line,
                      sizeof(state.debugBuf) - state.debugBufIdx);
        state.debugBufIdx += len;
    }

    return len;
}

SCENARIO("NoteDebug")
{
    memset(&state, 0, sizeof(state));

    SECTION("Hook not set") {
        CHECK(!_noteIsDebugOutputActive());

        NoteDebug(NULL);
        CHECK(!state.debugOutputCalled);
    }

    SECTION("Hook set") {
        NoteSetFnDebugOutput(MyDebugOutput);

        SECTION("Active") {
            CHECK(_noteIsDebugOutputActive());
        }

        SECTION("Hook called") {
            NoteDebug(NULL);

#ifdef NOTE_NODEBUG
            CHECK(!state.debugOutputCalled);
#else
            CHECK(state.debugOutputCalled);
#endif
        }

        SECTION("NoteDebugln") {
            NoteDebugln("test");
#ifdef NOTE_NODEBUG
            CHECK(!state.debugOutputCalled);
#else
            CHECK(!strcmp(state.debugBuf, "test\r\n"));
#endif
        }

        SECTION("NoteDebugIntln") {
            const char* expectedString;

            SECTION("Just number") {
                expectedString = "1\r\n";
                NoteDebugIntln(NULL, 1);
            }

            SECTION("String and number") {
                expectedString = "test1\r\n";
                NoteDebugIntln("test", 1);
            }

#ifdef NOTE_NODEBUG
            CHECK(!state.debugOutputCalled);
#else
            CHECK(state.debugOutputCalled);
            CHECK(!strcmp(state.debugBuf, expectedString));
#endif
        }

        SECTION("NoteDebugWithLevel") {
            const char msg[] = "my message";

            SECTION("Debug level messages NOT logged by default") {
                NOTE_C_LOG_DEBUG(msg);

                CHECK(!state.debugOutputCalled);
            }

            SECTION("Error level messages logged by default") {
                NOTE_C_LOG_ERROR(msg);

#ifdef NOTE_NODEBUG
                CHECK(!state.debugOutputCalled);
#else
                CHECK(state.debugOutputCalled);
                CHECK(strstr(state.debugBuf, msg) != NULL);
#ifdef NOTE_C_LOG_SHOW_FILE_AND_LINE
                CHECK(strstr(state.debugBuf, __FILE__) != NULL);
#endif // NOTE_C_LOG_SHOW_FILE_AND_LINE
#endif // NOTE_NODEBUG
            }
        }
    }
}

}


