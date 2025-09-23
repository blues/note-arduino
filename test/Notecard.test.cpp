#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "TestFunction.hpp"

#include "Notecard.h"
#include "mock/mock-parameters.hpp"
#include "mock/NoteI2c_Mock.hpp"
#include "mock/NoteLog_Mock.hpp"
#include "mock/NoteSerial_Mock.hpp"
#include "mock/NoteTxn_Mock.hpp"

// Compile command: g++ -Wall -Wextra -Wpedantic mock/mock-arduino.cpp mock/mock-note-c-note.c mock/NoteI2c_Mock.cpp mock/NoteLog_Mock.cpp mock/NoteSerial_Mock.cpp mock/NoteTime_Mock.cpp mock/NoteTxn_Mock.cpp ../src/Notecard.cpp Notecard.test.cpp -std=c++11 -I. -I../src -DNOTE_MOCK -ggdb -O0 -o notecard.tests && ./notecard.tests || echo "Tests Result: $?"

int test_notecard_begin_i2c_sets_user_agent_to_note_arduino()
{
  int result;

   // Arrange
  ////////////

  static const char * const EXPECTED_USER_AGENT = "note-arduino " NOTE_ARDUINO_VERSION;
  Notecard notecard;
  NoteI2c_Mock mockI2c;

  noteSetUserAgent_Parameters.reset();

   // Action
  ///////////

  notecard.begin(&mockI2c);

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
    std::cout << "\tnoteSetUserAgent_Parameters.agent == " << noteSetUserAgent_Parameters.agent_cache.c_str() << ", EXPECTED: " << EXPECTED_USER_AGENT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_sets_user_agent_to_note_arduino_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  static const char * const EXPECTED_USER_AGENT = "note-arduino " NOTE_ARDUINO_VERSION;
  Notecard notecard;
  noteSetUserAgent_Parameters.reset();

   // Action
  ///////////

  notecard.begin(static_cast<NoteI2c *>(nullptr));

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
    std::cout << "\tnoteSetUserAgent_Parameters.agent == " << noteSetUserAgent_Parameters.agent_cache.c_str() << ", EXPECTED: " << EXPECTED_USER_AGENT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_shares_a_memory_allocation_function_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteI2c_Mock mockI2c;
  noteSetFnDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(&mockI2c);

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
    std::cout << "\tnoteSetFnDefault_Parameters.mallocfn == 0x" << std::hex << !!noteSetFnDefault_Parameters.mallocfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_sets_memory_allocation_function_pointer_to_nullptr_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  noteSetFnDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(static_cast<NoteI2c *>(nullptr));

   // Assert
  ///////////

  if (!noteSetFnDefault_Parameters.mallocfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDefault_Parameters.mallocfn == 0x" << std::hex << noteSetFnDefault_Parameters.mallocfn << ", EXPECTED: 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_shares_a_memory_free_function_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteI2c_Mock mockI2c;
  noteSetFnDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(&mockI2c);

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
    std::cout << "\tnoteSetFnDefault_Parameters.freefn == 0x" << std::hex << !!noteSetFnDefault_Parameters.freefn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_sets_memory_free_function_pointer_to_nullptr_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  noteSetFnDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(static_cast<NoteI2c *>(nullptr));

   // Assert
  ///////////

  if (!noteSetFnDefault_Parameters.freefn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDefault_Parameters.freefn == 0x" << std::hex << noteSetFnDefault_Parameters.freefn << ", EXPECTED: 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_shares_a_delay_function_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteI2c_Mock mockI2c;
  noteSetFnDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(&mockI2c);

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
    std::cout << "\tnoteSetFnDefault_Parameters.delayfn == 0x" << std::hex << !!noteSetFnDefault_Parameters.delayfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_sets_delay_function_pointer_to_nullptr_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  noteSetFnDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(static_cast<NoteI2c *>(nullptr));

   // Arrange
  ////////////

  if (!noteSetFnDefault_Parameters.delayfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDefault_Parameters.delayfn == 0x" << std::hex << noteSetFnDefault_Parameters.delayfn << ", EXPECTED: 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_shares_a_millis_function_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteI2c_Mock mockI2c;
  noteSetFnDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(&mockI2c);

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
    std::cout << "\tnoteSetFnDefault_Parameters.millisfn == 0x" << std::hex << !!noteSetFnDefault_Parameters.millisfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_sets_millis_function_pointer_to_nullptr_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  noteSetFnDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(static_cast<NoteI2c *>(nullptr));

   // Assert
  ///////////

  if (!noteSetFnDefault_Parameters.millisfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDefault_Parameters.millisfn == 0x" << std::hex << noteSetFnDefault_Parameters.millisfn << ", EXPECTED: 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_does_not_modify_i2c_address_parameter_before_passing_to_note_c()
{
  int result;

   // Arrange
  ////////////

  // 0x1B binary representation => 0001 1011
  const uint16_t EXPECTED_ADDRESS = 0x1B;

  Notecard notecard;
  NoteI2c_Mock mockI2c;
  noteSetFnI2CDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(&mockI2c, EXPECTED_ADDRESS);

   // Assert
  ///////////

  if (EXPECTED_ADDRESS == noteSetFnI2CDefault_Parameters.i2caddr)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2CDefault_Parameters.i2caddr == 0x" << std::hex << noteSetFnI2CDefault_Parameters.i2caddr << ", EXPECTED: " << EXPECTED_ADDRESS << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_sets_i2c_address_parameter_to_zero_before_passing_to_note_c_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  // 0x1B binary representation => 0001 1011
  const uint16_t SUPPLIED_ADDRESS = 0x1B;

  Notecard notecard;
  noteSetFnI2C_Parameters.reset();
  noteSetFnI2C_Parameters.i2caddr = NOTE_I2C_ADDR_DEFAULT;

   // Action
  ///////////

  notecard.begin(static_cast<NoteI2c *>(nullptr), SUPPLIED_ADDRESS);

   // Assert
  ///////////

  if (!noteSetFnI2C_Parameters.i2caddr)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2C_Parameters.i2caddr == 0x" << std::hex << noteSetFnI2C_Parameters.i2caddr << ", EXPECTED: 0x00" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_does_not_modify_i2c_mtu_parameter_before_passing_to_note_c()
{
  int result;

   // Arrange
  ////////////

  const uint32_t EXPECTED_RESULT = 79;

  Notecard notecard;
  NoteI2c_Mock mockI2c;
  noteSetFnI2CDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(&mockI2c, 0x1B, EXPECTED_RESULT);

   // Assert
  ///////////

  if (EXPECTED_RESULT == noteSetFnI2CDefault_Parameters.i2cmtu)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2CDefault_Parameters.i2cmtu == " << noteSetFnI2CDefault_Parameters.i2cmtu << ", EXPECTED: " << EXPECTED_RESULT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_sets_i2c_mtu_parameter_to_zero_before_passing_to_note_c_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  const uint32_t SUPPLIED_VALUE = 79;

  Notecard notecard;
  noteSetFnI2C_Parameters.reset();
  noteSetFnI2C_Parameters.i2cmtu = NOTE_I2C_MAX_DEFAULT;

   // Action
  ///////////

  notecard.begin(static_cast<NoteI2c *>(nullptr), 0x1B, SUPPLIED_VALUE);

   // Assert
  ///////////

  if (!noteSetFnI2C_Parameters.i2cmtu)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2C_Parameters.i2cmtu == " << noteSetFnI2C_Parameters.i2cmtu << ", EXPECTED: 0" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_shares_an_i2c_reset_function_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteI2c_Mock mockI2c;
  noteSetFnI2CDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(&mockI2c);

   // Assert
  ///////////

  if (noteSetFnI2CDefault_Parameters.resetfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2CDefault_Parameters.resetfn == 0x" << std::hex << !!noteSetFnI2CDefault_Parameters.resetfn << ", EXPECTED: not 0x0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_sets_i2c_reset_function_pointer_to_nullptr_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  noteSetFnI2C_Parameters.reset();
  noteSetFnI2C_Parameters.resetfn = reinterpret_cast<i2cResetFn>(0x19790917);

   // Action
  ///////////

  notecard.begin(static_cast<NoteI2c *>(nullptr));

   // Assert
  ///////////

  if (!noteSetFnI2C_Parameters.resetfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2C_Parameters.resetfn == 0x" << std::hex << noteSetFnI2C_Parameters.resetfn << ", EXPECTED: 0x0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_shares_an_i2c_transmit_function_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteI2c_Mock mockI2c;
  noteSetFnI2CDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(&mockI2c);

   // Assert
  ///////////

  if (noteSetFnI2CDefault_Parameters.transmitfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2CDefault_Parameters.transmitfn == 0x" << std::hex << !!noteSetFnI2CDefault_Parameters.transmitfn << ", EXPECTED: not 0x0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_sets_i2c_transmit_function_pointer_to_nullptr_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  noteSetFnI2C_Parameters.reset();
  noteSetFnI2C_Parameters.transmitfn = reinterpret_cast<i2cTransmitFn>(0x19790917);

   // Action
  ///////////

  notecard.begin(static_cast<NoteI2c *>(nullptr));

   // Assert
  ///////////

  if (!noteSetFnI2C_Parameters.transmitfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2C_Parameters.transmitfn == 0x" << std::hex << noteSetFnI2C_Parameters.transmitfn << ", EXPECTED: 0x0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_shares_an_i2c_receive_function_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteI2c_Mock mockI2c;
  noteSetFnI2CDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(&mockI2c);

   // Assert
  ///////////

  if (noteSetFnI2CDefault_Parameters.receivefn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2CDefault_Parameters.receivefn == 0x" << std::hex << !!noteSetFnI2CDefault_Parameters.receivefn << ", EXPECTED: not 0x0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_sets_i2c_receive_function_pointer_to_nullptr_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  noteSetFnI2C_Parameters.reset();
  noteSetFnI2C_Parameters.receivefn = reinterpret_cast<i2cReceiveFn>(0x19790917);

   // Action
  ///////////

  notecard.begin(static_cast<NoteI2c *>(nullptr));

   // Assert
  ///////////

  if (!noteSetFnI2C_Parameters.receivefn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2C_Parameters.receivefn == 0x" << std::hex << noteSetFnI2C_Parameters.receivefn << ", EXPECTED: 0x0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_default_parameter_for_i2cMax_is_passed_to_note_c()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteI2c_Mock mockI2c;
  noteSetFnI2CDefault_Parameters.reset();
  noteSetFnI2CDefault_Parameters.i2cmtu = (NOTE_I2C_MAX_DEFAULT - 1);

   // Action
  ///////////

  notecard.begin(&mockI2c, 0x1B);

   // Assert
  ///////////

  if (NOTE_I2C_MAX_DEFAULT == noteSetFnI2CDefault_Parameters.i2cmtu)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2CDefault_Parameters.i2cmtu == " << noteSetFnI2CDefault_Parameters.i2cmtu << ", EXPECTED: " << NOTE_I2C_MAX_DEFAULT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_default_parameter_for_i2cAddress_is_passed_to_note_c()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteI2c_Mock mockI2c;
  noteSetFnI2CDefault_Parameters.reset();
  noteSetFnI2CDefault_Parameters.i2caddr = (NOTE_I2C_ADDR_DEFAULT - 1);

   // Action
  ///////////

  notecard.begin(&mockI2c);

   // Assert
  ///////////

  if (NOTE_I2C_ADDR_DEFAULT == noteSetFnI2CDefault_Parameters.i2caddr)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2CDefault_Parameters.i2caddr == 0x" << std::hex << noteSetFnI2CDefault_Parameters.i2caddr << ", EXPECTED: 0x" << std::hex << NOTE_I2C_ADDR_DEFAULT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_sets_user_agent_to_note_arduino()
{
  int result;

   // Arrange
  ////////////

  static const char * const EXPECTED_USER_AGENT = "note-arduino " NOTE_ARDUINO_VERSION;
  Notecard notecard;
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  noteSetUserAgent_Parameters.reset();

   // Action
  ///////////

  notecard.begin(&mockSerial);

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
    std::cout << "\tnoteSetUserAgent_Parameters.agent == " << noteSetUserAgent_Parameters.agent_cache.c_str() << ", EXPECTED: " << EXPECTED_USER_AGENT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_sets_user_agent_to_note_arduino_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  static const char * const EXPECTED_USER_AGENT = "note-arduino " NOTE_ARDUINO_VERSION;
  Notecard notecard;
  noteSetUserAgent_Parameters.reset();

   // Action
  ///////////

  notecard.begin(static_cast<NoteSerial *>(nullptr));

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
    std::cout << "\tnoteSetUserAgent_Parameters.agent == " << noteSetUserAgent_Parameters.agent_cache.c_str() << ", EXPECTED: " << EXPECTED_USER_AGENT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_shares_a_memory_allocation_function_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  noteSetFnDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(&mockSerial);

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

int test_notecard_begin_serial_sets_memory_allocation_function_pointer_to_nullptr_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  noteSetFnDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(static_cast<NoteSerial *>(nullptr));

   // Assert
  ///////////

  if (!noteSetFnDefault_Parameters.mallocfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDefault_Parameters.mallocfn == 0x" << std::hex << noteSetFnDefault_Parameters.mallocfn << ", EXPECTED: 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_shares_a_memory_free_function_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  noteSetFnDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(&mockSerial);

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

int test_notecard_begin_serial_sets_memory_free_function_pointer_to_nullptr_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  noteSetFnDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(static_cast<NoteSerial *>(nullptr));

   // Assert
  ///////////

  if (!noteSetFnDefault_Parameters.freefn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDefault_Parameters.freefn == 0x" << std::hex << noteSetFnDefault_Parameters.freefn << ", EXPECTED: 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_shares_a_delay_function_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  noteSetFnDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(&mockSerial);

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

int test_notecard_begin_serial_sets_delay_function_pointer_to_nullptr_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  noteSetFnDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(static_cast<NoteSerial *>(nullptr));

   // Assert
  ///////////

  if (!noteSetFnDefault_Parameters.delayfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDefault_Parameters.delayfn == 0x" << std::hex << noteSetFnDefault_Parameters.delayfn << ", EXPECTED: 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_shares_a_millis_function_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  noteSetFnDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(&mockSerial);

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

int test_notecard_begin_serial_sets_millis_function_pointer_to_nullptr_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  noteSetFnDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(static_cast<NoteSerial *>(nullptr));

   // Assert
  ///////////

  if (!noteSetFnDefault_Parameters.millisfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDefault_Parameters.millisfn == 0x" << std::hex << noteSetFnDefault_Parameters.millisfn << ", EXPECTED: 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_shares_a_serial_reset_function_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  noteSetFnSerialDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(&mockSerial);

   // Assert
  ///////////

  if (noteSetFnSerialDefault_Parameters.resetfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnSerialDefault_Parameters.resetfn == " << !!noteSetFnSerialDefault_Parameters.resetfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_sets_serial_reset_function_pointer_to_nullptr_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  noteSetFnSerial_Parameters.reset();
  noteSetFnSerial_Parameters.resetfn = reinterpret_cast<serialResetFn>(0x19790917);

   // Action
  ///////////

  notecard.begin(static_cast<NoteSerial *>(nullptr));

   // Assert
  ///////////

  if (!noteSetFnSerial_Parameters.resetfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnSerial_Parameters.resetfn == 0x" << std::hex << noteSetFnSerial_Parameters.resetfn << ", EXPECTED: 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_shares_a_serial_transmit_function_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  noteSetFnSerialDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(&mockSerial);

   // Assert
  ///////////

  if (noteSetFnSerialDefault_Parameters.writefn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnSerialDefault_Parameters.writefn == " << !!noteSetFnSerialDefault_Parameters.writefn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_sets_transmit_function_pointer_to_nullptr_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  noteSetFnSerial_Parameters.reset();
  noteSetFnSerial_Parameters.writefn = reinterpret_cast<serialTransmitFn>(0x19790917);

   // Action
  ///////////

  notecard.begin(static_cast<NoteSerial *>(nullptr));

   // Assert
  ///////////

  if (!noteSetFnSerial_Parameters.writefn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnSerial_Parameters.writefn == 0x" << std::hex << noteSetFnSerial_Parameters.writefn << ", EXPECTED: 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_shares_a_serial_available_function_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  noteSetFnSerialDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(&mockSerial);

   // Assert
  ///////////

  if (noteSetFnSerialDefault_Parameters.availfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnSerialDefault_Parameters.availfn == " << !!noteSetFnSerialDefault_Parameters.availfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_sets_serial_available_function_pointer_to_nullptr_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  noteSetFnSerial_Parameters.reset();
  noteSetFnSerial_Parameters.availfn = reinterpret_cast<serialAvailableFn>(0x19790917);

   // Action
  ///////////

  notecard.begin(static_cast<NoteSerial *>(nullptr));

   // Assert
  ///////////

  if (!noteSetFnSerial_Parameters.availfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnSerial_Parameters.availfn == 0x" << std::hex << noteSetFnSerial_Parameters.availfn << ", EXPECTED: 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_shares_a_serial_receive_function_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  noteSetFnSerialDefault_Parameters.reset();

   // Action
  ///////////

  notecard.begin(&mockSerial);

   // Assert
  ///////////

  if (noteSetFnSerialDefault_Parameters.readfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnSerialDefault_Parameters.readfn == " << !!noteSetFnSerialDefault_Parameters.readfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_sets_serial_receive_function_pointer_to_nullptr_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  noteSetFnSerial_Parameters.reset();
  noteSetFnSerial_Parameters.readfn = reinterpret_cast<serialReceiveFn>(0x19790917);

   // Action
  ///////////

  notecard.begin(static_cast<NoteSerial *>(nullptr));

   // Assert
  ///////////

  if (!noteSetFnSerial_Parameters.readfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnSerial_Parameters.readfn == 0x" << std::hex << noteSetFnSerial_Parameters.readfn << ", EXPECTED: 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_end_does_not_call_wire_end_when_the_i2c_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Mock mockSerial;
  noteSetFnSerial_Parameters.reset();
  twoWireEnd_Parameters.reset();
  notecard.begin(&mockSerial);
  noteGetFnI2C_Parameters.reset();
  noteGetFnSerial_Parameters.reset();
  make_note_i2c_Parameters.reset();

   // Action
  ///////////

  notecard.end();

   // Assert
  ///////////

  if (!make_note_i2c_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\ttwoWireEnd_Parameters.invoked == " << !!twoWireEnd_Parameters.invoked << ", 0 (false)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_end_clears_all_i2c_interface_function_pointers_when_the_i2c_interface_has_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteI2c_Mock mockI2c;
  noteSetFnI2C_Parameters.reset();
  notecard.begin(&mockI2c);
  noteGetFnSerial_Parameters.reset();
  noteGetFnI2C_Parameters.reset();
  noteGetFnI2C_Parameters.transmitFn_result = reinterpret_cast<i2cTransmitFn>(0x1);

   // Action
  ///////////

  notecard.end();

   // Assert
  ///////////

  if (!noteSetFnI2C_Parameters.resetfn
   && !noteSetFnI2C_Parameters.transmitfn
   && !noteSetFnI2C_Parameters.receivefn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2C_Parameters.resetfn    == 0x" << std::hex << !!noteSetFnI2C_Parameters.resetfn    << ", EXPECTED: 0x0 (`nullptr`)" << std::endl;
    std::cout << "\tnoteSetFnI2C_Parameters.transmitfn == 0x" << std::hex << !!noteSetFnI2C_Parameters.transmitfn << ", EXPECTED: 0x0 (`nullptr`)" << std::endl;
    std::cout << "\tnoteSetFnI2C_Parameters.receivefn  == 0x" << std::hex << !!noteSetFnI2C_Parameters.receivefn  << ", EXPECTED: 0x0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_end_does_not_call_serial_end_when_the_serial_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Mock mockI2c;
  noteSetFnDefault_Parameters.reset();
  notecard.begin(&mockI2c);
  hardwareSerialEnd_Parameters.reset();
  noteGetFnI2C_Parameters.reset();
  noteGetFnSerial_Parameters.reset();
  make_note_serial_Parameters.reset();

   // Action
  ///////////

  notecard.end();

   // Assert
  ///////////

  if (!make_note_serial_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\thardwareSerialEnd_Parameters.invoked == " << !!hardwareSerialEnd_Parameters.invoked  << ", EXPECTED: 0 (false)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_end_clears_all_serial_interface_function_pointers_when_the_serial_interface_has_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  noteSetFnDefault_Parameters.reset();
  notecard.begin(&mockSerial);
  noteGetFnI2C_Parameters.reset();
  noteGetFnSerial_Parameters.reset();
  noteGetFnSerial_Parameters.transmitFn_result = reinterpret_cast<serialTransmitFn>(0x1);

   // Action
  ///////////

  notecard.end();

   // Assert
  ///////////

  if (!noteSetFnSerial_Parameters.resetfn
   && !noteSetFnSerial_Parameters.writefn
   && !noteSetFnSerial_Parameters.availfn
   && !noteSetFnSerial_Parameters.readfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnSerial_Parameters.readfn  == 0x" << !!noteSetFnSerial_Parameters.readfn  << ", EXPECTED: 0x0 (`nullptr`)" << std::endl;
    std::cout << "\tnoteSetFnSerial_Parameters.writefn == 0x" << !!noteSetFnSerial_Parameters.writefn << ", EXPECTED: 0x0 (`nullptr`)" << std::endl;
    std::cout << "\tnoteSetFnSerial_Parameters.availfn == 0x" << !!noteSetFnSerial_Parameters.availfn << ", EXPECTED: 0x0 (`nullptr`)" << std::endl;
    std::cout << "\tnoteSetFnSerial_Parameters.resetfn == 0x" << !!noteSetFnSerial_Parameters.resetfn << ", EXPECTED: 0x0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_end_clears_all_platform_interface_function_pointers()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteI2c_Mock mockI2c;
  noteSetFn_Parameters.reset();
  notecard.begin(&mockI2c);

   // Action
  ///////////

  notecard.end();

   // Assert
  ///////////

  if (!noteSetFn_Parameters.mallocfn
   && !noteSetFn_Parameters.freefn
   && !noteSetFn_Parameters.delayfn
   && !noteSetFn_Parameters.millisfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFn_Parameters.mallocfn == 0x" << std::hex << noteSetFn_Parameters.mallocfn << ", EXPECTED: 0x0 (`nullptr`)" << std::endl;
    std::cout << "\tnoteSetFn_Parameters.freefn   == 0x" << std::hex << noteSetFn_Parameters.freefn   << ", EXPECTED: 0x0 (`nullptr`)" << std::endl;
    std::cout << "\tnoteSetFn_Parameters.delayfn  == 0x" << std::hex << noteSetFn_Parameters.delayfn  << ", EXPECTED: 0x0 (`nullptr`)" << std::endl;
    std::cout << "\tnoteSetFn_Parameters.millisfn == 0x" << std::hex << noteSetFn_Parameters.millisfn << ", EXPECTED: 0x0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_end_provides_nullptr_to_make_note_i2c_to_free_associated_memory_when_the_i2c_interface_has_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteI2c_Mock mockI2c;  // Instantiate NoteI2c (mocked)
  make_note_i2c_Parameters.reset();
  make_note_i2c_Parameters.i2c_parameters = &Wire;
  notecard.begin(&mockI2c);
  noteGetFnSerial_Parameters.reset();
  noteGetFnI2C_Parameters.reset();
  noteGetFnI2C_Parameters.transmitFn_result = reinterpret_cast<i2cTransmitFn>(0x1);

   // Action
  ////////////

  notecard.end();

   // Assert
  ////////////

  if (nullptr == make_note_i2c_Parameters.i2c_parameters)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tmake_note_i2c_Parameters.i2c_parameters == 0x" << std::hex << !!make_note_i2c_Parameters.i2c_parameters << ", EXPECTED: 0x0 (nullptr)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_end_invokes_make_note_serial_nullptr_method_to_free_associated_memory_when_the_serial_interface_has_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  make_note_serial_Parameters.reset();
  make_note_serial_Parameters.serial_parameters = &Serial;
  notecard.begin(&mockSerial);
  noteGetFnI2C_Parameters.reset();
  noteGetFnSerial_Parameters.reset();
  noteGetFnSerial_Parameters.transmitFn_result = reinterpret_cast<serialTransmitFn>(0x1);

   // Action
  ////////////

  notecard.end();

   // Assert
  ////////////

  if (nullptr == make_note_serial_Parameters.serial_parameters)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tmake_note_serial_Parameters.serial_parameters == 0x" << std::hex << !!make_note_serial_Parameters.serial_parameters << ", EXPECTED: 0x0 (nullptr)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_setDebugOutputStream_shares_a_debug_log_function_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteLog_Mock mockLog;
  noteSetFnDebugOutput_Parameters.reset();

   // Action
  ///////////

  notecard.setDebugOutputStream(&mockLog);

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

int test_notecard_setDebugOutputStream_clears_the_debug_log_function_pointer_when_nullptr_is_provided()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
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

int test_notecard_clearDebugOutputStream_clears_the_debug_log_function_pointer()
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

int test_notecard_setFn_shares_a_memory_allocation_function_pointer()
{
  int result;

   // Arrange
  ////////////

  const mallocFn mockMallocFn = reinterpret_cast<mallocFn>(0x19790917);

  Notecard notecard;
  noteSetFn_Parameters.reset();

   // Action
  ///////////

  notecard.setFn(mockMallocFn, nullptr, nullptr, nullptr);

   // Assert
  ///////////

  if (noteSetFn_Parameters.mallocfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFn_Parameters.mallocfn == " << !!noteSetFn_Parameters.mallocfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_setFn_shares_a_memory_deallocation_function_pointer()
{
  int result;

   // Arrange
  ////////////

  const freeFn mockFreeFn = reinterpret_cast<freeFn>(0x19790917);

  Notecard notecard;
  noteSetFn_Parameters.reset();

   // Action
  ///////////

  notecard.setFn(nullptr, mockFreeFn, nullptr, nullptr);

   // Assert
  ///////////

  if (noteSetFn_Parameters.freefn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFn_Parameters.freefn == " << !!noteSetFn_Parameters.freefn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_setFn_shares_a_delay_function_pointer()
{
  int result;

   // Arrange
  ////////////

  const delayMsFn mockDelayFn = reinterpret_cast<delayMsFn>(0x19790917);

  Notecard notecard;
  noteSetFn_Parameters.reset();

   // Action
  ///////////

  notecard.setFn(nullptr, nullptr, mockDelayFn, nullptr);

   // Assert
  ///////////

  if (noteSetFn_Parameters.delayfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFn_Parameters.delayfn == " << !!noteSetFn_Parameters.delayfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_setFn_shares_a_millis_function_pointer()
{
  int result;

   // Arrange
  ////////////

  const getMsFn mockMillisFn = reinterpret_cast<getMsFn>(0x19790917);

  Notecard notecard;
  noteSetFn_Parameters.reset();

   // Action
  ///////////

  notecard.setFn(nullptr, nullptr, nullptr, mockMillisFn);

   // Assert
  ///////////

  if (noteSetFn_Parameters.millisfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFn_Parameters.millisfn == " << !!noteSetFn_Parameters.millisfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_setFnI2cMutex_does_not_modify_locking_mutex_func_parameter_value_before_passing_to_note_c()
{
  int result;

   // Arrange
  ////////////
  const mutexFn lockFn = reinterpret_cast<mutexFn>(0x19790917);
  Notecard notecard;
  noteSetFnI2CMutex_Parameters.reset();

   // Action
  ///////////

  notecard.setFnI2cMutex(lockFn,nullptr);

   // Assert
  ///////////

  if (lockFn == noteSetFnI2CMutex_Parameters.lockI2cFn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\nnoteSetFnI2CMutex_Parameters.lockI2cFn == " << !!noteSetFnI2CMutex_Parameters.lockI2cFn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_setFnI2cMutex_does_not_modify_unlocking_mutex_func_parameter_value_before_passing_to_note_c()
{
  int result;

   // Arrange
  ////////////
  const mutexFn unlockFn = reinterpret_cast<mutexFn>(0x19790917);
  Notecard notecard;
  noteSetFnI2CMutex_Parameters.reset();

   // Action
  ///////////

  notecard.setFnI2cMutex(nullptr,unlockFn);

   // Assert
  ///////////

  if (unlockFn == noteSetFnI2CMutex_Parameters.unlockI2cFn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\nnoteSetFnI2CMutex_Parameters.unlockI2cFn == " << !!noteSetFnI2CMutex_Parameters.unlockI2cFn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_setFnNoteMutex_does_not_modify_locking_mutex_func_parameter_value_before_passing_to_note_c()
{
  int result;

   // Arrange
  ////////////
  const mutexFn lockFn = reinterpret_cast<mutexFn>(0x19790917);
  Notecard notecard;
  noteSetFnNoteMutex_Parameters.reset();

   // Action
  ///////////

  notecard.setFnNoteMutex(lockFn,nullptr);

   // Assert
  ///////////

  if (lockFn == noteSetFnNoteMutex_Parameters.lockNoteFn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\nnoteSetFnNoteMutex_Parameters.lockNoteFn == " << !!noteSetFnNoteMutex_Parameters.lockNoteFn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_setFnNoteMutex_does_not_modify_unlocking_mutex_func_parameter_value_before_passing_to_note_c()
{
  int result;

   // Arrange
  ////////////
  const mutexFn unlockFn = reinterpret_cast<mutexFn>(0x19790917);
  Notecard notecard;
  noteSetFnNoteMutex_Parameters.reset();

   // Action
  ///////////

  notecard.setFnNoteMutex(nullptr,unlockFn);

   // Assert
  ///////////

  if (unlockFn == noteSetFnNoteMutex_Parameters.unlockNoteFn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\nnoteSetFnNoteMutex_Parameters.unlockNoteFn == " << !!noteSetFnNoteMutex_Parameters.unlockNoteFn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_setTransactionPins_shares_a_transaction_start_function_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteTxn_Mock mockTxn;
  noteSetFnTransaction_Parameters.reset();

   // Action
  ///////////

  notecard.setTransactionPins(&mockTxn);

   // Assert
  ///////////

  if (noteSetFnTransaction_Parameters.startfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnTransaction_Parameters.startfn == " << !!noteSetFnTransaction_Parameters.startfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_setTransactionPins_shares_a_transaction_stop_function_pointer()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteTxn_Mock mockTxn;
  noteSetFnTransaction_Parameters.reset();

   // Action
  ///////////

  notecard.setTransactionPins(&mockTxn);

   // Assert
  ///////////

  if (noteSetFnTransaction_Parameters.stopfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnTransaction_Parameters.stopfn == " << !!noteSetFnTransaction_Parameters.stopfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_setTransactionPins_clears_the_transaction_start_function_pointer_when_nullptr_is_provided()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteTxn_Mock mockTxn;
  noteSetFnTransaction_Parameters.reset();
  noteSetFnTransaction_Parameters.startfn = reinterpret_cast<txnStartFn>(&result);

   // Action
  ///////////

  notecard.setTransactionPins(nullptr);

   // Assert
  ///////////

  if (!noteSetFnTransaction_Parameters.startfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnTransaction_Parameters.startfn == " << noteSetFnTransaction_Parameters.startfn << ", EXPECTED: 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_setTransactionPins_clears_the_transaction_stop_function_pointer_when_nullptr_is_provided()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  NoteTxn_Mock mockTxn;
  noteSetFnTransaction_Parameters.reset();
  noteSetFnTransaction_Parameters.stopfn = reinterpret_cast<txnStopFn>(&result);

   // Action
  ///////////

  notecard.setTransactionPins(nullptr);

   // Assert
  ///////////

  if (!noteSetFnTransaction_Parameters.stopfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnTransaction_Parameters.stopfn == " << noteSetFnTransaction_Parameters.stopfn << ", EXPECTED: 0 (`nullptr`)" << std::endl;
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

int test_notecard_sendRequestWithRetry_does_not_modify_j_object_parameter_value_before_passing_to_note_c()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  J * EXPECTED_JSON = reinterpret_cast<J *>(malloc(sizeof(J)));
  J * json_cpy;
  assert(nullptr != EXPECTED_JSON);

  noteRequestWithRetry_Parameters.reset();
  memset(EXPECTED_JSON, 0x55, sizeof(J));
  {
    json_cpy = reinterpret_cast<J *>(malloc(sizeof(J)));
    assert(nullptr != json_cpy);
    memcpy(json_cpy, EXPECTED_JSON, sizeof(J));
  }

   // Action
  ///////////

  notecard.sendRequestWithRetry(json_cpy, 0);

   // Assert
  ///////////

  if (!memcmp(EXPECTED_JSON, noteRequestWithRetry_Parameters.req, sizeof(J)))
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteRequestWithRetry_Parameters.req != EXPECTED_JSON" << std::endl;
    std::cout << "[";
  }

  free(json_cpy);
  free(EXPECTED_JSON);
  return result;
}

int test_notecard_sendRequestWithRetry_does_not_modify_timeout_parameter_value_before_passing_to_note_c()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  uint32_t EXPECTED_TIMEOUT = 10;
  noteRequestWithRetry_Parameters.reset();

   // Action
  ///////////

  notecard.sendRequestWithRetry(nullptr, EXPECTED_TIMEOUT);

   // Assert
  ///////////

  if (EXPECTED_TIMEOUT == noteRequestWithRetry_Parameters.timeoutSeconds)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteRequestWithRetry_Parameters.timeoutSeconds != EXPECTED_TIMEOUT" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_sendRequestWithRetry_does_not_modify_note_c_result_value_before_returning_to_caller()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  const bool EXPECTED_RESULT = true;

  noteRequestWithRetry_Parameters.reset();
  noteRequestWithRetry_Parameters.result = EXPECTED_RESULT;

   // Action
  ///////////

  const bool ACTUAL_RESULT = notecard.sendRequestWithRetry(nullptr, 0);

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
    std::cout << "\tnotecard.sendRequestWithRetry(nullptr, 0) == \"" << ACTUAL_RESULT << "\", EXPECTED: \"" << EXPECTED_RESULT << "\"" << std::endl;
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

int test_notecard_requestAndResponseWithRetry_does_not_modify_j_object_parameter_value_before_passing_to_note_c()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  J *EXPECTED_JSON = reinterpret_cast<J *>(malloc(sizeof(J)));
  J *json_cpy;
  assert(nullptr != EXPECTED_JSON);

  noteRequestResponseWithRetry_Parameters.reset();
  memset(EXPECTED_JSON, 0x55, sizeof(J));
  {
    json_cpy = reinterpret_cast<J *>(malloc(sizeof(J)));
    assert(nullptr != json_cpy);
    memcpy(json_cpy, EXPECTED_JSON, sizeof(J));
  }

   // Action
  ///////////

  notecard.requestAndResponseWithRetry(json_cpy, 0);

   // Assert
  ///////////

  if (!memcmp(EXPECTED_JSON, noteRequestResponseWithRetry_Parameters.req, sizeof(J)))
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteRequestResponseWithRetry_Parameters.req != EXPECTED_JSON" << std::endl;
    std::cout << "[";
  }

  free(json_cpy);
  free(EXPECTED_JSON);
  return result;
}

int test_notecard_requestAndResponseWithRetry_does_not_modify_timeout_parameter_value_before_passing_to_note_c()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  uint32_t EXPECTED_TIMEOUT = 10;
  noteRequestResponseWithRetry_Parameters.reset();

   // Action
  ///////////

  notecard.requestAndResponseWithRetry(nullptr, EXPECTED_TIMEOUT);

   // Assert
  ///////////

  if (EXPECTED_TIMEOUT == noteRequestResponseWithRetry_Parameters.timeoutSeconds)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteRequestResponseWithRetry_Parameters.timeoutSeconds != EXPECTED_TIMEOUT" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_requestAndResponseWithRetry_does_not_modify_note_c_result_value_before_returning_to_caller()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;
  J * EXPECTED_JSON = reinterpret_cast<J *>(malloc(sizeof(J)));
  assert(nullptr != EXPECTED_JSON);

  noteRequestResponseWithRetry_Parameters.reset();
  memset(EXPECTED_JSON, 0x55, sizeof(J));
  {
    noteRequestResponseWithRetry_Parameters.result = reinterpret_cast<J *>(malloc(sizeof(J)));
    assert(nullptr != noteRequestResponseWithRetry_Parameters.result);
    memcpy(noteRequestResponseWithRetry_Parameters.result, EXPECTED_JSON, sizeof(J));
  }

   // Action
  ///////////

  const J * const ACTUAL_RESULT = notecard.requestAndResponseWithRetry(nullptr, 0);

   // Assert
  ///////////

  if (!memcmp(EXPECTED_JSON, ACTUAL_RESULT, sizeof(J)))
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnotecard.requestAndResponseWithRetry(nullptr, 0) != EXPECTED_JSON" << std::endl;
    std::cout << "[";
  }

  free(noteRequestResponseWithRetry_Parameters.result);
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
  NoteI2c_Mock mockI2c;  // Instantiate NoteI2c (mocked)
  notecard.begin(&mockI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cReceiveFn noteI2cReceive = noteSetFnI2CDefault_Parameters.receivefn;
  noteI2cReceive_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cReceive(ADDRESS, response_buffer, SIZE, &actual_response_size);

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
  NoteI2c_Mock mockI2c;  // Instantiate NoteI2c (mocked)
  notecard.begin(&mockI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cReceiveFn noteI2cReceive = noteSetFnI2CDefault_Parameters.receivefn;
  noteI2cReceive_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cReceive(EXPECTED_ADDRESS, response_buffer, SIZE, &actual_response_size);

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
  NoteI2c_Mock mockI2c;  // Instantiate NoteI2c (mocked)
  notecard.begin(&mockI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cReceiveFn noteI2cReceive = noteSetFnI2CDefault_Parameters.receivefn;
  noteI2cReceive_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cReceive(ADDRESS, response_buffer, SIZE, &actual_response_size);

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
  NoteI2c_Mock mockI2c;  // Instantiate NoteI2c (mocked)
  notecard.begin(&mockI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cReceiveFn noteI2cReceive = noteSetFnI2CDefault_Parameters.receivefn;
  noteI2cReceive_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cReceive(ADDRESS, response_buffer, EXPECTED_SIZE, &actual_response_size);

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
  NoteI2c_Mock mockI2c;  // Instantiate NoteI2c (mocked)
  notecard.begin(&mockI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cReceiveFn noteI2cReceive = noteSetFnI2CDefault_Parameters.receivefn;
  noteI2cReceive_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cReceive(ADDRESS, response_buffer, SIZE, &actual_response_size);

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
  NoteI2c_Mock mockI2c;  // Instantiate NoteI2c (mocked)
  notecard.begin(&mockI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cReceiveFn noteI2cReceive = noteSetFnI2CDefault_Parameters.receivefn;
  noteI2cReceive_Parameters.reset();  // Clear the structure for testing results
  noteI2cReceive_Parameters.result = "i2c: fake test error!";

   // Action
  ///////////

  const char * const ACTUAL_RESULT = noteI2cReceive(ADDRESS, response_buffer, SIZE, &actual_response_size);

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
  NoteI2c_Mock mockI2c;
  notecard.begin(&mockI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cReceiveFn noteI2cReceive = noteSetFnI2CDefault_Parameters.receivefn;

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteI2c *>(nullptr));
  noteI2cReceive_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cReceive(ADDRESS, response_buffer, SIZE, &actual_response_size);

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
  NoteI2c_Mock mockI2c;
  notecard.begin(&mockI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cReceiveFn noteI2cReceive = noteSetFnI2CDefault_Parameters.receivefn;

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteI2c *>(nullptr));
  noteI2cReceive_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  const char * const ACTUAL_RESULT = noteI2cReceive(ADDRESS, response_buffer, SIZE, &actual_response_size);

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
    std::cout << "\tACTUAL_RESULT == " << std::string(ACTUAL_RESULT).c_str() << ", EXPECTED: " << EXPECTED_RESULT << std::endl;
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
  NoteI2c_Mock mockI2c;  // Instantiate NoteI2c (mocked)
  notecard.begin(&mockI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cResetFn noteI2cReset = noteSetFnI2CDefault_Parameters.resetfn;
  noteI2cReset_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cReset(ADDRESS);

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
  NoteI2c_Mock mockI2c;  // Instantiate NoteI2c (mocked)
  notecard.begin(&mockI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cResetFn noteI2cReset = noteSetFnI2CDefault_Parameters.resetfn;
  noteI2cReset_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cReset(EXPECTED_ADDRESS);

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
  NoteI2c_Mock mockI2c;  // Instantiate NoteI2c (mocked)
  notecard.begin(&mockI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cResetFn noteI2cReset = noteSetFnI2CDefault_Parameters.resetfn;
  noteI2cReset_Parameters.reset();  // Clear the structure for testing results
  noteI2cReset_Parameters.result = true;

   // Action
  ///////////

  const bool ACTUAL_RESULT = noteI2cReset(ADDRESS);

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
  NoteI2c_Mock mockI2c;
  notecard.begin(&mockI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cResetFn noteI2cReset = noteSetFnI2CDefault_Parameters.resetfn;

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteI2c *>(nullptr));
  noteI2cReset_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cReset(ADDRESS);

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
  NoteI2c_Mock mockI2c;
  notecard.begin(&mockI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cResetFn noteI2cReset = noteSetFnI2CDefault_Parameters.resetfn;

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteI2c *>(nullptr));
  noteI2cReset_Parameters.reset();  // Clear the structure for testing results
  noteI2cReset_Parameters.result = true;  // Ensure that if the mock were to be invoked the test would fail

   // Action
  ///////////

  const bool ACTUAL_RESULT = noteI2cReset(ADDRESS);

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
  uint8_t transmit_buffer[] = "Transmissible data";

  Notecard notecard;
  NoteI2c_Mock mockI2c;  // Instantiate NoteI2c (mocked)
  notecard.begin(&mockI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cTransmitFn noteI2cTransmit = noteSetFnI2CDefault_Parameters.transmitfn;
  noteI2cTransmit_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cTransmit(ADDRESS, transmit_buffer, SIZE);

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
  uint8_t transmit_buffer[] = "Transmissible data";

  Notecard notecard;
  NoteI2c_Mock mockI2c;  // Instantiate NoteI2c (mocked)
  notecard.begin(&mockI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cTransmitFn noteI2cTransmit = noteSetFnI2CDefault_Parameters.transmitfn;
  noteI2cTransmit_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cTransmit(EXPECTED_ADDRESS, transmit_buffer, SIZE);

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
  NoteI2c_Mock mockI2c;  // Instantiate NoteI2c (mocked)
  notecard.begin(&mockI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cTransmitFn noteI2cTransmit = noteSetFnI2CDefault_Parameters.transmitfn;
  noteI2cTransmit_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cTransmit(ADDRESS, transmit_buffer, SIZE);

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
    std::cout << "\tnoteI2cTransmit_Parameters.buffer == " << noteI2cTransmit_Parameters.buffer_cache.c_str() << ", EXPECTED: " << transmit_buffer << std::endl;
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
  uint8_t transmit_buffer[] = "Transmissible data";

  Notecard notecard;
  NoteI2c_Mock mockI2c;  // Instantiate NoteI2c (mocked)
  notecard.begin(&mockI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cTransmitFn noteI2cTransmit = noteSetFnI2CDefault_Parameters.transmitfn;
  noteI2cTransmit_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cTransmit(ADDRESS, transmit_buffer, EXPECTED_SIZE);

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
  uint8_t transmit_buffer[] = "Transmissible data";

  Notecard notecard;
  NoteI2c_Mock mockI2c;  // Instantiate NoteI2c (mocked)
  notecard.begin(&mockI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cTransmitFn noteI2cTransmit = noteSetFnI2CDefault_Parameters.transmitfn;
  noteI2cTransmit_Parameters.reset();  // Clear the structure for testing results
  noteI2cTransmit_Parameters.result = "i2c: fake test error!";

   // Action
  ///////////

  const char * const ACTUAL_RESULT = noteI2cTransmit(ADDRESS, transmit_buffer, SIZE);

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
  uint8_t transmit_buffer[] = "Transmissible data";

  Notecard notecard;
  NoteI2c_Mock mockI2c;
  notecard.begin(&mockI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cTransmitFn noteI2cTransmit = noteSetFnI2CDefault_Parameters.transmitfn;

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteI2c *>(nullptr));
  noteI2cTransmit_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cTransmit(ADDRESS, transmit_buffer, SIZE);

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
  uint8_t transmit_buffer[] = "Transmissible data";
  const char * const EXPECTED_RESULT = "i2c: A call to Notecard::begin() is required. {io}";

  Notecard notecard;
  NoteI2c_Mock mockI2c;
  notecard.begin(&mockI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cTransmitFn noteI2cTransmit = noteSetFnI2CDefault_Parameters.transmitfn;

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteI2c *>(nullptr));
  noteI2cTransmit_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  const char * const ACTUAL_RESULT = noteI2cTransmit(ADDRESS, transmit_buffer, SIZE);

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
    std::cout << "\tACTUAL_RESULT == " << std::string(ACTUAL_RESULT).c_str() << ", EXPECTED: " << EXPECTED_RESULT << std::endl;
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
  notecard.setDebugOutputStream(&mockLog);  // Provides access to the hidden static callback methods through `note-c` mocks
  debugOutputFn noteLogPrint = noteSetFnDebugOutput_Parameters.fn;  // Capture the internal Notecard log function, `noteLogPrint`
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
  notecard.setDebugOutputStream(&mockLog);  // Provides access to the hidden static callback methods through `note-c` mocks
  debugOutputFn noteLogPrint = noteSetFnDebugOutput_Parameters.fn;  // Capture the internal Notecard log function, `noteLogPrint`
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
  notecard.setDebugOutputStream(&mockLog);  // Provides access to the hidden static callback methods through `note-c` mocks
  debugOutputFn noteLogPrint = noteSetFnDebugOutput_Parameters.fn;  // Capture the internal Notecard log function, `noteLogPrint`
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
  NoteLog_Mock mockLog;
  notecard.setDebugOutputStream(&mockLog);  // Provides access to the internal static callback methods through `note-c` mocks
  debugOutputFn noteLogPrint = noteSetFnDebugOutput_Parameters.fn;  // Capture the internal Notecard log function, `noteLogPrint`

  // Reset to ensure interface is not instantiated
  notecard.setDebugOutputStream(nullptr);
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
  NoteLog_Mock mockLog;
  notecard.setDebugOutputStream(&mockLog);  // Provides access to the internal static callback methods through `note-c` mocks
  debugOutputFn noteLogPrint = noteSetFnDebugOutput_Parameters.fn;  // Capture the internal Notecard log function, `noteLogPrint`

  // Reset to ensure interface is not instantiated
  notecard.setDebugOutputStream(nullptr);
  noteLogPrint_Parameters.reset();  // Clear the structure for testing NoteLog::print results
  noteLogPrint_Parameters.result = sizeof(message);  // Ensure that if the mock were to be invoked the test would fail

   // Action
  ///////////

  const size_t ACTUAL_RESULT = noteLogPrint(message);

   // Assert
  ///////////

  if (!ACTUAL_RESULT && !noteLogPrint_Parameters.invoked)
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
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  notecard.begin(&mockSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialAvailableFn noteSerialAvailable = noteSetFnSerialDefault_Parameters.availfn;  // Capture the internal Notecard serial function, `noteSerialAvailable`
  noteSerialAvailable_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSerialAvailable();

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
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  notecard.begin(&mockSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialAvailableFn noteSerialAvailable = noteSetFnSerialDefault_Parameters.availfn;  // Capture the internal Notecard serial function, `noteSerialAvailable`
  noteSerialAvailable_Parameters.reset();  // Clear the structure for testing results
  noteSerialAvailable_Parameters.result = true;

   // Action
  ///////////

  const bool ACTUAL_RESULT = noteSerialAvailable();

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

  // Capture the internal Notecard log function, `noteSerialAvailable`
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  notecard.begin(&mockSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialAvailableFn noteSerialAvailable = noteSetFnSerialDefault_Parameters.availfn;  // Capture the internal Notecard serial function, `noteSerialAvailable`

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteSerial *>(nullptr));
  noteSerialAvailable_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSerialAvailable();

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

  // Capture the internal Notecard log function, `noteSerialAvailable`
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  notecard.begin(&mockSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialAvailableFn noteSerialAvailable = noteSetFnSerialDefault_Parameters.availfn;  // Capture the internal Notecard serial function, `noteSerialAvailable`

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteSerial *>(nullptr));
  noteSerialAvailable_Parameters.reset();  // Clear the structure for testing results
  noteSerialAvailable_Parameters.result = true;  // Ensure that if the mock were to be invoked the test would fail

   // Action
  ///////////

  const bool ACTUAL_RESULT = noteSerialAvailable();

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
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  notecard.begin(&mockSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialReceiveFn noteSerialReceive = noteSetFnSerialDefault_Parameters.readfn;  // Capture the internal Notecard serial function, `noteSerialReceive`
  noteSerialReceive_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSerialReceive();

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
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  notecard.begin(&mockSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialReceiveFn noteSerialReceive = noteSetFnSerialDefault_Parameters.readfn;  // Capture the internal Notecard serial function, `noteSerialReceive`
  noteSerialReceive_Parameters.reset();  // Clear the structure for testing results
  noteSerialReceive_Parameters.result = 'Z';

   // Action
  ///////////

  const char ACTUAL_RESULT = noteSerialReceive();

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

  // Capture the internal Notecard log function, `noteSerialAvailable`
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  notecard.begin(&mockSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialReceiveFn noteSerialReceive = noteSetFnSerialDefault_Parameters.readfn;  // Capture the internal Notecard serial function, `noteSerialReceive`

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteSerial *>(nullptr));
  noteSerialReceive_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSerialReceive();

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

  static const char EXPECTED_RESULT = '\0';
  Notecard notecard;

  // Capture the internal Notecard log function, `noteSerialAvailable`
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  notecard.begin(&mockSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialReceiveFn noteSerialReceive = noteSetFnSerialDefault_Parameters.readfn;  // Capture the internal Notecard serial function, `noteSerialReceive`

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteSerial *>(nullptr));
  noteSerialReceive_Parameters.reset();  // Clear the structure for testing results
  noteSerialReceive_Parameters.result = 'Z';

   // Action
  ///////////

  const char ACTUAL_RESULT = noteSerialReceive();

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
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  notecard.begin(&mockSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialResetFn noteSerialReset = noteSetFnSerialDefault_Parameters.resetfn;  // Capture the internal Notecard serial function, `noteSerialReset`
  noteSerialReset_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSerialReset();

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
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  notecard.begin(&mockSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialResetFn noteSerialReset = noteSetFnSerialDefault_Parameters.resetfn;  // Capture the internal Notecard serial function, `noteSerialReset`
  noteSerialReset_Parameters.reset();  // Clear the structure for testing results
  noteSerialReset_Parameters.result = true;

   // Action
  ///////////

  const bool ACTUAL_RESULT = noteSerialReset();

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

  // Capture the internal Notecard log function, `noteSerialAvailable`
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  notecard.begin(&mockSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialResetFn noteSerialReset = noteSetFnSerialDefault_Parameters.resetfn;  // Capture the internal Notecard serial function, `noteSerialReset`

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteSerial *>(nullptr));
  noteSerialReset_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSerialReset();

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

  // Capture the internal Notecard log function, `noteSerialAvailable`
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  notecard.begin(&mockSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialResetFn noteSerialReset = noteSetFnSerialDefault_Parameters.resetfn;  // Capture the internal Notecard serial function, `noteSerialReset`

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteSerial *>(nullptr));
  noteSerialReset_Parameters.reset();  // Clear the structure for testing results
  noteSerialReset_Parameters.result = true;

   // Action
  ///////////

  const bool ACTUAL_RESULT = noteSerialReset();

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
  static const size_t LEN = sizeof(text);
  static const bool FLUSH = true;

  Notecard notecard;
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  notecard.begin(&mockSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialTransmitFn noteSerialTransmit = noteSetFnSerialDefault_Parameters.writefn;  // Capture the internal Notecard serial function, `noteSerialTransmit`
  noteSerialTransmit_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSerialTransmit(text, LEN, FLUSH);

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
    std::cout << "\tnoteSerialTransmit_Parameters.invoked == " << noteSerialTransmit_Parameters.invoked << ", EXPECTED: > 0" << std::endl;
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
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  notecard.begin(&mockSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialTransmitFn noteSerialTransmit = noteSetFnSerialDefault_Parameters.writefn;  // Capture the internal Notecard serial function, `noteSerialTransmit`
  noteSerialTransmit_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSerialTransmit(expected_text, LEN, FLUSH);

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
    std::cout << "\tnoteSerialTransmit_Parameters.buffer == " << std::string(reinterpret_cast<char *>(noteSerialTransmit_Parameters.buffer)).c_str() << ", EXPECTED: " << expected_text << std::endl;
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
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  notecard.begin(&mockSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialTransmitFn noteSerialTransmit = noteSetFnSerialDefault_Parameters.writefn;  // Capture the internal Notecard serial function, `noteSerialTransmit`
  noteSerialTransmit_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSerialTransmit(text, EXPECTED_LEN, FLUSH);

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
    std::cout << "\tnoteSerialTransmit_Parameters.size == " << noteSerialTransmit_Parameters.size << ", EXPECTED: " << EXPECTED_LEN << std::endl;
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
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  notecard.begin(&mockSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialTransmitFn noteSerialTransmit = noteSetFnSerialDefault_Parameters.writefn;  // Capture the internal Notecard serial function, `noteSerialTransmit`
  noteSerialTransmit_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSerialTransmit(text, LEN, EXPECTED_FLUSH);

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
    std::cout << "\tnoteSerialTransmit_Parameters.flush == " << noteSerialTransmit_Parameters.flush << ", EXPECTED: " << EXPECTED_FLUSH << std::endl;
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

  // Capture the internal Notecard log function, `noteSerialAvailable`
  NoteSerial_Mock mockSerial;  // Instantiate NoteSerial (mocked)
  notecard.begin(&mockSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialTransmitFn noteSerialTransmit = noteSetFnSerialDefault_Parameters.writefn;  // Capture the internal Notecard serial function, `noteSerialTransmit`

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteSerial *>(nullptr));
  noteSerialTransmit_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteSerialTransmit(text, LEN, EXPECTED_FLUSH);

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

int test_static_callback_note_txn_start_invokes_notetxn_start()
{
  int result;

   // Arrange
  ////////////

  const size_t TIMEOUT_MS = 917;

  Notecard notecard;

  // Capture the internal Notecard transaction function, `noteTransactionStart`
  NoteTxn_Mock mockTxn;  // Instantiate NoteTxn (mocked)
  notecard.setTransactionPins(&mockTxn);  // Provides access to the hidden static callback methods through `note-c` mocks
  txnStartFn noteTransactionStart = noteSetFnTransaction_Parameters.startfn;  // Capture the internal Notecard serial function, `noteTransactionStart`
  noteTxnStart_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteTransactionStart(TIMEOUT_MS);

   // Assert
  ///////////

  if (noteTxnStart_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteTxnStart_Parameters.invoked == " << noteTxnStart_Parameters.invoked << ", EXPECTED: > 0" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_txn_start_does_not_modify_timeout_ms_parameter_before_passing_to_interface_method()
{
  int result;

   // Arrange
  ////////////

  const size_t TIMEOUT_MS = 917;

  Notecard notecard;

  // Capture the internal Notecard transaction function, `noteTransactionStart`
  NoteTxn_Mock mockTxn;  // Instantiate NoteTxn (mocked)
  notecard.setTransactionPins(&mockTxn);  // Provides access to the hidden static callback methods through `note-c` mocks
  txnStartFn noteTransactionStart = noteSetFnTransaction_Parameters.startfn;  // Capture the internal Notecard serial function, `noteTransactionStart`
  noteTxnStart_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteTransactionStart(TIMEOUT_MS);

   // Assert
  ///////////

  if (noteTxnStart_Parameters.timeout_ms == TIMEOUT_MS)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteTxnStart_Parameters.timeout_ms == " << noteTxnStart_Parameters.timeout_ms << ", EXPECTED: " << TIMEOUT_MS << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_txn_start_does_not_modify_interface_method_return_value()
{
  int result;

   // Arrange
  ////////////

  const size_t TIMEOUT_MS = 917;

  Notecard notecard;

  // Capture the internal Notecard transaction function, `noteTransactionStart`
  NoteTxn_Mock mockTxn;  // Instantiate NoteTxn (mocked)
  notecard.setTransactionPins(&mockTxn);  // Provides access to the hidden static callback methods through `note-c` mocks
  txnStartFn noteTransactionStart = noteSetFnTransaction_Parameters.startfn;  // Capture the internal Notecard serial function, `noteTransactionStart`
  noteTxnStart_Parameters.reset();  // Clear the structure for testing results
  noteTxnStart_Parameters.result = true;

   // Action
  ///////////

  const bool ACTUAL_RESULT = noteTransactionStart(TIMEOUT_MS);

   // Assert
  ///////////

  if (noteTxnStart_Parameters.result == ACTUAL_RESULT)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tACTUAL_RESULT == " << ACTUAL_RESULT << ", EXPECTED: " << noteTxnStart_Parameters.result << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_txn_start_does_not_call_interface_method_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  const size_t TIMEOUT_MS = 917;

  Notecard notecard;

  // Capture the internal Notecard transaction function, `noteTransactionStart`
  NoteTxn_Mock mockTxn;  // Instantiate NoteTxn (mocked)
  notecard.setTransactionPins(&mockTxn);  // Provides access to the hidden static callback methods through `note-c` mocks
  txnStartFn noteTransactionStart = noteSetFnTransaction_Parameters.startfn;  // Capture the internal Notecard serial function, `noteTransactionStart`

  // Reset to ensure the interface is not instantiated
  notecard.setTransactionPins(static_cast<NoteTxn *>(nullptr));
  noteTxnStart_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteTransactionStart(TIMEOUT_MS);

   // Assert
  ///////////

  if (!noteTxnStart_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteTxnStart_Parameters.invoked == " << noteTxnStart_Parameters.invoked << ", EXPECTED: zero (0)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_txn_start_returns_true_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  const bool EXPECTED_RESULT = true;
  const size_t TIMEOUT_MS = 917;

  Notecard notecard;

  // Capture the internal Notecard transaction function, `noteTransactionStart`
  NoteTxn_Mock mockTxn;  // Instantiate NoteTxn (mocked)
  notecard.setTransactionPins(&mockTxn);  // Provides access to the hidden static callback methods through `note-c` mocks
  txnStartFn noteTransactionStart = noteSetFnTransaction_Parameters.startfn;  // Capture the internal Notecard serial function, `noteTransactionStart`

  // Reset to ensure the interface is not instantiated
  notecard.setTransactionPins(static_cast<NoteTxn *>(nullptr));
  noteTxnStart_Parameters.reset();  // Clear the structure for testing results
  noteTxnStart_Parameters.result = false;

   // Action
  ///////////

  const bool ACTUAL_RESULT = noteTransactionStart(TIMEOUT_MS);

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
    std::cout << "\tACTUAL_RESULT == " << ACTUAL_RESULT << ", EXPECTED: " << EXPECTED_RESULT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_txn_stop_invokes_notetxn_stop()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;

  // Capture the internal Notecard transaction function, `noteTransactionStop`
  NoteTxn_Mock mockTxn;  // Instantiate NoteTxn (mocked)
  notecard.setTransactionPins(&mockTxn);  // Provides access to the hidden static callback methods through `note-c` mocks
  txnStopFn noteTransactionStop = noteSetFnTransaction_Parameters.stopfn;  // Capture the internal Notecard serial function, `noteTransactionStop`
  noteTxnStop_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteTransactionStop();

   // Assert
  ///////////

  if (noteTxnStop_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteTxnStop_Parameters.invoked == " << noteTxnStop_Parameters.invoked << ", EXPECTED: > 0" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_static_callback_note_txn_stop_does_not_call_interface_method_when_interface_has_not_been_instantiated()
{
  int result;

   // Arrange
  ////////////

  Notecard notecard;

  // Capture the internal Notecard transaction function, `noteTransactionStop`
  NoteTxn_Mock mockTxn;  // Instantiate NoteTxn (mocked)
  notecard.setTransactionPins(&mockTxn);  // Provides access to the hidden static callback methods through `note-c` mocks
  txnStopFn noteTransactionStop = noteSetFnTransaction_Parameters.stopfn;  // Capture the internal Notecard serial function, `noteTransactionStop`

  // Reset to ensure the interface is not instantiated
  notecard.setTransactionPins(static_cast<NoteTxn *>(nullptr));
  noteTxnStop_Parameters.reset();  // Clear the structure for testing results

   // Action
  ///////////

  noteTransactionStop();

   // Assert
  ///////////

  if (!noteTxnStop_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('c' + 'a' + 'l' + 'l' + 'b' + 'a' + 'c' + 'k');
    std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteTxnStop_Parameters.invoked == " << noteTxnStop_Parameters.invoked << ", EXPECTED: zero (0)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int main(void)
{
  TestFunction tests[] = {
      {test_notecard_begin_i2c_sets_user_agent_to_note_arduino, "test_notecard_begin_i2c_sets_user_agent_to_note_arduino"},
      {test_notecard_begin_i2c_sets_user_agent_to_note_arduino_when_interface_has_not_been_instantiated, "test_notecard_begin_i2c_sets_user_agent_to_note_arduino_when_interface_has_not_been_instantiated"},
      {test_notecard_begin_i2c_shares_a_memory_allocation_function_pointer, "test_notecard_begin_i2c_shares_a_memory_allocation_function_pointer"},
      {test_notecard_begin_i2c_sets_memory_allocation_function_pointer_to_nullptr_when_interface_has_not_been_instantiated, "test_notecard_begin_i2c_sets_memory_allocation_function_pointer_to_nullptr_when_interface_has_not_been_instantiated"},
      {test_notecard_begin_i2c_shares_a_memory_free_function_pointer, "test_notecard_begin_i2c_shares_a_memory_free_function_pointer"},
      {test_notecard_begin_i2c_sets_memory_free_function_pointer_to_nullptr_when_interface_has_not_been_instantiated, "test_notecard_begin_i2c_sets_memory_free_function_pointer_to_nullptr_when_interface_has_not_been_instantiated"},
      {test_notecard_begin_i2c_shares_a_delay_function_pointer, "test_notecard_begin_i2c_shares_a_delay_function_pointer"},
      {test_notecard_begin_i2c_sets_delay_function_pointer_to_nullptr_when_interface_has_not_been_instantiated, "test_notecard_begin_i2c_sets_delay_function_pointer_to_nullptr_when_interface_has_not_been_instantiated"},
      {test_notecard_begin_i2c_shares_a_millis_function_pointer, "test_notecard_begin_i2c_shares_a_millis_function_pointer"},
      {test_notecard_begin_i2c_sets_millis_function_pointer_to_nullptr_when_interface_has_not_been_instantiated, "test_notecard_begin_i2c_sets_millis_function_pointer_to_nullptr_when_interface_has_not_been_instantiated"},
      {test_notecard_begin_i2c_does_not_modify_i2c_address_parameter_before_passing_to_note_c, "test_notecard_begin_i2c_does_not_modify_i2c_address_parameter_before_passing_to_note_c"},
      {test_notecard_begin_i2c_sets_i2c_address_parameter_to_zero_before_passing_to_note_c_when_interface_has_not_been_instantiated, "test_notecard_begin_i2c_sets_i2c_address_parameter_to_zero_before_passing_to_note_c_when_interface_has_not_been_instantiated"},
      {test_notecard_begin_i2c_does_not_modify_i2c_mtu_parameter_before_passing_to_note_c, "test_notecard_begin_i2c_does_not_modify_i2c_mtu_parameter_before_passing_to_note_c"},
      {test_notecard_begin_i2c_sets_i2c_mtu_parameter_to_zero_before_passing_to_note_c_when_interface_has_not_been_instantiated, "test_notecard_begin_i2c_sets_i2c_mtu_parameter_to_zero_before_passing_to_note_c_when_interface_has_not_been_instantiated"},
      {test_notecard_begin_i2c_shares_an_i2c_reset_function_pointer, "test_notecard_begin_i2c_shares_an_i2c_reset_function_pointer"},
      {test_notecard_begin_i2c_sets_i2c_reset_function_pointer_to_nullptr_when_interface_has_not_been_instantiated, "test_notecard_begin_i2c_sets_i2c_reset_function_pointer_to_nullptr_when_interface_has_not_been_instantiated"},
      {test_notecard_begin_i2c_shares_an_i2c_transmit_function_pointer, "test_notecard_begin_i2c_shares_an_i2c_transmit_function_pointer"},
      {test_notecard_begin_i2c_sets_i2c_transmit_function_pointer_to_nullptr_when_interface_has_not_been_instantiated, "test_notecard_begin_i2c_sets_i2c_transmit_function_pointer_to_nullptr_when_interface_has_not_been_instantiated"},
      {test_notecard_begin_i2c_shares_an_i2c_receive_function_pointer, "test_notecard_begin_i2c_shares_an_i2c_receive_function_pointer"},
      {test_notecard_begin_i2c_sets_i2c_receive_function_pointer_to_nullptr_when_interface_has_not_been_instantiated, "test_notecard_begin_i2c_sets_i2c_receive_function_pointer_to_nullptr_when_interface_has_not_been_instantiated"},
      {test_notecard_begin_i2c_default_parameter_for_i2cMax_is_passed_to_note_c, "test_notecard_begin_i2c_default_parameter_for_i2c_mtu_is_passed_to_note_c"},
      {test_notecard_begin_i2c_default_parameter_for_i2cAddress_is_passed_to_note_c, "test_notecard_begin_i2c_default_parameter_for_i2c_address_is_passed_to_note_c"},
      {test_notecard_begin_serial_sets_user_agent_to_note_arduino, "test_notecard_begin_serial_sets_user_agent_to_note_arduino"},
      {test_notecard_begin_serial_sets_user_agent_to_note_arduino_when_interface_has_not_been_instantiated, "test_notecard_begin_serial_sets_user_agent_to_note_arduino_when_interface_has_not_been_instantiated"},
      {test_notecard_begin_serial_shares_a_memory_allocation_function_pointer, "test_notecard_begin_serial_shares_a_memory_allocation_function_pointer"},
      {test_notecard_begin_serial_sets_memory_allocation_function_pointer_to_nullptr_when_interface_has_not_been_instantiated, "test_notecard_begin_serial_sets_memory_allocation_function_pointer_to_nullptr_when_interface_has_not_been_instantiated"},
      {test_notecard_begin_serial_shares_a_memory_free_function_pointer, "test_notecard_begin_serial_shares_a_memory_free_function_pointer"},
      {test_notecard_begin_serial_sets_memory_free_function_pointer_to_nullptr_when_interface_has_not_been_instantiated, "test_notecard_begin_serial_sets_memory_free_function_pointer_to_nullptr_when_interface_has_not_been_instantiated"},
      {test_notecard_begin_serial_shares_a_delay_function_pointer, "test_notecard_begin_serial_shares_a_delay_function_pointer"},
      {test_notecard_begin_serial_sets_delay_function_pointer_to_nullptr_when_interface_has_not_been_instantiated, "test_notecard_begin_serial_sets_delay_function_pointer_to_nullptr_when_interface_has_not_been_instantiated"},
      {test_notecard_begin_serial_shares_a_millis_function_pointer, "test_notecard_begin_serial_shares_a_millis_function_pointer"},
      {test_notecard_begin_serial_sets_millis_function_pointer_to_nullptr_when_interface_has_not_been_instantiated, "test_notecard_begin_serial_sets_millis_function_pointer_to_nullptr_when_interface_has_not_been_instantiated"},
      {test_notecard_begin_serial_shares_a_serial_reset_function_pointer, "test_notecard_begin_serial_shares_a_serial_reset_function_pointer"},
      {test_notecard_begin_serial_sets_serial_reset_function_pointer_to_nullptr_when_interface_has_not_been_instantiated, "test_notecard_begin_serial_sets_serial_reset_function_pointer_to_nullptr_when_interface_has_not_been_instantiated"},
      {test_notecard_begin_serial_shares_a_serial_transmit_function_pointer, "test_notecard_begin_serial_shares_a_serial_transmit_function_pointer"},
      {test_notecard_begin_serial_sets_transmit_function_pointer_to_nullptr_when_interface_has_not_been_instantiated, "test_notecard_begin_serial_sets_transmit_function_pointer_to_nullptr_when_interface_has_not_been_instantiated"},
      {test_notecard_begin_serial_shares_a_serial_available_function_pointer, "test_notecard_begin_serial_shares_a_serial_available_function_pointer"},
      {test_notecard_begin_serial_sets_serial_available_function_pointer_to_nullptr_when_interface_has_not_been_instantiated, "test_notecard_begin_serial_sets_serial_available_function_pointer_to_nullptr_when_interface_has_not_been_instantiated"},
      {test_notecard_begin_serial_shares_a_serial_receive_function_pointer, "test_notecard_begin_serial_shares_a_serial_receive_function_pointer"},
      {test_notecard_begin_serial_sets_serial_receive_function_pointer_to_nullptr_when_interface_has_not_been_instantiated, "test_notecard_begin_serial_sets_serial_receive_function_pointer_to_nullptr_when_interface_has_not_been_instantiated"},
      {test_notecard_end_clears_all_i2c_interface_function_pointers_when_the_i2c_interface_has_been_instantiated, "test_notecard_end_clears_all_i2c_interface_function_pointers_when_the_i2c_interface_has_been_instantiated"},
      {test_notecard_end_does_not_call_wire_end_when_the_i2c_interface_has_not_been_instantiated, "test_notecard_end_does_not_call_wire_end_when_the_i2c_interface_has_not_been_instantiated"},
      {test_notecard_end_does_not_call_serial_end_when_the_serial_interface_has_not_been_instantiated, "test_notecard_end_does_not_call_serial_end_when_the_serial_interface_has_not_been_instantiated"},
      {test_notecard_end_clears_all_serial_interface_function_pointers_when_the_serial_interface_has_been_instantiated, "test_notecard_end_clears_all_serial_interface_function_pointers_when_the_serial_interface_has_been_instantiated"},
      {test_notecard_end_clears_all_platform_interface_function_pointers, "test_notecard_end_clears_all_platform_interface_function_pointers"},
      {test_notecard_end_provides_nullptr_to_make_note_i2c_to_free_associated_memory_when_the_i2c_interface_has_been_instantiated, "test_notecard_end_provides_nullptr_to_make_note_i2c_to_free_associated_memory_when_the_i2c_interface_has_been_instantiated"},
      {test_notecard_end_invokes_make_note_serial_nullptr_method_to_free_associated_memory_when_the_serial_interface_has_been_instantiated, "test_notecard_end_invokes_make_note_serial_nullptr_method_to_free_associated_memory_when_the_serial_interface_has_been_instantiated"},
      {test_notecard_setDebugOutputStream_shares_a_debug_log_function_pointer, "test_notecard_setDebugOutputStream_shares_a_debug_log_function_pointer"},
      {test_notecard_setDebugOutputStream_clears_the_debug_log_function_pointer_when_nullptr_is_provided, "test_notecard_setDebugOutputStream_clears_the_debug_log_function_pointer_when_nullptr_is_provided"},
      {test_notecard_clearDebugOutputStream_clears_the_debug_log_function_pointer, "test_notecard_clearDebugOutputStream_clears_the_debug_log_function_pointer"},
      {test_notecard_setFn_shares_a_memory_allocation_function_pointer, "test_notecard_setFn_shares_a_memory_allocation_function_pointer"},
      {test_notecard_setFn_shares_a_memory_deallocation_function_pointer, "test_notecard_setFn_shares_a_memory_deallocation_function_pointer"},
      {test_notecard_setFn_shares_a_delay_function_pointer, "test_notecard_setFn_shares_a_delay_function_pointer"},
      {test_notecard_setFn_shares_a_millis_function_pointer, "test_notecard_setFn_shares_a_millis_function_pointer"},
      {test_notecard_setFnI2cMutex_does_not_modify_locking_mutex_func_parameter_value_before_passing_to_note_c, "test_notecard_setFnI2cMutex_does_not_modify_locking_mutex_func_parameter_value_before_passing_to_note_c"},
      {test_notecard_setFnI2cMutex_does_not_modify_unlocking_mutex_func_parameter_value_before_passing_to_note_c, "test_notecard_setFnI2cMutex_does_not_modify_unlocking_mutex_func_parameter_value_before_passing_to_note_c"},
      {test_notecard_setFnNoteMutex_does_not_modify_locking_mutex_func_parameter_value_before_passing_to_note_c, "test_notecard_setFnNoteMutex_does_not_modify_locking_mutex_func_parameter_value_before_passing_to_note_c"},
      {test_notecard_setFnNoteMutex_does_not_modify_unlocking_mutex_func_parameter_value_before_passing_to_note_c, "test_notecard_setFnNoteMutex_does_not_modify_unlocking_mutex_func_parameter_value_before_passing_to_note_c"},
      {test_notecard_setTransactionPins_shares_a_transaction_start_function_pointer, "test_notecard_setTransactionPins_shares_a_transaction_start_function_pointer"},
      {test_notecard_setTransactionPins_shares_a_transaction_stop_function_pointer, "test_notecard_setTransactionPins_shares_a_transaction_stop_function_pointer"},
      {test_notecard_setTransactionPins_clears_the_transaction_start_function_pointer_when_nullptr_is_provided, "test_notecard_setTransactionPins_clears_the_transaction_start_function_pointer_when_nullptr_is_provided"},
      {test_notecard_setTransactionPins_clears_the_transaction_stop_function_pointer_when_nullptr_is_provided, "test_notecard_setTransactionPins_clears_the_transaction_stop_function_pointer_when_nullptr_is_provided"},
      {test_notecard_newRequest_does_not_modify_string_parameter_value_before_passing_to_note_c, "test_notecard_newRequest_does_not_modify_string_parameter_value_before_passing_to_note_c"},
      {test_notecard_newRequest_does_not_modify_note_c_result_value_before_returning_to_caller, "test_notecard_newRequest_does_not_modify_note_c_result_value_before_returning_to_caller"},
      {test_notecard_sendRequest_does_not_modify_j_object_parameter_value_before_passing_to_note_c, "test_notecard_sendRequest_does_not_modify_j_object_parameter_value_before_passing_to_note_c"},
      {test_notecard_sendRequest_does_not_modify_note_c_result_value_before_returning_to_caller, "test_notecard_sendRequest_does_not_modify_note_c_result_value_before_returning_to_caller"},
      {test_notecard_sendRequestWithRetry_does_not_modify_j_object_parameter_value_before_passing_to_note_c, "test_notecard_sendRequestWithRetry_does_not_modify_j_object_parameter_value_before_passing_to_note_c"},
      {test_notecard_sendRequestWithRetry_does_not_modify_timeout_parameter_value_before_passing_to_note_c, "test_notecard_sendRequestWithRetry_does_not_modify_timeout_parameter_value_before_passing_to_note_c"},
      {test_notecard_sendRequestWithRetry_does_not_modify_note_c_result_value_before_returning_to_caller, "test_notecard_sendRequestWithRetry_does_not_modify_note_c_result_value_before_returning_to_caller"},
      {test_notecard_requestAndResponse_does_not_modify_j_object_parameter_value_before_passing_to_note_c, "test_notecard_requestAndResponse_does_not_modify_j_object_parameter_value_before_passing_to_note_c"},
      {test_notecard_requestAndResponse_does_not_modify_note_c_result_value_before_returning_to_caller, "test_notecard_requestAndResponse_does_not_modify_note_c_result_value_before_returning_to_caller"},
      {test_notecard_requestAndResponseWithRetry_does_not_modify_j_object_parameter_value_before_passing_to_note_c, "test_notecard_requestAndResponseWithRetry_does_not_modify_j_object_parameter_value_before_passing_to_note_c"},
      {test_notecard_requestAndResponseWithRetry_does_not_modify_timeout_parameter_value_before_passing_to_note_c, "test_notecard_requestAndResponseWithRetry_does_not_modify_timeout_parameter_value_before_passing_to_note_c"},
      {test_notecard_requestAndResponseWithRetry_does_not_modify_note_c_result_value_before_returning_to_caller, "test_notecard_requestAndResponseWithRetry_does_not_modify_note_c_result_value_before_returning_to_caller"},
      {test_notecard_deleteResponse_does_not_modify_j_object_parameter_pointer_before_passing_to_note_c, "test_notecard_deleteResponse_does_not_modify_j_object_parameter_pointer_before_passing_to_note_c"},
      {test_notecard_logDebug_does_not_modify_string_parameter_value_before_passing_to_note_c, "test_notecard_logDebug_does_not_modify_string_parameter_value_before_passing_to_note_c"},
      {test_notecard_logDebugf_does_not_modify_string_parameter_value_before_passing_to_note_c, "test_notecard_logDebugf_does_not_modify_string_parameter_value_before_passing_to_note_c"},
      {test_notecard_debugSyncStatus_does_not_modify_pollFrequencyMs_parameter_before_passing_to_note_c, "test_notecard_debugSyncStatus_does_not_modify_pollFrequencyMs_parameter_before_passing_to_note_c"},
      {test_notecard_debugSyncStatus_does_not_modify_maxLevel_parameter_before_passing_to_note_c, "test_notecard_debugSyncStatus_does_not_modify_maxLevel_parameter_before_passing_to_note_c"},
      {test_notecard_debugSyncStatus_does_not_modify_note_c_result_value_before_returning_to_caller, "test_notecard_debugSyncStatus_does_not_modify_note_c_result_value_before_returning_to_caller"},
      {test_notecard_responseError_does_not_modify_j_object_parameter_value_before_passing_to_note_c, "test_notecard_responseError_does_not_modify_j_object_parameter_value_before_passing_to_note_c"},
      {test_notecard_responseError_does_not_modify_note_c_result_value_before_returning_to_caller, "test_notecard_responseError_does_not_modify_note_c_result_value_before_returning_to_caller"},
      {test_notecard_newCommand_does_not_modify_string_parameter_value_before_passing_to_note_c, "test_notecard_newCommand_does_not_modify_string_parameter_value_before_passing_to_note_c"},
      {test_notecard_newCommand_does_not_modify_note_c_result_value_before_returning_to_caller, "test_notecard_newCommand_does_not_modify_note_c_result_value_before_returning_to_caller"},
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
      {test_static_callback_note_i2c_transmit_does_not_modify_size_parameter_before_passing_to_interface_method, "test_static_callback_note_i2c_transmit_does_not_modify_size_parameter_before_passing_to_interface_method"},
      {test_static_callback_note_i2c_transmit_does_not_modify_interface_method_return_value, "test_static_callback_note_i2c_transmit_does_not_modify_interface_method_return_value"},
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
      {test_static_callback_note_txn_start_invokes_notetxn_start, "test_static_callback_note_txn_start_invokes_notetxn_start"},
      {test_static_callback_note_txn_start_does_not_modify_timeout_ms_parameter_before_passing_to_interface_method, "test_static_callback_note_txn_start_does_not_modify_timeout_ms_parameter_before_passing_to_interface_method"},
      {test_static_callback_note_txn_start_does_not_modify_interface_method_return_value, "test_static_callback_note_txn_start_does_not_modify_interface_method_return_value"},
      {test_static_callback_note_txn_start_does_not_call_interface_method_when_interface_has_not_been_instantiated, "test_static_callback_note_txn_start_does_not_call_interface_method_when_interface_has_not_been_instantiated"},
      {test_static_callback_note_txn_start_returns_true_when_interface_has_not_been_instantiated, "test_static_callback_note_txn_start_returns_true_when_interface_has_not_been_instantiated"},
      {test_static_callback_note_txn_stop_invokes_notetxn_stop, "test_static_callback_note_txn_stop_invokes_notetxn_stop"},
      {test_static_callback_note_txn_stop_does_not_call_interface_method_when_interface_has_not_been_instantiated, "test_static_callback_note_txn_stop_does_not_call_interface_method_when_interface_has_not_been_instantiated"},
  };

  return TestFunction::runTests(tests, (sizeof(tests) / sizeof(TestFunction)));
}
