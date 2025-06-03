/*!
 @file n_request.c

 Written by Ray Ozzie and Blues Inc. team.

 Copyright (c) 2019 Blues Inc. MIT License. Use of this source code is
 governed by licenses granted by the copyright holder including that found in
 the
 <a href="https://github.com/blues/note-c/blob/master/LICENSE">LICENSE</a>
 file.
 */

#include "n_lib.h"

#include <string.h>

static const int RETRY_DELAY_MS = 500;

// For flow tracing
static int suppressShowTransactions = 0;

// Flag that gets set whenever an error occurs that should force a reset
NOTE_C_STATIC bool resetRequired = true;

// CRC data
#ifndef NOTE_C_LOW_MEM
static uint16_t seqNo = 0;
#define CRC_FIELD_LENGTH        22  // ,"crc":"SSSS:CCCCCCCC"
#define CRC_FIELD_NAME_OFFSET   1
#define CRC_FIELD_NAME_TEST     "\"crc\":\""
#define ERR_FIELD_NAME_TEST     "\"err\":\""
NOTE_C_STATIC int32_t _crc32(const void* data, size_t length);
NOTE_C_STATIC char * _crcAdd(char *json, uint16_t seqno);
NOTE_C_STATIC bool _crcError(char *json, uint16_t shouldBeSeqno);

NOTE_C_STATIC bool notecardFirmwareSupportsCrc = false;
#endif // !NOTE_C_LOW_MEM

/*!
 @internal

 @brief Create a JSON object containing an error message.

 Create a dynamically allocated `J` object containing a single string field
 "err" whose value is the passed in error message.

 @param id The "id" from the original request that resulted in an error
 @param errmsg The error message.

 @returns A `J` object with the "err" field populated.
 */
NOTE_C_STATIC J * _errDoc(uint32_t id, const char *errmsg)
{
    J *rspdoc = JCreateObject();
    if (rspdoc != NULL) {
        JAddStringToObject(rspdoc, c_err, errmsg);
        JAddStringToObject(rspdoc, "src", "note-c");
        if (id) {
            JAddIntToObject(rspdoc, "id", id);
        }
        if (suppressShowTransactions == 0) {
            _DebugWithLevel(NOTE_C_LOG_LEVEL_ERROR, "[ERROR] ");
            _DebugWithLevel(NOTE_C_LOG_LEVEL_ERROR, "{\"err\":\"");
            _DebugWithLevel(NOTE_C_LOG_LEVEL_ERROR, errmsg);
            _DebugWithLevelLn(NOTE_C_LOG_LEVEL_ERROR, "\",\"src\":\"note-c\"}");
        }
    } else {
        NOTE_C_LOG_ERROR("Failed to allocate error document!");
    }

    return rspdoc;
}

/*!
 @brief Resume showing transaction details.
 */
void _noteResumeTransactionDebug(void)
{
    suppressShowTransactions--;
}

/*!
 @brief Suppress showing transaction details.
 */
void _noteSuspendTransactionDebug(void)
{
    suppressShowTransactions++;
}

/*!
 @internal

 @brief Calculate the transaction timeout.

  When note.add or web.* requests are used to transfer binary data, the
  time to complete the transaction can vary depending on the size of
  the payload and network conditions. Therefore, it's possible for
  these transactions to timeout prematurely.

  The algorithm executes the following logic:
  - If the request is a `note.add`, set the timeout value to the
    value of the "milliseconds" parameter, if it exists. If it
    doesn't, use the "seconds" parameter. If that doesn't exist,
    use the standard timeout of `CARD_INTER_TRANSACTION_TIMEOUT_SEC`.
  - If the request is a `web.*`, follow the same logic, but instead
    of using the standard timeout, use the Notecard timeout of 90
    seconds for all `web.*` transactions.

 @param req The request object.
 @param isReq Whether the request is a regular request or a command.

 @returns The timeout in milliseconds.
*/
NOTE_C_STATIC uint32_t _noteTransaction_calculateTimeoutMs(J *req, bool isReq)
{
    uint32_t result = (CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000);

    // Interrogate the request
    if (JContainsString(req, (isReq ? "req" : "cmd"), "note.add")) {
        if (JIsPresent(req, "milliseconds")) {
            NOTE_C_LOG_DEBUG("Using `milliseconds` parameter value for "
                             "timeout.");
            result = JGetInt(req, "milliseconds");
        } else if (JIsPresent(req, "seconds")) {
            NOTE_C_LOG_DEBUG("Using `seconds` parameter value for timeout.");
            result = (JGetInt(req, "seconds") * 1000);
        }
    } else if (JContainsString(req, (isReq ? "req" : "cmd"), "web.")) {
        NOTE_C_LOG_DEBUG("web.* request received.");
        if (JIsPresent(req, "milliseconds")) {
            NOTE_C_LOG_DEBUG("Using `milliseconds` parameter value for "
                             "timeout.");
            result = JGetInt(req, "milliseconds");
        } else if (JIsPresent(req, "seconds")) {
            NOTE_C_LOG_DEBUG("Using `seconds` parameter value for timeout.");
            result = (JGetInt(req, "seconds") * 1000);
        } else {
            NOTE_C_LOG_DEBUG("No `milliseconds` or `seconds` parameter "
                             "provided. Defaulting to 90-second timeout.");
            result = (90 * 1000);
        }
    }

    return result;
}

