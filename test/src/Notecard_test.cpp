#include <catch2/catch_test_macros.hpp>

#include "arduino_fakes.hpp"
#include "note_iface_fakes.hpp"

// Non-null address used to pre-load function-pointer arguments.
static int fake_sentinel;
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>


#include "Notecard.h"


// Called first in every TEST_CASE: RESET_FAKE on every FFF fake, then
// FFF_RESET_HISTORY() (see resetArduinoFakes() and resetNoteInterfaceFakes()).
static void resetFakes(void)
{
  resetArduinoFakes();
  resetNoteInterfaceFakes();
}

TEST_CASE("test_notecard_begin_i2c_sets_user_agent_to_note_arduino", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  static const char * const EXPECTED_USER_AGENT = "note-arduino " NOTE_ARDUINO_VERSION;
  Notecard notecard;
  NoteI2c_Fake fakeI2c;

  RESET_CAPTURE_FAKE(NoteSetUserAgent);

   // Action
  ///////////

  notecard.begin(&fakeI2c);

   // Assert
  ///////////

  CHECK(NoteSetUserAgent_fake.arg0_val);
  CHECK(!strcmp(EXPECTED_USER_AGENT, NoteSetUserAgent_fake.arg0_val));
}

TEST_CASE("test_notecard_begin_i2c_sets_user_agent_to_note_arduino_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  static const char * const EXPECTED_USER_AGENT = "note-arduino " NOTE_ARDUINO_VERSION;
  Notecard notecard;
  RESET_CAPTURE_FAKE(NoteSetUserAgent);

   // Action
  ///////////

  notecard.begin(static_cast<NoteI2c *>(nullptr));

   // Assert
  ///////////

  CHECK(NoteSetUserAgent_fake.arg0_val);
  CHECK(!strcmp(EXPECTED_USER_AGENT, NoteSetUserAgent_fake.arg0_val));
}

TEST_CASE("test_notecard_begin_i2c_shares_a_memory_allocation_function_pointer", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteI2c_Fake fakeI2c;
  RESET_FAKE(NoteSetFnDefault);

   // Action
  ///////////

  notecard.begin(&fakeI2c);

   // Assert
  ///////////

  CHECK(NoteSetFnDefault_fake.arg0_val);
}

TEST_CASE("test_notecard_begin_i2c_sets_memory_allocation_function_pointer_to_nullptr_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  RESET_FAKE(NoteSetFnDefault);

   // Action
  ///////////

  notecard.begin(static_cast<NoteI2c *>(nullptr));

   // Assert
  ///////////

  CHECK(!NoteSetFnDefault_fake.arg0_val);
}

TEST_CASE("test_notecard_begin_i2c_shares_a_memory_free_function_pointer", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteI2c_Fake fakeI2c;
  RESET_FAKE(NoteSetFnDefault);

   // Action
  ///////////

  notecard.begin(&fakeI2c);

   // Assert
  ///////////

  CHECK(NoteSetFnDefault_fake.arg1_val);
}

TEST_CASE("test_notecard_begin_i2c_sets_memory_free_function_pointer_to_nullptr_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  RESET_FAKE(NoteSetFnDefault);

   // Action
  ///////////

  notecard.begin(static_cast<NoteI2c *>(nullptr));

   // Assert
  ///////////

  CHECK(!NoteSetFnDefault_fake.arg1_val);
}

TEST_CASE("test_notecard_begin_i2c_shares_a_delay_function_pointer", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteI2c_Fake fakeI2c;
  RESET_FAKE(NoteSetFnDefault);

   // Action
  ///////////

  notecard.begin(&fakeI2c);

   // Arrange
  ////////////

  CHECK(NoteSetFnDefault_fake.arg2_val);
}

TEST_CASE("test_notecard_begin_i2c_sets_delay_function_pointer_to_nullptr_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  RESET_FAKE(NoteSetFnDefault);

   // Action
  ///////////

  notecard.begin(static_cast<NoteI2c *>(nullptr));

   // Arrange
  ////////////

  CHECK(!NoteSetFnDefault_fake.arg2_val);
}

TEST_CASE("test_notecard_begin_i2c_shares_a_millis_function_pointer", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteI2c_Fake fakeI2c;
  RESET_FAKE(NoteSetFnDefault);

   // Action
  ///////////

  notecard.begin(&fakeI2c);

   // Assert
  ///////////

  CHECK(NoteSetFnDefault_fake.arg3_val);
}

TEST_CASE("test_notecard_begin_i2c_sets_millis_function_pointer_to_nullptr_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  RESET_FAKE(NoteSetFnDefault);

   // Action
  ///////////

  notecard.begin(static_cast<NoteI2c *>(nullptr));

   // Assert
  ///////////

  CHECK(!NoteSetFnDefault_fake.arg3_val);
}

TEST_CASE("test_notecard_begin_i2c_does_not_modify_i2c_address_parameter_before_passing_to_note_c", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  // 0x1B binary representation => 0001 1011
  const uint16_t EXPECTED_ADDRESS = 0x1B;

  Notecard notecard;
  NoteI2c_Fake fakeI2c;
  RESET_FAKE(NoteSetFnI2CDefault);

   // Action
  ///////////

  notecard.begin(&fakeI2c, EXPECTED_ADDRESS);

   // Assert
  ///////////

  CHECK(EXPECTED_ADDRESS == NoteSetFnI2CDefault_fake.arg0_val);
}

TEST_CASE("test_notecard_begin_i2c_sets_i2c_address_parameter_to_zero_before_passing_to_note_c_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  // 0x1B binary representation => 0001 1011
  const uint16_t SUPPLIED_ADDRESS = 0x1B;

  Notecard notecard;
  RESET_FAKE(NoteSetFnI2C);
  NoteSetFnI2C_fake.arg0_val = NOTE_I2C_ADDR_DEFAULT;

   // Action
  ///////////

  notecard.begin(static_cast<NoteI2c *>(nullptr), SUPPLIED_ADDRESS);

   // Assert
  ///////////

  CHECK(!NoteSetFnI2C_fake.arg0_val);
}

TEST_CASE("test_notecard_begin_i2c_does_not_modify_i2c_mtu_parameter_before_passing_to_note_c", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const uint32_t EXPECTED_RESULT = 79;

  Notecard notecard;
  NoteI2c_Fake fakeI2c;
  RESET_FAKE(NoteSetFnI2CDefault);

   // Action
  ///////////

  notecard.begin(&fakeI2c, 0x1B, EXPECTED_RESULT);

   // Assert
  ///////////

  CHECK(EXPECTED_RESULT == NoteSetFnI2CDefault_fake.arg1_val);
}

TEST_CASE("test_notecard_begin_i2c_sets_i2c_mtu_parameter_to_zero_before_passing_to_note_c_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const uint32_t SUPPLIED_VALUE = 79;

  Notecard notecard;
  RESET_FAKE(NoteSetFnI2C);
  NoteSetFnI2C_fake.arg1_val = NOTE_I2C_MAX_DEFAULT;

   // Action
  ///////////

  notecard.begin(static_cast<NoteI2c *>(nullptr), 0x1B, SUPPLIED_VALUE);

   // Assert
  ///////////

  CHECK(!NoteSetFnI2C_fake.arg1_val);
}

TEST_CASE("test_notecard_begin_i2c_shares_an_i2c_reset_function_pointer", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteI2c_Fake fakeI2c;
  RESET_FAKE(NoteSetFnI2CDefault);

   // Action
  ///////////

  notecard.begin(&fakeI2c);

   // Assert
  ///////////

  CHECK(NoteSetFnI2CDefault_fake.arg2_val);
}

TEST_CASE("test_notecard_begin_i2c_sets_i2c_reset_function_pointer_to_nullptr_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  RESET_FAKE(NoteSetFnI2C);
  NoteSetFnI2C_fake.arg2_val = reinterpret_cast<i2cResetFn>(0x19790917);

   // Action
  ///////////

  notecard.begin(static_cast<NoteI2c *>(nullptr));

   // Assert
  ///////////

  CHECK(!NoteSetFnI2C_fake.arg2_val);
}

TEST_CASE("test_notecard_begin_i2c_shares_an_i2c_transmit_function_pointer", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteI2c_Fake fakeI2c;
  RESET_FAKE(NoteSetFnI2CDefault);

   // Action
  ///////////

  notecard.begin(&fakeI2c);

   // Assert
  ///////////

  CHECK(NoteSetFnI2CDefault_fake.arg3_val);
}

TEST_CASE("test_notecard_begin_i2c_sets_i2c_transmit_function_pointer_to_nullptr_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  RESET_FAKE(NoteSetFnI2C);
  NoteSetFnI2C_fake.arg3_val = reinterpret_cast<i2cTransmitFn>(0x19790917);

   // Action
  ///////////

  notecard.begin(static_cast<NoteI2c *>(nullptr));

   // Assert
  ///////////

  CHECK(!NoteSetFnI2C_fake.arg3_val);
}

TEST_CASE("test_notecard_begin_i2c_shares_an_i2c_receive_function_pointer", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteI2c_Fake fakeI2c;
  RESET_FAKE(NoteSetFnI2CDefault);

   // Action
  ///////////

  notecard.begin(&fakeI2c);

   // Assert
  ///////////

  CHECK(NoteSetFnI2CDefault_fake.arg4_val);
}

TEST_CASE("test_notecard_begin_i2c_sets_i2c_receive_function_pointer_to_nullptr_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  RESET_FAKE(NoteSetFnI2C);
  NoteSetFnI2C_fake.arg4_val = reinterpret_cast<i2cReceiveFn>(0x19790917);

   // Action
  ///////////

  notecard.begin(static_cast<NoteI2c *>(nullptr));

   // Assert
  ///////////

  CHECK(!NoteSetFnI2C_fake.arg4_val);
}

TEST_CASE("test_notecard_begin_i2c_default_parameter_for_i2cMax_is_passed_to_note_c", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteI2c_Fake fakeI2c;
  RESET_FAKE(NoteSetFnI2CDefault);
  NoteSetFnI2CDefault_fake.arg1_val = (NOTE_I2C_MAX_DEFAULT - 1);

   // Action
  ///////////

  notecard.begin(&fakeI2c, 0x1B);

   // Assert
  ///////////

  CHECK(NOTE_I2C_MAX_DEFAULT == NoteSetFnI2CDefault_fake.arg1_val);
}

