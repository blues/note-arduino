#include <catch2/catch_test_macros.hpp>

#include "arduino_fakes.hpp"
#include "NoteI2c_Arduino.hpp"

#include <cassert>
#include <cstring>


// Called first in every TEST_CASE: RESET_FAKE on every FFF fake, then
// FFF_RESET_HISTORY() (see resetArduinoFakes()).
static void resetFakes(void)
{
  resetArduinoFakes();
}

TEST_CASE("test_make_note_i2c_instantiates_notei2c_object", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  NoteI2c * notei2c = nullptr;

  // Action
  notei2c = make_note_i2c(Wire);

  // Assert
  CHECK(nullptr != notei2c);

  // Clean-up
  make_note_i2c(nullptr);
}

TEST_CASE("test_make_note_i2c_enforces_singleton_by_returning_same_notei2c_object_for_all_calls", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  NoteI2c * const notei2c_1 = make_note_i2c(Wire);

  // Action
  NoteI2c * const notei2c_2 = make_note_i2c(Wire);

  // Assert
  CHECK(notei2c_1 == notei2c_2);

  // Clean-up
  make_note_i2c(nullptr);
}

//int test_make_note_i2c_returns_nullptr_when_nullptr_is_passed_as_parameter()
TEST_CASE("test_make_note_i2c_deletes_singleton_when_nullptr_is_passed_as_parameter", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  NoteI2c * notei2c = make_note_i2c(Wire);
  assert(notei2c);

  // Action
  notei2c = make_note_i2c(nullptr);

  // Assert
  CHECK(nullptr == notei2c);
}

TEST_CASE("test_notei2c_arduino_constructor_invokes_twowire_parameter_begin_method", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  RESET_FAKE(TwoWire_begin);

  // Action
  NoteI2c_Arduino notei2c(Wire);

  // Assert
  CHECK(TwoWire_begin_fake.call_count);
}

#if not defined(WIRE_HAS_END)

TEST_CASE("test_notei2c_arduino_deconstructor_does_not_invoke_twowire_end_method", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  NoteI2c_Arduino * notei2c = new NoteI2c_Arduino(Wire);
  RESET_FAKE(TwoWire_end);

  // Action
  delete notei2c;

  // Assert
  CHECK(!TwoWire_end_fake.call_count);
}

#else // defined(WIRE_HAS_END)

TEST_CASE("test_notei2c_arduino_deconstructor_invokes_twowire_end_method", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  NoteI2c_Arduino * notei2c = new NoteI2c_Arduino(Wire);
  RESET_FAKE(TwoWire_end);

  // Action
  delete notei2c;

  // Assert
  CHECK(TwoWire_end_fake.call_count);
}

#endif // not defined(WIRE_HAS_END)

TEST_CASE("test_notei2c_arduino_receive_requests_response_data_from_notecard", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  const uint8_t REQUEST_SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t actual_response_size;

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_endTransmission);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_endTransmission, uint8_t, 0);
  NoteI2c_Arduino notei2c(Wire);

  // Action
  notei2c.receive(
    EXPECTED_ADDRESS,
    response_buffer,
    REQUEST_SIZE,
    &actual_response_size
  );

  // Assert
  CHECK(TwoWire_beginTransmission_fake.arg0_val == EXPECTED_ADDRESS);
  CHECK(TwoWire_write_byte_fake.arg0_history[0] == 0);
  CHECK(TwoWire_write_byte_fake.arg0_history[1] == REQUEST_SIZE);
  CHECK(TwoWire_endTransmission_fake.call_count);
}

TEST_CASE("test_notei2c_arduino_receive_will_retry_transmission_on_i2c_failure", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_RESULT = 0x17;
  const uint8_t REQUEST_SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t actual_response_size;

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_endTransmission);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_endTransmission, uint8_t, 1,0);
  NoteI2c_Arduino notei2c(Wire);

  // Action
  notei2c.receive(
    EXPECTED_RESULT,
    response_buffer,
    REQUEST_SIZE,
    &actual_response_size
  );

  // Assert
  CHECK(TwoWire_beginTransmission_fake.call_count > 1);
  CHECK(TwoWire_write_byte_fake.call_count > 2);
  CHECK(TwoWire_endTransmission_fake.call_count > 1);
}

TEST_CASE("test_notei2c_arduino_receive_will_not_retry_transmission_on_i2c_success", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_RESULT = 0x17;
  const uint8_t REQUEST_SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t actual_response_size;

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_endTransmission);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_endTransmission, uint8_t, 0);
  RESET_FAKE(TwoWire_requestFrom);
  TwoWire_requestFrom_fake.return_val = (REQUEST_SIZE + NoteI2c::REQUEST_HEADER_SIZE);
  RESET_FAKE(TwoWire_read);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_read, int, 0, REQUEST_SIZE, 'T', 'e', 's', 't', ' ', 'P', 'a', 's', 's', 'e', 'd', '!', '\0');
  NoteI2c_Arduino notei2c(Wire);

  // Action
  notei2c.receive(
    EXPECTED_RESULT,
    response_buffer,
    REQUEST_SIZE,
    &actual_response_size
  );

  // Assert
  CHECK(TwoWire_beginTransmission_fake.call_count == 1);
  CHECK(TwoWire_write_byte_fake.call_count == 2);
  CHECK(TwoWire_endTransmission_fake.call_count == 1);
}

