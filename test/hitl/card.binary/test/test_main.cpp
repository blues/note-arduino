
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

/**
  * For Arduino framework
  */
void setup()
{
    Serial.begin();
    while (!Serial);  // wait for connection
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


