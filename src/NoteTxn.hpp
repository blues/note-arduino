#ifndef NOTE_TXN_HPP
#define NOTE_TXN_HPP

#include <stddef.h>
#include <stdint.h>

class NoteTxn
{
public:
    /**************************************************************************/
    /*!
        @brief  Type used to abstract specific hardware implementation types.
    */
    /**************************************************************************/
    typedef void * param_t;

    virtual ~NoteTxn(void) {}

    /**************************************************************************/
    /*!
        @brief  A blocking call used to initiate a transaction with the Notecard
        @param[in]  timeout_ms
                The number of milliseconds to wait before aborting the request
        @returns A boolean indicating success.
    */
    /**************************************************************************/
    virtual bool start (uint32_t timeout_ms) = 0;

    /**************************************************************************/
    /*!
        @brief  Non-blocking call to terminate the transaction
    */
    /**************************************************************************/
    virtual void stop (void) = 0;
};

/******************************************************************************/
/*!
    @brief  Helper function to abstract, create and maintain a single instance
    of the NoteTxn interface implementation, as required by the underlying
    `note-c` library.
    @param[in] txn_parameters
               Pointer to the parameters required to instantiate
               the platform specific transaction implementation.
*/
/******************************************************************************/
NoteTxn * make_note_txn (
    NoteTxn::param_t txn_parameters
);

#endif // NOTE_TXN_HPP