TEST_CASE("test_notei2c_arduino_receive_will_only_retry_i2c_transmission_thrice", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_RESULT = 0x17;
  const uint8_t REQUEST_SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t actual_response_size;

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_endTransmission);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_endTransmission, uint8_t, 1,1,1,1,0);
  NoteI2c_Arduino notei2c(Wire);

  // Action
  notei2c.receive(
    EXPECTED_RESULT,
    response_buffer,
    REQUEST_SIZE,
    &actual_response_size
  );

  // Assert
  CHECK(TwoWire_beginTransmission_fake.call_count == 4);
  CHECK(TwoWire_write_byte_fake.call_count == 8);
  CHECK(TwoWire_endTransmission_fake.call_count == 4);
}

TEST_CASE("test_notei2c_arduino_receive_reads_response_data_from_notecard", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  const uint8_t REQUEST_SIZE = 13;
  const char * EXPECTED_RESPONSE = "Test Passed!";
  uint8_t response_buffer[32];
  uint32_t bytes_remaining;

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_endTransmission);
  RESET_FAKE(TwoWire_requestFrom);
  TwoWire_requestFrom_fake.return_val = (REQUEST_SIZE + NoteI2c::REQUEST_HEADER_SIZE);
  RESET_FAKE(TwoWire_read);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_read, int, 0, REQUEST_SIZE, 'T', 'e', 's', 't', ' ', 'P', 'a', 's', 's', 'e', 'd', '!', '\0');
  NoteI2c_Arduino notei2c(Wire);

  // Action
  notei2c.receive(
    EXPECTED_ADDRESS,
    response_buffer,
    REQUEST_SIZE,
    &bytes_remaining
  );

  // Assert
  CHECK(TwoWire_requestFrom_fake.arg0_val == EXPECTED_ADDRESS);
  CHECK(TwoWire_requestFrom_fake.arg1_val == (REQUEST_SIZE + NoteI2c::REQUEST_HEADER_SIZE));
  CHECK(!memcmp(response_buffer, EXPECTED_RESPONSE, REQUEST_SIZE));
}

TEST_CASE("test_notei2c_arduino_receive_updates_available_parameter_with_remaining_bytes", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  const uint8_t REQUEST_SIZE = 8;
  uint8_t response_buffer[32];
  uint32_t bytes_remaining;

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_endTransmission);
  RESET_FAKE(TwoWire_requestFrom);
  TwoWire_requestFrom_fake.return_val = (REQUEST_SIZE + NoteI2c::REQUEST_HEADER_SIZE);
  RESET_FAKE(TwoWire_read);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_read, int, 5, REQUEST_SIZE, 'T', 'e', 's', 't', ' ', 'P', 'a', 's');
  NoteI2c_Arduino notei2c(Wire);

  // Action
  notei2c.receive(
    EXPECTED_ADDRESS,
    response_buffer,
    REQUEST_SIZE,
    &bytes_remaining
  );

  // Assert
  CHECK(bytes_remaining == 5);
}

TEST_CASE("test_notei2c_arduino_receive_does_not_request_or_read_i2c_when_trasmission_error_occurs", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  const uint8_t REQUEST_SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t bytes_remaining;

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_endTransmission);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_endTransmission, uint8_t, 1,1,1,1);
  RESET_FAKE(TwoWire_requestFrom);
  TwoWire_requestFrom_fake.return_val = (REQUEST_SIZE + NoteI2c::REQUEST_HEADER_SIZE);
  RESET_FAKE(TwoWire_read);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_read, int, 0, REQUEST_SIZE, 'T', 'e', 's', 't', ' ', 'P', 'a', 's', 's', 'e', 'd', '!', '\0');
  NoteI2c_Arduino notei2c(Wire);

  // Action
  notei2c.receive(
    EXPECTED_ADDRESS,
    response_buffer,
    REQUEST_SIZE,
    &bytes_remaining
  );

  // Assert
  CHECK(!TwoWire_requestFrom_fake.call_count);
  CHECK(!TwoWire_read_fake.call_count);
}

