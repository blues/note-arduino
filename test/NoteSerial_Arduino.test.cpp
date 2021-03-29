#include "NoteSerial_Arduino.hpp"
#include "TestFunction.hpp"
#include "mock/mock-arduino.hpp"
#include "mock/mock-parameters.hpp"

#include <string.h>

// Compile command: g++ -Wall -Wextra -Wpedantic mock/mock-arduino.cpp ../src/NoteSerial_Arduino.cpp NoteSerial_Arduino.test.cpp -std=c++14 -I. -I../src -DMOCK && ./a.out || echo "Tests Result: $?"

int test_noteserial_arduino_constructor_invokes_hardware_serial_begin()
{
  int result;

  // Setup
  hardwareSerialBegin_Parameters.baud = 0;

  // Action
  NoteSerial_Arduino noteserial(Serial,9600);

  // Evaluate Result
  if (0 != hardwareSerialBegin_Parameters.baud)
  {
    result = 0;
  }
  else
  {
    result = 1;
  }

  return result;
}

int test_noteserial_arduino_constructor_does_not_modify_baud_parameter_before_passing_to_hardware_serial_begin()
{
  int result;
  const size_t EXPECTED_BAUD_RATE = 9600;

  // Setup
  hardwareSerialBegin_Parameters.baud = 0;

  // Action
  NoteSerial_Arduino noteserial(Serial,EXPECTED_BAUD_RATE);

  // Evaluate Result
  if (EXPECTED_BAUD_RATE == hardwareSerialBegin_Parameters.baud)
  {
    result = 0;
  }
  else
  {
    result = 2;
  }

  return result;
}

int test_noteserial_arduino_available_invokes_hardware_serial_available()
{
  int result;
  NoteSerial_Arduino noteserial(Serial,9600);

  // Setup
  hardwareSerialAvailable_Parameters.invoked = 0;

  // Action
  noteserial.available();

  // Evaluate Result
  if (hardwareSerialAvailable_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = 3;
  }

  return result;
}

int test_noteserial_arduino_available_does_not_modify_hardware_serial_available_result_value_before_returning_to_caller()
{
  int result;
  NoteSerial_Arduino noteserial(Serial,9600);
  const size_t EXPECTED_RESULT = 79;

  // Setup
  hardwareSerialAvailable_Parameters.result = EXPECTED_RESULT;

  // Action
  const size_t ACTUAL_RESULT = noteserial.available();

  // Evaluate Result
  if (ACTUAL_RESULT == EXPECTED_RESULT)
  {
    result = 0;
  }
  else
  {
    result = 4;
  }

  return result;
}

int test_noteserial_arduino_receive_invokes_hardware_serial_read()
{
  int result;
  NoteSerial_Arduino noteserial(Serial,9600);

  // Setup
  hardwareSerialRead_Parameters.invoked = 0;

  // Action
  noteserial.receive();

  // Evaluate Result
  if (hardwareSerialRead_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = 5;
  }

  return result;
}

int test_noteserial_arduino_receive_does_not_modify_hardware_serial_read_result_value_before_returning_to_caller()
{
  int result;
  const char EXPECTED_RESULT = 'z';
  NoteSerial_Arduino noteserial(Serial,9600);

  // Setup
  hardwareSerialRead_Parameters.result = EXPECTED_RESULT;

  // Action
  const char ACTUAL_RESULT = noteserial.receive();

  // Evaluate Result
  if (EXPECTED_RESULT == ACTUAL_RESULT)
  {
    result = 0;
  }
  else
  {
    result = 6;
  }

  return result;
}

int test_noteserial_arduino_reset_invokes_hardware_serial_begin()
{
  int result;
  NoteSerial_Arduino noteserial(Serial,9600);

  // Setup
  hardwareSerialBegin_Parameters.baud = 0;

  // Action
  noteserial.reset();

  // Evaluate Result
  if (0 != hardwareSerialBegin_Parameters.baud)
  {
    result = 0;
  }
  else
  {
    result = 7;
  }

  return result;
}