/*!
 @brief Suppress showing transaction details.
 */
void NoteSuspendTransactionDebug(void)
{
    _noteSuspendTransactionDebug();
}

/*!
 @brief Resume showing transaction details.
 */
void NoteResumeTransactionDebug(void)
{
    _noteResumeTransactionDebug();
}

/*!
 @brief Create a new JSON request.

 Creates a dynamically allocated `J` object with one field `"req"` whose value
 is the passed in request string.

 @param request The name of the request, for example `hub.set`.

 @returns A `J` object with the "req" field populated.
 */
J *NoteNewRequest(const char *request)
{
    J *reqdoc = JCreateObject();
    if (reqdoc != NULL) {
        JAddStringToObject(reqdoc, c_req, request);
    }
    return reqdoc;
}

/*!
 @brief Create a new JSON command.

 Create a dynamically allocated `J` object with one field `"cmd"` whose value is
 the passed in request string. The difference between a command and a request is
 that the Notecard does not send a response to commands, only to requests.

 @param request The name of the command (e.g. `card.attn`).

 @returns A `J` object with the "cmd" field populated.

 */
J *NoteNewCommand(const char *request)
{
    J *reqdoc = JCreateObject();
    if (reqdoc != NULL) {
        JAddStringToObject(reqdoc, c_cmd, request);
    }
    return reqdoc;
}

/*!
 @brief Send a request to the Notecard.

 The passed in request object is always freed, regardless of if the request was
 successful or not. The response from the Notecard, if any, is freed and not
 returned to the caller.

 @param req Pointer to a `J` request object.

 @returns `true` if successful and `false` if an error occurs (e.g. out of
          memory or the response from the Notecard has an "err" field). If req
          is a command rather than a request, a `true` return value indicates
          that the command was sent without error. However, since the Notecard
          sends no response to commands, it does not guarantee that the
          command was received and processed by the Notecard.

 @see `NoteRequestResponse` if you need to work with the response.
 */
bool NoteRequest(J *req)
{
    J *rsp = NoteRequestResponse(req);
    if (rsp == NULL) {
        return false;
    }

    // Check for a transaction error, and exit
    bool success = JIsNullString(rsp, c_err);
    JDelete(rsp);

    return success;
}

/*!
 @brief Send a request to the Notecard, retrying it until it succeeds or it
        times out.

 The passed in request object is always freed, regardless of if the request was
 successful or not. The response from the Notecard, if any, is freed and not
 returned to the caller.

 @param req Pointer to a `J` request object.
 @param timeoutSeconds Time limit for retires, in seconds, if there is no
        response, or if the response contains an I/O error.

 @returns `true` if successful and `false` if an error occurs (e.g. out of
          memory or the response from the Notecard has an "err" field).

 @see `NoteRequestResponseWithRetry` if you need to work with the response.
 */
bool NoteRequestWithRetry(J *req, uint32_t timeoutSeconds)
{
    J *rsp = NoteRequestResponseWithRetry(req, timeoutSeconds);
    // If there is no response return false
    if (rsp == NULL) {
        return false;
    }

    // Check for a transaction error, and exit
    bool success = JIsNullString(rsp, c_err);
    JDelete(rsp);

    return success;
}

/*!
 @brief Send a request to the Notecard and return the response.

 The passed in request object is always freed, regardless of if the request was
 successful or not.

 @param req Pointer to a `J` request object.

 @returns A `J` object with the response or NULL if there was an error sending
          the request.

 @see `NoteResponseError` to check the response for errors.
 */
J *NoteRequestResponse(J *req)
{
    // Exit if null request. This allows safe execution of the form
    // NoteRequestResponse(NoteNewRequest("xxx"))
    if (req == NULL) {
        return NULL;
    }
    // Execute the transaction
    J *rsp = NoteTransaction(req);
    // Free the request and exit
    JDelete(req);
    return rsp;
}

