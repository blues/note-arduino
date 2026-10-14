// FFF fakes for the note-arduino interfaces (NoteI2c, NoteLog, NoteSerial,
// NoteTxn, NoteTime) used by the Notecard unit tests.
#ifndef NOTE_IFACE_FAKES_HPP
#define NOTE_IFACE_FAKES_HPP

#include <stddef.h>
#include <stdint.h>

#include "fff.h"
#include "NoteI2c.hpp"
#include "NoteLog.hpp"
#include "NoteSerial.hpp"
#include "NoteTime.h"
#include "NoteTxn.hpp"

DECLARE_FAKE_VALUE_FUNC(NoteI2c *, make_note_i2c_with, void *);
DECLARE_FAKE_VALUE_FUNC(NoteLog *, make_note_log_with, void *);
DECLARE_FAKE_VALUE_FUNC(NoteSerial *, make_note_serial_with, void *);
DECLARE_FAKE_VALUE_FUNC(NoteTxn *, make_note_txn_with, void *);

DECLARE_FAKE_VALUE_FUNC(const char *, NoteI2c_receive, uint16_t, uint8_t *, uint16_t, uint32_t *);
DECLARE_FAKE_VALUE_FUNC(bool, NoteI2c_reset, uint16_t);
DECLARE_FAKE_VALUE_FUNC(const char *, NoteI2c_transmit, uint16_t, uint8_t *, uint16_t);
DECLARE_FAKE_VALUE_FUNC(size_t, NoteLog_print, const char *);
DECLARE_FAKE_VALUE_FUNC(size_t, NoteSerial_available);
DECLARE_FAKE_VALUE_FUNC(char, NoteSerial_receive);
DECLARE_FAKE_VALUE_FUNC(bool, NoteSerial_reset);
DECLARE_FAKE_VALUE_FUNC(size_t, NoteSerial_transmit, uint8_t *, size_t, bool);
DECLARE_FAKE_VALUE_FUNC(bool, NoteTxn_start, uint32_t);
DECLARE_FAKE_VOID_FUNC(NoteTxn_stop);
DECLARE_FAKE_VOID_FUNC(noteDelay, uint32_t);
DECLARE_FAKE_VALUE_FUNC(uint32_t, noteMillis);

// FFF cannot fake virtual methods, so these stateless stubs forward each
// interface method to the FFF fakes above.
class NoteI2c_Fake final : public NoteI2c
{
public:
    const char * receive(uint16_t a, uint8_t * b, uint16_t n, uint32_t * avail) override { return NoteI2c_receive(a, b, n, avail); }
    bool reset(uint16_t a) override { return NoteI2c_reset(a); }
    const char * transmit(uint16_t a, uint8_t * b, uint16_t n) override { return NoteI2c_transmit(a, b, n); }
};

class NoteLog_Fake final : public NoteLog
{
public:
    size_t print(const char * message) override { return NoteLog_print(message); }
};

class NoteSerial_Fake final : public NoteSerial
{
public:
    size_t available(void) override { return NoteSerial_available(); }
    char receive(void) override { return NoteSerial_receive(); }
    bool reset(void) override { return NoteSerial_reset(); }
    size_t transmit(uint8_t * b, size_t n, bool flush) override { return NoteSerial_transmit(b, n, flush); }
};

class NoteTxn_Fake final : public NoteTxn
{
public:
    bool start(uint32_t timeout_ms) override { return NoteTxn_start(timeout_ms); }
    void stop(void) override { NoteTxn_stop(); }
};

// Reset every interface fake (call after resetArduinoFakes()).
void resetNoteInterfaceFakes(void);

#endif // NOTE_IFACE_FAKES_HPP
