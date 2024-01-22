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
    NoteBinaryStoreReset();
}

// In the Arduino main loop which is called repeatedly, add outbound data every
// 30 seconds
void loop()
{
    // Stop the demo after five iterations to conserve data
    static unsigned event_counter = 0;
    if (++event_counter > 5)
    {
        usbSerial.println("[APP] Demo cycle complete. Program stopped. Press RESET to restart.");
        delay(10000); // 10 seconds
        return;
    }

    // Send data to the Notecard storage and pull it back
    {
        /////////////////////////////////////////////////
        // Transmit that beautiful bean footage
        /////////////////////////////////////////////////

        char data[64] = "https://youtu.be/0epWToAOlFY?t=21";
        uint32_t data_len = strlen(data);
        const uint32_t notecard_binary_area_offset = 0;
        NoteBinaryStoreTransmit(reinterpret_cast<uint8_t *>(data), data_len, sizeof(data), notecard_binary_area_offset);
        usbSerial.print("\n[APP] Transmitted ");
        usbSerial.print(data_len);
        usbSerial.println(" bytes.");

        // Log for the sake of curiosity (not necessary for operation)
        // NOTE: NoteBinaryMaxEncodedLength() is imprecise. It will most
        //       commonly return a number greater than the actual bytes encoded.
        //       However, in this contrived example there is no difference,
        //       so it works for the purposes of displaying the encoded data --
        //       which would never be done in practice.
        usbSerial.println("\n[APP] *** Encoded Binary Transmission ***");
        uint32_t tx_len = NoteBinaryCodecMaxEncodedLength(data_len);
        for (size_t i = 0 ; i < tx_len ; ++i) {
            usbSerial.print(data[i], HEX);
            usbSerial.print(" ");
            if ((i + 1) % 16 == 0) {
                usbSerial.println();
            }
        }
        usbSerial.println("\n[APP] *** Encoded Binary Transmission ***\n");

        /////////////////////////////////////////////////
        // Receive data from the Notecard binary data store
        /////////////////////////////////////////////////

        // Calcluate the length of the decoded data
        data_len = 0;
        NoteBinaryStoreDecodedLength(&data_len);

        // Create a buffer to receive the entire data store. This buffer must be
        // large enough to hold the encoded data that will be transferred from
        // the Notecard, as well as the terminating newline.
        // `NoteBinaryMaxEncodedLength()` will compute the worst-case size of
        // the encoded length plus the byte required for the newline terminator.
        uint32_t rx_buffer_len = NoteBinaryCodecMaxEncodedLength(data_len);
        uint8_t *rx_buffer = (uint8_t *)malloc(rx_buffer_len);

        // Receive the data
        NoteBinaryStoreReceive(reinterpret_cast<uint8_t *>(rx_buffer), rx_buffer_len, 0, data_len);
        usbSerial.print("\n[APP] Received ");
        usbSerial.print(data_len);
        usbSerial.println(" bytes.");

        // Display received buffer
        usbSerial.println("\n[APP] *** Decoded Data ***");
        for (size_t i = 0 ; i < data_len ; ++i) {
            usbSerial.print(rx_buffer[i]);
        }
        usbSerial.println("\n[APP] *** Decoded Data ***\n");

        // Free the receive buffer
        free(rx_buffer);

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
                NoteBinaryStoreReset();
            }
        }
    }

    // Delay between sends
    delay(30 * 1000); // 30 seconds
}