TEST_CASE("test_notecard_begin_i2c_default_parameter_for_i2cAddress_is_passed_to_note_c", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteI2c_Fake fakeI2c;
  RESET_FAKE(NoteSetFnI2CDefault);
  NoteSetFnI2CDefault_fake.arg0_val = (NOTE_I2C_ADDR_DEFAULT - 1);

   // Action
  ///////////

  notecard.begin(&fakeI2c);

   // Assert
  ///////////

  CHECK(NOTE_I2C_ADDR_DEFAULT == NoteSetFnI2CDefault_fake.arg0_val);
}

TEST_CASE("test_notecard_begin_serial_sets_user_agent_to_note_arduino", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  static const char * const EXPECTED_USER_AGENT = "note-arduino " NOTE_ARDUINO_VERSION;
  Notecard notecard;
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  RESET_CAPTURE_FAKE(NoteSetUserAgent);

   // Action
  ///////////

  notecard.begin(&fakeSerial);

   // Assert
  ///////////

  CHECK(NoteSetUserAgent_fake.arg0_val);
  CHECK(!strcmp(EXPECTED_USER_AGENT, NoteSetUserAgent_fake.arg0_val));
}

TEST_CASE("test_notecard_begin_serial_sets_user_agent_to_note_arduino_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  static const char * const EXPECTED_USER_AGENT = "note-arduino " NOTE_ARDUINO_VERSION;
  Notecard notecard;
  RESET_CAPTURE_FAKE(NoteSetUserAgent);

   // Action
  ///////////

  notecard.begin(static_cast<NoteSerial *>(nullptr));

   // Assert
  ///////////

  CHECK(NoteSetUserAgent_fake.arg0_val);
  CHECK(!strcmp(EXPECTED_USER_AGENT, NoteSetUserAgent_fake.arg0_val));
}

TEST_CASE("test_notecard_begin_serial_shares_a_memory_allocation_function_pointer", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  RESET_FAKE(NoteSetFnDefault);

   // Action
  ///////////

  notecard.begin(&fakeSerial);

   // Assert
  ///////////

  CHECK(NoteSetFnDefault_fake.arg0_val);
}

TEST_CASE("test_notecard_begin_serial_sets_memory_allocation_function_pointer_to_nullptr_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  RESET_FAKE(NoteSetFnDefault);

   // Action
  ///////////

  notecard.begin(static_cast<NoteSerial *>(nullptr));

   // Assert
  ///////////

  CHECK(!NoteSetFnDefault_fake.arg0_val);
}

TEST_CASE("test_notecard_begin_serial_shares_a_memory_free_function_pointer", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  RESET_FAKE(NoteSetFnDefault);

   // Action
  ///////////

  notecard.begin(&fakeSerial);

   // Assert
  ///////////

  CHECK(NoteSetFnDefault_fake.arg1_val);
}

TEST_CASE("test_notecard_begin_serial_sets_memory_free_function_pointer_to_nullptr_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  RESET_FAKE(NoteSetFnDefault);

   // Action
  ///////////

  notecard.begin(static_cast<NoteSerial *>(nullptr));

   // Assert
  ///////////

  CHECK(!NoteSetFnDefault_fake.arg1_val);
}

TEST_CASE("test_notecard_begin_serial_shares_a_delay_function_pointer", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  RESET_FAKE(NoteSetFnDefault);

   // Action
  ///////////

  notecard.begin(&fakeSerial);

   // Assert
  ///////////

  CHECK(NoteSetFnDefault_fake.arg2_val);
}

TEST_CASE("test_notecard_begin_serial_sets_delay_function_pointer_to_nullptr_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  RESET_FAKE(NoteSetFnDefault);

   // Action
  ///////////

  notecard.begin(static_cast<NoteSerial *>(nullptr));

   // Assert
  ///////////

  CHECK(!NoteSetFnDefault_fake.arg2_val);
}

TEST_CASE("test_notecard_begin_serial_shares_a_millis_function_pointer", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  RESET_FAKE(NoteSetFnDefault);

   // Action
  ///////////

  notecard.begin(&fakeSerial);

   // Assert
  ///////////

  CHECK(NoteSetFnDefault_fake.arg3_val);
}

TEST_CASE("test_notecard_begin_serial_sets_millis_function_pointer_to_nullptr_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  RESET_FAKE(NoteSetFnDefault);

   // Action
  ///////////

  notecard.begin(static_cast<NoteSerial *>(nullptr));

   // Assert
  ///////////

  CHECK(!NoteSetFnDefault_fake.arg3_val);
}

TEST_CASE("test_notecard_begin_serial_shares_a_serial_reset_function_pointer", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  RESET_FAKE(NoteSetFnSerialDefault);

   // Action
  ///////////

  notecard.begin(&fakeSerial);

   // Assert
  ///////////

  CHECK(NoteSetFnSerialDefault_fake.arg0_val);
}

TEST_CASE("test_notecard_begin_serial_sets_serial_reset_function_pointer_to_nullptr_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  RESET_FAKE(NoteSetFnSerial);
  NoteSetFnSerial_fake.arg0_val = reinterpret_cast<serialResetFn>(0x19790917);

   // Action
  ///////////

  notecard.begin(static_cast<NoteSerial *>(nullptr));

   // Assert
  ///////////

  CHECK(!NoteSetFnSerial_fake.arg0_val);
}

TEST_CASE("test_notecard_begin_serial_shares_a_serial_transmit_function_pointer", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  RESET_FAKE(NoteSetFnSerialDefault);

   // Action
  ///////////

  notecard.begin(&fakeSerial);

   // Assert
  ///////////

  CHECK(NoteSetFnSerialDefault_fake.arg1_val);
}

TEST_CASE("test_notecard_begin_serial_sets_transmit_function_pointer_to_nullptr_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  RESET_FAKE(NoteSetFnSerial);
  NoteSetFnSerial_fake.arg1_val = reinterpret_cast<serialTransmitFn>(0x19790917);

   // Action
  ///////////

  notecard.begin(static_cast<NoteSerial *>(nullptr));

   // Assert
  ///////////

  CHECK(!NoteSetFnSerial_fake.arg1_val);
}

TEST_CASE("test_notecard_begin_serial_shares_a_serial_available_function_pointer", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  RESET_FAKE(NoteSetFnSerialDefault);

   // Action
  ///////////

  notecard.begin(&fakeSerial);

   // Assert
  ///////////

  CHECK(NoteSetFnSerialDefault_fake.arg2_val);
}

TEST_CASE("test_notecard_begin_serial_sets_serial_available_function_pointer_to_nullptr_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  RESET_FAKE(NoteSetFnSerial);
  NoteSetFnSerial_fake.arg2_val = reinterpret_cast<serialAvailableFn>(0x19790917);

   // Action
  ///////////

  notecard.begin(static_cast<NoteSerial *>(nullptr));

   // Assert
  ///////////

  CHECK(!NoteSetFnSerial_fake.arg2_val);
}

TEST_CASE("test_notecard_begin_serial_shares_a_serial_receive_function_pointer", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  RESET_FAKE(NoteSetFnSerialDefault);

   // Action
  ///////////

  notecard.begin(&fakeSerial);

   // Assert
  ///////////

  CHECK(NoteSetFnSerialDefault_fake.arg3_val);
}

TEST_CASE("test_notecard_begin_serial_sets_serial_receive_function_pointer_to_nullptr_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  RESET_FAKE(NoteSetFnSerial);
  NoteSetFnSerial_fake.arg3_val = reinterpret_cast<serialReceiveFn>(0x19790917);

   // Action
  ///////////

  notecard.begin(static_cast<NoteSerial *>(nullptr));

   // Assert
  ///////////

  CHECK(!NoteSetFnSerial_fake.arg3_val);
}

TEST_CASE("test_notecard_end_does_not_call_wire_end_when_the_i2c_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Fake fakeSerial;
  RESET_FAKE(NoteSetFnSerial);
  RESET_FAKE(TwoWire_end);
  notecard.begin(&fakeSerial);
  RESET_FAKE(NoteGetFnI2C);
  RESET_FAKE(NoteGetFnSerial);
  RESET_FAKE(make_note_i2c_with);

   // Action
  ///////////

  notecard.end();

   // Assert
  ///////////

  CHECK(!make_note_i2c_with_fake.call_count);
}

TEST_CASE("test_notecard_end_clears_all_i2c_interface_function_pointers_when_the_i2c_interface_has_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteI2c_Fake fakeI2c;
  RESET_FAKE(NoteSetFnI2C);
  notecard.begin(&fakeI2c);
  RESET_FAKE(NoteGetFnSerial);
  RESET_FAKE(NoteGetFnI2C);
  NoteGetFnI2C_fake.custom_fake = [](uint32_t *, uint32_t *, i2cResetFn *, i2cTransmitFn * transmitFn, i2cReceiveFn *) {
    if (transmitFn) { *transmitFn = reinterpret_cast<i2cTransmitFn>(0x1); }
  };

   // Action
  ///////////

  notecard.end();

   // Assert
  ///////////

  CHECK(!NoteSetFnI2C_fake.arg2_val);
  CHECK(!NoteSetFnI2C_fake.arg3_val);
  CHECK(!NoteSetFnI2C_fake.arg4_val);
}

TEST_CASE("test_notecard_end_does_not_call_serial_end_when_the_serial_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Fake fakeI2c;
  RESET_FAKE(NoteSetFnDefault);
  notecard.begin(&fakeI2c);
  RESET_FAKE(HardwareSerial_end);
  RESET_FAKE(NoteGetFnI2C);
  RESET_FAKE(NoteGetFnSerial);
  RESET_FAKE(make_note_serial_with);

   // Action
  ///////////

  notecard.end();

   // Assert
  ///////////

  CHECK(!make_note_serial_with_fake.call_count);
}

TEST_CASE("test_notecard_end_clears_all_serial_interface_function_pointers_when_the_serial_interface_has_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  RESET_FAKE(NoteSetFnDefault);
  notecard.begin(&fakeSerial);
  RESET_FAKE(NoteGetFnI2C);
  RESET_FAKE(NoteGetFnSerial);
  NoteGetFnSerial_fake.custom_fake = [](serialResetFn *, serialTransmitFn * transmitFn, serialAvailableFn *, serialReceiveFn *) {
    if (transmitFn) { *transmitFn = reinterpret_cast<serialTransmitFn>(0x1); }
  };

   // Action
  ///////////

  notecard.end();

   // Assert
  ///////////

  CHECK(!NoteSetFnSerial_fake.arg0_val);
  CHECK(!NoteSetFnSerial_fake.arg1_val);
  CHECK(!NoteSetFnSerial_fake.arg2_val);
  CHECK(!NoteSetFnSerial_fake.arg3_val);
}

