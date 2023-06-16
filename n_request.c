/*!
 * @file n_request.c
 *
 * Written by Ray Ozzie and Blues Inc. team.
 *
 * Copyright (c) 2019 Blues Inc. MIT License. Use of this source code is
 * governed by licenses granted by the copyright holder including that found in
 * the
 * <a href="https://github.com/blues/note-c/blob/master/LICENSE">LICENSE</a>
 * file.
 *
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
#ifndef NOTE_LOWMEM
static uint16_t lastRequestSeqno = 0;
#define CRC_FIELD_LENGTH		22	// ,"crc":"SSSS:CCCCCCCC"
#define	CRC_FIELD_NAME_OFFSET	1
#define	CRC_FIELD_NAME_TEST		"\"crc\":\""
#define REQUEST_RETRIES_ALLOWED 10
NOTE_C_STATIC int32_t crc32(const void* data, size_t length);
NOTE_C_STATIC char *crcAdd(char *json, uint16_t seqno);
NOTE_C_STATIC bool crcError(char *json, uint16_t shouldBeSeqno);
static bool notecardSupportsCrc = false;
#endif

/**************************************************************************/
/*!
  @brief  Create an error response document.
  @param   errmsg
  The error message from the Notecard
  @returns a `J` cJSON object with the error response.
*/
/**************************************************************************/
static J *errDoc(const char *errmsg)
{
    J *rspdoc = JCreateObject();
    if (rspdoc != NULL) {
        JAddStringToObject(rspdoc, c_err, errmsg);
    }
    if (suppressShowTransactions == 0) {
        _Debug("{\"err\":\"");
        _Debug(errmsg);
        _Debug("\"}\n");
    }
    return rspdoc;
}

/**************************************************************************/
/*!
  @brief  Suppress showing transaction details.
*/
/**************************************************************************/
void NoteSuspendTransactionDebug()
{
    suppressShowTransactions++;
}

/**************************************************************************/
/*!
  @brief  Resume showing transaction details.
*/
/**************************************************************************/
void NoteResumeTransactionDebug()
{
    suppressShowTransactions--;
}

/**************************************************************************/
/*!
  @brief  Create a new request object to populate before sending to the Notecard.
  @param   request is The name of the request, for example `hub.set`.
  @returns a `J` cJSON object with the request name pre-populated.
*/
/**************************************************************************/
J *NoteNewRequest(const char *request)
{
    J *reqdoc = JCreateObject();
    if (reqdoc != NULL) {
        JAddStringToObject(reqdoc, c_req, request);
    }
    return reqdoc;
}

/**************************************************************************/
/*!
  @brief  Create a new command object to populate before sending to the Notecard.
  @param   request is the name of the command, for example `hub.set`.
  @returns a `J` cJSON object with the request name pre-populated.
*/
/**************************************************************************/
J *NoteNewCommand(const char *request)
{
    J *reqdoc = JCreateObject();
    if (reqdoc != NULL) {
        JAddStringToObject(reqdoc, c_cmd, request);
    }
    return reqdoc;
}

/**************************************************************************/
/*!
  @brief  Send a request to the Notecard.
  Frees the request structure from memory after sending the request.
  @param   req
  The `J` cJSON request object.
  @returns a boolean. Returns `true` if successful or `false` if an error
  occurs, such as an out-of-memory or if an error was returned from
  the transaction in the c_err field. If sending a `cmd` a 'true'
  response indicates that the 'cmd` was sent without error, but
  since the Notecard sends no response it does not guarantee that
  the 'cmd' was received and processed by the Notecard.
*/
/**************************************************************************/
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

/**************************************************************************/
/*!
  @brief  Send a request to the Notecard.
  Frees the request structure from memory after sending the request.
  Retries the request for up to the specified timeoutSeconds if there is
  no response, or if the response indicates an io error.
  @param   req
  The `J` cJSON request object.
  @param   timeoutSeconds
  Upper limit for retries if there is no response, or if the
  response contains an io error.
  @returns a boolean. Returns `true` if successful or `false` if an error
  occurs, such as an out-of-memory or if an error was returned from
  the transaction in the c_err field.
*/
/**************************************************************************/
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

