// FFF fakes for the Arduino core (pins, timing, Stream, HardwareSerial,
// SoftwareSerial, TwoWire) and for the note-c API used by note-arduino.
// Included by src/ in place of <Arduino.h>/<Wire.h>/<note-c/note.h> when
// NOTE_MOCK is defined.
#ifndef ARDUINO_FAKES_HPP
#define ARDUINO_FAKES_HPP

#include <stddef.h>
#include <stdint.h>

#include <string>
#include <vector>

#include "fff.h"
#include "note-c/note.h"

#define _NOTE_C_STRINGIZE(x) #x
#define NOTE_C_STRINGIZE(x) _NOTE_C_STRINGIZE(x)

#define SERIAL_RX_BUFFER_SIZE 79

enum PinMode {
    INPUT = 0x19,
    INPUT_PULLUP,
    OUTPUT,
};

enum PinState {
    HIGH = 0x79,
    LOW,
};

// Arduino core functions
DECLARE_FAKE_VOID_FUNC(delay, unsigned long);
DECLARE_FAKE_VALUE_FUNC(int, digitalRead, uint8_t);
DECLARE_FAKE_VOID_FUNC(digitalWrite, uint8_t, uint8_t);
DECLARE_FAKE_VALUE_FUNC(unsigned long, millis);
DECLARE_FAKE_VOID_FUNC(pinMode, uint8_t, uint8_t);

// FFF cannot fake C++ class methods, so the classes below are minimal,
// stateless stubs whose methods forward to the free FFF fakes declared here.
DECLARE_FAKE_VALUE_FUNC(size_t, Stream_print, const char *);
struct Stream {
    virtual ~Stream(void) {}
    virtual size_t print(const char * str) { return Stream_print(str); }
};

DECLARE_FAKE_VALUE_FUNC(bool, HardwareSerial_operator_bool);
DECLARE_FAKE_VALUE_FUNC(unsigned int, HardwareSerial_available);
DECLARE_FAKE_VOID_FUNC(HardwareSerial_begin, unsigned int);
DECLARE_FAKE_VOID_FUNC(HardwareSerial_end);
DECLARE_FAKE_VOID_FUNC(HardwareSerial_flush);
DECLARE_FAKE_VALUE_FUNC(char, HardwareSerial_read);
DECLARE_FAKE_VALUE_FUNC(size_t, HardwareSerial_write, uint8_t *, size_t);
struct HardwareSerial : public Stream {
    operator bool() { return HardwareSerial_operator_bool(); }
    unsigned int available(void) { return HardwareSerial_available(); }
    void begin(unsigned int baud) { HardwareSerial_begin(baud); }
    void end(void) { HardwareSerial_end(); }
    void flush(void) { HardwareSerial_flush(); }
    char read(void) { return HardwareSerial_read(); }
    size_t write(uint8_t * buffer, size_t size) { return HardwareSerial_write(buffer, size); }
};

DECLARE_FAKE_VALUE_FUNC(bool, SoftwareSerial_operator_bool);
DECLARE_FAKE_VALUE_FUNC(unsigned int, SoftwareSerial_available);
DECLARE_FAKE_VOID_FUNC(SoftwareSerial_begin, unsigned int);
DECLARE_FAKE_VOID_FUNC(SoftwareSerial_end);
DECLARE_FAKE_VOID_FUNC(SoftwareSerial_flush);
DECLARE_FAKE_VALUE_FUNC(char, SoftwareSerial_read);
DECLARE_FAKE_VALUE_FUNC(size_t, SoftwareSerial_write, uint8_t *, size_t);
struct SoftwareSerial : public Stream {
    operator bool() { return SoftwareSerial_operator_bool(); }
    unsigned int available(void) { return SoftwareSerial_available(); }
    void begin(unsigned int baud) { SoftwareSerial_begin(baud); }
    void end(void) { SoftwareSerial_end(); }
    void flush(void) { SoftwareSerial_flush(); }
    char read(void) { return SoftwareSerial_read(); }
    size_t write(uint8_t * buffer, size_t size) { return SoftwareSerial_write(buffer, size); }
};

