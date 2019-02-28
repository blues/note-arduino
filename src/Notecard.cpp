// Copyright 2018 Inca Roads LLC.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include <Arduino.h>
#include <Wire.h>
#include <Notecard.h>

// Forward references to C-callable functions defined below
extern "C" {
    void serialReset(void);
    void serialWriteLine(char *text);
    void serialWrite(char *text);
    bool serialAvailable(void);
    char serialRead(void);
    void i2cReset(void);
    char *i2cMasterTransmit(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size, uint32_t TimeoutMs);
    char *i2cMasterReceive(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size, uint32_t TimeoutMs);
}

// Debugging
#define I2C_DATA_TRACE          false

// Arduino serial port
static HardwareSerial *ioSerial = NULL;

// Initialize for serial I/O
bool NotecardInitSerial(void *selectedSerialPort) {
    ioSerial = (HardwareSerial *) selectedSerialPort;
    NotecardSetFnSerial(serialReset, serialWriteLine, serialWrite, serialAvailable, serialRead);
}

// Initialize for I2C I/O
bool NotecardInitI2C(uint32_t i2caddress, uint32_t i2cmax) {
    NotecardSetFnI2C(i2caddress, i2cmax, i2cReset, i2cMasterTransmit, i2cMasterReceive);
}

// Serial port reset
void serialReset() {
    ioSerial->end();
    NotecardInitSerial(ioSerial);
}

// Serial write \n-terminated line and flush function
void serialWriteLn(char *text) {
    ioSerial->println(text);
    ioSerial->flush();
}

// Serial write function
void serialWrite(char *text) {
    ioSerial->write(text);
}

// Serial "is anything available" function
bool serialAvailable() {
    return (ioSerial->available() > 0);
}

// Serial read a byte function
char serialRead() {
    return ioSerial->read();
}

// I2C port reset
void i2cReset() {
    NotecardFnLockI2C();
    Wire.begin();
    NotecardFnUnlockI2C();
}

// Transmits in master mode an amount of data in blocking mode.  The address
// is the actual address; the caller should have shifted it right so that the
// low bit is NOT the read/write bit.  If TimeoutMs == 0, the default timeout is used.
// An error message is returned, else NULL if success.
char *i2cMasterTransmit(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size, uint32_t TimeoutMs) {
#if I2C_DATA_TRACE
    NotecardFnDebug("i2c transmit: %d\n  ", Size);
    for (int i=0; i<Size; i++)
        NotecardFnDebug("%02x", pBuffer[i]);
    NotecardFnDebug("\n");
#endif
    NotecardFnLockI2C();
    Wire.beginTransmission((int) DevAddress);
    bool success = (Wire.write(pBuffer, Size) == Size);
    if (Wire.endTransmission() != 0)
        success = false;
    NotecardFnUnlockI2C();
    if (!success) {
        i2cReset();
        return "i2c: write error";
    }
    return NULL;
}

// Receives in master mode an amount of data in blocking mode.  The address
// is the actual address; the caller should have shifted it right so that the
// low bit is NOT the read/write bit.  If TimeoutMs == 0, the default timeout is used.
// An error message is returned, else NULL for success
char *i2cMasterReceive(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size, uint32_t TimeoutMs) {
    if (Size == 0)
        return NULL;
    if (TimeoutMs == 0)
        TimeoutMs = 5000;
#if I2C_DATA_TRACE
    uint8_t *original = pBuffer;
    NotecardFnDebug("i2c receive: %d\n  ", Size);
#endif
    NotecardFnLockI2C();
    int startMs = NotecardFnGetMs();
    int len = Wire.requestFrom((int) DevAddress, Size);
    if (!Wire.available()) {
        NotecardFnUnlockI2C();
        i2cReset();
#if I2C_DATA_TRACE
        NotecardFnDebug("i2c no response\n");
#endif
        return "i2c: no response";
    }
    if (len != Size) {
        NotecardFnUnlockI2C();
        i2cReset();
#if I2C_DATA_TRACE
        NotecardFnDebug("i2c incorrect amount of data: %d expected, %d actual\n", Size, len);
#endif
        if (len == 0)
            return "i2c: no data received";
        return "i2c: incorrect amount of data received";
    }
    // This must be a wire-speed loop.  We've observed that on ESP32, even a call to NotecardFnGetMs()
    // will cause serious I2C data loss issues.
    while (Size) {
        while (Size && Wire.available()) {
            *pBuffer++ = Wire.read();
            Size--;
        }
    }            
    NotecardFnUnlockI2C();
#if I2C_DATA_TRACE
    for (int i=0; i<len; i++)
        NotecardFnDebug("%02x", original[i]);
    NotecardFnDebug("\n");
#endif
    return NULL;
}
