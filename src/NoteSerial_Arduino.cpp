#include "NoteSerial_Arduino.hpp"

NoteSerial *
make_note_serial (
    NoteSerial::channel_t serial_channel_,
    size_t baud_rate_
)
{
    static NoteSerial * note_serial = nullptr;
    if (!serial_channel_) {
        if (note_serial) {
            delete note_serial;
            note_serial = nullptr;
        }
    } else if (!note_serial) {
        note_serial = new NoteSerial_Arduino(*reinterpret_cast<HardwareSerial *>(serial_channel_), baud_rate_);
    }
    return note_serial;
}

NoteSerial_Arduino::NoteSerial_Arduino
(
    HardwareSerial & hw_serial_,
    size_t baud_rate_
) :
    _notecardSerial(hw_serial_),
    _notecardSerialSpeed(baud_rate_)
{
    _notecardSerial.begin(_notecardSerialSpeed);
}

size_t
NoteSerial_Arduino::available (
    void
)
{
    return _notecardSerial.available();
}

char
NoteSerial_Arduino::receive (
    void
)
{
    return _notecardSerial.read();
}

bool
NoteSerial_Arduino::reset (
    void
)
{
    _notecardSerial.begin(_notecardSerialSpeed);

    return true;
}

size_t
NoteSerial_Arduino::transmit (
    uint8_t *buffer,
    size_t size,
    bool flush
)
{
    size_t result;
    result = _notecardSerial.write(buffer, size);
    if (flush) {
        _notecardSerial.flush();
    }
    return result;
}