TEST_CASE("test_notei2c_arduino_receive_will_not_attempt_to_read_when_i2c_port_request_from_returns_zero", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  const uint8_t REQUEST_SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t bytes_remaining;

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_endTransmission);
  RESET_FAKE(TwoWire_requestFrom);
  TwoWire_requestFrom_fake.return_val = 0;
  RESET_FAKE(TwoWire_read);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_read, int, 0, REQUEST_SIZE, 'T', 'e', 's', 't', ' ', 'P', 'a', 's', 's', 'e', 'd', '!', '\0');
  NoteI2c_Arduino notei2c(Wire);

  // Action
  notei2c.receive(
    EXPECTED_ADDRESS,
    response_buffer,
    REQUEST_SIZE,
    &bytes_remaining
  );

  // Assert
  CHECK(TwoWire_requestFrom_fake.call_count);
  CHECK(!TwoWire_read_fake.call_count);
}

TEST_CASE("test_notei2c_arduino_receive_will_not_attempt_to_read_remaining_bytes_when_first_call_to_two_wire_read_returns_available_value_greater_than_note_i2c_request_max_size_minus_note_i2c_request_header_size", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint8_t AVAILABLE_SIZE = ((NoteI2c_Arduino::REQUEST_MAX_SIZE - NoteI2c_Arduino::REQUEST_HEADER_SIZE) + 1);
  const uint16_t EXPECTED_ADDRESS = 0x17;
  const uint8_t REQUEST_SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t bytes_remaining;

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_endTransmission);
  RESET_FAKE(TwoWire_requestFrom);
  TwoWire_requestFrom_fake.return_val = (REQUEST_SIZE + NoteI2c::REQUEST_HEADER_SIZE);
  RESET_FAKE(TwoWire_read);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_read, int, AVAILABLE_SIZE, AVAILABLE_SIZE, AVAILABLE_SIZE, AVAILABLE_SIZE, REQUEST_SIZE, 'T', 'e', 's', 't', ' ', 'P', 'a', 's', 's', 'e', 'd', '!', '\0');
  NoteI2c_Arduino notei2c(Wire);

  // Action
  notei2c.receive(
    EXPECTED_ADDRESS,
    response_buffer,
    REQUEST_SIZE,
    &bytes_remaining
  );

  // Assert
  CHECK(TwoWire_requestFrom_fake.call_count);
  CHECK((4 >= TwoWire_read_fake.call_count));
}

TEST_CASE("test_notei2c_arduino_receive_will_not_attempt_to_read_when_i2c_port_request_from_returns_value_other_than_buffer_size_plus_note_i2c_request_header_size", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  const uint8_t REQUEST_SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t bytes_remaining;

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_endTransmission);
  RESET_FAKE(TwoWire_requestFrom);
  TwoWire_requestFrom_fake.return_val = sizeof(response_buffer);
  RESET_FAKE(TwoWire_read);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_read, int, 0, REQUEST_SIZE, 'T', 'e', 's', 't', ' ', 'P', 'a', 's', 's', 'e', 'd', '!', '\0');
  NoteI2c_Arduino notei2c(Wire);

  // Action
  notei2c.receive(
    EXPECTED_ADDRESS,
    response_buffer,
    REQUEST_SIZE,
    &bytes_remaining
  );

  // Assert
  CHECK(TwoWire_requestFrom_fake.call_count);
  CHECK(!TwoWire_read_fake.call_count);
}

TEST_CASE("test_notei2c_arduino_receive_will_not_read_full_response_when_i2c_over_serial_protocol_returns_value_other_than_buffer_size", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  const uint8_t REQUEST_SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t bytes_remaining;

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_endTransmission);
  RESET_FAKE(TwoWire_requestFrom);
  TwoWire_requestFrom_fake.return_val = (REQUEST_SIZE + NoteI2c::REQUEST_HEADER_SIZE);
  RESET_FAKE(TwoWire_read);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_read, int, 0, sizeof(response_buffer), 0, sizeof(response_buffer), 0, sizeof(response_buffer), 0, sizeof(response_buffer), 'T', 'e', 's', 't', ' ', 'P', 'a', 's', 's', 'e', 'd', '!', '\0');
  NoteI2c_Arduino notei2c(Wire);

  // Action
  notei2c.receive(
    EXPECTED_ADDRESS,
    response_buffer,
    REQUEST_SIZE,
    &bytes_remaining
  );

  // Assert
  CHECK(TwoWire_requestFrom_fake.call_count);
  CHECK(TwoWire_read_fake.call_count == 8);
}

TEST_CASE("test_notei2c_arduino_receive_returns_nullptr_on_success", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  const uint8_t REQUEST_SIZE = 13;
  const char * EXPECTED_RESPONSE = "Test Passed!";
  uint8_t response_buffer[32];
  uint32_t bytes_remaining;

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_endTransmission);
  RESET_FAKE(TwoWire_requestFrom);
  TwoWire_requestFrom_fake.return_val = (REQUEST_SIZE + NoteI2c::REQUEST_HEADER_SIZE);
  RESET_FAKE(TwoWire_read);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_read, int, 0, REQUEST_SIZE, 'T', 'e', 's', 't', ' ', 'P', 'a', 's', 's', 'e', 'd', '!', '\0');
  NoteI2c_Arduino notei2c(Wire);

  // Action
  const char * const ACTUAL_RESULT = notei2c.receive(
    EXPECTED_ADDRESS,
    response_buffer,
    REQUEST_SIZE,
    &bytes_remaining
  );

  // Assert
  CHECK(!memcmp(response_buffer, EXPECTED_RESPONSE, REQUEST_SIZE));
  CHECK(ACTUAL_RESULT == nullptr);
}

