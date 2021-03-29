#ifndef MOCK_ARDUINO_HPP
#define MOCK_ARDUINO_HPP

#include <stddef.h>
#include <stdint.h>

#define WIRE_HAS_END 1

void delay (unsigned int);
size_t millis (void);

struct HardwareSerial {
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
    size_t invoked;
    size_t result;
};

struct HardwareSerialBegin_Parameters {
    HardwareSerialBegin_Parameters(
        void
    ) :
        baud(0)
    { }
    unsigned int baud;
};

struct HardwareSerialFlush_Parameters {
    HardwareSerialFlush_Parameters(
        void
    ) :
        invoked(0)
    { }
    size_t invoked;
};


struct HardwareSerialRead_Parameters {
    HardwareSerialRead_Parameters(
        void
    ) :
        invoked(0),
        result('\0')
    { }
    size_t invoked;
    char result;
};

struct HardwareSerialWrite_Parameters {
    HardwareSerialWrite_Parameters(
        void
    ) :
        buffer(nullptr),
        size(0),
        result(0)
    { }
    uint8_t * buffer;
    size_t size;
    size_t result;
};

struct Stream {
    long unsigned int print(const char *);
};

struct TwoWire {
    void begin(void);
    void beginTransmission(int);
    void end(void);
    int endTransmission(void);
    unsigned char read(void);
    int requestFrom(int dev_addr, unsigned int read_len);
    void write(unsigned char c);
    long unsigned int write(unsigned char * msg, long unsigned int len);
};

struct TwoWireBegin_Parameters {
    TwoWireBegin_Parameters(
        void
    ) :
        called(false)
    { }
    bool called;
};

extern HardwareSerial Serial;
extern HardwareSerialAvailable_Parameters hardwareSerialAvailable_Parameters;
extern HardwareSerialBegin_Parameters hardwareSerialBegin_Parameters;
extern HardwareSerialFlush_Parameters hardwareSerialFlush_Parameters;
extern HardwareSerialRead_Parameters hardwareSerialRead_Parameters;
extern HardwareSerialWrite_Parameters hardwareSerialWrite_Parameters;
extern Stream dbgserial;
extern TwoWire Wire;
extern TwoWireBegin_Parameters twoWireBegin_Parameters;

#endif
