#include "NoteSerial_Arduino.hpp"
#include "TestFunction.hpp"

#include <cassert>
#include <cstring>
#include <iostream>

// Compile command: g++ -Wall -Wextra -Wpedantic mock/mock-arduino.cpp mock/mock-note-c-note.c ../src/NoteSerial_Arduino.cpp NoteSerial_Arduino.test.cpp -std=c++11 -I. -I../src -DNOTE_MOCK -ggdb -O0 -o noteSerial_arduino.tests && ./noteSerial_arduino.tests || echo "Tests Result: $?"

int test_make_note_serial_instantiates_noteserial_object()
{
    int result;

    // Arrange
    NoteSerial * noteserial = nullptr;
    MakeNoteSerial_ArduinoParameters<HardwareSerial> arduino_parameters(Serial, 9600);

    // Action
    noteserial = make_note_serial<MakeNoteSerial_ArduinoParameters<HardwareSerial>>(arduino_parameters);

    // Assert
    if (nullptr != noteserial)
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('s' + 'e' + 'r' + 'i' + 'a' + 'l');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\tnoteserial == " << !!noteserial << ", EXPECTED: not nullptr" << std::endl;
        std::cout << "[";
    }

    // Clean-up
    make_note_serial(nullptr);

    return result;
}

int test_make_note_serial_enforces_singleton_by_returning_same_noteserial_object_for_all_calls()
{
    int result;

    // Arrange
    MakeNoteSerial_ArduinoParameters<HardwareSerial> arduino_parameters(Serial, 9600);
    NoteSerial * const noteserial_1 = make_note_serial<MakeNoteSerial_ArduinoParameters<HardwareSerial>>(arduino_parameters);

    // Action
    NoteSerial * const noteserial_2 = make_note_serial<MakeNoteSerial_ArduinoParameters<HardwareSerial>>(arduino_parameters);

    // Assert
    if (noteserial_1 == noteserial_2)
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('s' + 'e' + 'r' + 'i' + 'a' + 'l');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\tnoteserial_2 == " << std::hex << noteserial_2 << ", EXPECTED: " << noteserial_1 << std::endl;
        std::cout << "[";
    }

    // Clean-up
    make_note_serial(nullptr);

    return result;
}

//int test_make_note_serial_returns_nullptr_when_nullptr_is_passed_as_parameter()
int test_make_note_serial_deletes_singleton_when_nullptr_is_passed_as_parameter()
{
    int result;

    // Arrange
    MakeNoteSerial_ArduinoParameters<HardwareSerial> arduino_parameters(Serial, 9600);
    NoteSerial * noteserial = make_note_serial<MakeNoteSerial_ArduinoParameters<HardwareSerial>>(arduino_parameters);
    assert(noteserial);

    // Action
    noteserial = make_note_serial(nullptr);

    // Assert
    if (nullptr == noteserial)
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('s' + 'e' + 'r' + 'i' + 'a' + 'l');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\tnoteserial == " << std::hex << noteserial << ", EXPECTED: 0 (nullptr)" << std::endl;
        std::cout << "[";
    }

    return result;
}

int test_noteserial_arduino_constructor_invokes_hardware_serial_parameter_begin_method()
{
    int result;

    // Arrange
    hardwareSerialBegin_Parameters.reset();

    // Action
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);

    // Assert
    if (hardwareSerialBegin_Parameters.invoked)
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('s' + 'e' + 'r' + 'i' + 'a' + 'l');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\thardwareSerialBegin_Parameters.invoked == " << !!hardwareSerialBegin_Parameters.invoked << ", EXPECTED: " << true << std::endl;
        std::cout << "[";
    }

    return result;
}

int test_noteserial_arduino_constructor_does_not_modify_baud_parameter_before_passing_to_hardware_serial_begin()
{
    int result;

    // Arrange
    const size_t EXPECTED_BAUD_RATE = 9600;

    hardwareSerialBegin_Parameters.reset();

    // Action
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, EXPECTED_BAUD_RATE);

    // Assert
    if (EXPECTED_BAUD_RATE == hardwareSerialBegin_Parameters.baud)
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('s' + 'e' + 'r' + 'i' + 'a' + 'l');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\thardwareSerialBegin_Parameters.baud == " << hardwareSerialBegin_Parameters.baud << ", EXPECTED: " << EXPECTED_BAUD_RATE << std::endl;
        std::cout << "[";
    }

    return result;
}