TEST_CASE("test_notei2c_arduino_receive_returns_error_message_on_i2c_transmission_failure_1", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  const uint8_t REQUEST_SIZE = 13;
  const char * EXPECTED_RESULT = "i2c|rx: data too long to fit in transmit buffer {io}{i2c}";
  uint8_t response_buffer[32];
  uint32_t bytes_remaining;

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_endTransmission);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_endTransmission, uint8_t, 1,1,1,1);
  RESET_FAKE(TwoWire_requestFrom);
  TwoWire_requestFrom_fake.return_val = (REQUEST_SIZE + NoteI2c::REQUEST_HEADER_SIZE);
  RESET_FAKE(TwoWire_read);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_read, int, 0, REQUEST_SIZE, 'T', 'e', 's', 't', ' ', 'P', 'a', 's', 's', 'e', 'd', '!', '\0');
  NoteI2c_Arduino notei2c(Wire);

  // Action
  const char * const ACTUAL_RESULT = notei2c.receive(
    EXPECTED_ADDRESS,
    response_buffer,
    REQUEST_SIZE,
    &bytes_remaining
  );

  // Assert
  CHECK(ACTUAL_RESULT);
  CHECK(!memcmp(ACTUAL_RESULT, EXPECTED_RESULT, strlen(EXPECTED_RESULT)));
}

TEST_CASE("test_notei2c_arduino_receive_returns_error_message_on_i2c_transmission_failure_2", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  const uint8_t REQUEST_SIZE = 13;
  const char * EXPECTED_RESULT = "i2c|rx: received NACK on transmit of address {io}{i2c}";
  uint8_t response_buffer[32];
  uint32_t bytes_remaining;

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_endTransmission);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_endTransmission, uint8_t, 2,2,2,2);
  RESET_FAKE(TwoWire_requestFrom);
  TwoWire_requestFrom_fake.return_val = (REQUEST_SIZE + NoteI2c::REQUEST_HEADER_SIZE);
  RESET_FAKE(TwoWire_read);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_read, int, 0, REQUEST_SIZE, 'T', 'e', 's', 't', ' ', 'P', 'a', 's', 's', 'e', 'd', '!', '\0');
  NoteI2c_Arduino notei2c(Wire);

  // Action
  const char * const ACTUAL_RESULT = notei2c.receive(
    EXPECTED_ADDRESS,
    response_buffer,
    REQUEST_SIZE,
    &bytes_remaining
  );

  // Assert
  CHECK(ACTUAL_RESULT);
  CHECK(!memcmp(ACTUAL_RESULT, EXPECTED_RESULT, strlen(EXPECTED_RESULT)));
}

TEST_CASE("test_notei2c_arduino_receive_returns_error_message_on_i2c_transmission_failure_3", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  const uint8_t REQUEST_SIZE = 13;
  const char * EXPECTED_RESULT = "i2c|rx: received NACK on transmit of data {io}{i2c}";
  uint8_t response_buffer[32];
  uint32_t bytes_remaining;

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_endTransmission);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_endTransmission, uint8_t, 3,3,3,3);
  RESET_FAKE(TwoWire_requestFrom);
  TwoWire_requestFrom_fake.return_val = (REQUEST_SIZE + NoteI2c::REQUEST_HEADER_SIZE);
  RESET_FAKE(TwoWire_read);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_read, int, 0, REQUEST_SIZE, 'T', 'e', 's', 't', ' ', 'P', 'a', 's', 's', 'e', 'd', '!', '\0');
  NoteI2c_Arduino notei2c(Wire);

  // Action
  const char * const ACTUAL_RESULT = notei2c.receive(
    EXPECTED_ADDRESS,
    response_buffer,
    REQUEST_SIZE,
    &bytes_remaining
  );

  // Assert
  CHECK(ACTUAL_RESULT);
  CHECK(!memcmp(ACTUAL_RESULT, EXPECTED_RESULT, strlen(EXPECTED_RESULT)));
}