/*!
 @brief Send a request to the Notecard, retrying it until it succeeds or it
        times out, and return the response.

 The passed in request object is always freed, regardless of if the request was
 successful or not.

 @param req Pointer to a `J` request object.
 @param timeoutSeconds Time limit for retires, in seconds, if there is no
        response, or if the response contains an I/O error.

 @returns A `J` object with the response or NULL if there was an error sending
          the request.

 @see `NoteResponseError` to check the response for errors.
 */
J *NoteRequestResponseWithRetry(J *req, uint32_t timeoutSeconds)
{
    // Exit if null request. This allows safe execution of the form
    // NoteRequestResponse(NoteNewRequest("xxx"))
    if (req == NULL) {
        return NULL;
    }

    J *rsp;

    // Calculate expiry time in milliseconds
    uint32_t startMs = _GetMs();
    uint32_t timeoutMs = timeoutSeconds * 1000;

    while(true) {
        // Execute the transaction
        rsp = NoteTransaction(req);

        // Loop if there is no response, or if there is an io error
        if ((rsp == NULL) || (JContainsString(rsp, c_err, c_ioerr) && !JContainsString(rsp, c_err, c_unsupported))) {

            // Free error response
            if (rsp != NULL) {
                JDelete(rsp);
                rsp = NULL;
            }
        } else {

            // Exit loop on non-null response without io error
            break;
        }

        // Exit loop on timeout
        if (_GetMs() - startMs >= timeoutMs) {
            break;
        }
    }

    // Free the request
    JDelete(req);

    // Return the response
    return rsp;
}

/*!
 @brief Send a request to the Notecard and return the response.

 Unlike `NoteRequestResponse`, this function expects the request to be a valid
 JSON C-string, rather than a `J` object. The string is expected to be
 newline-terminated, otherwise the call produces undefined behavior. The
 response is returned as a dynamically allocated JSON C-string. The response
 string is verbatim what was sent by the Notecard, which IS newline-terminated.
 The caller is responsible for freeing the response string. If the request was a
 command (i.e. it uses "cmd" instead of "req"), this function returns NULL,
 because the Notecard does not send a response to commands.

 @param reqJSON A valid newline-terminated JSON C-string containing the request.

 @returns A newline-terminated JSON C-string with the response, or NULL
          if there was no response or if there was an error.

 @note When a "cmd" is sent, it is not possible to determine if an error occurred.

 @note Unlike the `NoteRequest*` functions, this function does not automatically
       free the request JSON string. It is not possible to know if the parameter
       is a string literal. As such, it is the caller's responsibility to manage
       the memory associated with the request string.
*/
char * NoteRequestResponseJSON(const char *reqJSON)
{
    const uint32_t transactionTimeoutMs = (CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000);
    char *rspJSON = NULL;
    char *allocatedJSON = NULL; // required to free the string if it is not newline-terminated
    bool isCmdPipeline = false;

    if (reqJSON == NULL) {
        return NULL;
    }

    // Make sure that we get access to the Notecard before transacting.
    if (!_TransactionStart(transactionTimeoutMs)) {
        return NULL;
    }

    _LockNote();

    // Manually tokenize the string to search for multiple embedded
    // commands (cannot use strtok)
    for (;;) {
        const char *endPtr;
        const char * const newlinePtr = strchr(reqJSON, '\n');

        // If string is not newline-terminated, then allocate a new
        // string and terminate it
        if (NULL == newlinePtr) {
            // All JSON strings should be newline-terminated to meet the
            // specification, however this is required to ensure backward
            // compatibility with the previous implementation.
            const size_t allocLen = strlen(reqJSON);
            if (0 == allocLen) {
                NOTE_C_LOG_ERROR(ERRSTR("request: jsonbuf zero length", c_bad));
                break;
            }

            NOTE_C_LOG_WARN(ERRSTR("Memory allocation due to malformed request (not newline-terminated)", c_bad));
            allocatedJSON = _Malloc(allocLen + 2);  // +2 for newline and null-terminator
            if (allocatedJSON == NULL) {
                NOTE_C_LOG_ERROR(ERRSTR("request: jsonbuf malloc failed", c_mem));
                break;
            }

            memcpy(allocatedJSON, reqJSON, allocLen);
            allocatedJSON[allocLen] = '\n';
            allocatedJSON[allocLen + 1] = '\0';
            reqJSON = allocatedJSON;
            endPtr = &allocatedJSON[allocLen];
        } else {
            isCmdPipeline = !(strlen(newlinePtr) == 1);
            endPtr = newlinePtr;
        }
        const size_t reqLen = ((endPtr - reqJSON) + 1);

        bool isCmd = false;
        if (strstr(reqJSON, "\"cmd\":") != NULL) {
            // Only call `JParse()` after verifying the provided request
            // appears to contain a command (i.e. we find `"cmd":`).
            J *jsonObj = JParse(reqJSON);
            if (!jsonObj) {
                // Invalid JSON.
                if (NULL == newlinePtr) {
                    _Free((void *)reqJSON);
                }
                break;
            }
            isCmd = JIsPresent(jsonObj, "cmd");
            JDelete(jsonObj);
        }

        if (!isCmd) {
            const char *errstr = _Transaction(reqJSON, reqLen, &rspJSON, transactionTimeoutMs);
            if (errstr != NULL) {
                NOTE_C_LOG_ERROR(errstr);

                // Extract ID from the request JSON, if present
                uint32_t id = 0;
                J *req = JParse(reqJSON);
                if (req != NULL) {
                    id = JGetInt(req, "id");
                    JDelete(req);
                }

                // Use _errDoc() to create a well-formed JSON error string
                J *errdoc = _errDoc(id, errstr);
                if (errdoc != NULL) {
                    char *errdocJSON = JPrintUnformatted(errdoc);
                    JDelete(errdoc);
                    if (errdocJSON != NULL) {
                        uint32_t errdocJSONLen = strlen(errdocJSON);
                        rspJSON = (char *) _Malloc(errdocJSONLen+2);
                        if (rspJSON != NULL) {
                            memcpy(rspJSON, errdocJSON, errdocJSONLen);
                            rspJSON[errdocJSONLen++] = '\n';
                            rspJSON[errdocJSONLen] = '\0';
                        }
                        _Free((void *)errdocJSON);
                    }
                }
            }
            if (allocatedJSON) {
                _Free((void *)allocatedJSON);
                allocatedJSON = NULL;
            }
            break;
        } else {
            // If it's a command, the Notecard will not respond, so we pass
            // NULL for the response parameter.
            const char *errstr = _Transaction(reqJSON, reqLen, NULL, transactionTimeoutMs);
            reqJSON = (endPtr + 1);  // Move to the next command in the pipeline
            if (errstr != NULL) {
                NOTE_C_LOG_ERROR(errstr);
            }
        }

        // Clean up if we allocated a new string
        (void)isCmdPipeline;
        if (allocatedJSON) {
            _Free((void *)allocatedJSON);
            break;
        }
        if (!isCmdPipeline) {
            break;
        }
    } // for(;;)

    _UnlockNote();
    _TransactionStop();

    return rspJSON;
}

