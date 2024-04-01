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

#ifdef NOTE_C_TEST
#include "test_static.h"
#else
#define NOTE_C_STATIC static
#endif

// For flow tracing
static int suppressShowTransactions = 0;

// Flag that gets set whenever an error occurs that should force a reset
static bool resetRequired = true;

// CRC data
#ifndef NOTE_C_LOW_MEM
static uint16_t lastRequestSeqno = 0;
#define CRC_FIELD_LENGTH        22  // ,"crc":"SSSS:CCCCCCCC"
#define CRC_FIELD_NAME_OFFSET   1
#define CRC_FIELD_NAME_TEST     "\"crc\":\""
NOTE_C_STATIC int32_t crc32(const void* data, size_t length);
NOTE_C_STATIC char * crcAdd(char *json, uint16_t seqno);
NOTE_C_STATIC bool crcError(char *json, uint16_t shouldBeSeqno);

static bool notecardSupportsCrc = false;
#endif

/*!
 @internal

 @brief Create a JSON object containing an error message.

 Create a dynamically allocated `J` object containing a single string field
 "err" whose value is the passed in error message.

 @param id The "id" from the original request that resulted in an error
 @param errmsg The error message.

 @returns A `J` object with the "err" field populated.
 */
NOTE_C_STATIC J * errDoc(uint32_t id, const char *errmsg)
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
    }

    return rspdoc;
}

/*!
 @brief Suppress showing transaction details.
 */
void NoteSuspendTransactionDebug(void)
{
    suppressShowTransactions++;
}

/*!
 @brief Resume showing transaction details.
 */