TEST_CASE("test_notecard_end_clears_all_platform_interface_function_pointers", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteI2c_Fake fakeI2c;
  RESET_FAKE(NoteSetFn);
  notecard.begin(&fakeI2c);

   // Action
  ///////////

  notecard.end();

   // Assert
  ///////////

  CHECK(!NoteSetFn_fake.arg0_val);
  CHECK(!NoteSetFn_fake.arg1_val);
  CHECK(!NoteSetFn_fake.arg2_val);
  CHECK(!NoteSetFn_fake.arg3_val);
}

TEST_CASE("test_notecard_end_provides_nullptr_to_make_note_i2c_to_free_associated_memory_when_the_i2c_interface_has_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteI2c_Fake fakeI2c;  // Instantiate NoteI2c (faked)
  RESET_FAKE(make_note_i2c_with);
  make_note_i2c_with_fake.arg0_val = &Wire;
  notecard.begin(&fakeI2c);
  RESET_FAKE(NoteGetFnSerial);
  RESET_FAKE(NoteGetFnI2C);
  NoteGetFnI2C_fake.custom_fake = [](uint32_t *, uint32_t *, i2cResetFn *, i2cTransmitFn * transmitFn, i2cReceiveFn *) {
    if (transmitFn) { *transmitFn = reinterpret_cast<i2cTransmitFn>(0x1); }
  };

   // Action
  ////////////

  notecard.end();

   // Assert
  ////////////

  CHECK(nullptr == make_note_i2c_with_fake.arg0_val);
}

TEST_CASE("test_notecard_end_invokes_make_note_serial_nullptr_method_to_free_associated_memory_when_the_serial_interface_has_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  RESET_FAKE(make_note_serial_with);
  make_note_serial_with_fake.arg0_val = &Serial;
  notecard.begin(&fakeSerial);
  RESET_FAKE(NoteGetFnI2C);
  RESET_FAKE(NoteGetFnSerial);
  NoteGetFnSerial_fake.custom_fake = [](serialResetFn *, serialTransmitFn * transmitFn, serialAvailableFn *, serialReceiveFn *) {
    if (transmitFn) { *transmitFn = reinterpret_cast<serialTransmitFn>(0x1); }
  };

   // Action
  ////////////

  notecard.end();

   // Assert
  ////////////

  CHECK(nullptr == make_note_serial_with_fake.arg0_val);
}

TEST_CASE("test_notecard_setDebugOutputStream_shares_a_debug_log_function_pointer", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteLog_Fake fakeLog;
  RESET_FAKE(NoteSetFnDebugOutput);

   // Action
  ///////////

  notecard.setDebugOutputStream(&fakeLog);

   // Assert
  ///////////

  CHECK(NoteSetFnDebugOutput_fake.arg0_val);
}

TEST_CASE("test_notecard_setDebugOutputStream_clears_the_debug_log_function_pointer_when_nullptr_is_provided", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  RESET_FAKE(NoteSetFnDebugOutput);
  NoteSetFnDebugOutput_fake.arg0_val = reinterpret_cast<debugOutputFn>(&fake_sentinel);

   // Action
  ///////////

  notecard.setDebugOutputStream(nullptr);

   // Assert
  ///////////

  CHECK(!NoteSetFnDebugOutput_fake.arg0_val);
}

TEST_CASE("test_notecard_clearDebugOutputStream_clears_the_debug_log_function_pointer", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;

  RESET_FAKE(NoteSetFnDebugOutput);
  NoteSetFnDebugOutput_fake.arg0_val = reinterpret_cast<debugOutputFn>(&fake_sentinel);

   // Action
  ///////////

  notecard.clearDebugOutputStream();

   // Assert
  ///////////

  CHECK(!NoteSetFnDebugOutput_fake.arg0_val);
}

TEST_CASE("test_notecard_setFn_shares_a_memory_allocation_function_pointer", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const mallocFn mockMallocFn = reinterpret_cast<mallocFn>(0x19790917);

  Notecard notecard;
  RESET_FAKE(NoteSetFn);

   // Action
  ///////////

  notecard.setFn(mockMallocFn, nullptr, nullptr, nullptr);

   // Assert
  ///////////

  CHECK(NoteSetFn_fake.arg0_val);
}

TEST_CASE("test_notecard_setFn_shares_a_memory_deallocation_function_pointer", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const freeFn mockFreeFn = reinterpret_cast<freeFn>(0x19790917);

  Notecard notecard;
  RESET_FAKE(NoteSetFn);

   // Action
  ///////////

  notecard.setFn(nullptr, mockFreeFn, nullptr, nullptr);

   // Assert
  ///////////

  CHECK(NoteSetFn_fake.arg1_val);
}

TEST_CASE("test_notecard_setFn_shares_a_delay_function_pointer", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const delayMsFn mockDelayFn = reinterpret_cast<delayMsFn>(0x19790917);

  Notecard notecard;
  RESET_FAKE(NoteSetFn);

   // Action
  ///////////

  notecard.setFn(nullptr, nullptr, mockDelayFn, nullptr);

   // Assert
  ///////////

  CHECK(NoteSetFn_fake.arg2_val);
}

TEST_CASE("test_notecard_setFn_shares_a_millis_function_pointer", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const getMsFn mockMillisFn = reinterpret_cast<getMsFn>(0x19790917);

  Notecard notecard;
  RESET_FAKE(NoteSetFn);

   // Action
  ///////////

  notecard.setFn(nullptr, nullptr, nullptr, mockMillisFn);

   // Assert
  ///////////

  CHECK(NoteSetFn_fake.arg3_val);
}

TEST_CASE("test_notecard_setFnI2cMutex_does_not_modify_locking_mutex_func_parameter_value_before_passing_to_note_c", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////
  const mutexFn lockFn = reinterpret_cast<mutexFn>(0x19790917);
  Notecard notecard;
  RESET_FAKE(NoteSetFnI2CMutex);

   // Action
  ///////////

  notecard.setFnI2cMutex(lockFn,nullptr);

   // Assert
  ///////////

  CHECK(lockFn == NoteSetFnI2CMutex_fake.arg0_val);
}

TEST_CASE("test_notecard_setFnI2cMutex_does_not_modify_unlocking_mutex_func_parameter_value_before_passing_to_note_c", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////
  const mutexFn unlockFn = reinterpret_cast<mutexFn>(0x19790917);
  Notecard notecard;
  RESET_FAKE(NoteSetFnI2CMutex);

   // Action
  ///////////

  notecard.setFnI2cMutex(nullptr,unlockFn);

   // Assert
  ///////////

  CHECK(unlockFn == NoteSetFnI2CMutex_fake.arg1_val);
}

TEST_CASE("test_notecard_setFnNoteMutex_does_not_modify_locking_mutex_func_parameter_value_before_passing_to_note_c", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////
  const mutexFn lockFn = reinterpret_cast<mutexFn>(0x19790917);
  Notecard notecard;
  RESET_FAKE(NoteSetFnNoteMutex);

   // Action
  ///////////

  notecard.setFnNoteMutex(lockFn,nullptr);

   // Assert
  ///////////

  CHECK(lockFn == NoteSetFnNoteMutex_fake.arg0_val);
}

TEST_CASE("test_notecard_setFnNoteMutex_does_not_modify_unlocking_mutex_func_parameter_value_before_passing_to_note_c", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////
  const mutexFn unlockFn = reinterpret_cast<mutexFn>(0x19790917);
  Notecard notecard;
  RESET_FAKE(NoteSetFnNoteMutex);

   // Action
  ///////////

  notecard.setFnNoteMutex(nullptr,unlockFn);

   // Assert
  ///////////

  CHECK(unlockFn == NoteSetFnNoteMutex_fake.arg1_val);
}

TEST_CASE("test_notecard_setTransactionPins_shares_a_transaction_start_function_pointer", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteTxn_Fake fakeTxn;
  RESET_FAKE(NoteSetFnTransaction);

   // Action
  ///////////

  notecard.setTransactionPins(&fakeTxn);

   // Assert
  ///////////

  CHECK(NoteSetFnTransaction_fake.arg0_val);
}

TEST_CASE("test_notecard_setTransactionPins_shares_a_transaction_stop_function_pointer", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteTxn_Fake fakeTxn;
  RESET_FAKE(NoteSetFnTransaction);

   // Action
  ///////////

  notecard.setTransactionPins(&fakeTxn);

   // Assert
  ///////////

  CHECK(NoteSetFnTransaction_fake.arg1_val);
}

TEST_CASE("test_notecard_setTransactionPins_clears_the_transaction_start_function_pointer_when_nullptr_is_provided", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteTxn_Fake fakeTxn;
  RESET_FAKE(NoteSetFnTransaction);
  NoteSetFnTransaction_fake.arg0_val = reinterpret_cast<txnStartFn>(&fake_sentinel);

   // Action
  ///////////

  notecard.setTransactionPins(nullptr);

   // Assert
  ///////////

  CHECK(!NoteSetFnTransaction_fake.arg0_val);
}

TEST_CASE("test_notecard_setTransactionPins_clears_the_transaction_stop_function_pointer_when_nullptr_is_provided", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteTxn_Fake fakeTxn;
  RESET_FAKE(NoteSetFnTransaction);
  NoteSetFnTransaction_fake.arg1_val = reinterpret_cast<txnStopFn>(&fake_sentinel);

   // Action
  ///////////

  notecard.setTransactionPins(nullptr);

   // Assert
  ///////////

  CHECK(!NoteSetFnTransaction_fake.arg1_val);
}

TEST_CASE("test_notecard_newRequest_does_not_modify_string_parameter_value_before_passing_to_note_c", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  const char * const EXPECTED_RESULT = "Hello, Test!";

  RESET_CAPTURE_FAKE(NoteNewRequest);

   // Action
  ///////////

  notecard.newRequest(EXPECTED_RESULT);

   // Assert
  ///////////

  CHECK(!strcmp(EXPECTED_RESULT, NoteNewRequest_arg0_str.c_str()));
}

