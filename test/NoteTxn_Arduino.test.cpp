#include "NoteTxn_Arduino.hpp"
#include "TestFunction.hpp"
#include "mock/mock-arduino.hpp"
#include "mock/mock-parameters.hpp"

#include <cassert>

// Compile command: g++ -std=c++11 -Wall -Wextra -Wpedantic mock/mock-arduino.cpp mock/mock-note-c-note.c ../src/NoteTxn_Arduino.cpp NoteTxn_Arduino.test.cpp -I. -I../src -DNOTE_MOCK -ggdb -O0 -o noteTxn_arduino.tests && ./noteTxn_arduino.tests || echo "Tests Result: $?"

int test_make_note_txn_instantiates_notetxn_object()
{
  int result;

  // Arrange
  NoteTxn * notetxn = nullptr;
  uint8_t txn_pins[2] = {19, 79};

  // Action
  notetxn = make_note_txn(txn_pins);

  // Assert
  if (nullptr != notetxn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('t' + 'x' + 'n');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnotetxn == " << !!notetxn << ", EXPECTED: not nullptr" << std::endl;
    std::cout << "[";
  }

  // Clean-up
  uint8_t invalid_pins[2] = {0};
  make_note_txn(invalid_pins);

  return result;
}

int test_make_note_txn_enforces_singleton_by_returning_same_notetxn_object_for_all_calls()
{
  int result;

  // Arrange
  uint8_t txn_pins_1[2] = {19, 79};
  NoteTxn * const notetxn_1 = make_note_txn(txn_pins_1);

  // Action
  uint8_t txn_pins_2[2] = {9, 17};
  NoteTxn * const notetxn_2 = make_note_txn(txn_pins_2);

  // Assert
  if (notetxn_1 == notetxn_2)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('t' + 'x' + 'n');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnotetxn_2 == " << std::hex << notetxn_2 << ", EXPECTED: " << notetxn_1 << std::endl;
    std::cout << "[";
  }

  // Clean-up
  uint8_t invalid_pins[2] = {0};
  make_note_txn(invalid_pins);

  return result;
}

