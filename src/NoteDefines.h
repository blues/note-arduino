#ifndef NOTE_DEFINES_H
#define NOTE_DEFINES_H

// Define the version of the `note-arduino` library
#define NOTE_ARDUINO_VERSION_MAJOR 1
#define NOTE_ARDUINO_VERSION_MINOR 7
#define NOTE_ARDUINO_VERSION_PATCH 1

#define NOTE_ARDUINO_VERSION NOTE_C_STRINGIZE(NOTE_ARDUINO_VERSION_MAJOR) "." NOTE_C_STRINGIZE(NOTE_ARDUINO_VERSION_MINOR) "." NOTE_C_STRINGIZE(NOTE_ARDUINO_VERSION_PATCH)

// Unified attribute for marking functions as deprecated
#if defined(__GNUC__) | defined(__clang__)
    #define NOTE_ARDUINO_DEPRECATED __attribute__((__deprecated__))
#elif defined(_MSC_VER)
    #define NOTE_ARDUINO_DEPRECATED __declspec(deprecated)
#else
    #define NOTE_ARDUINO_DEPRECATED
    #define NOTE_ARDUINO_NO_DEPRECATED_ATTR
#endif // __GNUC__ || __clang__

// Switches for enabling/disabling features
#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR) || defined(ARDUINO_ARCH_STM32) || defined(ARDUINO_ARCH_NRF52) || (defined(ARDUINO_ARCH_RP2040) && !defined(ARDUINO_ARCH_MBED))
    #define NOTE_ARDUINO_SOFTWARE_SERIAL_SUPPORT
#endif

#endif // NOTE_DEFINES_H
