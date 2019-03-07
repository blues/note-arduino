// Copyright 2018 Inca Roads LLC.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include <Notecard.h>
#include <Wire.h>

// Forward references to C-callable functions defined below
extern "C" {
    void serialReset(void);
    void serialWriteLine(char *text);
    void serialWrite(char *text);
    bool serialAvailable(void);
    char serialRead(void);
    void i2cReset(void);
    char *i2cMasterTransmit(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size);
    char *i2cMasterReceive(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size, uint32_t *avail);
}

// Debugging
#define I2C_DATA_TRACE          false

// Arduino serial port
static HardwareSerial *hwSerial = NULL;

// Initialize for serial I/O
bool NotecardInitSerial(HardwareSerial *selectedSerialPort) {
    hwSerial = selectedSerialPort;
    NotecardSetFnSerial(serialReset, serialWriteLine, serialWrite, serialAvailable, serialRead);
}

// Initialize for I2C I/O
bool NotecardInitI2C() {
    NotecardSetFnI2C(0, 0, i2cReset, i2cMasterTransmit, i2cMasterReceive);
}

// Initialize for I2C I/O with extended details
bool NotecardInitI2CExt(uint32_t i2caddress, uint32_t i2cmax) {
    NotecardSetFnI2C(i2caddress, i2cmax, i2cReset, i2cMasterTransmit, i2cMasterReceive);
}

// Serial port reset
void serialReset() {
    hwSerial->end();
    NotecardInitSerial(hwSerial);
}

// Serial write \n-terminated line and flush function
void serialWriteLn(char *text) {
    hwSerial->println(text);
    hwSerial->flush();
}

// Serial write function
void serialWrite(char *text) {
    hwSerial->write(text);
}

// Serial "is anything available" function
bool serialAvailable() {
    return (hwSerial->available() > 0);
}

// Serial read a byte function
char serialRead() {
    return hwSerial->read();
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
char *i2cMasterTransmit(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size) {
    NotecardFnDelayMs(1);   // Don't do transactions more frequently than every 1mS
#if I2C_DATA_TRACE
    NotecardFnDebug("i2c transmit len: \n", Size);
    for (int i=0; i<Size; i++)
        NotecardFnDebug("%c", pBuffer[i]);
    NotecardFnDebug("  ");
    for (int i=0; i<Size; i++)
        NotecardFnDebug("%02x", pBuffer[i]);
    NotecardFnDebug("\n");
#endif
    if (Size > NotecardFnI2CMax() || Size > 255)
        return "i2c: write too large";
    int writelen = sizeof(uint8_t) + Size;
    NotecardFnLockI2C();
    Wire.beginTransmission((int) DevAddress);
    uint8_t reg = Size;
    bool success = (Wire.write(&reg, sizeof(uint8_t)) == sizeof(uint8_t));
    if (success) success = (Wire.write(pBuffer, Size) == Size);
    if (Wire.endTransmission() != 0)
        success = false;
    NotecardFnUnlockI2C();
    if (!success) {
        i2cReset();
        return "i2c: write error";
    }
    return NULL;
}

// Receives in master mode an amount of data in blocking mode.
char *i2cMasterReceive(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size, uint32_t *available) {
    NotecardFnDelayMs(1);   // Don't do transactions more frequently than every 1mS
    if (Size > NotecardFnI2CMax() || Size > 255)
        return "i2c: read too large";
#if I2C_DATA_TRACE
    uint8_t *original = pBuffer;
    if (Size)
        NotecardFnDebug("i2c receive: %d\n  ", Size);
#endif
    NotecardFnLockI2C();
    char *errstr = NULL;
    uint8_t goodbyte = 0;
    uint8_t availbyte = 0;

    Wire.beginTransmission((int) DevAddress);
    Wire.write(0);
    Wire.write((uint8_t)Size);
    Wire.endTransmission();

    int readlen = Size + (sizeof(uint8_t)*2);
    int len = Wire.requestFrom((int) DevAddress, readlen);
    if (len == 0) {
        errstr = "i2c: no response";
    } else if (len != readlen) {
#if I2C_DATA_TRACE
        NotecardFnDebug("i2c incorrect amount of data: %d expected, %d actual\n", readlen, len);
#endif
        errstr = "i2c: incorrect amount of data received";
    } else {
        availbyte = Wire.read();
        goodbyte = Wire.read();
        if (goodbyte != Size) {
            NotecardFnDebug("%d < %d, received:\n", goodbyte, Size);
            for (int i=0; i<Size; i++)
                NotecardFnDebug("%c", Wire.read());
            NotecardFnDebug("\n");
            errstr = "i2c: less data was available than requested";
        } else {
            for (int i=0; i<Size; i++)
                *pBuffer++ = Wire.read();
        }
    }

    NotecardFnUnlockI2C();
    if (errstr != NULL) {
        NotecardFnDebug("%s\n", errstr);
        return errstr;
    }
#if I2C_DATA_TRACE
    if (Size) {
        for (int i=0; i<Size; i++)
            NotecardFnDebug("%02x", original[i]);
        NotecardFnDebug("\n", availbyte);
    }
    if (availbyte)
        NotecardFnDebug("%d available\n", availbyte);
#endif
    *available = availbyte;
    return NULL;
}