//int test_make_note_txn_returns_nullptr_when_same_pins_are_passed_as_parameter()
int test_make_note_txn_deletes_singleton_when_same_pins_are_passed_as_parameter()
{
  int result;

  // Arrange
  uint8_t txn_pins[2] = {19, 79};
  NoteTxn * notetxn = make_note_txn(txn_pins);
  assert(notetxn);

  // Action
  uint8_t invalid_pins[2] = {0};
  notetxn = make_note_txn(invalid_pins);

  // Assert
  if (nullptr == notetxn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('t' + 'x' + 'n');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnotetxn == " << std::hex << notetxn << ", EXPECTED: 0 (nullptr)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notetxn_arduino_constructor_floats_ctx_pin()
{
  int result;

  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;
  pinMode_Parameters.reset();

  // Action
  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);

  // Assert
  if (pinMode_Parameters.pin_mode[CTX_PIN][(pinMode_Parameters.invoked[CTX_PIN] - 1)] == INPUT)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('t' + 'x' + 'n');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tpinMode_Parameters.pin_mode[CTX_PIN][(pinMode_Parameters.invoked[CTX_PIN] - 1)] == 0x" << std::hex << static_cast<size_t>(pinMode_Parameters.pin_mode[CTX_PIN][(pinMode_Parameters.invoked[CTX_PIN] - 1)]) << ", EXPECTED: 0x" << std::hex << INPUT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notetxn_arduino_constructor_floats_rtx_pin()
{
  int result;

  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;
  pinMode_Parameters.reset();

  // Action
  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);

  // Assert
  if (pinMode_Parameters.pin_mode[RTX_PIN][(pinMode_Parameters.invoked[RTX_PIN] - 1)] == INPUT)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('t' + 'x' + 'n');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tpinMode_Parameters.pin_mode[RTX_PIN][(pinMode_Parameters.invoked[RTX_PIN] - 1)] == 0x" << std::hex << static_cast<size_t>(pinMode_Parameters.pin_mode[RTX_PIN][(pinMode_Parameters.invoked[RTX_PIN] - 1)]) << ", EXPECTED: 0x" << std::hex << INPUT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notetxn_arduino_start_initially_configures_ctx_pin_as_input_pullup()
{
  int result;

  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;
  const size_t TIMEOUT_MS = 917;

  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);
  digitalRead_Parameters.reset();
  digitalRead_Parameters.default_result[CTX_PIN] = HIGH;
  pinMode_Parameters.reset();

  // Action
  notetxn.start(TIMEOUT_MS);

  // Assert
  if (
      pinMode_Parameters.pin_mode.find(CTX_PIN) != pinMode_Parameters.pin_mode.end()
   && pinMode_Parameters.pin_mode[CTX_PIN][0] == INPUT_PULLUP
  ) {
    result = 0;
  }
  else
  {
    result = static_cast<int>('t' + 'x' + 'n');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    if (pinMode_Parameters.pin_mode.find(CTX_PIN) != pinMode_Parameters.pin_mode.end()) {
      std::cout << "\tpinMode_Parameters.pin_mode[CTX_PIN][0] == 0x" << std::hex << static_cast<unsigned>(pinMode_Parameters.pin_mode[CTX_PIN][0]) << ", EXPECTED: 0x" << std::hex << static_cast<unsigned>(INPUT_PULLUP) << std::endl;
    } else {
      std::cout << "\tpinMode_Parameters.pin_mode[CTX_PIN] does not exist!" << std::endl;
    }
    std::cout << "[";
  }

  return result;
}

int test_notetxn_arduino_start_configures_rtx_pin_as_output()
{
  int result;

  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;
  const size_t TIMEOUT_MS = 917;

  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);
  digitalRead_Parameters.reset();
  digitalRead_Parameters.default_result[CTX_PIN] = HIGH;
  pinMode_Parameters.reset();

  // Action
  notetxn.start(TIMEOUT_MS);

  // Assert
  if (
      pinMode_Parameters.pin_mode.find(RTX_PIN) != pinMode_Parameters.pin_mode.end()
   && pinMode_Parameters.pin_mode[RTX_PIN][(pinMode_Parameters.invoked[RTX_PIN] - 1)] == OUTPUT
  ) {
    result = 0;
  }
  else
  {
    result = static_cast<int>('t' + 'x' + 'n');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    if (pinMode_Parameters.pin_mode.find(RTX_PIN) != pinMode_Parameters.pin_mode.end()) {
      std::cout << "\tpinMode_Parameters.pin_mode[RTX_PIN][(pinMode_Parameters.invoked[RTX_PIN] - 1)] == 0x" << std::hex << static_cast<unsigned>(pinMode_Parameters.pin_mode[RTX_PIN][(pinMode_Parameters.invoked[RTX_PIN] - 1)]) << ", EXPECTED: 0x" << std::hex << static_cast<unsigned>(OUTPUT) << std::endl;
    } else {
      std::cout << "\tpinMode_Parameters.pin_mode[RTX_PIN] does not exist!" << std::endl;
    }
    std::cout << "[";
  }

  return result;
}

int test_notetxn_arduino_start_sets_rtx_pin_high()
{
  int result;

  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;
  const size_t TIMEOUT_MS = 917;

  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);
  digitalWrite_Parameters.reset();
  digitalRead_Parameters.reset();
  digitalRead_Parameters.default_result[CTX_PIN] = HIGH;

  // Action
  notetxn.start(TIMEOUT_MS);

  // Assert
  if (
      digitalWrite_Parameters.pin_val.find(RTX_PIN) != digitalWrite_Parameters.pin_val.end()
   && digitalWrite_Parameters.pin_val[RTX_PIN][(digitalWrite_Parameters.invoked[RTX_PIN] - 1)] == HIGH
  ) {
    result = 0;
  }
  else
  {
    result = static_cast<int>('t' + 'x' + 'n');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    if (digitalWrite_Parameters.pin_val.find(RTX_PIN) != digitalWrite_Parameters.pin_val.end()) {
      std::cout << "\tdigitalWrite_Parameters.pin_val[RTX_PIN][(digitalWrite_Parameters.invoked[RTX_PIN] - 1)] == 0x" << std::hex << static_cast<unsigned>(digitalWrite_Parameters.pin_val[RTX_PIN][(digitalWrite_Parameters.invoked[RTX_PIN] - 1)]) << ", EXPECTED: 0x" << std::hex << static_cast<unsigned>(HIGH) << std::endl;
    } else {
      std::cout << "\tdigitalWrite_Parameters.pin_val[RTX_PIN] does not exist!" << std::endl;
    }
    std::cout << "[";
  }

  return result;
}

