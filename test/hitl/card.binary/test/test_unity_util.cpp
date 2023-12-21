#include "test_unity_util.h"
#include <string.h>

test_suite_hook_fn test_suite_setup, test_suite_teardown;
test_suite_filter_fn test_suite_filter;

void test_suite_hooks(test_suite_hook_fn setup, test_suite_hook_fn teardown, test_suite_filter_fn filter)
{
    test_suite_setup = setup;
    test_suite_teardown = teardown;
    test_suite_filter = filter;
}

void setUp()
{
    if (test_suite_setup) {
        test_suite_setup();
    }
}

void tearDown()
{
    if (test_suite_teardown) {
        test_suite_teardown();
    }
}

bool filterTest(const char* name)
{
    if (test_suite_filter) {
        return test_suite_filter(name);
    }
    return true;
}