int test_noteserial_arduino_reset_invokes_hardware_serial_begin_with_the_baud_parameter_that_was_originally_supplied_to_the_constructor()
{
  int result;
  const size_t EXPECTED_BAUD_RATE = 9600;
  NoteSerial_Arduino noteserial(Serial,EXPECTED_BAUD_RATE);

  // Setup
  hardwareSerialBegin_Parameters.baud = 0;

  // Action
  noteserial.reset();

  // Evaluate Result
  if (EXPECTED_BAUD_RATE == hardwareSerialBegin_Parameters.baud)
  {
    result = 0;
  }
  else
  {
    result = 8;
  }

  return result;
}

int test_noteserial_arduino_reset_always_returns_true()
{
  int result;
  NoteSerial_Arduino noteserial(Serial,9600);

  // Setup

  // Action
  const char ACTUAL_RESULT = noteserial.reset();

  // Evaluate Result
  if (true == ACTUAL_RESULT)
  {
    result = 0;
  }
  else
  {
    result = 9;
  }

  return result;
}

int test_noteserial_arduino_transmit_invokes_hardware_serial_write()
{
  int result;
  NoteSerial_Arduino noteserial(Serial,9600);

  // Setup
  hardwareSerialWrite_Parameters.buffer = nullptr;
  hardwareSerialWrite_Parameters.size = 0;

  // Action
  noteserial.transmit(reinterpret_cast<uint8_t *>(const_cast<char *>("Hello, Test 10!")), 16, true);

  // Evaluate Result
  if (nullptr != hardwareSerialWrite_Parameters.buffer)
  {
    result = 0;
  }
  else
  {
    result = 10;
  }

  return result;
}

int test_noteserial_arduino_transmit_does_not_modify_buffer_parameter_value_before_passing_to_hardware_serial_write()
{
  int result;
  NoteSerial_Arduino noteserial(Serial,9600);
  const char EXPECTED_RESULT[] = "Hello, Test 11!";
  char str_copy[32];

  // Setup
  strcpy(str_copy, EXPECTED_RESULT);
  hardwareSerialWrite_Parameters.buffer = nullptr;
  hardwareSerialWrite_Parameters.size = 0;

  // Action
  noteserial.transmit(reinterpret_cast<uint8_t *>(const_cast<char *>(str_copy)), strlen(str_copy), true);

  // Evaluate Result
  if (0 == strcmp(EXPECTED_RESULT, const_cast<const char *>(reinterpret_cast<char *>(hardwareSerialWrite_Parameters.buffer))))
  {
    result = 0;
  }
  else
  {
    result = 11;
  }

  return result;
}

int test_noteserial_arduino_transmit_does_not_modify_size_parameter_value_before_passing_to_hardware_serial_write()
{
  int result;
  NoteSerial_Arduino noteserial(Serial,9600);
  const size_t EXPECTED_RESULT = 16;

  // Setup
  hardwareSerialWrite_Parameters.buffer = nullptr;
  hardwareSerialWrite_Parameters.size = 0;

  // Action
  noteserial.transmit(reinterpret_cast<uint8_t *>(const_cast<char *>("Hello, Test 12!")), EXPECTED_RESULT, true);

  // Evaluate Result
  if (EXPECTED_RESULT == hardwareSerialWrite_Parameters.size)
  {
    result = 0;
  }
  else
  {
    result = 12;
  }

  return result;
}

int test_noteserial_arduino_transmit_invokes_hardware_serial_flush_when_flush_parameter_is_true()
{
  int result;
  NoteSerial_Arduino noteserial(Serial,9600);

  // Setup
  hardwareSerialFlush_Parameters.invoked = 0;

  // Action
  noteserial.transmit(reinterpret_cast<uint8_t *>(const_cast<char *>("Hello, Test 13!")), 16, true);

  // Evaluate Result
  if (hardwareSerialFlush_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = 13;
  }

  return result;
}

int test_noteserial_arduino_transmit_does_not_invoke_hardware_serial_flush_when_flush_parameter_is_false()
{
  int result;
  NoteSerial_Arduino noteserial(Serial,9600);

  // Setup
  hardwareSerialFlush_Parameters.invoked = 0;

  // Action
  noteserial.transmit(reinterpret_cast<uint8_t *>(const_cast<char *>("Hello, Test 14!")), 16, false);

  // Evaluate Result
  if (!hardwareSerialFlush_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = 14;
  }

  return result;
}

