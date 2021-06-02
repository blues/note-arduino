#ifndef MOCK_PARAMETERS_HPP
#define MOCK_PARAMETERS_HPP

#include "note-c/note.h"

#ifdef NoteDeleteResponse
#undef NoteDeleteResponse
#endif

void MockNoteDeleteResponse(J*);
#define NoteDeleteResponse(x) MockNoteDeleteResponse(x)

#ifdef NoteResponseError
#undef NoteResponseError
#endif

bool MockNoteResponseError(J*);
#define NoteResponseError(x) MockNoteResponseError(x)

struct NoteDebug_Parameters {
    NoteDebug_Parameters(
        void
    ) :
        message(nullptr)
    { }
    const char *message;
};

struct NoteDebugSyncStatus_Parameters {
    NoteDebugSyncStatus_Parameters(
        void
    ) :
        pollFrequencyMs(0),
        maxLevel(0),
        result(false)
    { }

    int pollFrequencyMs;
    int maxLevel;
    bool result;
};

struct NoteDeleteResponse_Parameters {
    NoteDeleteResponse_Parameters(
        void
    ) :
        response(nullptr)
    { }
    J *response;
};

struct NoteNewCommand_Parameters {
    NoteNewCommand_Parameters(
        void
    ) :
        request(nullptr),
        result(nullptr)
    { }
    const char *request;
    J *result;
};

struct NoteNewRequest_Parameters {
    NoteNewRequest_Parameters(
        void
    ) :
        request(nullptr),
        result(nullptr)
    { }
    const char *request;
    J *result;
};

struct NoteRequest_Parameters {
    NoteRequest_Parameters(
        void
    ) :
        req(nullptr),
        result(false)
    { }
    J *req;
    bool result;
};

struct NoteRequestResponse_Parameters {
    NoteRequestResponse_Parameters(
        void
    ) :
        req(nullptr),
        result(nullptr)
    { }
    J *req;
    J *result;
};

struct NoteResponseError_Parameters {
    NoteResponseError_Parameters(
        void
    ) :
        rsp(nullptr)
    { }
    J *rsp;
    bool result;
};

struct NoteSetFnDebugOutput_Parameters {
    NoteSetFnDebugOutput_Parameters(
        void
    ) :
        fn(nullptr)
    { }
    debugOutputFn fn;
};

struct NoteSetFnDefault_Parameters {
    NoteSetFnDefault_Parameters(
        void
    ) :
        mallocfn(nullptr),
        freefn(nullptr),
        delayfn(nullptr),
        millisfn(nullptr)
    { }
    mallocFn mallocfn;
    freeFn freefn;
    delayMsFn delayfn;
    getMsFn millisfn;
};

struct NoteSetFnI2C_Parameters {
    NoteSetFnI2C_Parameters(
        void
    ) :
        i2caddr(0),
        i2cmax(0),
        resetfn(nullptr),
        transmitfn(nullptr),
        receivefn(nullptr)
    { }
    uint32_t i2caddr;
    uint32_t i2cmax;
    i2cResetFn resetfn;
    i2cTransmitFn transmitfn;
    i2cReceiveFn receivefn;
};

struct NoteSetFnSerial_Parameters {
    NoteSetFnSerial_Parameters(
        void
    ) :
        resetfn(nullptr),
        writefn(nullptr),
        availfn(nullptr),
        readfn(nullptr)
    { }
    serialResetFn resetfn;
    serialTransmitFn writefn;
    serialAvailableFn availfn;
    serialReceiveFn readfn;
};

extern NoteDebug_Parameters noteDebug_Parameters;
extern NoteDebugSyncStatus_Parameters noteDebugSyncStatus_Parameters;
extern NoteDeleteResponse_Parameters noteDeleteResponse_Parameters;
extern NoteNewCommand_Parameters noteNewCommand_Parameters;
extern NoteNewRequest_Parameters noteNewRequest_Parameters;
extern NoteRequest_Parameters noteRequest_Parameters;
extern NoteRequestResponse_Parameters noteRequestResponse_Parameters;
extern NoteResponseError_Parameters noteResponseError_Parameters;
extern NoteSetFnDebugOutput_Parameters noteSetFnDebugOutput_Parameters;
extern NoteSetFnDefault_Parameters noteSetFnDefault_Parameters;
extern NoteSetFnI2C_Parameters noteSetFnI2C_Parameters;
extern NoteSetFnSerial_Parameters noteSetFnSerial_Parameters;

#endif // MOCK_PARAMETERS_HPP
