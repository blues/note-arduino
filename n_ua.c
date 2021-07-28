/*!
 * @file n_ua.c
 *
 * Written by Ray Ozzie and Blues Inc. team.
 *
 * Copyright (c) 2019 Blues Inc. MIT License. Use of this source code is
 * governed by licenses granted by the copyright holder including that found in
 * the
 * <a href="https://github.com/blues/note-c/blob/master/LICENSE">LICENSE</a>
 * file.
 *
 */

#ifndef NOTE_LOWMEM

#include "n_lib.h"

// Override-able statics
static char *n_agent = "note-c";
static char *n_os_name = NULL;
static char *n_os_platform = NULL;
static char *n_os_family = NULL;
static char *n_os_version = NULL;
static int n_cpu_mem = 0;
static int n_cpu_mhz = 0;
static int n_cpu_cores = 0;
static char *n_cpu_vendor = NULL;
static char *n_cpu_name = NULL;

/**************************************************************************/
/*!
    @brief  Method to return user agent object
  @returns a `J` cJSON object with the user agent object.
*/
/**************************************************************************/
J *noteUserAgent()
{

    J *ua = JCreateObject();
    if (ua == NULL) {
        return ua;
    }

#if defined(__cplusplus)
#define PLUS " c++"
#else
#define PLUS ""
#endif

#if defined(__ICCARM__)
    char *compiler = "iar arm" PLUS " " __VER__;
#elif defined(__IAR_SYSTEMS_ICC__)
    char *compiler = "iar" PLUS " " __VER__;
#elif defined(__clang__)
    char *compiler = "clang" PLUS " " __VERSION__;
#elif defined(__GNUC__)
    char *compiler = "gcc" PLUS " " __VERSION__;
#elif defined(__ATOLLIC__) && defined(__GNUC__)
    char *compiler = "atollic gcc" PLUS " " __VERSION__;
#elif defined(_MSC_FULL_VER)
    char *compiler = "msc" PLUS " " _MSC_FULL_VER;
#elif defined(__STDC_VERSION___)
    char *compiler = "STDC" PLUS " " __STDC_VERSION__;
#else
    char *compiler = "unknown" PLUS " " __VERSION__
#endif

#if defined(ARDUINO_ARCH_ARC32)
    char *arch = "arc32";
#elif defined(ARDUINO_ARCH_AVR)
    char *arch = "avr";
#elif defined(ARDUINO_ARCH_ESP32)
    char *arch = "esp32";
#elif defined(ARDUINO_ARCH_ESP8266)
    char *arch = "esp8266";
#elif defined(ARDUINO_ARCH_MBED)
    char *arch = "mbed";
#elif defined(ARDUINO_ARCH_MEGAAVR)
    char *arch = "megaavr";
#elif defined(ARDUINO_ARCH_NRF52840)
    char *arch = "nrf52840";
#elif defined(ARDUINO_ARCH_NRF52)
    char *arch = "nrf52";
#elif defined(ARDUINO_ARCH_NRF51)
    char *arch = "nrf51";
#elif defined(ARDUINO_ARCH_PIC32)
    char *arch = "pic32";
#elif defined(ARDUINO_ARCH_SAMD)
    char *arch = "samd";
#elif defined(ARDUINO_ARCH_SAM)
    char *arch = "sam";
#elif defined(ARDUINO_ARCH_SPRESENSE)
    char *arch = "spresence";
#elif defined(ARDUINO_ARCH_STM32F0)
    char *arch = "stm32f0";
#elif defined(ARDUINO_ARCH_STM32F1)
    char *arch = "stm32f1";
#elif defined(ARDUINO_ARCH_STM32F4)
    char *arch = "stm32f4";
#elif defined(ARDUINO_ARCH_STM32G0)
    char *arch = "stm32g0";
#elif defined(ARDUINO_ARCH_STM32L4)
    char *arch = "stm32f4";
#elif defined(ARDUINO_ARCH_STM32U5)
    char *arch = "stm32u5";
#elif defined(ARDUINO_ARCH_STM32)
    char *arch = "stm32";
#else
    char *arch = "";
#endif

    JAddStringToObject(ua, "agent", n_agent);
    JAddStringToObject(ua, "compiler", compiler);
    JAddStringToObject(ua, "req_interface", NoteActiveInterface());

    if (n_cpu_mem != 0) {
        JAddNumberToObject(ua, "cpu_mem", n_cpu_mem);
    }
    if (n_cpu_mhz != 0) {
        JAddNumberToObject(ua, "cpu_mhz", n_cpu_mhz);
    }
    if (n_cpu_cores != 0) {
        JAddNumberToObject(ua, "cpu_cores", n_cpu_cores);
    }
    if (n_cpu_vendor != NULL) {
        JAddStringToObject(ua, "cpu_vendor", n_cpu_vendor);
    }
    if (n_cpu_name != NULL) {
        JAddStringToObject(ua, "cpu_name", n_cpu_name);
    }

    if (n_os_name != NULL) {
        JAddStringToObject(ua, "os_name", n_os_name);
    }
    if (n_os_platform != NULL) {
        JAddStringToObject(ua, "os_platform", n_os_platform);
    }
    if (n_os_family != NULL) {
        JAddStringToObject(ua, "os_family", n_os_family);
    }
    if (n_os_version != NULL) {
        JAddStringToObject(ua, "os_version", n_os_version);
    }

    return ua;

}

/**************************************************************************/
/*!
    @brief  Set key UA fields from a higher level library context
*/
/**************************************************************************/
void NoteSetUserAgent(char *agent)
{
    n_agent = agent;
}

/**************************************************************************/
/*!
    @brief  Set key UA fields from a higher level library context
*/
/**************************************************************************/
void NoteSetUserAgentOS(char *os_name, char *os_platform, char *os_family, char *os_version)
{
    n_os_name = os_name;
    n_os_platform = os_platform;
    n_os_family = os_family;
    n_os_version = os_version;
}

/**************************************************************************/
/*!
    @brief  Set key UA fields from a higher level library context
*/
/**************************************************************************/
void NoteSetUserAgentCPU(int cpu_mem, int cpu_mhz, int cpu_cores, char *cpu_vendor, char *cpu_name)
{
    n_cpu_mem = cpu_mem;
    n_cpu_mhz = cpu_mhz;
    n_cpu_cores = cpu_cores;
    n_cpu_vendor = cpu_vendor;
    n_cpu_name = cpu_name;
}

#endif