/*!
 @brief Send a request to the Notecard and return the response.

 This function doesn't free the passed in request object. The caller is
 responsible for freeing it.

 @param req Pointer to a `J` request object.

 @returns A `J` object with the response or NULL if there was an error sending
          the request.

 @see `NoteResponseError` to check the response for errors.
 */
J *NoteTransaction(J *req)
{
    return _noteTransactionShouldLock(req, true);
}

/**************************************************************************/
/*!
  @brief Same as `NoteTransaction`, but takes an additional parameter that
  indicates if the Notecard should be locked.
  @param   req
  The `J` cJSON request object.
  @param   lockNotecard
  Set to `true` if the Notecard should be locked and `false` otherwise.
  @returns a `J` cJSON object with the response, or NULL if there is
  insufficient memory.
*/
/**************************************************************************/
J *_noteTransactionShouldLock(J *req, bool lockNotecard)
{
    // Validate in case of memory failure of the requestor
    if (req == NULL) {
        NOTE_C_LOG_ERROR(ERRSTR("NULL request", c_bad));
        return NULL;
    }

    // Serialize the JSON request
    char *json = JPrintUnformatted(req); // `json` allocated, must be freed
    if (json == NULL) {
        if (lockNotecard) {
            _UnlockNote();
        }
        _TransactionStop();
        NOTE_C_LOG_ERROR(ERRSTR("failed to serialize JSON request", c_mem));
        return NULL;
    }

    // Determine the request or command type
    const char * const reqApi = JGetString(req, "req");
    const bool reqFound = reqApi[0];  // test for non-empty string
    const char * const cmdApi = JGetString(req, "cmd");
    const bool cmdFound = cmdApi[0];  // test for non-empty string

    // If neither `"req"` nor `"cmd"` are found, then we have an error
    // condition. If both are present, then we have undefined behavior.
    if (!reqFound && !cmdFound) {
        _Free(json);
        NOTE_C_LOG_ERROR(ERRSTR("neither req nor cmd found in API invocation (invalid JSON)", c_bad));
        return NULL;
    } else if (reqFound && cmdFound) {
        _Free(json);
        NOTE_C_LOG_ERROR(ERRSTR("both req and cmd present in API invocation (undefined behavior)", c_bad));
        return NULL;
    }

    // Extract the ID of the request so that errors can be returned with the same ID
    const uint32_t id = JGetInt(req, "id");

    // Ensure the Notecard is ready
    if (!_TransactionStart(CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000)) {
        _Free(json);
        const char *errStr = ERRSTR("Notecard not ready (CTX/RTX) {io}", c_ioerr);
        if (cmdFound) {
            NOTE_C_LOG_ERROR(errStr);
            return NULL;
        }
        return _errDoc(id, errStr);
    }

    // Inject the user agent object only when we're doing a `hub.set` and
    // specifying the product UID together. The goal is to only piggyback
    // user agent data when the host is initializing the Notecard, as opposed
    // to every time the host does a `hub.set` to change mode.
#ifndef NOTE_DISABLE_USER_AGENT
    if (!JIsPresent(req, "body") && JContainsString(req, (reqFound ? "req" : "cmd"), "hub.set") && JIsPresent(req, "product")) {
        J *body = NoteUserAgent();
        if (body != NULL) {
            JAddItemToObject(req, "body", body);
            NOTE_C_LOG_DEBUG("Added user-agent to request");
        } else {
            NOTE_C_LOG_ERROR(ERRSTR("Failed to add user-agent to request", c_mem));
        }
    }
#endif

    // If a reset of the I/O interface is required for any reason, do it now.
    // We must do this before acquiring lock.
    if (resetRequired) {
        NOTE_C_LOG_DEBUG("Resetting Notecard I/O Interface...");
        if (!NoteReset()) {
            _Free(json);
            _TransactionStop();
            const char *errStr = ERRSTR("failed to reset Notecard interface {io}", c_iobad);
            if (cmdFound) {
                NOTE_C_LOG_ERROR(errStr);
                return NULL;
            }
            return _errDoc(id, errStr);
        }
    }

    // Take the lock on the Notecard.  This is required to ensure that we don't
    // have multiple threads trying to access the Notecard at the same time.
    if (lockNotecard) {
        _LockNote();
    }

    // Calculate the transaction timeout based on the parameters in the request.
    const uint32_t transactionTimeoutMs = _noteTransaction_calculateTimeoutMs(req, reqFound);

#ifndef NOTE_C_LOW_MEM
    /*
    * Add a CRC value, so the request may be retried if it is received
    * in a corrupted state.
    *
    * NOTE: This can only performed on requests, because commands do not have a
    *       'response channel'. As such, we have no ability to understand if a
    *       command failed and should be retried. A sequence number is included
    *       as part of the CRC data, so that two identical but separate requests
    *       are not mistaken as the same request being retried.
    *
    *   req   cmd  response
    *  found found expected
    *  ----- ----- --------
    *    0     0    ERROR
    *    0     1      0
    *    1     0      1
    *    1     1    1 (UB)
    */
    bool crcAddedToRequest = false;
    if (reqFound) {
        char *newJson = _crcAdd(json, seqNo);
        if (newJson != NULL) {
            _Free(json);
            json = newJson;
            crcAddedToRequest = true;
        }
    }
#endif // !NOTE_C_LOW_MEM

    // If we're performing retries, this is where we come back to
    // after a failed transaction.
    const char *errStr = NULL;
    char *rspJsonStr = NULL;
    J *rsp = NULL;
    for (uint8_t lastRequestRetries = 0; lastRequestRetries <= CARD_REQUEST_RETRIES_ALLOWED; ++lastRequestRetries) {
        // free on retry
        if (rsp != NULL) {
            JDelete(rsp);
        }

        // reset variables
        errStr = NULL;
        rspJsonStr = NULL;
        rsp = NULL;

        // Trace request unless suppressed
        if (suppressShowTransactions == 0) {
            NOTE_C_LOG_INFO(json);
        }

        // In-place replacement of NULL-terminator with a newline character.
        // The Notecard expects a newline-terminated string to understand the
        // end of the request.
        const size_t jsonLen = strlen(json);
        json[jsonLen] = '\n';

        // Perform the transaction
        if (cmdFound) {
            errStr = _Transaction(json, (jsonLen + 1), NULL, transactionTimeoutMs);
            // break;  // No response expected for commands and no ability to retry.
        } else {
            errStr = _Transaction(json, (jsonLen + 1), &rspJsonStr, transactionTimeoutMs);
        }

        // Restore NULL-terminator
        json[jsonLen] = '\0';

        ////////////////////////
        // Request retry logic
        ////////////////////////

        // Handle transaction errors
        if (errStr != NULL) {
            _Free(rspJsonStr);
            // If there's an I/O error on the transaction, retry
            if (NoteErrorContains(errStr, c_ioerr)) {
                NOTE_C_LOG_WARN(ERRSTR("retrying... transaction failure", c_iobad));
                resetRequired = !_Reset();
                _DelayMs(RETRY_DELAY_MS);
                continue;  // I/O error, retry
            } else {
                NOTE_C_LOG_DEBUG(ERRSTR("transaction failure", c_bad));
                break;  // Fatal error, do not retry
            }
        } else if (cmdFound) {
            NOTE_C_LOG_DEBUG("Command successfully sent to Notecard");
            break;  // No response expected and no further ability to retry.
        }

        // Inspect the Notecard Response
        if (rspJsonStr == NULL) {
            // If the response is NULL, then we have a timeout or other error
            errStr = ERRSTR("response expected, but response is NULL {io}", c_ioerr);
            NOTE_C_LOG_WARN(ERRSTR("retrying... no response", c_iobad));
            _DelayMs(RETRY_DELAY_MS);
            continue;  // I/O error, retry
        }

#ifndef NOTE_C_LOW_MEM
        // If we sent a CRC in the request, examine the response JSON to see if
        // it has a CRC error.  Note that the CRC is stripped from the
        // rspJsonStr as a side-effect of this method.
        if (crcAddedToRequest && _crcError(rspJsonStr, seqNo)) {
            _Free(rspJsonStr);
            errStr = ERRSTR("CRC error {io}", c_iobad);
            NOTE_C_LOG_WARN(ERRSTR("retrying... CRC error", c_iobad));
            _DelayMs(RETRY_DELAY_MS);
            continue;
        }
#endif // !NOTE_C_LOW_MEM

        // Error types
        bool isBadBin = false;
        bool isIoError = false;

        // Error detection / classification
        rsp = JParse(rspJsonStr);
        if (rsp != NULL) {
            isBadBin = JContainsString(rsp, c_err, c_badbinerr);
            isIoError = JContainsString(rsp, c_err, c_ioerr) && !JContainsString(rsp, c_err, c_unsupported);
        } else {
            // Failed to parse response as JSON
            isIoError = true;
#ifndef NOTE_C_LOW_MEM
            _DebugWithLevel(NOTE_C_LOG_LEVEL_ERROR, "[ERROR] ");
            _DebugWithLevel(NOTE_C_LOG_LEVEL_ERROR, "invalid JSON {io}: ");
            _DebugWithLevel(NOTE_C_LOG_LEVEL_ERROR, rspJsonStr);
#else
            NOTE_C_LOG_ERROR(c_ioerr);
#endif // !NOTE_C_LOW_MEM
        }

        // Error handling
        if (isIoError || isBadBin) {
            if (rsp != NULL) {
                NOTE_C_LOG_ERROR(JGetString(rsp, c_err));
            }
            if (isBadBin) {
                NOTE_C_LOG_DEBUG("{bad-bin} errors not eligible for retry");
                break;
            } else {
                _Free(rspJsonStr);
                errStr = ERRSTR("corrupt response {io}", c_ioerr);
                NOTE_C_LOG_WARN(ERRSTR("retrying... corrupt response", c_iobad));
                _DelayMs(RETRY_DELAY_MS);
                continue;
            }
        }

        // Transaction completed
        break;
    } // end of retry loop

    // Free the original serialized JSON request
    _Free(json);

#ifndef NOTE_C_LOW_MEM
    // Request processing complete, regardless of success or error.
    // Now, advance the request sequence number.
    seqNo++;
#endif // !NOTE_C_LOW_MEM

    // Return an empty object (with no err field) when no response is expected
    if (cmdFound) {
        if (lockNotecard) {
            _UnlockNote();
        }
        _TransactionStop();
        return JCreateObject();
    }

    // Handle error condition
    if (errStr != NULL) {
        if (rsp != NULL) {
            JDelete(rsp);
            rsp = NULL;
        }
        NoteResetRequired(); // queue up a reset
        J *errRsp = _errDoc(id, errStr);
        if (lockNotecard) {
            _UnlockNote();
        }
        _TransactionStop();
        return errRsp;
    }

    // Log and discard the response JSON
    if (suppressShowTransactions == 0) {
        NOTE_C_LOG_INFO(rspJsonStr);
    }
    _Free(rspJsonStr);

    // Release the Notecard lock
    if (lockNotecard) {
        _UnlockNote();
    }

    // Inform the Notecard that the transaction is complete.
    // This allows the Notecard (ESP) to drop into low power mode.
    _TransactionStop();

    // Done
    return rsp;
}

