// Copyright 2018 Inca Roads LLC.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include <Arduino.h>
#include <note-c/note.h>
#include <Notecard.h>

TwoWire *Notecard::_i2cPort;
HardwareSerial *Notecard::_notecardSerial;
int Notecard::_notecardSerialSpeed;
Stream *Notecard::_debugSerial;
bool Notecard::_debugSerialInitialized;

// Debugging
#define I2C_DATA_TRACE false
static const char *i2cerr = "i2c?";

// 2018-06 ST Microelectronics has a HAL bug that causes an infinite hang.
// This code enables us to exercise that code path to test the state of the bug.
int _readLengthAdjustment = 0;

// Initialize for I2C I/O
void Notecard::begin(uint32_t i2caddress, uint32_t i2cmax, TwoWire &wirePort) {
	NoteSetFnDefault(malloc, free, delay, millis);
	_i2cPort = &wirePort;

	NoteSetFnI2C(i2caddress, i2cmax, Notecard::noteI2CReset,
							 Notecard::noteI2CTransmit, Notecard::noteI2CReceive);
}

// Initialize for serial I/O
void Notecard::begin(HardwareSerial &selectedSerialPort, int selectedSpeed)
{
	NoteSetFnDefault(malloc, free, delay, millis);
	_notecardSerial = &selectedSerialPort;
	_notecardSerialSpeed = selectedSpeed;

	NoteSetFnSerial(Notecard::noteSerialReset, Notecard::noteSerialTransmit,
									Notecard::noteSerialAvailable, Notecard::noteSerialReceive);
	_notecardSerial->begin(_notecardSerialSpeed);
}

void Notecard::setDebugOutputStream(Stream &dbgserial) {
	_debugSerial = &dbgserial;
	_debugSerialInitialized = true;
	NoteSetFnDebugOutput(Notecard::debugSerialOutput);
}

// Method enabling us to test the state of the ST Microelectronics I2C HAL issue
void Notecard::i2cTest(int Adjustment) {
	_readLengthAdjustment = Adjustment;
}

// Serial output method
size_t Notecard::debugSerialOutput(const char *message) {
	if (!_debugSerialInitialized)
		return 0;
	return(_debugSerial->print(message));
}

// Serial port reset
bool Notecard::noteSerialReset() {
	_notecardSerial->end();
	_notecardSerial->begin(_notecardSerialSpeed);

	return true;
}

// Serial transmit function
void Notecard::noteSerialTransmit(uint8_t *text, size_t len, bool flush) {
	_notecardSerial->write(text, len);
	if (flush)
		_notecardSerial->flush();
}

// Serial 'is anything available?' function
bool Notecard::noteSerialAvailable() {
	return (_notecardSerial->available() > 0);
}

// Serial read a byte function, guaranteed only ever to be called if there is data Available()
char Notecard::noteSerialReceive() {
	return _notecardSerial->read();
}

// I2C port reset
bool Notecard::noteI2CReset() {
	_i2cPort->begin();

	return true;
}

// Transmits in master mode an amount of data in blocking mode.	 The address
// is the actual address; the caller should have shifted it right so that the
// low bit is NOT the read/write bit.  If TimeoutMs == 0, the default timeout is used.
// An error message is returned, else NULL if success.
const char *Notecard::noteI2CTransmit(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size) {
#if I2C_DATA_TRACE
	NoteDebugf("i2c transmit len: \n", Size);
	for (int i=0; i<Size; i++)
		NoteDebugf("%c", pBuffer[i]);
	NoteDebugf("  ");
	for (int i=0; i<Size; i++)
		NoteDebugf("%02x", pBuffer[i]);
	NoteDebugf("\n");
#endif
	_i2cPort->beginTransmission((int) DevAddress);
	uint8_t reg = Size;
	bool success = (_i2cPort->write(&reg, sizeof(uint8_t)) == sizeof(uint8_t));
	if (success) success = (_i2cPort->write(pBuffer, Size) == Size);
	if (_i2cPort->endTransmission() != 0)
		success = false;
	if (!success)
		return ERRSTR("i2c: write error",i2cerr);
	return NULL;
}

// Receives in master mode an amount of data in blocking mode.
const char *Notecard::noteI2CReceive(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size, uint32_t *available) {
#if I2C_DATA_TRACE
	uint8_t *original = pBuffer;
	if (Size)
		NoteDebugf("i2c receive: %d\n	", Size);
#endif
	const char *errstr = NULL;
	uint8_t goodbyte = 0;
	uint8_t availbyte = 0;

	// Retry errors, because it's harmless to do so
	for (int i=0; i<3; i++) {
		_i2cPort->beginTransmission((int) DevAddress);
		_i2cPort->write((uint8_t)0);
		_i2cPort->write((uint8_t)Size);
		uint8_t result = _i2cPort->endTransmission();
		if (result == 0) {
			errstr = NULL;
			break;
		}
#ifdef ERRDBG
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
#else
		errstr = i2cerr;
#endif
	}

	// Only receive if we successfully began transmission
	if (errstr == NULL) {

		int readlen = Size + (sizeof(uint8_t)*2);
		int len = _i2cPort->requestFrom((int) DevAddress, readlen+_readLengthAdjustment);
		if (len == 0) {
			errstr = ERRSTR("i2c: no response",i2cerr);
		} else if (len != readlen) {
#if I2C_DATA_TRACE
			NoteDebugf("i2c incorrect amount of data: %d expected, %d actual\n", readlen, len);
#endif
			errstr = ERRSTR("i2c: incorrect amount of data received",i2cerr);
		} else {
			availbyte = _i2cPort->read();
			goodbyte = _i2cPort->read();
			if (goodbyte != Size) {
#if I2C_DATA_TRACE
				NoteDebugf("%d != %d, received:\n", goodbyte, Size);
				for (int i=0; i<Size; i++)
					NoteDebugf("%c", _i2cPort.read());
				NoteDebugf("\n");
#endif
				errstr = ERRSTR("i2c: incorrect amount of data",i2cerr);
			} else {
				for (int i=0; i<Size; i++)
					*pBuffer++ = _i2cPort->read();
			}
		}
	}

	if (errstr != NULL) {
		NoteDebugln(errstr);
		return errstr;
	}
#if I2C_DATA_TRACE
	if (Size) {
		for (int i=0; i<Size; i++)
			NoteDebugf("%02x", original[i]);
		NoteDebugf("\n", availbyte);
	}
	if (availbyte)
		NoteDebugf("%d available\n", availbyte);
#endif
	*available = availbyte;
	return NULL;
}