TEST_CASE("test_notecard_newRequest_does_not_modify_note_c_result_value_before_returning_to_caller", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  J * const EXPECTED_JSON = reinterpret_cast<J *>(malloc(sizeof(J))); // JCreateObject();
  assert(nullptr != EXPECTED_JSON);

  RESET_CAPTURE_FAKE(NoteNewRequest);
  memset(EXPECTED_JSON, 0x55, sizeof(J)); //assert(nullptr != JAddStringToObject(EXPECTED_JSON,"key1", "value1"));
  {
    NoteNewRequest_fake.return_val = reinterpret_cast<J *>(malloc(sizeof(J)));
    assert(nullptr != NoteNewRequest_fake.return_val);
    memcpy(NoteNewRequest_fake.return_val, EXPECTED_JSON, sizeof(J));
  } //assert(nullptr != (NoteNewRequest_fake.return_val = JDuplicate(EXPECTED_JSON,true)));

   // Action
  ///////////

  const J * const ACTUAL_RESULT = notecard.newRequest(nullptr);

   // Assert
  ///////////

  CHECK(!memcmp(ACTUAL_RESULT, EXPECTED_JSON, sizeof(J)) /*JCompare(EXPECTED_JSON, json_result, false)*/);

  free(NoteNewRequest_fake.return_val);
  free(EXPECTED_JSON);
}

TEST_CASE("test_notecard_sendRequest_does_not_modify_j_object_parameter_value_before_passing_to_note_c", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  J * EXPECTED_JSON = reinterpret_cast<J *>(malloc(sizeof(J))); // JCreateObject();
  J * json_cpy;
  assert(nullptr != EXPECTED_JSON);

  RESET_FAKE(NoteRequest);
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

  CHECK(!memcmp(EXPECTED_JSON, NoteRequest_fake.arg0_val, sizeof(J)));

  free(json_cpy);
  free(EXPECTED_JSON);
}

TEST_CASE("test_notecard_sendRequest_does_not_modify_note_c_result_value_before_returning_to_caller", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  const bool EXPECTED_RESULT = true;

  RESET_FAKE(NoteRequest);
  NoteRequest_fake.return_val = EXPECTED_RESULT;

   // Action
  ///////////

  const bool ACTUAL_RESULT = notecard.sendRequest(nullptr);

   // Assert
  ///////////

  CHECK(EXPECTED_RESULT == ACTUAL_RESULT);
}

TEST_CASE("test_notecard_sendRequestWithRetry_does_not_modify_j_object_parameter_value_before_passing_to_note_c", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  J * EXPECTED_JSON = reinterpret_cast<J *>(malloc(sizeof(J)));
  J * json_cpy;
  assert(nullptr != EXPECTED_JSON);

  RESET_FAKE(NoteRequestWithRetry);
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

  CHECK(!memcmp(EXPECTED_JSON, NoteRequestWithRetry_fake.arg0_val, sizeof(J)));

  free(json_cpy);
  free(EXPECTED_JSON);
}

TEST_CASE("test_notecard_sendRequestWithRetry_does_not_modify_timeout_parameter_value_before_passing_to_note_c", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  uint32_t EXPECTED_TIMEOUT = 10;
  RESET_FAKE(NoteRequestWithRetry);

   // Action
  ///////////

  notecard.sendRequestWithRetry(nullptr, EXPECTED_TIMEOUT);

   // Assert
  ///////////

  CHECK(EXPECTED_TIMEOUT == NoteRequestWithRetry_fake.arg1_val);
}

TEST_CASE("test_notecard_sendRequestWithRetry_does_not_modify_note_c_result_value_before_returning_to_caller", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  const bool EXPECTED_RESULT = true;

  RESET_FAKE(NoteRequestWithRetry);
  NoteRequestWithRetry_fake.return_val = EXPECTED_RESULT;

   // Action
  ///////////

  const bool ACTUAL_RESULT = notecard.sendRequestWithRetry(nullptr, 0);

   // Assert
  ///////////

  CHECK(EXPECTED_RESULT == ACTUAL_RESULT);
}

TEST_CASE("test_notecard_requestAndResponse_does_not_modify_j_object_parameter_value_before_passing_to_note_c", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  J *EXPECTED_JSON = reinterpret_cast<J *>(malloc(sizeof(J))); // JCreateObject();
  J *json_cpy;
  assert(nullptr != EXPECTED_JSON);

  RESET_FAKE(NoteRequestResponse);
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

  CHECK(!memcmp(EXPECTED_JSON, NoteRequestResponse_fake.arg0_val, sizeof(J)) /*JCompare(EXPECTED_JSON, NoteRequestResponse_fake.arg0_val, false)*/);

  free(json_cpy);
  free(EXPECTED_JSON);
}

TEST_CASE("test_notecard_requestAndResponse_does_not_modify_note_c_result_value_before_returning_to_caller", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  J * EXPECTED_JSON = reinterpret_cast<J *>(malloc(sizeof(J))); // JCreateObject();
  assert(nullptr != EXPECTED_JSON);

  RESET_FAKE(NoteRequestResponse);
  memset(EXPECTED_JSON, 0x55, sizeof(J)); //assert(nullptr != JAddStringToObject(EXPECTED_JSON,"key1", "value1"));
  {
    NoteRequestResponse_fake.return_val = reinterpret_cast<J *>(malloc(sizeof(J)));
    assert(nullptr != NoteRequestResponse_fake.return_val);
    memcpy(NoteRequestResponse_fake.return_val, EXPECTED_JSON, sizeof(J));
  } //assert(nullptr != (NoteRequestResponse_fake.return_val = JDuplicate(EXPECTED_JSON,true)));

   // Action
  ///////////

  const J * const ACTUAL_RESULT = notecard.requestAndResponse(nullptr);

   // Assert
  ///////////

  CHECK(!memcmp(EXPECTED_JSON, ACTUAL_RESULT, sizeof(J)) /*JCompare(EXPECTED_JSON, json_result, false)*/);

  free(NoteRequestResponse_fake.return_val);
  free(EXPECTED_JSON);
}

TEST_CASE("test_notecard_requestAndResponseWithRetry_does_not_modify_j_object_parameter_value_before_passing_to_note_c", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  J *EXPECTED_JSON = reinterpret_cast<J *>(malloc(sizeof(J)));
  J *json_cpy;
  assert(nullptr != EXPECTED_JSON);

  RESET_FAKE(NoteRequestResponseWithRetry);
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

  CHECK(!memcmp(EXPECTED_JSON, NoteRequestResponseWithRetry_fake.arg0_val, sizeof(J)));

  free(json_cpy);
  free(EXPECTED_JSON);
}

TEST_CASE("test_notecard_requestAndResponseWithRetry_does_not_modify_timeout_parameter_value_before_passing_to_note_c", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  uint32_t EXPECTED_TIMEOUT = 10;
  RESET_FAKE(NoteRequestResponseWithRetry);

   // Action
  ///////////

  notecard.requestAndResponseWithRetry(nullptr, EXPECTED_TIMEOUT);

   // Assert
  ///////////

  CHECK(EXPECTED_TIMEOUT == NoteRequestResponseWithRetry_fake.arg1_val);
}

TEST_CASE("test_notecard_requestAndResponseWithRetry_does_not_modify_note_c_result_value_before_returning_to_caller", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  J * EXPECTED_JSON = reinterpret_cast<J *>(malloc(sizeof(J)));
  assert(nullptr != EXPECTED_JSON);

  RESET_FAKE(NoteRequestResponseWithRetry);
  memset(EXPECTED_JSON, 0x55, sizeof(J));
  {
    NoteRequestResponseWithRetry_fake.return_val = reinterpret_cast<J *>(malloc(sizeof(J)));
    assert(nullptr != NoteRequestResponseWithRetry_fake.return_val);
    memcpy(NoteRequestResponseWithRetry_fake.return_val, EXPECTED_JSON, sizeof(J));
  }

   // Action
  ///////////

  const J * const ACTUAL_RESULT = notecard.requestAndResponseWithRetry(nullptr, 0);

   // Assert
  ///////////

  CHECK(!memcmp(EXPECTED_JSON, ACTUAL_RESULT, sizeof(J)));

  free(NoteRequestResponseWithRetry_fake.return_val);
  free(EXPECTED_JSON);
}

TEST_CASE("test_notecard_deleteResponse_does_not_modify_j_object_parameter_pointer_before_passing_to_note_c", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  J *const EXPECTED_POINTER = reinterpret_cast<J *>(0x19790917);

  RESET_FAKE(MockNoteDeleteResponse);

   // Action
  ///////////

  notecard.deleteResponse(EXPECTED_POINTER);

   // Assert
  ///////////

  CHECK(EXPECTED_POINTER == MockNoteDeleteResponse_fake.arg0_val);
}

TEST_CASE("test_notecard_logDebug_does_not_modify_string_parameter_value_before_passing_to_note_c", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const char str[] = "Hello, Test!";
  Notecard notecard;

  RESET_CAPTURE_FAKE(NoteDebug);

   // Action
  ///////////

  notecard.logDebug(str);

   // Assert
  ///////////

  CHECK(!strcmp(str, NoteDebug_arg0_str.c_str()));
}

TEST_CASE("test_notecard_logDebugf_does_not_modify_string_parameter_value_before_passing_to_note_c", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const char str[] = "Hello, Test 123!";
  Notecard notecard;

  RESET_CAPTURE_FAKE(NoteDebug);

   // Action
  ///////////

  notecard.logDebugf("Hello, %s %d%c", "Test", 123, '!');

   // Assert
  ///////////

  CHECK(!strcmp(str, NoteDebug_arg0_str.c_str()));
}

TEST_CASE("test_notecard_debugSyncStatus_does_not_modify_pollFrequencyMs_parameter_before_passing_to_note_c", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  const int EXPECTED_RESULT = 79;

  RESET_FAKE(NoteDebugSyncStatus);

   // Action
  ///////////

  notecard.debugSyncStatus(EXPECTED_RESULT, 0);

   // Assert
  ///////////

  CHECK(EXPECTED_RESULT == NoteDebugSyncStatus_fake.arg0_val);
}

