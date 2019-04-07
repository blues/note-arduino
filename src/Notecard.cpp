// Copyright 2018 Inca Roads LLC.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include <Notecard.h>
#include <Wire.h>

// Forward references to C-callable functions defined below
extern "C" {
    void noteSerialReset(void);
    void noteSerialWriteLine(char *text);
    void noteSerialWrite(uint8_t *text, size_t len);
    bool noteSerialAvailable(void);
    char noteSerialRead(void);
    void noteI2CReset(void);
    char *noteI2CTransmit(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size);
    char *noteI2CReceive(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size, uint32_t *avail);
}

// Debugging
#define I2C_DATA_TRACE          false

// Arduino serial port
static HardwareSerial *hwSerial = NULL;

// Initialize for serial I/O
bool NoteInitSerial(HardwareSerial *selectedSerialPort) {
    hwSerial = selectedSerialPort;
    NoteSetFnSerial(noteSerialReset, noteSerialWriteLine, noteSerialWrite, noteSerialAvailable, noteSerialRead);
    return true;
}

// Initialize for I2C I/O
bool NoteInitI2C() {
    NoteSetFnI2C(0, 0, noteI2CReset, noteI2CTransmit, noteI2CReceive);
    return true;
}

// Initialize for I2C I/O with extended details
bool NoteInitI2CExt(uint32_t i2caddress, uint32_t i2cmax) {
    NoteSetFnI2C(i2caddress, i2cmax, noteI2CReset, noteI2CTransmit, noteI2CReceive);
    return true;
}

// Serial port reset
void noteSerialReset() {
    hwSerial->end();
    NoteInitSerial(hwSerial);
}

// Serial write \n-terminated line and flush function
void noteSerialWriteLine(char *text) {
    hwSerial->println(text);
    hwSerial->flush();
}

// Serial write function
void noteSerialWrite(uint8_t *text, size_t len) {
    hwSerial->write(text, len);
}

// Serial "is anything available" function
bool noteSerialAvailable() {
    return (hwSerial->available() > 0);
}

// Serial read a byte function
char noteSerialRead() {
    return hwSerial->read();
}

// I2C port reset
void noteI2CReset() {
    NoteFnLockI2C();
    Wire.begin();
    NoteFnUnlockI2C();
}

// Transmits in master mode an amount of data in blocking mode.  The address
// is the actual address; the caller should have shifted it right so that the
// low bit is NOT the read/write bit.  If TimeoutMs == 0, the default timeout is used.
// An error message is returned, else NULL if success.
char *noteI2CTransmit(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size) {
    NoteFnDelayMs(1);   // Don't do transactions more frequently than every 1mS
#if I2C_DATA_TRACE
    NoteFnDebug("i2c transmit len: \n", Size);
    for (int i=0; i<Size; i++)
        NoteFnDebug("%c", pBuffer[i]);
    NoteFnDebug("  ");
    for (int i=0; i<Size; i++)
        NoteFnDebug("%02x", pBuffer[i]);
    NoteFnDebug("\n");
#endif
    if (Size > NoteFnI2CMax() || Size > 255)
        return "i2c: write too large";
    int writelen = sizeof(uint8_t) + Size;
    NoteFnLockI2C();
    Wire.beginTransmission((int) DevAddress);
    uint8_t reg = Size;
    bool success = (Wire.write(&reg, sizeof(uint8_t)) == sizeof(uint8_t));
    if (success) success = (Wire.write(pBuffer, Size) == Size);
    if (Wire.endTransmission() != 0)
        success = false;
    NoteFnUnlockI2C();
    if (!success) {
        noteI2CReset();
        return "i2c: write error";
    }
    return NULL;
}

// Receives in master mode an amount of data in blocking mode.
char *noteI2CReceive(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size, uint32_t *available) {
    NoteFnDelayMs(1);   // Don't do transactions more frequently than every 1mS
    if (Size > NoteFnI2CMax() || Size > 255)
        return "i2c: read too large";
#if I2C_DATA_TRACE
    uint8_t *original = pBuffer;
    if (Size)
        NoteFnDebug("i2c receive: %d\n  ", Size);
#endif
    NoteFnLockI2C();
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
        NoteFnDebug("i2c incorrect amount of data: %d expected, %d actual\n", readlen, len);
#endif
        errstr = "i2c: incorrect amount of data received";
    } else {
        availbyte = Wire.read();
        goodbyte = Wire.read();
        if (goodbyte != Size) {
            NoteFnDebug("%d != %d, received:\n", goodbyte, Size);
            for (int i=0; i<Size; i++)
                NoteFnDebug("%c", Wire.read());
            NoteFnDebug("\n");
            errstr = "i2c: incorrect amount of data";
        } else {
            for (int i=0; i<Size; i++)
                *pBuffer++ = Wire.read();
        }
    }

    NoteFnUnlockI2C();
    if (errstr != NULL) {
        NoteFnDebug("%s\n", errstr);
        return errstr;
    }
#if I2C_DATA_TRACE
    if (Size) {
        for (int i=0; i<Size; i++)
            NoteFnDebug("%02x", original[i]);
        NoteFnDebug("\n", availbyte);
    }
    if (availbyte)
        NoteFnDebug("%d available\n", availbyte);
#endif
    *available = availbyte;
    return NULL;
}
