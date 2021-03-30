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
  Notecard notecard;

  // Setup
  noteSetFnDefault_Parameters.mallocfn = nullptr;

  // Action
  notecard.begin(0x1B, 79);

  // Evaluate Result
  if (nullptr != noteSetFnDefault_Parameters.mallocfn)
  {
    result = 0;
  }
  else
  {
    result = 1;
  }

  return result;
}

int test_notecard_begin_i2c_shares_a_memory_free_functon_pointer()
{
  int result;
  Notecard notecard;

  // Setup
  noteSetFnDefault_Parameters.freefn = nullptr;

  // Action
  notecard.begin(0x1B, 79);

  // Evaluate Result
  if (nullptr != noteSetFnDefault_Parameters.freefn)
  {
    result = 0;
  }
  else
  {
    result = 2;
  }

  return result;
}

int test_notecard_begin_i2c_shares_a_delay_functon_pointer()
{
  int result;
  Notecard notecard;

  // Setup
  noteSetFnDefault_Parameters.delayfn = nullptr;

  // Action
  notecard.begin(0x1B, 79);

  // Evaluate Result
  if (nullptr != noteSetFnDefault_Parameters.delayfn)
  {
    result = 0;
  }
  else
  {
    result = 3;
  }

  return result;
}

int test_notecard_begin_i2c_shares_a_millis_functon_pointer()
{
  int result;
  Notecard notecard;

  // Setup
  noteSetFnDefault_Parameters.millisfn = nullptr;

  // Action
  notecard.begin(0x1B, 79);

  // Evaluate Result
  if (nullptr != noteSetFnDefault_Parameters.millisfn)
  {
    result = 0;
  }
  else
  {
    result = 4;
  }

  return result;
}

int test_notecard_begin_i2c_does_not_modify_i2c_address_parameter_before_passing_to_note_c()
{
  int result;
  Notecard notecard;
  // 0x1B binary representation => 0001 1011
  const uint16_t i2c_addr = 0x1B;

  // Setup
  noteSetFnI2C_Parameters.i2caddr = (i2c_addr - 1);

  // Action
  notecard.begin(i2c_addr, 79);

  // Evaluate Result
  if (i2c_addr == noteSetFnI2C_Parameters.i2caddr)
  {
    result = 0;
  }
  else
  {
    result = 5;
  }

  return result;
}

int test_notecard_begin_i2c_does_not_modify_i2c_max_parameter_before_passing_to_note_c()
{
  int result;
  Notecard notecard;
  const uint32_t i2c_max = 79;

  // Setup
  noteSetFnI2C_Parameters.i2cmax = (i2c_max - 1);

  // Action
  notecard.begin(0x1B, i2c_max);

  // Evaluate Result
  if (i2c_max == noteSetFnI2C_Parameters.i2cmax)
  {
    result = 0;
  }
  else
  {
    result = 6;
  }

  return result;
}

int test_notecard_begin_i2c_shares_an_i2c_reset_functon_pointer()
{
  int result;
  Notecard notecard;

  // Setup
  noteSetFnI2C_Parameters.resetfn = nullptr;

  // Action
  notecard.begin(0x1B, 79);

  // Evaluate Result
  if (nullptr != noteSetFnI2C_Parameters.resetfn)
  {
    result = 0;
  }
  else
  {
    result = 7;
  }

  return result;
}

int test_notecard_begin_i2c_shares_an_i2c_transmit_functon_pointer()
{
  int result;
  Notecard notecard;

  // Setup
  noteSetFnI2C_Parameters.transmitfn = nullptr;

  // Action
  notecard.begin(0x1B, 79);

  // Evaluate Result
  if (nullptr != noteSetFnI2C_Parameters.transmitfn)
  {
    result = 0;
  }
  else
  {
    result = 8;
  }

  return result;
}

int test_notecard_begin_i2c_shares_an_i2c_receive_functon_pointer()
{
  int result;
  Notecard notecard;

  // Setup
  noteSetFnI2C_Parameters.receivefn = nullptr;

  // Action
  notecard.begin(0x1B, 79);

  // Evaluate Result
  if (nullptr != noteSetFnI2C_Parameters.receivefn)
  {
    result = 0;
  }
  else
  {
    result = 9;
  }

  return result;
}

