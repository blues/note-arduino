// Copyright 2019 Blues Inc.  All rights reserved.
//
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.
//
// This example does the same function as the basic "Using Library" example, but
// demonstrates how best to use the Notecard API when your app uses either:
// a) higher-frequency sampling
// b) binary samples
//
// When using the standard Notecard "note.add" request for sending data to the
// service through outbound queues (notefiles ending in .qo), the Notecard
// limits the number of "pending notes" (the number of notes waiting to be sent
// to the service) to 100, because under these common circumstances the pending
// notes are all buffered in memory.
//
// When a customer application generates data at a higher frequency, or a longer
// sync period, such that more than 100 notes will be "waiting" to be uploaded,
// the developer should use a feature known as the "Note Template". An
// application using a Note Template declares once, up-front, what the JSON
// object will 'look like' in terms of JSON fields, data types, and data length.
// By doing this, the Notecard can store the data for each "note.add" directly
// into a region of Flash storage permitting more than 100 notes to be pending
// for subsequent upload.
//
// This example shows how a developer would declare and set a template, both for
// their own JSON object as well as for their own binary payload. If a binary
// payload is not needed, it can simply be eliminated from the example.

#include <stdlib.h>

#include <Notecard.h>

// Note that both of these definitions are optional; just prefix either line
// with `//` to remove it.
// - Remove txRxPinsSerial if you wired your Notecard using I2C SDA/SCL pins
//   instead of serial RX/TX
// - Remove usbSerial if you don't want the Notecard library to output debug
//   information

// #define txRxPinsSerial Serial1
#define usbSerial Serial

// This is the unique Product Identifier for your device.  This Product ID tells
// the Notecard what type of device has embedded the Notecard, and by extension
// which vendor or customer is in charge of "managing" it. In order to set this
// value, you must first register with notehub.io and "claim" a unique product
// ID for your device.  It could be something as simple as as your email address
// in reverse, such as "com.gmail.smith.lisa.test-device" or
// "com.outlook.gates.bill.demo"

// This is the unique Product Identifier for your device
#ifndef PRODUCT_UID
#define PRODUCT_UID "" // "com.my-company.my-name:my-project"
#pragma message "PRODUCT_UID is not defined in this example. Please ensure your Notecard has a product identifier set before running this example or define it in code here. More details at https://dev.blues.io/tools-and-sdks/samples/product-uid"
#endif

#define myProductID PRODUCT_UID
Notecard notecard;

// A sample binary object, just for binary payload simulation
struct myBinaryPayload
{
    double temp;
    double voltage;
};

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

    // "notecard.newRequest()" uses the bundled "J" json package to allocate a
    //"req", which is a JSON object for the request to which we will then add
    // Request arguments.  The function allocates a "req" request structure
    // using malloc() and initializes its "req" field with the type of request.
    J *req = notecard.newRequest("hub.set");

    // This command (required) causes the data to be delivered to the Project on
    // notehub.io that has claimed this Product ID (see above).
    if (myProductID[0])
    {
        JAddStringToObject(req, "product", myProductID);
    }

    // This command determines how often the Notecard connects to the service.
    JAddStringToObject(req, "mode", "periodic");
    JAddNumberToObject(req, "outbound", 5);

    // Issue the request, telling the Notecard how and how often to access the
    // service. This results in a JSON message to Notecard formatted like:
    //     {
    //       "req"     : "service.set",
    //       "product" : myProductID,
    //       "mode"    : "continuous"
    //     }
    // Note that `notecard.sendRequestWithRetry()` always frees the request data
    // structure, and it returns "true" if success or "false" if there is any
    // failure. It is important to use `sendRequestWithRetry()` on the first
    // message from the MCU to the Notecard, because there will always be a
    // hardware race condition on cold boot and the Notecard must be ready to
    // receive and process the message.
    notecard.sendRequestWithRetry(req, 5); // 5 seconds

    // Create a template Note that we will register. This template note will
    // look "similar" to the Notes that will later be added with note.add, in
    // that the data types are used to intuit what the ultimate field data types
    // will be, and their maximum length.
    req = notecard.newRequest("note.template");
    if (req != NULL)
    {
        // Create the body for a template that will be used to send notes below
        J *body = JAddObjectToObject(req, "body");
        if (body != NULL)
        {
            // Define the JSON template
            JAddStringToObject(body, "status", TSTRING(12)); // maximum string length
            JAddNumberToObject(body, "temp", TFLOAT64);      // floating point (double)
            JAddNumberToObject(body, "voltage", TFLOAT64);   // floating point (double)
            JAddNumberToObject(body, "count", TUINT32);      // 32-bit unsigned integer
        }

        // Create a template of the payload that will be used to send
        // notes below
        JAddNumberToObject(req, "length", sizeof(myBinaryPayload));

        // Register the template in the output queue notefile
        JAddStringToObject(req, "file", "sensors.qo");
        notecard.sendRequest(req);
    }
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

    // Rather than simulating a temperature reading, use a Notecard request to
    // read the temp from the Notecard's built-in temperature sensor. We use
    // `notecard.requestAndResponse()` to indicate that we would like to examine
    // the response of the transaction. This method takes a JSON data structure,
    // "request", as input, then processes it and returns a JSON data structure,
    // "response" with the response.  Note that because the Notecard library
    // uses malloc(), developers must always check for NULL to ensure that there
    // was enough memory available on the microcontroller to satisfy the
    // allocation request.
    double temperature = 0;
    J *rsp = notecard.requestAndResponse(notecard.newRequest("card.temp"));
    if (rsp != NULL)
    {
        temperature = JGetNumber(rsp, "value");
        notecard.deleteResponse(rsp);
    }

    // Do the same to retrieve the voltage that is detected by the Notecard on
    // its `V+` pin.
    double voltage = 0;
    rsp = notecard.requestAndResponse(notecard.newRequest("card.voltage"));
    if (rsp != NULL)
    {
        voltage = JGetNumber(rsp, "value");
        notecard.deleteResponse(rsp);
    }

    // Add a binary data structure to the simulation
    struct myBinaryPayload binaryData;
    binaryData.temp = temperature;
    binaryData.voltage = voltage;

    // Enqueue the measurement to the Notecard for transmission to the Notehub
    J *req = notecard.newRequest("note.add");
    if (req != NULL)
    {
        JAddStringToObject(req, "file", "sensors.qo");
        J *body = JAddObjectToObject(req, "body");
        if (body != NULL)
        {
            JAddStringToObject(body, "status", temperature > 26.67 ? "hot" : "normal"); // 80F
            JAddNumberToObject(body, "temp", temperature);
            JAddNumberToObject(body, "voltage", voltage);
            JAddNumberToObject(body, "count", eventCounter);
        }
        JAddBinaryToObject(req, "payload", &binaryData, sizeof(binaryData));
        notecard.sendRequest(req);
    }

    // Delay between measurements
    delay(15 * 1000); // 5 seconds
}