/*!
 @brief Mark that a reset will be required before doing further I/O on a given
        port.
 */
void NoteResetRequired(void)
{
    resetRequired = true;
}

/*!
 @brief Initialize or re-initialize the I/O inferface (I2C/UART).

 @returns True if the reset was successful and false if not.
 */
bool NoteReset(void)
{
    _LockNote();
    resetRequired = !_Reset();
    _UnlockNote();
    return !resetRequired;
}

/*!
 @brief Check to see if a Notecard error is present in a JSON string.

 Only Notecard errors enclosed in `{}` (e.g. `{io}` for an I/O error) are
 guaranteed by the API.

 @param errstr The string to check for errors.
 @param errtype The error substring to search for in errstr.

 @returns `true` if errstr contains errtype and `false` otherwise.
 */
bool NoteErrorContains(const char *errstr, const char *errtype)
{
    return (strstr(errstr, errtype) != NULL);
}

/*!
 @brief Clean error strings out of the specified buffer.

 Notecard errors are enclosed in {} (e.g. {io} for an I/O error). This
 function takes the input string and removes all errors from it, meaning it
 removes any substrings matching the pattern {some error string}, including the
 braces.

 @param begin A C-string to to clean of errors.
 */
void NoteErrorClean(char *begin)
{
    while (true) {
        char *end = &begin[strlen(begin)+1];
        char *beginBrace = strchr(begin, '{');
        if (beginBrace == NULL) {
            break;
        }
        if (beginBrace>begin && *(beginBrace-1) == ' ') {
            beginBrace--;
        }
        char *endBrace = strchr(beginBrace, '}');
        if (endBrace == NULL) {
            break;
        }
        char *afterBrace = endBrace + 1;
        memmove(beginBrace, afterBrace, end-afterBrace);
    }
}