int test_noteserial_arduino_deconstructor_invokes_hardware_serial_end_method()
{
    int result;

    // Arrange
    hardwareSerialEnd_Parameters.reset();
    NoteSerial_Arduino<HardwareSerial> * noteserial = new NoteSerial_Arduino<HardwareSerial>(Serial, 9600);

    // Action
    delete noteserial;

    // Assert
    if (hardwareSerialEnd_Parameters.invoked)
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('s' + 'e' + 'r' + 'i' + 'a' + 'l');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\thardwareSerialEnd_Parameters.invoked == " << !!hardwareSerialEnd_Parameters.invoked << ", EXPECTED: " << true << std::endl;
        std::cout << "[";
    }

    return result;
}

int test_noteserial_arduino_available_invokes_hardware_serial_available()
{
    int result;

    // Arrange
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);
    hardwareSerialAvailable_Parameters.reset();

    // Action
    noteserial.available();

    // Assert
    if (hardwareSerialAvailable_Parameters.invoked)
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('s' + 'e' + 'r' + 'i' + 'a' + 'l');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\thardwareSerialAvailable_Parameters.invoked == " << !!hardwareSerialAvailable_Parameters.invoked << ", EXPECTED: " << true << std::endl;
        std::cout << "[";
    }

    return result;
}

int test_noteserial_arduino_available_does_not_modify_hardware_serial_available_result_value_before_returning_to_caller()
{
    int result;

    // Arrange
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);
    const size_t EXPECTED_RESULT = 79;

    hardwareSerialAvailable_Parameters.reset();
    hardwareSerialAvailable_Parameters.result = EXPECTED_RESULT;

    // Action
    const size_t ACTUAL_RESULT = noteserial.available();

    // Assert
    if (ACTUAL_RESULT == EXPECTED_RESULT)
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('s' + 'e' + 'r' + 'i' + 'a' + 'l');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\tnoteserial.available() == " << ACTUAL_RESULT << ", EXPECTED: " << EXPECTED_RESULT << std::endl;
        std::cout << "[";
    }

    return result;
}

int test_noteserial_arduino_receive_invokes_hardware_serial_read()
{
    int result;

    // Arrange
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);

    hardwareSerialRead_Parameters.reset();

    // Action
    noteserial.receive();

    // Assert
    if (hardwareSerialRead_Parameters.invoked)
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('s' + 'e' + 'r' + 'i' + 'a' + 'l');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\thardwareSerialRead_Parameters.invoked == " << hardwareSerialRead_Parameters.invoked << ", EXPECTED: " << true << std::endl;
        std::cout << "[";
    }

    return result;
}

int test_noteserial_arduino_receive_does_not_modify_hardware_serial_read_result_value_before_returning_to_caller()
{
    int result;

    // Arrange
    const char EXPECTED_RESULT = 'z';
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);

    hardwareSerialRead_Parameters.reset();
    hardwareSerialRead_Parameters.result = EXPECTED_RESULT;

    // Action
    const char ACTUAL_RESULT = noteserial.receive();

    // Assert
    if (ACTUAL_RESULT == EXPECTED_RESULT)
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('s' + 'e' + 'r' + 'i' + 'a' + 'l');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\tnoteserial.receive() == " << ACTUAL_RESULT << ", EXPECTED: " << EXPECTED_RESULT << std::endl;
        std::cout << "[";
    }

    return result;
}

int test_noteserial_arduino_reset_invokes_hardware_serial_end()
{
    int result;

    // Arrange
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);
    hardwareSerialEnd_Parameters.reset();

    // Action
    noteserial.reset();

    // Assert
    if (hardwareSerialEnd_Parameters.invoked)
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('s' + 'e' + 'r' + 'i' + 'a' + 'l');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\thardwareSerialEnd_Parameters.invoked == " << !!hardwareSerialEnd_Parameters.invoked << ", EXPECTED: " << true << std::endl;
        std::cout << "[";
    }

    return result;
}

int test_noteserial_arduino_reset_invokes_hardware_serial_begin()
{
    int result;

    // Arrange
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);
    hardwareSerialBegin_Parameters.reset();

    // Action
    noteserial.reset();

    // Assert
    if (hardwareSerialBegin_Parameters.invoked)
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('s' + 'e' + 'r' + 'i' + 'a' + 'l');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\thardwareSerialBegin_Parameters.invoked == " << !!hardwareSerialBegin_Parameters.invoked << ", EXPECTED: " << true << std::endl;
        std::cout << "[";
    }

    return result;
}

