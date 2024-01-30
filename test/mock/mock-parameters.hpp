#ifndef MOCK_PARAMETERS_HPP
#define MOCK_PARAMETERS_HPP

#include <string>
#include <vector>

#include "note-c/note.h"

#define NoteDebugWithLevel(...)
#define NoteDebugWithLevelLn(...)

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

struct JAddIntToObject_Parameters {
    JAddIntToObject_Parameters(
        void
    ) :
        invoked(0),
        default_result(nullptr)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        object.clear();
        name.clear();
        number.clear();
        result.clear();
        default_result = nullptr;
    }
    size_t invoked;
    std::vector<J *> object;
    std::vector<std::string> name;
    std::vector<JINTEGER> number;
    std::vector<J *> result;
    J *default_result;
};

struct NoteDebug_Parameters {
    NoteDebug_Parameters(
        void
    ) :
        invoked(0),
        message(nullptr)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        message = nullptr;
        message_cache.clear();
    }
    size_t invoked;
    const char *message;
    std::string message_cache;
};

struct NoteDebugSyncStatus_Parameters {
    NoteDebugSyncStatus_Parameters(
        void
    ) :
        invoked(0),
        pollFrequencyMs(0),
        maxLevel(0),
        result(false)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        pollFrequencyMs = 0;
        maxLevel = 0;
        result = false;
    }
    size_t invoked;
    int pollFrequencyMs;
    int maxLevel;
    bool result;
};

struct NoteDeleteResponse_Parameters {
    NoteDeleteResponse_Parameters(
        void
    ) :
        invoked(0),
        response(nullptr)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        response = nullptr;
    }
    size_t invoked;
    J *response;
};

struct NoteNewCommand_Parameters {
    NoteNewCommand_Parameters(
        void
    ) :
        invoked(0),
        request(nullptr),
        result(nullptr)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        request = nullptr;
        request_cache.clear();
        result = nullptr;
    }
    size_t invoked;
    const char *request;
    std::string request_cache;
    J *result;
};

struct NoteNewRequest_Parameters {
    NoteNewRequest_Parameters(
        void
    ) :
        invoked(0),
        request(nullptr),
        result(nullptr)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        request = nullptr;
        request_cache.clear();
        result = nullptr;
    }
    size_t invoked;
    const char *request;
    std::string request_cache;
    J *result;
};

struct NoteRequest_Parameters {
    NoteRequest_Parameters(
        void
    ) :
        invoked(0),
        req(nullptr),
        result(false)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        req = nullptr;
        result = false;
    }
    size_t invoked;
    J *req;
    bool result;
};

struct NoteRequestWithRetry_Parameters {
    NoteRequestWithRetry_Parameters(
        void
    ) :
        invoked(0),
        req(nullptr),
        timeoutSeconds(0),
        result(false)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        req = nullptr;
        timeoutSeconds = 0;
        result = false;
    }
    size_t invoked;
    J *req;
    uint32_t timeoutSeconds;
    bool result;
};

struct NoteRequestResponse_Parameters {
    NoteRequestResponse_Parameters(
        void
    ) :
        invoked(0),
        req(nullptr),
        result(nullptr)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        req = nullptr;
        result = nullptr;
    }
    size_t invoked;
    J *req;
    J *result;
};

struct NoteRequestResponseWithRetry_Parameters {
    NoteRequestResponseWithRetry_Parameters(
        void
    ) :
        invoked(0),
        req(nullptr),
        timeoutSeconds(0),
        result(nullptr)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        req = nullptr;
        timeoutSeconds = 0;
        result = nullptr;
    }
    size_t invoked;
    J *req;
    uint32_t timeoutSeconds;
    J *result;
};

struct NoteResponseError_Parameters {
    NoteResponseError_Parameters(
        void
    ) :
        invoked(0),
        rsp(nullptr),
        result(false)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        rsp = nullptr;
        result = false;
    }
    size_t invoked;
    J *rsp;
    bool result;
};

struct NoteSetFnDebugOutput_Parameters {
    NoteSetFnDebugOutput_Parameters(
        void
    ) :
        invoked(0),
        fn(nullptr)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        fn = nullptr;
    }
    size_t invoked;
    debugOutputFn fn;
};

struct NoteSetFnDefault_Parameters {
    NoteSetFnDefault_Parameters(
        void
    ) :
        invoked(0),
        mallocfn(nullptr),
        freefn(nullptr),
        delayfn(nullptr),
        millisfn(nullptr)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        mallocfn = nullptr;
        freefn = nullptr;
        delayfn = nullptr;
        millisfn = nullptr;
    }
    size_t invoked;
    mallocFn mallocfn;
    freeFn freefn;
    delayMsFn delayfn;
    getMsFn millisfn;
};