int test_notetxn_arduino_start_blocks_until_ctx_pin_goes_high()
{
  int result;

  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;
  const size_t TIMEOUT_MS = 917;

  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);
  digitalRead_Parameters.reset();
  digitalRead_Parameters.result[CTX_PIN].push_back(LOW);
  digitalRead_Parameters.result[CTX_PIN].push_back(LOW);
  digitalRead_Parameters.result[CTX_PIN].push_back(LOW);
  digitalRead_Parameters.result[CTX_PIN].push_back(HIGH);
  noteGetMs_Parameters.reset();
  noteGetMs_Parameters.default_result = (TIMEOUT_MS - 1);

  // Action
  notetxn.start(TIMEOUT_MS);

  // Assert
  if (
      digitalRead_Parameters.invoked[CTX_PIN] == digitalRead_Parameters.result[CTX_PIN].size()
   && digitalRead_Parameters.result[CTX_PIN][(digitalRead_Parameters.invoked[CTX_PIN] - 1)] == HIGH
   && noteGetMs_Parameters.result.empty()  // Only returns value less than `TIMEOUT_MS`
  ) {
    result = 0;
  }
  else
  {
    result = static_cast<int>('t' + 'x' + 'n');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    if (digitalRead_Parameters.result.find(CTX_PIN) != digitalRead_Parameters.result.end()) {
      std::cout << "\tdigitalRead_Parameters.invoked[CTX_PIN] == " << std::dec << static_cast<unsigned>(digitalRead_Parameters.invoked[CTX_PIN]) << ", EXPECTED: " << std::dec << static_cast<unsigned>(digitalRead_Parameters.result[CTX_PIN].size()) << std::endl;
      std::cout << "\tdigitalRead_Parameters.result[CTX_PIN][(digitalRead_Parameters.invoked[CTX_PIN] - 1)] == 0x" << std::hex << static_cast<unsigned>(digitalRead_Parameters.result[CTX_PIN][(digitalRead_Parameters.invoked[CTX_PIN] - 1)]) << ", EXPECTED: 0x" << std::hex << static_cast<unsigned>(HIGH) << std::endl;
    } else {
      std::cout << "\tdigitalRead_Parameters.result[CTX_PIN] does not exist!" << std::endl;
    }
    std::cout << "[";
  }

  return result;
}