int test_noteserial_arduino_reset_invokes_hardware_serial_begin_with_the_baud_parameter_that_was_originally_supplied_to_the_constructor()
{
    int result;

    // Arrange
    const size_t EXPECTED_BAUD_RATE = 9600;
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, EXPECTED_BAUD_RATE);

    hardwareSerialBegin_Parameters.reset();

    // Action
    noteserial.reset();

    // Assert
    if (EXPECTED_BAUD_RATE == hardwareSerialBegin_Parameters.baud)
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('s' + 'e' + 'r' + 'i' + 'a' + 'l');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\thardwareSerialBegin_Parameters.baud == " << hardwareSerialBegin_Parameters.baud << ", EXPECTED: " << EXPECTED_BAUD_RATE << std::endl;
        std::cout << "[";
    }

    return result;
}

int test_noteserial_arduino_reset_always_returns_true()
{
    int result;

    // Arrange
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);

    // Action
    const bool ACTUAL_RESULT = noteserial.reset();

    // Assert
    if (ACTUAL_RESULT)
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('s' + 'e' + 'r' + 'i' + 'a' + 'l');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\tnoteserial.reset() == " << ACTUAL_RESULT << ", EXPECTED: " << true << std::endl;
        std::cout << "[";
    }

    return result;
}

int test_noteserial_arduino_transmit_invokes_hardware_serial_write()
{
    int result;

    // Arrange
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);

    hardwareSerialWrite_Parameters.reset();

    // Action
    noteserial.transmit(reinterpret_cast<uint8_t *>(const_cast<char *>("Hello, Test!")), 13, true);

    // Assert
    if (hardwareSerialWrite_Parameters.invoked)
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('s' + 'e' + 'r' + 'i' + 'a' + 'l');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\thardwareSerialWrite_Parameters.invoked == " << !!hardwareSerialWrite_Parameters.invoked << ", EXPECTED: " << true << std::endl;
        std::cout << "[";
    }

    return result;
}

int test_noteserial_arduino_transmit_does_not_modify_buffer_parameter_value_before_passing_to_hardware_serial_write()
{
    int result;

    // Arrange
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);
    const char EXPECTED_RESULT[] = "Hello, Test!";

    hardwareSerialWrite_Parameters.reset();

    // Action
    noteserial.transmit(reinterpret_cast<uint8_t *>(const_cast<char *>(EXPECTED_RESULT)), strlen(EXPECTED_RESULT), true);

    // Assert
    if (!strcmp(EXPECTED_RESULT, hardwareSerialWrite_Parameters.buffer_cache.c_str()))
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('s' + 'e' + 'r' + 'i' + 'a' + 'l');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\thardwareSerialWrite_Parameters.buffer_cache.c_str() == \"" << hardwareSerialWrite_Parameters.buffer_cache.c_str() << "\", EXPECTED: \"" << EXPECTED_RESULT << "\"" << std::endl;
        std::cout << "[";
    }

    return result;
}

int test_noteserial_arduino_transmit_does_not_modify_size_parameter_value_before_passing_to_hardware_serial_write()
{
    int result;

    // Arrange
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);
    const size_t EXPECTED_RESULT = 13;

    hardwareSerialWrite_Parameters.reset();

    // Action
    noteserial.transmit(reinterpret_cast<uint8_t *>(const_cast<char *>("Hello, Test!")), EXPECTED_RESULT, true);

    // Assert
    if (EXPECTED_RESULT == hardwareSerialWrite_Parameters.size)
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('s' + 'e' + 'r' + 'i' + 'a' + 'l');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\thardwareSerialWrite_Parameters.size == " << hardwareSerialWrite_Parameters.size << ", EXPECTED: " << EXPECTED_RESULT << std::endl;
        std::cout << "[";
    }

    return result;
}

int test_noteserial_arduino_transmit_invokes_hardware_serial_flush_when_flush_parameter_is_true()
{
    int result;

    // Arrange
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);
    hardwareSerialFlush_Parameters.reset();

    // Action
    noteserial.transmit(reinterpret_cast<uint8_t *>(const_cast<char *>("Hello, Test!")), 13, true);

    // Assert
    if (hardwareSerialFlush_Parameters.invoked)
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('s' + 'e' + 'r' + 'i' + 'a' + 'l');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\thardwareSerialFlush_Parameters.invoked == " << !!hardwareSerialFlush_Parameters.invoked << ", EXPECTED: " << true << std::endl;
        std::cout << "[";
    }

    return result;
}

