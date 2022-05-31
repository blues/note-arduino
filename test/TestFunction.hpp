#ifndef TEST_FUNCTION_HPP
#define TEST_FUNCTION_HPP

#include <cstddef>
#include <iostream>

class TestFunction
{
public:
    typedef int (*test_fn)(void);

    TestFunction(
        test_fn fn_,
        const char *name_) : name(name_),
                             fn(fn_)
    {
    }

    int operator()()
    {
        return fn();
    }

    static int runTests(TestFunction *tests_, size_t cnt_)
    {
        int result = 0;

        std::cout << "Running " << cnt_ << " tests..." << std::endl;
        for (size_t i = 0; i < cnt_; ++i)
        {
            std::cout << "[";
            if (0 == (result = tests_[i]()))
            {
                std::cout << "\33[32mpassed\33[0m";
            }
            else
            {
                std::cout << "\33[31mFAILED\33[0m";
            }

            std::cout << "] " << tests_[i].name << std::endl;
            if (result)
            {
                break;
            }
        }

        return result;
    }

    const char *name;

private:
    const test_fn fn;
};

#endif // TEST_FUNCTION_HPP
