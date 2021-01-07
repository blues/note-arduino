#ifndef MOCK_ARDUINO_HPP
#define MOCK_ARDUINO_HPP

#include <cstddef>

void delay (unsigned int);
size_t millis (void);

struct HardwareSerial {
    unsigned int available (void);
    void begin(unsigned int baud);
    void flush(void);
    char read (void);
    void write(unsigned char * text, unsigned int len);
};

struct HardwareSerialBegin_Parameters {
    HardwareSerialBegin_Parameters(
        void
    ) :
        baud(0)
    { }
    unsigned int baud;
};

struct Stream {
    long unsigned int print(const char *);
};

struct TwoWire {
    void begin(void);
    void beginTransmission(int);
    int endTransmission(void);
    unsigned char read(void);
    int requestFrom(int dev_addr, unsigned int read_len);
    void write(unsigned char c);
    long unsigned int write(unsigned char * msg, long unsigned int len);
};

extern HardwareSerial Serial;
extern HardwareSerialBegin_Parameters hardwareSerialBegin_Parameters;
extern TwoWire Wire;
extern Stream dbgserial;

#endif
