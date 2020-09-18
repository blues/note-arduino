// Copyright 2018 Inca Roads LLC.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "n_lib.h"

// For flow tracing
static int suppressShowTransactions = 0;

// Flag that gets set whenever an error occurs that should force a reset
static bool resetRequired = true;

// Create an error response document
static J *errDoc(const char *errmsg) {
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

// Suppress or resume showing of transactions
void NoteSuspendTransactionDebug() {
	suppressShowTransactions++;
}
void NoteResumeTransactionDebug() {
	suppressShowTransactions--;
}

// Lock for mutual exclusion, not only because access to the card must be serialized, but also because
// both C++ and ArduinoJSON call malloc() which is not a thread-safe operation.
J *NoteNewRequest(const char *request) {
    J *reqdoc = JCreateObject();
    if (reqdoc != NULL)
        JAddStringToObject(reqdoc, c_req, request);
    return reqdoc;
}

// Perform a request, FREEING THE REQUEST STRUCTURE, then returning true if success and
// false if either we ran into an error such as out-of-memory or if an error was returned
// from the transaction in the c_err field.
bool NoteRequest(J *req) {
    // Exit if null request.  This allows safe execution of the form NoteRequest(NoteNewRequest("xxx"))
    if (req == NULL)
        return false;
    // Execute the transaction
    J *rsp = NoteTransaction(req);
    if (rsp == NULL) {
        JDelete(req);
        return false;
    }
    // Check for a transaction error, and exit
    bool success = JIsNullString(rsp, c_err);
    JDelete(req);
    JDelete(rsp);
    return success;
}

// Perform a request, FREEING THE REQUEST STRUCTURE, and returning a reply structure or
// NULL if there's insufficient memory.
J *NoteRequestResponse(J *req) {
    // Exit if null request.  This allows safe execution of the form NoteRequestResponse(NoteNewRequest("xxx"))
    if (req == NULL)
        return NULL;
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

// Perform a JSON transaction as a string, and return the response JSON as a string that should be freed with JFree
char *NoteRequestResponseJSON(char *reqJSON) {

    // Parse the incoming JSON string
    J *req = JParse(reqJSON);
    if (req == NULL)
        return NULL;

    // Perform the transaction and free the req
    J *rsp = NoteRequestResponse(req);
    if (rsp == NULL)
        return NULL;

    // Convert response back to JSON and delete it
    char *json = JPrintUnformatted(rsp);
    NoteDeleteResponse(rsp);
    if (json == NULL)
        return NULL;

    // Done
    return json;

}

// Initiate a transaction to the card using reqdoc, and return the result in rspdoc.  This does
// NOT free the request structure.
J *NoteTransaction(J *req) {

    // If a reset of the module is required for any reason, do it now.
    // We must do this before acquiring lock.
    if (resetRequired) {
        if (!NoteReset())
            return NULL;
    }

    // Lock
    _LockNote();

    // Serialize the JSON requet
    char *json = JPrintUnformatted(req);
    if (json == NULL) {
        J *rsp = errDoc(ERRSTR("can't convert to JSON",c_bad));
        _UnlockNote();
        return rsp;
    }
    
	if (suppressShowTransactions == 0) {
	    _Debugln(json);
	}

    // Pertform the transaction
    char *responseJSON;
    const char *errStr = _Transaction(json, &responseJSON);

    // Free the json
    JFree(json);

    // If error, queue up a reset
    if (errStr != NULL) {
		NoteResetRequired();
        J *rsp = errDoc(errStr);
        _UnlockNote();
        return rsp;
    }

    // Parse the reply from the card on the input stream
    J *rspdoc = JParse(responseJSON);
    if (rspdoc == NULL) {
        _Debug("invalid JSON: ");
		_Debug(responseJSON);
        _Free(responseJSON);
        J *rsp = errDoc(ERRSTR("unrecognized response from card",c_bad));
        _UnlockNote();
        return rsp;
    }

    // Debug
	if (suppressShowTransactions == 0) {
	    _Debugln(responseJSON);
	}

    // Discard the buffer now that it's parsed
    _Free(responseJSON);

    // Unlock
    _UnlockNote();

    // Done
    return rspdoc;
    
}

// Mark that a reset will be required before doing further I/O on a given port
void NoteResetRequired() {
    resetRequired = true;
}

// Initialize or re-initialize the module, returning false if anything fails
bool NoteReset() {
    _LockNote();
    resetRequired = !_Reset();
    _UnlockNote();
    return !resetRequired;
}

// Check to see if a notecard error is present
bool NoteErrorContains(const char *errstr, const char *errtype) {
    return (strstr(errstr, errtype) != NULL);
}

// Clean error strings out of the specified buffer
void NoteErrorClean(char *begin) {
    while (true) {
        char *end = &begin[strlen(begin)+1];
        char *beginBrace = strchr(begin, '{');
        if (beginBrace == NULL)
            break;
        if (beginBrace>begin && *(beginBrace-1) == ' ')
            beginBrace--;
        char *endBrace = strchr(beginBrace, '}');
        if (endBrace == NULL)
            break;
        char *afterBrace = endBrace + 1;
        memmove(beginBrace, afterBrace, end-afterBrace);
    }
}
