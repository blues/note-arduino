#include "mock-parameters.hpp"

NoteDebug_Parameters noteDebug_Parameters;
NoteDebugSyncStatus_Parameters noteDebugSyncStatus_Parameters;
NoteDeleteResponse_Parameters noteDeleteResponse_Parameters;
NoteNewCommand_Parameters noteNewCommand_Parameters;
NoteNewRequest_Parameters noteNewRequest_Parameters;
NoteRequest_Parameters noteRequest_Parameters;
NoteRequestResponse_Parameters noteRequestResponse_Parameters;
NoteResponseError_Parameters noteResponseError_Parameters;
NoteSetFnDebugOutput_Parameters noteSetFnDebugOutput_Parameters;
NoteSetFnDefault_Parameters noteSetFnDefault_Parameters;
NoteSetFnI2C_Parameters noteSetFnI2C_Parameters;
NoteSetFnSerial_Parameters noteSetFnSerial_Parameters;

void
MockNoteDeleteResponse (
    J *response
) {
    // Record invocation(s)
    ++noteDeleteResponse_Parameters.invoked;

    // Stash parameter(s)
    noteDeleteResponse_Parameters.response = response;
}

bool
MockNoteResponseError (
    J *rsp
) {
    // Record invocation(s)
    ++noteResponseError_Parameters.invoked;

    // Stash parameter(s)
    noteResponseError_Parameters.rsp = rsp;

    // Return user-supplied result
    return noteResponseError_Parameters.result;
}

void
NoteDebug(
    const char *message
) {
    // Record invocation(s)
    ++noteDebug_Parameters.invoked;

    // Stash parameter(s)
    noteDebug_Parameters.message = message;
    noteDebug_Parameters.message_cache = message;
}

bool
NoteDebugSyncStatus(
    int pollFrequencyMs,
    int maxLevel
) {
    // Record invocation(s)
    ++noteDebugSyncStatus_Parameters.invoked;

    // Stash parameter(s)
    noteDebugSyncStatus_Parameters.pollFrequencyMs = pollFrequencyMs;
    noteDebugSyncStatus_Parameters.maxLevel = maxLevel;

    // Return user-supplied result
    return noteDebugSyncStatus_Parameters.result;
}

J *
NoteNewCommand(
    const char *request
) {
    // Record invocation(s)
    ++noteNewCommand_Parameters.invoked;

    // Stash parameter(s)
    noteNewCommand_Parameters.request = request;
    if (request) {
        noteNewCommand_Parameters.request_cache = request;
    }

    // Return user-supplied result
    return noteNewCommand_Parameters.result;
}

J *
NoteNewRequest(
    const char *request
) {
    // Record invocation(s)
    ++noteNewRequest_Parameters.invoked;

    // Stash parameter(s)
    noteNewRequest_Parameters.request = request;
    if (request) {
        noteNewRequest_Parameters.request_cache = request;
    }

    // Return user-supplied result
    return noteNewRequest_Parameters.result;
}

bool
NoteRequest(
    J *req
) {
    // Record invocation(s)
    ++noteRequest_Parameters.invoked;

    // Stash parameter(s)
    noteRequest_Parameters.req = req;

    // Return user-supplied result
    return noteRequest_Parameters.result;
}

J *
NoteRequestResponse(
    J *req
) {
    // Record invocation(s)
    ++noteRequestResponse_Parameters.invoked;

    // Stash parameter(s)
    noteRequestResponse_Parameters.req = req;

    // Return user-supplied result
    return noteRequestResponse_Parameters.result;
}

void
NoteSetFnDebugOutput(
    debugOutputFn fn
) {
    // Record invocation(s)
    ++noteSetFnDebugOutput_Parameters.invoked;

    // Stash parameter(s)
    noteSetFnDebugOutput_Parameters.fn = fn;
}

void
NoteSetFnDefault(
    mallocFn mallocfn,
    freeFn freefn,
    delayMsFn delayfn,
    getMsFn millisfn
) {
    // Record invocation(s)
    ++noteSetFnDefault_Parameters.invoked;

    // Stash parameter(s)
    noteSetFnDefault_Parameters.mallocfn = mallocfn;
    noteSetFnDefault_Parameters.freefn = freefn;
    noteSetFnDefault_Parameters.delayfn = delayfn;
    noteSetFnDefault_Parameters.millisfn = millisfn;
}

void
NoteSetFnI2C(
    uint32_t i2caddr,
    uint32_t i2cmax,
    i2cResetFn resetfn,
    i2cTransmitFn transmitfn,
    i2cReceiveFn receivefn
) {
    // Record invocation(s)
    ++noteSetFnI2C_Parameters.invoked;

    // Stash parameter(s)
    noteSetFnI2C_Parameters.i2caddr = i2caddr;
    noteSetFnI2C_Parameters.i2cmax = i2cmax;
    noteSetFnI2C_Parameters.resetfn = resetfn;
    noteSetFnI2C_Parameters.transmitfn = transmitfn;
    noteSetFnI2C_Parameters.receivefn = receivefn;
}

void
NoteSetFnSerial(
    serialResetFn resetfn,
    serialTransmitFn writefn,
    serialAvailableFn availfn,
    serialReceiveFn readfn
) {
    // Record invocation(s)
    ++noteSetFnSerial_Parameters.invoked;

    // Stash parameter(s)
    noteSetFnSerial_Parameters.resetfn = resetfn;
    noteSetFnSerial_Parameters.writefn = writefn;
    noteSetFnSerial_Parameters.availfn = availfn;
    noteSetFnSerial_Parameters.readfn = readfn;
}
