
#include "NotecardBinary.h"
#include <Arduino.h>
#include "test_binary_generators.h"

#include "test_unity_util.h"

bool testsuite_binary_generators_filter(const char* name)
{
    return true;
}

void testsuite_binary_generators_setUp()
{
}

void testsuite_binary_generators_tearDown()
{
}

typedef std::function<void(BinaryGenerator& image, uint8_t* buf, size_t bufSize, size_t chunkSize, size_t total_read)> content_check_fn;

void check_binary(BinaryGenerator& image, size_t total_size, size_t bufSize, content_check_fn contentCheck)
{
    TEST_ASSERT_EQUAL(total_size, image.length());
    TEST_ASSERT_EQUAL(0, image.offset());

    uint8_t buf[bufSize];
    size_t total_read = 0;
    while (image.remaining()) {
        TEST_ASSERT_FALSE(image.isEOF());
        TEST_ASSERT_EQUAL(total_read, image.offset());
        TEST_ASSERT_EQUAL(total_size-total_read, image.remaining());
        size_t chunk = image.read(buf, bufSize);
        total_read += chunk;
        contentCheck(image, buf, bufSize, chunk, total_read);
    }
    TEST_ASSERT_TRUE(image.isEOF());
}

content_check_fn check_content_fixed_value(uint8_t value)
{
    return [=](BinaryGenerator& image, uint8_t* buf, size_t bufSize, size_t chunkSize, size_t totalRead) {
        for (size_t i=0; i<chunkSize; i++) {
            if (buf[i]!=value) {
                TEST_PRINTF("incorrect value at offset %d: %d != %d", totalRead, value, buf[i]);
                TEST_FAIL_MESSAGE("BinaryGenerator validation failed.");
            }
        }
    };
}

TEST(test_fixed_value_generator)
{
    uint8_t pattern[128];
    memset(pattern, 0x7, sizeof(pattern));
    BufferBinaryGenerator chunk(pattern, sizeof(pattern));
    size_t binary_size = 20*1024;
    RepeatedBinaryGenerator image(chunk, binary_size);

    check_binary(image, binary_size, 95, check_content_fixed_value(0x7));
}

TEST(test_random_generator)
{
    uint32_t seed = 1234;
    RandomBinaryGenerator image(LARGE_SIZE, seed);
    uint8_t buf[20];
    image.read(buf, sizeof(buf));

    uint8_t buf2[20];
    image.reset();
    image.read(buf2, sizeof(buf2));

    TEST_ASSERT_TRUE(!memcmp(buf, buf2, sizeof(buf)));
    uint8_t v1 = buf[0];
    uint8_t v2 = buf[1];
    uint8_t v3 = buf[2];
    TEST_ASSERT_EQUAL(136, v1);
    TEST_ASSERT_EQUAL(40, v2);
    TEST_ASSERT_EQUAL(142, v3);
}

void testsuite_binary_generators()
{
    TEST_SUITE(binary_generators);
    RUN_TEST(test_fixed_value_generator);
    RUN_TEST(test_random_generator);
}


