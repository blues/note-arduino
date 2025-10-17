/*!
 * @file NoteDebugf_test.cpp
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

char buf[32] = {0};

size_t DebugHook(const char *text)
{
    strcpy(buf, text);
    return strlen(text);
}

SCENARIO("NoteDebugf")
{
    NoteSetFnDefault(malloc, free, NULL, NULL);
    const char msg[] = "Hello world!";
    NoteSetFnDebugOutput(DebugHook);

    NoteDebugf("Hello %s!", "world");
#ifdef NOTE_NODEBUG
    CHECK(strcmp(buf, msg) != 0);
#else
    CHECK(strcmp(buf, msg) == 0);
#endif
    buf[0] = '\0';

    // Unset the hook and make sure it wasn't called again.
    NoteSetFnDebugOutput(NULL);
    NoteDebugf("Hello %s!", "world");
    CHECK(strcmp(buf, "") == 0);
}

}