TEST_CASE("test_notei2c_arduino_receive_returns_error_message_on_i2c_transmission_failure_4", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  const uint8_t REQUEST_SIZE = 13;
  const char * EXPECTED_RESULT = "i2c|rx: unknown error on TwoWire::endTransmission() {io}{i2c}";
  uint8_t response_buffer[32];
  uint32_t bytes_remaining;

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_endTransmission);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_endTransmission, uint8_t, 4,4,4,4);
  RESET_FAKE(TwoWire_requestFrom);
  TwoWire_requestFrom_fake.return_val = (REQUEST_SIZE + NoteI2c::REQUEST_HEADER_SIZE);
  RESET_FAKE(TwoWire_read);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_read, int, 0, REQUEST_SIZE, 'T', 'e', 's', 't', ' ', 'P', 'a', 's', 's', 'e', 'd', '!', '\0');
  NoteI2c_Arduino notei2c(Wire);

  // Action
  const char * const ACTUAL_RESULT = notei2c.receive(
    EXPECTED_ADDRESS,
    response_buffer,
    REQUEST_SIZE,
    &bytes_remaining
  );

  // Assert
  CHECK(ACTUAL_RESULT);
  CHECK(!memcmp(ACTUAL_RESULT, EXPECTED_RESULT, strlen(EXPECTED_RESULT)));
}

TEST_CASE("test_notei2c_arduino_receive_returns_error_message_on_i2c_transmission_failure_5", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  const uint8_t REQUEST_SIZE = 13;
  const char * EXPECTED_RESULT = "i2c|rx: timeout {io}{i2c}";
  uint8_t response_buffer[32];
  uint32_t bytes_remaining;

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_endTransmission);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_endTransmission, uint8_t, 5,5,5,5);
  RESET_FAKE(TwoWire_requestFrom);
  TwoWire_requestFrom_fake.return_val = (REQUEST_SIZE + NoteI2c::REQUEST_HEADER_SIZE);
  RESET_FAKE(TwoWire_read);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_read, int, 0, REQUEST_SIZE, 'T', 'e', 's', 't', ' ', 'P', 'a', 's', 's', 'e', 'd', '!', '\0');
  NoteI2c_Arduino notei2c(Wire);

  // Action
  const char * const ACTUAL_RESULT = notei2c.receive(
    EXPECTED_ADDRESS,
    response_buffer,
    REQUEST_SIZE,
    &bytes_remaining
  );

  // Assert
  CHECK(ACTUAL_RESULT);
  CHECK(!memcmp(ACTUAL_RESULT, EXPECTED_RESULT, strlen(EXPECTED_RESULT)));
}

TEST_CASE("test_notei2c_arduino_receive_returns_error_message_on_unexpected_i2c_transmission_failure", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  const uint8_t REQUEST_SIZE = 13;
  const char * EXPECTED_RESULT = "i2c|rx: unknown error encountered during I2C transmission {io}{i2c}";
  uint8_t response_buffer[32];
  uint32_t bytes_remaining;

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_endTransmission);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_endTransmission, uint8_t, 6,6,6,6);
  RESET_FAKE(TwoWire_requestFrom);
  TwoWire_requestFrom_fake.return_val = (REQUEST_SIZE + NoteI2c::REQUEST_HEADER_SIZE);
  RESET_FAKE(TwoWire_read);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_read, int, 0, REQUEST_SIZE, 'T', 'e', 's', 't', ' ', 'P', 'a', 's', 's', 'e', 'd', '!', '\0');
  NoteI2c_Arduino notei2c(Wire);

  // Action
  const char * const ACTUAL_RESULT = notei2c.receive(
    EXPECTED_ADDRESS,
    response_buffer,
    REQUEST_SIZE,
    &bytes_remaining
  );

  // Assert
  CHECK(ACTUAL_RESULT);
  CHECK(!memcmp(ACTUAL_RESULT, EXPECTED_RESULT, strlen(EXPECTED_RESULT)));
}

TEST_CASE("test_notei2c_arduino_receive_returns_error_message_on_i2c_protocol_failure_to_respond", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  const uint8_t REQUEST_SIZE = 13;
  const char * EXPECTED_RESULT = "i2c|rx: no response to read request {io}{i2c}";
  uint8_t response_buffer[32];
  uint32_t bytes_remaining;

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_endTransmission);
  RESET_FAKE(TwoWire_requestFrom);
  RESET_FAKE(TwoWire_read);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_read, int, 0, REQUEST_SIZE, 'T', 'e', 's', 't', ' ', 'P', 'a', 's', 's', 'e', 'd', '!', '\0');
  NoteI2c_Arduino notei2c(Wire);

  // Action
  const char * const ACTUAL_RESULT = notei2c.receive(
    EXPECTED_ADDRESS,
    response_buffer,
    REQUEST_SIZE,
    &bytes_remaining
  );

  // Assert
  CHECK(ACTUAL_RESULT);
  CHECK(!memcmp(ACTUAL_RESULT, EXPECTED_RESULT, strlen(EXPECTED_RESULT)));
}