struct NoteSetFnI2C_Parameters {
    NoteSetFnI2C_Parameters(
        void
    ) :
        invoked(0),
        i2caddr(0),
        i2cmax(0),
        resetfn(nullptr),
        transmitfn(nullptr),
        receivefn(nullptr)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        i2caddr = 0;
        i2cmax = 0;
        resetfn = nullptr;
        transmitfn = nullptr;
        receivefn = nullptr;
    }
    size_t invoked;
    uint32_t i2caddr;
    uint32_t i2cmax;
    i2cResetFn resetfn;
    i2cTransmitFn transmitfn;
    i2cReceiveFn receivefn;
};

struct NoteSetFnI2CMutex_Parameters {
    NoteSetFnI2CMutex_Parameters(
        void
    ) :
        invoked(0),
        lockI2cFn(nullptr),
        unlockI2cFn(nullptr)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        lockI2cFn = nullptr;
        unlockI2cFn = nullptr;
    }
    size_t invoked;
    mutexFn lockI2cFn;
    mutexFn unlockI2cFn;
};

struct NoteSetFnNoteMutex_Parameters {
    NoteSetFnNoteMutex_Parameters(
        void
    ) :
        invoked(0),
        lockNoteFn(nullptr),
        unlockNoteFn(nullptr)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        lockNoteFn = nullptr;
        unlockNoteFn = nullptr;
    }
    size_t invoked;
    mutexFn lockNoteFn;
    mutexFn unlockNoteFn;
};

struct NoteSetFnSerial_Parameters {
    NoteSetFnSerial_Parameters(
        void
    ) :
        invoked(0),
        resetfn(nullptr),
        writefn(nullptr),
        availfn(nullptr),
        readfn(nullptr)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        resetfn = nullptr;
        writefn = nullptr;
        availfn = nullptr;
        readfn = nullptr;
    }
    size_t invoked;
    serialResetFn resetfn;
    serialTransmitFn writefn;
    serialAvailableFn availfn;
    serialReceiveFn readfn;
};

struct NoteSetFnTransaction_Parameters {
    NoteSetFnTransaction_Parameters(
        void
    ) :
        invoked(0),
        startfn(nullptr),
        stopfn(nullptr)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        startfn = nullptr;
        stopfn = nullptr;
    }
    size_t invoked;
    txnStartFn startfn;
    txnStopFn stopfn;
};

struct NoteSetUserAgent_Parameters {
    NoteSetUserAgent_Parameters(
        void
    ) :
        invoked(0),
        agent(nullptr)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        agent = nullptr;
        agent_cache.clear();
    }
    size_t invoked;
    const char *agent;
    std::string agent_cache;
};

extern JAddIntToObject_Parameters jAddIntToObject_Parameters;
extern NoteDebug_Parameters noteDebug_Parameters;
extern NoteDebugSyncStatus_Parameters noteDebugSyncStatus_Parameters;
extern NoteDeleteResponse_Parameters noteDeleteResponse_Parameters;
extern NoteNewCommand_Parameters noteNewCommand_Parameters;
extern NoteNewRequest_Parameters noteNewRequest_Parameters;
extern NoteRequest_Parameters noteRequest_Parameters;
extern NoteRequestWithRetry_Parameters noteRequestWithRetry_Parameters;
extern NoteRequestResponse_Parameters noteRequestResponse_Parameters;
extern NoteRequestResponseWithRetry_Parameters noteRequestResponseWithRetry_Parameters;
extern NoteResponseError_Parameters noteResponseError_Parameters;
extern NoteSetFnDebugOutput_Parameters noteSetFnDebugOutput_Parameters;
extern NoteSetFnDefault_Parameters noteSetFnDefault_Parameters;
extern NoteSetFnI2C_Parameters noteSetFnI2C_Parameters;
extern NoteSetFnI2CMutex_Parameters noteSetFnI2CMutex_Parameters;
extern NoteSetFnNoteMutex_Parameters noteSetFnNoteMutex_Parameters;
extern NoteSetFnSerial_Parameters noteSetFnSerial_Parameters;
extern NoteSetFnTransaction_Parameters noteSetFnTransaction_Parameters;
extern NoteSetUserAgent_Parameters noteSetUserAgent_Parameters;

#endif // MOCK_PARAMETERS_HPP
