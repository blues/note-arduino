cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0095 NEW)

if ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed.
    Please create a build directory and use `cmake ..` inside it.
    NOTE: cmake will now create CMakeCache.txt and CMakeFiles/*.
          You must delete them, or cmake will refuse to work.")
endif()

project(note_c)

# Automatically ignore CMake build directory.
if(NOT EXISTS ${PROJECT_BINARY_DIR}/.gitignore)
    file(WRITE ${PROJECT_BINARY_DIR}/.gitignore "*")
endif()

option(NOTE_C_BUILD_DOCS "Build docs." OFF)
option(NOTE_C_BUILD_TESTS "Build tests." ON)
option(NOTE_C_COVERAGE "Compile for test NOTE_C_COVERAGE reporting." OFF)
option(NOTE_C_LOW_MEM "Build the library tailored for low memory usage." OFF)
option(NOTE_C_MEM_CHECK "Run tests with Valgrind." OFF)
option(NOTE_C_NO_LIBC "Build the library without linking against libc, generating errors for any undefined symbols." OFF)
option(NOTE_C_SHOW_MALLOC "Build the library with flags required to log memory usage." OFF)
option(NOTE_C_SINGLE_PRECISION "Use single precision for JSON floating point numbers." OFF)

set(NOTE_C_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
add_library(note_c SHARED)
target_sources(
    note_c
    PRIVATE
        ${NOTE_C_SRC_DIR}/n_atof.c
        ${NOTE_C_SRC_DIR}/n_b64.c
        ${NOTE_C_SRC_DIR}/n_cjson.c
        ${NOTE_C_SRC_DIR}/n_cjson_helpers.c
        ${NOTE_C_SRC_DIR}/n_cobs.c
        ${NOTE_C_SRC_DIR}/n_const.c
        ${NOTE_C_SRC_DIR}/n_ftoa.c
        ${NOTE_C_SRC_DIR}/n_helpers.c
        ${NOTE_C_SRC_DIR}/n_hooks.c
        ${NOTE_C_SRC_DIR}/n_i2c.c
        ${NOTE_C_SRC_DIR}/n_md5.c
        ${NOTE_C_SRC_DIR}/n_printf.c
        ${NOTE_C_SRC_DIR}/n_request.c
        ${NOTE_C_SRC_DIR}/n_serial.c
        ${NOTE_C_SRC_DIR}/n_str.c
)
target_compile_options(
    note_c
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -Og
        -ggdb
    PUBLIC
        -m32
        -mfpmath=sse
        -msse2
)
target_include_directories(
    note_c
    PUBLIC ${NOTE_C_SRC_DIR}
)
target_link_directories(
    note_c
    PUBLIC
        /lib32
        /usr/lib32
        /usr/lib/gcc/x86_64-linux-gnu/12/32
)
target_link_options(
    note_c
    PUBLIC
        -m32
        -Wl,-melf_i386
)

if(NOTE_C_LOW_MEM)
    target_compile_definitions(
        note_c
        PUBLIC
            NOTE_C_LOW_MEM
    )
else()
    # This file is empty if NOTE_C_LOW_MEM is defined, which leads to a warning
    # about an empty translation unit, so we only add it to the build if
    # NOTE_C_LOW_MEM is false.
    target_sources(
        note_c
        PRIVATE
            ${NOTE_C_SRC_DIR}/n_ua.c
    )
endif()

if(NOTE_C_NO_LIBC)
    target_link_options(
        note_c
        PRIVATE
            -nostdlib
            -nodefaultlibs
            LINKER:--no-undefined
    )
endif()

if(NOTE_C_SHOW_MALLOC)
    target_compile_definitions(
        note_c
        PUBLIC
            NOTE_C_SHOW_MALLOC
    )
endif()

if(NOTE_C_SINGLE_PRECISION)
    target_compile_definitions(
        note_c
        PUBLIC
            NOTE_C_SINGLE_PRECISION
    )
endif()

if(NOTE_C_BUILD_TESTS)
    # Including CTest here rather than in test/CMakeLists.txt allows us to run
    # ctest from the root build directory (e.g. build/ instead of build/test/).
    # We also need to set MEMORYCHECK_COMMAND_OPTIONS before including this.
    # See: https://stackoverflow.com/a/60741757
    if(NOTE_C_MEM_CHECK)
        # Go ahead and make sure we can find valgrind while we're here.
        find_program(VALGRIND valgrind REQUIRED)
        message(STATUS "Found valgrind: ${VALGRIND}")
        set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --error-exitcode=1")
    endif(NOTE_C_MEM_CHECK)
    include(CTest)

    target_compile_definitions(
        note_c
        PUBLIC
            NOTE_C_TEST
    )
    target_include_directories(
        note_c
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/test/include
    )

    add_subdirectory(test)
endif(NOTE_C_BUILD_TESTS)

if(NOTE_C_BUILD_DOCS)
    add_subdirectory(docs)
endif(NOTE_C_BUILD_DOCS)
