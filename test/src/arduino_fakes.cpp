#include "arduino_fakes.hpp"

DEFINE_FFF_GLOBALS;

DEFINE_FAKE_VOID_FUNC(delay, unsigned long);
DEFINE_FAKE_VALUE_FUNC(int, digitalRead, uint8_t);
DEFINE_FAKE_VOID_FUNC(digitalWrite, uint8_t, uint8_t);
DEFINE_FAKE_VALUE_FUNC(unsigned long, millis);
DEFINE_FAKE_VOID_FUNC(pinMode, uint8_t, uint8_t);

DEFINE_FAKE_VALUE_FUNC(size_t, Stream_print, const char *);

DEFINE_FAKE_VALUE_FUNC(bool, HardwareSerial_operator_bool);
DEFINE_FAKE_VALUE_FUNC(unsigned int, HardwareSerial_available);
DEFINE_FAKE_VOID_FUNC(HardwareSerial_begin, unsigned int);
DEFINE_FAKE_VOID_FUNC(HardwareSerial_end);
DEFINE_FAKE_VOID_FUNC(HardwareSerial_flush);
DEFINE_FAKE_VALUE_FUNC(char, HardwareSerial_read);
DEFINE_FAKE_VALUE_FUNC(size_t, HardwareSerial_write, uint8_t *, size_t);

DEFINE_FAKE_VALUE_FUNC(bool, SoftwareSerial_operator_bool);
DEFINE_FAKE_VALUE_FUNC(unsigned int, SoftwareSerial_available);
DEFINE_FAKE_VOID_FUNC(SoftwareSerial_begin, unsigned int);
DEFINE_FAKE_VOID_FUNC(SoftwareSerial_end);
DEFINE_FAKE_VOID_FUNC(SoftwareSerial_flush);
DEFINE_FAKE_VALUE_FUNC(char, SoftwareSerial_read);
DEFINE_FAKE_VALUE_FUNC(size_t, SoftwareSerial_write, uint8_t *, size_t);

DEFINE_FAKE_VOID_FUNC(TwoWire_begin);
DEFINE_FAKE_VOID_FUNC(TwoWire_beginTransmission, uint8_t);
DEFINE_FAKE_VOID_FUNC(TwoWire_end);
DEFINE_FAKE_VALUE_FUNC(uint8_t, TwoWire_endTransmission);
DEFINE_FAKE_VALUE_FUNC(int, TwoWire_read);
DEFINE_FAKE_VALUE_FUNC(uint8_t, TwoWire_requestFrom, int, int);
DEFINE_FAKE_VALUE_FUNC(size_t, TwoWire_write_byte, uint8_t);
DEFINE_FAKE_VALUE_FUNC(size_t, TwoWire_write_buffer, uint8_t *, size_t);

HardwareSerial Serial;
SoftwareSerial SoftSerial;
TwoWire Wire;