int test_notecard_begin_i2c_default_parameter_for_i2c_max_is_passed_to_note_c()
{
  int result;
  Notecard notecard;

  // Setup
  noteSetFnI2C_Parameters.i2cmax = (NOTE_I2C_MAX_DEFAULT - 1);

  // Action
  notecard.begin(0x1B);

  // Evaluate Result
  if (NOTE_I2C_MAX_DEFAULT == noteSetFnI2C_Parameters.i2cmax)
  {
    result = 0;
  }
  else
  {
    result = 10;
  }

  return result;
}

int test_notecard_begin_i2c_default_parameter_for_i2c_address_is_passed_to_note_c()
{
  int result;
  Notecard notecard;

  // Setup
  noteSetFnI2C_Parameters.i2caddr = (NOTE_I2C_ADDR_DEFAULT - 1);

  // Action
  notecard.begin();

  // Evaluate Result
  if (NOTE_I2C_ADDR_DEFAULT == noteSetFnI2C_Parameters.i2caddr)
  {
    result = 0;
  }
  else
  {
    result = 11;
  }

  return result;
}

int test_notecard_begin_serial_shares_a_memory_allocation_functon_pointer()
{
  int result;
  Notecard notecard;

  // Setup
  noteSetFnDefault_Parameters.mallocfn = nullptr;

  // Action
  notecard.begin(Serial, 9600);

  // Evaluate Result
  if (nullptr != noteSetFnDefault_Parameters.mallocfn)
  {
    result = 0;
  }
  else
  {
    result = 12;
  }

  return result;
}

int test_notecard_begin_serial_shares_a_memory_free_functon_pointer()
{
  int result;
  Notecard notecard;

  // Setup
  noteSetFnDefault_Parameters.freefn = nullptr;

  // Action
  notecard.begin(Serial, 9600);

  // Evaluate Result
  if (nullptr != noteSetFnDefault_Parameters.freefn)
  {
    result = 0;
  }
  else
  {
    result = 13;
  }

  return result;
}

int test_notecard_begin_serial_shares_a_delay_functon_pointer()
{
  int result;
  Notecard notecard;

  // Setup
  noteSetFnDefault_Parameters.delayfn = nullptr;

  // Action
  notecard.begin(Serial, 9600);

  // Evaluate Result
  if (nullptr != noteSetFnDefault_Parameters.delayfn)
  {
    result = 0;
  }
  else
  {
    result = 14;
  }

  return result;
}

int test_notecard_begin_serial_shares_a_millis_functon_pointer()
{
  int result;
  Notecard notecard;

  // Setup
  noteSetFnDefault_Parameters.millisfn = nullptr;

  // Action
  notecard.begin(Serial, 9600);

  // Evaluate Result
  if (nullptr != noteSetFnDefault_Parameters.millisfn)
  {
    result = 0;
  }
  else
  {
    result = 15;
  }

  return result;
}

int test_notecard_begin_serial_shares_a_serial_reset_functon_pointer()
{
  int result;
  Notecard notecard;

  // Setup
  noteSetFnSerial_Parameters.resetfn = nullptr;

  // Action
  notecard.begin(Serial, 9600);

  // Evaluate Result
  if (nullptr != noteSetFnSerial_Parameters.resetfn)
  {
    result = 0;
  }
  else
  {
    result = 16;
  }

  return result;
}

int test_notecard_begin_serial_shares_a_serial_transmit_functon_pointer()
{
  int result;
  Notecard notecard;

  // Setup
  noteSetFnSerial_Parameters.writefn = nullptr;

  // Action
  notecard.begin(Serial, 9600);

  // Evaluate Result
  if (nullptr != noteSetFnSerial_Parameters.writefn)
  {
    result = 0;
  }
  else
  {
    result = 17;
  }

  return result;
}