TEST_CASE("test_notei2c_arduino_receive_returns_error_message_on_i2c_protocol_unexpected_raw_byte_count", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  const uint8_t REQUEST_SIZE = 13;
  const char * EXPECTED_RESULT = "i2c|rx: unexpected raw byte count {io}{i2c}";
  uint8_t response_buffer[32];
  uint32_t bytes_remaining;

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_endTransmission);
  RESET_FAKE(TwoWire_requestFrom);
  TwoWire_requestFrom_fake.return_val = sizeof(response_buffer);
  RESET_FAKE(TwoWire_read);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_read, int, 0, REQUEST_SIZE, 'T', 'e', 's', 't', ' ', 'P', 'a', 's', 's', 'e', 'd', '!', '\0');
  NoteI2c_Arduino notei2c(Wire);

  // Action
  const char * const ACTUAL_RESULT = notei2c.receive(
    EXPECTED_ADDRESS,
    response_buffer,
    REQUEST_SIZE,
    &bytes_remaining
  );

  // Assert
  CHECK(ACTUAL_RESULT);
  CHECK(!memcmp(ACTUAL_RESULT, EXPECTED_RESULT, strlen(EXPECTED_RESULT)));
}

TEST_CASE("test_notei2c_arduino_receive_returns_error_message_on_serial_over_i2c_protocol_unexpected_available_byte_count", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint8_t AVAILABLE_SIZE = ((NoteI2c_Arduino::REQUEST_MAX_SIZE - NoteI2c_Arduino::REQUEST_HEADER_SIZE) + 1);
  const uint16_t EXPECTED_ADDRESS = 0x17;
  const uint8_t REQUEST_SIZE = 13;
  const char * EXPECTED_RESULT = "serial-over-i2c|rx: available byte count greater than max allowed {io}{i2c}";
  uint8_t response_buffer[32];
  uint32_t bytes_remaining;

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_endTransmission);
  RESET_FAKE(TwoWire_requestFrom);
  TwoWire_requestFrom_fake.return_val = (REQUEST_SIZE + NoteI2c::REQUEST_HEADER_SIZE);
  RESET_FAKE(TwoWire_read);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_read, int, AVAILABLE_SIZE, AVAILABLE_SIZE, AVAILABLE_SIZE, AVAILABLE_SIZE, REQUEST_SIZE, 'T', 'e', 's', 't', ' ', 'P', 'a', 's', 's', 'e', 'd', '!', '\0');
  NoteI2c_Arduino notei2c(Wire);

  // Action
  const char * const ACTUAL_RESULT = notei2c.receive(
    EXPECTED_ADDRESS,
    response_buffer,
    REQUEST_SIZE,
    &bytes_remaining
  );

  // Assert
  CHECK(ACTUAL_RESULT);
  CHECK(!memcmp(ACTUAL_RESULT, EXPECTED_RESULT, strlen(EXPECTED_RESULT)));
}

TEST_CASE("test_notei2c_arduino_receive_returns_error_message_on_serial_over_i2c_protocol_unexpected_protocol_byte_count", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  const uint8_t REQUEST_SIZE = 13;
  const char * EXPECTED_RESULT = "serial-over-i2c|rx: unexpected protocol byte count {io}{i2c}";
  uint8_t response_buffer[32];
  uint32_t bytes_remaining;

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_endTransmission);
  RESET_FAKE(TwoWire_requestFrom);
  TwoWire_requestFrom_fake.return_val = (REQUEST_SIZE + NoteI2c::REQUEST_HEADER_SIZE);
  RESET_FAKE(TwoWire_read);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_read, int, 5, 8, 'T', 'e', 's', 't', ' ', 'P', 'a', 's');
  NoteI2c_Arduino notei2c(Wire);

  // Action
  const char * const ACTUAL_RESULT = notei2c.receive(
    EXPECTED_ADDRESS,
    response_buffer,
    REQUEST_SIZE,
    &bytes_remaining
  );

  // Assert
  CHECK(ACTUAL_RESULT);
  CHECK(!memcmp(ACTUAL_RESULT, EXPECTED_RESULT, strlen(EXPECTED_RESULT)));
}

TEST_CASE("test_notei2c_arduino_reset_invokes_begin_method_on_constructor_twowire_parameter", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t DEVICE_ADDRESS = 0x17;
  NoteI2c_Arduino notei2c(Wire);
  RESET_FAKE(TwoWire_begin);

  // Action
  notei2c.reset(DEVICE_ADDRESS);

  // Assert
  CHECK(TwoWire_begin_fake.call_count);
}

TEST_CASE("test_notei2c_arduino_reset_returns_true", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t DEVICE_ADDRESS = 0x17;
  NoteI2c_Arduino notei2c(Wire);
  RESET_FAKE(TwoWire_begin);

  // Action
  const bool ACTUAL_RESULT = notei2c.reset(DEVICE_ADDRESS);

  // Assert
  CHECK(ACTUAL_RESULT);
}