int test_noteserial_arduino_transmit_does_not_invoke_hardware_serial_flush_when_flush_parameter_is_false()
{
    int result;

    // Arrange
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);
    hardwareSerialFlush_Parameters.reset();

    // Action
    noteserial.transmit(reinterpret_cast<uint8_t *>(const_cast<char *>("Hello, Test 14!")), 16, false);

    // Assert
    if (!hardwareSerialFlush_Parameters.invoked)
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('s' + 'e' + 'r' + 'i' + 'a' + 'l');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\thardwareSerialFlush_Parameters.invoked == " << !!hardwareSerialFlush_Parameters.invoked << ", EXPECTED: " << false << std::endl;
        std::cout << "[";
    }

    return result;
}

int test_noteserial_arduino_transmit_does_not_modify_hardware_serial_write_result_value_before_returning_to_caller()
{
    int result;

    // Arrange
    const size_t EXPECTED_RESULT = 13;
    NoteSerial_Arduino<HardwareSerial> noteserial(Serial, 9600);

    hardwareSerialWrite_Parameters.reset();
    hardwareSerialWrite_Parameters.result = EXPECTED_RESULT;

    // Action
    const size_t ACTUAL_RESULT = noteserial.transmit(reinterpret_cast<uint8_t *>(const_cast<char *>("Hello, Test!")), 13, true);

    // Assert
    if (ACTUAL_RESULT == EXPECTED_RESULT)
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('s' + 'e' + 'r' + 'i' + 'a' + 'l');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\tnoteserial.transmit(reinterpret_cast<uint8_t *>(const_cast<char *>(\"Hello, Test!\")), 13, true) == " << ACTUAL_RESULT << ", EXPECTED: " << EXPECTED_RESULT << std::endl;
        std::cout << "[";
    }

    return result;
}

int main(void)
{
    TestFunction tests[] = {
        {test_make_note_serial_instantiates_noteserial_object, "test_make_note_serial_instantiates_noteserial_object"},
        {test_make_note_serial_enforces_singleton_by_returning_same_noteserial_object_for_all_calls, "test_make_note_serial_enforces_singleton_by_returning_same_noteserial_object_for_all_calls"},
        {test_make_note_serial_deletes_singleton_when_nullptr_is_passed_as_parameter, "test_make_note_serial_deletes_singleton_when_nullptr_is_passed_as_parameter"},
        {test_noteserial_arduino_constructor_invokes_hardware_serial_parameter_begin_method, "test_noteserial_arduino_constructor_invokes_hardware_serial_parameter_begin_method"},
        {test_noteserial_arduino_constructor_does_not_modify_baud_parameter_before_passing_to_hardware_serial_begin, "test_noteserial_arduino_constructor_does_not_modify_baud_parameter_before_passing_to_hardware_serial_begin"},
        {test_noteserial_arduino_deconstructor_invokes_hardware_serial_end_method, "test_noteserial_arduino_deconstructor_invokes_hardware_serial_end_method"},
        {test_noteserial_arduino_available_invokes_hardware_serial_available, "test_noteserial_arduino_available_invokes_hardware_serial_available"},
        {test_noteserial_arduino_available_does_not_modify_hardware_serial_available_result_value_before_returning_to_caller, "test_noteserial_arduino_available_does_not_modify_hardware_serial_available_result_value_before_returning_to_caller"},
        {test_noteserial_arduino_receive_invokes_hardware_serial_read, "test_noteserial_arduino_receive_invokes_hardware_serial_read"},
        {test_noteserial_arduino_receive_does_not_modify_hardware_serial_read_result_value_before_returning_to_caller, "test_noteserial_arduino_receive_does_not_modify_hardware_serial_read_result_value_before_returning_to_caller"},
        {test_noteserial_arduino_reset_invokes_hardware_serial_begin, "test_noteserial_arduino_reset_invokes_hardware_serial_begin"},
        {test_noteserial_arduino_reset_invokes_hardware_serial_begin_with_the_baud_parameter_that_was_originally_supplied_to_the_constructor, "test_noteserial_arduino_reset_invokes_hardware_serial_begin_with_the_baud_parameter_that_was_originally_supplied_to_the_constructor"},
        {test_noteserial_arduino_reset_invokes_hardware_serial_end, "test_noteserial_arduino_reset_invokes_hardware_serial_end"},
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
