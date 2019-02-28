// Copyright 2019 Blues Inc.  All rights reserved.
// License <here>.
// (Use Adafruit BMP085 as a good structural example.)

#include <Notecard.h>

// The serial port connected to the Notecard.  If this is not defined, I2C is used instead.
#define serial Serial2
  
// Tell the Notehub which of its accounts manages this device, and configure how often to upload.
void setup() {

    // Initialize the physical I/O channel to the Notecard
#ifdef serial
    serial.begin(9600);
    notecardInitSerial(&serial);
#else
    notecardInitI2C();
#endif

    // Tell the Notecard how and how often to access the service.  "RequestBegin()" initializes 
    // the J json package and allocates "req", a JSON object for the request.  Then, "Request()" 
    // sends the JSON request to the Notecard and allocates "rsp", a JSON object for the response.
    // Finally, RequestEnd() deallocates the JSON objects associated with the request and response.
    J *req = NotecardNewRequest("service.set");
    JAddStringToObject(req, "product", "ray@ozzie.net");
    JAddNumberToObject(req, "minutes", 15);
    J *rsp = notecardTransaction(req);
    JDelete(req);
    JDelete(rsp);

}
  
// Once every minute, randomly simulate a temperature and humidity sensor measurement, and add it
// as a new Note within a new or existing "air.qo" Notefile - an outbound queue as indicated by ".qo".
void loop() {

    // Simulate taking a sensor measurement
    int temperature = random(5, 25);
    int humidity = random(30, 60);

    // Enqueue the simulated measurement to the Notecard.  
    J *req = NotecardNewRequest("note.add");
    JAddStringToObject(req, "file", "air.qo");
    J *body = JCreateObject();
    JAddNumberToObject(body, "temp", temperature);
    JAddNumberToObject(body, "humid", humidity);
    JAddItemToObject(req, "body", body);
    J *rsp = notecardTransaction(req);
    JDelete(req);
    JDelete(rsp);

    // Simulate a delay between measurements
    delay(60000);

}
