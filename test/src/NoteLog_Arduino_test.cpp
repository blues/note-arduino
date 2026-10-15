#include <catch2/catch_test_macros.hpp>

#include "arduino_fakes.hpp"
#include "NoteLog_Arduino.hpp"

#include <cassert>
#include <cstring>
#include <iostream>


// Called first in every TEST_CASE: RESET_FAKE on every FFF fake, then
// FFF_RESET_HISTORY() (see resetArduinoFakes()).
static void resetFakes(void)
{
  resetArduinoFakes();
}

TEST_CASE("test_make_note_log_instantiates_notelog_object", "[NoteLog_Arduino]")
{
  resetFakes();
    // Arrange
    NoteLog * notelog = nullptr;
    Stream & serial_stream = Serial;

    // Action
    notelog = make_note_log(serial_stream);

    // Assert
    CHECK(nullptr != notelog);

    // Clean-up
    make_note_log(nullptr);
}

TEST_CASE("test_make_note_log_enforces_singleton_by_returning_same_notelog_object_for_all_calls", "[NoteLog_Arduino]")
{
  resetFakes();
    // Arrange
    Stream & serial_stream = Serial;
    NoteLog * const notelog_1 = make_note_log(serial_stream);

    // Action
    NoteLog * const notelog_2 = make_note_log(serial_stream);

    // Assert
    CHECK(notelog_1 == notelog_2);

    // Clean-up
    make_note_log(nullptr);
}

//int test_make_note_log_returns_nullptr_when_nullptr_is_passed_as_parameter()
TEST_CASE("test_make_note_log_deletes_singleton_when_nullptr_is_passed_as_parameter", "[NoteLog_Arduino]")
{
  resetFakes();
    // Arrange
    Stream & serial_stream = Serial;
    NoteLog * notelog = make_note_log(serial_stream);
    assert(notelog);

    // Action
    notelog = make_note_log(nullptr);

    // Assert
    CHECK(nullptr == notelog);
}

TEST_CASE("test_notelog_arduino_print_does_not_modify_str_parameter_value_before_passing_to_stream_print", "[NoteLog_Arduino]")
{
  resetFakes();
    // Arrange
    NoteLog_Arduino notelog(&Serial);
    const char EXPECTED_RESULT[] = "Hello, Test!";

    RESET_CAPTURE_FAKE(Stream_print);

    // Action
    notelog.print(EXPECTED_RESULT);

    // Assert
    CHECK(!strcmp(EXPECTED_RESULT, Stream_print_arg0_str.c_str()));
}

TEST_CASE("test_notelog_arduino_print_does_not_modify_stream_print_result_value_before_returning_to_caller", "[NoteLog_Arduino]")
{
  resetFakes();
    // Arrange
    NoteLog_Arduino notelog(&Serial);
    const size_t EXPECTED_RESULT = 13;

    RESET_CAPTURE_FAKE(Stream_print);
    Stream_print_fake.return_val = EXPECTED_RESULT;

    // Action
    const size_t ACTUAL_RESULT = notelog.print("Hello, Test!");

    // Assert
    CHECK(ACTUAL_RESULT == EXPECTED_RESULT);
}