int test_noteserial_arduino_transmit_does_not_modify_hardware_serial_write_result_value_before_returning_to_caller()
{
  int result;
  const size_t EXPECTED_RESULT = 16;

  // Setup
  hardwareSerialWrite_Parameters.result = EXPECTED_RESULT;
  NoteSerial_Arduino noteserial(Serial,9600);

  // Action
  const char ACTUAL_RESULT = noteserial.transmit(reinterpret_cast<uint8_t *>(const_cast<char *>("Hello, Test 15!")), 16, true);

  // Evaluate Result
  if (EXPECTED_RESULT == ACTUAL_RESULT)
  {
    result = 0;
  }
  else
  {
    result = 15;
  }

  return result;
}

int main(void)
{
  TestFunction tests[] = {
      {test_noteserial_arduino_constructor_invokes_hardware_serial_begin, "test_noteserial_arduino_constructor_invokes_hardware_serial_begin"},
      {test_noteserial_arduino_constructor_does_not_modify_baud_parameter_before_passing_to_hardware_serial_begin, "test_noteserial_arduino_constructor_does_not_modify_baud_parameter_before_passing_to_hardware_serial_begin"},
      {test_noteserial_arduino_available_invokes_hardware_serial_available, "test_noteserial_arduino_available_invokes_hardware_serial_available"},
      {test_noteserial_arduino_available_does_not_modify_hardware_serial_available_result_value_before_returning_to_caller, "test_noteserial_arduino_available_does_not_modify_hardware_serial_available_result_value_before_returning_to_caller"},
      {test_noteserial_arduino_receive_invokes_hardware_serial_read, "test_noteserial_arduino_receive_invokes_hardware_serial_read"},
      {test_noteserial_arduino_receive_does_not_modify_hardware_serial_read_result_value_before_returning_to_caller, "test_noteserial_arduino_receive_does_not_modify_hardware_serial_read_result_value_before_returning_to_caller"},
      {test_noteserial_arduino_reset_invokes_hardware_serial_begin, "test_noteserial_arduino_reset_invokes_hardware_serial_begin"},
      {test_noteserial_arduino_reset_invokes_hardware_serial_begin_with_the_baud_parameter_that_was_originally_supplied_to_the_constructor, "test_noteserial_arduino_reset_invokes_hardware_serial_begin_with_the_baud_parameter_that_was_originally_supplied_to_the_constructor"},
      {test_noteserial_arduino_reset_always_returns_true, "test_noteserial_arduino_reset_always_returns_true"},
      {test_noteserial_arduino_transmit_invokes_hardware_serial_write, "test_noteserial_arduino_transmit_invokes_hardware_serial_write"},
      {test_noteserial_arduino_transmit_does_not_modify_buffer_parameter_value_before_passing_to_hardware_serial_write, "test_noteserial_arduino_transmit_does_not_modify_buffer_parameter_value_before_passing_to_hardware_serial_write"},
      {test_noteserial_arduino_transmit_does_not_modify_size_parameter_value_before_passing_to_hardware_serial_write, "test_noteserial_arduino_transmit_does_not_modify_size_parameter_value_before_passing_to_hardware_serial_write"},
      {test_noteserial_arduino_transmit_invokes_hardware_serial_flush_when_flush_parameter_is_true, "test_noteserial_arduino_transmit_invokes_hardware_serial_flush_when_flush_parameter_is_true"},
      {test_noteserial_arduino_transmit_does_not_invoke_hardware_serial_flush_when_flush_parameter_is_false, "test_noteserial_arduino_transmit_does_not_invoke_hardware_serial_flush_when_flush_parameter_is_false"},
      {test_noteserial_arduino_transmit_does_not_modify_hardware_serial_write_result_value_before_returning_to_caller, "test_noteserial_arduino_transmit_does_not_modify_hardware_serial_write_result_value_before_returning_to_caller"}
  };

  return TestFunction::runTests(tests, (sizeof(tests) / sizeof(TestFunction)));
}
