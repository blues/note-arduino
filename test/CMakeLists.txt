cmake_minimum_required(VERSION 3.20)

project(NoteArduinoUnitTests LANGUAGES C CXX)

option(NOTE_ARDUINO_COVERAGE "Compile the unit tests for coverage reporting." OFF)
option(NOTE_ARDUINO_MEM_CHECK "Run the unit tests with Valgrind." OFF)

include(FetchContent)

# Catch2 v3 requires C++14.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "Downloading Catch2...")
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.3.0
)
FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include(Catch)

if(NOTE_ARDUINO_MEM_CHECK)
    find_program(MEMORYCHECK_COMMAND valgrind REQUIRED)
    # A leak or memory error fails the run.
    set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --error-exitcode=66")
    include(CTest)
endif()
enable_testing()

set(NOTE_ARDUINO_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src)
set(TEST_TARGETS "")

# add_test(<unit> [<extra compile definition>...])
# Builds test/src/<unit>_test.cpp together with src/<unit>.cpp and the FFF
# fakes into one executable per unit, then registers each Catch2 TEST_CASE
# with CTest.
macro(add_test TEST_UNIT)
    set(TEST_DEFINES ${ARGN})
    set(TEST_NAME ${TEST_UNIT}_test)
    set(TEST_SUFFIX "")
    foreach(TEST_DEFINE ${TEST_DEFINES})
        string(APPEND TEST_NAME "_${TEST_DEFINE}")
        string(APPEND TEST_SUFFIX " [${TEST_DEFINE}]")
    endforeach()
    set(TEST_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${TEST_UNIT}_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/arduino_fakes.cpp
        ${NOTE_ARDUINO_SRC_DIR}/${TEST_UNIT}.cpp
    )
    if(${TEST_UNIT} STREQUAL "Notecard")
        list(APPEND TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/note_iface_fakes.cpp)
    endif()
    add_executable(${TEST_NAME} ${TEST_SOURCES})
    target_include_directories(${TEST_NAME}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE ${NOTE_ARDUINO_SRC_DIR}
    )
    target_compile_definitions(${TEST_NAME} PRIVATE NOTE_MOCK ${TEST_DEFINES})
    target_link_libraries(${TEST_NAME} PRIVATE Catch2::Catch2WithMain)
    target_compile_options(${TEST_NAME}
        PRIVATE -Wall
        PRIVATE -Wextra
        PRIVATE -Werror
        PRIVATE -Wpedantic
        PRIVATE -Wno-deprecated-declarations
        PRIVATE -O0
        PRIVATE -ggdb
    )
    if(NOTE_ARDUINO_COVERAGE)
        target_compile_options(${TEST_NAME} PRIVATE --coverage)
        target_link_options(${TEST_NAME} PRIVATE --coverage)
    endif()

    list(APPEND TEST_TARGETS ${TEST_NAME})

    catch_discover_tests(${TEST_NAME} TEST_SUFFIX "${TEST_SUFFIX}")
endmacro(add_test)

add_test(Notecard)
add_test(NoteI2c_Arduino)
add_test(NoteI2c_Arduino WIRE_HAS_END)
add_test(NoteLog_Arduino)
add_test(NoteSerial_Arduino)
add_test(NoteTxn_Arduino)

if(NOTE_ARDUINO_COVERAGE)
    find_program(LCOV lcov REQUIRED)
    message(STATUS "Found lcov: ${LCOV}")

    set(COVERAGE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../coverage)
    add_custom_target(
        coverage
        COMMAND mkdir -p ${COVERAGE_DIR}
        # Zero out the coverage counters from any previous runs.
        COMMAND lcov --zerocounters --directory ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    # Only src/ is measured: the tests, fakes and Catch2 are excluded.
    add_custom_command(
        TARGET coverage POST_BUILD
        COMMAND lcov --capture --no-external
                --base-directory ${CMAKE_CURRENT_SOURCE_DIR}/..
                --directory ${CMAKE_CURRENT_BINARY_DIR}
                --rc branch_coverage=1
                --exclude */test/* --exclude */build/_deps/*
                --output-file ${COVERAGE_DIR}/lcov.info
        COMMAND lcov --summary --rc branch_coverage=1 ${COVERAGE_DIR}/lcov.info
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
        VERBATIM
    )
    add_dependencies(coverage ${TEST_TARGETS})
endif(NOTE_ARDUINO_COVERAGE)
