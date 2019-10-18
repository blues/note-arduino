// Copyright 2018 Inca Roads LLC.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "n_lib.h"

// We've noticed that there's an instability in some cards' implementations of I2C, and as a result
// we introduce an intentional delay before each and every I2C I/O.	 The timing was computed empirically based
// on a number of commercial devices.
static void _DelayIO() {
	_DelayMs(6);
}

// Initiate a transaction to the notecard using reqdoc, and return the result in rspdoc
const char *i2cNoteTransaction(char *json, char **jsonResponse) {

	// Append '\n' to the transaction
	int jsonLen = strlen(json)+1;
	uint8_t *transmitBuf = (uint8_t *) _Malloc(jsonLen);
	if (transmitBuf == NULL)
		return "insufficient memory";
	memcpy(transmitBuf, json, jsonLen-1);
	transmitBuf[jsonLen-1] = '\n';

	// Transmit the request in chunks, but also in segments so as not to overwhelm the notecard's interrupt buffers
	const char *errstr;
	uint8_t *chunk = transmitBuf;
	uint32_t sentInSegment = 0;
	while (jsonLen > 0) {
		int chunklen = (uint8_t) (jsonLen > (int)_I2CMax() ? (int)_I2CMax() : jsonLen);
		_LockI2C();
		_DelayIO();
		errstr = _I2CTransmit(_I2CAddress(), chunk, chunklen);
		if (errstr != NULL) {
			_Free(transmitBuf);
			_I2CReset();
			_UnlockI2C();
			_Debug("i2c transmit: ");
			_Debug(errstr);
			_Debug("\n");
			return errstr;
		}
		_UnlockI2C();
		chunk += chunklen;
		jsonLen -= chunklen;
		sentInSegment += chunklen;
		if (sentInSegment > CARD_REQUEST_SEGMENT_MAX_LEN) {
			sentInSegment -= CARD_REQUEST_SEGMENT_MAX_LEN;
			_DelayMs(CARD_REQUEST_SEGMENT_DELAY_MS);
		}
	}

	// Free the transmit buffer
	_Free(transmitBuf);

	// Dynamically grow the buffer as we read.	Note that we always put the +1 in the alloc
	// so we can be assured that it can be null-terminated, which must be the case because
	// our json parser requires a null-terminated string.
	int jsonbufAllocLen = 1024;
	char *jsonbuf = (char *) _Malloc(jsonbufAllocLen+1);
	if (jsonbuf == NULL) {
		_Debug("transaction: jsonbuf malloc failed\n");
		return "insufficient memory";
	}

	// Loop, building a reply buffer out of received chunks.  We'll build the reply in the same
	// buffer we used to transmit, and will grow it as necessary.
	bool receivedNewline = false;
	int jsonbufLen = 0;
	int chunklen = 0;
	uint32_t start = _GetMs();
	while (true) {

		// Grow the buffer as necessary to read this next chunk
		if (jsonbufLen + chunklen > jsonbufAllocLen) {
			int growlen = 1024;
			if (chunklen > growlen)
				jsonbufAllocLen += chunklen;
			else
				jsonbufAllocLen += growlen;
			char *jsonbufNew = (char *) _Malloc(jsonbufAllocLen+1);
			if (jsonbufNew == NULL) {
				_Debug("transaction: jsonbuf grow malloc failed\n");
				_Free(jsonbuf);
				return "insufficient memory";
			}
			memcpy(jsonbufNew, jsonbuf, jsonbufLen);
			_Free(jsonbuf);
			jsonbuf = jsonbufNew;
		}

		// Read the chunk
		uint32_t available;
		_LockI2C();
		_DelayIO();
		const char *err = _I2CReceive(_I2CAddress(), (uint8_t *) &jsonbuf[jsonbufLen], chunklen, &available);
		_UnlockI2C();
		if (err != NULL) {
			_Free(jsonbuf);
			_Debug("i2c receive error\n");
			return err;
		}

		// We've now received the chunk
		jsonbufLen += chunklen;

		// If the last byte of the chunk is \n, chances are that we're done.  However, just so
		// that we pull everything pending from the module, we only exit when we've received
		// a newline AND there's nothing left available from the module.
		if (jsonbufLen > 0 && jsonbuf[jsonbufLen-1] == '\n')
			receivedNewline = true;

		// For the next iteration, read the min of what's available and what we're permitted to read
		chunklen = (int) (available > _I2CMax() ? _I2CMax() : available);

		// If there's something available on the notecard for us to receive, do it
		if (chunklen > 0)
			continue;

		// If there's nothing available AND we've received a newline, we're done
		if (receivedNewline)
			break;

		// If we've timed out and nothing's available, exit
		if (_GetMs() >= start + (NOTECARD_TRANSACTION_TIMEOUT_SEC*1000)) {
			_Free(jsonbuf);
			_Debug("reply to request didn't arrive from module in time\n");
			return "notecard request or response was lost";
		}

		// Delay, simply waiting for the Note to process the request
		_DelayMs(50);

	}

	// Null-terminate it, using the +1 space that we'd allocated in the buffer
	jsonbuf[jsonbufLen] = '\0';

	// Return it
	*jsonResponse = jsonbuf;
	return NULL;
}

// Initialize or re-initialize the module, returning false if anything fails
bool i2cNoteReset() {

	// Reset the I2C subsystem
	_LockI2C();
	_I2CReset();
	_UnlockI2C();

	// Synchronize by guaranteeing not only that I2C works, but that we drain the remainder of any
	// pending partial reply from a previously-aborted session.	 This outer loop does retries on
	// I2C error, and is simply here for robustness.
	bool notecardReady = false;
	int retries;
	for (retries=0; !notecardReady && retries<3; retries++) {

		_Debug("notecard i2c reset\n");

		// Loop to drain all chunks of data that may be ready to transmit to us
		int chunklen = 0;
		while (true) {

			// Read the next chunk of available data
			uint32_t available;
			uint8_t buffer[128];
			chunklen = (chunklen > (int)sizeof(buffer)) ? (int)sizeof(buffer) : chunklen;
			chunklen = (chunklen > (int)_I2CMax()) ? (int)_I2CMax() : chunklen;
			_LockI2C();
			_DelayIO();
			const char *err = _I2CReceive(_I2CAddress(), buffer, chunklen, &available);
			_UnlockI2C();
			if (err) break;

			// If nothing left, we're ready to transmit a command to receive the data
			if (available == 0) {
				notecardReady = true;
				break;
			}

			// Read everything that's left on the module
			chunklen = available;

		}

		// Exit loop if success
		if (notecardReady)
			break;

		// Reinitialize i2c if there's no response
		_LockI2C();
		_I2CReset();
		_UnlockI2C();
		_Debug("warning: notecard not responding\n");
		_DelayMs(2000);

	}

	// Done
	return notecardReady;
}