int test_notecard_begin_serial_shares_a_serial_available_functon_pointer()
{
  int result;
  Notecard notecard;

  // Setup
  noteSetFnSerial_Parameters.availfn = nullptr;

  // Action
  notecard.begin(Serial, 9600);

  // Evaluate Result
  if (nullptr != noteSetFnSerial_Parameters.availfn)
  {
    result = 0;
  }
  else
  {
    result = 18;
  }

  return result;
}

int test_notecard_begin_serial_shares_a_serial_receive_functon_pointer()
{
  int result;
  Notecard notecard;

  // Setup
  noteSetFnSerial_Parameters.readfn = nullptr;

  // Action
  notecard.begin(Serial, 9600);

  // Evaluate Result
  if (nullptr != noteSetFnSerial_Parameters.readfn)
  {
    result = 0;
  }
  else
  {
    result = 19;
  }

  return result;
}

int test_notecard_begin_serial_initializes_the_provided_serial_interface_with_provided_speed()
{
  int result;
  Notecard notecard;
  const unsigned int baud = 9600;

  // Setup
  hardwareSerialBegin_Parameters.baud = (baud - 1);

  // Action
  notecard.begin(Serial, baud);

  // Evaluate Result
  if (baud == hardwareSerialBegin_Parameters.baud)
  {
    result = 0;
  }
  else
  {
    result = 20;
  }

  return result;
}

int test_notecard_setDebugOutputStream_shares_a_debug_log_functon_pointer()
{
  int result;
  Notecard notecard;

  // Setup
  noteSetFnDebugOutput_Parameters.fn = nullptr;

  // Action
  notecard.setDebugOutputStream(dbgserial);

  // Evaluate Result
  if (nullptr != noteSetFnDebugOutput_Parameters.fn)
  {
    result = 0;
  }
  else
  {
    result = 21;
  }

  return result;
}

int test_notecard_clearDebugOutputStream_clears_the_debug_log_functon_pointer()
{
  int result;
  Notecard notecard;

  // Setup
  noteSetFnDebugOutput_Parameters.fn = reinterpret_cast<debugOutputFn>(&result);

  // Action
  notecard.clearDebugOutputStream();

  // Evaluate Result
  if (nullptr == noteSetFnDebugOutput_Parameters.fn)
  {
    result = 0;
  }
  else
  {
    result = 22;
  }

  return result;
}

int test_notecard_newRequest_does_not_modify_string_parameter_value_before_passing_to_note_c()
{
  int result;
  Notecard notecard;
  const char *str = "Hello, Test 23!";
  char str_copy[32];

  // Setup
  strcpy(str_copy, str);
  noteNewRequest_Parameters.request = nullptr;
  noteNewRequest_Parameters.result = nullptr;

  // Action
  notecard.newRequest(str_copy);

  // Evaluate Result
  if (0 == strcmp(str, noteNewRequest_Parameters.request))
  {
    result = 0;
  }
  else
  {
    result = 23;
  }

  return result;
}

int test_notecard_newRequest_does_not_modify_note_c_result_value_before_returning_to_caller()
{
  int result;
  Notecard notecard;
  J *json_obj = reinterpret_cast<J *>(malloc(sizeof(J))); // JCreateObject();
  assert(nullptr != json_obj);

  // Setup
  memset(json_obj, 0x55, sizeof(J)); //assert(nullptr != JAddStringToObject(json_obj,"key1", "value1"));
  {
    noteNewRequest_Parameters.result = reinterpret_cast<J *>(malloc(sizeof(J)));
    assert(nullptr != noteNewRequest_Parameters.result);
    memcpy(noteNewRequest_Parameters.result, json_obj, sizeof(J));
  } //assert(nullptr != (noteNewRequest_Parameters.result = JDuplicate(json_obj,true)));

  // Action
  J *json_result = notecard.newRequest(nullptr);

  // Evaluate Result
  if (0 == memcmp(json_obj, json_result, sizeof(J)) /*JCompare(json_obj, json_result, false)*/)
  {
    result = 0;
  }
  else
  {
    result = 24;
  }

  free(noteNewRequest_Parameters.result);
  free(json_obj);
  return result;
}

