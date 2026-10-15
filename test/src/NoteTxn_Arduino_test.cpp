#include <catch2/catch_test_macros.hpp>

#include "arduino_fakes.hpp"
#include "NoteTxn_Arduino.hpp"

#include <cassert>


// Called first in every TEST_CASE: RESET_FAKE on every FFF fake, then
// FFF_RESET_HISTORY() (see resetArduinoFakes()).
static void resetFakes(void)
{
  resetArduinoFakes();
}

TEST_CASE("test_make_note_txn_instantiates_notetxn_object", "[NoteTxn_Arduino]")
{
  resetFakes();
  // Arrange
  NoteTxn * notetxn = nullptr;
  uint8_t txn_pins[2] = {19, 79};

  // Action
  notetxn = make_note_txn(txn_pins);

  // Assert
  CHECK(nullptr != notetxn);

  // Clean-up
  uint8_t invalid_pins[2] = {0};
  make_note_txn(invalid_pins);
}

TEST_CASE("test_make_note_txn_enforces_singleton_by_returning_same_notetxn_object_for_all_calls", "[NoteTxn_Arduino]")
{
  resetFakes();
  // Arrange
  uint8_t txn_pins_1[2] = {19, 79};
  NoteTxn * const notetxn_1 = make_note_txn(txn_pins_1);

  // Action
  uint8_t txn_pins_2[2] = {9, 17};
  NoteTxn * const notetxn_2 = make_note_txn(txn_pins_2);

  // Assert
  CHECK(notetxn_1 == notetxn_2);

  // Clean-up
  uint8_t invalid_pins[2] = {0};
  make_note_txn(invalid_pins);
}

//int test_make_note_txn_returns_nullptr_when_same_pins_are_passed_as_parameter()
TEST_CASE("test_make_note_txn_deletes_singleton_when_same_pins_are_passed_as_parameter", "[NoteTxn_Arduino]")
{
  resetFakes();
  // Arrange
  uint8_t txn_pins[2] = {19, 79};
  NoteTxn * notetxn = make_note_txn(txn_pins);
  assert(notetxn);

  // Action
  uint8_t invalid_pins[2] = {0};
  notetxn = make_note_txn(invalid_pins);

  // Assert
  CHECK(nullptr == notetxn);
}

TEST_CASE("test_notetxn_arduino_constructor_floats_ctx_pin", "[NoteTxn_Arduino]")
{
  resetFakes();
  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;
  RESET_FAKE(pinMode);

  // Action
  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);

  // Assert
  CHECK(pinMode_values(CTX_PIN)[(pinMode_invoked(CTX_PIN) - 1)] == INPUT);
}

TEST_CASE("test_notetxn_arduino_constructor_floats_rtx_pin", "[NoteTxn_Arduino]")
{
  resetFakes();
  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;
  RESET_FAKE(pinMode);

  // Action
  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);

  // Assert
  CHECK(pinMode_values(RTX_PIN)[(pinMode_invoked(RTX_PIN) - 1)] == INPUT);
}

TEST_CASE("test_notetxn_arduino_start_initially_configures_ctx_pin_as_input_pullup", "[NoteTxn_Arduino]")
{
  resetFakes();
  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;
  const size_t TIMEOUT_MS = 917;

  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);
  RESET_FAKE(digitalRead);
  digitalRead_fake.return_val = HIGH;
  RESET_FAKE(pinMode);

  // Action
  notetxn.start(TIMEOUT_MS);

  // Assert
  REQUIRE(pinMode_invoked(CTX_PIN));
  CHECK(pinMode_values(CTX_PIN)[0] == INPUT_PULLUP);
}

TEST_CASE("test_notetxn_arduino_start_configures_rtx_pin_as_output", "[NoteTxn_Arduino]")
{
  resetFakes();
  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;
  const size_t TIMEOUT_MS = 917;

  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);
  RESET_FAKE(digitalRead);
  digitalRead_fake.return_val = HIGH;
  RESET_FAKE(pinMode);

  // Action
  notetxn.start(TIMEOUT_MS);

  // Assert
  REQUIRE(pinMode_invoked(RTX_PIN));
  CHECK(pinMode_values(RTX_PIN)[(pinMode_invoked(RTX_PIN) - 1)] == OUTPUT);
}

