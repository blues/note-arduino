// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>
#include <note-c/note.h>

bool NoteInitI2C(void);
bool NoteInitI2CExt(uint32_t i2cAddress, uint32_t i2cMax);
bool NoteInitSerial(HardwareSerial *serial);