int test_notecard_sendRequest_does_not_modify_j_object_parameter_value_before_passing_to_note_c()
{
  int result;
  Notecard notecard;
  J *json_obj = reinterpret_cast<J *>(malloc(sizeof(J))); // JCreateObject();
  J *json_cpy;
  assert(nullptr != json_obj);

  // Setup
  noteRequest_Parameters.req = nullptr;
  memset(json_obj, 0x55, sizeof(J)); //assert(nullptr != JAddStringToObject(json_obj,"key1", "value1"));
  {
    json_cpy = reinterpret_cast<J *>(malloc(sizeof(J)));
    assert(nullptr != json_cpy);
    memcpy(json_cpy, json_obj, sizeof(J));
  } //assert(nullptr != (json_cpy = JDuplicate(json_obj,true)));

  // Action
  notecard.sendRequest(json_cpy);

  // Evaluate Result
  if (0 == memcmp(json_obj, noteRequest_Parameters.req, sizeof(J)) /*JCompare(json_obj, noteSendRequest_Parameters.req, false)*/)
  {
    result = 0;
  }
  else
  {
    result = 25;
  }

  free(json_cpy);
  free(json_obj);
  return result;
}

int test_notecard_sendRequest_does_not_modify_note_c_result_value_before_returning_to_caller()
{
  int result;
  Notecard notecard;
  const bool expected_result = true;

  // Setup
  noteRequest_Parameters.result = expected_result;

  // Action
  bool send_result = notecard.sendRequest(nullptr);

  // Evaluate Result
  if (expected_result == send_result)
  {
    result = 0;
  }
  else
  {
    result = 26;
  }

  return result;
}

int test_notecard_requestAndResponse_does_not_modify_j_object_parameter_value_before_passing_to_note_c()
{
  int result;
  Notecard notecard;
  J *json_obj = reinterpret_cast<J *>(malloc(sizeof(J))); // JCreateObject();
  J *json_cpy;
  assert(nullptr != json_obj);

  // Setup
  noteRequestResponse_Parameters.req = nullptr;
  memset(json_obj, 0x55, sizeof(J)); //assert(nullptr != JAddStringToObject(json_obj,"key1", "value1"));
  {
    json_cpy = reinterpret_cast<J *>(malloc(sizeof(J)));
    assert(nullptr != json_cpy);
    memcpy(json_cpy, json_obj, sizeof(J));
  } //assert(nullptr != (json_cpy = JDuplicate(json_obj,true)));

  // Action
  notecard.requestAndResponse(json_cpy);

  // Evaluate Result
  if (0 == memcmp(json_obj, noteRequestResponse_Parameters.req, sizeof(J)) /*JCompare(json_obj, noteRequestResponse_Parameters.req, false)*/)
  {
    result = 0;
  }
  else
  {
    result = 27;
  }

  free(json_cpy);
  free(json_obj);
  return result;
}

int test_notecard_requestAndResponse_does_not_modify_note_c_result_value_before_returning_to_caller()
{
  int result;
  Notecard notecard;
  J *json_obj = reinterpret_cast<J *>(malloc(sizeof(J))); // JCreateObject();
  assert(nullptr != json_obj);

  // Setup
  memset(json_obj, 0x55, sizeof(J)); //assert(nullptr != JAddStringToObject(json_obj,"key1", "value1"));
  {
    noteRequestResponse_Parameters.result = reinterpret_cast<J *>(malloc(sizeof(J)));
    assert(nullptr != noteRequestResponse_Parameters.result);
    memcpy(noteRequestResponse_Parameters.result, json_obj, sizeof(J));
  } //assert(nullptr != (noteRequestResponse_Parameters.result = JDuplicate(json_obj,true)));

  // Action
  J *json_result = notecard.requestAndResponse(nullptr);

  // Evaluate Result
  if (0 == memcmp(json_obj, json_result, sizeof(J)) /*JCompare(json_obj, json_result, false)*/)
  {
    result = 0;
  }
  else
  {
    result = 28;
  }

  free(noteRequestResponse_Parameters.result);
  free(json_obj);
  return result;
}