DECLARE_FAKE_VOID_FUNC(TwoWire_begin);
DECLARE_FAKE_VOID_FUNC(TwoWire_beginTransmission, uint8_t);
DECLARE_FAKE_VOID_FUNC(TwoWire_end);
DECLARE_FAKE_VALUE_FUNC(uint8_t, TwoWire_endTransmission);
DECLARE_FAKE_VALUE_FUNC(int, TwoWire_read);
DECLARE_FAKE_VALUE_FUNC(uint8_t, TwoWire_requestFrom, int, int);
DECLARE_FAKE_VALUE_FUNC(size_t, TwoWire_write_byte, uint8_t);
DECLARE_FAKE_VALUE_FUNC(size_t, TwoWire_write_buffer, uint8_t *, size_t);
struct TwoWire {
    void begin(void) { TwoWire_begin(); }
    void beginTransmission(uint8_t address) { TwoWire_beginTransmission(address); }
    void end(void) { TwoWire_end(); }
    uint8_t endTransmission(void) { return TwoWire_endTransmission(); }
    int read(void) { return TwoWire_read(); }
    uint8_t requestFrom(int address, int quantity) { return TwoWire_requestFrom(address, quantity); }
    size_t write(uint8_t c) { return TwoWire_write_byte(c); }
    size_t write(uint8_t * buffer, size_t size) { return TwoWire_write_buffer(buffer, size); }
};

extern HardwareSerial Serial;
extern SoftwareSerial SoftSerial;
extern TwoWire Wire;

// note-c API
#define NoteDebugWithLevel(...)
#define NoteDebugWithLevelLn(...)
#ifdef NoteDeleteResponse
#undef NoteDeleteResponse
#endif
#define NoteDeleteResponse(x) MockNoteDeleteResponse(x)
#ifdef NoteResponseError
#undef NoteResponseError
#endif
#define NoteResponseError(x) MockNoteResponseError(x)

extern "C" {
DECLARE_FAKE_VALUE_FUNC(J *, JAddIntToObject, J *, const char *, JINTEGER);
DECLARE_FAKE_VOID_FUNC(MockNoteDeleteResponse, J *);
DECLARE_FAKE_VALUE_FUNC(bool, MockNoteResponseError, J *);
DECLARE_FAKE_VOID_FUNC(NoteDebug, const char *);
DECLARE_FAKE_VALUE_FUNC(bool, NoteDebugSyncStatus, int, int);
DECLARE_FAKE_VOID_FUNC(NoteDelayMs, uint32_t);
DECLARE_FAKE_VOID_FUNC(NoteGetFnI2C, uint32_t *, uint32_t *, i2cResetFn *, i2cTransmitFn *, i2cReceiveFn *);
DECLARE_FAKE_VOID_FUNC(NoteGetFnSerial, serialResetFn *, serialTransmitFn *, serialAvailableFn *, serialReceiveFn *);
DECLARE_FAKE_VALUE_FUNC(uint32_t, NoteGetMs);
DECLARE_FAKE_VALUE_FUNC(J *, NoteNewCommand, const char *);
DECLARE_FAKE_VALUE_FUNC(J *, NoteNewRequest, const char *);
DECLARE_FAKE_VALUE_FUNC(bool, NoteRequest, J *);
DECLARE_FAKE_VALUE_FUNC(bool, NoteRequestWithRetry, J *, uint32_t);
DECLARE_FAKE_VALUE_FUNC(J *, NoteRequestResponse, J *);
DECLARE_FAKE_VALUE_FUNC(J *, NoteRequestResponseWithRetry, J *, uint32_t);
DECLARE_FAKE_VOID_FUNC(NoteSetFnDebugOutput, debugOutputFn);
DECLARE_FAKE_VOID_FUNC(NoteSetFn, mallocFn, freeFn, delayMsFn, getMsFn);
DECLARE_FAKE_VOID_FUNC(NoteSetFnDefault, mallocFn, freeFn, delayMsFn, getMsFn);
DECLARE_FAKE_VOID_FUNC(NoteSetFnI2C, uint32_t, uint32_t, i2cResetFn, i2cTransmitFn, i2cReceiveFn);
DECLARE_FAKE_VOID_FUNC(NoteSetFnI2CDefault, uint32_t, uint32_t, i2cResetFn, i2cTransmitFn, i2cReceiveFn);
DECLARE_FAKE_VOID_FUNC(NoteSetFnI2CMutex, mutexFn, mutexFn);
DECLARE_FAKE_VOID_FUNC(NoteSetFnNoteMutex, mutexFn, mutexFn);
DECLARE_FAKE_VOID_FUNC(NoteSetFnSerial, serialResetFn, serialTransmitFn, serialAvailableFn, serialReceiveFn);
DECLARE_FAKE_VOID_FUNC(NoteSetFnSerialDefault, serialResetFn, serialTransmitFn, serialAvailableFn, serialReceiveFn);
DECLARE_FAKE_VOID_FUNC(NoteSetFnTransaction, txnStartFn, txnStopFn);
DECLARE_FAKE_VOID_FUNC(NoteSetI2CAddress, uint32_t);
DECLARE_FAKE_VOID_FUNC(NoteSetI2CMtu, uint32_t);
DECLARE_FAKE_VOID_FUNC(NoteSetUserAgent, char *);
}