extern "C" {
DEFINE_FAKE_VALUE_FUNC(J *, JAddIntToObject, J *, const char *, JINTEGER);
DEFINE_FAKE_VOID_FUNC(MockNoteDeleteResponse, J *);
DEFINE_FAKE_VALUE_FUNC(bool, MockNoteResponseError, J *);
DEFINE_FAKE_VOID_FUNC(NoteDebug, const char *);
DEFINE_FAKE_VALUE_FUNC(bool, NoteDebugSyncStatus, int, int);
DEFINE_FAKE_VOID_FUNC(NoteDelayMs, uint32_t);
DEFINE_FAKE_VOID_FUNC(NoteGetFnI2C, uint32_t *, uint32_t *, i2cResetFn *, i2cTransmitFn *, i2cReceiveFn *);
DEFINE_FAKE_VOID_FUNC(NoteGetFnSerial, serialResetFn *, serialTransmitFn *, serialAvailableFn *, serialReceiveFn *);
DEFINE_FAKE_VALUE_FUNC(uint32_t, NoteGetMs);
DEFINE_FAKE_VALUE_FUNC(J *, NoteNewCommand, const char *);
DEFINE_FAKE_VALUE_FUNC(J *, NoteNewRequest, const char *);
DEFINE_FAKE_VALUE_FUNC(bool, NoteRequest, J *);
DEFINE_FAKE_VALUE_FUNC(bool, NoteRequestWithRetry, J *, uint32_t);
DEFINE_FAKE_VALUE_FUNC(J *, NoteRequestResponse, J *);
DEFINE_FAKE_VALUE_FUNC(J *, NoteRequestResponseWithRetry, J *, uint32_t);
DEFINE_FAKE_VOID_FUNC(NoteSetFnDebugOutput, debugOutputFn);
DEFINE_FAKE_VOID_FUNC(NoteSetFn, mallocFn, freeFn, delayMsFn, getMsFn);
DEFINE_FAKE_VOID_FUNC(NoteSetFnDefault, mallocFn, freeFn, delayMsFn, getMsFn);
DEFINE_FAKE_VOID_FUNC(NoteSetFnI2C, uint32_t, uint32_t, i2cResetFn, i2cTransmitFn, i2cReceiveFn);
DEFINE_FAKE_VOID_FUNC(NoteSetFnI2CDefault, uint32_t, uint32_t, i2cResetFn, i2cTransmitFn, i2cReceiveFn);
DEFINE_FAKE_VOID_FUNC(NoteSetFnI2CMutex, mutexFn, mutexFn);
DEFINE_FAKE_VOID_FUNC(NoteSetFnNoteMutex, mutexFn, mutexFn);
DEFINE_FAKE_VOID_FUNC(NoteSetFnSerial, serialResetFn, serialTransmitFn, serialAvailableFn, serialReceiveFn);
DEFINE_FAKE_VOID_FUNC(NoteSetFnSerialDefault, serialResetFn, serialTransmitFn, serialAvailableFn, serialReceiveFn);
DEFINE_FAKE_VOID_FUNC(NoteSetFnTransaction, txnStartFn, txnStopFn);
DEFINE_FAKE_VOID_FUNC(NoteSetI2CAddress, uint32_t);
DEFINE_FAKE_VOID_FUNC(NoteSetI2CMtu, uint32_t);
DEFINE_FAKE_VOID_FUNC(NoteSetUserAgent, char *);
}

std::string NoteDebug_arg0_str;
std::string NoteNewCommand_arg0_str;
std::string NoteNewRequest_arg0_str;
std::string NoteSetUserAgent_arg0_str;
std::string Stream_print_arg0_str;
std::string HardwareSerial_write_arg0_str;
std::string SoftwareSerial_write_arg0_str;

namespace {
void captureNoteDebug(const char * s) { if (s) { NoteDebug_arg0_str = s; } }
J * captureNoteNewCommand(const char * s) { if (s) { NoteNewCommand_arg0_str = s; } return NoteNewCommand_fake.return_val; }
J * captureNoteNewRequest(const char * s) { if (s) { NoteNewRequest_arg0_str = s; } return NoteNewRequest_fake.return_val; }
void captureNoteSetUserAgent(char * s) { if (s) { NoteSetUserAgent_arg0_str = s; } }
size_t captureStreamPrint(const char * s) { Stream_print_arg0_str = s; return Stream_print_fake.return_val; }
size_t captureHardwareSerialWrite(uint8_t * b, size_t) { HardwareSerial_write_arg0_str = reinterpret_cast<char *>(b); return HardwareSerial_write_fake.return_val; }
size_t captureSoftwareSerialWrite(uint8_t * b, size_t) { SoftwareSerial_write_arg0_str = reinterpret_cast<char *>(b); return SoftwareSerial_write_fake.return_val; }
}