#ifndef NOTE_C_LOW_MEM

/*!
 @brief Convert a hex string to a 64-bit unsigned integer.

 @param p The hex string to convert.
 @param maxLen The length of the hex string.

 @returns The converted number.
 */
uint64_t _n_atoh(char *p, int maxLen)
{
    uint64_t n = 0;
    char *ep = p+maxLen;

    while (p < ep) {
        char ch = *p++;
        bool digit = (ch >= '0' && ch <= '9');
        bool lcase = (ch >= 'a' && ch <= 'f');
        bool space = (ch == ' ');
        bool ucase = (ch >= 'A' && ch <= 'F');
        if (!digit && !lcase && !space && !ucase) {
            break;
        }
        n *= 16;
        if (digit) {
            n += ch - '0';
        } else if (lcase) {
            n += 10 + (ch - 'a');
        } else if (ucase) {
            n += 10 + (ch - 'A');
        }
    }

    return (n);
}

static uint32_t lut[16] = {
    0x00000000, 0x1DB71064, 0x3B6E20C8, 0x26D930AC, 0x76DC4190, 0x6B6B51F4,
    0x4DB26158, 0x5005713C, 0xEDB88320, 0xF00F9344, 0xD6D6A3E8, 0xCB61B38C,
    0x9B64C2B0, 0x86D3D2D4, 0xA00AE278, 0xBDBDF21C
};

