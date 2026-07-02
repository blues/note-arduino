// Copyright 2026 Blues Inc.  All rights reserved.
//
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.
//
// This example demonstrates Notecard Outboard Firmware Update, which lets you
// update this host MCU's firmware over-the-air from Notehub, with no host code
// required to perform the download. The Notecard receives the new firmware
// image and reprograms the host over the DFU signals on a Notecarrier F.
//
// The sketch does two things:
//
//   1. In setup(), it configures the Notecard to enable Outboard Firmware
//      Update (see the `card.dfu`, `card.aux` and `dfu.status` requests below).
//   2. In loop(), it blinks the built-in LED with a recognizable pattern and
//      reports a firmware version. This is the "payload" you will update: build
//      and upload a new version of this sketch to Notehub, apply it to the
//      host, and watch the blink pattern (and reported version) change.
//
// This example targets STM32-based Blues hosts on a Notecarrier F, such as the
// Swan and Cygnet Feathers and the STM32F405 Feather. See DFU_MCU_TYPE below
// for hosts (like the SparkFun MicroMod STM32) whose boot pin is inverted.
//
// For the full walkthrough, see:
// https://dev.blues.io/notehub/host-firmware-updates/notecard-outboard-firmware-update/

// Include the Arduino library for the Notecard
#include <Notecard.h>

// If the Notecard is connected to a serial port, define it here.  For example,
// if you are using the Adafruit Feather NRF52840 Express, the RX/TX pins (and
// thus the Notecard) are on Serial1. However, if you are using an M5Stack Basic
// Core IoT Development Kit, you would connect the R2 pin to the Notecard's TX
// pin, and the M5Stack's T2 pin to the Notecard's RX pin, and then would use
// Serial2.
//
// Also, you may define a debug output port where you can watch transactions as
// they are sent to and from the Notecard.  When using the Arduino IDE this is
// typically "Serial", but you can use any available port.
//
// Note that both of these definitions are optional; just prefix either line
// with `//` to remove it.
//
// - Remove `txRxPinsSerial` if you wired your Notecard using I2C SDA/SCL pins,
//   instead of serial RX/TX.
// - Remove `usbSerial` if you don't want the Notecard library to output debug
//   information.

// #define txRxPinsSerial Serial1
#define usbSerial Serial

// This is the unique Product Identifier for your device
#ifndef PRODUCT_UID
#define PRODUCT_UID "" // "com.my-company.my-name:my-project"
#pragma message "PRODUCT_UID is not defined in this example. Please ensure your Notecard has a product identifier set before running this example or define it in code here. More details at https://dev.blues.io/tools-and-sdks/samples/product-uid"
#endif

#define myProductID PRODUCT_UID

// The host MCU type reported to the Notecard via `card.dfu`. Use "stm32" for
// most STM32 hosts (Swan, Cygnet, STM32F405 Feather). Use "stm32-bi" for hosts
// whose boot pin is active LOW / inverted, such as the SparkFun MicroMod STM32.
#ifndef DFU_MCU_TYPE
#define DFU_MCU_TYPE "stm32"
#endif

// The firmware version reported to Notehub via `dfu.status`. Bump this (and
// change the blink pattern below) when you build a new image to update to.
#define FIRMWARE_VERSION "1.0.0"

Notecard notecard;

// One-time Arduino initialization
void setup()
{
    // Set up the built-in LED so loop() can show a recognizable running pattern.
    pinMode(LED_BUILTIN, OUTPUT);

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

    // Put the Notecard in continuous mode so it maintains a live session with
    // Notehub. Outboard Firmware Update requires the Notecard to be in
    // "continuous" or "periodic" mode. `sendRequestWithRetry()` is important on
    // the first message after a cold boot, to handle the hardware race
    // condition while the Notecard becomes ready.
    J *req = notecard.newRequest("hub.set");
    if (myProductID[0])
    {
        JAddStringToObject(req, "product", myProductID);
    }
    JAddStringToObject(req, "mode", "continuous");
    notecard.sendRequestWithRetry(req, 5); // 5 seconds

    // Enable Outboard Firmware Update for this host and tell the Notecard which
    // MCU type it is driving. On a Notecarrier F the DFU signals are routed over
    // the Notecard's shared AUX pins, so we set `mode` to "aux" here.
    req = notecard.newRequest("card.dfu");
    JAddStringToObject(req, "name", DFU_MCU_TYPE);
    JAddBoolToObject(req, "on", true);
    JAddStringToObject(req, "mode", "aux");
    notecard.sendRequest(req);

    // Free the AUX pins so they can be used for Outboard Firmware Update.
    req = notecard.newRequest("card.aux");
    JAddStringToObject(req, "mode", "off");
    notecard.sendRequest(req);

    // Enable host DFU and report the running firmware version to Notehub.
    req = notecard.newRequest("dfu.status");
    JAddBoolToObject(req, "on", true);
    JAddStringToObject(req, "version", FIRMWARE_VERSION);
    notecard.sendRequest(req);
}

// In the Arduino main loop, blink the built-in LED with a recognizable pattern.
// After an Outboard Firmware Update, change this pattern (and bump
// FIRMWARE_VERSION above) so you can visually confirm the update was applied.
void loop()
{
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
}
