#include "NoteLog_Arduino.hpp"
#include "TestFunction.hpp"

#include <cassert>
#include <cstring>
#include <iostream>

// Compile command: g++ -Wall -Wextra -Wpedantic mock/mock-arduino.cpp ../src/NoteLog_Arduino.cpp NoteLog_Arduino.test.cpp -std=c++11 -I. -I../src -DNOTE_MOCK -ggdb -O0 -o noteLog_arduino.tests && ./noteLog_arduino.tests || echo "Tests Result: $?"

int test_make_note_log_instantiates_notelog_object()
{
    int result;

    // Arrange
    NoteLog * notelog = nullptr;
    Stream & serial_stream = Serial;

    // Action
    notelog = make_note_log(serial_stream);

    // Assert
    if (nullptr != notelog)
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('d' + 'e' + 'b' + 'u' + 'g');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\tnotelog == " << !!notelog << ", EXPECTED: not nullptr" << std::endl;
        std::cout << "[";
    }

    // Clean-up
    make_note_log(nullptr);

    return result;
}

int test_make_note_log_enforces_singleton_by_returning_same_notelog_object_for_all_calls()
{
    int result;

    // Arrange
    Stream & serial_stream = Serial;
    NoteLog * const notelog_1 = make_note_log(serial_stream);

    // Action
    NoteLog * const notelog_2 = make_note_log(serial_stream);

    // Assert
    if (notelog_1 == notelog_2)
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('d' + 'e' + 'b' + 'u' + 'g');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\tnotelog_2 == " << std::hex << notelog_2 << ", EXPECTED: " << notelog_1 << std::endl;
        std::cout << "[";
    }

    // Clean-up
    make_note_log(nullptr);

    return result;
}

//int test_make_note_log_returns_nullptr_when_nullptr_is_passed_as_parameter()
int test_make_note_log_deletes_singleton_when_nullptr_is_passed_as_parameter()
{
    int result;

    // Arrange
    Stream & serial_stream = Serial;
    NoteLog * notelog = make_note_log(serial_stream);
    assert(notelog);

    // Action
    notelog = make_note_log(nullptr);

    // Assert
    if (nullptr == notelog)
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('d' + 'e' + 'b' + 'u' + 'g');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\tnotelog == " << std::hex << notelog << ", EXPECTED: 0 (nullptr)" << std::endl;
        std::cout << "[";
    }

    return result;
}

int test_notelog_arduino_print_does_not_modify_str_parameter_value_before_passing_to_stream_print()
{
    int result;

    // Arrange
    NoteLog_Arduino notelog(&Serial);
    const char EXPECTED_RESULT[] = "Hello, Test!";

    streamPrint_Parameters.reset();

    // Action
    notelog.print(EXPECTED_RESULT);

    // Assert
    if (!strcmp(EXPECTED_RESULT, streamPrint_Parameters.str_cache.c_str()))
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('d' + 'e' + 'b' + 'u' + 'g');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\tstreamPrint_Parameters.str_cache.c_str() == \"" << streamPrint_Parameters.str_cache.c_str() << "\", EXPECTED: \"" << EXPECTED_RESULT << "\"" << std::endl;
        std::cout << "[";
    }

    return result;
}

int test_notelog_arduino_print_does_not_modify_stream_print_result_value_before_returning_to_caller()
{
    int result;

    // Arrange
    NoteLog_Arduino notelog(&Serial);
    const size_t EXPECTED_RESULT = 13;

    streamPrint_Parameters.reset();
    streamPrint_Parameters.result = EXPECTED_RESULT;

    // Action
    const size_t ACTUAL_RESULT = notelog.print("Hello, Test!");

    // Assert
    if (ACTUAL_RESULT == EXPECTED_RESULT)
    {
        result = 0;
    }
    else
    {
        result = static_cast<int>('d' + 'e' + 'b' + 'u' + 'g');
        std::cout << "\33[31mFAILED\33[0m] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\tnotelog.print(\"Hello, Test!\") == " << ACTUAL_RESULT << ", EXPECTED: " << EXPECTED_RESULT << std::endl;
        std::cout << "[";
    }

    return result;
}

int main(void)
{
    TestFunction tests[] = {
        {test_make_note_log_instantiates_notelog_object, "test_make_note_log_instantiates_notelog_object"},
        {test_make_note_log_enforces_singleton_by_returning_same_notelog_object_for_all_calls, "test_make_note_log_enforces_singleton_by_returning_same_notelog_object_for_all_calls"},
        {test_make_note_log_deletes_singleton_when_nullptr_is_passed_as_parameter, "test_make_note_log_deletes_singleton_when_nullptr_is_passed_as_parameter"},
        {test_notelog_arduino_print_does_not_modify_str_parameter_value_before_passing_to_stream_print, "test_notelog_arduino_print_does_not_modify_buffer_parameter_value_before_passing_to_stream_print"},
        {test_notelog_arduino_print_does_not_modify_stream_print_result_value_before_returning_to_caller, "test_notelog_arduino_print_does_not_modify_stream_print_result_value_before_returning_to_caller"},
    };

    return TestFunction::runTests(tests, (sizeof(tests) / sizeof(TestFunction)));
}
