// Copyright 2022 Blues Inc.  All rights reserved.
//
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.
//
// This tutorial requires a Notecarrier-F (or equivalently-wired carrier board)
// designed enable the Notecard's ATTN pin to control a host MCU's power supply.

#include <Notecard.h>

// This is the unique Product Identifier for your device
#ifndef PRODUCT_UID
#define PRODUCT_UID "" // "com.my-company.my-name:my-project"
#pragma message "PRODUCT_UID is not defined in this example. Please ensure your Notecard has a product identifier set before running this example or define it in code here. More details at https://dev.blues.io/tools-and-sdks/samples/product-uid"
#endif

// Parameters for this example
#define myProductID PRODUCT_UID
#define notehubUploadPeriodMins 10
#define hostSleepSeconds 60

// Arduino serial debug monitor port definitions.
// Note this is using the TX/RX pins for logging and not the USB connector. An
// FTDI cable is required to read the logs produced by this sketch. Power is
// delivered over USB, which interferes with the way the Notecard puts the
// Feather MCU to sleep. As a result, USB cannot be used to capture logs.
#ifdef ARDUINO_AVR_UNO
  #define txRxSerial Serial
#elif ARDUINO_FEATHER_F405
  // https://github.com/stm32duino/Arduino_Core_STM32/issues/1990
  #define txRxSerial Serial3
#elif ARDUINO_NUCLEO_L432KC
  #define txRxSerial Serial2
#else
  #define txRxSerial Serial1
#endif

// Notecard I2C port definitions
Notecard notecard;

// When the Notecard puts the host MCU to sleep, it enables the host to save
// 'state' inside the Notecard while it's asleep, and to retrieve this state
// when it awakens.  These are several 'segments' of state that may individually
// be saved.
struct
{
    int cycles;
} globalState;
const char globalSegmentID[] = "GLOB";

struct
{
    int measurements;
} tempSensorState;
const char tempSensorSegmentID[] = "TEMP";

struct
{
    int measurements;
} voltSensorState;
const char voltSensorSegmentID[] = "VOLT";

// One-time Arduino initialization
void setup()
{
    // Set up for debug output (if available).
#ifdef txRxSerial
    // If you open Arduino's serial terminal window, you'll be able to watch
    // JSON objects being transferred to and from the Notecard for each request.
    txRxSerial.begin(115200);
    const size_t usb_timeout_ms = 3000;
    for (const size_t start_ms = millis(); !txRxSerial && (millis() - start_ms) < usb_timeout_ms;)
        ;

    // For low-memory platforms, don't turn on internal Notecard logs.
#ifndef NOTE_C_LOW_MEM
    notecard.setDebugOutputStream(txRxSerial);
#else
#pragma message("INFO: Notecard debug logs disabled. (non-fatal)")
#endif // !NOTE_C_LOW_MEM
#endif // txRxSerial


    // Initialize the physical I2C I/O channel to the Notecard
    notecard.begin();

    // Determine whether or not this is a 'clean boot', or if we're
    // restarting after having been put to sleep by the Notecard.
    NotePayloadDesc payload;
    bool retrieved = NotePayloadRetrieveAfterSleep(&payload);

    // If the payload was successfully retrieved, attempt to restore state from
    // the payload
    if (retrieved)
    {
        // Restore the various state data structures
        retrieved &= NotePayloadGetSegment(&payload, globalSegmentID, &globalState, sizeof(globalState));
        retrieved &= NotePayloadGetSegment(&payload, tempSensorSegmentID, &tempSensorState, sizeof(tempSensorState));
        retrieved &= NotePayloadGetSegment(&payload, voltSensorSegmentID, &voltSensorState, sizeof(voltSensorState));

        // We're done with the payload, so we can free it
        NotePayloadFree(&payload);
    }

    // If this is our first time through, initialize the Notecard and state
    if (!retrieved)
    {

        // Initialize operating state
        memset(&globalState, 0, sizeof(globalState));
        memset(&tempSensorState, 0, sizeof(tempSensorState));
        memset(&voltSensorState, 0, sizeof(voltSensorState));

        // Initialize the Notecard
        J *req = notecard.newRequest("hub.set");
        if (myProductID[0])
        {
            JAddStringToObject(req, "product", myProductID);
        }
        JAddStringToObject(req, "mode", "periodic");
        JAddNumberToObject(req, "outbound", notehubUploadPeriodMins);
        notecard.sendRequestWithRetry(req, 5); // 5 seconds

        // Because many devs will be using oscilloscopes or joulescopes to
        // closely examine power consumption, it can be helpful during
        // development to provide a stable and repeatable power consumption
        // environment. In the Notecard's default configuration, the
        // accelerometer is 'on'.  As such, when debugging, devs may see tiny
        // little blips from time to time on the scope. These little blips are
        // caused by accelerometer interrupt processing, when developers
        // accidentally tap the notecard or carrier.  As such, to help during
        // development and measurement, this request disables the accelerometer.
        req = notecard.newRequest("card.motion.mode");
        JAddBoolToObject(req, "stop", true);
        notecard.sendRequest(req);
    }
}

