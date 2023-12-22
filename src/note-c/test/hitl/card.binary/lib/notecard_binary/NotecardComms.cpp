#ifdef PLATFORMIO

#include "NotecardComms.h"
#include <Arduino.h>

size_t aux_serial_baudrate = NOTECARD_IF_AUX_SERIAL_BAUDRATE;

#ifndef FEATHER_AUX_EN_PIN
#define FEATHER_AUX_EN_PIN D5
#endif

bool set_aux_serial_baudrate(size_t baudrate, NotecardInterface nif)
{
    pinMode(FEATHER_AUX_EN_PIN, OUTPUT);
    digitalWrite(FEATHER_AUX_EN_PIN, HIGH);

    if (nif==NOTECARD_IF_AUX_SERIAL) {
        notecard.logDebug("WARNING: trying to change aux serial baudrate over aux serial.\n");
    }
    initialize_notecard_interface(nif);

    J* req = NoteNewRequest("card.aux.serial");
    JAddStringToObject(req, "mode", "req");
    JAddNumberToObject(req, "rate", baudrate);
    J* rsp = NoteRequestResponseWithRetry(req, 10);
    bool success = false;
    if (rsp==nullptr) {
        notecard.logDebug("No response to `card.aux.serial`.\n");
    } else if (!JIsNullString(rsp, "err")) {
        notecard.logDebugf("Error response to `card.aux.serial`\n");
    }

    const char* mode = JGetString(rsp, "mode");
    size_t rate = JGetNumber(rsp, "rate");
    if (strcmp(mode, "req")) {
        notecard.logDebugf("expected 'mode':'req', got %s\n", mode);
    } else if (rate != baudrate) {
        notecard.logDebugf("expected 'rate':%d, got %d\n", baudrate, rate);
    } else {
        aux_serial_baudrate = baudrate;
        success = true;
    }
    JDelete(rsp);
    return success;
}

bool initialize_notecard_interface(NotecardInterface iface)
{
    // Initialize the hardware I/O channel to the Notecard
    switch (iface) {
    default:
        notecard.logDebug("Unknown Notecard interface given.");
        return false;

    case NOTECARD_IF_AUX_SERIAL:
        // workaround a bug in note-arduino that doesn't call Serial.begin() when the serial port
        // is changed.
        NOTECARD_IF_AUX_SERIAL_PORT.end();
        NOTECARD_IF_AUX_SERIAL_PORT.begin(aux_serial_baudrate);
        notecard.begin(NOTECARD_IF_AUX_SERIAL_PORT, aux_serial_baudrate);
        return true;

    case NOTECARD_IF_SERIAL:
        notecard.begin(NOTECARD_IF_SERIAL_PORT, NOTECARD_IF_SERIAL_BAUDRATE);
        return true;

    case NOTECARD_IF_I2C:
        notecard.begin(NOTECARD_IF_I2C_ADDRESS);
        return true;
    }
}

#if 0 // Notecard.end() is vaporware ;-)
bool uninitialize_notecard_interface(NotecardInterface iface)
{
    // Initialize the hardware I/O channel to the Notecard
    switch (iface) {
    default:
        notecard.logDebug("Unknown Notecard interface given.");
        return false;

    case NOTECARD_IF_AUX_SERIAL:
        notecard.end(NOTECARD_IF_AUX_SERIAL_PORT, aux_serial_baudrate);
        return true;

    case NOTECARD_IF_SERIAL:
        notecard.end(NOTECARD_IF_SERIAL_PORT, NOTECARD_IF_SERIAL_BAUDRATE);
        return true;

    case NOTECARD_IF_I2C:
        notecard.end(NOTECARD_IF_I2C_ADDRESS);
        return true;
    }
}
#endif

extern "C" {
    uint32_t cobsEncode(uint8_t *ptr, uint32_t length, uint8_t eop, uint8_t *dst);
}

size_t readDataUntilTimeout(Stream& serial, size_t timeout, uint8_t* buf, size_t bufLen, size_t dataLen, size_t& duration)
{

    // encode the data to match what's on the wire
    const size_t dataShift = (bufLen - dataLen);
    memmove(buf + dataShift, buf, dataLen);
    size_t encLen = cobsEncode(buf + dataShift, dataLen, '\n', buf);
    buf[encLen] = '\n';



    size_t count = 0;
    size_t begin = millis();
    size_t matchIndex = 0;  // current index being matched in the input buffer
    for (;;) {

        size_t start = millis();
        while (!serial.available()) {
            if ((millis()-start)>timeout) {
                duration = millis()-begin;
                return count;
            }
        }

        while (serial.available()) {
            uint8_t ch = serial.read();
            if (buf[matchIndex]==ch) {
                matchIndex++;
                if (matchIndex>=bufLen) {
                    notecard.logDebugf("matched the complete image at offset %d\n", count);
                }
            } else {
                if (matchIndex>10) {
                    notecard.logDebugf("matched %d bytes at offset %d\n", matchIndex, count);
                }
                matchIndex = 0;
            }
            count++;
        }
    }
}


HardwareSerial Serial2(A0,A3);
HardwareSerial Serial3(A5,A4); // A5 is RX, A4 is TX

#endif // PLATFORMIO
