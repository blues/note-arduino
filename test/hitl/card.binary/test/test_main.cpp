
#include "test_card_binary.h"
#include "test_binary_generators.h"
#include "test_unity_util.h"
#include <Arduino.h>


int runUnityTests(void)
{
    UNITY_BEGIN();
    testsuite_binary_generators();
    testsuite_card_binary();
    delay(1000);        // sometimes the device drops the serial connection too quickly
    return UNITY_END();
}

// This function blocks until it receives the string "start\n" or "start\r\n" on
// Serial (Swan USB). This prevents the tests from running unprompted, like if
// the Swan gets reset or power is lost and restored.
void waitForStartCommand()
{
    String input;
    while (true) {
        while (Serial.available()) {
            char c = Serial.read();
            input += c;
            if (c == '\n') {
                if (input == "start\n" || input == "start\r\n") {
                    return;
                }
                // Reset string on invalid input.
                input = "";
            }
        }
    }
}

/**
  * For Arduino framework
  */
void setup()
{
    Serial.begin();
    while (!Serial);  // wait for connection

    Serial.println("Waiting to receive \"start\"...");
    waitForStartCommand();

    // After receiving start, delay for 5 seconds. In after_upload.py, we send
    // start\n to the Swan after flashing the firmware. It takes time for
    // PlatformIO to move from the "upload" step to the "test" step, where it
    // begins capturing test output. So, this delay is to ensure we don't miss
    // capturing any test output.
    delay(5000);

    Serial.println("Start received. Running tests...");
    runUnityTests();
}

void loop()
{
    // ideally would sleep here
    // or allow the tests to be rerun without reset, but Serial reports a connection when there isn't one.
    // Tried adding Serial.end() but no help.
    delay(60*1000);
}

// #if SERIAL_RX_BUFFER_SIZE!=16384
//  #error serial buffer is too small
// #endif