TEST_CASE("test_notetxn_arduino_start_sets_rtx_pin_high", "[NoteTxn_Arduino]")
{
  resetFakes();
  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;
  const size_t TIMEOUT_MS = 917;

  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);
  RESET_FAKE(digitalWrite);
  RESET_FAKE(digitalRead);
  digitalRead_fake.return_val = HIGH;

  // Action
  notetxn.start(TIMEOUT_MS);

  // Assert
  REQUIRE(digitalWrite_invoked(RTX_PIN));
  CHECK(digitalWrite_values(RTX_PIN)[(digitalWrite_invoked(RTX_PIN) - 1)] == HIGH);
}

TEST_CASE("test_notetxn_arduino_start_blocks_until_ctx_pin_goes_high", "[NoteTxn_Arduino]")
{
  resetFakes();
  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;
  const size_t TIMEOUT_MS = 917;

  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);
  RESET_FAKE(digitalRead);
  int digitalRead_results[] = {LOW, LOW, LOW, HIGH};
  SET_RETURN_SEQ(digitalRead, digitalRead_results, 4);
  RESET_FAKE(NoteGetMs);
  NoteGetMs_fake.return_val = (TIMEOUT_MS - 1);

  // Action
  notetxn.start(TIMEOUT_MS);

  // Assert
  CHECK(digitalRead_invoked(CTX_PIN) == static_cast<size_t>(digitalRead_fake.return_val_seq_len));
  CHECK(digitalRead_results[(digitalRead_invoked(CTX_PIN) - 1)] == HIGH);
  CHECK(0 == NoteGetMs_fake.return_val_seq_len);
}

TEST_CASE("test_notetxn_arduino_start_blocks_until_timeout_ms", "[NoteTxn_Arduino]")
{
  resetFakes();
  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;
  const size_t TIMEOUT_MS = 917;

  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);
  RESET_FAKE(digitalRead);
  digitalRead_fake.return_val = LOW;
  RESET_FAKE(NoteDelayMs);
  RESET_FAKE(NoteGetMs);
  uint32_t noteGetMs_results[] = {36, 274, 515, 801, 0};
  noteGetMs_results[4] = (TIMEOUT_MS + noteGetMs_results[0]);
  SET_RETURN_SEQ(NoteGetMs, noteGetMs_results, 5);

  // Action
  notetxn.start(TIMEOUT_MS);

  // Assert
  CHECK(digitalRead_invoked(CTX_PIN));
  CHECK(0 == digitalRead_fake.return_val_seq_len);
  CHECK(NoteGetMs_fake.call_count == static_cast<unsigned int>(NoteGetMs_fake.return_val_seq_len));
}

TEST_CASE("test_notetxn_arduino_start_leaves_rtx_pin_as_output_when_ctx_responds_high", "[NoteTxn_Arduino]")
{
  resetFakes();
  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;
  const size_t TIMEOUT_MS = 917;

  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);
  RESET_FAKE(digitalRead);
  digitalRead_fake.return_val = HIGH;
  RESET_FAKE(millis);
  RESET_FAKE(pinMode);

  // Action
  notetxn.start(TIMEOUT_MS);

  // Assert
  CHECK(digitalRead_invoked(CTX_PIN));
  CHECK(0 == digitalRead_fake.return_val_seq_len);
  CHECK(pinMode_values(RTX_PIN)[(pinMode_invoked(RTX_PIN) - 1)] == OUTPUT);
}

