// Copyright 2019 Blues Inc.  All rights reserved.
//
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.
//
// This example contains the complete source for the Sensor Tutorial at
// https//blues.dev
// https://blues.dev/guides-and-tutorials/collecting-sensor-data/notecarrier-f/blues-wireless-swan/c-cpp-arduino-wiring/

// Include the Arduino library for the Notecard
#include <Notecard.h>
#include <NotecardPseudoSensor.h>

// #define txRxPinsSerial Serial1
#define usbSerial Serial

// This is the unique Product Identifier for your device
#ifndef PRODUCT_UID
#define PRODUCT_UID "" // "com.my-company.my-name:my-project"
#pragma message "PRODUCT_UID is not defined in this example. Please ensure your Notecard has a product identifier set before running this example or define it in code here. More details at https://dev.blues.io/tools-and-sdks/samples/product-uid"
#endif

#define myProductID PRODUCT_UID

using namespace blues;

Notecard notecard;
NotecardPseudoSensor sensor(notecard);

// One-time Arduino initialization
void setup()
{
    // Set up for debug output (if available).
#ifdef usbSerial
    // If you open Arduino's serial terminal window, you'll be able to watch
    // JSON objects being transferred to and from the Notecard for each request.
    usbSerial.begin(115200);
    const size_t usb_timeout_ms = 3000;
    for (const size_t start_ms = millis(); !usbSerial && (millis() - start_ms) < usb_timeout_ms;)
        ;

    // For low-memory platforms, don't turn on internal Notecard logs.
#ifndef NOTE_C_LOW_MEM
    notecard.setDebugOutputStream(usbSerial);
#else
#pragma message("INFO: Notecard debug logs disabled. (non-fatal)")
#endif // !NOTE_C_LOW_MEM
#endif // usbSerial

    // Initialize the physical I/O channel to the Notecard
#ifdef txRxPinsSerial
    notecard.begin(txRxPinsSerial, 9600);
#else
    notecard.begin();
#endif

    J *req = notecard.newRequest("hub.set");
    if (myProductID[0])
    {
        JAddStringToObject(req, "product", myProductID);
    }
    JAddStringToObject(req, "mode", "continuous");
    notecard.sendRequestWithRetry(req, 5); // 5 seconds
}

// In the Arduino main loop which is called repeatedly, add outbound data every
// 15 seconds
void loop()
{
    // Count the simulated measurements that we send to the cloud, and stop the
    // demo before long.
    static unsigned eventCounter = 0;
    if (++eventCounter > 25)
    {
        usbSerial.println("[APP] Demo cycle complete. Program stopped. Press RESET to restart.");
        delay(10000); // 10 seconds
        return;
    }

    float temperature = sensor.temp();
    float humidity = sensor.humidity();

    usbSerial.print("[APP] Temperature = ");
    usbSerial.print(temperature);
    usbSerial.println(" *C");
    usbSerial.print("[APP] Humidity = ");
    usbSerial.print(humidity);
    usbSerial.println(" %");

    J *req = notecard.newRequest("note.add");
    if (req != NULL)
    {
        JAddStringToObject(req, "file", "sensors.qo");
        JAddBoolToObject(req, "sync", true);
        J *body = JAddObjectToObject(req, "body");
        if (body)
        {
            JAddNumberToObject(body, "temp", temperature);
            JAddNumberToObject(body, "humidity", humidity);
        }
        notecard.sendRequest(req);
    }

    delay(15000);
}
