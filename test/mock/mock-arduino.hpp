#ifndef MOCK_ARDUINO_HPP
#define MOCK_ARDUINO_HPP

#include <stddef.h>
#include <stdint.h>

#include <string>
#include <vector>

void delay (unsigned int);
size_t millis (void);

struct Stream {
    virtual size_t print(const char * str);
};

struct StreamPrint_Parameters {
    StreamPrint_Parameters(
        void
    ) :
        invoked(0),
        str(nullptr),
        result(0)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        str = nullptr;
        str_cache.clear();
        result = 0;
    }
    size_t invoked;
    const char * str;
    std::string str_cache;
    size_t result;
};

struct HardwareSerial : public Stream {
    unsigned int available (void);
    void begin(unsigned int baud);
    void flush(void);
    char read (void);
    size_t write(uint8_t * buffer, size_t size);
};

struct HardwareSerialAvailable_Parameters {
    HardwareSerialAvailable_Parameters(
        void
    ) :
        invoked(0),
        result(0)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        result = 0;
    }
    size_t invoked;
    size_t result;
};

struct HardwareSerialBegin_Parameters {
    HardwareSerialBegin_Parameters(
        void
    ) :
        baud(0)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        baud = 0;
    }
    size_t invoked;
    unsigned int baud;
};

struct HardwareSerialFlush_Parameters {
    HardwareSerialFlush_Parameters(
        void
    ) :
        invoked(0)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
    }
    size_t invoked;
};


struct HardwareSerialRead_Parameters {
    HardwareSerialRead_Parameters(
        void
    ) :
        invoked(0),
        result(0)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        result = 0;
    }
    size_t invoked;
    char result;
};

struct HardwareSerialWrite_Parameters {
    HardwareSerialWrite_Parameters(
        void
    ) :
        invoked(0),
        buffer(nullptr),
        size(0),
        result(0)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        buffer = nullptr;
        buffer_cache.clear();
        size = 0;
        result = 0;
    }
    size_t invoked;
    uint8_t * buffer;
    std::string buffer_cache;
    size_t size;
    size_t result;
};

struct TwoWire {
    void begin(void);
    void beginTransmission(uint8_t address);
    void end(void);
    uint8_t endTransmission(void);
    int read(void);
    uint8_t requestFrom(int address, int quantity);
    size_t write(uint8_t c);
    size_t write(uint8_t * buffer, size_t size);
};

struct TwoWireBegin_Parameters {
    TwoWireBegin_Parameters(
        void
    ) :
        invoked(0)
    { }
    void reset (
        void
    ) {
        invoked = 0;
    }
    size_t invoked;
};

struct TwoWireBeginTransmission_Parameters {
    TwoWireBeginTransmission_Parameters(
        void
    ) :
        invoked(0),
        address(0)
    { }
    void reset (
        void
    ) {
        invoked = 0;
        address = 0;
    }
    size_t invoked;
    uint8_t address;
};

struct TwoWireEnd_Parameters {
    TwoWireEnd_Parameters(
        void
    ) :
        invoked(0)
    { }
    void reset (
        void
    ) {
        invoked = 0;
    }
    size_t invoked;
};

struct TwoWireEndTransmission_Parameters {
    TwoWireEndTransmission_Parameters(
        void
    ) :
        invoked(0)
    { }
    void reset (
        void
    ) {
        invoked = 0;
        results.clear();
    }
    size_t invoked;
    std::vector<uint8_t> results;
};

struct TwoWireRead_Parameters {
    TwoWireRead_Parameters(
        void
    ) :
        invoked(0)
    { }
    void reset (
        void
    ) {
        invoked = 0;
        results.clear();
    }
    size_t invoked;
    std::vector<int> results;
};

struct TwoWireRequestFrom_Parameters {
    TwoWireRequestFrom_Parameters(
        void
    ) :
        invoked(0),
        address(0),
        quantity(0),
        result(0)
    { }
    void reset (
        void
    ) {
        invoked = 0;
        address = 0;
        quantity = 0;
        result = 0;
    }
    size_t invoked;
    int address;
    int quantity;
    uint8_t result;
};

struct TwoWireWriteByte_Parameters {
    TwoWireWriteByte_Parameters(
        void
    ) :
        invoked(0),
        c(0),
        result(0)
    { }
    void reset (
        void
    ) {
        invoked = 0;
        c = 0;
        result = 0;
        write_buffer.clear();
    }

    size_t invoked;
    uint8_t c;
    size_t result;
    std::vector<uint8_t> write_buffer;
};

struct TwoWireWriteBuffer_Parameters {
    TwoWireWriteBuffer_Parameters(
        void
    ) :
        invoked(0),
        buffer(nullptr),
        size(0),
        result(0)
    { }
    void reset (
        void
    ) {
        invoked = 0;
        buffer = nullptr;
        size = 0;
        result = 0;
    }

    size_t invoked;
    uint8_t * buffer;
    uint16_t size;
    size_t result;
};

extern HardwareSerial Serial;
extern HardwareSerialAvailable_Parameters hardwareSerialAvailable_Parameters;
extern HardwareSerialBegin_Parameters hardwareSerialBegin_Parameters;
extern HardwareSerialFlush_Parameters hardwareSerialFlush_Parameters;
extern HardwareSerialRead_Parameters hardwareSerialRead_Parameters;
extern HardwareSerialWrite_Parameters hardwareSerialWrite_Parameters;
extern StreamPrint_Parameters streamPrint_Parameters;
extern TwoWire Wire;
extern TwoWireBegin_Parameters twoWireBegin_Parameters;
extern TwoWireBeginTransmission_Parameters twoWireBeginTransmission_Parameters;
extern TwoWireEnd_Parameters twoWireEnd_Parameters;
extern TwoWireEndTransmission_Parameters twoWireEndTransmission_Parameters;
extern TwoWireRead_Parameters twoWireRead_Parameters;
extern TwoWireRequestFrom_Parameters twoWireRequestFrom_Parameters;
extern TwoWireWriteByte_Parameters twoWireWriteByte_Parameters;
extern TwoWireWriteBuffer_Parameters twoWireWriteBuffer_Parameters;

#endif // MOCK_ARDUINO_HPP