TEST_CASE("test_notetxn_arduino_start_leaves_rtx_pin_high_when_ctx_responds_high", "[NoteTxn_Arduino]")
{
  resetFakes();
  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;
  const size_t TIMEOUT_MS = 917;

  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);
  RESET_FAKE(digitalRead);
  digitalRead_fake.return_val = HIGH;
  RESET_FAKE(digitalWrite);
  RESET_FAKE(NoteGetMs);

  // Action
  notetxn.start(TIMEOUT_MS);

  // Assert
  CHECK(digitalRead_invoked(CTX_PIN));
  CHECK(0 == digitalRead_fake.return_val_seq_len);
  CHECK(digitalWrite_values(RTX_PIN)[(digitalWrite_invoked(RTX_PIN) - 1)] == HIGH);
}

TEST_CASE("test_notetxn_arduino_start_leaves_ctx_pin_floating_when_ctx_responds_high", "[NoteTxn_Arduino]")
{
  resetFakes();
  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;
  const size_t TIMEOUT_MS = 917;

  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);
  RESET_FAKE(digitalRead);
  digitalRead_fake.return_val = HIGH;
  RESET_FAKE(pinMode);

  // Action
  notetxn.start(TIMEOUT_MS);

  // Assert
  CHECK(pinMode_invoked(CTX_PIN) > 1);
  CHECK(pinMode_values(CTX_PIN)[(pinMode_invoked(CTX_PIN) - 1)] == INPUT);
}

TEST_CASE("test_notetxn_arduino_start_leaves_ctx_pin_floating_on_timeout", "[NoteTxn_Arduino]")
{
  resetFakes();
  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;
  const size_t TIMEOUT_MS = 917;

  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);
  RESET_FAKE(digitalRead);
  digitalRead_fake.return_val = LOW;
  RESET_FAKE(NoteGetMs);
  uint32_t noteGetMs_results[] = {36, 274, 515, 801, 0};
  noteGetMs_results[4] = (TIMEOUT_MS + noteGetMs_results[0]);
  SET_RETURN_SEQ(NoteGetMs, noteGetMs_results, 5);
  RESET_FAKE(pinMode);

  // Action
  notetxn.start(TIMEOUT_MS);

  // Assert
  CHECK(digitalRead_invoked(CTX_PIN));
  CHECK(0 == digitalRead_fake.return_val_seq_len);
  CHECK(NoteGetMs_fake.call_count == static_cast<unsigned int>(NoteGetMs_fake.return_val_seq_len));
  CHECK(pinMode_invoked(CTX_PIN) > 1);
  CHECK(pinMode_values(CTX_PIN)[(pinMode_invoked(CTX_PIN) - 1)] == INPUT);
}

TEST_CASE("test_notetxn_arduino_start_floats_rtx_pin_on_timeout", "[NoteTxn_Arduino]")
{
  resetFakes();
  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;
  const size_t TIMEOUT_MS = 917;

  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);
  RESET_FAKE(digitalRead);
  digitalRead_fake.return_val = LOW;
  RESET_FAKE(NoteGetMs);
  uint32_t noteGetMs_results[] = {36, 274, 515, 801, 0};
  noteGetMs_results[4] = (TIMEOUT_MS + noteGetMs_results[0]);
  SET_RETURN_SEQ(NoteGetMs, noteGetMs_results, 5);
  RESET_FAKE(pinMode);

  // Action
  notetxn.start(TIMEOUT_MS);

  // Assert
  CHECK(digitalRead_invoked(CTX_PIN));
  CHECK(0 == digitalRead_fake.return_val_seq_len);
  CHECK(NoteGetMs_fake.call_count == static_cast<unsigned int>(NoteGetMs_fake.return_val_seq_len));
  CHECK(pinMode_values(RTX_PIN)[(pinMode_invoked(RTX_PIN) - 1)] == INPUT);
}

TEST_CASE("test_notetxn_arduino_stop_floats_rtx_pin", "[NoteTxn_Arduino]")
{
  resetFakes();
  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;

  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);
  RESET_FAKE(digitalWrite);
  RESET_FAKE(pinMode);

  // Action
  notetxn.stop();

  // Assert
  CHECK(pinMode_invoked(RTX_PIN));
  CHECK(pinMode_values(RTX_PIN)[(pinMode_invoked(RTX_PIN) - 1)] == INPUT);
}
