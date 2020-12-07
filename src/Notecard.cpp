/*!
 * @file Notecard.cpp
 *
 * @mainpage Arduino Library for the Notecard
 *
 * @section intro_sec Introduction
 *
 * The note-arduino Arduino library for communicating with the
 * <a href="https://blues.io">Blues Wireless</a>
 * Notecard via serial or I2C.
 *
 * This library allows you to control a Notecard by writing an Arduino sketch in
 * C or C++. Your sketch may programmatically configure Notecard and send Notes
 * to <a href="https://notehub.io">Notehub.io</a>.
 *
 * @section dependencies Dependencies
 *
 * This library is a wrapper around and depends upon the
 * <a href="https://github.com/blues/note-c">note-c library</a>, which it
 * includes as a git submodule.
 *
 * In addition, this library requires a physical
 * connection to a Notecard over I2C or Serial to be functional.
 *
 * @section author Author
 *
 * Written by Ray Ozzie and Brandon Satrom for Blues Inc.
 *
 * @section license License
 *
 * Copyright (c) 2019 Blues Inc. MIT License. Use of this source code is
 * governed by licenses granted by the copyright holder including that found in
 * the
 * <a href="https://github.com/blues/note-arduino/blob/master/LICENSE">LICENSE</a>
 * file.
 *
 */

#include <Arduino.h>
#include <note-c/note.h>
#include <Notecard.h>

TwoWire *Notecard::_i2cPort;
HardwareSerial *Notecard::_notecardSerial;
int Notecard::_notecardSerialSpeed;
Stream *Notecard::_debugSerial;
bool Notecard::_debugSerialInitialized;

#define I2C_DATA_TRACE false ///< Enable Tracing for I2C Reads and Writes
static const char *i2cerr = "i2c?";

// 2018-06 ST Microelectronics has a HAL bug that causes an infinite hang.
// This code enables us to exercise that code path to test the state of the bug.
int _readLengthAdjustment = 0;

/***************************************************************************
 PUBLIC FUNCTIONS
 ***************************************************************************/

/**************************************************************************/
/*!
    @brief  Initialize the Notecard for I2C.
            This function configures the Notecard to use the I2C bus
						for communication with the host.
    @param    i2caddress
              The I2C Address to use for the Notecard.
    @param    i2cmax
              The max length of each message to send from the host to
							the Notecard. Used to ensure the messages are sized appropriately
							for the host.
		@param    wirePort
							The TwoWire implementation to use for I2C communication.
*/
/**************************************************************************/
void Notecard::begin(uint32_t i2caddress, uint32_t i2cmax, TwoWire &wirePort) {
	NoteSetFnDefault(malloc, free, delay, millis);
	_i2cPort = &wirePort;

	NoteSetFnI2C(i2caddress, i2cmax, Notecard::noteI2CReset,
							 Notecard::noteI2CTransmit, Notecard::noteI2CReceive);
}

/**************************************************************************/
/*!
    @brief  Initialize the Notecard for Serial communication.
            This function configures the Notecard to use Serial
						for communication with the host.
    @param    selectedSerialPort
              The HardwareSerial bus to use.
    @param    selectedSpeed
              The baud rate to use for communicating with the Notecard
							from the host.
*/
/**************************************************************************/
void Notecard::begin(HardwareSerial &selectedSerialPort, int selectedSpeed)
{
	NoteSetFnDefault(malloc, free, delay, millis);
	_notecardSerial = &selectedSerialPort;
	_notecardSerialSpeed = selectedSpeed;

	NoteSetFnSerial(Notecard::noteSerialReset, Notecard::noteSerialTransmit,
									Notecard::noteSerialAvailable, Notecard::noteSerialReceive);
	_notecardSerial->begin(_notecardSerialSpeed);
}

/**************************************************************************/
/*!
    @brief  Set the debug output source.
            This function takes a Stream object (for example, `Serial`)
						and configures it as a source for writing debug messages
						during development.
    @param    dbgserial
              The Stream object to use for debug output.
*/
/**************************************************************************/
void Notecard::setDebugOutputStream(Stream &dbgserial) {
	_debugSerial = &dbgserial;
	_debugSerialInitialized = true;
	NoteSetFnDebugOutput(Notecard::debugSerialOutput);
}

/**************************************************************************/
/*!
    @brief  Clear the debug output source.
*/
/**************************************************************************/
void Notecard::clearDebugOutputStream() {
	_debugSerialInitialized = false;
	NoteSetFnDebugOutput(NULL);
}

/**************************************************************************/
/*!
    @brief  Adjust the I2C read length.
            Method enabling a developer to test the state of a known issue
						with the I2C HAL on some ST Microelectronics boards.
    @param    Adjustment
              The read length to override.
*/
/**************************************************************************/
void Notecard::i2cTest(int Adjustment) {
	_readLengthAdjustment = Adjustment;
}

/**************************************************************************/
/*!
    @brief  Creates a new request object for population by the host.
            This function accepts a request string (for example, `note.add`)
						and initializes a JSON Object to return to the host.
    @param    request
              The request name, for example, `note.add`.
    @return A `J` JSON Object populated with the request name.
*/
/**************************************************************************/
J *Notecard::newRequest(const char *request) {
	return NoteNewRequest(request);
}