void loop()
{
    // Bump the number of cycles
    if (++globalState.cycles > 25)
    {
        txRxSerial.println("[APP] Demo cycle complete. Program stopped. Press RESET to restart.");
        delay(10000); // 10 seconds
        return;
    }

    // Simulation of a device taking a measurement of a temperature sensor.
    // Because we don't have an actual external hardware sensor in this example,
    // we're just retrieving the internal surface temperature of the Notecard.
    double currentTemperature = 0.0;
    J *rsp = notecard.requestAndResponse(notecard.newRequest("card.temp"));
    if (rsp != NULL)
    {
        currentTemperature = JGetNumber(rsp, "value");
        notecard.deleteResponse(rsp);
        tempSensorState.measurements++;
    }

    // Simulation of a device taking a measurement of a voltage sensor. Because
    // we don't have an actual external hardware sensor in this example, we're
    // just retrieving the battery voltage being supplied to the Notecard.
    double currentVoltage = 0.0;
    rsp = notecard.requestAndResponse(notecard.newRequest("card.voltage"));
    if (rsp != NULL)
    {
        currentVoltage = JGetNumber(rsp, "value");
        notecard.deleteResponse(rsp);
        voltSensorState.measurements++;
    }

    // Add a note to the Notecard containing the sensor readings
    J *req = notecard.newRequest("note.add");
    if (req != NULL)
    {
        JAddStringToObject(req, "file", "example.qo");
        J *body = JAddObjectToObject(req, "body");
        if (body != NULL)
        {
            JAddNumberToObject(body, "cycles", globalState.cycles);
            JAddNumberToObject(body, "temperature", currentTemperature);
            JAddNumberToObject(body, "temperature_measurements", tempSensorState.measurements);
            JAddNumberToObject(body, "voltage", currentVoltage);
            JAddNumberToObject(body, "voltage_measurements", voltSensorState.measurements);
        }
        notecard.sendRequest(req);
    }

    // Put ourselves back to sleep for a fixed period of time
    NotePayloadDesc payload = {0, 0, 0};
    NotePayloadAddSegment(&payload, globalSegmentID, &globalState, sizeof(globalState));
    NotePayloadAddSegment(&payload, voltSensorSegmentID, &voltSensorState, sizeof(voltSensorState));
    NotePayloadAddSegment(&payload, tempSensorSegmentID, &tempSensorState, sizeof(tempSensorState));
    NotePayloadSaveAndSleep(&payload, hostSleepSeconds, NULL);

    // We should never return here, because the Notecard put us to sleep. If we
    // do get here, it's because the Notecarrier was configured to supply power
    // to this host MCU without being switched by the ATTN pin.
    delay(15000);
}
