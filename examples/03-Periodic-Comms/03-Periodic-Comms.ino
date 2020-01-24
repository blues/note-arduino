//
// Copyright 2020 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.
//
// This example does the same function as the "using library" example, but rather than
// keeping the modem turned on constantly this example demonstrates how a developer would
// gather sensor measurements "offline", then perform uploads on a periodic basis.
//

// Define the pin number of the pushbutton pin
#define buttonPin			7				// Change to any GPIO pin where there is an active-high button
#define	buttonPressedState	LOW				// Active high, or active low
#define ledPin				LED_RED			// Change to any GPIO pin where there is an LED

// Include the Arduino library for the Notecard

#include <Notecard.h>

// Set this to the Notecard's serial port.	If using I2C, comment this line out using //
#define notecard Serial1

// This is the unique Product Identifier for your device.
#define myProductID "org.coca-cola.soda.vending-machine.v2"

// Button handling
#define BUTTON_IDLE			0
#define BUTTON_PRESS		1
#define BUTTON_DOUBLEPRESS	2
int buttonPress(void);

// One-time Arduino initialization
void setup() {

	// Initialize Arduino GPIO pins
	pinMode(ledPin, OUTPUT);
	pinMode(buttonPin, buttonPressedState == LOW ? INPUT_PULLUP : INPUT);

	// During development, set up for debug output from the Notecard.  Note that the initial delay is
	// required by some Arduino cards before debug UART output can be successfully displayed in the
	// Arduino IDE, including the Adafruit Feather nRF52840 Express.
	delay(2500);
	Serial.begin(115200);
	NoteSetDebugOutputStream(Serial);

	// Initialize the physical I/O channel to the Notecard
#ifdef notecard
	NoteInitSerial(notecard, 9600);
#else
	NoteInitI2C();
#endif

	// Service configuration request
	J *req = NoteNewRequest("service.set");

	// This command (required) causes the data to be delivered to the Project on notehub.io that has claimed
	// this Product ID.	 (see above)
	JAddStringToObject(req, "product", myProductID);

	// This command determines how often the Notecard connects to the service.
	// Because of the power requirements of a continuous connection, a battery powered device would instead
	// only sample its sensors occasionally, and would only upload to the service on a periodic basis.
	// Generally this might be something like 60 minutes, or perhaps even 60 min * 12 hours = 720 min.
	// For the purpose of this demonstration, however, we'll connect at most every 2 minutes.  If no
	// outbound data is present at that 2 minute mark, no connection will be made.	However, if any
	// outbound data is present, the modem will power-on and all pending data will be transmitted.
	JAddStringToObject(req, "mode", "periodic");
	JAddNumberToObject(req, "minutes", 2);

	// Issue the request
	NoteRequest(req);

}

// In the Arduino main loop which is called repeatedly, add outbound data every 15 seconds
void loop() {

	// Wait for a button press, or perform idle activities
	int buttonState = buttonPress();
	switch (buttonState) {

	case BUTTON_IDLE:
		static int lastStatusMs = 0;
		if (NoteDebugSyncStatus(2500, 0))
			lastStatusMs = millis();
		if (millis() > lastStatusMs + 15000) {
			lastStatusMs = millis();
			Serial.printf("%05d press button to simulate a sensor measurement\n", lastStatusMs/1000);
		}
		return;

	case BUTTON_DOUBLEPRESS:
		NoteRequest(NoteNewRequest("service.sync"));
		return;

	}

	// The button was pressed, so we should begin a transaction
	digitalWrite(ledPin, HIGH);

	// Simulate an event counter of some kind
	static unsigned eventCounter = 0;
	eventCounter = eventCounter + 1;

	// Read the notecard's current temperature and voltage, as simulated sensor measurements
	double temperature = 0;
	J *rsp = NoteRequestResponse(NoteNewRequest("card.temp"));
	if (rsp != NULL) {
		temperature = JGetNumber(rsp, "value");
		NoteDeleteResponse(rsp);
	}
	double voltage = 0;
	rsp = NoteRequestResponse(NoteNewRequest("card.voltage"));
	if (rsp != NULL) {
		voltage = JGetNumber(rsp, "value");
		NoteDeleteResponse(rsp);
	}

	// Enqueue the measurement to the Notecard for transmission to the Notehub.	 These measurements
	// will be staged in the Notecard's flash memory until it's time to transmit them to the service.
	J *req = NoteNewRequest("note.add");
	if (req != NULL) {
		JAddStringToObject(req, "file", "sensors.qo");
		J *body = JCreateObject();
		if (body != NULL) {
			JAddNumberToObject(body, "temp", temperature);
			JAddNumberToObject(body, "voltage", voltage);
			JAddNumberToObject(body, "count", eventCounter);
			JAddItemToObject(req, "body", body);
		}
		NoteRequest(req);
	}

	// Done with transaction
	digitalWrite(ledPin, LOW);

}

// Button handling
int buttonPress() {

	// Detect the "press" transition
	static bool buttonBeingDebounced = false;
	int buttonState = digitalRead(buttonPin);
	if (buttonState != buttonPressedState) {
		if (buttonBeingDebounced) {
			buttonBeingDebounced = false;
		}
		return BUTTON_IDLE;
	}
	if (buttonBeingDebounced)
		return BUTTON_IDLE;
	buttonBeingDebounced = true;

	// Wait to see if this is a double-press
	bool buttonDoublePress = false;
	int buttonPressed = millis();
	bool buttonReleased = false;
	int ignoreBounceMs = 100;
	int doublePressMs = 750;
	while (millis() < buttonPressed+doublePressMs) {
		if (millis() < buttonPressed+ignoreBounceMs)
			continue;
		if (digitalRead(buttonPin) != buttonPressedState) {
			if (!buttonReleased)
				buttonReleased = true;
			continue;
		}
		if (buttonReleased) {
			buttonDoublePress = true;
			break;
		}
	}

	return (buttonDoublePress ? BUTTON_DOUBLEPRESS : BUTTON_PRESS);

}