int test_notecard_deleteResponse_does_not_modify_j_object_parameter_pointer_before_passing_to_note_c()
{
  int result;
  Notecard notecard;
  J *const json_obj = reinterpret_cast<J *>(0x19790917);

  // Setup
  noteDeleteResponse_Parameters.response = nullptr;

  // Action
  notecard.deleteResponse(json_obj);

  // Evaluate Result
  if (json_obj == noteDeleteResponse_Parameters.response)
  {
    result = 0;
  }
  else
  {
    result = 29;
  }

  return result;
}

int test_notecard_logDebug_does_not_modify_string_parameter_value_before_passing_to_note_c()
{
  int result;
  Notecard notecard;
  const char str[] = "Hello, Test 30!";
  char str_copy[32];

  // Setup
  strcpy(str_copy, str);
  noteDebug_Parameters.message = nullptr;

  // Action
  notecard.logDebug(str_copy);

  // Evaluate Result
  if (0 == strcmp(str, noteDebug_Parameters.message))
  {
    result = 0;
  }
  else
  {
    result = 30;
  }

  return result;
}

int test_notecard_logDebugf_does_not_modify_string_parameter_value_before_passing_to_note_c()
{
  int result;
  Notecard notecard;
  const char str[] = "Hello, Test 30!";

  // Setup
  noteDebug_Parameters.message = nullptr;

  // Action
  notecard.logDebugf("Hello, %s %d%c", "Test", 30, '!');

  // Evaluate Result
  if (0 == strcmp(str, noteDebug_Parameters.message))
  {
    result = 0;
  }
  else
  {
    result = 31;
  }

  return result;
}

int test_notecard_debugSyncStatus_does_not_modify_pollFrequencyMs_parameter_before_passing_to_note_c()
{
  int result;
  Notecard notecard;
  const int pollFrequencyMs = 79;

  // Setup
  noteDebugSyncStatus_Parameters.pollFrequencyMs = (pollFrequencyMs - 1);

  // Action
  notecard.debugSyncStatus(pollFrequencyMs, 0);

  // Evaluate Result
  if (pollFrequencyMs == noteDebugSyncStatus_Parameters.pollFrequencyMs)
  {
    result = 0;
  }
  else
  {
    result = 32;
  }

  return result;
}

int test_notecard_debugSyncStatus_does_not_modify_maxLevel_parameter_before_passing_to_note_c()
{
  int result;
  Notecard notecard;
  const int maxLevel = 79;

  // Setup
  noteDebugSyncStatus_Parameters.maxLevel = (maxLevel - 1);

  // Action
  notecard.debugSyncStatus(0, maxLevel);

  // Evaluate Result
  if (maxLevel == noteDebugSyncStatus_Parameters.maxLevel)
  {
    result = 0;
  }
  else
  {
    result = 33;
  }

  return result;
}

int test_notecard_debugSyncStatus_does_not_modify_note_c_result_value_before_returning_to_caller()
{
  int result;
  Notecard notecard;
  const bool expected_result = true;

  // Setup
  noteDebugSyncStatus_Parameters.result = expected_result;

  // Action
  bool send_result = notecard.debugSyncStatus(0, 0);

  // Evaluate Result
  if (expected_result == send_result)
  {
    result = 0;
  }
  else
  {
    result = 34;
  }

  return result;
}

int test_notecard_responseError_does_not_modify_j_object_parameter_value_before_passing_to_note_c()
{
  int result;
  Notecard notecard;
  J *json_obj = reinterpret_cast<J *>(malloc(sizeof(J))); // JCreateObject();
  J *json_cpy;
  assert(nullptr != json_obj);

  // Setup
  noteResponseError_Parameters.rsp = nullptr;
  memset(json_obj, 0x55, sizeof(J)); //assert(nullptr != JAddStringToObject(json_obj,"key1", "value1"));
  {
    json_cpy = reinterpret_cast<J *>(malloc(sizeof(J)));
    assert(nullptr != json_cpy);
    memcpy(json_cpy, json_obj, sizeof(J));
  } //assert(nullptr != (json_cpy = JDuplicate(json_obj,true)));

  // Action
  notecard.responseError(json_cpy);

  // Evaluate Result
  if (0 == memcmp(json_obj, noteResponseError_Parameters.rsp, sizeof(J)) /*JCompare(json_obj, noteRequestResponse_Parameters.req, false)*/)
  {
    result = 0;
  }
  else
  {
    result = 35;
  }

  free(json_cpy);
  free(json_obj);
  return result;
}