/**************************************************************************/
/*!
  @brief  Send a request to the Notecard and return the response.
  Frees the request structure from memory after sending the request.
  @param   req
  The `J` cJSON request object.
  @returns a `J` cJSON object with the response, or NULL if there is
  insufficient memory.
*/
/**************************************************************************/
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

/**************************************************************************/
/*!
  @brief  Send a request to the Notecard and return the response.
  Frees the request structure from memory after sending the request.
  Retries the request for up to the specified timeoutSeconds if there is
  no response, or if the response indicates an io error.
  @param   req
  The `J` cJSON request object.
  @param   timeoutSeconds
  Upper limit for retries if there is no response, or if the
  response contains an io error.
  @returns a `J` cJSON object with the response, or NULL if there is
  insufficient memory.
*/
/**************************************************************************/
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
        if ( (rsp == NULL) || JContainsString(rsp, c_err, c_ioerr)) {

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

    if (rsp == NULL) {
        return NULL;
    }

    // Return the response
    return rsp;
}

/**************************************************************************/
/*!
  @brief  Given a JSON string, send a request to the Notecard.
  Frees the request structure from memory after sending the request.
  @param   reqJSON
  A c-string containing the JSON request object.
  @returns a c-string with the JSON response from the Notecard. After
  parsed by the developer, should be freed with `JFree`.
*/
/**************************************************************************/
char *NoteRequestResponseJSON(char *reqJSON)
{

    // Parse the incoming JSON string
    J *req = JParse(reqJSON);
    if (req == NULL) {
        return NULL;
    }

    // Perform the transaction and free the req
    J *rsp = NoteRequestResponse(req);
    if (rsp == NULL) {
        return NULL;
    }

    // Convert response back to JSON and delete it
    char *json = JPrintUnformatted(rsp);
    NoteDeleteResponse(rsp);
    if (json == NULL) {
        return NULL;
    }

    // Done
    return json;

}