TEST_CASE("test_notecard_debugSyncStatus_does_not_modify_maxLevel_parameter_before_passing_to_note_c", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  const int EXPECTED_RESULT = 79;

  RESET_FAKE(NoteDebugSyncStatus);

   // Action
  ///////////

  notecard.debugSyncStatus(0, EXPECTED_RESULT);

   // Assert
  ///////////

  CHECK(EXPECTED_RESULT == NoteDebugSyncStatus_fake.arg1_val);
}

TEST_CASE("test_notecard_debugSyncStatus_does_not_modify_note_c_result_value_before_returning_to_caller", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  const bool EXPECTED_RESULT = true;

  RESET_FAKE(NoteDebugSyncStatus);
  NoteDebugSyncStatus_fake.return_val = EXPECTED_RESULT;

   // Action
  ///////////

  const bool ACTUAL_RESULT = notecard.debugSyncStatus(0, 0);

   // Assert
  ///////////

  CHECK(EXPECTED_RESULT == ACTUAL_RESULT);
}

TEST_CASE("test_notecard_responseError_does_not_modify_j_object_parameter_value_before_passing_to_note_c", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  J * EXPECTED_JSON = reinterpret_cast<J *>(malloc(sizeof(J))); // JCreateObject();
  J * json_cpy;
  assert(nullptr != EXPECTED_JSON);

  RESET_FAKE(MockNoteResponseError);
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

  CHECK(!memcmp(EXPECTED_JSON, MockNoteResponseError_fake.arg0_val, sizeof(J)));

  free(json_cpy);
  free(EXPECTED_JSON);
}

TEST_CASE("test_notecard_responseError_does_not_modify_note_c_result_value_before_returning_to_caller", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  const bool EXPECTED_RESULT = true;

  RESET_FAKE(MockNoteResponseError);
  MockNoteResponseError_fake.return_val = EXPECTED_RESULT;

   // Action
  ///////////

  const bool ACTUAL_RESULT = notecard.responseError(nullptr);

   // Assert
  ///////////

  CHECK(EXPECTED_RESULT == ACTUAL_RESULT);
}

TEST_CASE("test_notecard_newCommand_does_not_modify_string_parameter_value_before_passing_to_note_c", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  const char EXPECTED_RESULT[] = "Hello, Test!";

  RESET_CAPTURE_FAKE(NoteNewCommand);

   // Action
  ///////////

  notecard.newCommand(EXPECTED_RESULT);

   // Assert
  ///////////

  CHECK(!strcmp(EXPECTED_RESULT, NoteNewCommand_arg0_str.c_str()));
}

TEST_CASE("test_notecard_newCommand_does_not_modify_note_c_result_value_before_returning_to_caller", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  J * EXPECTED_JSON = reinterpret_cast<J *>(malloc(sizeof(J))); // JCreateObject();
  assert(nullptr != EXPECTED_JSON);

  RESET_CAPTURE_FAKE(NoteNewCommand);
  memset(EXPECTED_JSON, 0x55, sizeof(J)); //assert(nullptr != JAddStringToObject(EXPECTED_JSON,"key1", "value1"));
  {
    NoteNewCommand_fake.return_val = reinterpret_cast<J *>(malloc(sizeof(J)));
    assert(nullptr != NoteNewCommand_fake.return_val);
    memcpy(NoteNewCommand_fake.return_val, EXPECTED_JSON, sizeof(J));
  } //assert(nullptr != (NoteNewCommand_fake.return_val = JDuplicate(EXPECTED_JSON,true)));

   // Action
  ///////////

  const J * const ACTUAL_RESULT = notecard.newCommand(nullptr);

   // Assert
  ///////////

  CHECK(!memcmp(EXPECTED_JSON, ACTUAL_RESULT, sizeof(J)) /*JCompare(EXPECTED_JSON, ACTUAL_RESULT, false)*/);

  free(NoteNewCommand_fake.return_val);
  free(EXPECTED_JSON);
}

