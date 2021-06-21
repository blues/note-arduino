#include "NoteLog_Arduino.hpp"
#include "TestFunction.hpp"
#include "mock/mock-arduino.hpp"
#include "mock/mock-parameters.hpp"

#include <string.h>

// Compile command: g++ -Wall -Wextra -Wpedantic mock/mock-arduino.cpp ../src/NoteLog_Arduino.cpp NoteLog_Arduino.test.cpp -std=c++11 -I. -I../src -DMOCK && ./a.out || echo "Tests Result: $?"

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
        std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
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
        std::cout << "FAILED] " << __FILE__ << ":" << __LINE__ << std::endl;
        std::cout << "\tnotelog.print(\"Hello, Test!\") == " << ACTUAL_RESULT << ", EXPECTED: " << EXPECTED_RESULT << std::endl;
        std::cout << "[";
    }

    return result;
}

int main(void)
{
    TestFunction tests[] = {
        {test_notelog_arduino_print_does_not_modify_str_parameter_value_before_passing_to_stream_print, "test_notelog_arduino_print_does_not_modify_buffer_parameter_value_before_passing_to_stream_print"},
        {test_notelog_arduino_print_does_not_modify_stream_print_result_value_before_returning_to_caller, "test_notelog_arduino_print_does_not_modify_stream_print_result_value_before_returning_to_caller"},
    };

    return TestFunction::runTests(tests, (sizeof(tests) / sizeof(TestFunction)));
}
