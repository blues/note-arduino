// Copyright 2018 Inca Roads LLC.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "n_lib.h"

// For flow tracing
#define SHOW_TRANSACTIONS

// Flag that gets set whenever an error occurs that should force a reset
static bool resetRequired = true;

// Create an error response document
static J *errDoc(const char *errmsg) {
    J *rspdoc = JCreateObject();
    if (rspdoc != NULL)
        JAddStringToObject(rspdoc, "err", errmsg);
#ifdef SHOW_TRANSACTIONS
    _Debug("{\"err\":\"");
	_Debug(errmsg);
	_Debug("\"}\n");
#endif
    return rspdoc;
}

// Lock for mutual exclusion, not only because access to the card must be serialized, but also because
// both C++ and ArduinoJSON call malloc() which is not a thread-safe operation.
J *NoteNewRequest(const char *request) {
    J *reqdoc = JCreateObject();
    if (reqdoc != NULL)
        JAddStringToObject(reqdoc, "req", request);
    return reqdoc;
}

// Perform a request, FREEING THE REQUEST STRUCTURE, then returning true if success and
// false if either we ran into an error such as out-of-memory or if an error was returned
// from the transaction in the "err" field.
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
    bool success = JIsNullString(rsp, "err");
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
        J *rsp = errDoc("can't convert to JSON");
        _UnlockNote();
        return rsp;
    }
    
#ifdef SHOW_TRANSACTIONS
    _Debug(json);
	_Debug("\n");
#endif

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
        _Free(responseJSON);
        _Debug("unable to parse response JSON:\n");
		_Debug(responseJSON);
        J *rsp = errDoc("unrecognized response from card");
        _UnlockNote();
        return rsp;
    }

    // Debug
#ifdef SHOW_TRANSACTIONS
    _Debug(responseJSON);
	_Debug("\n");
#endif

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
    resetRequired = !_NoteReset();
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