void resetArduinoFakes(void)
{
    RESET_FAKE(delay);
    RESET_FAKE(digitalRead);
    RESET_FAKE(digitalWrite);
    RESET_FAKE(millis);
    RESET_FAKE(pinMode);
    RESET_FAKE(Stream_print);
    RESET_FAKE(HardwareSerial_operator_bool);
    RESET_FAKE(HardwareSerial_available);
    RESET_FAKE(HardwareSerial_begin);
    RESET_FAKE(HardwareSerial_end);
    RESET_FAKE(HardwareSerial_flush);
    RESET_FAKE(HardwareSerial_read);
    RESET_FAKE(HardwareSerial_write);
    RESET_FAKE(SoftwareSerial_operator_bool);
    RESET_FAKE(SoftwareSerial_available);
    RESET_FAKE(SoftwareSerial_begin);
    RESET_FAKE(SoftwareSerial_end);
    RESET_FAKE(SoftwareSerial_flush);
    RESET_FAKE(SoftwareSerial_read);
    RESET_FAKE(SoftwareSerial_write);
    RESET_FAKE(TwoWire_begin);
    RESET_FAKE(TwoWire_beginTransmission);
    RESET_FAKE(TwoWire_end);
    RESET_FAKE(TwoWire_endTransmission);
    RESET_FAKE(TwoWire_read);
    RESET_FAKE(TwoWire_requestFrom);
    RESET_FAKE(TwoWire_write_byte);
    RESET_FAKE(TwoWire_write_buffer);
    RESET_FAKE(JAddIntToObject);
    RESET_FAKE(MockNoteDeleteResponse);
    RESET_FAKE(MockNoteResponseError);
    RESET_FAKE(NoteDebug);
    RESET_FAKE(NoteDebugSyncStatus);
    RESET_FAKE(NoteDelayMs);
    RESET_FAKE(NoteGetFnI2C);
    RESET_FAKE(NoteGetFnSerial);
    RESET_FAKE(NoteGetMs);
    RESET_FAKE(NoteNewCommand);
    RESET_FAKE(NoteNewRequest);
    RESET_FAKE(NoteRequest);
    RESET_FAKE(NoteRequestWithRetry);
    RESET_FAKE(NoteRequestResponse);
    RESET_FAKE(NoteRequestResponseWithRetry);
    RESET_FAKE(NoteSetFnDebugOutput);
    RESET_FAKE(NoteSetFn);
    RESET_FAKE(NoteSetFnDefault);
    RESET_FAKE(NoteSetFnI2C);
    RESET_FAKE(NoteSetFnI2CDefault);
    RESET_FAKE(NoteSetFnI2CMutex);
    RESET_FAKE(NoteSetFnNoteMutex);
    RESET_FAKE(NoteSetFnSerial);
    RESET_FAKE(NoteSetFnSerialDefault);
    RESET_FAKE(NoteSetFnTransaction);
    RESET_FAKE(NoteSetI2CAddress);
    RESET_FAKE(NoteSetI2CMtu);
    RESET_FAKE(NoteSetUserAgent);
    FFF_RESET_HISTORY();

    NoteDebug_arg0_str.clear();
    NoteNewCommand_arg0_str.clear();
    NoteNewRequest_arg0_str.clear();
    NoteSetUserAgent_arg0_str.clear();
    Stream_print_arg0_str.clear();
    HardwareSerial_write_arg0_str.clear();
    SoftwareSerial_write_arg0_str.clear();
    NoteDebug_fake.custom_fake = captureNoteDebug;
    NoteNewCommand_fake.custom_fake = captureNoteNewCommand;
    NoteNewRequest_fake.custom_fake = captureNoteNewRequest;
    NoteSetUserAgent_fake.custom_fake = captureNoteSetUserAgent;
    Stream_print_fake.custom_fake = captureStreamPrint;
    HardwareSerial_write_fake.custom_fake = captureHardwareSerialWrite;
    SoftwareSerial_write_fake.custom_fake = captureSoftwareSerialWrite;

    // A begun serial port reports ready, as the Arduino core does.
    HardwareSerial_operator_bool_fake.return_val = true;
    SoftwareSerial_operator_bool_fake.return_val = true;
}
