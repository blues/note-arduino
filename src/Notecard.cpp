// Copyright 2018 Inca Roads LLC.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include <Notecard.h>
#include <Wire.h>

// 2018-06 ST Microelectronics has a HAL bug that causes an infinite hang.  This code enables
// us to exercise that code path to test the state of the bug.
static int readLengthAdjustment = 0;

// Forward references to C-callable functions defined below
extern "C" {
	void noteSerialReset(void);
	void noteSerialTransmit(uint8_t *text, size_t len, bool flush);
	bool noteSerialAvailable(void);
	char noteSerialReceive(void);
	void noteI2CReset(void);
	const char *noteI2CTransmit(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size);
	const char *noteI2CReceive(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size, uint32_t *avail);
	size_t debugSerialOutput(const char *message);
}

// For serial debug output
static bool debugSerialInitialized;
static Stream *debugSerial;

// Debugging
#define I2C_DATA_TRACE false

// Arduino serial port
static bool hwSerialInitialized;
static HardwareSerial *hwSerial;
static int hwSerialSpeed;

// Initialize for serial I/O
void NoteInitSerial(HardwareSerial &selectedSerialPort, int selectedSpeed) {
	NoteSetFnDefault(malloc, free, delay, millis);
	hwSerial = &selectedSerialPort;
	hwSerialSpeed = selectedSpeed;
	NoteSetFnSerial(noteSerialReset, noteSerialTransmit, noteSerialAvailable, noteSerialReceive);
	hwSerial->begin(hwSerialSpeed);
}

// Initialize for I2C I/O
void NoteInitI2C() {
	NoteSetFnDefault(malloc, free, delay, millis);
	NoteSetFnI2C(NOTE_I2C_ADDR_DEFAULT, NOTE_I2C_MAX_DEFAULT, noteI2CReset, noteI2CTransmit, noteI2CReceive);
}

// Initialize for I2C I/O with extended details
void NoteInitI2CExt(uint32_t i2caddress, uint32_t i2cmax) {
	NoteSetFnDefault(malloc, free, delay, millis);
	NoteSetFnI2C(i2caddress, i2cmax, noteI2CReset, noteI2CTransmit, noteI2CReceive);
}

// Serial output method
size_t debugSerialOutput(const char *message) {
	if (!debugSerialInitialized)
		return 0;
	return(debugSerial->print(message));
}

void NoteSetDebugOutputStream(Stream &dbgserial) {
	debugSerial = &dbgserial;
	debugSerialInitialized = true;
	NoteSetFnDebugOutput(debugSerialOutput);
}

// Serial port reset
void noteSerialReset() {
	hwSerial->end();
	hwSerial->begin(hwSerialSpeed);
}

// Serial transmit function
void noteSerialTransmit(uint8_t *text, size_t len, bool flush) {
	hwSerial->write(text, len);
	if (flush)
		hwSerial->flush();
}

// Serial "is anything available" function
bool noteSerialAvailable() {
	return (hwSerial->available() > 0);
}

// Serial read a byte function, guaranteed only ever to be called if there is data Available()
char noteSerialReceive() {
	return hwSerial->read();
}

// I2C port reset
void noteI2CReset() {
	Wire.begin();
}

// Transmits in master mode an amount of data in blocking mode.	 The address
// is the actual address; the caller should have shifted it right so that the
// low bit is NOT the read/write bit.  If TimeoutMs == 0, the default timeout is used.
// An error message is returned, else NULL if success.
const char *noteI2CTransmit(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size) {
#if I2C_DATA_TRACE
	NoteFnDebug("i2c transmit len: \n", Size);
	for (int i=0; i<Size; i++)
		NoteFnDebug("%c", pBuffer[i]);
	NoteFnDebug("  ");
	for (int i=0; i<Size; i++)
		NoteFnDebug("%02x", pBuffer[i]);
	NoteFnDebug("\n");
#endif
	Wire.beginTransmission((int) DevAddress);
	uint8_t reg = Size;
	bool success = (Wire.write(&reg, sizeof(uint8_t)) == sizeof(uint8_t));
	if (success) success = (Wire.write(pBuffer, Size) == Size);
	if (Wire.endTransmission() != 0)
		success = false;
	if (!success)
		return "i2c: write error";
	return NULL;
}

// Receives in master mode an amount of data in blocking mode.
const char *noteI2CReceive(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size, uint32_t *available) {
#if I2C_DATA_TRACE
	uint8_t *original = pBuffer;
	if (Size)
		NoteFnDebug("i2c receive: %d\n	", Size);
#endif
	const char *errstr = NULL;
	uint8_t goodbyte = 0;
	uint8_t availbyte = 0;

	// Retry errors, because it's harmless to do so
	for (int i=0; i<3; i++) {
		Wire.beginTransmission((int) DevAddress);
		Wire.write((uint8_t)0);
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
		int len = Wire.requestFrom((int) DevAddress, readlen+readLengthAdjustment);
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

// Method enabling us to test the state of the ST Microelectronics I2C HAL issue
void NoteI2CTest(int Adjustment) {
	readLengthAdjustment = Adjustment;
}
