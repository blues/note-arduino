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

    // Reset the state of the Notecard's binary store to a known value.
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
        // Chunk transmit that beautiful bean footage
        /////////////////////////////////////////////////

        // For the purposes of this example, we'll use a string literal as our
        // data source. In a real application, you might be reading from an
        // EEPROM or other large data source.
        const char data_source[] = "https://youtu.be/0epWToAOlFY?t=21";
        const uint32_t data_source_len = strlen(data_source);

        // We intend to transmit the buffer in chunks of 8 bytes. The data is
        // encoded in place, so we will need to allocate a buffer that is large
        // enough to hold the encoded data, as well as the terminating newline.
        // `NoteBinaryMaxEncodedLength()` will compute the worst-case size of
        // the encoded length plus the byte required for the newline terminator.
        const uint32_t tx_chunk_size = 8;
        const uint32_t tx_buffer_len = NoteBinaryCodecMaxEncodedLength(tx_chunk_size);
        uint8_t *tx_buffer = (uint8_t *)malloc(tx_buffer_len);

        // Transmit the data in chunks of 8 bytes
        uint32_t notecard_binary_area_offset = 0;
        for (size_t chunk = 0 ; notecard_binary_area_offset < data_source_len ; ++chunk) {
            uint32_t data_len = (((data_source_len - notecard_binary_area_offset) > tx_chunk_size)
                                  ? tx_chunk_size
                                  : (data_source_len - notecard_binary_area_offset));

            // Copy bytes from the data source into the buffer. Note that the
            // data must be copied sequentially into the Notecard binary area.
            // Therefore, we use the offset for the Notecard binary area as the
            // offset into the data source to ensure our data is aligned.
            memcpy(tx_buffer, (data_source + notecard_binary_area_offset), data_len);

            // Transmit the chunk
            usbSerial.print("[APP] Transmitting chunk #");
            usbSerial.print(chunk);
            usbSerial.print(", containing ");
            usbSerial.print(data_len);
            usbSerial.println(" bytes.");
            if (NoteBinaryStoreTransmit(reinterpret_cast<uint8_t *>(tx_buffer), data_len, tx_buffer_len, notecard_binary_area_offset)) {
                --chunk;
                usbSerial.println("[APP] Failed to transmit.");
                continue;
            }

            // Update the offset
            notecard_binary_area_offset += data_len;
            usbSerial.print("[APP] Transmitted ");
            usbSerial.print(data_len);
            usbSerial.println(" bytes.");

            // Log for the sake of curiosity (not necessary for operation)
            // NOTE: NoteBinaryMaxEncodedLength() is imprecise. It will most
            //       commonly return a number greater than the actual bytes
            //       encoded. However, in this contrived example there is no
            //       difference, so it works for the purposes of displaying the
            //       encoded data -- which would never be done in practice.
            usbSerial.println("\n[APP] *** Encoded Binary Transmission ***");
            uint32_t tx_len = NoteBinaryCodecMaxEncodedLength(data_len);
            for (size_t i = 0 ; i < tx_len ; ++i) {
                usbSerial.print(tx_buffer[i], HEX);
                usbSerial.print(" ");
                if ((i + 1) % 16 == 0) {
                    usbSerial.println();
                }
            }
            usbSerial.println("\n[APP] *** Encoded Binary Transmission ***\n");
        }

        // Free the transmit buffer
        free(tx_buffer);

        /////////////////////////////////////////////////
        // Chunk receive data from the Notecard binary store
        /////////////////////////////////////////////////

        // For the purposes of this example, we'll use a buffer as our data
        // store. In a real application, you might be writing to an EEPROM or
        // other large data store.
        char data_store[64] = {0};

        // Calcluate the length of the decoded data
        uint32_t rx_data_len = 0;
        NoteBinaryStoreDecodedLength(&rx_data_len);

        // We intend to receive the Notecard's binary data store in chunks of
        // 12 bytes. The `offset` and `length` values, used to request data, are
        // based on decoded data size, not encoded size. Therefore, we need to
        // allocate a buffer large enough to accommodate the encoded data to be
        // transferred from the Notecard, as well as the terminating newline.

        // `NoteBinaryMaxEncodedLength()` will compute the worst-case size of
        // the encoded length plus the byte required for the newline terminator.
        const uint32_t rx_chunk_size = 12;
        const uint32_t rx_buffer_len = NoteBinaryCodecMaxEncodedLength(rx_chunk_size);
        uint8_t *rx_buffer = (uint8_t *)malloc(rx_buffer_len);

        // Receive the data in chunks of 12 bytes
        notecard_binary_area_offset = 0;
        for (size_t chunk = 0 ; notecard_binary_area_offset < rx_data_len ; ++chunk) {
            uint32_t rx_len = (((rx_data_len - notecard_binary_area_offset) > rx_chunk_size)
                               ? rx_chunk_size
                               : (rx_data_len - notecard_binary_area_offset));

            // Receive the chunk
            usbSerial.print("[APP] Receiving chunk #");
            usbSerial.print(chunk);
            usbSerial.print(", containing ");
            usbSerial.print(rx_len);
            usbSerial.println(" bytes.");
            if (NoteBinaryStoreReceive(reinterpret_cast<uint8_t *>(rx_buffer), rx_buffer_len, notecard_binary_area_offset, rx_len)) {
                --chunk;
                usbSerial.println("[APP] Failed to receive.");
                continue;
            }

            // Copy bytes from the buffer into the data store. Note that the
            // data will be pulled sequentially from the Notecard binary area.
            // Therefore, we can use the offset for the Notecard binary area as
            // the offset into the data store to ensure our data is aligned.
            memcpy((data_store + notecard_binary_area_offset), rx_buffer, rx_len);

            // Update the offset
            notecard_binary_area_offset += rx_len;
            usbSerial.print("\n[APP] Received ");
            usbSerial.print(rx_len);
            usbSerial.println(" bytes.");

            // Log for the sake of curiosity
            usbSerial.println("[APP] *** Decoded Binary Data ***");
            for (size_t i = 0 ; i < rx_len ; ++i) {
                usbSerial.print(rx_buffer[i], HEX);
                usbSerial.print(" ");
            }
            usbSerial.println("\n[APP] *** Decoded Binary Data ***\n");
        }

        // Display complete buffer
        usbSerial.println("[APP] *** Decoded Data ***");
        for (size_t i = 0 ; i < rx_data_len ; ++i) {
            usbSerial.print(data_store[i]);
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
                // The binary store is cleared on successful transmission, but
                // we need to reset it manually if the request failed.
                NoteBinaryStoreReset();
            }
        }
    }

    // Delay between sends
    delay(30 * 1000); // 30 seconds
}
