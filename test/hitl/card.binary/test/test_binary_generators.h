
#pragma once
#include "NotecardBinary.h"

/**
 * @brief Unity and PlatformIO support generation of multiple distinct binaries.
 * However, I didn't feel it necessary to produce another binary for these tests.
 */
void testsuite_binary_generators();


struct BuildAllSameValue {
    uint8_t buf[128];
    BufferBinaryGenerator chunk;
    RepeatedBinaryGenerator image;

    BuildAllSameValue(size_t length, uint8_t value)
        : chunk(buf, sizeof(buf)), image(chunk, length)
    {
        memset(buf, value, sizeof(buf));
    }

    BinaryGenerator& operator ()()
    {
        return image;
    }
};

struct AllZeros : BuildAllSameValue {
    AllZeros(size_t length) : BuildAllSameValue(length, 0) {}
};

struct AllSevens : BuildAllSameValue {
    AllSevens(size_t length) : BuildAllSameValue(length, 7) {}
};

struct BuildFromBinaryImage {
    BufferBinaryGenerator image;

    BuildFromBinaryImage(size_t length, const BinaryImage& binary)
        : image(binary.data, binary.length) {}

    BinaryGenerator& operator ()()
    {
        return image;
    }
};

struct SmallBinaryImage : BuildFromBinaryImage {
    SmallBinaryImage(size_t size) : BuildFromBinaryImage(size, small_binary) {}
};

struct BuildRandom {
    RandomBinaryGenerator image;
    BuildRandom(size_t size, int seed) : image(size, seed) {}
    BinaryGenerator& operator ()()
    {
        return image;
    }
};