void NoteResumeTransactionDebug(void)
{
    suppressShowTransactions--;
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
    if (rsp == NULL) {
        JDelete(req);
        return NULL;
    }
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
*/
char * NoteRequestResponseJSON(const char *reqJSON)
{
    uint32_t transactionTimeoutMs = (CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000);
    char *rspJSON = NULL;

    if (reqJSON == NULL) {
        return NULL;
    }

    // Make sure that we get access to the Notecard before transacting.
    if (!_TransactionStart(transactionTimeoutMs)) {
        return NULL;
    }

    _LockNote();

    // Manually tokenize the string to search for multiple embedded commands (cannot use strtok)
    for (;;) {
        const char *endPtr;
        const char * const newlinePtr = strchr(reqJSON, '\n');

        // If string is not newline-terminated, then allocate a new string and terminate it
        if (NULL == newlinePtr) {
            // All JSON strings should be newline-terminated to meet the specification, however
            // this is required to ensure backward compatibility with the previous implementation.
            const size_t tempLen = strlen(reqJSON);
            if (0 == tempLen) {
                NOTE_C_LOG_ERROR(ERRSTR("request: jsonbuf zero length", c_bad));
                break;
            }

            NOTE_C_LOG_WARN(ERRSTR("Memory allocation due to malformed request (not newline-terminated)", c_bad));
            char * const temp = _Malloc(tempLen + 2);  // +2 for newline and null-terminator
            if (temp == NULL) {
                NOTE_C_LOG_ERROR(ERRSTR("request: jsonbuf malloc failed", c_mem));
                break;
            }

            memcpy(temp, reqJSON, tempLen);
            temp[tempLen] = '\n';
            temp[tempLen + 1] = '\0';
            reqJSON = temp;
            endPtr = &temp[tempLen];
        } else {
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
                uint32_t id = 0;
                if (reqJSON != NULL) {
                    J *req = JParse(reqJSON);
                    if (req != NULL) {
                        id = JGetInt(req, "id");
                        JDelete(req);
                    }
                }
                J *errdoc = errDoc(id, errstr);
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
            if (NULL == newlinePtr) {
                _Free((void *)reqJSON);
            }
            break;
        } else {
            // If it's a command, the Notecard will not respond, so we pass NULL for
            // the response parameter.
            const char *errstr = _Transaction(reqJSON, reqLen, NULL, transactionTimeoutMs);
            reqJSON = (endPtr + 1);
            if (errstr != NULL) {
                NOTE_C_LOG_ERROR(errstr);
            }
        }

        // Clean up if we allocated a new string
        if (NULL == newlinePtr) {
            _Free((void *)reqJSON);
        }
    }

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
    return noteTransactionShouldLock(req, true);
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
J *noteTransactionShouldLock(J *req, bool lockNotecard)
{
    // Validate in case of memory failure of the requestor
    if (req == NULL) {
        return NULL;
    }

    // Make sure that we get access to the notecard hardware before we begin
    if (!_TransactionStart(CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000)) {
        return NULL;
    }

    // Determine the request or command type
    const char *reqType = JGetString(req, "req");
    const char *cmdType = JGetString(req, "cmd");

    // Add the user agent object only when we're doing a hub.set and only when we're
    // specifying the product UID.  The intent is that we only piggyback user agent
    // data when the host is initializing the Notecard, as opposed to every time
    // the host does a hub.set to change mode.
#ifndef NOTE_DISABLE_USER_AGENT
    if (!JIsPresent(req, "body") && (strcmp(reqType, "hub.set") == 0) && JIsPresent(req, "product")) {
        J *body = NoteUserAgent();
        if (body != NULL) {
            JAddItemToObject(req, "body", body);
        }
    }
#endif

    // Determine whether or not a response will be expected, by virtue of "cmd" being present
    bool noResponseExpected = (reqType[0] == '\0' && cmdType[0] != '\0');

    // If a reset of the module is required for any reason, do it now.
    // We must do this before acquiring lock.
    if (resetRequired) {
        if (!NoteReset()) {
            _TransactionStop();
            return NULL;
        }
    }

    if (lockNotecard) {
        _LockNote();
    }

    // Extract the ID of the request so that errors can be returned with the same ID
    uint32_t id = JGetInt(req, "id");

    // Serialize the JSON request
    char *json = JPrintUnformatted(req);
    if (json == NULL) {
        J *errRsp = errDoc(id, ERRSTR("can't convert to JSON", c_bad));
        if (lockNotecard) {
            _UnlockNote();
        }
        _TransactionStop();
        return errRsp;
    }

    // If it is a request (as opposed to a command), include a CRC so that the
    // request might be retried if it is received in a corrupted state.  (We can
    // only do this on requests because for cmd's there is no 'response channel'
    // where we can find out that the cmd failed.  Note that a Seqno is included
    // as part of the CRC data so that two identical requests occurring within the
    // modulus of seqno never are mistaken as being the same request being retried.
    uint8_t lastRequestRetries = 0;
#ifndef NOTE_C_LOW_MEM
    bool lastRequestCrcAdded = false;
    if (!noResponseExpected) {
        char *newJson = crcAdd(json, lastRequestSeqno);
        if (newJson != NULL) {
            JFree(json);
            json = newJson;
            lastRequestCrcAdded = true;
        }
    }
#endif // !NOTE_C_LOW_MEM

    // When note.add or web.* requests are used to transfer binary data, the
    // time to complete the transaction can vary depending on the size of
    // the payload and network conditions. Therefore, it's possible for
    // these transactions to timeout prematurely.
    //
    // The algorithm below, executes the following logic:
    //   - If the request is a `note.add`, set the timeout value to the
    //     value of the "milliseconds" parameter, if it exists. If it
    //     doesn't, use the "seconds" parameter. If that doesn't exist,
    //     use the standard timeout of `CARD_INTER_TRANSACTION_TIMEOUT_SEC`.
    //   - If the request is a `web.*`, follow the same logic, but instead
    //     of using the standard timeout, use the Notecard timeout of 90
    //     seconds for all `web.*` transactions.
    uint32_t transactionTimeoutMs = (CARD_INTER_TRANSACTION_TIMEOUT_SEC * 1000);

    // Interrogate the request
    if (JContainsString(req, (reqType ? "req" : "cmd"), "note.add")) {
        if (JIsPresent(req, "milliseconds")) {
            NOTE_C_LOG_DEBUG("Using `milliseconds` parameter value for "
                             "timeout.");
            transactionTimeoutMs = JGetInt(req, "milliseconds");
        } else if (JIsPresent(req, "seconds")) {
            NOTE_C_LOG_DEBUG("Using `seconds` parameter value for timeout.");
            transactionTimeoutMs = (JGetInt(req, "seconds") * 1000);
        }
    } else if (JContainsString(req, (reqType ? "req" : "cmd"), "web.")) {
        NOTE_C_LOG_DEBUG("web.* request received.");
        if (JIsPresent(req, "milliseconds")) {
            NOTE_C_LOG_DEBUG("Using `milliseconds` parameter value for "
                             "timeout.");
            transactionTimeoutMs = JGetInt(req, "milliseconds");
        } else if (JIsPresent(req, "seconds")) {
            NOTE_C_LOG_DEBUG("Using `seconds` parameter value for timeout.");
            transactionTimeoutMs = (JGetInt(req, "seconds") * 1000);
        } else {
            NOTE_C_LOG_DEBUG("No `milliseconds` or `seconds` parameter "
                             "provided. Defaulting to 90-second timeout.");
            transactionTimeoutMs = (90 * 1000);
        }
    }

    // If we're performing retries, this is where we come back to
    const char *errStr = NULL;
    char *responseJSON = NULL;
    J *rsp = NULL;
    while (true) {
        // If no retry possibility, break out
        if (lastRequestRetries > CARD_REQUEST_RETRIES_ALLOWED) {
            break;
        } else {
            // free on retry
            JDelete(rsp);
        }

        // reset variables
        rsp = NULL;
        responseJSON = NULL;

        // Trace
        if (suppressShowTransactions == 0) {
            NOTE_C_LOG_INFO(json);
        }

        // Swap NULL-terminator for newline-terminator
        const size_t jsonLen = strlen(json);
        json[jsonLen] = '\n';

        // Perform the transaction
        if (noResponseExpected) {
            errStr = _Transaction(json, (jsonLen + 1), NULL, transactionTimeoutMs);
            break;
        }
        errStr = _Transaction(json, (jsonLen + 1), &responseJSON, transactionTimeoutMs);

        // Swap newline-terminator for NULL-terminator
        json[jsonLen] = '\0';

        // If there's an I/O error on the transaction, retry
        if (errStr != NULL) {
            JFree(responseJSON);
            resetRequired = !_Reset();
            lastRequestRetries++;
            NOTE_C_LOG_WARN(ERRSTR("retrying I/O error detected by host", c_iobad));
            _DelayMs(500);
            continue;
        }

#ifndef NOTE_C_LOW_MEM
        // If we sent a CRC in the request, examine the response JSON to see if
        // it has a CRC error.  Note that the CRC is stripped from the
        // responseJSON as a side-effect of this method.
        if (lastRequestCrcAdded && crcError(responseJSON, lastRequestSeqno)) {
            JFree(responseJSON);
            errStr = "crc error {io}";
            lastRequestRetries++;
            NOTE_C_LOG_ERROR(ERRSTR("CRC error on response", c_iobad));
            _DelayMs(500);
            continue;
        }
#endif // !NOTE_C_LOW_MEM

        // See if the response JSON can't be unmarshaled, or if it contains an {io} error
        rsp = JParse(responseJSON);
        bool isBadBin = false;
        bool isIoError = false;
        if (rsp != NULL) {
            isBadBin = JContainsString(rsp, c_err, c_badbinerr);
            isIoError = JContainsString(rsp, c_err, c_ioerr) && !JContainsString(rsp, c_err, c_unsupported);
        } else {
            // Failed to parse response as JSON
            if (responseJSON == NULL) {
                NOTE_C_LOG_ERROR(ERRSTR("response expected, but response is NULL.", c_ioerr));
            } else {
#ifndef NOTE_C_LOW_MEM
                _DebugWithLevel(NOTE_C_LOG_LEVEL_ERROR, "[ERROR] ");
                _DebugWithLevel(NOTE_C_LOG_LEVEL_ERROR, "invalid JSON: ");
                _DebugWithLevel(NOTE_C_LOG_LEVEL_ERROR, responseJSON);
#else
                NOTE_C_LOG_ERROR(c_ioerr);
#endif
            }
            isIoError = true;
        }
        if (isIoError || isBadBin) {
            NOTE_C_LOG_ERROR(JGetString(rsp, c_err));
            JFree(responseJSON);

            if (isBadBin) {
                errStr = ERRSTR("notecard binary i/o error {bad-bin}{io}", c_badbinerr);
                NOTE_C_LOG_DEBUG("{bad-bin} is not elibigle for retry");
                break;
            } else {
                errStr = ERRSTR("notecard i/o error {io}", c_ioerr);
                lastRequestRetries++;
                NOTE_C_LOG_WARN(ERRSTR("retrying I/O error detected by notecard", c_iobad));
                _DelayMs(500);
                continue;
            }
        }

        // Transaction completed
        break;
    }

    // Bump the request sequence number now that we've processed this request, success or error
#ifndef NOTE_C_LOW_MEM
    lastRequestSeqno++;
#endif

    // Free the original serialized JSON request
    JFree(json);

    // If error, queue up a reset
    if (errStr != NULL) {
        JDelete(rsp);
        NoteResetRequired();
        J *errRsp = errDoc(id, errStr);
        if (lockNotecard) {
            _UnlockNote();
        }
        _TransactionStop();
        return errRsp;
    }

    // Exit with a blank object (with no err field) if no response expected
    if (noResponseExpected) {
        if (lockNotecard) {
            _UnlockNote();
        }
        _TransactionStop();
        return JCreateObject();
    }

    // Debug
    if (suppressShowTransactions == 0) {
        NOTE_C_LOG_INFO(responseJSON);
    }

    // Discard the buffer now that it has been logged
    JFree(responseJSON);

    if (lockNotecard) {
        _UnlockNote();
    }

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
 @brief Initialize or re-initialize the module.

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
uint64_t n_atoh(char *p, int maxLen)
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
NOTE_C_STATIC int32_t crc32(const void* data, size_t length)
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
NOTE_C_STATIC char *crcAdd(char *json, uint16_t seqno)
{

    // Allocate a block the size of the input json plus the size of
    // the field to be added.  Note that the input JSON ends in '"}' and
    // this will be replaced with a combination of 4 hex digits of the
    // seqno plus 8 hex digits of the CRC32, and the '}' will be
    // transformed into ',"crc":"SSSS:CCCCCCCC"}' where SSSS is the
    // seqno and CCCCCCCC is the crc32.  Note that the comma is
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
    n_htoa16(seqno, (uint8_t *) &newJson[newJsonLen]);
    newJsonLen += 4;                                        // +11
    newJson[newJsonLen++] = ':';                            // +12
    n_htoa32(crc32(json, jsonLen), &newJson[newJsonLen]);
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
 */
NOTE_C_STATIC bool crcError(char *json, uint16_t shouldBeSeqno)
{
    // Strip off any crlf for crc calculation
    size_t jsonLen = strlen(json);
    while (jsonLen > 0 && json[jsonLen-1] <= ' ') {
        jsonLen--;
    }
    // Minimum valid JSON is "{}" (2 bytes) and must end with a closing "}".
    if (jsonLen < CRC_FIELD_LENGTH+2 || json[jsonLen-1] != '}') {
        return false;
    }
    // See if it has a compliant CRC field
    size_t fieldOffset = ((jsonLen-1) - CRC_FIELD_LENGTH);
    if (memcmp(&json[fieldOffset+CRC_FIELD_NAME_OFFSET], CRC_FIELD_NAME_TEST, sizeof(CRC_FIELD_NAME_TEST)-1) != 0) {
        // If we've seen a CRC before, we should see one every time
        return notecardSupportsCrc ? true : false;
    }
    // If we get here, we've seen at least one CRC from the Notecard, so we should expect it.
    notecardSupportsCrc = true;
    char *p = &json[fieldOffset + CRC_FIELD_NAME_OFFSET + (sizeof(CRC_FIELD_NAME_TEST)-1)];
    uint16_t actualSeqno = (uint16_t) n_atoh(p, 4);
    uint32_t actualCrc32 = (uint32_t) n_atoh(p+5, 8);
    json[fieldOffset++] = '}';
    json[fieldOffset] = '\0';
    uint32_t shouldBeCrc32 = crc32(json, fieldOffset);
    return (shouldBeSeqno != actualSeqno || shouldBeCrc32 != actualCrc32);
}

#endif // !NOTE_C_LOW_MEM
