// Copyright 2019 Blues Inc.  All rights reserved.
// License <here>.
// (Use Adafruit BMP085 as a good structural example.)

// The serial port connected to the Notecard
#define serial Serial2

// Tell the Notehub which of its accounts manages this device, and configure how often to upload.
void setup() {
    serial.begin(9600);
    serial.println("{\"req\":\"service.set\",\"product\":\"ray@ozzie.net\",\"minutes\":15}");
}

// Once every minute, randomly simulate a temperature and humidity sensor measurement, and add it
// as a new Note within a new or existing "air.qo" Notefile - an outbound queue as indicated by ".qo".
void loop() {
    char request[150];
    snprintf(request, sizeof(request),
             "{\"req\":\"note.add\",\"file\":\"air.qo\",\"body\":{\"temp\":%d,\"humid\":%d}}",
             random(5, 25),     // Simulated Temperature
             random(30, 60));   // Simulated Humidity
    serial.println(request);
    delay(60000);
}