#if not defined(WIRE_HAS_END)

TEST_CASE("test_notei2c_arduino_reset_does_not_invoke_end_method_on_constructor_twowire_parameter_ifndef_WIRE_HAS_END", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t DEVICE_ADDRESS = 0x17;
  NoteI2c_Arduino notei2c(Wire);
  RESET_FAKE(TwoWire_begin);
  RESET_FAKE(TwoWire_end);

  // Action
  notei2c.reset(DEVICE_ADDRESS);

  // Assert
  CHECK(!TwoWire_end_fake.call_count);
}

#else // defined(WIRE_HAS_END)

TEST_CASE("test_notei2c_arduino_reset_invokes_end_method_on_constructor_twowire_parameter_ifdef_WIRE_HAS_END", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t DEVICE_ADDRESS = 0x17;
  NoteI2c_Arduino notei2c(Wire);
  RESET_FAKE(TwoWire_begin);
  RESET_FAKE(TwoWire_end);

  // Action
  notei2c.reset(DEVICE_ADDRESS);

  // Assert
  CHECK(TwoWire_end_fake.call_count);
}

#endif // not defined(WIRE_HAS_END)

TEST_CASE("test_notei2c_arduino_transmit_translates_parameters_for_arduino_two_wire", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  uint8_t write_buffer[17] = {'H','e','l','l','o',',',' ','N','o','t','e','c','a','r','d','!','\0'};
  const uint8_t REQUEST_SIZE = sizeof(write_buffer);

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  TwoWire_write_byte_fake.return_val = 1;
  RESET_FAKE(TwoWire_write_buffer);
  TwoWire_write_buffer_fake.return_val = REQUEST_SIZE;
  RESET_FAKE(TwoWire_endTransmission);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_endTransmission, uint8_t, 0);
  NoteI2c_Arduino notei2c(Wire);

  // Action
  notei2c.transmit(
    EXPECTED_ADDRESS,
    write_buffer,
    REQUEST_SIZE
  );

  // Assert
  CHECK(TwoWire_beginTransmission_fake.arg0_val == EXPECTED_ADDRESS);
  CHECK(TwoWire_write_byte_fake.arg0_history[0] == REQUEST_SIZE);
  CHECK(!memcmp(TwoWire_write_buffer_fake.arg0_val, write_buffer, REQUEST_SIZE));
  CHECK(TwoWire_endTransmission_fake.call_count);
}

TEST_CASE("test_notei2c_arduino_transmit_returns_nullptr_on_success", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  uint8_t write_buffer[17] = {'H','e','l','l','o',',',' ','N','o','t','e','c','a','r','d','!','\0'};
  const uint8_t REQUEST_SIZE = sizeof(write_buffer);

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  TwoWire_write_byte_fake.return_val = 1;
  RESET_FAKE(TwoWire_write_buffer);
  TwoWire_write_buffer_fake.return_val = REQUEST_SIZE;
  RESET_FAKE(TwoWire_endTransmission);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_endTransmission, uint8_t, 0);
  NoteI2c_Arduino notei2c(Wire);

  // Action
  const char * const ACTUAL_RESULT = notei2c.transmit(
    EXPECTED_ADDRESS,
    write_buffer,
    REQUEST_SIZE
  );

  // Assert
  CHECK(ACTUAL_RESULT == nullptr);
}

TEST_CASE("test_notei2c_arduino_transmit_returns_error_message_on_i2c_transmission_failure1", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  uint8_t write_buffer[17] = {'H','e','l','l','o',',',' ','N','o','t','e','c','a','r','d','!','\0'};
  const uint8_t REQUEST_SIZE = sizeof(write_buffer);
  const char * EXPECTED_RESULT = "i2c|tx: data too long to fit in transmit buffer {io}{i2c}";

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_write_buffer);
  RESET_FAKE(TwoWire_endTransmission);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_endTransmission, uint8_t, 1);
  NoteI2c_Arduino notei2c(Wire);

  // Action
  const char * const ACTUAL_RESULT = notei2c.transmit(
    EXPECTED_ADDRESS,
    write_buffer,
    REQUEST_SIZE
  );

  // Assert
  CHECK(ACTUAL_RESULT);
  CHECK(!memcmp(ACTUAL_RESULT, EXPECTED_RESULT, strlen(EXPECTED_RESULT)));
}

TEST_CASE("test_notei2c_arduino_transmit_returns_error_message_on_i2c_transmission_failure2", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  uint8_t write_buffer[17] = {'H','e','l','l','o',',',' ','N','o','t','e','c','a','r','d','!','\0'};
  const uint8_t REQUEST_SIZE = sizeof(write_buffer);
  const char * EXPECTED_RESULT = "i2c|tx: received NACK on transmit of address {io}{i2c}";

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_write_buffer);
  RESET_FAKE(TwoWire_endTransmission);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_endTransmission, uint8_t, 2);
  NoteI2c_Arduino notei2c(Wire);

  // Action
  const char * const ACTUAL_RESULT = notei2c.transmit(
    EXPECTED_ADDRESS,
    write_buffer,
    REQUEST_SIZE
  );

  // Assert
  CHECK(ACTUAL_RESULT);
  CHECK(!memcmp(ACTUAL_RESULT, EXPECTED_RESULT, strlen(EXPECTED_RESULT)));
}

