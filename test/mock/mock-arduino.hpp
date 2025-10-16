#ifndef MOCK_ARDUINO_HPP
#define MOCK_ARDUINO_HPP

#include <stddef.h>
#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#define _NOTE_C_STRINGIZE(x) #x
#define NOTE_C_STRINGIZE(x) _NOTE_C_STRINGIZE(x)

#define SERIAL_RX_BUFFER_SIZE 79

enum PinMode {
    INPUT = 0x19,
    INPUT_PULLUP,
    OUTPUT,
};

enum PinState {
    HIGH = 0x79,
    LOW,
};

void delay(unsigned long ms);

struct Delay_Parameters {
    Delay_Parameters (
        void
    ) :
        invoked(0),
        mock_time(false),
        ms(0)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        mock_time = false;
        ms = 0;
    }
    size_t invoked;
    bool mock_time;
    unsigned long ms;
};

int digitalRead(uint8_t pin);

struct DigitalRead_Parameters {
    DigitalRead_Parameters (
        void
    ) :
        pin(0)
    { }
    void
    reset (
        void
    ) {
        invoked.clear();
        pin = 0;
        default_result.clear();
        result.clear();
    }
    std::map<uint8_t, size_t> invoked;
    uint8_t pin;
    std::map<uint8_t, int> default_result;
    std::map<uint8_t, std::vector<int> > result;
};

void digitalWrite(uint8_t pin, uint8_t val);

struct DigitalWrite_Parameters {
    DigitalWrite_Parameters (
        void
    )
    { }
    void
    reset (
        void
    ) {
        invoked.clear();
        pin_val.clear();
    }
    std::map<uint8_t, size_t> invoked;
    std::map<uint8_t, std::vector<uint8_t> > pin_val;
};

unsigned long millis (void);

struct Millis_Parameters {
    Millis_Parameters (
        void
    ) :
        invoked(0),
        default_result(0),
        result(0)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        default_result = 0;
        result.clear();
    }
    size_t invoked;
    unsigned long default_result;
    std::vector<unsigned long> result;
};

void pinMode(uint8_t pin, uint8_t mode);

struct PinMode_Parameters {
    PinMode_Parameters (
        void
    )
    { }
    void
    reset (
        void
    ) {
        invoked.clear();
        pin_mode.clear();
    }
    std::map<uint8_t, size_t> invoked;
    std::map<uint8_t, std::vector<uint8_t> > pin_mode;
};

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
    operator bool();
    unsigned int available (void);
    void begin(unsigned int baud);
    void end(void);
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
        baud(0),
        invoked(0)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        baud = 0;
    }
    unsigned int baud;
    size_t invoked;
};

struct HardwareSerialEnd_Parameters {
    HardwareSerialEnd_Parameters(
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

struct HardwareSerialOperatorBool_Parameters {
    HardwareSerialOperatorBool_Parameters(
        void
    ) :
        invoked(0),
        result(true)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        result = true;
    }
    size_t invoked;
    bool result;
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

struct SoftwareSerial : public Stream {
    operator bool();
    unsigned int available (void);
    void begin(unsigned int baud);
    void end(void);
    void flush(void);
    char read (void);
    size_t write(uint8_t * buffer, size_t size);
};

struct SoftwareSerialAvailable_Parameters {
    SoftwareSerialAvailable_Parameters(
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

struct SoftwareSerialBegin_Parameters {
    SoftwareSerialBegin_Parameters(
        void
    ) :
        baud(0),
        invoked(0)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        baud = 0;
    }
    unsigned int baud;
    size_t invoked;
};

struct SoftwareSerialEnd_Parameters {
    SoftwareSerialEnd_Parameters(
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

struct SoftwareSerialFlush_Parameters {
    SoftwareSerialFlush_Parameters(
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

struct SoftwareSerialOperatorBool_Parameters {
    SoftwareSerialOperatorBool_Parameters(
        void
    ) :
        invoked(0),
        result(true)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        result = true;
    }
    size_t invoked;
    bool result;
};

struct SoftwareSerialRead_Parameters {
    SoftwareSerialRead_Parameters(
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

struct SoftwareSerialWrite_Parameters {
    SoftwareSerialWrite_Parameters(
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

extern Delay_Parameters delay_Parameters;
extern DigitalRead_Parameters digitalRead_Parameters;
extern DigitalWrite_Parameters digitalWrite_Parameters;
extern Millis_Parameters millis_Parameters;
extern PinMode_Parameters pinMode_Parameters;

extern HardwareSerial Serial;
extern HardwareSerialAvailable_Parameters hardwareSerialAvailable_Parameters;
extern HardwareSerialBegin_Parameters hardwareSerialBegin_Parameters;
extern HardwareSerialEnd_Parameters hardwareSerialEnd_Parameters;
extern HardwareSerialFlush_Parameters hardwareSerialFlush_Parameters;
extern HardwareSerialOperatorBool_Parameters hardwareSerialOperatorBool_Parameters;
extern HardwareSerialRead_Parameters hardwareSerialRead_Parameters;
extern HardwareSerialWrite_Parameters hardwareSerialWrite_Parameters;

extern SoftwareSerial SoftSerial;
extern SoftwareSerialAvailable_Parameters softwareSerialAvailable_Parameters;
extern SoftwareSerialBegin_Parameters softwareSerialBegin_Parameters;
extern SoftwareSerialEnd_Parameters softwareSerialEnd_Parameters;
extern SoftwareSerialFlush_Parameters softwareSerialFlush_Parameters;
extern SoftwareSerialOperatorBool_Parameters softwareSerialOperatorBool_Parameters;
extern SoftwareSerialRead_Parameters softwareSerialRead_Parameters;
extern SoftwareSerialWrite_Parameters softwareSerialWrite_Parameters;

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
