// Copyright 2019 Blues Inc.  All rights reserved.
// License <here>.
// (Use Adafruit BMP085 as a good structural example.)

#include <Notecard.h>

// If the Notecard is connected to a Serial port, define it here.  If the Notecard is connected using
// the I2C interface, leave this commented out.
//#define serialPort Serial2
  
// Tell the Notehub which of its accounts manages this device, and configure how often to upload.
void setup() {

	// Set up for debug output.  If you open Arduino's serial terminal window, you'll be able to
	// watch JSON objects being transferred to and from the Notecard for each request.
	Serial.begin(115200);
    NoteSetDebugOutputPort(&Serial);

    // Initialize the physical I/O channel to the Notecard
#ifdef serialPort
    serialPort.begin(9600);
    NoteInitSerial(&serialPort);
#else
    NoteInitI2C();
#endif

    // "NoteNewRequest()" uses the bundled "J" json package to allocate a "req", which is a JSON object
	// for the request to which we will then add Request arguments.
    J *req = NoteNewRequest("service.set");

	// This argument causes the data to be delivered to the Personal Project in our notehub.io account.
    JAddStringToObject(req, "product", "YOUR_EMAIL_REGISTERED_WITH_NOTEHUB.IO");

	// This argument causes the modem to power on and connect continuously to the notehub.io service
    JAddStringToObject(req, "mode", "continuous");

	// Issue the request, telling the Notecard how and how often to access the service. 
    NoteRequest(req);

}
  
// Repeatedly, get some sensor data from the Notecard's own internal sensors, and enqueue 
// that data as a new Note within a Notefile that will then be synchronized with the service.
// We can name the JSON fields and the  notefile anything we like.  We add the ".qo" extension
// to the Notefile name to indicate to the Notecard that this is an "outbound queue" of Notes.
void loop() {
	static unsigned messagesSent = 0;
    double temperature, voltage;

	// Use the temperature sensor on the Notecard to get a sample temperature measurement
	NoteGetTemperature(&temperature);

    // Use the voltage ADC on the Notecard to get a sample voltage measurement
	NoteGetVoltage(&voltage);

    // Enqueue the measurement to the Notecard for transmission to the Notehub, indicating that
	// this is "urgent" so that the data synchronizes immediately rather than being deferred.
    J *body = JCreateObject();
    JAddNumberToObject(body, "temperature", temperature);
    JAddNumberToObject(body, "voltage", voltage);
    JAddNumberToObject(body, "count", ++messagesSent);
	NoteSend("sensors.qo", body, true);

    // Delay between measurements
    delay(15000);

}
