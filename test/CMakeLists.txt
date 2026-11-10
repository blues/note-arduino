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
set(TESTED_UNITS "")

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
    list(APPEND TESTED_UNITS ${TEST_UNIT})

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

    option(NOTE_ARDUINO_COVERAGE_EXCEPTION_BRANCHES
           "Count compiler-made exception branches in the coverage numbers." OFF)

    get_filename_component(REPO_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/.. REALPATH)
    set(COVERAGE_DIR ${REPO_ROOT}/coverage)

    # A unit with no test would not appear in the report, so its lines would
    # not count as missed. Compile each such unit (never run it): lcov reads
    # the zero counters of its .gcno file and the unit counts as 0% covered.
    set(UNTESTED_TARGETS "")
    file(GLOB NOTE_ARDUINO_UNITS CONFIGURE_DEPENDS ${NOTE_ARDUINO_SRC_DIR}/*.cpp)
    foreach(UNIT_SOURCE ${NOTE_ARDUINO_UNITS})
        get_filename_component(UNIT ${UNIT_SOURCE} NAME_WE)
        if(NOT UNIT IN_LIST TESTED_UNITS)
            message(STATUS "Coverage: ${UNIT} has no test; counting it as uncovered.")
            add_library(${UNIT}_untested OBJECT ${UNIT_SOURCE})
            target_include_directories(${UNIT}_untested
                PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
                PRIVATE ${NOTE_ARDUINO_SRC_DIR}
            )
            target_compile_definitions(${UNIT}_untested PRIVATE NOTE_MOCK)
            target_compile_options(${UNIT}_untested
                PRIVATE -Wall -Wextra -Werror -Wpedantic -Wno-deprecated-declarations
                PRIVATE -O0 -ggdb --coverage
            )
            list(APPEND UNTESTED_TARGETS ${UNIT}_untested)
        endif()
    endforeach()

    # lcov 2 names the option "branch_coverage". lcov 1 names it
    # "lcov_branch_coverage" and ignores the other name (it then writes no
    # branch data at all).
    execute_process(COMMAND ${LCOV} --version OUTPUT_VARIABLE LCOV_VERSION_TEXT)
    if(LCOV_VERSION_TEXT MATCHES "version ([0-9]+)" AND CMAKE_MATCH_1 LESS 2)
        set(LCOV_BRANCH_RC lcov_branch_coverage=1)
        message(WARNING "lcov ${CMAKE_MATCH_1} found. The coverage baseline is made with lcov 2. "
                        "lcov 1 counts exception branches that lcov 2 skips, so the numbers differ.")
    else()
        set(LCOV_BRANCH_RC branch_coverage=1)
    endif()

    # Measure only src/ (not note-c, tests, fakes or Catch2).
    set(LCOV_SCOPE
        --no-external
        --base-directory ${REPO_ROOT}
        --include "${REPO_ROOT}/src/*"
        --exclude "${REPO_ROOT}/src/note-c/*"
        --rc ${LCOV_BRANCH_RC}
    )
    # Compiler-made exception branches are not code paths a test can choose.
    if(NOT NOTE_ARDUINO_COVERAGE_EXCEPTION_BRANCHES)
        list(APPEND LCOV_SCOPE --rc no_exception_branch=1)
    endif()
    # lcov 2 reports an exclude pattern that matches nothing as an error.
    if(LCOV_VERSION_TEXT MATCHES "version ([0-9]+)" AND CMAKE_MATCH_1 GREATER_EQUAL 2)
        list(APPEND LCOV_SCOPE --ignore-errors unused,unused)
    endif()

    add_custom_target(
        coverage
        COMMAND mkdir -p ${COVERAGE_DIR}
        # Zero out the coverage counters from any previous runs.
        COMMAND lcov --zerocounters --directory ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    add_custom_command(
        TARGET coverage POST_BUILD
        # 1. Zero-count baseline of every compiled unit (tested or not).
        COMMAND lcov --capture --initial ${LCOV_SCOPE}
                --directory ${CMAKE_CURRENT_BINARY_DIR}
                --output-file ${COVERAGE_DIR}/lcov-initial.info
        # 2. Counts from the test run.
        COMMAND lcov --capture ${LCOV_SCOPE}
                --directory ${CMAKE_CURRENT_BINARY_DIR}
                --output-file ${COVERAGE_DIR}/lcov-tests.info
        # 3. Merge them. The tracefile is what Coveralls reads.
        COMMAND lcov --rc ${LCOV_BRANCH_RC}
                --add-tracefile ${COVERAGE_DIR}/lcov-initial.info
                --add-tracefile ${COVERAGE_DIR}/lcov-tests.info
                --output-file ${COVERAGE_DIR}/lcov.info
        COMMAND lcov --summary --rc ${LCOV_BRANCH_RC} ${COVERAGE_DIR}/lcov.info
        WORKING_DIRECTORY ${REPO_ROOT}
        VERBATIM
    )
    add_dependencies(coverage ${TEST_TARGETS} ${UNTESTED_TARGETS})
endif(NOTE_ARDUINO_COVERAGE)