TEST_CASE("test_static_callback_note_i2c_receive_invokes_notei2c_receive", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x17;
  const uint16_t SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t actual_response_size;

  Notecard notecard;
  NoteI2c_Fake fakeI2c;  // Instantiate NoteI2c (faked)
  notecard.begin(&fakeI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cReceiveFn noteI2cReceive = NoteSetFnI2CDefault_fake.arg4_val;
  RESET_FAKE(NoteI2c_receive);  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cReceive(ADDRESS, response_buffer, SIZE, &actual_response_size);

   // Assert
  ///////////

  CHECK(NoteI2c_receive_fake.call_count);
}

TEST_CASE("test_static_callback_note_i2c_receive_does_not_modify_device_address_parameter_before_passing_to_interface_method", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  // 0x1B binary representation => 0001 1011
  const uint16_t EXPECTED_ADDRESS = 0x1B;
  const uint16_t SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t actual_response_size;

  Notecard notecard;
  NoteI2c_Fake fakeI2c;  // Instantiate NoteI2c (faked)
  notecard.begin(&fakeI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cReceiveFn noteI2cReceive = NoteSetFnI2CDefault_fake.arg4_val;
  RESET_FAKE(NoteI2c_receive);  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cReceive(EXPECTED_ADDRESS, response_buffer, SIZE, &actual_response_size);

   // Assert
  ///////////

  CHECK(EXPECTED_ADDRESS == NoteI2c_receive_fake.arg0_val);
}

TEST_CASE("test_static_callback_note_i2c_receive_does_not_modify_buffer_parameter_address_before_passing_to_interface_method", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;
  const uint16_t SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t actual_response_size;

  Notecard notecard;
  NoteI2c_Fake fakeI2c;  // Instantiate NoteI2c (faked)
  notecard.begin(&fakeI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cReceiveFn noteI2cReceive = NoteSetFnI2CDefault_fake.arg4_val;
  RESET_FAKE(NoteI2c_receive);  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cReceive(ADDRESS, response_buffer, SIZE, &actual_response_size);

   // Assert
  ///////////

  CHECK(response_buffer == NoteI2c_receive_fake.arg1_val);
}

TEST_CASE("test_static_callback_note_i2c_receive_does_not_modify_size_parameter_before_passing_to_interface_method", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;
  const uint16_t EXPECTED_SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t actual_response_size;

  Notecard notecard;
  NoteI2c_Fake fakeI2c;  // Instantiate NoteI2c (faked)
  notecard.begin(&fakeI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cReceiveFn noteI2cReceive = NoteSetFnI2CDefault_fake.arg4_val;
  RESET_FAKE(NoteI2c_receive);  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cReceive(ADDRESS, response_buffer, EXPECTED_SIZE, &actual_response_size);

   // Assert
  ///////////

  CHECK(EXPECTED_SIZE == NoteI2c_receive_fake.arg2_val);
}

TEST_CASE("test_static_callback_note_i2c_receive_does_not_modify_available_parameter_address_before_passing_to_interface_method", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;
  const uint16_t SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t actual_response_size;

  Notecard notecard;
  NoteI2c_Fake fakeI2c;  // Instantiate NoteI2c (faked)
  notecard.begin(&fakeI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cReceiveFn noteI2cReceive = NoteSetFnI2CDefault_fake.arg4_val;
  RESET_FAKE(NoteI2c_receive);  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cReceive(ADDRESS, response_buffer, SIZE, &actual_response_size);

   // Assert
  ///////////

  CHECK(&actual_response_size == NoteI2c_receive_fake.arg3_val);
}

TEST_CASE("test_static_callback_note_i2c_receive_does_not_modify_interface_method_return_value", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;
  const uint16_t SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t actual_response_size;

  Notecard notecard;
  NoteI2c_Fake fakeI2c;  // Instantiate NoteI2c (faked)
  notecard.begin(&fakeI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cReceiveFn noteI2cReceive = NoteSetFnI2CDefault_fake.arg4_val;
  RESET_FAKE(NoteI2c_receive);  // Clear the structure for testing results
  NoteI2c_receive_fake.return_val = "i2c: fake test error!";

   // Action
  ///////////

  const char * const ACTUAL_RESULT = noteI2cReceive(ADDRESS, response_buffer, SIZE, &actual_response_size);

   // Assert
  ///////////

  CHECK(!strcmp(ACTUAL_RESULT, NoteI2c_receive_fake.return_val));
}

TEST_CASE("test_static_callback_note_i2c_receive_does_not_call_interface_method_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;
  const uint16_t SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t actual_response_size;

  Notecard notecard;
  NoteI2c_Fake fakeI2c;
  notecard.begin(&fakeI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cReceiveFn noteI2cReceive = NoteSetFnI2CDefault_fake.arg4_val;

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteI2c *>(nullptr));
  RESET_FAKE(NoteI2c_receive);  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cReceive(ADDRESS, response_buffer, SIZE, &actual_response_size);

   // Assert
  ///////////

  CHECK(!NoteI2c_receive_fake.call_count);
}

TEST_CASE("test_static_callback_note_i2c_receive_returns_error_message_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;
  const uint16_t SIZE = 13;
  uint8_t response_buffer[32];
  uint32_t actual_response_size;
  const char * const EXPECTED_RESULT = "i2c: A call to Notecard::begin() is required. {io}";

  Notecard notecard;
  NoteI2c_Fake fakeI2c;
  notecard.begin(&fakeI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cReceiveFn noteI2cReceive = NoteSetFnI2CDefault_fake.arg4_val;

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteI2c *>(nullptr));
  RESET_FAKE(NoteI2c_receive);  // Clear the structure for testing results

   // Action
  ///////////

  const char * const ACTUAL_RESULT = noteI2cReceive(ADDRESS, response_buffer, SIZE, &actual_response_size);

   // Assert
  ///////////

  CHECK(!strcmp(ACTUAL_RESULT,EXPECTED_RESULT));
}

TEST_CASE("test_static_callback_note_i2c_reset_invokes_notei2c_reset", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x17;

  Notecard notecard;
  NoteI2c_Fake fakeI2c;  // Instantiate NoteI2c (faked)
  notecard.begin(&fakeI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cResetFn noteI2cReset = NoteSetFnI2CDefault_fake.arg2_val;
  RESET_FAKE(NoteI2c_reset);  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cReset(ADDRESS);

   // Assert
  ///////////

  CHECK(NoteI2c_reset_fake.call_count);
}

TEST_CASE("test_static_callback_note_i2c_reset_does_not_modify_device_address_parameter_before_passing_to_interface_method", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  // 0x1B binary representation => 0001 1011
  const uint16_t EXPECTED_ADDRESS = 0x1B;

  Notecard notecard;
  NoteI2c_Fake fakeI2c;  // Instantiate NoteI2c (faked)
  notecard.begin(&fakeI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cResetFn noteI2cReset = NoteSetFnI2CDefault_fake.arg2_val;
  RESET_FAKE(NoteI2c_reset);  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cReset(EXPECTED_ADDRESS);

   // Assert
  ///////////

  CHECK(EXPECTED_ADDRESS == NoteI2c_reset_fake.arg0_val);
}

TEST_CASE("test_static_callback_note_i2c_reset_does_not_modify_interface_method_return_value", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;

  Notecard notecard;
  NoteI2c_Fake fakeI2c;  // Instantiate NoteI2c (faked)
  notecard.begin(&fakeI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cResetFn noteI2cReset = NoteSetFnI2CDefault_fake.arg2_val;
  RESET_FAKE(NoteI2c_reset);  // Clear the structure for testing results
  NoteI2c_reset_fake.return_val = true;

   // Action
  ///////////

  const bool ACTUAL_RESULT = noteI2cReset(ADDRESS);

   // Assert
  ///////////

  CHECK(ACTUAL_RESULT == NoteI2c_reset_fake.return_val);
}

TEST_CASE("test_static_callback_note_i2c_reset_does_not_call_interface_method_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;

  Notecard notecard;
  NoteI2c_Fake fakeI2c;
  notecard.begin(&fakeI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cResetFn noteI2cReset = NoteSetFnI2CDefault_fake.arg2_val;

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteI2c *>(nullptr));
  RESET_FAKE(NoteI2c_reset);  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cReset(ADDRESS);

   // Assert
  ///////////

  CHECK(!NoteI2c_reset_fake.call_count);
}

TEST_CASE("test_static_callback_note_i2c_reset_returns_false_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;

  Notecard notecard;
  NoteI2c_Fake fakeI2c;
  notecard.begin(&fakeI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cResetFn noteI2cReset = NoteSetFnI2CDefault_fake.arg2_val;

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteI2c *>(nullptr));
  RESET_FAKE(NoteI2c_reset);  // Clear the structure for testing results
  NoteI2c_reset_fake.return_val = true;  // Ensure that if the mock were to be invoked the test would fail

   // Action
  ///////////

  const bool ACTUAL_RESULT = noteI2cReset(ADDRESS);

   // Assert
  ///////////

  CHECK(!ACTUAL_RESULT);
}

TEST_CASE("test_static_callback_note_i2c_transmit_invokes_notei2c_transmit", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x17;
  const uint16_t SIZE = 13;
  uint8_t transmit_buffer[] = "Transmissible data";

  Notecard notecard;
  NoteI2c_Fake fakeI2c;  // Instantiate NoteI2c (faked)
  notecard.begin(&fakeI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cTransmitFn noteI2cTransmit = NoteSetFnI2CDefault_fake.arg3_val;
  RESET_FAKE(NoteI2c_transmit);  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cTransmit(ADDRESS, transmit_buffer, SIZE);

   // Assert
  ///////////

  CHECK(NoteI2c_transmit_fake.call_count);
}

TEST_CASE("test_static_callback_note_i2c_transmit_does_not_modify_device_address_parameter_before_passing_to_interface_method", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  // 0x1B binary representation => 0001 1011
  const uint16_t EXPECTED_ADDRESS = 0x1B;
  const uint16_t SIZE = 13;
  uint8_t transmit_buffer[] = "Transmissible data";

  Notecard notecard;
  NoteI2c_Fake fakeI2c;  // Instantiate NoteI2c (faked)
  notecard.begin(&fakeI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cTransmitFn noteI2cTransmit = NoteSetFnI2CDefault_fake.arg3_val;
  RESET_FAKE(NoteI2c_transmit);  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cTransmit(EXPECTED_ADDRESS, transmit_buffer, SIZE);

   // Assert
  ///////////

  CHECK(EXPECTED_ADDRESS == NoteI2c_transmit_fake.arg0_val);
}

TEST_CASE("test_static_callback_note_i2c_transmit_does_not_modify_buffer_parameter_before_passing_to_interface_method", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;
  const uint16_t SIZE = 13;
  uint8_t transmit_buffer[] = "Test Passed!";

  Notecard notecard;
  NoteI2c_Fake fakeI2c;  // Instantiate NoteI2c (faked)
  notecard.begin(&fakeI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cTransmitFn noteI2cTransmit = NoteSetFnI2CDefault_fake.arg3_val;
  RESET_FAKE(NoteI2c_transmit);  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cTransmit(ADDRESS, transmit_buffer, SIZE);

   // Assert
  ///////////

  CHECK(!strcmp(reinterpret_cast<char *>(transmit_buffer),reinterpret_cast<char *>(NoteI2c_transmit_fake.arg1_val)));
}

TEST_CASE("test_static_callback_note_i2c_transmit_does_not_modify_size_parameter_before_passing_to_interface_method", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;
  const uint16_t EXPECTED_SIZE = 13;
  uint8_t transmit_buffer[] = "Transmissible data";

  Notecard notecard;
  NoteI2c_Fake fakeI2c;  // Instantiate NoteI2c (faked)
  notecard.begin(&fakeI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cTransmitFn noteI2cTransmit = NoteSetFnI2CDefault_fake.arg3_val;
  RESET_FAKE(NoteI2c_transmit);  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cTransmit(ADDRESS, transmit_buffer, EXPECTED_SIZE);

   // Assert
  ///////////

  CHECK(EXPECTED_SIZE == NoteI2c_transmit_fake.arg2_val);
}

TEST_CASE("test_static_callback_note_i2c_transmit_does_not_modify_interface_method_return_value", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;
  const uint16_t SIZE = 13;
  uint8_t transmit_buffer[] = "Transmissible data";

  Notecard notecard;
  NoteI2c_Fake fakeI2c;  // Instantiate NoteI2c (faked)
  notecard.begin(&fakeI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cTransmitFn noteI2cTransmit = NoteSetFnI2CDefault_fake.arg3_val;
  RESET_FAKE(NoteI2c_transmit);  // Clear the structure for testing results
  NoteI2c_transmit_fake.return_val = "i2c: fake test error!";

   // Action
  ///////////

  const char * const ACTUAL_RESULT = noteI2cTransmit(ADDRESS, transmit_buffer, SIZE);

   // Assert
  ///////////

  CHECK(!strcmp(ACTUAL_RESULT, NoteI2c_transmit_fake.return_val));
}

TEST_CASE("test_static_callback_note_i2c_transmit_does_not_call_interface_method_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;
  const uint16_t SIZE = 13;
  uint8_t transmit_buffer[] = "Transmissible data";

  Notecard notecard;
  NoteI2c_Fake fakeI2c;
  notecard.begin(&fakeI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cTransmitFn noteI2cTransmit = NoteSetFnI2CDefault_fake.arg3_val;

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteI2c *>(nullptr));
  RESET_FAKE(NoteI2c_transmit);  // Clear the structure for testing results

   // Action
  ///////////

  noteI2cTransmit(ADDRESS, transmit_buffer, SIZE);

   // Assert
  ///////////

  CHECK(!NoteI2c_transmit_fake.call_count);
}

TEST_CASE("test_static_callback_note_i2c_transmit_returns_error_message_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const uint16_t ADDRESS = 0x1B;
  const uint16_t SIZE = 13;
  uint8_t transmit_buffer[] = "Transmissible data";
  const char * const EXPECTED_RESULT = "i2c: A call to Notecard::begin() is required. {io}";

  Notecard notecard;
  NoteI2c_Fake fakeI2c;
  notecard.begin(&fakeI2c);  // Provides access to the hidden static callback methods through `note-c` mocks
  i2cTransmitFn noteI2cTransmit = NoteSetFnI2CDefault_fake.arg3_val;

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteI2c *>(nullptr));
  RESET_FAKE(NoteI2c_transmit);  // Clear the structure for testing results

   // Action
  ///////////

  const char * const ACTUAL_RESULT = noteI2cTransmit(ADDRESS, transmit_buffer, SIZE);

   // Assert
  ///////////

  CHECK(!strcmp(ACTUAL_RESULT,EXPECTED_RESULT));
}

TEST_CASE("test_static_callback_note_log_print_invokes_notelog_print", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const char message[] = "Test passed!";

  Notecard notecard;
  NoteLog_Fake fakeLog;  // Instantiate NoteLog (faked)
  notecard.setDebugOutputStream(&fakeLog);  // Provides access to the hidden static callback methods through `note-c` mocks
  debugOutputFn noteLogPrint = NoteSetFnDebugOutput_fake.arg0_val;  // Capture the internal Notecard log function, `noteLogPrint`
  RESET_FAKE(NoteLog_print);  // Clear the structure for testing NoteLog::print results

   // Action
  ///////////

  noteLogPrint(message);

   // Assert
  ///////////

  CHECK(NoteLog_print_fake.call_count);
}

TEST_CASE("test_static_callback_note_log_print_does_not_modify_message_parameter_before_passing_to_interface_method", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const char message[] = "Test passed!";

  Notecard notecard;
  NoteLog_Fake fakeLog;  // Instantiate NoteLog (faked)
  notecard.setDebugOutputStream(&fakeLog);  // Provides access to the hidden static callback methods through `note-c` mocks
  debugOutputFn noteLogPrint = NoteSetFnDebugOutput_fake.arg0_val;  // Capture the internal Notecard log function, `noteLogPrint`
  RESET_FAKE(NoteLog_print);  // Clear the structure for testing NoteLog::print results

   // Action
  ///////////

  noteLogPrint(message);

   // Assert
  ///////////

  CHECK(!strcmp(message, NoteLog_print_fake.arg0_val));
}

TEST_CASE("test_static_callback_note_log_print_does_not_modify_interface_method_return_value", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const char message[] = "Test passed!";

  Notecard notecard;
  NoteLog_Fake fakeLog;  // Instantiate NoteLog (faked)
  notecard.setDebugOutputStream(&fakeLog);  // Provides access to the hidden static callback methods through `note-c` mocks
  debugOutputFn noteLogPrint = NoteSetFnDebugOutput_fake.arg0_val;  // Capture the internal Notecard log function, `noteLogPrint`
  RESET_FAKE(NoteLog_print);  // Clear the structure for testing NoteLog::print results
  NoteLog_print_fake.return_val = sizeof(message);

   // Action
  ///////////

  const size_t ACTUAL_RESULT = noteLogPrint(message);

   // Assert
  ///////////

  CHECK(ACTUAL_RESULT == NoteLog_print_fake.return_val);
}

TEST_CASE("test_static_callback_note_log_print_does_not_call_interface_method_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const char message[] = "Test passed!";

  Notecard notecard;

  // Capture the internal Notecard log function, `noteLogPrint`
  NoteLog_Fake fakeLog;
  notecard.setDebugOutputStream(&fakeLog);  // Provides access to the internal static callback methods through `note-c` mocks
  debugOutputFn noteLogPrint = NoteSetFnDebugOutput_fake.arg0_val;  // Capture the internal Notecard log function, `noteLogPrint`

  // Reset to ensure interface is not instantiated
  notecard.setDebugOutputStream(nullptr);
  RESET_FAKE(NoteLog_print);  // Clear the structure for testing NoteLog::print results

   // Action
  ///////////

  noteLogPrint(message);

   // Assert
  ///////////

  CHECK(!NoteLog_print_fake.call_count);
}

TEST_CASE("test_static_callback_note_log_print_returns_false_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const char message[] = "Test passed!";

  Notecard notecard;

  // Capture the internal Notecard log function, `noteLogPrint`
  NoteLog_Fake fakeLog;
  notecard.setDebugOutputStream(&fakeLog);  // Provides access to the internal static callback methods through `note-c` mocks
  debugOutputFn noteLogPrint = NoteSetFnDebugOutput_fake.arg0_val;  // Capture the internal Notecard log function, `noteLogPrint`

  // Reset to ensure interface is not instantiated
  notecard.setDebugOutputStream(nullptr);
  RESET_FAKE(NoteLog_print);  // Clear the structure for testing NoteLog::print results
  NoteLog_print_fake.return_val = sizeof(message);  // Ensure that if the mock were to be invoked the test would fail

   // Action
  ///////////

  const size_t ACTUAL_RESULT = noteLogPrint(message);

   // Assert
  ///////////

  CHECK(!ACTUAL_RESULT);
  CHECK(!NoteLog_print_fake.call_count);
}

TEST_CASE("test_static_callback_note_serial_available_invokes_noteserial_available", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  notecard.begin(&fakeSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialAvailableFn noteSerialAvailable = NoteSetFnSerialDefault_fake.arg2_val;  // Capture the internal Notecard serial function, `noteSerialAvailable`
  RESET_FAKE(NoteSerial_available);  // Clear the structure for testing results

   // Action
  ///////////

  noteSerialAvailable();

   // Assert
  ///////////

  CHECK(NoteSerial_available_fake.call_count);
}

TEST_CASE("test_static_callback_note_serial_available_does_not_modify_interface_method_return_value", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  notecard.begin(&fakeSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialAvailableFn noteSerialAvailable = NoteSetFnSerialDefault_fake.arg2_val;  // Capture the internal Notecard serial function, `noteSerialAvailable`
  RESET_FAKE(NoteSerial_available);  // Clear the structure for testing results
  NoteSerial_available_fake.return_val = true;

   // Action
  ///////////

  const bool ACTUAL_RESULT = noteSerialAvailable();

   // Assert
  ///////////

  CHECK(ACTUAL_RESULT == NoteSerial_available_fake.return_val);
}

TEST_CASE("test_static_callback_note_serial_available_does_not_call_interface_method_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;

  // Capture the internal Notecard log function, `noteSerialAvailable`
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  notecard.begin(&fakeSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialAvailableFn noteSerialAvailable = NoteSetFnSerialDefault_fake.arg2_val;  // Capture the internal Notecard serial function, `noteSerialAvailable`

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteSerial *>(nullptr));
  RESET_FAKE(NoteSerial_available);  // Clear the structure for testing results

   // Action
  ///////////

  noteSerialAvailable();

   // Assert
  ///////////

  CHECK(!NoteSerial_available_fake.call_count);
}

TEST_CASE("test_static_callback_note_serial_available_returns_false_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;

  // Capture the internal Notecard log function, `noteSerialAvailable`
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  notecard.begin(&fakeSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialAvailableFn noteSerialAvailable = NoteSetFnSerialDefault_fake.arg2_val;  // Capture the internal Notecard serial function, `noteSerialAvailable`

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteSerial *>(nullptr));
  RESET_FAKE(NoteSerial_available);  // Clear the structure for testing results
  NoteSerial_available_fake.return_val = true;  // Ensure that if the mock were to be invoked the test would fail

   // Action
  ///////////

  const bool ACTUAL_RESULT = noteSerialAvailable();

   // Assert
  ///////////

  CHECK(!ACTUAL_RESULT);
}

TEST_CASE("test_static_callback_note_serial_receive_invokes_noteserial_receive", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  notecard.begin(&fakeSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialReceiveFn noteSerialReceive = NoteSetFnSerialDefault_fake.arg3_val;  // Capture the internal Notecard serial function, `noteSerialReceive`
  RESET_FAKE(NoteSerial_receive);  // Clear the structure for testing results

   // Action
  ///////////

  noteSerialReceive();

   // Assert
  ///////////

  CHECK(NoteSerial_receive_fake.call_count);
}

TEST_CASE("test_static_callback_note_serial_receive_does_not_modify_interface_method_return_value", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  notecard.begin(&fakeSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialReceiveFn noteSerialReceive = NoteSetFnSerialDefault_fake.arg3_val;  // Capture the internal Notecard serial function, `noteSerialReceive`
  RESET_FAKE(NoteSerial_receive);  // Clear the structure for testing results
  NoteSerial_receive_fake.return_val = 'Z';

   // Action
  ///////////

  const char ACTUAL_RESULT = noteSerialReceive();

   // Assert
  ///////////

  CHECK(ACTUAL_RESULT == NoteSerial_receive_fake.return_val);
}

TEST_CASE("test_static_callback_note_serial_receive_does_not_call_interface_method_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;

  // Capture the internal Notecard log function, `noteSerialAvailable`
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  notecard.begin(&fakeSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialReceiveFn noteSerialReceive = NoteSetFnSerialDefault_fake.arg3_val;  // Capture the internal Notecard serial function, `noteSerialReceive`

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteSerial *>(nullptr));
  RESET_FAKE(NoteSerial_receive);  // Clear the structure for testing results

   // Action
  ///////////

  noteSerialReceive();

   // Assert
  ///////////

  CHECK(!NoteSerial_receive_fake.call_count);
}

TEST_CASE("test_static_callback_note_serial_receive_returns_false_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  static const char EXPECTED_RESULT = '\0';
  Notecard notecard;

  // Capture the internal Notecard log function, `noteSerialAvailable`
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  notecard.begin(&fakeSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialReceiveFn noteSerialReceive = NoteSetFnSerialDefault_fake.arg3_val;  // Capture the internal Notecard serial function, `noteSerialReceive`

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteSerial *>(nullptr));
  RESET_FAKE(NoteSerial_receive);  // Clear the structure for testing results
  NoteSerial_receive_fake.return_val = 'Z';

   // Action
  ///////////

  const char ACTUAL_RESULT = noteSerialReceive();

   // Assert
  ///////////

  CHECK(ACTUAL_RESULT == EXPECTED_RESULT);
}

TEST_CASE("test_static_callback_note_serial_reset_invokes_noteserial_reset", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  notecard.begin(&fakeSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialResetFn noteSerialReset = NoteSetFnSerialDefault_fake.arg0_val;  // Capture the internal Notecard serial function, `noteSerialReset`
  RESET_FAKE(NoteSerial_reset);  // Clear the structure for testing results

   // Action
  ///////////

  noteSerialReset();

   // Assert
  ///////////

  CHECK(NoteSerial_reset_fake.call_count);
}

TEST_CASE("test_static_callback_note_serial_reset_does_not_modify_interface_method_return_value", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  notecard.begin(&fakeSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialResetFn noteSerialReset = NoteSetFnSerialDefault_fake.arg0_val;  // Capture the internal Notecard serial function, `noteSerialReset`
  RESET_FAKE(NoteSerial_reset);  // Clear the structure for testing results
  NoteSerial_reset_fake.return_val = true;

   // Action
  ///////////

  const bool ACTUAL_RESULT = noteSerialReset();

   // Assert
  ///////////

  CHECK(ACTUAL_RESULT == NoteSerial_reset_fake.return_val);
}

TEST_CASE("test_static_callback_note_serial_reset_does_not_call_interface_method_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;

  // Capture the internal Notecard log function, `noteSerialAvailable`
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  notecard.begin(&fakeSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialResetFn noteSerialReset = NoteSetFnSerialDefault_fake.arg0_val;  // Capture the internal Notecard serial function, `noteSerialReset`

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteSerial *>(nullptr));
  RESET_FAKE(NoteSerial_reset);  // Clear the structure for testing results

   // Action
  ///////////

  noteSerialReset();

   // Assert
  ///////////

  CHECK(!NoteSerial_reset_fake.call_count);
}

TEST_CASE("test_static_callback_note_serial_reset_returns_false_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;

  // Capture the internal Notecard log function, `noteSerialAvailable`
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  notecard.begin(&fakeSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialResetFn noteSerialReset = NoteSetFnSerialDefault_fake.arg0_val;  // Capture the internal Notecard serial function, `noteSerialReset`

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteSerial *>(nullptr));
  RESET_FAKE(NoteSerial_reset);  // Clear the structure for testing results
  NoteSerial_reset_fake.return_val = true;

   // Action
  ///////////

  const bool ACTUAL_RESULT = noteSerialReset();

   // Assert
  ///////////

  CHECK(!ACTUAL_RESULT);
}

TEST_CASE("test_static_callback_note_serial_transmit_invokes_noteserial_transmit", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  uint8_t text[] = "Test passed!";
  static const size_t LEN = sizeof(text);
  static const bool FLUSH = true;

  Notecard notecard;
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  notecard.begin(&fakeSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialTransmitFn noteSerialTransmit = NoteSetFnSerialDefault_fake.arg1_val;  // Capture the internal Notecard serial function, `noteSerialTransmit`
  RESET_FAKE(NoteSerial_transmit);  // Clear the structure for testing results

   // Action
  ///////////

  noteSerialTransmit(text, LEN, FLUSH);

   // Assert
  ///////////

  CHECK(NoteSerial_transmit_fake.call_count);
}

TEST_CASE("test_static_callback_note_serial_transmit_does_not_modify_text_parameter_before_passing_to_interface_method", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  uint8_t expected_text[] = "Test passed!";
  const size_t LEN = sizeof(expected_text);
  const bool FLUSH = true;

  Notecard notecard;
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  notecard.begin(&fakeSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialTransmitFn noteSerialTransmit = NoteSetFnSerialDefault_fake.arg1_val;  // Capture the internal Notecard serial function, `noteSerialTransmit`
  RESET_FAKE(NoteSerial_transmit);  // Clear the structure for testing results

   // Action
  ///////////

  noteSerialTransmit(expected_text, LEN, FLUSH);

   // Assert
  ///////////

  CHECK(!strcmp(reinterpret_cast<char *>(expected_text),reinterpret_cast<char *>(NoteSerial_transmit_fake.arg0_val)));
}

TEST_CASE("test_static_callback_note_serial_transmit_does_not_modify_length_parameter_before_passing_to_interface_method", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  uint8_t text[] = "Test passed!";
  const size_t EXPECTED_LEN = sizeof(text);
  const bool FLUSH = true;

  Notecard notecard;
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  notecard.begin(&fakeSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialTransmitFn noteSerialTransmit = NoteSetFnSerialDefault_fake.arg1_val;  // Capture the internal Notecard serial function, `noteSerialTransmit`
  RESET_FAKE(NoteSerial_transmit);  // Clear the structure for testing results

   // Action
  ///////////

  noteSerialTransmit(text, EXPECTED_LEN, FLUSH);

   // Assert
  ///////////

  CHECK(EXPECTED_LEN == NoteSerial_transmit_fake.arg1_val);
}

TEST_CASE("test_static_callback_note_serial_transmit_does_not_modify_flush_parameter_before_passing_to_interface_method", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  uint8_t text[] = "Test passed!";
  const size_t LEN = sizeof(text);
  const bool EXPECTED_FLUSH = true;

  Notecard notecard;
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  notecard.begin(&fakeSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialTransmitFn noteSerialTransmit = NoteSetFnSerialDefault_fake.arg1_val;  // Capture the internal Notecard serial function, `noteSerialTransmit`
  RESET_FAKE(NoteSerial_transmit);  // Clear the structure for testing results

   // Action
  ///////////

  noteSerialTransmit(text, LEN, EXPECTED_FLUSH);

   // Assert
  ///////////

  CHECK(EXPECTED_FLUSH == NoteSerial_transmit_fake.arg2_val);
}

TEST_CASE("test_static_callback_note_serial_transmit_does_not_call_interface_method_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  uint8_t text[] = "Test passed!";
  const size_t LEN = sizeof(text);
  const bool EXPECTED_FLUSH = true;

  Notecard notecard;

  // Capture the internal Notecard log function, `noteSerialAvailable`
  NoteSerial_Fake fakeSerial;  // Instantiate NoteSerial (faked)
  notecard.begin(&fakeSerial);  // Provides access to the hidden static callback methods through `note-c` mocks
  serialTransmitFn noteSerialTransmit = NoteSetFnSerialDefault_fake.arg1_val;  // Capture the internal Notecard serial function, `noteSerialTransmit`

  // Reset to ensure the interface is not instantiated
  notecard.begin(static_cast<NoteSerial *>(nullptr));
  RESET_FAKE(NoteSerial_transmit);  // Clear the structure for testing results

   // Action
  ///////////

  noteSerialTransmit(text, LEN, EXPECTED_FLUSH);

   // Assert
  ///////////

  CHECK(!NoteSerial_transmit_fake.call_count);
}

TEST_CASE("test_static_callback_note_txn_start_invokes_notetxn_start", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const size_t TIMEOUT_MS = 917;

  Notecard notecard;

  // Capture the internal Notecard transaction function, `noteTransactionStart`
  NoteTxn_Fake fakeTxn;  // Instantiate NoteTxn (faked)
  notecard.setTransactionPins(&fakeTxn);  // Provides access to the hidden static callback methods through `note-c` mocks
  txnStartFn noteTransactionStart = NoteSetFnTransaction_fake.arg0_val;  // Capture the internal Notecard serial function, `noteTransactionStart`
  RESET_FAKE(NoteTxn_start);  // Clear the structure for testing results

   // Action
  ///////////

  noteTransactionStart(TIMEOUT_MS);

   // Assert
  ///////////

  CHECK(NoteTxn_start_fake.call_count);
}

TEST_CASE("test_static_callback_note_txn_start_does_not_modify_timeout_ms_parameter_before_passing_to_interface_method", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const size_t TIMEOUT_MS = 917;

  Notecard notecard;

  // Capture the internal Notecard transaction function, `noteTransactionStart`
  NoteTxn_Fake fakeTxn;  // Instantiate NoteTxn (faked)
  notecard.setTransactionPins(&fakeTxn);  // Provides access to the hidden static callback methods through `note-c` mocks
  txnStartFn noteTransactionStart = NoteSetFnTransaction_fake.arg0_val;  // Capture the internal Notecard serial function, `noteTransactionStart`
  RESET_FAKE(NoteTxn_start);  // Clear the structure for testing results

   // Action
  ///////////

  noteTransactionStart(TIMEOUT_MS);

   // Assert
  ///////////

  CHECK(NoteTxn_start_fake.arg0_val == TIMEOUT_MS);
}

TEST_CASE("test_static_callback_note_txn_start_does_not_modify_interface_method_return_value", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const size_t TIMEOUT_MS = 917;

  Notecard notecard;

  // Capture the internal Notecard transaction function, `noteTransactionStart`
  NoteTxn_Fake fakeTxn;  // Instantiate NoteTxn (faked)
  notecard.setTransactionPins(&fakeTxn);  // Provides access to the hidden static callback methods through `note-c` mocks
  txnStartFn noteTransactionStart = NoteSetFnTransaction_fake.arg0_val;  // Capture the internal Notecard serial function, `noteTransactionStart`
  RESET_FAKE(NoteTxn_start);  // Clear the structure for testing results
  NoteTxn_start_fake.return_val = true;

   // Action
  ///////////

  const bool ACTUAL_RESULT = noteTransactionStart(TIMEOUT_MS);

   // Assert
  ///////////

  CHECK(NoteTxn_start_fake.return_val == ACTUAL_RESULT);
}

TEST_CASE("test_static_callback_note_txn_start_does_not_call_interface_method_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const size_t TIMEOUT_MS = 917;

  Notecard notecard;

  // Capture the internal Notecard transaction function, `noteTransactionStart`
  NoteTxn_Fake fakeTxn;  // Instantiate NoteTxn (faked)
  notecard.setTransactionPins(&fakeTxn);  // Provides access to the hidden static callback methods through `note-c` mocks
  txnStartFn noteTransactionStart = NoteSetFnTransaction_fake.arg0_val;  // Capture the internal Notecard serial function, `noteTransactionStart`

  // Reset to ensure the interface is not instantiated
  notecard.setTransactionPins(static_cast<NoteTxn *>(nullptr));
  RESET_FAKE(NoteTxn_start);  // Clear the structure for testing results

   // Action
  ///////////

  noteTransactionStart(TIMEOUT_MS);

   // Assert
  ///////////

  CHECK(!NoteTxn_start_fake.call_count);
}

TEST_CASE("test_static_callback_note_txn_start_returns_true_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  const bool EXPECTED_RESULT = true;
  const size_t TIMEOUT_MS = 917;

  Notecard notecard;

  // Capture the internal Notecard transaction function, `noteTransactionStart`
  NoteTxn_Fake fakeTxn;  // Instantiate NoteTxn (faked)
  notecard.setTransactionPins(&fakeTxn);  // Provides access to the hidden static callback methods through `note-c` mocks
  txnStartFn noteTransactionStart = NoteSetFnTransaction_fake.arg0_val;  // Capture the internal Notecard serial function, `noteTransactionStart`

  // Reset to ensure the interface is not instantiated
  notecard.setTransactionPins(static_cast<NoteTxn *>(nullptr));
  RESET_FAKE(NoteTxn_start);  // Clear the structure for testing results
  NoteTxn_start_fake.return_val = false;

   // Action
  ///////////

  const bool ACTUAL_RESULT = noteTransactionStart(TIMEOUT_MS);

   // Assert
  ///////////

  CHECK(ACTUAL_RESULT == EXPECTED_RESULT);
}

TEST_CASE("test_static_callback_note_txn_stop_invokes_notetxn_stop", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;

  // Capture the internal Notecard transaction function, `noteTransactionStop`
  NoteTxn_Fake fakeTxn;  // Instantiate NoteTxn (faked)
  notecard.setTransactionPins(&fakeTxn);  // Provides access to the hidden static callback methods through `note-c` mocks
  txnStopFn noteTransactionStop = NoteSetFnTransaction_fake.arg1_val;  // Capture the internal Notecard serial function, `noteTransactionStop`
  RESET_FAKE(NoteTxn_stop);  // Clear the structure for testing results

   // Action
  ///////////

  noteTransactionStop();

   // Assert
  ///////////

  CHECK(NoteTxn_stop_fake.call_count);
}

TEST_CASE("test_static_callback_note_txn_stop_does_not_call_interface_method_when_interface_has_not_been_instantiated", "[Notecard]")
{
  resetFakes();
   // Arrange
  ////////////

  Notecard notecard;

  // Capture the internal Notecard transaction function, `noteTransactionStop`
  NoteTxn_Fake fakeTxn;  // Instantiate NoteTxn (faked)
  notecard.setTransactionPins(&fakeTxn);  // Provides access to the hidden static callback methods through `note-c` mocks
  txnStopFn noteTransactionStop = NoteSetFnTransaction_fake.arg1_val;  // Capture the internal Notecard serial function, `noteTransactionStop`

  // Reset to ensure the interface is not instantiated
  notecard.setTransactionPins(static_cast<NoteTxn *>(nullptr));
  RESET_FAKE(NoteTxn_stop);  // Clear the structure for testing results

   // Action
  ///////////

  noteTransactionStop();

   // Assert
  ///////////

  CHECK(!NoteTxn_stop_fake.call_count);
}