/**************************************************************************/
/*!
    @brief  Sends a request to the Notecard.
            This function takes a populated `J` JSON request object
						and sends it to the Notecard.
    @param    req
              A `J` JSON request object.
    @return `True` if the message was successfully sent to the Notecard,
						`False` if there was an error.
*/
/**************************************************************************/
bool Notecard::sendRequest(J *req) {
	return NoteRequest(req);
}

/**************************************************************************/
/*!
    @brief  Sends a request to the Notecard and return the JSON Response.
            This function takes a populated `J` JSON request object
						and sends it to the Notecard.
    @param    req
              A `J` JSON request object.
    @return `J` JSON Object with the response from the Notecard.
*/
/**************************************************************************/
J *Notecard::requestAndResponse(J *req) {
	return NoteRequestResponse(req);
}

/**************************************************************************/
/*!
    @brief  Deletes a `J` JSON response object from memory.
    @param    rsp
              A `J` JSON response object.
*/
/**************************************************************************/
void Notecard::deleteResponse(J *rsp) {
	NoteDeleteResponse(rsp);
}

/**************************************************************************/
/*!
    @brief  Write a message to the serial debug stream.
    @param    message
              A string to log to the serial debug stream.
*/
/**************************************************************************/
void Notecard::logDebug(const char *message) {
	NoteDebug(message);
}


/**************************************************************************/
/*!
    @brief  Write a formatted message to the serial debug stream.
    @param    format
              A format string to log to the serial debug stream.
	  @param    ... one or more values to interpolate into the format string.
*/
/**************************************************************************/
void Notecard::logDebugf(const char *format, ...) {
    char message[256];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
	NoteDebug(message);
}

/**************************************************************************/
/*!
    @brief  Periodically show Notecard sync status,
						returning TRUE if something was displayed
    @param    pollFrequencyMs
              The frequency to poll the Notecard for sync status.
		@param		maxLevel
							The maximum log level to output to the debug console. Pass
							-1 for all.
    @return `True` if a pending response was displayed to the debug stream.
*/
/**************************************************************************/
bool Notecard::debugSyncStatus(int pollFrequencyMs, int maxLevel) {
	return NoteDebugSyncStatus(pollFrequencyMs, maxLevel);
}

/**************************************************************************/
/*!
    @brief  Determines if there is an error string present in a response object.
    @param    rsp
              A `J` JSON Response object.
    @return `True` if the response object contains an error.
*/
/**************************************************************************/
bool Notecard::responseError(J *rsp) {
	return NoteResponseError(rsp);
}

/***************************************************************************
 PRIVATE FUNCTIONS
 ***************************************************************************/

/**************************************************************************/
/*!
    @brief  Writes a message to the debug Serial stream.
    @param    message
              The message to log.
    @return The number of bytes written.
*/
/**************************************************************************/
size_t Notecard::debugSerialOutput(const char *message) {
	if (!_debugSerialInitialized)
		return 0;
	return(_debugSerial->print(message));
}

/**************************************************************************/
/*!
    @brief  Resets the serial port.
    @return `True` if the Serial port is available.
*/
/**************************************************************************/
bool Notecard::noteSerialReset() {
	_notecardSerial->begin(_notecardSerialSpeed);

	return (!!_notecardSerial);
}

/**************************************************************************/
/*!
    @brief  Writes a message to the Notecard Serial port.
    @param    text
              The text to write.
		@param    len
              The number of bytes to write.
		@param    flush
              `True` to flush to Serial.
*/
/**************************************************************************/
void Notecard::noteSerialTransmit(uint8_t *text, size_t len, bool flush) {
	_notecardSerial->write(text, len);
	if (flush)
		_notecardSerial->flush();
}

/**************************************************************************/
/*!
    @brief  Determines if the Notecard Serial port has data available.
    @return `True` if there are bytes available to read.
*/
/**************************************************************************/
bool Notecard::noteSerialAvailable() {
	return (_notecardSerial->available() > 0);
}

/**************************************************************************/
/*!
    @brief  Read a byte from the Notecard Serial port. guaranteed only ever to
						be called if there is data available.
    @return a single character byte.
*/
/**************************************************************************/
char Notecard::noteSerialReceive() {
	return _notecardSerial->read();
}


/**************************************************************************/
/*!
    @brief  Resets the I2C port. Required by note-c, but not implemented as
						the developer should call `Wire.begin()` themselves before
						initializing the library.
    @return `True`.
*/
/**************************************************************************/
bool Notecard::noteI2CReset(uint16_t DevAddress) {
	_i2cPort->begin();
	return true;
}

/**************************************************************************/
/*!
    @brief  Transmits an amount of data from the host in blocking mode.
    @param    DevAddress
              The I2C address.
		@param    pBuffer
              The data to transmit over I2C. The caller should have shifted
							it right so that the low bit is NOT the read/write bit.
		@param    Size
              the number of bytes to transmit.
		@returns A string with an error, or `NULL` if the transmit was successful.
*/
/**************************************************************************/
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

/**************************************************************************/
/*!
    @brief  Receives an amount of data from the Notecard in blocking mode.
    @param    DevAddress
              The I2C address.
		@param    pBuffer
              The data to transmit over I2C. The caller should have shifted
							it right so that the low bit is NOT the read/write bit.
		@param    Size
              the number of bytes to transmit.
		@param    available
							The number of bytes available to read, out param,
							updated by the function.
		@returns A string with an error, or `NULL` if the receive was successful.
*/
/**************************************************************************/
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
