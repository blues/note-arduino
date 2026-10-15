#include <catch2/catch_test_macros.hpp>

#include "arduino_fakes.hpp"
#include "NoteSerial_Arduino.hpp"

#include <cassert>
#include <cstring>
#include <iostream>


// Called first in every TEST_CASE: RESET_FAKE on every FFF fake, then
// FFF_RESET_HISTORY() (see resetArduinoFakes()).
static void resetFakes(void)
{
  resetArduinoFakes();
}

TEST_CASE("test_make_note_serial_instantiates_noteserial_object", "[NoteSerial_Arduino]")
{
  resetFakes();
    // Arrange
    NoteSerial * noteserial = nullptr;
    MakeNoteSerial_ArduinoParameters<HardwareSerial> arduino_parameters(Serial, 9600);

    // Action
    noteserial = make_note_serial<MakeNoteSerial_ArduinoParameters<HardwareSerial>>(arduino_parameters);

    // Assert
    CHECK(nullptr != noteserial);

    // Clean-up
    make_note_serial(nullptr);
}

TEST_CASE("test_make_note_serial_enforces_singleton_by_returning_same_noteserial_object_for_all_calls", "[NoteSerial_Arduino]")
{
  resetFakes();
    // Arrange
    MakeNoteSerial_ArduinoParameters<HardwareSerial> arduino_parameters(Serial, 9600);
    NoteSerial * const noteserial_1 = make_note_serial<MakeNoteSerial_ArduinoParameters<HardwareSerial>>(arduino_parameters);

    // Action
    NoteSerial * const noteserial_2 = make_note_serial<MakeNoteSerial_ArduinoParameters<HardwareSerial>>(arduino_parameters);

    // Assert
    CHECK(noteserial_1 == noteserial_2);

    // Clean-up
    make_note_serial(nullptr);
}

//int test_make_note_serial_returns_nullptr_when_nullptr_is_passed_as_parameter()
TEST_CASE("test_make_note_serial_deletes_singleton_when_nullptr_is_passed_as_parameter", "[NoteSerial_Arduino]")
{
  resetFakes();
    // Arrange
    MakeNoteSerial_ArduinoParameters<HardwareSerial> arduino_parameters(Serial, 9600);
    NoteSerial * noteserial = make_note_serial<MakeNoteSerial_ArduinoParameters<HardwareSerial>>(arduino_parameters);
    assert(noteserial);

    // Action
    noteserial = make_note_serial(nullptr);

    // Assert
    CHECK(nullptr == noteserial);
}

TEST_CASE("test_noteserial_arduino_constructor_invokes_hardware_serial_parameter_begin_method", "[NoteSerial_Arduino]")
{
  resetFakes();
    // Arrange
    RESET_FAKE(HardwareSerial_begin);

    // Action
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);

    // Assert
    CHECK(HardwareSerial_begin_fake.call_count);
}

TEST_CASE("test_noteserial_arduino_constructor_does_not_modify_baud_parameter_before_passing_to_hardware_serial_begin", "[NoteSerial_Arduino]")
{
  resetFakes();
    // Arrange
    const size_t EXPECTED_BAUD_RATE = 9600;

    RESET_FAKE(HardwareSerial_begin);

    // Action
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, EXPECTED_BAUD_RATE);

    // Assert
    CHECK(EXPECTED_BAUD_RATE == HardwareSerial_begin_fake.arg0_val);
}

TEST_CASE("test_noteserial_arduino_deconstructor_invokes_hardware_serial_end_method", "[NoteSerial_Arduino]")
{
  resetFakes();
    // Arrange
    RESET_FAKE(HardwareSerial_end);
    NoteSerial_Arduino<HardwareSerial> * noteserial = new NoteSerial_Arduino<HardwareSerial>(Serial, 9600);

    // Action
    delete noteserial;

    // Assert
    CHECK(HardwareSerial_end_fake.call_count);
}

TEST_CASE("test_noteserial_arduino_available_invokes_hardware_serial_available", "[NoteSerial_Arduino]")
{
  resetFakes();
    // Arrange
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);
    RESET_FAKE(HardwareSerial_available);

    // Action
    noteserial.available();

    // Assert
    CHECK(HardwareSerial_available_fake.call_count);
}

TEST_CASE("test_noteserial_arduino_available_does_not_modify_hardware_serial_available_result_value_before_returning_to_caller", "[NoteSerial_Arduino]")
{
  resetFakes();
    // Arrange
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);
    const size_t EXPECTED_RESULT = 79;

    RESET_FAKE(HardwareSerial_available);
    HardwareSerial_available_fake.return_val = EXPECTED_RESULT;

    // Action
    const size_t ACTUAL_RESULT = noteserial.available();

    // Assert
    CHECK(ACTUAL_RESULT == EXPECTED_RESULT);
}

TEST_CASE("test_noteserial_arduino_receive_invokes_hardware_serial_read", "[NoteSerial_Arduino]")
{
  resetFakes();
    // Arrange
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);

    RESET_FAKE(HardwareSerial_read);

    // Action
    noteserial.receive();

    // Assert
    CHECK(HardwareSerial_read_fake.call_count);
}

