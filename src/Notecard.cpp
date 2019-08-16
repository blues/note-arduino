// Copyright 2018 Inca Roads LLC.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include <Notecard.h>
#include <Wire.h>

#define DEBUG_READ_TOO_LONG     false       // If ST bug present, immediate notecard crash
#define DEBUG_READ_TOO_SHORT    false

// We've observed sloppy hardware designs - specifically with regard to capacitance on the
// I2C pullups - that behaved more robustly if we inserted a delay between transactions.
#define CONSERVATIVE_DELAY_MS   5

// Forward references to C-callable functions defined below
extern "C" {
    void noteSerialReset(void);
    void noteSerialWriteLine(const char *text);
    void noteSerialWrite(uint8_t *text, size_t len);
    bool noteSerialAvailable(void);
    char noteSerialRead(void);
    void noteI2CReset(void);
    const char *noteI2CTransmit(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size);
    const char *noteI2CReceive(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size, uint32_t *avail);
    size_t debugSerialOutput(const char *message);
}

// For serial debug output
HardwareSerial *debugSerial = NULL;

// Debugging
#define I2C_DATA_TRACE          false

// Arduino serial port
static HardwareSerial *hwSerial = NULL;

// Initialize for serial I/O
void NoteInitSerial(HardwareSerial *selectedSerialPort) {
    NoteSetFnDefault(malloc, free, delay, millis);
    hwSerial = selectedSerialPort;
    NoteSetFnSerial(noteSerialReset, noteSerialWriteLine, noteSerialWrite, noteSerialAvailable, noteSerialRead);
}

// Initialize for I2C I/O
void NoteInitI2C() {
    NoteSetFnDefault(malloc, free, delay, millis);
    NoteSetFnI2C(0, 0, noteI2CReset, noteI2CTransmit, noteI2CReceive);
}

// Initialize for I2C I/O with extended details
void NoteInitI2CExt(uint32_t i2caddress, uint32_t i2cmax) {
    NoteSetFnDefault(malloc, free, delay, millis);
    NoteSetFnI2C(i2caddress, i2cmax, noteI2CReset, noteI2CTransmit, noteI2CReceive);
}

// Serial output method
size_t debugSerialOutput(const char *message) {
    if (debugSerial == NULL)
        return 0;
    return(debugSerial->print(message));
}

void NoteSetDebugOutputPort(HardwareSerial *dbgserial) {
    debugSerial = dbgserial;
    NoteSetFnDebugOutput(debugSerialOutput);
}

// Serial port reset
void noteSerialReset() {
    hwSerial->end();
    NoteInitSerial(hwSerial);
}

// Serial write \n-terminated line and flush function
void noteSerialWriteLine(const char *text) {
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
const char *noteI2CTransmit(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size) {
    NoteFnDelayMs(1);  // Don't do transactions more frequently than every 1mS
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
    NoteFnLockI2C();
    NoteFnDelayMs(CONSERVATIVE_DELAY_MS);
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
const char *noteI2CReceive(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size, uint32_t *available) {
    if (Size > NoteFnI2CMax() || Size > 255)
        return "i2c: read too large";
#if I2C_DATA_TRACE
    uint8_t *original = pBuffer;
    if (Size)
        NoteFnDebug("i2c receive: %d\n  ", Size);
#endif
    NoteFnLockI2C();
    NoteFnDelayMs(CONSERVATIVE_DELAY_MS);
    const char *errstr = NULL;
    uint8_t goodbyte = 0;
    uint8_t availbyte = 0;

    // Retry errors, because it's harmless to do so
    for (int i=0; i<3; i++) {
        Wire.beginTransmission((int) DevAddress);
        Wire.write(0);
        Wire.write((uint8_t)Size);
        uint8_t result = Wire.endTransmission();
        if (result == 0) {
            errstr = NULL;
            break;
        }
        switch (result) {
        case 1:
            // Interestingly, this is the error that is returned when
            // some random device on the I2C bus is holding SCL low
            errstr = "data too long to fit in transmit buffer";
            break;
        case 2:
            errstr = "received NACK on transmit of address";
            break;
        case 3:
            errstr = "received NACK on transmit of data";
            break;
        case 4:
            errstr = "unknown error on endTransmission";
            break;
        }
    }

    // Only receive if we successfully began transmission
    if (errstr == NULL) {

        int readlen = Size + (sizeof(uint8_t)*2);
#if DEBUG_READ_TOO_LONG
        int len = Wire.requestFrom((int) DevAddress, readlen+10);
#elif DEBUG_READ_TOO_SHORT
        int len = Wire.requestFrom((int) DevAddress, readlen-1);
#else
        int len = Wire.requestFrom((int) DevAddress, readlen);
#endif
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
