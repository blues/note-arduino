
// This also needs to be added to build_flags in platformio.ini
#ifndef UNITY_INCLUDE_PRINT_FORMATTED
#define UNITY_INCLUDE_PRINT_FORMATTED
#endif

#include "unity.h"

// Use this macro to declare the test functions as static, so the compiler generates a warning/error
// If we forget to add it to the list of tests.
#define TEST(x) static void x()

typedef void (*test_suite_hook_fn)(void);
typedef bool (*test_suite_filter_fn)(const char* name);

void test_suite_hooks(test_suite_hook_fn setup, test_suite_hook_fn teardown, test_suite_filter_fn filter);

// Update the current filename so reports point to the correct source location.
#define TEST_SUITE(name) \
    Unity.TestFile = __FILE__; \
    extern void testsuite_ ## name ## _setUp(void); \
    extern void testsuite_ ## name ## _tearDown(void); \
    test_suite_hooks(testsuite_ ## name ## _setUp, \
        testsuite_ ## name ## _tearDown, \
        testsuite_ ## name ## _filter);

extern bool filterTest(const char* name);

#define RUN_FILTER(func) \
    if (filterTest(#func)) { \
        RUN_TEST(func); \
    }


