/*!
 * @file _noteSetActiveInterface_test.cpp
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

extern volatile int hookActiveInterface;

extern void * notecardReset;
extern void * notecardTransaction;
extern void * notecardChunkedReceive;
extern void * notecardChunkedTransmit;

extern void _noteSetActiveInterface(int interface);

namespace
{

SCENARIO("_noteSetActiveInterface")
{
    GIVEN("Interface hooks have not been set (i.e. NULL)") {
        hookActiveInterface = NOTE_C_INTERFACE_NONE;

        notecardReset = NULL;
        notecardTransaction = NULL;
        notecardChunkedReceive = NULL;
        notecardChunkedTransmit = NULL;

        WHEN("_noteSetActiveInterface is called with NOTE_C_INTERFACE_NONE") {
            _noteSetActiveInterface(NOTE_C_INTERFACE_NONE);

            THEN("The interface is set to none") {
                CHECK(NOTE_C_INTERFACE_NONE == hookActiveInterface);
            }

            THEN("The interface hooks are set to NULL") {
                CHECK(notecardReset == NULL);
                CHECK(notecardTransaction == NULL);
                CHECK(notecardChunkedReceive == NULL);
                CHECK(notecardChunkedTransmit == NULL);
            }
        }

        WHEN("_noteSetActiveInterface is called with NOTE_C_INTERFACE_I2C") {
            _noteSetActiveInterface(NOTE_C_INTERFACE_I2C);

            THEN("The I2C interface is set") {
                CHECK(NOTE_C_INTERFACE_I2C == hookActiveInterface);
            }

            THEN("The interface hooks are set") {
                CHECK(reinterpret_cast<void *>(_i2cNoteReset) == notecardReset);
                CHECK(reinterpret_cast<void *>(_i2cNoteTransaction) == notecardTransaction);
                CHECK(reinterpret_cast<void *>(_i2cNoteChunkedReceive) == notecardChunkedReceive);
                CHECK(reinterpret_cast<void *>(_i2cNoteChunkedTransmit) == notecardChunkedTransmit);
            }
        }

        WHEN("_noteSetActiveInterface is called with NOTE_C_INTERFACE_SERIAL") {
            _noteSetActiveInterface(NOTE_C_INTERFACE_SERIAL);

            THEN("The serial interface is set") {
                CHECK(NOTE_C_INTERFACE_SERIAL == hookActiveInterface);
            }

            THEN("The interface hooks are set") {
                CHECK(reinterpret_cast<void *>(_serialNoteReset) == notecardReset);
                CHECK(reinterpret_cast<void *>(_serialNoteTransaction) == notecardTransaction);
                CHECK(reinterpret_cast<void *>(_serialChunkedReceive) == notecardChunkedReceive);
                CHECK(reinterpret_cast<void *>(_serialChunkedTransmit) == notecardChunkedTransmit);
            }
        }

        WHEN("_noteSetActiveInterface is called with an unrecognized interface") {
            _noteSetActiveInterface(0x79);

            THEN("The interface is set to none") {
                CHECK(NOTE_C_INTERFACE_NONE == hookActiveInterface);
            }

            THEN("The interface hooks are set to NULL") {
                CHECK(notecardReset == NULL);
                CHECK(notecardTransaction == NULL);
                CHECK(notecardChunkedReceive == NULL);
                CHECK(notecardChunkedTransmit == NULL);
            }
        }
    }

    GIVEN("Interface hooks have previously been set") {
        hookActiveInterface = 79;

        notecardReset = reinterpret_cast<void *>(0x19790917);
        notecardTransaction = reinterpret_cast<void *>(0x19800206);
        notecardChunkedReceive = reinterpret_cast<void *>(0x20130829);
        notecardChunkedTransmit = reinterpret_cast<void *>(0x20180731);

        WHEN("_noteSetActiveInterface is called with NOTE_C_INTERFACE_NONE") {
            _noteSetActiveInterface(NOTE_C_INTERFACE_NONE);

            THEN("The interface is set to none") {
                CHECK(NOTE_C_INTERFACE_NONE == hookActiveInterface);
            }

            THEN("The interface hooks are set to NULL") {
                CHECK(notecardReset == NULL);
                CHECK(notecardTransaction == NULL);
                CHECK(notecardChunkedReceive == NULL);
                CHECK(notecardChunkedTransmit == NULL);
            }
        }

        WHEN("_noteSetActiveInterface is called with NOTE_C_INTERFACE_I2C") {
            _noteSetActiveInterface(NOTE_C_INTERFACE_I2C);

            THEN("The I2C interface is set") {
                CHECK(NOTE_C_INTERFACE_I2C == hookActiveInterface);
            }

            THEN("The interface hooks are set") {
                CHECK(reinterpret_cast<void *>(_i2cNoteReset) == notecardReset);
                CHECK(reinterpret_cast<void *>(_i2cNoteTransaction) == notecardTransaction);
                CHECK(reinterpret_cast<void *>(_i2cNoteChunkedReceive) == notecardChunkedReceive);
                CHECK(reinterpret_cast<void *>(_i2cNoteChunkedTransmit) == notecardChunkedTransmit);
            }
        }

        WHEN("_noteSetActiveInterface is called with NOTE_C_INTERFACE_SERIAL") {
            _noteSetActiveInterface(NOTE_C_INTERFACE_SERIAL);

            THEN("The serial interface is set") {
                CHECK(NOTE_C_INTERFACE_SERIAL == hookActiveInterface);
            }

            THEN("The interface hooks are set") {
                CHECK(reinterpret_cast<void *>(_serialNoteReset) == notecardReset);
                CHECK(reinterpret_cast<void *>(_serialNoteTransaction) == notecardTransaction);
                CHECK(reinterpret_cast<void *>(_serialChunkedReceive) == notecardChunkedReceive);
                CHECK(reinterpret_cast<void *>(_serialChunkedTransmit) == notecardChunkedTransmit);
            }
        }

        WHEN("_noteSetActiveInterface is called with an unrecognized interface") {
            _noteSetActiveInterface(0x79);

            THEN("The interface is set to none") {
                CHECK(NOTE_C_INTERFACE_NONE == hookActiveInterface);
            }

            THEN("The interface hooks are set to NULL") {
                CHECK(notecardReset == NULL);
                CHECK(notecardTransaction == NULL);
                CHECK(notecardChunkedReceive == NULL);
                CHECK(notecardChunkedTransmit == NULL);
            }
        }
    }
}

}