TEST_CASE("test_notei2c_arduino_transmit_returns_error_message_on_i2c_transmission_failure3", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  uint8_t write_buffer[17] = {'H','e','l','l','o',',',' ','N','o','t','e','c','a','r','d','!','\0'};
  const uint8_t REQUEST_SIZE = sizeof(write_buffer);
  const char * EXPECTED_RESULT = "i2c|tx: received NACK on transmit of data {io}{i2c}";

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_write_buffer);
  RESET_FAKE(TwoWire_endTransmission);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_endTransmission, uint8_t, 3);
  NoteI2c_Arduino notei2c(Wire);

  // Action
  const char * const ACTUAL_RESULT = notei2c.transmit(
    EXPECTED_ADDRESS,
    write_buffer,
    REQUEST_SIZE
  );

  // Assert
  CHECK(ACTUAL_RESULT);
  CHECK(!memcmp(ACTUAL_RESULT, EXPECTED_RESULT, strlen(EXPECTED_RESULT)));
}

TEST_CASE("test_notei2c_arduino_transmit_returns_error_message_on_i2c_transmission_failure4", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  uint8_t write_buffer[17] = {'H','e','l','l','o',',',' ','N','o','t','e','c','a','r','d','!','\0'};
  const uint8_t REQUEST_SIZE = sizeof(write_buffer);
  const char * EXPECTED_RESULT = "i2c|tx: unknown error on TwoWire::endTransmission() {io}{i2c}";

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_write_buffer);
  RESET_FAKE(TwoWire_endTransmission);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_endTransmission, uint8_t, 4);
  NoteI2c_Arduino notei2c(Wire);

  // Action
  const char * const ACTUAL_RESULT = notei2c.transmit(
    EXPECTED_ADDRESS,
    write_buffer,
    REQUEST_SIZE
  );

  // Assert
  CHECK(ACTUAL_RESULT);
  CHECK(!memcmp(ACTUAL_RESULT, EXPECTED_RESULT, strlen(EXPECTED_RESULT)));
}

TEST_CASE("test_notei2c_arduino_transmit_returns_error_message_on_i2c_transmission_failure5", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  uint8_t write_buffer[17] = {'H','e','l','l','o',',',' ','N','o','t','e','c','a','r','d','!','\0'};
  const uint8_t REQUEST_SIZE = sizeof(write_buffer);
  const char * EXPECTED_RESULT = "i2c|tx: timeout {io}{i2c}";

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_write_buffer);
  RESET_FAKE(TwoWire_endTransmission);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_endTransmission, uint8_t, 5);
  NoteI2c_Arduino notei2c(Wire);

  // Action
  const char * const ACTUAL_RESULT = notei2c.transmit(
    EXPECTED_ADDRESS,
    write_buffer,
    REQUEST_SIZE
  );

  // Assert
  CHECK(ACTUAL_RESULT);
  CHECK(!memcmp(ACTUAL_RESULT, EXPECTED_RESULT, strlen(EXPECTED_RESULT)));
}

TEST_CASE("test_notei2c_arduino_transmit_returns_error_message_on_unexpected_i2c_transmission_failure", "[NoteI2c_Arduino]")
{
  resetFakes();
  // Arrange
  const uint16_t EXPECTED_ADDRESS = 0x17;
  uint8_t write_buffer[17] = {'H','e','l','l','o',',',' ','N','o','t','e','c','a','r','d','!','\0'};
  const uint8_t REQUEST_SIZE = sizeof(write_buffer);
  const char * EXPECTED_RESULT = "i2c|tx: unknown error encountered during I2C transmission {io}{i2c}";

  RESET_FAKE(TwoWire_beginTransmission);
  RESET_FAKE(TwoWire_write_byte);
  RESET_FAKE(TwoWire_write_buffer);
  RESET_FAKE(TwoWire_endTransmission);
  SET_RETURN_SEQ_THEN_ZERO(TwoWire_endTransmission, uint8_t, 6);
  NoteI2c_Arduino notei2c(Wire);

  // Action
  const char * const ACTUAL_RESULT = notei2c.transmit(
    EXPECTED_ADDRESS,
    write_buffer,
    REQUEST_SIZE
  );

  // Assert
  CHECK(ACTUAL_RESULT);
  CHECK(!memcmp(ACTUAL_RESULT, EXPECTED_RESULT, strlen(EXPECTED_RESULT)));
}
