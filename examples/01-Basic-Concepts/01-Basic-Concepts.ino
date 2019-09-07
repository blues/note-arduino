//
// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.
//
// This is the simplest example of how a device may send commands to the Notecard over a serial
// port by using nothing but simple "print line" functions targeting that Arduino serial port.
//

// This configuration parameter defines which serial port on your Arduino device is connected to
// the Notecard.  For example, if you are using an M5Stack Basic Core IoT Development Kit, you
// would connect the R2 pin to the Notecard's TX pin, and the M5Stack's T2 pin to the Notecard's RX pin,
// and then set these #defines to use Serial2 below.

#define notecard Serial2

// This is the unique Product Identifier for your device.  This Product ID tells the Notecard what
// type of device has embedded the Notecard, and by extension which vendor or customer is in charge
// of "managing" it.  In order to set this value, you must first register with notehub.io and
// "claim" a unique product ID for your device.	 It could be something as simple as as your email
// address in reverse, such as "com.gmail.smith.lisa.test-device" or "com.outlook.gates.bill.demo"

#define myProductID "org.coca-cola.soda.vending-machine.v2"

// One-time Arduino initialization
void setup() {

	// Initialize the serial port being used by the Notecard, and send newlines to clear out any data
	// that the Arduino software may have pending so that we always start sending commands "cleanly".
	// We use the speed of 9600 because the Notecard's RX/TX pins are always configured for that speed.
	notecard.begin(9600);
	notecard.println("\n");

	// This command (required) causes the data to be delivered to the Project on notehub.io that has claimed
	// this Product ID.	 (see above)
	notecard.println("{\"req\":\"service.set\",\"product\":\"" myProductID "\"}");

	// This command (optional) command causes the Notecard to immediately establish a session with
	// the service at notehub.io, and to keep it active continuously.  By default, without this, the
	// Notecard will connect periodically (hourly) with the service if any data is pending to be uploaded.
	notecard.println("{\"req\":\"service.set\",\"mode\":\"continuous\"}");

}

// In the Arduino main loop which is called repeatedly, add outbound data every 15 seconds
void loop() {

	// Simulate an event counter of some kind
	static unsigned eventCounter = 0;
	eventCounter = eventCounter + 1;

	// Simulate a temperature reading, between 5.0 and 35.0 degrees C
	double temperature = (double) random(50, 350) / 10.0;

	// Simulate a voltage reading, between 3.1 and 4.2 degrees
	double voltage = (double) random(31, 42) / 10.0;

	// Add a "note" to the Notecard, in a queue that we will choose to name "sensor.qo" because it will
	// contain our simulated "sensor data" and it is to be placed in a "queue" that is "outbound".
	// The "body" of the note is a JSON object completely of our own design, and is passed straight
	// through as-is to notehub.io and beyond.	(Note that we add the "start" flag for demonstration
	// purposes to upload the data instantaneously, so that if you are looking at this on notehub.io
	// you will see the data appearing 'live'.)
	char message[150];
	snprintf(message, sizeof(message),
			 "{"
			 "\"req\":\"note.add\""
			 ","
			 "\"start\":true"
			 ","
			 "\"file\":\"sensor.qo\""
			 ","
			 "\"body\":{\"temp\":%f,\"voltage\":%f,\"count\":%d}"
			 "}",
			 temperature, voltage, eventCounter);
	notecard.println(message);

	// Delay between simulatedmeasurements
	delay(15000);

}
