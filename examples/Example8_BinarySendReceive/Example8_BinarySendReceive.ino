// Copyright 2023 Blues Inc. All rights reserved.
//
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.
//
// This example exercises the Notecard's ability to send and receive a binary

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <Notecard.h>

// Note that both of these definitions are optional; just prefix either line
// with `//` to remove it.
// - Remove txRxPinsSerial if you wired your Notecard using I2C SDA/SCL pins
//   instead of serial RX/TX
// - Remove usbSerial if you don't want the Notecard library to output debug
//   information

// #define txRxPinsSerial Serial1
#define usbSerial Serial

// This is the unique Product Identifier for your device
#ifndef PRODUCT_UID
#define PRODUCT_UID "" // "com.my-company.my-name:my-project"
#pragma message "PRODUCT_UID is not defined in this example. Please ensure your Notecard has a product identifier set before running this example or define it in code here. More details at https://dev.blues.io/tools-and-sdks/samples/product-uid"
#endif

#define myProductID PRODUCT_UID
Notecard notecard;

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
    notecard.setDebugOutputStream(usbSerial);
#endif

    // Initialize the physical I/O channel to the Notecard
#ifdef txRxPinsSerial
    notecard.begin(txRxPinsSerial, 9600);
#else
    notecard.begin();
#endif

    // Configure the productUID, and instruct the Notecard to stay connected to
    // the service.
    J *req = notecard.newRequest("hub.set");
    if (myProductID[0])
    {
        JAddStringToObject(req, "product", myProductID);
    }
    JAddStringToObject(req, "mode", "continuous");
    notecard.sendRequestWithRetry(req, 5); // 5 seconds

    // Reset the state of the Notecard's binary data store to a known value.
    NoteBinaryReset();
}

// In the Arduino main loop which is called repeatedly, add outbound data every
// 30 seconds
void loop()
{
    // Stop the demo after five iterations to conserve data
    static unsigned event_counter = 0;
    if (++event_counter > 5)
    {
        while(1);
    }

    // Send data to the Notecard storage and pull it back
    {
        /////////////////////////////////////////////////
        // Transmit that beautiful bean footage
        /////////////////////////////////////////////////
        char data[64] = "https://youtu.be/0epWToAOlFY?t=21";
        size_t data_len = strlen(data);
        const size_t notecard_binary_area_offset = 0;
        NoteBinaryTransmit(reinterpret_cast<uint8_t *>(data), data_len, sizeof(data), notecard_binary_area_offset);
        notecard.logDebugf("\n[INFO] Transmitted %d bytes.\n", data_len);

        // Log for the sake of curiosity
        notecard.logDebug("\n*** Encoded Binary Transmission ***\n");
        size_t tx_len = NoteBinaryEncodedLength(reinterpret_cast<uint8_t *>(data), data_len);
        for (size_t i = 0 ; i < tx_len ; ++i) {
            notecard.logDebugf("%02x ", data[i]);
            if ((i + 1) % 16 == 0) {
                notecard.logDebug("\n");
            }
        }
        notecard.logDebug("\n*** Encoded Binary Transmission ***\n\n");

        /////////////////////////////////////////////////
        // Receive data from the Notecard binary data store
        /////////////////////////////////////////////////
        size_t rx_buffer_len = 0;
        NoteBinaryRequiredRxMaxBuffer(&rx_buffer_len);
        uint8_t *rx_buffer = (uint8_t *)malloc(rx_buffer_len);
        data_len = NOTE_C_BINARY_RX_ALL; // NOTE_C_BINARY_RX_ALL is a special value
                                         // meaning "return all bytes from offset"
        NoteBinaryReceive(reinterpret_cast<uint8_t *>(rx_buffer), rx_buffer_len, notecard_binary_area_offset, &data_len);
        notecard.logDebugf("\n[INFO] Received %d bytes.\n", data_len);

        // Display received buffer
        notecard.logDebug("\n*** Decoded Data ***\n");
        for (size_t i = 0 ; i < data_len ; ++i) {
            notecard.logDebugf("%c", rx_buffer[i]);
        }
        notecard.logDebug("\n*** Decoded Data ***\n\n");

        // NOTE: The binary data store is not cleared on receive, which
        //       allows us to submit it to Notehub in the next step.
    }

    // Send it to Notehub
    {
        // Submit binary object to the Notehub using `note.add`. This will send
        // the binary to Notehub in the payload field of the Note. The payload
        // will not be visible in the Notehub UI, but the data will be forwarded
        // to any pre-configured routes.
        if (J *req = notecard.newRequest("note.add"))
        {
            JAddStringToObject(req, "file", "cobs.qo");
            JAddBoolToObject(req, "binary", true);
            JAddBoolToObject(req, "live", true);
            if (!notecard.sendRequest(req)) {
                // The binary data store is cleared on successful transmission,
                // but we need to reset it manually if the request failed.
                NoteBinaryReset();
            }
        }
    }

    // Delay between sends
    delay(30 * 1000); // 30 seconds
}