TEST_CASE("test_noteserial_arduino_receive_does_not_modify_hardware_serial_read_result_value_before_returning_to_caller", "[NoteSerial_Arduino]")
{
  resetFakes();
    // Arrange
    const char EXPECTED_RESULT = 'z';
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);

    RESET_FAKE(HardwareSerial_read);
    HardwareSerial_read_fake.return_val = EXPECTED_RESULT;

    // Action
    const char ACTUAL_RESULT = noteserial.receive();

    // Assert
    CHECK(ACTUAL_RESULT == EXPECTED_RESULT);
}

TEST_CASE("test_noteserial_arduino_reset_invokes_hardware_serial_end", "[NoteSerial_Arduino]")
{
  resetFakes();
    // Arrange
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);
    RESET_FAKE(HardwareSerial_end);

    // Action
    noteserial.reset();

    // Assert
    CHECK(HardwareSerial_end_fake.call_count);
}

TEST_CASE("test_noteserial_arduino_reset_invokes_hardware_serial_begin", "[NoteSerial_Arduino]")
{
  resetFakes();
    // Arrange
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);
    RESET_FAKE(HardwareSerial_begin);

    // Action
    noteserial.reset();

    // Assert
    CHECK(HardwareSerial_begin_fake.call_count);
}

TEST_CASE("test_noteserial_arduino_reset_invokes_hardware_serial_begin_with_the_baud_parameter_that_was_originally_supplied_to_the_constructor", "[NoteSerial_Arduino]")
{
  resetFakes();
    // Arrange
    const size_t EXPECTED_BAUD_RATE = 9600;
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, EXPECTED_BAUD_RATE);

    RESET_FAKE(HardwareSerial_begin);

    // Action
    noteserial.reset();

    // Assert
    CHECK(EXPECTED_BAUD_RATE == HardwareSerial_begin_fake.arg0_val);
}

TEST_CASE("test_noteserial_arduino_reset_always_returns_true", "[NoteSerial_Arduino]")
{
  resetFakes();
    // Arrange
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);

    // Action
    const bool ACTUAL_RESULT = noteserial.reset();

    // Assert
    CHECK(ACTUAL_RESULT);
}

TEST_CASE("test_noteserial_arduino_transmit_invokes_hardware_serial_write", "[NoteSerial_Arduino]")
{
  resetFakes();
    // Arrange
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);

    RESET_CAPTURE_FAKE(HardwareSerial_write);

    // Action
    noteserial.transmit(reinterpret_cast<uint8_t *>(const_cast<char *>("Hello, Test!")), 13, true);

    // Assert
    CHECK(HardwareSerial_write_fake.call_count);
}

TEST_CASE("test_noteserial_arduino_transmit_does_not_modify_buffer_parameter_value_before_passing_to_hardware_serial_write", "[NoteSerial_Arduino]")
{
  resetFakes();
    // Arrange
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);
    const char EXPECTED_RESULT[] = "Hello, Test!";

    RESET_CAPTURE_FAKE(HardwareSerial_write);

    // Action
    noteserial.transmit(reinterpret_cast<uint8_t *>(const_cast<char *>(EXPECTED_RESULT)), strlen(EXPECTED_RESULT), true);

    // Assert
    CHECK(!strcmp(EXPECTED_RESULT, HardwareSerial_write_arg0_str.c_str()));
}

TEST_CASE("test_noteserial_arduino_transmit_does_not_modify_size_parameter_value_before_passing_to_hardware_serial_write", "[NoteSerial_Arduino]")
{
  resetFakes();
    // Arrange
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);
    const size_t EXPECTED_RESULT = 13;

    RESET_CAPTURE_FAKE(HardwareSerial_write);

    // Action
    noteserial.transmit(reinterpret_cast<uint8_t *>(const_cast<char *>("Hello, Test!")), EXPECTED_RESULT, true);

    // Assert
    CHECK(EXPECTED_RESULT == HardwareSerial_write_fake.arg1_val);
}

TEST_CASE("test_noteserial_arduino_transmit_invokes_hardware_serial_flush_when_flush_parameter_is_true", "[NoteSerial_Arduino]")
{
  resetFakes();
    // Arrange
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);
    RESET_FAKE(HardwareSerial_flush);

    // Action
    noteserial.transmit(reinterpret_cast<uint8_t *>(const_cast<char *>("Hello, Test!")), 13, true);

    // Assert
    CHECK(HardwareSerial_flush_fake.call_count);
}

TEST_CASE("test_noteserial_arduino_transmit_does_not_invoke_hardware_serial_flush_when_flush_parameter_is_false", "[NoteSerial_Arduino]")
{
  resetFakes();
    // Arrange
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);
    RESET_FAKE(HardwareSerial_flush);

    // Action
    noteserial.transmit(reinterpret_cast<uint8_t *>(const_cast<char *>("Hello, Test 14!")), 16, false);

    // Assert
    CHECK(!HardwareSerial_flush_fake.call_count);
}

TEST_CASE("test_noteserial_arduino_transmit_does_not_modify_hardware_serial_write_result_value_before_returning_to_caller", "[NoteSerial_Arduino]")
{
  resetFakes();
    // Arrange
    const size_t EXPECTED_RESULT = 13;
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);

    RESET_CAPTURE_FAKE(HardwareSerial_write);
    HardwareSerial_write_fake.return_val = EXPECTED_RESULT;

    // Action
    const size_t ACTUAL_RESULT = noteserial.transmit(reinterpret_cast<uint8_t *>(const_cast<char *>("Hello, Test!")), 13, true);

    // Assert
    CHECK(ACTUAL_RESULT == EXPECTED_RESULT);
}
