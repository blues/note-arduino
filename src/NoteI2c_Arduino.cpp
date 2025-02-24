#include "NoteI2c_Arduino.hpp"

#if defined(NOTE_C_LOW_MEM)
static const char *i2cerr = "i2c {io}";
#endif

// Singleton instance of the NoteI2c_Arduino class
namespace instance {
    inline NoteI2c* & note_i2c (void) {
        static NoteI2c* note_i2c = nullptr;
        return note_i2c;
    }
};

NoteI2c *
make_note_i2c (
    nullptr_t
) {
    NoteI2c* & note_i2c = instance::note_i2c();
    if (note_i2c) {
        delete note_i2c;
        note_i2c = nullptr;
    }
    return note_i2c;
}

template <typename T>
NoteI2c *
make_note_i2c (
    T & i2c_parameters_
)
{
    NoteI2c* & note_i2c = instance::note_i2c();
    if (!note_i2c) {
        note_i2c = new NoteI2c_Arduino(i2c_parameters_);
    }

    return note_i2c;
}

NoteI2c_Arduino::NoteI2c_Arduino
(
    TwoWire & i2c_bus_
) :
    _i2cPort(i2c_bus_)
{
    _i2cPort.begin();
}

NoteI2c_Arduino::~NoteI2c_Arduino (
    void
)
{
#if WIRE_HAS_END
    _i2cPort.end();
#endif
}

const char *
NoteI2c_Arduino::receive (
    uint16_t device_address_,
    uint8_t * buffer_,
    uint16_t requested_byte_count_,
    uint32_t * available_
)
{
    const char *result = nullptr;

    const size_t retry_count = 3;
    size_t i = 0;
    do {
        uint8_t transmission_error = 0;

        // Request response data from Notecard
        _i2cPort.beginTransmission(static_cast<uint8_t>(device_address_));
        _i2cPort.write(static_cast<uint8_t>(0));
        _i2cPort.write(static_cast<uint8_t>(requested_byte_count_));
        transmission_error = _i2cPort.endTransmission();

        switch (transmission_error) {
        case 0:
            // I2C transmission was successful
            result = nullptr;
            break;
        case 1:
            result = ERRSTR("i2c: data too long to fit in transmit buffer {io}",i2cerr);
            break;
        case 2:
            result = ERRSTR("i2c: received NACK on transmit of address {io}",i2cerr);
            break;
        case 3:
            result = ERRSTR("i2c: received NACK on transmit of data {io}",i2cerr);
            break;
        case 4:
            result = ERRSTR("i2c: unknown error on TwoWire::endTransmission() {io}",i2cerr);
            break;
        case 5:
            result = ERRSTR("i2c: timeout {io}",i2cerr);
            break;
        default:
            result = ERRSTR("i2c: unknown error encounter during I2C transmission {io}",i2cerr);
        }

        // Read and cache response from Notecard
        if (!transmission_error) {
            // Delay briefly ensuring that the Notecard can
            // deliver the data in real-time to the I2C ISR
            ::delay(2);

            const int request_length = requested_byte_count_ + NoteI2c::REQUEST_HEADER_SIZE;
            const int response_length = _i2cPort.requestFrom((int)device_address_, request_length);
            if (!response_length) {
                result = ERRSTR("serial-over-i2c: no response to read request {io}",i2cerr);
            } else if (response_length != request_length) {
                result = ERRSTR("serial-over-i2c: unexpected raw byte count {io}",i2cerr);
            } else {
                // Ensure available byte count is within expected range
                static const size_t AVAILABLE_MAX = (NoteI2c::REQUEST_MAX_SIZE - NoteI2c::REQUEST_HEADER_SIZE);
                uint32_t available = _i2cPort.read();
                if (available > AVAILABLE_MAX) {
                    result = ERRSTR("serial-over-i2c: available byte count greater than max allowed {io}",i2cerr);
                } else if (requested_byte_count_ != static_cast<uint8_t>(_i2cPort.read())) {
                    // Ensure protocol response length matches size request
                    result = ERRSTR("serial-over-i2c: unexpected protocol byte count {io}",i2cerr);
                } else {
                    // Update available with remaining bytes
                    *available_ = available;

                    for (size_t i = 0 ; i < requested_byte_count_ ; ++i) {
                        //TODO: Perf test against indexed buffer reads
                        *buffer_++ = _i2cPort.read();
                    }
                    result = nullptr;
                    break;
                }
            }
        }

        // Flash stalls have been observed on the Notecard ESP. Delaying
        // between retries provides time for the Notecard to recover from
        // the resource contention.
        ::delay(1000);
        NOTE_C_LOG_ERROR(result);
        NOTE_C_LOG_WARN("serial-over-i2c: reattempting to read Notecard response");
    } while (result && (i++ < retry_count));

    return result;
}

bool
NoteI2c_Arduino::reset (
    uint16_t device_address_
)
{
    (void)device_address_;
#if WIRE_HAS_END
    _i2cPort.end();
#endif
    _i2cPort.begin();
    return true;
}

const char *
NoteI2c_Arduino::transmit (
    uint16_t device_address_,
    uint8_t * buffer_,
    uint16_t size_
)
{
    const char * result = nullptr;
    uint8_t transmission_error = 0;

    _i2cPort.beginTransmission(static_cast<uint8_t>(device_address_));
    _i2cPort.write(static_cast<uint8_t>(size_));
    _i2cPort.write(buffer_, size_);
    transmission_error = _i2cPort.endTransmission();

    if (transmission_error) {
        switch (transmission_error) {
        case 1:
            result = ERRSTR("i2c: data too long to fit in transmit buffer {io}",i2cerr);
            break;
        case 2:
            result = ERRSTR("i2c: received NACK on transmit of address {io}",i2cerr);
            break;
        case 3:
            result = ERRSTR("i2c: received NACK on transmit of data {io}",i2cerr);
            break;
        case 4:
            result = ERRSTR("i2c: unknown error on TwoWire::endTransmission() {io}",i2cerr);
            break;
        case 5:
            result = ERRSTR("i2c: timeout {io}",i2cerr);
            break;
        default:
            result = ERRSTR("i2c: unknown error encounter during I2C transmission {io}",i2cerr);
        }
    }

    return result;
}

// Explicitly instantiate the template function for the supported types
template NoteI2c * make_note_i2c<TwoWire>(TwoWire &);