/**************************************************************************/
/*!
  @brief  Initiate a transaction to the Notecard and return the response.
  Does NOT free the request structure from memory after sending
  the request.
  @param   req
  The `J` cJSON request object.
  @returns a `J` cJSON object with the response, or NULL if there is
  insufficient memory.
*/
/**************************************************************************/
J *NoteTransaction(J *req)
{

    // Validate in case of memory failure of the requestor
    if (req == NULL) {
        return NULL;
    }

    // Make sure that we get access to the notecard hardware before we begin
    if (!_TransactionStart(NOTECARD_TRANSACTION_TIMEOUT_SEC*1000)) {
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

    // Lock
    _LockNote();

    // Serialize the JSON request
    char *json = JPrintUnformatted(req);
    if (json == NULL) {
        J *rsp = errDoc(ERRSTR("can't convert to JSON",c_bad));
        _UnlockNote();
        _TransactionStop();
        return rsp;
    }

    // If it is a request (as opposed to a command), include a CRC so that the
    // request might be retried if it is received in a corrupted state.  (We can
    // only do this on requests because for cmd's there is no 'response channel'
    // where we can find out that the cmd failed.  Note that a Seqno is included
    // as part of the CRC data so that two identical requests occurring within the
    // modulus of seqno never are mistaken as being the same request being retried.
#ifndef NOTE_LOWMEM
    uint8_t lastRequestRetries = 0;
    bool lastRequestCrcAdded = false;
    if (!noResponseExpected) {
        char *newJson = crcAdd(json, lastRequestSeqno);
        if (newJson != NULL) {
            JFree(json);
            json = newJson;
            lastRequestCrcAdded = true;
        }
    }
#endif // !NOTE_LOWMEM

    // If we're performing retries, this is where we come back to
    const char *errStr;
    char *responseJSON = NULL;
    while (true) {

#ifndef NOTE_LOWMEM
        // If no retry possibility, break out
        if (lastRequestRetries > REQUEST_RETRIES_ALLOWED) {
            break;
        }
#endif // !NOTE_LOWMEM


        // Trace
        if (suppressShowTransactions == 0) {
            _Debugln(json);
        }

        // Perform the transaction
        if (noResponseExpected) {
            errStr = _Transaction(json, NULL);
            break;
        }
        errStr = _Transaction(json, &responseJSON);

#ifndef NOTE_LOWMEM
        // If there's an I/O error on the transaction, retry
        if (errStr != NULL) {
            resetRequired = !_Reset();
            lastRequestRetries++;
            _Debugln("retrying I/O error detected by host");
            _DelayMs(500);
            continue;
        }

        // If we sent a CRC in the request, examine the response JSON to see if
        // it has a CRC error.  Note that the CRC is stripped from the
        // responseJSON as a side-effect of this method.
        if (lastRequestCrcAdded && crcError(responseJSON, lastRequestSeqno)) {
            _Free(responseJSON);
            errStr = "crc error {io}";
            lastRequestRetries++;
            _Debugln("CRC error on response");
            _DelayMs(500);
            continue;
        }

        // See if the response JSON can't be unmarshaled, or if it contains an {io} error
        J *rsp = JParse(responseJSON);
        bool isIoError = (rsp == NULL);
        if (rsp != NULL) {
            isIoError = NoteErrorContains(JGetString(rsp, c_err), c_ioerr);
            if (isIoError) {
                _Debug("err: ");
                _Debugln(JGetString(rsp, c_err));
            }
            JDelete(rsp);
        }
        if (isIoError) {
            _Free(responseJSON);
            errStr = "notecard i/o error {io}";
            lastRequestRetries++;
            _Debugln("retrying I/O error detected by notecard");
            _DelayMs(500);
            continue;
        }
#endif // !NOTE_LOWMEM

        // Transaction completed
        break;
    }

    // Bump the request sequence number now that we've processed this request, success or error
#ifndef NOTE_LOWMEM
    lastRequestSeqno++;
#endif

    // Free the json
    JFree(json);

    // If error, queue up a reset
    if (errStr != NULL) {
        NoteResetRequired();
        J *rsp = errDoc(errStr);
        _UnlockNote();
        _TransactionStop();
        return rsp;
    }

    // Exit with a blank object (with no err field) if no response expected
    if (noResponseExpected) {
        _UnlockNote();
        _TransactionStop();
        return JCreateObject();
    }

    // Parse the reply from the card on the input stream
    J *rspdoc = JParse(responseJSON);
    if (rspdoc == NULL) {
        if (responseJSON != NULL) {
            _Debug("invalid JSON: ");
            _Debug(responseJSON);
            _Free(responseJSON);
        }
        J *rsp = errDoc(ERRSTR("unrecognized response from card {io}",c_iobad));
        _UnlockNote();
        _TransactionStop();
        return rsp;
    }

    // Debug
    if (suppressShowTransactions == 0) {
        if (responseJSON[strlen(responseJSON)-1] == '\n') {
            _Debug(responseJSON);
        } else {
            _Debugln(responseJSON);
        }
    }

    // Discard the buffer now that it's parsed
    _Free(responseJSON);

    // Unlock
    _UnlockNote();

    // Stop the transaction
    _TransactionStop();

    // Done
    return rspdoc;

}

/**************************************************************************/
/*!
  @brief  Mark that a reset will be required before doing further I/O on
  a given port.
*/
/**************************************************************************/
void NoteResetRequired()
{
    resetRequired = true;
}

/**************************************************************************/
/*!
  @brief  Initialize or re-initialize the module, returning false if
  anything fails.
  @returns a boolean. `true` if the reset was successful, `false`, if not.
*/
/**************************************************************************/
bool NoteReset()
{
    _LockNote();
    resetRequired = !_Reset();
    _UnlockNote();
    return !resetRequired;
}

/**************************************************************************/
/*!
  @brief  Check to see if a Notecard error is present in a JSON string.
  @param   errstr
  The error string.
  @param   errtype
  The error type string.
  @returns boolean. `true` if the string contains the error provided, `false`
  if not.
*/
/**************************************************************************/
bool NoteErrorContains(const char *errstr, const char *errtype)
{
    return (strstr(errstr, errtype) != NULL);
}

/**************************************************************************/
/*!
  @brief  Clean error strings out of the specified buffer.
  @param   begin
  The string buffer to clear of error strings.
*/
/**************************************************************************/
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

#ifndef NOTE_LOWMEM
/**************************************************************************/
/*!
  @brief  n_atoh - Convert a string to hex
  @param   p
  The character data to convert
  @param   maxlen
  The maximum length of that character data
  @returns The number as converted from hex
*/
/**************************************************************************/
uint64_t n_atoh(char *p, int maxlen)
{
    uint64_t n = 0;
    char *ep = p+maxlen;
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
#endif

#ifndef NOTE_LOWMEM
/**************************************************************************/
/*!
  @brief  crc32 - Small lookup-table half-byte CRC32 algorithm
  https://create.stephan-brumme.com/crc32/#half-byte
  @param   data
  The data used to calculate the CRC
  @param   length
  The length of that data
  @returns The CRC32 of the data
*/
/**************************************************************************/
static uint32_t lut[16] = {
    0x00000000,0x1DB71064,0x3B6E20C8,0x26D930AC,0x76DC4190,0x6B6B51F4,0x4DB26158,0x5005713C,
    0xEDB88320,0xF00F9344,0xD6D6A3E8,0xCB61B38C,0x9B64C2B0,0x86D3D2D4,0xA00AE278,0xBDBDF21C
};
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
#endif

#ifndef NOTE_LOWMEM
/**************************************************************************/
/*!
  @brief  crcAdd
  @param   json
  The JSON transaction to CRC and to add the CRC to
  @param   seqno
  A 16-bit sequence number to include as a part of the CRC
  @returns new JSON with crc added, else NULL if failure
*/
/**************************************************************************/
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
    newJson[newJsonLen++] = (isEmptyObject ? ' ' : ',');	// Replace }
    newJson[newJsonLen++] = '"';							// +1
    newJson[newJsonLen++] = 'c';							// +2
    newJson[newJsonLen++] = 'r';							// +3
    newJson[newJsonLen++] = 'c';							// +4
    newJson[newJsonLen++] = '"';							// +5
    newJson[newJsonLen++] = ':';							// +6
    newJson[newJsonLen++] = '"';							// +7
    n_htoa16(seqno, (uint8_t *) &newJson[newJsonLen]);
    newJsonLen += 4;										// +11
    newJson[newJsonLen++] = ':';							// +12
    n_htoa32(crc32(json, jsonLen), &newJson[newJsonLen]);
    newJsonLen += 8;										// +20
    newJson[newJsonLen++] = '"';							// +21
    newJson[newJsonLen++] = '}';							// +22 == CRC_FIELD_LENGTH
    newJson[newJsonLen] = '\0';								// null-terminated as it came in
    return newJson;
}
#endif

#ifndef NOTE_LOWMEM
/**************************************************************************/
/*!
  @brief  crcError
  @param   json
  The JSON transaction for which the CRC should be checked.  Note
  that if a CRC field is not present in the JSON, it is considered
  a valid transaction because old Notecards do not have the code
  with which to calculate and piggyback a CRC field.  Note that the
  CRC is stripped from the input JSON regardless of whether or not
  there was an error.
  @param  shouldBeSeqno
  The expected sequence number.
  @returns True if there's an error and false otherwise.
*/
/**************************************************************************/
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
#endif
