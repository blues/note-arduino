#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "Notecard.h"
#include "TestFunction.hpp"
#include "mock/mock-arduino.hpp"
#include "mock/mock-parameters.hpp"

// Compile command: g++ -Wall -Wextra -Wpedantic mock/mock-arduino.cpp mock/mock-note-c-note.c ../src/Notecard.cpp ../src/NoteSerial_Arduino.cpp Notecard.test.cpp -std=c++14 -I. -I../src -DMOCK && ./a.out || echo "Tests Result: $?"

int test_notecard_begin_i2c_shares_a_memory_allocation_functon_pointer()
{
  int result;

  // Arrange
  Notecard notecard;

  noteSetFnDefault_Parameters.reset();

  // Action
  notecard.begin(0x1B, 79);

  // Assert
  if (noteSetFnDefault_Parameters.mallocfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDefault_Parameters.mallocfn == " << !!noteSetFnDefault_Parameters.mallocfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_shares_a_memory_free_functon_pointer()
{
  int result;

  // Arrange
  Notecard notecard;

  noteSetFnDefault_Parameters.reset();

  // Action
  notecard.begin(0x1B, 79);

  // Assert
  if (noteSetFnDefault_Parameters.freefn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDefault_Parameters.freefn == " << !!noteSetFnDefault_Parameters.freefn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_shares_a_delay_functon_pointer()
{
  int result;

  // Arrange
  Notecard notecard;

  noteSetFnDefault_Parameters.reset();

  // Action
  notecard.begin(0x1B, 79);

  // Arrange
  if (noteSetFnDefault_Parameters.delayfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDefault_Parameters.delayfn == " << !!noteSetFnDefault_Parameters.delayfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_shares_a_millis_functon_pointer()
{
  int result;

  // Arrange
  Notecard notecard;

  noteSetFnDefault_Parameters.reset();

  // Action
  notecard.begin(0x1B, 79);

  // Assert
  if (noteSetFnDefault_Parameters.millisfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDefault_Parameters.millisfn == " << !!noteSetFnDefault_Parameters.millisfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_does_not_modify_i2c_address_parameter_before_passing_to_note_c()
{
  int result;

  // Arrange
  Notecard notecard;
  // 0x1B binary representation => 0001 1011
  const uint16_t EXPECTED_ADDRESS = 0x1B;

  noteSetFnI2C_Parameters.reset();

  // Action
  notecard.begin(EXPECTED_ADDRESS, 79);

  // Assert
  if (EXPECTED_ADDRESS == noteSetFnI2C_Parameters.i2caddr)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2C_Parameters.i2caddr == " << noteSetFnI2C_Parameters.i2caddr << ", EXPECTED: " << EXPECTED_ADDRESS << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_does_not_modify_i2c_max_parameter_before_passing_to_note_c()
{
  int result;

  // Arrange
  Notecard notecard;
  const uint32_t EXPECTED_RESULT = 79;

  noteSetFnI2C_Parameters.reset();

  // Action
  notecard.begin(0x1B, EXPECTED_RESULT);

  // Assert
  if (EXPECTED_RESULT == noteSetFnI2C_Parameters.i2cmax)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2C_Parameters.i2cmax == " << noteSetFnI2C_Parameters.i2cmax << ", EXPECTED: " << EXPECTED_RESULT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_shares_an_i2c_reset_functon_pointer()
{
  int result;

  // Arrange
  Notecard notecard;

  noteSetFnI2C_Parameters.reset();

  // Action
  notecard.begin(0x1B, 79);

  // Assert
  if (noteSetFnI2C_Parameters.resetfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2C_Parameters.resetfn == " << !!noteSetFnI2C_Parameters.resetfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_shares_an_i2c_transmit_functon_pointer()
{
  int result;

  // Arrange
  Notecard notecard;

  noteSetFnI2C_Parameters.reset();

  // Action
  notecard.begin(0x1B, 79);

  // Assert
  if (noteSetFnI2C_Parameters.transmitfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2C_Parameters.transmitfn == " << !!noteSetFnI2C_Parameters.transmitfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_shares_an_i2c_receive_functon_pointer()
{
  int result;

  // Arrange
  Notecard notecard;

  noteSetFnI2C_Parameters.reset();

  // Action
  notecard.begin(0x1B, 79);

  // Assert
  if (noteSetFnI2C_Parameters.receivefn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2C_Parameters.receivefn == " << !!noteSetFnI2C_Parameters.receivefn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_default_parameter_for_i2c_max_is_passed_to_note_c()
{
  int result;

  // Arrange
  Notecard notecard;

  noteSetFnI2C_Parameters.reset();
  noteSetFnI2C_Parameters.i2cmax = (NOTE_I2C_MAX_DEFAULT - 1);

  // Action
  notecard.begin(0x1B);

  // Assert
  if (NOTE_I2C_MAX_DEFAULT == noteSetFnI2C_Parameters.i2cmax)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2C_Parameters.i2cmax == " << noteSetFnI2C_Parameters.i2cmax << ", EXPECTED: " << NOTE_I2C_MAX_DEFAULT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_default_parameter_for_i2c_address_is_passed_to_note_c()
{
  int result;

  // Arrange
  Notecard notecard;

  noteSetFnI2C_Parameters.reset();
  noteSetFnI2C_Parameters.i2caddr = (NOTE_I2C_ADDR_DEFAULT - 1);

  // Action
  notecard.begin();

  // Assert
  if (NOTE_I2C_ADDR_DEFAULT == noteSetFnI2C_Parameters.i2caddr)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnI2C_Parameters.i2caddr == " << noteSetFnI2C_Parameters.i2caddr << ", EXPECTED: " << NOTE_I2C_ADDR_DEFAULT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_shares_a_memory_allocation_functon_pointer()
{
  int result;

  // Arrange
  Notecard notecard;

  noteSetFnDefault_Parameters.reset();

  // Action
  notecard.begin(Serial, 9600);

  // Assert
  if (noteSetFnDefault_Parameters.mallocfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDefault_Parameters.mallocfn == " << !!noteSetFnDefault_Parameters.mallocfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_shares_a_memory_free_functon_pointer()
{
  int result;

  // Arrange
  Notecard notecard;

  noteSetFnDefault_Parameters.reset();

  // Action
  notecard.begin(Serial, 9600);

  // Assert
  if (noteSetFnDefault_Parameters.freefn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDefault_Parameters.freefn == " << !!noteSetFnDefault_Parameters.freefn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_shares_a_delay_functon_pointer()
{
  int result;

  // Arrange
  Notecard notecard;

  noteSetFnDefault_Parameters.reset();

  // Action
  notecard.begin(Serial, 9600);

  // Assert
  if (noteSetFnDefault_Parameters.delayfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDefault_Parameters.delayfn == " << !!noteSetFnDefault_Parameters.delayfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_shares_a_millis_functon_pointer()
{
  int result;

  // Arrange
  Notecard notecard;

  noteSetFnDefault_Parameters.reset();

  // Action
  notecard.begin(Serial, 9600);

  // Assert
  if (nullptr != noteSetFnDefault_Parameters.millisfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDefault_Parameters.millisfn == " << !!noteSetFnDefault_Parameters.millisfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_shares_a_serial_reset_functon_pointer()
{
  int result;

  // Arrange
  Notecard notecard;

  noteSetFnSerial_Parameters.reset();

  // Action
  notecard.begin(Serial, 9600);

  // Assert
  if (noteSetFnSerial_Parameters.resetfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnSerial_Parameters.resetfn == " << !!noteSetFnSerial_Parameters.resetfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_shares_a_serial_transmit_functon_pointer()
{
  int result;

  // Arrange
  Notecard notecard;

  noteSetFnSerial_Parameters.reset();

  // Action
  notecard.begin(Serial, 9600);

  // Assert
  if (noteSetFnSerial_Parameters.writefn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnSerial_Parameters.writefn == " << !!noteSetFnSerial_Parameters.writefn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_shares_a_serial_available_functon_pointer()
{
  int result;

  // Arrange
  Notecard notecard;

  noteSetFnSerial_Parameters.reset();

  // Action
  notecard.begin(Serial, 9600);

  // Assert
  if (noteSetFnSerial_Parameters.availfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnSerial_Parameters.availfn == " << !!noteSetFnSerial_Parameters.availfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_shares_a_serial_receive_functon_pointer()
{
  int result;

  // Arrange
  Notecard notecard;

  noteSetFnSerial_Parameters.reset();

  // Action
  notecard.begin(Serial, 9600);

  // Assert
  if (noteSetFnSerial_Parameters.readfn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnSerial_Parameters.readfn == " << !!noteSetFnSerial_Parameters.readfn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_initializes_the_provided_serial_interface_with_provided_speed()
{
  int result;

  // Arrange
  Notecard notecard;
  const unsigned int EXPECTED_BAUD_RATE = 9600;

  hardwareSerialBegin_Parameters.reset();

  // Action
  notecard.begin(Serial, EXPECTED_BAUD_RATE);

  // Assert
  if (EXPECTED_BAUD_RATE == hardwareSerialBegin_Parameters.baud)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\thardwareSerialBegin_Parameters.baud == " << hardwareSerialBegin_Parameters.baud << ", EXPECTED: " << EXPECTED_BAUD_RATE << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_setDebugOutputStream_shares_a_debug_log_functon_pointer()
{
  int result;

  // Arrange
  Notecard notecard;

  noteSetFnDebugOutput_Parameters.reset();

  // Action
  notecard.setDebugOutputStream(Serial);

  // Assert
  if (noteSetFnDebugOutput_Parameters.fn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDebugOutput_Parameters.fn == " << !!noteSetFnDebugOutput_Parameters.fn << ", EXPECTED: not 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_clearDebugOutputStream_clears_the_debug_log_functon_pointer()
{
  int result;

  // Arrange
  Notecard notecard;

  noteSetFnDebugOutput_Parameters.reset();
  noteSetFnDebugOutput_Parameters.fn = reinterpret_cast<debugOutputFn>(&result);

  // Action
  notecard.clearDebugOutputStream();

  // Assert
  if (!noteSetFnDebugOutput_Parameters.fn)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteSetFnDebugOutput_Parameters.fn == " << !!noteSetFnDebugOutput_Parameters.fn << ", EXPECTED: 0 (`nullptr`)" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_newRequest_does_not_modify_string_parameter_value_before_passing_to_note_c()
{
  int result;

  // Arrange
  Notecard notecard;
  const char * const EXPECTED_RESULT = "Hello, Test!";

  noteNewRequest_Parameters.reset();

  // Action
  notecard.newRequest(EXPECTED_RESULT);

  // Assert
  if (!strcmp(EXPECTED_RESULT, noteNewRequest_Parameters.request_cache.c_str()))
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteNewRequest_Parameters.request_cache.c_str() == \"" << noteNewRequest_Parameters.request_cache.c_str() << "\", EXPECTED: \"" << EXPECTED_RESULT << "\"" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_newRequest_does_not_modify_note_c_result_value_before_returning_to_caller()
{
  int result;

  // Arrange
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
  const J * const ACTUAL_RESULT = notecard.newRequest(nullptr);

  // Assert
  if (!memcmp(ACTUAL_RESULT, EXPECTED_JSON, sizeof(J)) /*JCompare(EXPECTED_JSON, json_result, false)*/)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
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
  notecard.sendRequest(json_cpy);

  // Assert
  if (!memcmp(EXPECTED_JSON, noteRequest_Parameters.req, sizeof(J)) /*JCompare(EXPECTED_JSON, noteSendRequest_Parameters.req, false)*/)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
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
  Notecard notecard;
  const bool EXPECTED_RESULT = true;

  noteRequest_Parameters.reset();
  noteRequest_Parameters.result = EXPECTED_RESULT;

  // Action
  const bool ACTUAL_RESULT = notecard.sendRequest(nullptr);

  // Assert
  if (EXPECTED_RESULT == ACTUAL_RESULT)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnotecard.sendRequest(nullptr) == \"" << ACTUAL_RESULT << "\", EXPECTED: \"" << EXPECTED_RESULT << "\"" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_requestAndResponse_does_not_modify_j_object_parameter_value_before_passing_to_note_c()
{
  int result;

  // Arrange
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
  notecard.requestAndResponse(json_cpy);

  // Assert
  if (!memcmp(EXPECTED_JSON, noteRequestResponse_Parameters.req, sizeof(J)) /*JCompare(EXPECTED_JSON, noteRequestResponse_Parameters.req, false)*/)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
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
  const J * const ACTUAL_RESULT = notecard.requestAndResponse(nullptr);

  // Assert
  if (!memcmp(EXPECTED_JSON, ACTUAL_RESULT, sizeof(J)) /*JCompare(EXPECTED_JSON, json_result, false)*/)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
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
  Notecard notecard;
  J *const EXPECTED_POINTER = reinterpret_cast<J *>(0x19790917);

  noteDeleteResponse_Parameters.reset();

  // Action
  notecard.deleteResponse(EXPECTED_POINTER);

  // Assert
  if (EXPECTED_POINTER == noteDeleteResponse_Parameters.response)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteDeleteResponse_Parameters.response == " << std::hex << noteDeleteResponse_Parameters.response << ", EXPECTED: " << EXPECTED_POINTER << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_logDebug_does_not_modify_string_parameter_value_before_passing_to_note_c()
{
  int result;

  // Arrange
  const char str[] = "Hello, Test!";
  Notecard notecard;

  noteDebug_Parameters.reset();

  // Action
  notecard.logDebug(str);

  // Assert
  if (!strcmp(str, noteDebug_Parameters.message_cache.c_str()))
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteDebug_Parameters.message_cache.c_str() == \"" << noteDebug_Parameters.message_cache.c_str() << "\", EXPECTED: \"" << str << "\"" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_logDebugf_does_not_modify_string_parameter_value_before_passing_to_note_c()
{
  int result;

  // Arrange
  const char str[] = "Hello, Test 123!";
  Notecard notecard;

  noteDebug_Parameters.reset();

  // Action
  notecard.logDebugf("Hello, %s %d%c", "Test", 123, '!');

  // Assert
  if (!strcmp(str, noteDebug_Parameters.message_cache.c_str()))
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteDebug_Parameters.message_cache.c_str() == \"" << noteDebug_Parameters.message_cache.c_str() << "\", EXPECTED: \"" << str << "\"" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_debugSyncStatus_does_not_modify_pollFrequencyMs_parameter_before_passing_to_note_c()
{
  int result;

  // Arrange
  Notecard notecard;
  const int EXPECTED_RESULT = 79;

  noteDebugSyncStatus_Parameters.reset();

  // Action
  notecard.debugSyncStatus(EXPECTED_RESULT, 0);

  // Assert
  if (EXPECTED_RESULT == noteDebugSyncStatus_Parameters.pollFrequencyMs)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteDebugSyncStatus_Parameters.pollFrequencyMs == " << noteDebugSyncStatus_Parameters.pollFrequencyMs << ", EXPECTED: " << EXPECTED_RESULT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_debugSyncStatus_does_not_modify_maxLevel_parameter_before_passing_to_note_c()
{
  int result;

  // Arrange
  Notecard notecard;
  const int EXPECTED_RESULT = 79;

  noteDebugSyncStatus_Parameters.reset();

  // Action
  notecard.debugSyncStatus(0, EXPECTED_RESULT);

  // Assert
  if (EXPECTED_RESULT == noteDebugSyncStatus_Parameters.maxLevel)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteDebugSyncStatus_Parameters.maxLevel == " << noteDebugSyncStatus_Parameters.maxLevel << ", EXPECTED: " << EXPECTED_RESULT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_debugSyncStatus_does_not_modify_note_c_result_value_before_returning_to_caller()
{
  int result;

  // Arrange
  Notecard notecard;
  const bool EXPECTED_RESULT = true;

  noteDebugSyncStatus_Parameters.reset();
  noteDebugSyncStatus_Parameters.result = EXPECTED_RESULT;

  // Action
  const bool ACTUAL_RESULT = notecard.debugSyncStatus(0, 0);

  // Assert
  if (EXPECTED_RESULT == ACTUAL_RESULT)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnotecard.debugSyncStatus(0, 0) == " << ACTUAL_RESULT << ", EXPECTED: " << EXPECTED_RESULT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_responseError_does_not_modify_j_object_parameter_value_before_passing_to_note_c()
{
  int result;

  // Arrange
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
  notecard.responseError(json_cpy);

  // Assert
  if (!memcmp(EXPECTED_JSON, noteResponseError_Parameters.rsp, sizeof(J)) /*JCompare(EXPECTED_JSON, noteRequestResponse_Parameters.rsp, false)*/)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
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
  Notecard notecard;
  const bool EXPECTED_RESULT = true;

  noteResponseError_Parameters.reset();
  noteResponseError_Parameters.result = EXPECTED_RESULT;

  // Action
  const bool ACTUAL_RESULT = notecard.responseError(nullptr);

  // Assert
  if (EXPECTED_RESULT == ACTUAL_RESULT)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnotecard.responseError(nullptr) == " << ACTUAL_RESULT << ", EXPECTED: " << EXPECTED_RESULT << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_default_parameter_for_wirePort_has_begin_method_called()
{
  int result;

  // Arrange
  Notecard notecard;

  twoWireBegin_Parameters.reset();

  // Action
  notecard.begin();

  // Assert
  if (twoWireBegin_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\ttwoWireBegin_Parameters.invoked == " << !!twoWireBegin_Parameters.invoked << ", EXPECTED: " << true << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_i2c_parameter_for_wirePort_has_begin_method_called()
{
  int result;

  // Arrange
  Notecard notecard;
  TwoWire mockWire;

  twoWireBegin_Parameters.reset();

  // Action
  notecard.begin(NOTE_I2C_ADDR_DEFAULT, NOTE_I2C_MAX_DEFAULT, mockWire);

  // Assert
  if (twoWireBegin_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\ttwoWireBegin_Parameters.invoked == " << !!twoWireBegin_Parameters.invoked << ", EXPECTED: " << true << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_begin_serial_default_parameter_for_baud_rate_is_passed_to_note_c()
{
  int result;

  // Arrange
  Notecard notecard;
  const unsigned int EXPECTED_BAUD_RATE = 9600;

  hardwareSerialBegin_Parameters.reset();

  // Action
  notecard.begin(Serial);

  // Assert
  if (EXPECTED_BAUD_RATE == hardwareSerialBegin_Parameters.baud)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\thardwareSerialBegin_Parameters.baud == " << hardwareSerialBegin_Parameters.baud << ", EXPECTED: " << EXPECTED_BAUD_RATE << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_newCommand_does_not_modify_string_parameter_value_before_passing_to_note_c()
{
  int result;

  // Arrange
  Notecard notecard;
  const char EXPECTED_RESULT[] = "Hello, Test!";

  noteNewCommand_Parameters.reset();

  // Action
  notecard.newCommand(EXPECTED_RESULT);

  // Assert
  if (!strcmp(EXPECTED_RESULT, noteNewCommand_Parameters.request_cache.c_str()))
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnoteNewCommand_Parameters.request_cache.c_str() == \"" << noteNewCommand_Parameters.request_cache.c_str() << "\", EXPECTED: \"" << EXPECTED_RESULT << "\"" << std::endl;
    std::cout << "[";
  }

  return result;
}

int test_notecard_newCommand_does_not_modify_note_c_result_value_before_returning_to_caller()
{
  int result;

  // Arrange
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
  const J * const ACTUAL_RESULT = notecard.newCommand(nullptr);

  // Assert
  if (!memcmp(EXPECTED_JSON, ACTUAL_RESULT, sizeof(J)) /*JCompare(EXPECTED_JSON, ACTUAL_RESULT, false)*/)
  {
    result = 0;
  }
  else
  {
    result = static_cast<int>('n' + 'o' + 't' + 'e' + 'c' + 'a' + 'r' + 'd');
    std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "\tnotecard.newCommand(nullptr) != EXPECTED_JSON" << std::endl;
    std::cout << "[";
  }

  free(noteNewCommand_Parameters.result);
  free(EXPECTED_JSON);
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
      {test_notecard_begin_serial_shares_a_memory_allocation_functon_pointer, "test_notecard_begin_serial_shares_a_memory_allocation_functon_pointer"},
      {test_notecard_begin_serial_shares_a_memory_free_functon_pointer, "test_notecard_begin_serial_shares_a_memory_free_functon_pointer"},
      {test_notecard_begin_serial_shares_a_delay_functon_pointer, "test_notecard_begin_serial_shares_a_delay_functon_pointer"},
      {test_notecard_begin_serial_shares_a_millis_functon_pointer, "test_notecard_begin_serial_shares_a_millis_functon_pointer"},
      {test_notecard_begin_serial_shares_a_serial_reset_functon_pointer, "test_notecard_begin_serial_shares_a_serial_reset_functon_pointer"},
      {test_notecard_begin_serial_shares_a_serial_transmit_functon_pointer, "test_notecard_begin_serial_shares_a_serial_transmit_functon_pointer"},
      {test_notecard_begin_serial_shares_a_serial_available_functon_pointer, "test_notecard_begin_serial_shares_a_serial_available_functon_pointer"},
      {test_notecard_begin_serial_shares_a_serial_receive_functon_pointer, "test_notecard_begin_serial_shares_a_serial_receive_functon_pointer"},
      {test_notecard_begin_serial_initializes_the_provided_serial_interface_with_provided_speed, "test_notecard_begin_serial_initializes_the_provided_serial_interface_with_provided_speed"},
      {test_notecard_setDebugOutputStream_shares_a_debug_log_functon_pointer, "test_notecard_setDebugOutputStream_shares_a_debug_log_functon_pointer"},
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
      {test_notecard_begin_i2c_default_parameter_for_wirePort_has_begin_method_called, "test_notecard_begin_i2c_default_parameter_for_wirePort_has_begin_method_called"},
      {test_notecard_begin_i2c_parameter_for_wirePort_has_begin_method_called, "test_notecard_begin_i2c_parameter_for_wirePort_has_begin_method_called"},
      {test_notecard_begin_serial_default_parameter_for_baud_rate_is_passed_to_note_c, "test_notecard_begin_serial_default_parameter_for_baud_rate_is_passed_to_note_c"},
      {test_notecard_newCommand_does_not_modify_string_parameter_value_before_passing_to_note_c, "test_notecard_newRequest_does_not_modify_string_parameter_value_before_passing_to_note_c"},
      {test_notecard_newCommand_does_not_modify_note_c_result_value_before_returning_to_caller, "test_notecard_newRequest_does_not_modify_note_c_result_value_before_returning_to_caller"},
  };

  return TestFunction::runTests(tests, (sizeof(tests) / sizeof(TestFunction)));
}
