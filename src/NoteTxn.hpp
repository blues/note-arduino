#ifndef NOTE_TXN_HPP
#define NOTE_TXN_HPP

#include <stddef.h>
#include <stdint.h>

class NoteTxn
{
public:

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
template<typename T> NoteTxn * make_note_txn (T & txn_parameters);
NoteTxn * make_note_txn (nullptr_t);

#endif // NOTE_TXN_HPP