// Some arguments are transient C strings or buffers. These capture fakes copy
// the argument into a std::string at call time (FFF only stores the pointer).
extern std::string NoteDebug_arg0_str;
extern std::string NoteNewCommand_arg0_str;
extern std::string NoteNewRequest_arg0_str;
extern std::string NoteSetUserAgent_arg0_str;
extern std::string Stream_print_arg0_str;
extern std::string HardwareSerial_write_arg0_str;
extern std::string SoftwareSerial_write_arg0_str;

// Per-pin views of the FFF argument history of the pin fakes.
#define PIN_FAKE_VIEWS(FAKE) \
    inline size_t FAKE##_invoked(uint8_t pin) { \
        size_t count = 0; \
        const unsigned int len = (FAKE##_fake.call_count < FFF_ARG_HISTORY_LEN) ? FAKE##_fake.call_count : FFF_ARG_HISTORY_LEN; \
        for (unsigned int i = 0; i < len; ++i) { if (FAKE##_fake.arg0_history[i] == pin) { ++count; } } \
        return count; \
    }
PIN_FAKE_VIEWS(digitalRead)
PIN_FAKE_VIEWS(digitalWrite)
PIN_FAKE_VIEWS(pinMode)
#define PIN_FAKE_VALUES(FAKE) \
    inline std::vector<uint8_t> FAKE##_values(uint8_t pin) { \
        std::vector<uint8_t> values; \
        const unsigned int len = (FAKE##_fake.call_count < FFF_ARG_HISTORY_LEN) ? FAKE##_fake.call_count : FFF_ARG_HISTORY_LEN; \
        for (unsigned int i = 0; i < len; ++i) { if (FAKE##_fake.arg0_history[i] == pin) { values.push_back(FAKE##_fake.arg1_history[i]); } } \
        return values; \
    }
PIN_FAKE_VALUES(digitalWrite)
PIN_FAKE_VALUES(pinMode)

// Reset every Arduino/note-c fake and the FFF call history.
void resetArduinoFakes(void);

// RESET_FAKE() for a capture fake, keeping its capturing custom_fake.
#define RESET_CAPTURE_FAKE(FAKE) \
    do { \
        auto saved_custom_fake_ = FAKE##_fake.custom_fake; \
        RESET_FAKE(FAKE); \
        FAKE##_fake.custom_fake = saved_custom_fake_; \
        FAKE##_arg0_str.clear(); \
    } while (0)

// Load a return sequence that falls back to 0 once exhausted.
#define FFF_CAT_(a, b) a##b
#define FFF_CAT(a, b) FFF_CAT_(a, b)
#define SET_RETURN_SEQ_THEN_ZERO(FAKE, TYPE, ...) \
    TYPE FFF_CAT(FAKE##_seq_, __LINE__)[] = {__VA_ARGS__, 0}; \
    SET_RETURN_SEQ(FAKE, FFF_CAT(FAKE##_seq_, __LINE__), \
        (sizeof(FFF_CAT(FAKE##_seq_, __LINE__)) / sizeof(TYPE)))

#endif // ARDUINO_FAKES_HPP
