#include "arduino_fakes.hpp"
#include "note_iface_fakes.hpp"

DEFINE_FAKE_VALUE_FUNC(NoteI2c *, make_note_i2c_with, void *);
DEFINE_FAKE_VALUE_FUNC(NoteLog *, make_note_log_with, void *);
DEFINE_FAKE_VALUE_FUNC(NoteSerial *, make_note_serial_with, void *);
DEFINE_FAKE_VALUE_FUNC(NoteTxn *, make_note_txn_with, void *);
DEFINE_FAKE_VALUE_FUNC(const char *, NoteI2c_receive, uint16_t, uint8_t *, uint16_t, uint32_t *);
DEFINE_FAKE_VALUE_FUNC(bool, NoteI2c_reset, uint16_t);
DEFINE_FAKE_VALUE_FUNC(const char *, NoteI2c_transmit, uint16_t, uint8_t *, uint16_t);
DEFINE_FAKE_VALUE_FUNC(size_t, NoteLog_print, const char *);
DEFINE_FAKE_VALUE_FUNC(size_t, NoteSerial_available);
DEFINE_FAKE_VALUE_FUNC(char, NoteSerial_receive);
DEFINE_FAKE_VALUE_FUNC(bool, NoteSerial_reset);
DEFINE_FAKE_VALUE_FUNC(size_t, NoteSerial_transmit, uint8_t *, size_t, bool);
DEFINE_FAKE_VALUE_FUNC(bool, NoteTxn_start, uint32_t);
DEFINE_FAKE_VOID_FUNC(NoteTxn_stop);
DEFINE_FAKE_VOID_FUNC(noteDelay, uint32_t);
DEFINE_FAKE_VALUE_FUNC(uint32_t, noteMillis);

// make_note_*() are the factory entry points Notecard.cpp calls.
NoteI2c * make_note_i2c(nullptr_t) { return make_note_i2c_with(nullptr); }
NoteLog * make_note_log(nullptr_t) { return make_note_log_with(nullptr); }
NoteSerial * make_note_serial(nullptr_t) { return make_note_serial_with(nullptr); }
NoteTxn * make_note_txn(nullptr_t) { return make_note_txn_with(nullptr); }
template <> NoteI2c * make_note_i2c<TwoWire>(TwoWire & w) { return make_note_i2c_with(&w); }
template <> NoteLog * make_note_log<HardwareSerial>(HardwareSerial & s) { return make_note_log_with(&s); }
template <> NoteSerial * make_note_serial<HardwareSerial>(HardwareSerial & s) { return make_note_serial_with(&s); }
template <> NoteTxn * make_note_txn<const uint8_t[2]>(const uint8_t (&p)[2]) { return make_note_txn_with(const_cast<uint8_t *>(p)); }

void resetNoteInterfaceFakes(void)
{
    RESET_FAKE(make_note_i2c_with);
    RESET_FAKE(make_note_log_with);
    RESET_FAKE(make_note_serial_with);
    RESET_FAKE(make_note_txn_with);
    RESET_FAKE(NoteI2c_receive);
    RESET_FAKE(NoteI2c_reset);
    RESET_FAKE(NoteI2c_transmit);
    RESET_FAKE(NoteLog_print);
    RESET_FAKE(NoteSerial_available);
    RESET_FAKE(NoteSerial_receive);
    RESET_FAKE(NoteSerial_reset);
    RESET_FAKE(NoteSerial_transmit);
    RESET_FAKE(NoteTxn_start);
    RESET_FAKE(NoteTxn_stop);
    RESET_FAKE(noteDelay);
    RESET_FAKE(noteMillis);
    FFF_RESET_HISTORY();
}
