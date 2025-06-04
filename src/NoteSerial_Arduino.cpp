#include "NoteSerial_Arduino.hpp"

#include "NoteDefines.h"

#ifndef NOTE_MOCK
#ifdef NOTE_ARDUINO_SOFTWARE_SERIAL_SUPPORT
#include <SoftwareSerial.h>
#endif
#else
#include "mock/mock-arduino.hpp"
#endif

#define NOTE_C_SERIAL_TIMEOUT_MS 3500

// Template Meta-Programming (TMP) to extract the nested template type
template <typename nested_type>
struct ExtractNestedTemplateType {
    // Default case: no extraction
};
template <typename nested_type>
struct ExtractNestedTemplateType<MakeNoteSerial_ArduinoParameters<nested_type>> {
    using type = nested_type;
};

// Singleton instance of the NoteSerial_Arduino class
namespace instance {
    inline NoteSerial* & note_serial (void) {
        static NoteSerial* note_serial = nullptr;
        return note_serial;
    }
};

NoteSerial *
make_note_serial (
    nullptr_t
) {
    NoteSerial* & note_serial = instance::note_serial();
    if (note_serial) {
        delete note_serial;
        note_serial = nullptr;
    }
    return note_serial;
}

template <typename T>
NoteSerial *
make_note_serial (
    T & serial_parameters_
) {
    NoteSerial* & note_serial = instance::note_serial();
    if (!note_serial) {
        using serial_type = typename ExtractNestedTemplateType<T>::type;
        note_serial = new NoteSerial_Arduino<serial_type>(serial_parameters_.serial, serial_parameters_.baud_rate);
    }

    return note_serial;
}

template <typename T>
NoteSerial_Arduino<T>::NoteSerial_Arduino
(
    T & serial_,
    size_t baud_rate_
) :
    _notecardSerial(serial_),
    _notecardSerialSpeed(baud_rate_)
{
    _notecardSerial.begin(_notecardSerialSpeed);

    // Wait for the serial port to be ready
    for (const size_t startMs = ::millis() ; !_notecardSerial && ((::millis() - startMs) < NOTE_C_SERIAL_TIMEOUT_MS) ;);
}

template <typename T>
NoteSerial_Arduino<T>::~NoteSerial_Arduino (
    void
)
{
    _notecardSerial.end();
}

template <typename T>
size_t
NoteSerial_Arduino<T>::available (
    void
)
{
    return _notecardSerial.available();
}

template <typename T>
char
NoteSerial_Arduino<T>::receive (
    void
)
{
    return _notecardSerial.read();
}

template <typename T>
bool
NoteSerial_Arduino<T>::reset (
    void
)
{
    _notecardSerial.end();
    _notecardSerial.begin(_notecardSerialSpeed);

    return true;
}

template <typename T>
size_t
NoteSerial_Arduino<T>::transmit (
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

// Explicitly instantiate the classes and methods for the supported types
template class NoteSerial_Arduino<HardwareSerial>;
template NoteSerial * make_note_serial<MakeNoteSerial_ArduinoParameters<HardwareSerial>>(MakeNoteSerial_ArduinoParameters<HardwareSerial> &);

#ifdef NOTE_ARDUINO_SOFTWARE_SERIAL_SUPPORT
template class NoteSerial_Arduino<SoftwareSerial>;
template NoteSerial * make_note_serial<MakeNoteSerial_ArduinoParameters<SoftwareSerial>>(MakeNoteSerial_ArduinoParameters<SoftwareSerial> &);
#endif