/*!
 @brief Compute the CRC32 of the passed in buffer.

 Small lookup-table half-byte CRC32 algorithm. See
 https://create.stephan-brumme.com/crc32/#half-byte

 @param data The buffer.
 @param length The length of the buffer.

 @returns The CRC32 of the buffer.
 */
NOTE_C_STATIC int32_t _crc32(const void* data, size_t length)
{
    uint32_t previousCrc32 = 0;
    uint32_t crc = ~previousCrc32;
    unsigned char* current = (unsigned char*) data;

    while (length--) {
        crc = lut[(crc ^  *current      ) & 0x0F] ^ (crc >> 4);
        crc = lut[(crc ^ (*current >> 4)) & 0x0F] ^ (crc >> 4);
        current++;
    }

    return ~crc;
}

/*!
 @brief Append a "crc" field to the passed in JSON buffer.

 The "crc" field has a value of the form "SSSS:CCCCCCCC", where SSSS is the
 passed in sequence number and CCCCCCCC is the CRC32.

 @param json The JSON buffer to both add the CRC32 to and to compute the
        CRC32 over.
 @param seqno A 16-bit sequence number to include as a part of the CRC.

 @returns A dynamically-allocated version of the passed in buffer with the CRC
          field added or NULL on error.
 */
NOTE_C_STATIC char *_crcAdd(char *json, uint16_t seqno)
{

    // Allocate a block the size of the input json plus the size of
    // the field to be added.  Note that the input JSON ends in '"}' and
    // this will be replaced with a combination of 4 hex digits of the
    // seqno plus 8 hex digits of the CRC32, and the '}' will be
    // transformed into ',"crc":"SSSS:CCCCCCCC"}' where SSSS is the
    // seqno and CCCCCCCC is the CRC32.  Note that the comma is
    // replaced with a space if the input json doesn't contain
    // any fields, so that we always return compliant JSON.

    size_t jsonLen = strlen(json);

    // Minimum JSON is "{}" and must end with a closing "}".
    if (jsonLen < 2 || json[jsonLen-1] != '}') {
        return NULL;
    }
    char *newJson = (char *) _Malloc(jsonLen+CRC_FIELD_LENGTH+1);
    if (newJson == NULL) {
        return NULL;
    }

    bool isEmptyObject = (memchr(json, ':', jsonLen) == NULL);
    size_t newJsonLen = jsonLen-1;

    memcpy(newJson, json, newJsonLen);
    newJson[newJsonLen++] = (isEmptyObject ? ' ' : ',');    // Replace }
    newJson[newJsonLen++] = '"';                            // +1
    newJson[newJsonLen++] = 'c';                            // +2
    newJson[newJsonLen++] = 'r';                            // +3
    newJson[newJsonLen++] = 'c';                            // +4
    newJson[newJsonLen++] = '"';                            // +5
    newJson[newJsonLen++] = ':';                            // +6
    newJson[newJsonLen++] = '"';                            // +7
    _n_htoa16(seqno, (uint8_t *) &newJson[newJsonLen]);
    newJsonLen += 4;                                        // +11
    newJson[newJsonLen++] = ':';                            // +12
    _n_htoa32(_crc32(json, jsonLen), &newJson[newJsonLen]);
    newJsonLen += 8;                                        // +20
    newJson[newJsonLen++] = '"';                            // +21
    newJson[newJsonLen++] = '}';                            // +22 == CRC_FIELD_LENGTH
    newJson[newJsonLen] = '\0';                             // null-terminated as it came in

    return newJson;
}