int test_notecard_responseError_does_not_modify_note_c_result_value_before_returning_to_caller()
{
  int result;
  Notecard notecard;
  const bool expected_result = true;

  // Setup
  noteResponseError_Parameters.result = expected_result;

  // Action
  bool send_result = notecard.responseError(nullptr);

  // Evaluate Result
  if (expected_result == send_result)
  {
    result = 0;
  }
  else
  {
    result = 36;
  }

  return result;
}

int test_notecard_begin_i2c_default_parameter_for_wirePort_has_begin_method_called()
{
  int result;
  Notecard notecard;

  // Setup
  twoWireBegin_Parameters.invoked = 0;

  // Action
  notecard.begin();

  // Evaluate Result
  if (twoWireBegin_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = 37;
  }

  return result;
}

int test_notecard_begin_i2c_parameter_for_wirePort_has_begin_method_called()
{
  int result;
  Notecard notecard;
  TwoWire mockWire;

  // Setup
  twoWireBegin_Parameters.invoked = 0;

  // Action
  notecard.begin(NOTE_I2C_ADDR_DEFAULT, NOTE_I2C_MAX_DEFAULT, mockWire);

  // Evaluate Result
  if (twoWireBegin_Parameters.invoked)
  {
    result = 0;
  }
  else
  {
    result = 38;
  }

  return result;
}

int test_notecard_begin_serial_default_parameter_for_baud_rate_is_passed_to_note_c()
{
  int result;
  Notecard notecard;
  const unsigned int baud = 9600;

  // Setup
  hardwareSerialBegin_Parameters.baud = (baud - 1);

  // Action
  notecard.begin(Serial);

  // Evaluate Result
  if (baud == hardwareSerialBegin_Parameters.baud)
  {
    result = 0;
  }
  else
  {
    result = 39;
  }

  return result;
}

int test_notecard_newCommand_does_not_modify_string_parameter_value_before_passing_to_note_c()
{
  int result;
  Notecard notecard;
  const char *str = "Hello, Test 23!";
  char str_copy[32];

  // Setup
  strcpy(str_copy, str);
  noteNewCommand_Parameters.request = nullptr;
  noteNewCommand_Parameters.result = nullptr;

  // Action
  notecard.newCommand(str_copy);

  // Evaluate Result
  if (0 == strcmp(str, noteNewCommand_Parameters.request))
  {
    result = 0;
  }
  else
  {
    result = 40;
  }

  return result;
}

int test_notecard_newCommand_does_not_modify_note_c_result_value_before_returning_to_caller()
{
  int result;
  Notecard notecard;
  J *json_obj = reinterpret_cast<J *>(malloc(sizeof(J))); // JCreateObject();
  assert(nullptr != json_obj);

  // Setup
  memset(json_obj, 0x55, sizeof(J)); //assert(nullptr != JAddStringToObject(json_obj,"key1", "value1"));
  {
    noteNewCommand_Parameters.result = reinterpret_cast<J *>(malloc(sizeof(J)));
    assert(nullptr != noteNewCommand_Parameters.result);
    memcpy(noteNewCommand_Parameters.result, json_obj, sizeof(J));
  } //assert(nullptr != (noteNewCommand_Parameters.result = JDuplicate(json_obj,true)));

  // Action
  J *json_result = notecard.newCommand(nullptr);

  // Evaluate Result
  if (0 == memcmp(json_obj, json_result, sizeof(J)) /*JCompare(json_obj, json_result, false)*/)
  {
    result = 0;
  }
  else
  {
    result = 41;
  }

  free(noteNewCommand_Parameters.result);
  free(json_obj);
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