int test_notetxn_arduino_start_blocks_until_timeout_ms()
{
  int result;

  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;
  const size_t TIMEOUT_MS = 917;

  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);
  digitalRead_Parameters.reset();
  digitalRead_Parameters.default_result[CTX_PIN] = LOW;
  noteDelayMs_Parameters.reset();
  noteGetMs_Parameters.reset();
  noteGetMs_Parameters.result.push_back(36);
  noteGetMs_Parameters.result.push_back(274);
  noteGetMs_Parameters.result.push_back(515);
  noteGetMs_Parameters.result.push_back(801);
  noteGetMs_Parameters.result.push_back(TIMEOUT_MS + noteGetMs_Parameters.result[0]);

  // Action
  notetxn.start(TIMEOUT_MS);

  // Assert
  if (
      digitalRead_Parameters.invoked[CTX_PIN] // Called at least once
   && digitalRead_Parameters.result[CTX_PIN].empty() // Only returns `LOW`
   && noteGetMs_Parameters.invoked == noteGetMs_Parameters.result.size()
  ) {
    result = 0;
  }
  else
  {
    result = static_cast<int>('t' + 'x' + 'n');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tdigitalRead_Parameters.invoked[CTX_PIN] == " << std::dec << static_cast<unsigned>(digitalRead_Parameters.invoked[CTX_PIN]) << ", EXPECTED: > 0" << std::endl;
    std::cout << "\tdigitalRead_Parameters.result[CTX_PIN].size() == " << std::dec << static_cast<unsigned>(digitalRead_Parameters.result[CTX_PIN].size()) << ", EXPECTED: 0" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notetxn_arduino_start_leaves_rtx_pin_as_output_when_ctx_responds_high()
{
  int result;

  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;
  const size_t TIMEOUT_MS = 917;

  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);
  digitalRead_Parameters.reset();
  digitalRead_Parameters.default_result[CTX_PIN] = HIGH;
  millis_Parameters.reset();
  pinMode_Parameters.reset();

  // Action
  notetxn.start(TIMEOUT_MS);

  // Assert
  if (
      digitalRead_Parameters.invoked[CTX_PIN] // Called at least once
   && digitalRead_Parameters.result[CTX_PIN].empty() // Only returns `LOW`
   && pinMode_Parameters.pin_mode[RTX_PIN][(pinMode_Parameters.invoked[RTX_PIN] - 1)] == OUTPUT
  ) {
    result = 0;
  }
  else
  {
    result = static_cast<int>('t' + 'x' + 'n');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tdigitalRead_Parameters.invoked[CTX_PIN] == " << std::dec << static_cast<unsigned>(digitalRead_Parameters.invoked[CTX_PIN]) << ", EXPECTED: > 0" << std::endl;
    std::cout << "\tdigitalRead_Parameters.result[CTX_PIN].size() == " << std::dec << static_cast<unsigned>(digitalRead_Parameters.result[CTX_PIN].size()) << ", EXPECTED: 0" << std::endl;
    std::cout << "\tpinMode_Parameters.pin_mode[RTX_PIN][(pinMode_Parameters.invoked[RTX_PIN] - 1)] == 0x" << std::hex << static_cast<unsigned>(pinMode_Parameters.pin_mode[RTX_PIN][(pinMode_Parameters.invoked[RTX_PIN] - 1)]) << ", EXPECTED: 0x" << std::hex << static_cast<unsigned>(OUTPUT) << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notetxn_arduino_start_leaves_rtx_pin_high_when_ctx_responds_high()
{
  int result;

  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;
  const size_t TIMEOUT_MS = 917;

  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);
  digitalRead_Parameters.reset();
  digitalRead_Parameters.default_result[CTX_PIN] = HIGH;
  digitalWrite_Parameters.reset();
  noteGetMs_Parameters.reset();

  // Action
  notetxn.start(TIMEOUT_MS);

  // Assert
  if (
      digitalRead_Parameters.invoked[CTX_PIN] // Called at least once
   && digitalRead_Parameters.result[CTX_PIN].empty() // Only returns `LOW`
   && digitalWrite_Parameters.pin_val[RTX_PIN][(digitalWrite_Parameters.invoked[RTX_PIN] - 1)] == HIGH
  ) {
    result = 0;
  }
  else
  {
    result = static_cast<int>('t' + 'x' + 'n');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tdigitalRead_Parameters.invoked[CTX_PIN] == " << std::dec << static_cast<unsigned>(digitalRead_Parameters.invoked[CTX_PIN]) << ", EXPECTED: > 0" << std::endl;
    std::cout << "\tdigitalRead_Parameters.result[CTX_PIN].size() == " << std::dec << static_cast<unsigned>(digitalRead_Parameters.result[CTX_PIN].size()) << ", EXPECTED: 0" << std::endl;
    std::cout << "\tdigitalWrite_Parameters.pin_val[RTX_PIN][(digitalWrite_Parameters.invoked[RTX_PIN] - 1)] == 0x" << std::hex << static_cast<unsigned>(digitalWrite_Parameters.pin_val[RTX_PIN][(digitalWrite_Parameters.invoked[RTX_PIN] - 1)]) << ", EXPECTED: 0x" << std::hex << static_cast<unsigned>(HIGH) << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notetxn_arduino_start_leaves_ctx_pin_floating_when_ctx_responds_high()
{
  int result;

  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;
  const size_t TIMEOUT_MS = 917;

  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);
  digitalRead_Parameters.reset();
  digitalRead_Parameters.default_result[CTX_PIN] = HIGH;
  pinMode_Parameters.reset();

  // Action
  notetxn.start(TIMEOUT_MS);

  // Assert
  if (
      pinMode_Parameters.invoked[CTX_PIN] > 1 // Called at least twice
   && pinMode_Parameters.pin_mode[CTX_PIN][(pinMode_Parameters.invoked[CTX_PIN] - 1)] == INPUT
  ) {
    result = 0;
  }
  else
  {
    result = static_cast<int>('t' + 'x' + 'n');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tpinMode_Parameters.invoked[CTX_PIN] == " << std::dec << static_cast<unsigned>(pinMode_Parameters.invoked[CTX_PIN]) << ", EXPECTED: > 1" << std::endl;
    std::cout << "\tpinMode_Parameters.pin_mode[CTX_PIN][(pinMode_Parameters.invoked[CTX_PIN] - 1)] == 0x" << std::hex << static_cast<unsigned>(pinMode_Parameters.pin_mode[CTX_PIN][(pinMode_Parameters.invoked[CTX_PIN] - 1)]) << ", EXPECTED: 0x" << std::hex << static_cast<unsigned>(INPUT) << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notetxn_arduino_start_leaves_ctx_pin_floating_on_timeout()
{
  int result;

  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;
  const size_t TIMEOUT_MS = 917;

  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);
  digitalRead_Parameters.reset();
  digitalRead_Parameters.default_result[CTX_PIN] = LOW;
  noteGetMs_Parameters.reset();
  noteGetMs_Parameters.result.push_back(36);
  noteGetMs_Parameters.result.push_back(274);
  noteGetMs_Parameters.result.push_back(515);
  noteGetMs_Parameters.result.push_back(801);
  noteGetMs_Parameters.result.push_back(TIMEOUT_MS + noteGetMs_Parameters.result[0]);
  pinMode_Parameters.reset();

  // Action
  notetxn.start(TIMEOUT_MS);

  // Assert
  if (
      digitalRead_Parameters.invoked[CTX_PIN] // Called at least once
   && digitalRead_Parameters.result[CTX_PIN].empty() // Only returns `LOW`
   && noteGetMs_Parameters.invoked == noteGetMs_Parameters.result.size()
   && pinMode_Parameters.invoked[CTX_PIN] > 1 // Called at least twice
   && pinMode_Parameters.pin_mode[CTX_PIN][(pinMode_Parameters.invoked[CTX_PIN] - 1)] == INPUT
  ) {
    result = 0;
  }
  else
  {
    result = static_cast<int>('t' + 'x' + 'n');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tdigitalRead_Parameters.invoked[CTX_PIN] == " << std::dec << static_cast<unsigned>(digitalRead_Parameters.invoked[CTX_PIN]) << ", EXPECTED: > 0" << std::endl;
    std::cout << "\tdigitalRead_Parameters.result[CTX_PIN].size() == " << std::dec << static_cast<unsigned>(digitalRead_Parameters.result[CTX_PIN].size()) << ", EXPECTED: 0" << std::endl;
    std::cout << "\tpinMode_Parameters.pin_mode[CTX_PIN][(pinMode_Parameters.invoked[CTX_PIN] - 1)] == 0x" << std::hex << static_cast<unsigned>(pinMode_Parameters.pin_mode[CTX_PIN][(pinMode_Parameters.invoked[CTX_PIN] - 1)]) << ", EXPECTED: 0x" << std::hex << static_cast<unsigned>(INPUT) << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notetxn_arduino_start_floats_rtx_pin_on_timeout()
{
  int result;

  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;
  const size_t TIMEOUT_MS = 917;

  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);
  digitalRead_Parameters.reset();
  digitalRead_Parameters.default_result[CTX_PIN] = LOW;
  noteGetMs_Parameters.reset();
  noteGetMs_Parameters.result.push_back(36);
  noteGetMs_Parameters.result.push_back(274);
  noteGetMs_Parameters.result.push_back(515);
  noteGetMs_Parameters.result.push_back(801);
  noteGetMs_Parameters.result.push_back(TIMEOUT_MS + noteGetMs_Parameters.result[0]);
  pinMode_Parameters.reset();

  // Action
  notetxn.start(TIMEOUT_MS);

  // Assert
  if (
      digitalRead_Parameters.invoked[CTX_PIN] // Called at least once
   && digitalRead_Parameters.result[CTX_PIN].empty() // Only returns `LOW`
   && noteGetMs_Parameters.invoked == noteGetMs_Parameters.result.size()
   && pinMode_Parameters.pin_mode[RTX_PIN][(pinMode_Parameters.invoked[RTX_PIN] - 1)] == INPUT
  ) {
    result = 0;
  }
  else
  {
    result = static_cast<int>('t' + 'x' + 'n');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tdigitalRead_Parameters.invoked[CTX_PIN] == " << std::dec << static_cast<unsigned>(digitalRead_Parameters.invoked[CTX_PIN]) << ", EXPECTED: > 0" << std::endl;
    std::cout << "\tdigitalRead_Parameters.result[CTX_PIN].size() == " << std::dec << static_cast<unsigned>(digitalRead_Parameters.result[CTX_PIN].size()) << ", EXPECTED: 0" << std::endl;
    std::cout << "\tpinMode_Parameters.pin_mode[RTX_PIN][(pinMode_Parameters.invoked[RTX_PIN] - 1)] == 0x" << std::hex << static_cast<unsigned>(pinMode_Parameters.pin_mode[RTX_PIN][(pinMode_Parameters.invoked[RTX_PIN] - 1)]) << ", EXPECTED: 0x" << std::hex << static_cast<unsigned>(INPUT) << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notetxn_arduino_stop_floats_rtx_pin()
{
  int result;

  // Arrange
  const uint8_t CTX_PIN = 19;
  const uint8_t RTX_PIN = 79;

  NoteTxn_Arduino notetxn(CTX_PIN, RTX_PIN);
  digitalWrite_Parameters.reset();
  pinMode_Parameters.reset();

  // Action
  notetxn.stop();

  // Assert
  if (
      pinMode_Parameters.invoked[RTX_PIN]
   && pinMode_Parameters.pin_mode[RTX_PIN][(pinMode_Parameters.invoked[RTX_PIN] - 1)] == INPUT
  ) {
    result = 0;
  }
  else
  {
    result = static_cast<int>('t' + 'x' + 'n');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    if (pinMode_Parameters.pin_mode.find(RTX_PIN) != pinMode_Parameters.pin_mode.end()) {
      std::cout << "\tpinMode_Parameters.invoked[RTX_PIN] == " << std::dec << static_cast<unsigned>(pinMode_Parameters.invoked[RTX_PIN]) << ", EXPECTED: > 0" << std::endl;
      std::cout << "\tpinMode_Parameters.pin_mode[RTX_PIN][(pinMode_Parameters.invoked[RTX_PIN] - 1)] == 0x" << std::hex << static_cast<unsigned>(pinMode_Parameters.pin_mode[RTX_PIN][(pinMode_Parameters.invoked[RTX_PIN] - 1)]) << ", EXPECTED: 0x" << std::hex << static_cast<unsigned>(INPUT) << std::endl;
    } else {
      std::cout << "\tpinMode_Parameters.pin_val[RTX_PIN] does not exist!" << std::endl;
    }
    std::cout << "[";
  }

  return result;
}

int main(void)
{
  TestFunction tests[] = {
      {test_make_note_txn_instantiates_notetxn_object, "test_make_note_txn_instantiates_notetxn_object"},
      {test_make_note_txn_enforces_singleton_by_returning_same_notetxn_object_for_all_calls, "test_make_note_txn_enforces_singleton_by_returning_same_notetxn_object_for_all_calls"},
      {test_make_note_txn_deletes_singleton_when_same_pins_are_passed_as_parameter, "test_make_note_txn_deletes_singleton_when_same_pins_are_passed_as_parameter"},
      {test_notetxn_arduino_constructor_floats_ctx_pin, "test_notetxn_arduino_constructor_floats_ctx_pin"},
      {test_notetxn_arduino_constructor_floats_rtx_pin, "test_notetxn_arduino_constructor_floats_rtx_pin"},
      {test_notetxn_arduino_start_initially_configures_ctx_pin_as_input_pullup, "test_notetxn_arduino_start_initially_configures_ctx_pin_as_input_pullup"},
      {test_notetxn_arduino_start_configures_rtx_pin_as_output, "test_notetxn_arduino_start_configures_rtx_pin_as_output"},
      {test_notetxn_arduino_start_sets_rtx_pin_high, "test_notetxn_arduino_start_sets_rtx_pin_high"},
      {test_notetxn_arduino_start_blocks_until_ctx_pin_goes_high, "test_notetxn_arduino_start_blocks_until_ctx_pin_goes_high"},
      {test_notetxn_arduino_start_blocks_until_timeout_ms, "test_notetxn_arduino_start_blocks_until_timeout_ms"},
      {test_notetxn_arduino_start_leaves_ctx_pin_floating_when_ctx_responds_high, "test_notetxn_arduino_start_leaves_ctx_pin_floating_when_ctx_responds_high"},
      {test_notetxn_arduino_start_leaves_rtx_pin_as_output_when_ctx_responds_high, "test_notetxn_arduino_start_leaves_rtx_pin_as_output_when_ctx_responds_high"},
      {test_notetxn_arduino_start_leaves_rtx_pin_high_when_ctx_responds_high, "test_notetxn_arduino_start_leaves_rtx_pin_high_when_ctx_responds_high"},
      {test_notetxn_arduino_start_leaves_ctx_pin_floating_on_timeout, "test_notetxn_arduino_start_leaves_ctx_pin_floating_on_timeout"},
      {test_notetxn_arduino_start_floats_rtx_pin_on_timeout, "test_notetxn_arduino_start_floats_rtx_pin_on_timeout"},
      {test_notetxn_arduino_stop_floats_rtx_pin, "test_notetxn_arduino_stop_floats_rtx_pin"},
  };

  return TestFunction::runTests(tests, (sizeof(tests) / sizeof(TestFunction)));
}
