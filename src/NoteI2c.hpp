#ifndef NOTE_I2C_HPP
#define NOTE_I2C_HPP

#include <stddef.h>
#include <stdint.h>

class NoteI2c
{
public:

    virtual ~NoteI2c(void) {}

    /**************************************************************************/
    /*!
        @brief  The Serial-over-I2C protocol receive callback implementation

        A blocking transaction filling the provided buffer with the requested
        number of bytes from the Notecard at the provided address. Upon success,
        a `nullptr` is returned, and `available` will populated with the number
        of bytes the Notecard is waiting to send. Otherwise, an error string
        describing the failure will be returned.

        @param[in]  device_address
                The I2C address.
        @param[out] buffer
                A buffer to hold the data read from the I2C controller. The
                buffer must be at least `requested_byte_count` bytes long.
        @param[in]  requested_byte_count
                The number of bytes requested.
        @param[out] available
                The number of bytes available for subsequent calls to receive().
        @returns A string with an error, or `nullptr` if the receive was
        successful.
    */
    /**************************************************************************/
    virtual const char * receive(uint16_t device_address, uint8_t * buffer, uint16_t requested_byte_count, uint32_t * available) = 0;

    /**************************************************************************/
    /*!
        @brief  The Serial-over-I2C protocol reset callback implementation
        @return `true`.
    */
    /**************************************************************************/
    virtual bool reset(uint16_t device_address) = 0;

    /**************************************************************************/
    /*!
        @brief  The Serial-over-I2C protocol transmit callback implementation

        A blocking transaction sending the provided buffer to the Notecard at
        the provided address. Upon success, a `nullptr` is returned, otherwise
        an error string describing the failure will be returned.

        @param[in] device_address
                The I2C address.
        @param[in] buffer
                The data to transmit over I2C. The caller should have already
                shifted it right so that the low bit is NOT the read/write bit.
        @param[in] size
                The number of bytes to transmit.
        @returns A string with an error, or `nullptr` if the transmission was
                successful.
    */
    /**************************************************************************/
    virtual const char * transmit(uint16_t device_address, uint8_t * buffer, uint16_t size) = 0;

    /**************************************************************************/
    /*!
        @brief  Size of the header for Serial-Over-I2C requests.

        @details The request made to the low-level I2C controller should be
                 for the `size` parameter supplied to the `receive` method.

        @see NoteI2c::receive
    */
    /**************************************************************************/
    static const size_t REQUEST_HEADER_SIZE = 2;

    /**************************************************************************/
    /*!
        @brief  Maximum size of a Serial-Over-I2C request.

        @details The requests made to and responses received from the low-level
                 I2C controller can be no larger than (REQUEST_MAX_SIZE
                 - REQUEST_HEADER_SIZE).

        @see NoteI2c::receive
    */
    /**************************************************************************/
    static const size_t REQUEST_MAX_SIZE = 255;
};

/******************************************************************************/
/*!
    @brief Creates a NoteI2c instance

    Helper function to abstract, create and maintain a single instance
    of the NoteI2c interface implementation, as required by the underlying
    `note-c` library.

    @param[in] i2c_parameters
               Pointer to the parameters required to instantiate
               the platform specific I2C implementation.
*/
/******************************************************************************/
template <typename T> NoteI2c * make_note_i2c (T & i2c_parameters);
NoteI2c * make_note_i2c (nullptr_t);

#endif // NOTE_I2C_HPP
