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

//#define txRxPinsSerial Serial1
#define usbSerial Serial

#define productUID "com.your-company.your-name:your_project"
Notecard notecard;

// One-time Arduino initialization
void setup()
{
#ifdef usbSerial
    delay(2500);
    usbSerial.begin(115200);
    notecard.setDebugOutputStream(usbSerial);
#endif

    // Initialize the physical I/O channel to the Notecard
#ifdef txRxPinsSerial
    notecard.begin(txRxPinsSerial, 9600);
#else
    Wire.begin();

    notecard.begin();
#endif

    J *req = notecard.newRequest("hub.set");
    JAddStringToObject(req, "product", productUID);
    JAddStringToObject(req, "mode", "continuous");
    notecard.sendRequest(req);

    if (!bmeSensor.begin()) {
        usbSerial.println("Could not find a valid BME680 sensor...");
    } else {
        usbSerial.println("BME680 Connected...");
    }

    bmeSensor.setTemperatureOversampling(BME680_OS_8X);
    bmeSensor.setHumidityOversampling(BME680_OS_2X);
}

void loop()
{
    if (! bmeSensor.performReading()) {
        usbSerial.println("Failed to obtain a reading...");
        delay(15000);
        return;
    }

    usbSerial.print("Temperature = ");
    usbSerial.print(bmeSensor.temperature);
    usbSerial.println(" *C");

    usbSerial.print("Humidity = ");
    usbSerial.print(bmeSensor.humidity);
    usbSerial.println(" %");

    J *req = notecard.newRequest("note.add");
    if (req != NULL) {
        JAddStringToObject(req, "file", "sensors.qo");
        JAddBoolToObject(req, "sync", true);

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
