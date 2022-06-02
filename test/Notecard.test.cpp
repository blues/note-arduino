#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "Notecard.h"
#include "NoteI2c_Arduino.hpp"
#include "NoteSerial_Arduino.hpp"
#include "TestFunction.hpp"
#include "mock/mock-arduino.hpp"
#include "mock/mock-parameters.hpp"
#include "mock/NoteI2c_Mock.hpp"
#include "mock/NoteLog_Mock.hpp"
#include "mock/NoteSerial_Mock.hpp"

// Compile command: g++ -Wall -Wextra -Wpedantic mock/mock-arduino.cpp mock/mock-note-c-note.c mock/NoteI2c_Mock.cpp mock/NoteLog_Mock.cpp mock/NoteSerial_Mock.cpp ../src/Notecard.cpp Notecard.test.cpp -std=c++11 -I. -I../src -DNOTE_MOCK && ./a.out || echo "Tests Result: $?"

int test_notecard_begin_i2c_shares_a_memory_allocation_functon_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;

  noteSetFnDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(0x1B, 79);

   // Assert
  ///////////

  if (noteSetFnDefault_Parameters.mallocfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDefault_Parameters.mallocfn == " << !!noteSetFnDefault_Parameters.mallocfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_shares_a_memory_free_functon_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;

  noteSetFnDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(0x1B, 79);

   // Assert
  ///////////

  if (noteSetFnDefault_Parameters.freefn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDefault_Parameters.freefn == " << !!noteSetFnDefault_Parameters.freefn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_shares_a_delay_functon_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;

  noteSetFnDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(0x1B, 79);

   // Arrange
  ////////////

  if (noteSetFnDefault_Parameters.delayfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDefault_Parameters.delayfn == " << !!noteSetFnDefault_Parameters.delayfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_shares_a_millis_functon_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;

  noteSetFnDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(0x1B, 79);

   // Assert
  ///////////

  if (noteSetFnDefault_Parameters.millisfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDefault_Parameters.millisfn == " << !!noteSetFnDefault_Parameters.millisfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_does_not_modify_i2c_address_parameter_before_passing_to_note_c()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  // 0x1B binary representation => 0001 1011
  const uint16_t EXPECTED_ADDRESS = 0x1B;

  noteSetFnI2C_Parameters.reset();

   // Action
  ///////////

  notecard.begin(EXPECTED_ADDRESS, 79);

   // Assert
  ///////////

  if (EXPECTED_ADDRESS == noteSetFnI2C_Parameters.i2caddr)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2C_Parameters.i2caddr == " << noteSetFnI2C_Parameters.i2caddr << ", EXPECTED: " << EXPECTED_ADDRESS << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_does_not_modify_i2c_max_parameter_before_passing_to_note_c()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  const uint32_t EXPECTED_RESULT = 79;

  noteSetFnI2C_Parameters.reset();

   // Action
  ///////////

  notecard.begin(0x1B, EXPECTED_RESULT);

   // Assert
  ///////////

  if (EXPECTED_RESULT == noteSetFnI2C_Parameters.i2cmax)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2C_Parameters.i2cmax == " << noteSetFnI2C_Parameters.i2cmax << ", EXPECTED: " << EXPECTED_RESULT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_shares_an_i2c_reset_functon_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;

  noteSetFnI2C_Parameters.reset();

   // Action
  ///////////

  notecard.begin(0x1B, 79);

   // Assert
  ///////////

  if (noteSetFnI2C_Parameters.resetfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2C_Parameters.resetfn == " << !!noteSetFnI2C_Parameters.resetfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_shares_an_i2c_transmit_functon_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;

  noteSetFnI2C_Parameters.reset();

   // Action
  ///////////

  notecard.begin(0x1B, 79);

   // Assert
  ///////////

  if (noteSetFnI2C_Parameters.transmitfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2C_Parameters.transmitfn == " << !!noteSetFnI2C_Parameters.transmitfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_shares_an_i2c_receive_functon_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;

  noteSetFnI2C_Parameters.reset();

   // Action
  ///////////

  notecard.begin(0x1B, 79);

   // Assert
  ///////////

  if (noteSetFnI2C_Parameters.receivefn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2C_Parameters.receivefn == " << !!noteSetFnI2C_Parameters.receivefn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_default_parameter_for_i2c_max_is_passed_to_note_c()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;

  noteSetFnI2C_Parameters.reset();
  noteSetFnI2C_Parameters.i2cmax = (NOTE_I2C_MAX_DEFAULT - 1);

   // Action
  ///////////

  notecard.begin(0x1B);

   // Assert
  ///////////

  if (NOTE_I2C_MAX_DEFAULT == noteSetFnI2C_Parameters.i2cmax)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2C_Parameters.i2cmax == " << noteSetFnI2C_Parameters.i2cmax << ", EXPECTED: " << NOTE_I2C_MAX_DEFAULT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_default_parameter_for_i2c_address_is_passed_to_note_c()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;

  noteSetFnI2C_Parameters.reset();
  noteSetFnI2C_Parameters.i2caddr = (NOTE_I2C_ADDR_DEFAULT - 1);

   // Action
  ///////////

  notecard.begin();

   // Assert
  ///////////

  if (NOTE_I2C_ADDR_DEFAULT == noteSetFnI2C_Parameters.i2caddr)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2C_Parameters.i2caddr == " << noteSetFnI2C_Parameters.i2caddr << ", EXPECTED: " << NOTE_I2C_ADDR_DEFAULT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_default_parameter_for_wirePort_instantiates_the_note_i2c_interface_with_wire()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;

  make_note_i2c_Parameters.reset();

   // Action
  ///////////

  notecard.begin();

   // Assert
  ///////////

  if (&Wire == make_note_i2c_Parameters.i2c_bus)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tmake_note_i2c_Parameters.i2c_bus == " << !!make_note_i2c_Parameters.i2c_bus << ", EXPECTED: " << &Wire << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_parameter_for_wirePort_instantiates_the_note_i2c_interface_with_wirePort()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  TwoWire mockWire;

  make_note_i2c_Parameters.reset();

   // Action
  ///////////

  notecard.begin(NOTE_I2C_ADDR_DEFAULT, NOTE_I2C_MAX_DEFAULT, mockWire);

   // Assert
  ///////////

  if (&mockWire == make_note_i2c_Parameters.i2c_bus)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tmake_note_i2c_Parameters.i2c_bus == " << make_note_i2c_Parameters.i2c_bus << ", EXPECTED: " << &mockWire << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_sets_user_agent_to_note_arduino()
{
  int result;

   // Arrange
  ////////////

  const char * const EXPECTED_USER_AGENT = "note-arduino";
  Notecard notecard;

  noteSetUserAgent_Parameters.reset();

   // Action
  ///////////

  notecard.begin();

   // Assert
  ///////////

  if (noteSetUserAgent_Parameters.agent && !strcmp(EXPECTED_USER_AGENT, noteSetUserAgent_Parameters.agent))
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetUserAgent_Parameters.agent == " << noteSetUserAgent_Parameters.agent << ", EXPECTED: " << EXPECTED_USER_AGENT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_shares_a_memory_allocation_functon_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;

  noteSetFnDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(Serial, 9600);

   // Assert
  ///////////

  if (noteSetFnDefault_Parameters.mallocfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDefault_Parameters.mallocfn == " << !!noteSetFnDefault_Parameters.mallocfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_shares_a_memory_free_functon_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;

  noteSetFnDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(Serial, 9600);

   // Assert
  ///////////

  if (noteSetFnDefault_Parameters.freefn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDefault_Parameters.freefn == " << !!noteSetFnDefault_Parameters.freefn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_shares_a_delay_functon_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;

  noteSetFnDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(Serial, 9600);

   // Assert
  ///////////

  if (noteSetFnDefault_Parameters.delayfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDefault_Parameters.delayfn == " << !!noteSetFnDefault_Parameters.delayfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_shares_a_millis_functon_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;

  noteSetFnDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(Serial, 9600);

   // Assert
  ///////////

  if (nullptr != noteSetFnDefault_Parameters.millisfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDefault_Parameters.millisfn == " << !!noteSetFnDefault_Parameters.millisfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_shares_a_serial_reset_functon_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;

  noteSetFnSerial_Parameters.reset();

   // Action
  ///////////

  notecard.begin(Serial, 9600);

   // Assert
  ///////////

  if (noteSetFnSerial_Parameters.resetfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnSerial_Parameters.resetfn == " << !!noteSetFnSerial_Parameters.resetfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_shares_a_serial_transmit_functon_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;

  noteSetFnSerial_Parameters.reset();

   // Action
  ///////////

  notecard.begin(Serial, 9600);

   // Assert
  ///////////

  if (noteSetFnSerial_Parameters.writefn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnSerial_Parameters.writefn == " << !!noteSetFnSerial_Parameters.writefn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_shares_a_serial_available_functon_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;

  noteSetFnSerial_Parameters.reset();

   // Action
  ///////////

  notecard.begin(Serial, 9600);

   // Assert
  ///////////

  if (noteSetFnSerial_Parameters.availfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnSerial_Parameters.availfn == " << !!noteSetFnSerial_Parameters.availfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_shares_a_serial_receive_functon_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;

  noteSetFnSerial_Parameters.reset();

   // Action
  ///////////

  notecard.begin(Serial, 9600);

   // Assert
  ///////////

  if (noteSetFnSerial_Parameters.readfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnSerial_Parameters.readfn == " << !!noteSetFnSerial_Parameters.readfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_initializes_the_provided_serial_interface_with_provided_speed()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  const unsigned int EXPECTED_BAUD_RATE = 9600;

  make_note_serial_Parameters.reset();

   // Action
  ///////////

  notecard.begin(Serial, EXPECTED_BAUD_RATE);

   // Assert
  ///////////

  if (EXPECTED_BAUD_RATE == make_note_serial_Parameters.baud_rate)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tmake_note_serial_Parameters.baud_rate == " << make_note_serial_Parameters.baud_rate << ", EXPECTED: " << EXPECTED_BAUD_RATE << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_initializes_the_provided_serial_interface_with_default_speed()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  const unsigned int EXPECTED_BAUD_RATE = 9600;

  make_note_serial_Parameters.reset();

   // Action
  ///////////

  notecard.begin(Serial);

   // Assert
  ///////////

  if (EXPECTED_BAUD_RATE == make_note_serial_Parameters.baud_rate)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tmake_note_serial_Parameters.baud_rate == " << make_note_serial_Parameters.baud_rate << ", EXPECTED: " << EXPECTED_BAUD_RATE << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_sets_user_agent_to_note_arduino()
{
  int result;

   // Arrange
  ////////////

  const char * const EXPECTED_USER_AGENT = "note-arduino";
  Notecard notecard;

  noteSetUserAgent_Parameters.reset();

   // Action
  ///////////

  notecard.begin(Serial);

   // Assert
  ///////////

  if (noteSetUserAgent_Parameters.agent && !strcmp(EXPECTED_USER_AGENT, noteSetUserAgent_Parameters.agent))
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetUserAgent_Parameters.agent == " << noteSetUserAgent_Parameters.agent << ", EXPECTED: " << EXPECTED_USER_AGENT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_setDebugOutputStream_does_not_modify_log_channel_parameter_before_passing_to_make_note_log()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;

  make_note_log_Parameters.reset();

   // Action
  ///////////

  notecard.setDebugOutputStream(&Serial);

   // Assert
  ///////////

  if (&Serial == make_note_log_Parameters.log_channel)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tmake_note_log_Parameters.log_channel == " << make_note_log_Parameters.log_channel << ", EXPECTED: " << &Serial << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_setDebugOutputStream_shares_a_debug_log_functon_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;

  make_note_log_Parameters.reset();
  noteSetFnDebugOutput_Parameters.reset();

  make_note_log_Parameters.result = reinterpret_cast<NoteLog *>(&result);

   // Action
  ///////////

  notecard.setDebugOutputStream(&Serial);

   // Assert
  ///////////

  if (noteSetFnDebugOutput_Parameters.fn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDebugOutput_Parameters.fn == " << !!noteSetFnDebugOutput_Parameters.fn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_setDebugOutputStream_clears_the_debug_log_functon_pointer_when_nullptr_is_provided()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;

  make_note_log_Parameters.reset();
  noteSetFnDebugOutput_Parameters.reset();
  noteSetFnDebugOutput_Parameters.fn = reinterpret_cast<debugOutputFn>(&result);

   // Action
  ///////////

  notecard.setDebugOutputStream(nullptr);

   // Assert
  ///////////

  if (!noteSetFnDebugOutput_Parameters.fn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDebugOutput_Parameters.fn == " << !!noteSetFnDebugOutput_Parameters.fn << ", EXPECTED: 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_clearDebugOutputStream_clears_the_debug_log_functon_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;

  noteSetFnDebugOutput_Parameters.reset();
  noteSetFnDebugOutput_Parameters.fn = reinterpret_cast<debugOutputFn>(&result);

   // Action
  ///////////

  notecard.clearDebugOutputStream();

   // Assert
  ///////////

  if (!noteSetFnDebugOutput_Parameters.fn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDebugOutput_Parameters.fn == " << !!noteSetFnDebugOutput_Parameters.fn << ", EXPECTED: 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_newRequest_does_not_modify_string_parameter_value_before_passing_to_note_c()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  const char * const EXPECTED_RESULT = "Hello, Test!";

  noteNewRequest_Parameters.reset();

   // Action
  ///////////

  notecard.newRequest(EXPECTED_RESULT);

   // Assert
  ///////////

  if (!strcmp(EXPECTED_RESULT, noteNewRequest_Parameters.request_cache.c_str()))
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteNewRequest_Parameters.request_cache.c_str() == \"" << noteNewRequest_Parameters.request_cache.c_str() << "\", EXPECTED: \"" << EXPECTED_RESULT << "\"" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_newRequest_does_not_modify_note_c_result_value_before_returning_to_caller()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  J * const EXPECTED_JSON = reinterpret_cast<J *>(malloc(sizeof(J))); // JCreateObject();
  assert(nullptr != EXPECTED_JSON);

  noteNewRequest_Parameters.reset();
  memset(EXPECTED_JSON, 0x55, sizeof(J)); //assert(nullptr != JAddStringToObject(EXPECTED_JSON,"key1", "value1"));
  {
    noteNewRequest_Parameters.result = reinterpret_cast<J *>(malloc(sizeof(J)));
    assert(nullptr != noteNewRequest_Parameters.result);
    memcpy(noteNewRequest_Parameters.result, EXPECTED_JSON, sizeof(J));
  } //assert(nullptr != (noteNewRequest_Parameters.result = JDuplicate(EXPECTED_JSON,true)));

   // Action
  ///////////

  const J * const ACTUAL_RESULT = notecard.newRequest(nullptr);

   // Assert
  ///////////

  if (!memcmp(ACTUAL_RESULT, EXPECTED_JSON, sizeof(J)) /*JCompare(EXPECTED_JSON, json_result, false)*/)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnotecard.newRequest(nullptr) != EXPECTED_JSON" << std::endl;
    std::cout << "[";
  }

  free(noteNewRequest_Parameters.result);
  free(EXPECTED_JSON);
  return result;
}

int test_notecard_sendRequest_does_not_modify_j_object_parameter_value_before_passing_to_note_c()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  J * EXPECTED_JSON = reinterpret_cast<J *>(malloc(sizeof(J))); // JCreateObject();
  J * json_cpy;
  assert(nullptr != EXPECTED_JSON);

  noteRequest_Parameters.reset();
  memset(EXPECTED_JSON, 0x55, sizeof(J)); //assert(nullptr != JAddStringToObject(EXPECTED_JSON,"key1", "value1"));
  {
    json_cpy = reinterpret_cast<J *>(malloc(sizeof(J)));
    assert(nullptr != json_cpy);
    memcpy(json_cpy, EXPECTED_JSON, sizeof(J));
  } //assert(nullptr != (json_cpy = JDuplicate(EXPECTED_JSON,true)));

   // Action
  ///////////

  notecard.sendRequest(json_cpy);

   // Assert
  ///////////

  if (!memcmp(EXPECTED_JSON, noteRequest_Parameters.req, sizeof(J)) /*JCompare(EXPECTED_JSON, noteSendRequest_Parameters.req, false)*/)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteRequest_Parameters.req != EXPECTED_JSON" << std::endl;
    std::cout << "[";
  }

  free(json_cpy);
  free(EXPECTED_JSON);
  return result;
}

int test_notecard_sendRequest_does_not_modify_note_c_result_value_before_returning_to_caller()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  const bool EXPECTED_RESULT = true;

  noteRequest_Parameters.reset();
  noteRequest_Parameters.result = EXPECTED_RESULT;

   // Action
  ///////////

  const bool ACTUAL_RESULT = notecard.sendRequest(nullptr);

   // Assert
  ///////////

  if (EXPECTED_RESULT == ACTUAL_RESULT)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnotecard.sendRequest(nullptr) == \"" << ACTUAL_RESULT << "\", EXPECTED: \"" << EXPECTED_RESULT << "\"" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_requestAndResponse_does_not_modify_j_object_parameter_value_before_passing_to_note_c()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  J *EXPECTED_JSON = reinterpret_cast<J *>(malloc(sizeof(J))); // JCreateObject();
  J *json_cpy;
  assert(nullptr != EXPECTED_JSON);

  noteRequestResponse_Parameters.reset();
  memset(EXPECTED_JSON, 0x55, sizeof(J)); //assert(nullptr != JAddStringToObject(EXPECTED_JSON,"key1", "value1"));
  {
    json_cpy = reinterpret_cast<J *>(malloc(sizeof(J)));
    assert(nullptr != json_cpy);
    memcpy(json_cpy, EXPECTED_JSON, sizeof(J));
  } //assert(nullptr != (json_cpy = JDuplicate(EXPECTED_JSON,true)));

   // Action
  ///////////

  notecard.requestAndResponse(json_cpy);

   // Assert
  ///////////

  if (!memcmp(EXPECTED_JSON, noteRequestResponse_Parameters.req, sizeof(J)) /*JCompare(EXPECTED_JSON, noteRequestResponse_Parameters.req, false)*/)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteRequestResponse_Parameters.req != EXPECTED_JSON" << std::endl;
    std::cout << "[";
  }

  free(json_cpy);
  free(EXPECTED_JSON);
  return result;
}

int test_notecard_requestAndResponse_does_not_modify_note_c_result_value_before_returning_to_caller()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  J * EXPECTED_JSON = reinterpret_cast<J *>(malloc(sizeof(J))); // JCreateObject();
  assert(nullptr != EXPECTED_JSON);

  noteRequestResponse_Parameters.reset();
  memset(EXPECTED_JSON, 0x55, sizeof(J)); //assert(nullptr != JAddStringToObject(EXPECTED_JSON,"key1", "value1"));
  {
    noteRequestResponse_Parameters.result = reinterpret_cast<J *>(malloc(sizeof(J)));
    assert(nullptr != noteRequestResponse_Parameters.result);
    memcpy(noteRequestResponse_Parameters.result, EXPECTED_JSON, sizeof(J));
  } //assert(nullptr != (noteRequestResponse_Parameters.result = JDuplicate(EXPECTED_JSON,true)));

   // Action
  ///////////

  const J * const ACTUAL_RESULT = notecard.requestAndResponse(nullptr);

   // Assert
  ///////////

  if (!memcmp(EXPECTED_JSON, ACTUAL_RESULT, sizeof(J)) /*JCompare(EXPECTED_JSON, json_result, false)*/)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnotecard.requestAndResponse(nullptr) != EXPECTED_JSON" << std::endl;
    std::cout << "[";
  }

  free(noteRequestResponse_Parameters.result);
  free(EXPECTED_JSON);
  return result;
}

int test_notecard_deleteResponse_does_not_modify_j_object_parameter_pointer_before_passing_to_note_c()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  J *const EXPECTED_POINTER = reinterpret_cast<J *>(0x19790917);

  noteDeleteResponse_Parameters.reset();

   // Action
  ///////////

  notecard.deleteResponse(EXPECTED_POINTER);

   // Assert
  ///////////

  if (EXPECTED_POINTER == noteDeleteResponse_Parameters.response)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteDeleteResponse_Parameters.response == " << std::hex << noteDeleteResponse_Parameters.response << ", EXPECTED: " << EXPECTED_POINTER << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_logDebug_does_not_modify_string_parameter_value_before_passing_to_note_c()
{
  int result;

   // Arrange
  ////////////

  const char str[] = "Hello, Test!";
  Notecard notecard;

  noteDebug_Parameters.reset();

   // Action
  ///////////

  notecard.logDebug(str);

   // Assert
  ///////////

  if (!strcmp(str, noteDebug_Parameters.message_cache.c_str()))
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteDebug_Parameters.message_cache.c_str() == \"" << noteDebug_Parameters.message_cache.c_str() << "\", EXPECTED: \"" << str << "\"" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_logDebugf_does_not_modify_string_parameter_value_before_passing_to_note_c()
{
  int result;

   // Arrange
  ////////////

  const char str[] = "Hello, Test 123!";
  Notecard notecard;

  noteDebug_Parameters.reset();

   // Action
  ///////////

  notecard.logDebugf("Hello, %s %d%c", "Test", 123, '!');

   // Assert
  ///////////

  if (!strcmp(str, noteDebug_Parameters.message_cache.c_str()))
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteDebug_Parameters.message_cache.c_str() == \"" << noteDebug_Parameters.message_cache.c_str() << "\", EXPECTED: \"" << str << "\"" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_debugSyncStatus_does_not_modify_pollFrequencyMs_parameter_before_passing_to_note_c()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  const int EXPECTED_RESULT = 79;

  noteDebugSyncStatus_Parameters.reset();

   // Action
  ///////////

  notecard.debugSyncStatus(EXPECTED_RESULT, 0);

   // Assert
  ///////////

  if (EXPECTED_RESULT == noteDebugSyncStatus_Parameters.pollFrequencyMs)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteDebugSyncStatus_Parameters.pollFrequencyMs == " << noteDebugSyncStatus_Parameters.pollFrequencyMs << ", EXPECTED: " << EXPECTED_RESULT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_debugSyncStatus_does_not_modify_maxLevel_parameter_before_passing_to_note_c()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  const int EXPECTED_RESULT = 79;

  noteDebugSyncStatus_Parameters.reset();

   // Action
  ///////////

  notecard.debugSyncStatus(0, EXPECTED_RESULT);

   // Assert
  ///////////

  if (EXPECTED_RESULT == noteDebugSyncStatus_Parameters.maxLevel)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteDebugSyncStatus_Parameters.maxLevel == " << noteDebugSyncStatus_Parameters.maxLevel << ", EXPECTED: " << EXPECTED_RESULT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_debugSyncStatus_does_not_modify_note_c_result_value_before_returning_to_caller()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  const bool EXPECTED_RESULT = true;

  noteDebugSyncStatus_Parameters.reset();
  noteDebugSyncStatus_Parameters.result = EXPECTED_RESULT;

   // Action
  ///////////

  const bool ACTUAL_RESULT = notecard.debugSyncStatus(0, 0);

   // Assert
  ///////////

  if (EXPECTED_RESULT == ACTUAL_RESULT)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnotecard.debugSyncStatus(0, 0) == " << ACTUAL_RESULT << ", EXPECTED: " << EXPECTED_RESULT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_responseError_does_not_modify_j_object_parameter_value_before_passing_to_note_c()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  J * EXPECTED_JSON = reinterpret_cast<J *>(malloc(sizeof(J))); // JCreateObject();
  J * json_cpy;
  assert(nullptr != EXPECTED_JSON);

  noteResponseError_Parameters.reset();
  memset(EXPECTED_JSON, 0x55, sizeof(J)); //assert(nullptr != JAddStringToObject(EXPECTED_JSON,"key1", "value1"));
  {
    json_cpy = reinterpret_cast<J *>(malloc(sizeof(J)));
    assert(nullptr != json_cpy);
    memcpy(json_cpy, EXPECTED_JSON, sizeof(J));
  } //assert(nullptr != (json_cpy = JDuplicate(EXPECTED_JSON,true)));

   // Action
  ///////////

  notecard.responseError(json_cpy);

   // Assert
  ///////////

  if (!memcmp(EXPECTED_JSON, noteResponseError_Parameters.rsp, sizeof(J)) /*JCompare(EXPECTED_JSON, noteRequestResponse_Parameters.rsp, false)*/)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteResponseError_Parameters.rsp != EXPECTED_JSON" << std::endl;
    std::cout << "[";
  }

  free(json_cpy);
  free(EXPECTED_JSON);
  return result;
}

int test_notecard_responseError_does_not_modify_note_c_result_value_before_returning_to_caller()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  const bool EXPECTED_RESULT = true;

  noteResponseError_Parameters.reset();
  noteResponseError_Parameters.result = EXPECTED_RESULT;

   // Action
  ///////////

  const bool ACTUAL_RESULT = notecard.responseError(nullptr);

   // Assert
  ///////////

  if (EXPECTED_RESULT == ACTUAL_RESULT)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnotecard.responseError(nullptr) == " << ACTUAL_RESULT << ", EXPECTED: " << EXPECTED_RESULT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_newCommand_does_not_modify_string_parameter_value_before_passing_to_note_c()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  const char EXPECTED_RESULT[] = "Hello, Test!";

  noteNewCommand_Parameters.reset();

   // Action
  ///////////

  notecard.newCommand(EXPECTED_RESULT);

   // Assert
  ///////////

  if (!strcmp(EXPECTED_RESULT, noteNewCommand_Parameters.request_cache.c_str()))
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteNewCommand_Parameters.request_cache.c_str() == \"" << noteNewCommand_Parameters.request_cache.c_str() << "\", EXPECTED: \"" << EXPECTED_RESULT << "\"" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_newCommand_does_not_modify_note_c_result_value_before_returning_to_caller()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  J * EXPECTED_JSON = reinterpret_cast<J *>(malloc(sizeof(J))); // JCreateObject();
  assert(nullptr != EXPECTED_JSON);

  noteNewCommand_Parameters.reset();
  memset(EXPECTED_JSON, 0x55, sizeof(J)); //assert(nullptr != JAddStringToObject(EXPECTED_JSON,"key1", "value1"));
  {
    noteNewCommand_Parameters.result = reinterpret_cast<J *>(malloc(sizeof(J)));
    assert(nullptr != noteNewCommand_Parameters.result);
    memcpy(noteNewCommand_Parameters.result, EXPECTED_JSON, sizeof(J));
  } //assert(nullptr != (noteNewCommand_Parameters.result = JDuplicate(EXPECTED_JSON,true)));

   // Action
  ///////////

  const J * const ACTUAL_RESULT = notecard.newCommand(nullptr);

   // Assert
  ///////////

  if (!memcmp(EXPECTED_JSON, ACTUAL_RESULT, sizeof(J)) /*JCompare(EXPECTED_JSON, ACTUAL_RESULT, false)*/)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnotecard.newCommand(nullptr) != EXPECTED_JSON" << std::endl;
    std::cout << "[";
  }

  free(noteNewCommand_Parameters.result);
  free(EXPECTED_JSON);
  return result;
}

int test_static_callback_note_i2c_receive_invokes_notei2c_receive()
{
  int result;

   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x17;
  const uint16_t SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t actual_response_size;

  Notecard notecard;
  NoteI2c_Arduino mockI2c_arduino(Wire);  // Instantiate NoteI2c (mocked)
  make_note_i2c_Parameters.reset();
  make_note_i2c_Parameters.result = &mockI2c_arduino;  // Return mocked interface
  notecard.begin();  // Provides access to the hidden static callback methods through `note-c` mocks
  noteI2cReceive_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnI2C_Parameters.receivefn(ADDRESS, response_buffer, SIZE, &actual_response_size);

   // Assert
  ///////////

  if (noteI2cReceive_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteI2cReceive_Parameters.invoked == " << noteI2cReceive_Parameters.invoked << ", EXPECTED: > 0" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_i2c_receive_does_not_modify_device_address_parameter_before_passing_to_interface_method()
{
  int result;

   // Arrange
  ////////////

  // 0x1B binary representation => 0001 1011
  const uint16_t EXPECTED_ADDRESS = 0x1B;
  const uint16_t SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t actual_response_size;

  Notecard notecard;
  NoteI2c_Arduino mockI2c_arduino(Wire);  // Instantiate NoteI2c (mocked)
  make_note_i2c_Parameters.reset();
  make_note_i2c_Parameters.result = &mockI2c_arduino;  // Return mocked interface
  notecard.begin();  // Provides access to the hidden static callback methods through `note-c` mocks
  noteI2cReceive_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnI2C_Parameters.receivefn(EXPECTED_ADDRESS, response_buffer, SIZE, &actual_response_size);

   // Assert
  ///////////

  if (EXPECTED_ADDRESS == noteI2cReceive_Parameters.device_address)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteI2cReceive_Parameters.device_address == " << noteI2cReceive_Parameters.device_address << ", EXPECTED: " << EXPECTED_ADDRESS << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_i2c_receive_does_not_modify_buffer_parameter_address_before_passing_to_interface_method()
{
  int result;

   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;
  const uint16_t SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t actual_response_size;

  Notecard notecard;
  NoteI2c_Arduino mockI2c_arduino(Wire);  // Instantiate NoteI2c (mocked)
  make_note_i2c_Parameters.reset();
  make_note_i2c_Parameters.result = &mockI2c_arduino;  // Return mocked interface
  notecard.begin();  // Provides access to the hidden static callback methods through `note-c` mocks
  noteI2cReceive_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnI2C_Parameters.receivefn(ADDRESS, response_buffer, SIZE, &actual_response_size);

   // Assert
  ///////////

  if (response_buffer == noteI2cReceive_Parameters.buffer)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << std::hex << "\tnoteI2cReceive_Parameters.buffer == " << noteI2cReceive_Parameters.buffer << ", EXPECTED: " << &response_buffer[0] << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_i2c_receive_does_not_modify_size_parameter_before_passing_to_interface_method()
{
  int result;

   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;
  const uint16_t EXPECTED_SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t actual_response_size;

  Notecard notecard;
  NoteI2c_Arduino mockI2c_arduino(Wire);  // Instantiate NoteI2c (mocked)
  make_note_i2c_Parameters.reset();
  make_note_i2c_Parameters.result = &mockI2c_arduino;  // Return mocked interface
  notecard.begin();  // Provides access to the hidden static callback methods through `note-c` mocks
  noteI2cReceive_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnI2C_Parameters.receivefn(ADDRESS, response_buffer, EXPECTED_SIZE, &actual_response_size);

   // Assert
  ///////////

  if (EXPECTED_SIZE == noteI2cReceive_Parameters.requested_byte_count)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteI2cReceive_Parameters.requested_byte_count == " << noteI2cReceive_Parameters.requested_byte_count << ", EXPECTED: " << EXPECTED_SIZE << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_i2c_receive_does_not_modify_available_parameter_address_before_passing_to_interface_method()
{
  int result;

   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;
  const uint16_t SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t actual_response_size;

  Notecard notecard;
  NoteI2c_Arduino mockI2c_arduino(Wire);  // Instantiate NoteI2c (mocked)
  make_note_i2c_Parameters.reset();
  make_note_i2c_Parameters.result = &mockI2c_arduino;  // Return mocked interface
  notecard.begin();  // Provides access to the hidden static callback methods through `note-c` mocks
  noteI2cReceive_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnI2C_Parameters.receivefn(ADDRESS, response_buffer, SIZE, &actual_response_size);

   // Assert
  ///////////

  if (&actual_response_size == noteI2cReceive_Parameters.available)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << std::hex << "\tnoteI2cReceive_Parameters.available == " << noteI2cReceive_Parameters.available << ", EXPECTED: " << &actual_response_size << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_i2c_receive_does_not_modify_interface_method_return_value()
{
  int result;

   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;
  const uint16_t SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t actual_response_size;

  Notecard notecard;
  NoteI2c_Arduino mockI2c_arduino(Wire);  // Instantiate NoteI2c (mocked)
  make_note_i2c_Parameters.reset();
  make_note_i2c_Parameters.result = &mockI2c_arduino;  // Return mocked interface
  notecard.begin();  // Provides access to the hidden static callback methods through `note-c` mocks
  noteI2cReceive_Parameters.reset();  // Clear the structure for testing results
  noteI2cReceive_Parameters.result = "i2c: fake test error!";

   // Action
  ///////////

  const char * const ACTUAL_RESULT = noteSetFnI2C_Parameters.receivefn(ADDRESS, response_buffer, SIZE, &actual_response_size);

   // Assert
  ///////////

  if (!strcmp(ACTUAL_RESULT, noteI2cReceive_Parameters.result))
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tACTUAL_RESULT == " << ACTUAL_RESULT << ", EXPECTED: " << noteI2cReceive_Parameters.result << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_i2c_receive_does_not_call_interface_method_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;
  const uint16_t SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t actual_response_size;

  Notecard notecard;
  make_note_i2c_Parameters.reset();
  notecard.begin();  // Provides access to the hidden static callback methods through `note-c` mocks
  noteI2cReceive_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnI2C_Parameters.receivefn(ADDRESS, response_buffer, SIZE, &actual_response_size);

   // Assert
  ///////////

  if (!noteI2cReceive_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteI2cReceive_Parameters.invoked == " << noteI2cReceive_Parameters.invoked << ", EXPECTED: zero (0)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_i2c_receive_returns_error_message_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;
  const uint16_t SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t actual_response_size;
  const char * const EXPECTED_RESULT = "i2c: A call to Notecard::begin() is required. {io}";

  Notecard notecard;
  make_note_i2c_Parameters.reset();
  notecard.begin();  // Provides access to the hidden static callback methods through `note-c` mocks
  noteI2cReceive_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  const char * const ACTUAL_RESULT = noteSetFnI2C_Parameters.receivefn(ADDRESS, response_buffer, SIZE, &actual_response_size);

   // Assert
  ///////////

  if (!strcmp(ACTUAL_RESULT,EXPECTED_RESULT))
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tACTUAL_RESULT == " << ACTUAL_RESULT << ", EXPECTED: " << EXPECTED_RESULT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_i2c_reset_invokes_notei2c_reset()
{
  int result;

   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x17;

  Notecard notecard;
  NoteI2c_Arduino mockI2c_arduino(Wire);  // Instantiate NoteI2c (mocked)
  make_note_i2c_Parameters.reset();
  make_note_i2c_Parameters.result = &mockI2c_arduino;  // Return mocked interface
  notecard.begin();  // Provides access to the hidden static callback methods through `note-c` mocks
  noteI2cReset_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnI2C_Parameters.resetfn(ADDRESS);

   // Assert
  ///////////

  if (noteI2cReset_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteI2cReset_Parameters.invoked == " << noteI2cReset_Parameters.invoked << ", EXPECTED: > 0" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_i2c_reset_does_not_modify_device_address_parameter_before_passing_to_interface_method()
{
  int result;

   // Arrange
  ////////////

  // 0x1B binary representation => 0001 1011
  const uint16_t EXPECTED_ADDRESS = 0x1B;

  Notecard notecard;
  NoteI2c_Arduino mockI2c_arduino(Wire);  // Instantiate NoteI2c (mocked)
  make_note_i2c_Parameters.reset();
  make_note_i2c_Parameters.result = &mockI2c_arduino;  // Return mocked interface
  notecard.begin();  // Provides access to the hidden static callback methods through `note-c` mocks
  noteI2cReset_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnI2C_Parameters.resetfn(EXPECTED_ADDRESS);

   // Assert
  ///////////

  if (EXPECTED_ADDRESS == noteI2cReset_Parameters.device_address)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteI2cReset_Parameters.device_address == " << noteI2cReset_Parameters.device_address << ", EXPECTED: " << EXPECTED_ADDRESS << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_i2c_reset_does_not_modify_interface_method_return_value()
{
  int result;

   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;

  Notecard notecard;
  NoteI2c_Arduino mockI2c_arduino(Wire);  // Instantiate NoteI2c (mocked)
  make_note_i2c_Parameters.reset();
  make_note_i2c_Parameters.result = &mockI2c_arduino;  // Return mocked interface
  notecard.begin();  // Provides access to the hidden static callback methods through `note-c` mocks
  noteI2cReset_Parameters.reset();  // Clear the structure for testing results
  noteI2cReset_Parameters.result = true;

   // Action
  ///////////

  const bool ACTUAL_RESULT = noteSetFnI2C_Parameters.resetfn(ADDRESS);

   // Assert
  ///////////

  if (ACTUAL_RESULT == noteI2cReset_Parameters.result)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tACTUAL_RESULT == " << ACTUAL_RESULT << ", EXPECTED: " << noteI2cReset_Parameters.result << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_i2c_reset_does_not_call_interface_method_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;

  Notecard notecard;
  make_note_i2c_Parameters.reset();
  notecard.begin();  // Provides access to the hidden static callback methods through `note-c` mocks
  noteI2cReset_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnI2C_Parameters.resetfn(ADDRESS);

   // Assert
  ///////////

  if (!noteI2cReset_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteI2cReset_Parameters.invoked == " << noteI2cReset_Parameters.invoked << ", EXPECTED: zero (0)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_i2c_reset_returns_false_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;

  Notecard notecard;
  make_note_i2c_Parameters.reset();
  notecard.begin();  // Provides access to the hidden static callback methods through `note-c` mocks
  noteI2cReset_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  const bool ACTUAL_RESULT = noteSetFnI2C_Parameters.resetfn(ADDRESS);

   // Assert
  ///////////

  if (!ACTUAL_RESULT)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tACTUAL_RESULT == " << ACTUAL_RESULT << ", EXPECTED: false (0)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_i2c_transmit_invokes_notei2c_transmit()
{
  int result;

   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x17;
  const uint16_t SIZE = 13;
  uint8_t transmit_buffer[32];

  Notecard notecard;
  NoteI2c_Arduino mockI2c_arduino(Wire);  // Instantiate NoteI2c (mocked)
  make_note_i2c_Parameters.reset();
  make_note_i2c_Parameters.result = &mockI2c_arduino;  // Return mocked interface
  notecard.begin();  // Provides access to the hidden static callback methods through `note-c` mocks
  noteI2cTransmit_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnI2C_Parameters.transmitfn(ADDRESS, transmit_buffer, SIZE);

   // Assert
  ///////////

  if (noteI2cTransmit_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteI2cTransmit_Parameters.invoked == " << noteI2cTransmit_Parameters.invoked << ", EXPECTED: > 0" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_i2c_transmit_does_not_modify_device_address_parameter_before_passing_to_interface_method()
{
  int result;

   // Arrange
  ////////////

  // 0x1B binary representation => 0001 1011
  const uint16_t EXPECTED_ADDRESS = 0x1B;
  const uint16_t SIZE = 13;
  uint8_t transmit_buffer[32];

  Notecard notecard;
  NoteI2c_Arduino mockI2c_arduino(Wire);  // Instantiate NoteI2c (mocked)
  make_note_i2c_Parameters.reset();
  make_note_i2c_Parameters.result = &mockI2c_arduino;  // Return mocked interface
  notecard.begin();  // Provides access to the hidden static callback methods through `note-c` mocks
  noteI2cTransmit_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnI2C_Parameters.transmitfn(EXPECTED_ADDRESS, transmit_buffer, SIZE);

   // Assert
  ///////////

  if (EXPECTED_ADDRESS == noteI2cTransmit_Parameters.device_address)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteI2cTransmit_Parameters.device_address == " << noteI2cTransmit_Parameters.device_address << ", EXPECTED: " << EXPECTED_ADDRESS << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_i2c_transmit_does_not_modify_buffer_parameter_before_passing_to_interface_method()
{
  int result;

   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;
  const uint16_t SIZE = 13;
  uint8_t transmit_buffer[] = "Test Passed!";

  Notecard notecard;
  NoteI2c_Arduino mockI2c_arduino(Wire);  // Instantiate NoteI2c (mocked)
  make_note_i2c_Parameters.reset();
  make_note_i2c_Parameters.result = &mockI2c_arduino;  // Return mocked interface
  notecard.begin();  // Provides access to the hidden static callback methods through `note-c` mocks
  noteI2cTransmit_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnI2C_Parameters.transmitfn(ADDRESS, transmit_buffer, SIZE);

   // Assert
  ///////////

  if (!strcmp(reinterpret_cast<char *>(transmit_buffer),reinterpret_cast<char *>(noteI2cTransmit_Parameters.buffer)))
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteI2cTransmit_Parameters.buffer == " << noteI2cTransmit_Parameters.buffer << ", EXPECTED: " << transmit_buffer << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_i2c_transmit_does_not_modify_size_parameter_before_passing_to_interface_method()
{
  int result;

   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;
  const uint16_t EXPECTED_SIZE = 13;
  uint8_t transmit_buffer[32];

  Notecard notecard;
  NoteI2c_Arduino mockI2c_arduino(Wire);  // Instantiate NoteI2c (mocked)
  make_note_i2c_Parameters.reset();
  make_note_i2c_Parameters.result = &mockI2c_arduino;  // Return mocked interface
  notecard.begin();  // Provides access to the hidden static callback methods through `note-c` mocks
  noteI2cTransmit_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnI2C_Parameters.transmitfn(ADDRESS, transmit_buffer, EXPECTED_SIZE);

   // Assert
  ///////////

  if (EXPECTED_SIZE == noteI2cTransmit_Parameters.size)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteI2cTransmit_Parameters.size == " << noteI2cTransmit_Parameters.size << ", EXPECTED: " << EXPECTED_SIZE << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_i2c_transmit_does_not_modify_interface_method_return_value()
{
  int result;

   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;
  const uint16_t SIZE = 13;
  uint8_t transmit_buffer[32];

  Notecard notecard;
  NoteI2c_Arduino mockI2c_arduino(Wire);  // Instantiate NoteI2c (mocked)
  make_note_i2c_Parameters.reset();
  make_note_i2c_Parameters.result = &mockI2c_arduino;  // Return mocked interface
  notecard.begin();  // Provides access to the hidden static callback methods through `note-c` mocks
  noteI2cTransmit_Parameters.reset();  // Clear the structure for testing results
  noteI2cTransmit_Parameters.result = "i2c: fake test error!";

   // Action
  ///////////

  const char * const ACTUAL_RESULT = noteSetFnI2C_Parameters.transmitfn(ADDRESS, transmit_buffer, SIZE);

   // Assert
  ///////////

  if (!strcmp(ACTUAL_RESULT, noteI2cTransmit_Parameters.result))
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tACTUAL_RESULT == " << ACTUAL_RESULT << ", EXPECTED: " << noteI2cTransmit_Parameters.result << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_i2c_transmit_does_not_call_interface_method_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;
  const uint16_t SIZE = 13;
  uint8_t transmit_buffer[32];

  Notecard notecard;
  make_note_i2c_Parameters.reset();
  notecard.begin();  // Provides access to the hidden static callback methods through `note-c` mocks
  noteI2cTransmit_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnI2C_Parameters.transmitfn(ADDRESS, transmit_buffer, SIZE);

   // Assert
  ///////////

  if (!noteI2cTransmit_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteI2cTransmit_Parameters == " << noteI2cTransmit_Parameters.invoked << ", EXPECTED: zero (0)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_i2c_transmit_returns_error_message_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;
  const uint16_t SIZE = 13;
  uint8_t transmit_buffer[32];
  const char * const EXPECTED_RESULT = "i2c: A call to Notecard::begin() is required. {io}";

  Notecard notecard;
  make_note_i2c_Parameters.reset();
  notecard.begin();  // Provides access to the hidden static callback methods through `note-c` mocks
  noteI2cTransmit_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  const char * const ACTUAL_RESULT = noteSetFnI2C_Parameters.transmitfn(ADDRESS, transmit_buffer, SIZE);

   // Assert
  ///////////

  if (!strcmp(ACTUAL_RESULT,EXPECTED_RESULT))
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tACTUAL_RESULT == " << ACTUAL_RESULT << ", EXPECTED: " << EXPECTED_RESULT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_log_print_invokes_notelog_print()
{
  int result;

   // Arrange
  ////////////

  const char message[] = "Test passed!";

  Notecard notecard;
  NoteLog_Mock mockLog;  // Instantiate NoteLog (mocked)
  make_note_log_Parameters.reset();
  make_note_log_Parameters.result = &mockLog;  // Return mocked interface
  notecard.setDebugOutputStream(&result);  // Provides access to the hidden static callback methods through `note-c` mocks
  size_t(*noteLogPrint)(const char *) = noteSetFnDebugOutput_Parameters.fn;  // Capture the internal Notecard log function, `noteLogPrint`
  noteLogPrint_Parameters.reset();  // Clear the structure for testing NoteLog::print results

   // Action
  ///////////

  noteLogPrint(message);

   // Assert
  ///////////

  if (noteLogPrint_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteLogPrint_Parameters.invoked == " << noteLogPrint_Parameters.invoked << ", EXPECTED: > 0" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_log_print_does_not_modify_message_parameter_before_passing_to_interface_method()
{
  int result;

   // Arrange
  ////////////

  const char message[] = "Test passed!";

  Notecard notecard;
  NoteLog_Mock mockLog;  // Instantiate NoteLog (mocked)
  make_note_log_Parameters.reset();
  make_note_log_Parameters.result = &mockLog;  // Return mocked interface
  notecard.setDebugOutputStream(&result);  // Provides access to the hidden static callback methods through `note-c` mocks
  size_t(*noteLogPrint)(const char *) = noteSetFnDebugOutput_Parameters.fn;  // Capture the internal Notecard log function, `noteLogPrint`
  noteLogPrint_Parameters.reset();  // Clear the structure for testing NoteLog::print results

   // Action
  ///////////

  noteLogPrint(message);

   // Assert
  ///////////

  if (!strcmp(message, noteLogPrint_Parameters.message))
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteLogPrint_Parameters.message == " << noteLogPrint_Parameters.message << ", EXPECTED: " << message << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_log_print_does_not_modify_interface_method_return_value()
{
  int result;

   // Arrange
  ////////////

  const char message[] = "Test passed!";

  Notecard notecard;
  NoteLog_Mock mockLog;  // Instantiate NoteLog (mocked)
  make_note_log_Parameters.reset();
  make_note_log_Parameters.result = &mockLog;  // Return mocked interface
  notecard.setDebugOutputStream(&result);  // Provides access to the hidden static callback methods through `note-c` mocks
  size_t(*noteLogPrint)(const char *) = noteSetFnDebugOutput_Parameters.fn;  // Capture the internal Notecard log function, `noteLogPrint`
  noteLogPrint_Parameters.reset();  // Clear the structure for testing NoteLog::print results
  noteLogPrint_Parameters.result = sizeof(message);

   // Action
  ///////////

  const size_t ACTUAL_RESULT = noteLogPrint(message);

   // Assert
  ///////////

  if (ACTUAL_RESULT == noteLogPrint_Parameters.result)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tACTUAL_RESULT == " << ACTUAL_RESULT << ", EXPECTED: " << noteLogPrint_Parameters.result << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_log_print_does_not_call_interface_method_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  const char message[] = "Test passed!";

  Notecard notecard;

  // Capture the internal Notecard log function, `noteLogPrint`
  make_note_log_Parameters.reset();
  make_note_log_Parameters.result = reinterpret_cast<NoteLog *>(&result);
  notecard.setDebugOutputStream(&result);  // Provides access to the internal static callback methods through `note-c` mocks
  size_t(*noteLogPrint)(const char *) = noteSetFnDebugOutput_Parameters.fn;

  // Ensure interface is not instantiated
  make_note_log_Parameters.reset();
  notecard.setDebugOutputStream(nullptr);  // Provides access to the hidden static callback methods through `note-c` mocks
  noteLogPrint_Parameters.reset();  // Clear the structure for testing NoteLog::print results

   // Action
  ///////////

  noteLogPrint(message);

   // Assert
  ///////////

  if (!noteLogPrint_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteLogPrint_Parameters.invoked == " << noteLogPrint_Parameters.invoked << ", EXPECTED: zero (0)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_log_print_returns_false_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  const char message[] = "Test passed!";

  Notecard notecard;

  // Capture the internal Notecard log function, `noteLogPrint`
  make_note_log_Parameters.reset();
  make_note_log_Parameters.result = reinterpret_cast<NoteLog *>(&result);
  notecard.setDebugOutputStream(&result);  // Provides access to the internal static callback methods through `note-c` mocks
  size_t(*noteLogPrint)(const char *) = noteSetFnDebugOutput_Parameters.fn;

  // Ensure interface is not instantiated
  make_note_log_Parameters.reset();
  notecard.setDebugOutputStream(&result);  // Provides access to the hidden static callback methods through `note-c` mocks
  noteLogPrint_Parameters.reset();  // Clear the structure for testing NoteLog::print results

   // Action
  ///////////

  const size_t ACTUAL_RESULT = noteLogPrint(message);

   // Assert
  ///////////

  if (!ACTUAL_RESULT)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tACTUAL_RESULT == " << ACTUAL_RESULT << ", EXPECTED: zero (0)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_serial_available_invokes_noteserial_available()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Arduino mockSerial_arduino(Serial,9600);  // Instantiate NoteSerial (mocked)
  make_note_serial_Parameters.reset();
  make_note_serial_Parameters.result = &mockSerial_arduino;  // Return mocked interface
  notecard.begin(Serial);  // Provides access to the hidden static callback methods through `note-c` mocks
  noteSerialAvailable_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnSerial_Parameters.availfn();

   // Assert
  ///////////

  if (noteSerialAvailable_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSerialAvailable_Parameters.invoked == " << noteSerialAvailable_Parameters.invoked << ", EXPECTED: > 0" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_serial_available_does_not_modify_interface_method_return_value()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Arduino mockSerial_arduino(Serial,9600);  // Instantiate NoteSerial (mocked)
  make_note_serial_Parameters.reset();
  make_note_serial_Parameters.result = &mockSerial_arduino;  // Return mocked interface
  notecard.begin(Serial);  // Provides access to the hidden static callback methods through `note-c` mocks
  noteSerialAvailable_Parameters.reset();  // Clear the structure for testing results
  noteSerialAvailable_Parameters.result = true;

   // Action
  ///////////

  const bool ACTUAL_RESULT = noteSetFnSerial_Parameters.availfn();

   // Assert
  ///////////

  if (ACTUAL_RESULT == noteSerialAvailable_Parameters.result)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tACTUAL_RESULT == " << ACTUAL_RESULT << ", EXPECTED: " << noteSerialAvailable_Parameters.result << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_serial_available_does_not_call_interface_method_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  make_note_serial_Parameters.reset();
  notecard.begin(Serial);  // Provides access to the hidden static callback methods through `note-c` mocks
  noteSerialAvailable_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnSerial_Parameters.availfn();

   // Assert
  ///////////

  if (!noteSerialAvailable_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSerialAvailable_Parameters.invoked == " << noteSerialAvailable_Parameters.invoked << ", EXPECTED: zero (0)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_serial_available_returns_false_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  make_note_serial_Parameters.reset();
  notecard.begin(Serial);  // Provides access to the hidden static callback methods through `note-c` mocks
  noteSerialAvailable_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  const bool ACTUAL_RESULT = noteSetFnSerial_Parameters.availfn();

   // Assert
  ///////////

  if (!ACTUAL_RESULT)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tACTUAL_RESULT == " << ACTUAL_RESULT << ", EXPECTED: false (0)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_serial_receive_invokes_noteserial_receive()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Arduino mockSerial_arduino(Serial,9600);  // Instantiate NoteSerial (mocked)
  make_note_serial_Parameters.reset();
  make_note_serial_Parameters.result = &mockSerial_arduino;  // Return mocked interface
  notecard.begin(Serial);  // Provides access to the hidden static callback methods through `note-c` mocks
  noteSerialReceive_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnSerial_Parameters.readfn();

   // Assert
  ///////////

  if (noteSerialReceive_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSerialReceive_Parameters.invoked == " << noteSerialReceive_Parameters.invoked << ", EXPECTED: > 0" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_serial_receive_does_not_modify_interface_method_return_value()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Arduino mockSerial_arduino(Serial,9600);  // Instantiate NoteSerial (mocked)
  make_note_serial_Parameters.reset();
  make_note_serial_Parameters.result = &mockSerial_arduino;  // Return mocked interface
  notecard.begin(Serial);  // Provides access to the hidden static callback methods through `note-c` mocks
  noteSerialReceive_Parameters.reset();  // Clear the structure for testing results
  noteSerialReceive_Parameters.result = 'Z';

   // Action
  ///////////

  const char ACTUAL_RESULT = noteSetFnSerial_Parameters.readfn();

   // Assert
  ///////////

  if (ACTUAL_RESULT == noteSerialReceive_Parameters.result)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tACTUAL_RESULT == " << ACTUAL_RESULT << ", EXPECTED: " << noteSerialReceive_Parameters.result << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_serial_receive_does_not_call_interface_method_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  make_note_serial_Parameters.reset();
  notecard.begin(Serial);  // Provides access to the hidden static callback methods through `note-c` mocks
  noteSerialReceive_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnSerial_Parameters.readfn();

   // Assert
  ///////////

  if (!noteSerialReceive_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSerialReceive_Parameters.invoked == " << noteSerialReceive_Parameters.invoked << ", EXPECTED: zero (0)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_serial_receive_returns_false_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  const char EXPECTED_RESULT = '\0';
  Notecard notecard;
  make_note_serial_Parameters.reset();
  notecard.begin(Serial);  // Provides access to the hidden static callback methods through `note-c` mocks
  noteSerialReceive_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  const char ACTUAL_RESULT = noteSetFnSerial_Parameters.readfn();

   // Assert
  ///////////

  if (ACTUAL_RESULT == EXPECTED_RESULT)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tACTUAL_RESULT == " << ACTUAL_RESULT << ", EXPECTED: NULL terminator ('\0')" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_serial_reset_invokes_noteserial_reset()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Arduino mockSerial_arduino(Serial,9600);  // Instantiate NoteSerial (mocked)
  make_note_serial_Parameters.reset();
  make_note_serial_Parameters.result = &mockSerial_arduino;  // Return mocked interface
  notecard.begin(Serial);  // Provides access to the hidden static callback methods through `note-c` mocks
  noteSerialReset_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnSerial_Parameters.resetfn();

   // Assert
  ///////////

  if (noteSerialReset_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSerialReset_Parameters.invoked == " << noteSerialReset_Parameters.invoked << ", EXPECTED: > 0" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_serial_reset_does_not_modify_interface_method_return_value()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Arduino mockSerial_arduino(Serial,9600);  // Instantiate NoteSerial (mocked)
  make_note_serial_Parameters.reset();
  make_note_serial_Parameters.result = &mockSerial_arduino;  // Return mocked interface
  notecard.begin(Serial);  // Provides access to the hidden static callback methods through `note-c` mocks
  noteSerialReset_Parameters.reset();  // Clear the structure for testing results
  noteSerialReset_Parameters.result = true;

   // Action
  ///////////

  const bool ACTUAL_RESULT = noteSetFnSerial_Parameters.resetfn();

   // Assert
  ///////////

  if (ACTUAL_RESULT == noteSerialReset_Parameters.result)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tACTUAL_RESULT == " << ACTUAL_RESULT << ", EXPECTED: " << noteSerialReset_Parameters.result << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_serial_reset_does_not_call_interface_method_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  make_note_serial_Parameters.reset();
  notecard.begin(Serial);  // Provides access to the hidden static callback methods through `note-c` mocks
  noteSerialReset_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnSerial_Parameters.resetfn();

   // Assert
  ///////////

  if (!noteSerialReset_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSerialReset_Parameters.invoked == " << noteSerialReset_Parameters.invoked << ", EXPECTED: zero (0)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_serial_reset_returns_false_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  make_note_serial_Parameters.reset();
  notecard.begin(Serial);  // Provides access to the hidden static callback methods through `note-c` mocks
  noteSerialReset_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  const bool ACTUAL_RESULT = noteSetFnSerial_Parameters.resetfn();

   // Assert
  ///////////

  if (!ACTUAL_RESULT)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tACTUAL_RESULT == " << ACTUAL_RESULT << ", EXPECTED: false (0)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_serial_transmit_invokes_noteserial_transmit()
{
  int result;

   // Arrange
  ////////////

  uint8_t text[] = "Test passed!";
  const size_t LEN = sizeof(text);
  const bool FLUSH = true;

  Notecard notecard;
  NoteSerial_Arduino mockSerial_arduino(Serial,9600);  // Instantiate NoteSerial (mocked)
  make_note_serial_Parameters.reset();
  make_note_serial_Parameters.result = &mockSerial_arduino;  // Return mocked interface
  notecard.begin(Serial);  // Provides access to the hidden static callback methods through `note-c` mocks
  noteSerialTransmit_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnSerial_Parameters.writefn(text, LEN, FLUSH);

   // Assert
  ///////////

  if (noteSerialTransmit_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\noteSerialTransmit_Parameters.invoked == " << noteSerialTransmit_Parameters.invoked << ", EXPECTED: > 0" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_serial_transmit_does_not_modify_text_parameter_before_passing_to_interface_method()
{
  int result;

   // Arrange
  ////////////

  uint8_t expected_text[] = "Test passed!";
  const size_t LEN = sizeof(expected_text);
  const bool FLUSH = true;

  Notecard notecard;
  NoteSerial_Arduino mockSerial_arduino(Serial,9600);  // Instantiate NoteSerial (mocked)
  make_note_serial_Parameters.reset();
  make_note_serial_Parameters.result = &mockSerial_arduino;  // Return mocked interface
  notecard.begin(Serial);  // Provides access to the hidden static callback methods through `note-c` mocks
  noteSerialTransmit_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnSerial_Parameters.writefn(expected_text, LEN, FLUSH);

   // Assert
  ///////////

  if (!strcmp(reinterpret_cast<char *>(expected_text),reinterpret_cast<char *>(noteSerialTransmit_Parameters.buffer)))
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\noteSerialTransmit_Parameters.buffer == " << noteSerialTransmit_Parameters.buffer << ", EXPECTED: " << expected_text << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_serial_transmit_does_not_modify_length_parameter_before_passing_to_interface_method()
{
  int result;

   // Arrange
  ////////////

  uint8_t text[] = "Test passed!";
  const size_t EXPECTED_LEN = sizeof(text);
  const bool FLUSH = true;

  Notecard notecard;
  NoteSerial_Arduino mockSerial_arduino(Serial,9600);  // Instantiate NoteSerial (mocked)
  make_note_serial_Parameters.reset();
  make_note_serial_Parameters.result = &mockSerial_arduino;  // Return mocked interface
  notecard.begin(Serial);  // Provides access to the hidden static callback methods through `note-c` mocks
  noteSerialTransmit_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnSerial_Parameters.writefn(text, EXPECTED_LEN, FLUSH);

   // Assert
  ///////////

  if (EXPECTED_LEN == noteSerialTransmit_Parameters.size)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\noteSerialTransmit_Parameters.size == " << noteSerialTransmit_Parameters.size << ", EXPECTED: " << EXPECTED_LEN << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_serial_transmit_does_not_modify_flush_parameter_before_passing_to_interface_method()
{
  int result;

   // Arrange
  ////////////

  uint8_t text[] = "Test passed!";
  const size_t LEN = sizeof(text);
  const bool EXPECTED_FLUSH = true;

  Notecard notecard;
  NoteSerial_Arduino mockSerial_arduino(Serial,9600);  // Instantiate NoteSerial (mocked)
  make_note_serial_Parameters.reset();
  make_note_serial_Parameters.result = &mockSerial_arduino;  // Return mocked interface
  notecard.begin(Serial);  // Provides access to the hidden static callback methods through `note-c` mocks
  noteSerialTransmit_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnSerial_Parameters.writefn(text, LEN, EXPECTED_FLUSH);

   // Assert
  ///////////

  if (EXPECTED_FLUSH == noteSerialTransmit_Parameters.flush)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\noteSerialTransmit_Parameters.flush == " << noteSerialTransmit_Parameters.flush << ", EXPECTED: " << EXPECTED_FLUSH << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_serial_transmit_does_not_call_interface_method_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  uint8_t text[] = "Test passed!";
  const size_t LEN = sizeof(text);
  const bool EXPECTED_FLUSH = true;

  Notecard notecard;
  make_note_serial_Parameters.reset();
  notecard.begin(Serial);  // Provides access to the hidden static callback methods through `note-c` mocks
  noteSerialTransmit_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSetFnSerial_Parameters.writefn(text, LEN, EXPECTED_FLUSH);

   // Assert
  ///////////

  if (!noteSerialTransmit_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSerialTransmit_Parameters.invoked == " << noteSerialTransmit_Parameters.invoked << ", EXPECTED: zero (0)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int main(void)
{
  TestFunction tests[] = {
      {test_notecard_begin_i2c_shares_a_memory_allocation_functon_pointer, "test_notecard_begin_i2c_shares_a_memory_allocation_functon_pointer"},
      {test_notecard_begin_i2c_shares_a_memory_free_functon_pointer, "test_notecard_begin_i2c_shares_a_memory_free_functon_pointer"},
      {test_notecard_begin_i2c_shares_a_delay_functon_pointer, "test_notecard_begin_i2c_shares_a_delay_functon_pointer"},
      {test_notecard_begin_i2c_shares_a_millis_functon_pointer, "test_notecard_begin_i2c_shares_a_millis_functon_pointer"},
      {test_notecard_begin_i2c_does_not_modify_i2c_address_parameter_before_passing_to_note_c, "test_notecard_begin_i2c_does_not_modify_i2c_address_parameter_before_passing_to_note_c"},
      {test_notecard_begin_i2c_does_not_modify_i2c_max_parameter_before_passing_to_note_c, "test_notecard_begin_i2c_does_not_modify_i2c_max_parameter_before_passing_to_note_c"},
      {test_notecard_begin_i2c_shares_an_i2c_reset_functon_pointer, "test_notecard_begin_i2c_shares_an_i2c_reset_functon_pointer"},
      {test_notecard_begin_i2c_shares_an_i2c_transmit_functon_pointer, "test_notecard_begin_i2c_shares_an_i2c_transmit_functon_pointer"},
      {test_notecard_begin_i2c_shares_an_i2c_receive_functon_pointer, "test_notecard_begin_i2c_shares_an_i2c_receive_functon_pointer"},
      {test_notecard_begin_i2c_default_parameter_for_i2c_max_is_passed_to_note_c, "test_notecard_begin_i2c_default_parameter_for_i2c_max_is_passed_to_note_c"},
      {test_notecard_begin_i2c_default_parameter_for_i2c_address_is_passed_to_note_c, "test_notecard_begin_i2c_default_parameter_for_i2c_address_is_passed_to_note_c"},
      {test_notecard_begin_i2c_default_parameter_for_wirePort_instantiates_the_note_i2c_interface_with_wire, "test_notecard_begin_i2c_default_parameter_for_wirePort_instantiates_the_note_i2c_interface_with_wire"},
      {test_notecard_begin_i2c_parameter_for_wirePort_instantiates_the_note_i2c_interface_with_wirePort, "test_notecard_begin_i2c_parameter_for_wirePort_instantiates_the_note_i2c_interface_with_wirePort"},
      {test_notecard_begin_i2c_sets_user_agent_to_note_arduino, "test_notecard_begin_i2c_sets_user_agent_to_note_arduino"},
      {test_notecard_begin_serial_shares_a_memory_allocation_functon_pointer, "test_notecard_begin_serial_shares_a_memory_allocation_functon_pointer"},
      {test_notecard_begin_serial_shares_a_memory_free_functon_pointer, "test_notecard_begin_serial_shares_a_memory_free_functon_pointer"},
      {test_notecard_begin_serial_shares_a_delay_functon_pointer, "test_notecard_begin_serial_shares_a_delay_functon_pointer"},
      {test_notecard_begin_serial_shares_a_millis_functon_pointer, "test_notecard_begin_serial_shares_a_millis_functon_pointer"},
      {test_notecard_begin_serial_shares_a_serial_reset_functon_pointer, "test_notecard_begin_serial_shares_a_serial_reset_functon_pointer"},
      {test_notecard_begin_serial_shares_a_serial_transmit_functon_pointer, "test_notecard_begin_serial_shares_a_serial_transmit_functon_pointer"},
      {test_notecard_begin_serial_shares_a_serial_available_functon_pointer, "test_notecard_begin_serial_shares_a_serial_available_functon_pointer"},
      {test_notecard_begin_serial_shares_a_serial_receive_functon_pointer, "test_notecard_begin_serial_shares_a_serial_receive_functon_pointer"},
      {test_notecard_begin_serial_initializes_the_provided_serial_interface_with_provided_speed, "test_notecard_begin_serial_initializes_the_provided_serial_interface_with_provided_speed"},
      {test_notecard_begin_serial_initializes_the_provided_serial_interface_with_default_speed, "test_notecard_begin_serial_initializes_the_provided_serial_interface_with_default_speed"},
      {test_notecard_begin_serial_sets_user_agent_to_note_arduino, "test_notecard_begin_serial_sets_user_agent_to_note_arduino"},
      {test_notecard_setDebugOutputStream_does_not_modify_log_channel_parameter_before_passing_to_make_note_log, "test_notecard_setDebugOutputStream_does_not_modify_log_channel_parameter_before_passing_to_make_note_log"},
      {test_notecard_setDebugOutputStream_shares_a_debug_log_functon_pointer, "test_notecard_setDebugOutputStream_shares_a_debug_log_functon_pointer"},
      {test_notecard_setDebugOutputStream_clears_the_debug_log_functon_pointer_when_nullptr_is_provided, "test_notecard_setDebugOutputStream_clears_the_debug_log_functon_pointer_when_nullptr_is_provided"},
      {test_notecard_clearDebugOutputStream_clears_the_debug_log_functon_pointer, "test_notecard_clearDebugOutputStream_clears_the_debug_log_functon_pointer"},
      {test_notecard_newRequest_does_not_modify_string_parameter_value_before_passing_to_note_c, "test_notecard_newRequest_does_not_modify_string_parameter_value_before_passing_to_note_c"},
      {test_notecard_newRequest_does_not_modify_note_c_result_value_before_returning_to_caller, "test_notecard_newRequest_does_not_modify_note_c_result_value_before_returning_to_caller"},
      {test_notecard_sendRequest_does_not_modify_j_object_parameter_value_before_passing_to_note_c, "test_notecard_sendRequest_does_not_modify_j_object_parameter_value_before_passing_to_note_c"},
      {test_notecard_sendRequest_does_not_modify_note_c_result_value_before_returning_to_caller, "test_notecard_sendRequest_does_not_modify_note_c_result_value_before_returning_to_caller"},
      {test_notecard_requestAndResponse_does_not_modify_j_object_parameter_value_before_passing_to_note_c, "test_notecard_requestAndResponse_does_not_modify_j_object_parameter_value_before_passing_to_note_c"},
      {test_notecard_requestAndResponse_does_not_modify_note_c_result_value_before_returning_to_caller, "test_notecard_requestAndResponse_does_not_modify_note_c_result_value_before_returning_to_caller"},
      {test_notecard_deleteResponse_does_not_modify_j_object_parameter_pointer_before_passing_to_note_c, "test_notecard_deleteResponse_does_not_modify_j_object_parameter_pointer_before_passing_to_note_c"},
      {test_notecard_logDebug_does_not_modify_string_parameter_value_before_passing_to_note_c, "test_notecard_logDebug_does_not_modify_string_parameter_value_before_passing_to_note_c"},
      {test_notecard_logDebugf_does_not_modify_string_parameter_value_before_passing_to_note_c, "test_notecard_logDebugf_does_not_modify_string_parameter_value_before_passing_to_note_c"},
      {test_notecard_debugSyncStatus_does_not_modify_pollFrequencyMs_parameter_before_passing_to_note_c, "test_notecard_debugSyncStatus_does_not_modify_pollFrequencyMs_parameter_before_passing_to_note_c"},
      {test_notecard_debugSyncStatus_does_not_modify_maxLevel_parameter_before_passing_to_note_c, "test_notecard_debugSyncStatus_does_not_modify_maxLevel_parameter_before_passing_to_note_c"},
      {test_notecard_debugSyncStatus_does_not_modify_note_c_result_value_before_returning_to_caller, "test_notecard_debugSyncStatus_does_not_modify_note_c_result_value_before_returning_to_caller"},
      {test_notecard_responseError_does_not_modify_j_object_parameter_value_before_passing_to_note_c, "test_notecard_responseError_does_not_modify_j_object_parameter_value_before_passing_to_note_c"},
      {test_notecard_responseError_does_not_modify_note_c_result_value_before_returning_to_caller, "test_notecard_responseError_does_not_modify_note_c_result_value_before_returning_to_caller"},
      {test_notecard_newCommand_does_not_modify_string_parameter_value_before_passing_to_note_c, "test_notecard_newRequest_does_not_modify_string_parameter_value_before_passing_to_note_c"},
      {test_notecard_newCommand_does_not_modify_note_c_result_value_before_returning_to_caller, "test_notecard_newRequest_does_not_modify_note_c_result_value_before_returning_to_caller"},
      {test_static_callback_note_i2c_receive_invokes_notei2c_receive, "test_static_callback_note_i2c_receive_invokes_notei2c_receive"},
      {test_static_callback_note_i2c_receive_does_not_modify_device_address_parameter_before_passing_to_interface_method, "test_static_callback_note_i2c_receive_does_not_modify_device_address_parameter_before_passing_to_interface_method"},
      {test_static_callback_note_i2c_receive_does_not_modify_buffer_parameter_address_before_passing_to_interface_method, "test_static_callback_note_i2c_receive_does_not_modify_buffer_parameter_address_before_passing_to_interface_method"},
      {test_static_callback_note_i2c_receive_does_not_modify_size_parameter_before_passing_to_interface_method, "test_static_callback_note_i2c_receive_does_not_modify_size_parameter_before_passing_to_interface_method"},
      {test_static_callback_note_i2c_receive_does_not_modify_available_parameter_address_before_passing_to_interface_method, "test_static_callback_note_i2c_receive_does_not_modify_available_parameter_address_before_passing_to_interface_method"},
      {test_static_callback_note_i2c_receive_does_not_modify_interface_method_return_value, "test_static_callback_note_i2c_receive_does_not_modify_interface_method_return_value"},
      {test_static_callback_note_i2c_receive_does_not_call_interface_method_when_interface_has_not_been_instantiated, "test_static_callback_note_i2c_receive_does_not_call_interface_method_when_interface_has_not_been_instantiated"},
      {test_static_callback_note_i2c_receive_returns_error_message_when_interface_has_not_been_instantiated, "test_static_callback_note_i2c_receive_returns_error_message_when_interface_has_not_been_instantiated"},
      {test_static_callback_note_i2c_reset_invokes_notei2c_reset, "test_static_callback_note_i2c_reset_invokes_notei2c_reset"},
      {test_static_callback_note_i2c_reset_does_not_modify_device_address_parameter_before_passing_to_interface_method, "test_static_callback_note_i2c_reset_does_not_modify_device_address_parameter_before_passing_to_interface_method"},
      {test_static_callback_note_i2c_reset_does_not_modify_interface_method_return_value, "test_static_callback_note_i2c_reset_does_not_modify_interface_method_return_value"},
      {test_static_callback_note_i2c_reset_does_not_call_interface_method_when_interface_has_not_been_instantiated, "test_static_callback_note_i2c_reset_does_not_call_interface_method_when_interface_has_not_been_instantiated"},
      {test_static_callback_note_i2c_reset_returns_false_when_interface_has_not_been_instantiated, "test_static_callback_note_i2c_reset_returns_false_when_interface_has_not_been_instantiated"},
      {test_static_callback_note_i2c_transmit_invokes_notei2c_transmit, "test_static_callback_note_i2c_transmit_invokes_notei2c_transmit"},
      {test_static_callback_note_i2c_transmit_does_not_modify_device_address_parameter_before_passing_to_interface_method, "test_static_callback_note_i2c_transmit_does_not_modify_device_address_parameter_before_passing_to_interface_method"},
      {test_static_callback_note_i2c_transmit_does_not_modify_buffer_parameter_before_passing_to_interface_method, "test_static_callback_note_i2c_transmit_does_not_modify_buffer_parameter_before_passing_to_interface_method"},
      {test_static_callback_note_i2c_transmit_does_not_modify_interface_method_return_value, "test_static_callback_note_i2c_transmit_does_not_modify_interface_method_return_value"},
      {test_static_callback_note_i2c_transmit_does_not_modify_size_parameter_before_passing_to_interface_method, "test_static_callback_note_i2c_transmit_does_not_modify_size_parameter_before_passing_to_interface_method"},
      {test_static_callback_note_i2c_transmit_does_not_call_interface_method_when_interface_has_not_been_instantiated, "test_static_callback_note_i2c_transmit_does_not_call_interface_method_when_interface_has_not_been_instantiated"},
      {test_static_callback_note_i2c_transmit_returns_error_message_when_interface_has_not_been_instantiated, "test_static_callback_note_i2c_transmit_returns_error_message_when_interface_has_not_been_instantiated"},
      {test_static_callback_note_log_print_invokes_notelog_print, "test_static_callback_note_log_print_invokes_notelog_print"},
      {test_static_callback_note_log_print_does_not_modify_message_parameter_before_passing_to_interface_method, "test_static_callback_note_log_print_does_not_modify_message_parameter_before_passing_to_interface_method"},
      {test_static_callback_note_log_print_does_not_modify_interface_method_return_value, "test_static_callback_note_log_print_does_not_modify_interface_method_return_value"},
      {test_static_callback_note_log_print_does_not_call_interface_method_when_interface_has_not_been_instantiated, "test_static_callback_note_log_print_does_not_call_interface_method_when_interface_has_not_been_instantiated"},
      {test_static_callback_note_log_print_returns_false_when_interface_has_not_been_instantiated, "test_static_callback_note_log_print_returns_false_when_interface_has_not_been_instantiated"},
      {test_static_callback_note_serial_available_invokes_noteserial_available, "test_static_callback_note_serial_available_invokes_noteserial_available"},
      {test_static_callback_note_serial_available_does_not_modify_interface_method_return_value, "test_static_callback_note_serial_available_does_not_modify_interface_method_return_value"},
      {test_static_callback_note_serial_available_does_not_call_interface_method_when_interface_has_not_been_instantiated, "test_static_callback_note_serial_available_does_not_call_interface_method_when_interface_has_not_been_instantiated"},
      {test_static_callback_note_serial_available_returns_false_when_interface_has_not_been_instantiated, "test_static_callback_note_serial_available_returns_false_when_interface_has_not_been_instantiated"},
      {test_static_callback_note_serial_receive_invokes_noteserial_receive, "test_static_callback_note_serial_receive_invokes_noteserial_receive"},
      {test_static_callback_note_serial_receive_does_not_modify_interface_method_return_value, "test_static_callback_note_serial_receive_does_not_modify_interface_method_return_value"},
      {test_static_callback_note_serial_receive_does_not_call_interface_method_when_interface_has_not_been_instantiated, "test_static_callback_note_serial_receive_does_not_call_interface_method_when_interface_has_not_been_instantiated"},
      {test_static_callback_note_serial_receive_returns_false_when_interface_has_not_been_instantiated, "test_static_callback_note_serial_receive_returns_false_when_interface_has_not_been_instantiated"},
      {test_static_callback_note_serial_reset_invokes_noteserial_reset, "test_static_callback_note_serial_reset_invokes_noteserial_reset"},
      {test_static_callback_note_serial_reset_does_not_modify_interface_method_return_value, "test_static_callback_note_serial_reset_does_not_modify_interface_method_return_value"},
      {test_static_callback_note_serial_reset_does_not_call_interface_method_when_interface_has_not_been_instantiated, "test_static_callback_note_serial_reset_does_not_call_interface_method_when_interface_has_not_been_instantiated"},
      {test_static_callback_note_serial_reset_returns_false_when_interface_has_not_been_instantiated, "test_static_callback_note_serial_reset_returns_false_when_interface_has_not_been_instantiated"},
      {test_static_callback_note_serial_transmit_invokes_noteserial_transmit, "test_static_callback_note_serial_transmit_invokes_noteserial_transmit"},
      {test_static_callback_note_serial_transmit_does_not_modify_text_parameter_before_passing_to_interface_method, "test_static_callback_note_serial_transmit_does_not_modify_text_parameter_before_passing_to_interface_method"},
      {test_static_callback_note_serial_transmit_does_not_modify_length_parameter_before_passing_to_interface_method, "test_static_callback_note_serial_transmit_does_not_modify_length_parameter_before_passing_to_interface_method"},
      {test_static_callback_note_serial_transmit_does_not_modify_flush_parameter_before_passing_to_interface_method, "test_static_callback_note_serial_transmit_does_not_modify_flush_parameter_before_passing_to_interface_method"},
      {test_static_callback_note_serial_transmit_does_not_call_interface_method_when_interface_has_not_been_instantiated, "test_static_callback_note_serial_transmit_does_not_call_interface_method_when_interface_has_not_been_instantiated"},
  };

  return TestFunction::runTests(tests, (sizeof(tests) / sizeof(TestFunction)));
}
