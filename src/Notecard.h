// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#ifndef Notecard_h
#define Notecard_h

#include <Arduino.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <note-c/note.h>

class Notecard
{
  public:
    void begin(uint32_t i2cAddress = NOTE_I2C_ADDR_DEFAULT,
                 uint32_t i2cMax = NOTE_I2C_MAX_DEFAULT,
                 TwoWire &wirePort = Wire);
    void begin(HardwareSerial &serial, int speed);
    void setDebugOutputStream(Stream &dbgserial);
    void i2cTest(int Adjustment);
    J *newRequest(const char *request);
    bool sendRequest(J *req);
    J *requestAndResponse(J *req);
    void deleteResponse(J *rsp);
    void logDebug(const char *message);
    void logDebugf(const char *format, ...);
    bool debugSyncStatus(int pollFrequencyMs, int maxLevel);
    bool responseError(J *rsp);

  private:
    static TwoWire *_i2cPort;
    static HardwareSerial *_notecardSerial;
    static int _notecardSerialSpeed;
    static Stream *_debugSerial;
    static bool _debugSerialInitialized;

    static bool noteSerialReset(void);
    static void noteSerialTransmit(uint8_t *text, size_t len, bool flush);
    static bool noteSerialAvailable(void);
    static char noteSerialReceive(void);
    static bool noteI2CReset(void);
    static const char *noteI2CTransmit(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size);
    static const char *noteI2CReceive(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size, uint32_t *avail);
    static size_t debugSerialOutput(const char *message);
};

#endif