/*!
 @brief Check the passed in JSON for CRC and sequence number errors.

 If the calculated CRC32 doesn't match what's in the buffer, that's an error. If
 the sequence number in the buffer doesn't match shouldBeSeqno, that's an error.
 If there is no CRC field in the buffer, that's not an error UNLESS this
 function has been given a buffer with a CRC field before. In this case, the
 lack of a CRC field is an error.

 @param json The JSON buffer for which the CRC should be checked. Note that the
        CRC is stripped from the input JSON regardless of whether or not there
        was an error.
 @param shouldBeSeqno The expected sequence number.

 @returns `true` if there's an error and `false` otherwise.

 @note The CRC is stripped from the input JSON regardless of whether or not
        there was an error.
 */
NOTE_C_STATIC bool _crcError(char *json, uint16_t shouldBeSeqno)
{
    // Trim whitespace (specifically "\r\n") for CRC calculation
    size_t jsonLen = strlen(json);
    while (jsonLen > 0 && json[jsonLen - 1] <= ' ') {
        jsonLen--;
    }

    // Ignore CRC check if the response contains an error
    // A valid JSON string begins with "{". Therefore, the presence of an error
    // message in a well-formed JSON string MUST result in a non-zero response.
    if (strstr(json, ERR_FIELD_NAME_TEST)) {
        return false;
    }

    // Skip if invalid JSON or is too short to contain a CRC parameter
    // Minimum length is "{}" (2 bytes) + CRC_FIELD_LENGTH
    // Valid JSON ends with a closing "}"
    if ((jsonLen < (CRC_FIELD_LENGTH + 2)) || (json[jsonLen - 1] != '}')) {
        return false;
    }

    // See if it has a compliant CRC field
    size_t fieldOffset = ((jsonLen - 1) - CRC_FIELD_LENGTH);
    if (memcmp(&json[fieldOffset + CRC_FIELD_NAME_OFFSET], CRC_FIELD_NAME_TEST, ((sizeof(CRC_FIELD_NAME_TEST) - 1)) != 0)) {
        // If we've seen a CRC before, we should see one every time
        return notecardFirmwareSupportsCrc;
    }

    // If we get here, we've seen at least one CRC from the Notecard, so we should expect it.
    notecardFirmwareSupportsCrc = true;

    // Extract the CRC field and the sequence number
    char *p = &json[fieldOffset + CRC_FIELD_NAME_OFFSET + (sizeof(CRC_FIELD_NAME_TEST) - 1)];
    uint16_t actualSeqno = (uint16_t) _n_atoh(p, 4);
    uint32_t actualCrc32 = (uint32_t) _n_atoh(p+5, 8);
    json[fieldOffset++] = '}';
    json[fieldOffset] = '\0';
    uint32_t shouldBeCrc32 = _crc32(json, fieldOffset);

    return (shouldBeSeqno != actualSeqno || shouldBeCrc32 != actualCrc32);
}

#endif // !NOTE_C_LOW_MEM
