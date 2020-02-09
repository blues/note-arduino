//
// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.
//
// This example shows the simplest possible method demonstrating how a device might poll a
// notefile used as an "inbound queue", using it to receive messages sent to the device from
// the service.  The message gets into the service by use of the Notehub's HTTP/HTTPS inbound
// request capability.
//
// In order to use this example,
// 1. Get the device up and running the code below, successfully connecting to the servie
// 2. Use the "Devices" view on notehub.io to determine the DeviceUID of the device, which
//    is a unique string that looks like "imei:000000000000000"
// 3. Use the "Settings / Project" view on notehub.io to determine the App UID of your project,
//    a unique string that looks like "app:00000000-0000-0000-0000-000000000000"
// 4. At the command line of your PC, send an HTTP message to the service such as:
//    curl -L 'http://api.notefile.net/req?project="app:00000000-0000-0000-0000-000000000000"&device="imei:000000000000000"' -d '{"req":"note.add","file":"my-inbound.qi","body":{"my-request-type":"my-request"}}'
//

#include <Notecard.h>

// Parameters for this example
#define	INBOUND_QUEUE_POLL_SECS		10
#define	INBOUND_QUEUE_NOTEFILE		"my-inbound.qi"
#define	INBOUND_QUEUE_COMMAND_FIELD	"my-request-type"

// Set this to the Notecard's serial port.  If using I2C, comment this line out using //
#define notecard Serial1

// This is the unique Product Identifier for your device.
#define myProductID "org.coca-cola.soda.vending-machine.v2"
#define myLiveDemo  true

// One-time Arduino initialization
void setup() {

	// Set up for debug output.
	delay(2500);
    Serial.begin(115200);
    NoteSetDebugOutputStream(Serial);

	// Initialize the physical I/O channel to the Notecard
#ifdef notecard
	NoteInitSerial(notecard, 9600);
#else
	NoteInitI2C();
#endif

	// This request marks this device as a "development device".  For development devices, the service
	// accepts inbound HTTP requests in an open, unauthenticated manner for developer convenience.
	// No devices should ever be deployed in this mode, for obvious reasons!
	J *req = NoteNewRequest("card.io");
	JAddStringToObject(req, "mode", "development-on");
	NoteRequest(req);

	// Configure the productUID, and instruct the Notecard to stay connected to the service
	req = NoteNewRequest("service.set");
	JAddStringToObject(req, "product", myProductID);
#if myLiveDemo
	JAddStringToObject(req, "mode", "continuous");
#else
	JAddStringToObject(req, "mode", "periodic");
	JAddNumberToObject(req, "minutes", 60);
#endif
	NoteRequest(req);

}

// In the Arduino main loop which is called repeatedly, add outbound data every 15 seconds
void loop() {

	// On a periodic basis, check the inbound queue for messages.  In a real-world application,
	// this would be checked using a frequency commensurate with the required inbound responsiveness.
	// For the most common "periodic" mode applications, this might be daily or weekly.  In this
	// example, where we are using "continuous" mode, we check quite often for demonstratio purposes.
	static int nextPollMs = 0;
	if (millis() > nextPollMs) {
		nextPollMs = millis() + (INBOUND_QUEUE_POLL_SECS * 1000);

		// Process all pending inbound requests
		while (true) {

			// Get the next available note from our inbound queue notefile, deleting it
			J *req = NoteNewRequest("note.get");
			JAddStringToObject(req, "file", INBOUND_QUEUE_NOTEFILE);
			JAddBoolToObject(req, "delete", true);
			J *rsp = NoteRequestResponse(req);
			if (rsp != NULL) {

				// If an error is returned, this means that no response is pending.  Note
				// that it's expected that this might return either a "note does not exist"
				// error if there are no pending inbound notes, or a "file does not exist" error
				// if the inbound queue hasn't yet been created on the service.
				if (NoteResponseError(rsp)) {
					NoteDeleteResponse(rsp);
					break;
				}

				// Get the note's body
				J *body = JGetObject(rsp, "body");
				if (body != NULL) {

					// Simulate Processing the response here
					char *myCommandType = JGetString(body, INBOUND_QUEUE_COMMAND_FIELD);
					NotePrintf("INBOUND REQUEST: %s\n\n", myCommandType);

				}

			}
	        NoteDeleteResponse(rsp);
		}
		
	}

}
