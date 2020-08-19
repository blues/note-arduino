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
#if defined(ARDUINO_ARCH_ESP32)
#define buttonPin			21
#define	buttonPressedState	HIGH
#define ledPin				13
#elif defined(ARDUINO_ARCH_NRF52)
#define buttonPin			7
#define	buttonPressedState	LOW
#define ledPin				LED_RED
#elif defined(ARDUINO_ARCH_NRF52)
#define buttonPin			7
#define	buttonPressedState	LOW
#define ledPin				LED_RED
#elif defined(ARDUINO_ARCH_AVR)
#define buttonPin			6
#define	buttonPressedState	LOW
#define ledPin				4
#else
#error "please add a board definition for button and led"
#define buttonPin			?				// Change to any GPIO pin where there is an active-high button
#define	buttonPressedState	?				// Active high, or active low
#define ledPin				?				// Change to any GPIO pin where there is an LED
#endif

// Include the Arduino library for the Notecard
#include <Notecard.h>

// Note that both of these definitions are optional; just prefix either line with // to remove it.
//  Remove serialNotecard if you wired your Notecard using I2C SDA/SCL pins instead of serial RX/TX
//  Remove serialDebug if you don't want the Notecard library to output debug information
#define serialNotecard Serial1
#define serialDebugOut Serial

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
#ifdef serialDebugOut
    delay(2500);
    serialDebugOut.begin(115200);
    NoteSetDebugOutputStream(serialDebugOut);
#endif

	// Initialize the physical I/O channel to the Notecard
#ifdef serialNotecard
	NoteInitSerial(serialNotecard, 9600);
#else
	NoteInitI2C();
#endif

	// Service configuration request
	J *req = NoteNewRequest("service.set");

	// This command (required) causes the data to be delivered to the Project on notehub.io that has claimed
	// this Product ID.	 (see above)
	JAddStringToObject(req, "product", myProductID);

	// This sets the notecard's connectivity mode to periodic, rather than being continuously connected
	JAddStringToObject(req, "mode", "periodic");

	// This parameter establishes how often the Notecard will check for data that is waiting to be
	// uploaded to the service.  Generally this might be something like 60 minutes, or perhaps even
	// 12 hours * 60 min = 720 min.  For the purpose of this demonstration, however, we'll set the
	// period such that it checks for outgoing data at most every 2 minutes.
	JAddNumberToObject(req, "minutes", 2);

	// This parameter establishes how often the Notecard will check for data that is waiting on the
	// service to be downloaded to the Notecard.  Generally this might be 12, 24, or even 48 hours,
	// however for the purpose of this demonstration we will connect to the service to check for
	// incoming data at least once every hour.
	JAddNumberToObject(req, "hours", 1);

	// Issue the request
	NoteRequest(req);

}

// In the Arduino main loop which is called repeatedly, add outbound data every 15 seconds
void loop() {
	static unsigned long lastStatusMs = 0;

	// Activity indicator
	digitalWrite(ledPin, HIGH);

	// Wait for a button press, or perform idle activities
	int buttonState = buttonPress();
	switch (buttonState) {

	case BUTTON_IDLE:
		if (NoteDebugSyncStatus(2500, 0))
			lastStatusMs = millis();
		if (millis() > lastStatusMs + 10000) {
			lastStatusMs = millis();
			NoteDebug("press button to simulate a sensor measurement\n");
		}
		delay(25);
		digitalWrite(ledPin, LOW);
		delay(100);
		return;

	case BUTTON_DOUBLEPRESS:
		NoteRequest(NoteNewRequest("service.sync"));
		digitalWrite(ledPin, LOW);
		return;

	}

	// The button was pressed, so we should begin a transaction
	NoteDebug("performing sensor measurement\n");
	lastStatusMs = millis();

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

	// Wait to see if this is a double-press
	bool buttonDoublePress = false;
	bool buttonReleased = false;
	unsigned long buttonPressedMs = millis();
	unsigned long ignoreBounceMs = 100;
	unsigned long doublePressMs = 750;
	while (millis() < buttonPressedMs+doublePressMs || digitalRead(buttonPin) == buttonPressedState) {
		if (millis() < buttonPressedMs+ignoreBounceMs)
			continue;
		if (digitalRead(buttonPin) != buttonPressedState) {
			if (!buttonReleased)
				buttonReleased = true;
			continue;
		}
		if (buttonReleased) {
			buttonDoublePress = true;
			if (digitalRead(buttonPin) != buttonPressedState)
				break;
		}
	}

	return (buttonDoublePress ? BUTTON_DOUBLEPRESS : BUTTON_PRESS);

}
