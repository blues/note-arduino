//
// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.
//
// This example contains the complete source for the Sensor Tutorial at dev.blues.io
// https://dev.blues.io/build/tutorials/sensor-tutorial/notecarrier-af/esp32/arduino-wiring/
//
// This tutorial requires an external Adafruit BME680 Sensor.
//

// Include the Arduino library for the Notecard
#include <Notecard.h>

#include <Wire.h>
#include <Adafruit_BME680.h>

Adafruit_BME680 bmeSensor;

//#define serialNotecard Serial1
#define serialDebug Serial

#define productUID "com.your-company.your-name:your_project"
Notecard notecard;

// One-time Arduino initialization
void setup() {
#ifdef serialDebug
    delay(2500);
    serialDebug.begin(115200);
    notecard.setDebugOutputStream(serialDebug);
#endif

	// Initialize the physical I/O channel to the Notecard
#ifdef serialNotecard
	notecard.begin(serialNotecard, 9600);
#else
	Wire.begin();

	notecard.begin();
#endif

  J *req = notecard.newRequest("hub.set");
  JAddStringToObject(req, "product", productUID);
  JAddStringToObject(req, "mode", "continuous");
  notecard.sendRequest(req);

  if (!bmeSensor.begin()) {
    serialDebug.println("Could not find a valid BME680 sensor...");
  } else {
    serialDebug.println("BME680 Connected...");
  }

  bmeSensor.setTemperatureOversampling(BME680_OS_8X);
  bmeSensor.setHumidityOversampling(BME680_OS_2X);
}

void loop() {
  if (! bmeSensor.performReading()) {
    serialDebug.println("Failed to obtain a reading...");
    delay(15000);
    return;
  }

  serialDebug.print("Temperature = ");
  serialDebug.print(bmeSensor.temperature);
  serialDebug.println(" *C");

  serialDebug.print("Humidity = ");
  serialDebug.print(bmeSensor.humidity);
  serialDebug.println(" %");

  J *req = notecard.newRequest("note.add");
  if (req != NULL) {
    JAddStringToObject(req, "file", "sensors.qo");
    JAddBoolToObject(req, "start", true);

    J *body = JCreateObject();
    if (body != NULL) {
      JAddNumberToObject(body, "temp", bmeSensor.temperature);
      JAddNumberToObject(body, "humidity", bmeSensor.humidity);
       JAddItemToObject(req, "body", body);
    }

    notecard.sendRequest(req);
  }

  delay(15000);
}
