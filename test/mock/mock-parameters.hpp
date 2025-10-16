#ifndef MOCK_PARAMETERS_HPP
#define MOCK_PARAMETERS_HPP

#include <cstdint>
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

struct NoteDelayMs_Parameters {
    NoteDelayMs_Parameters(
        void
    ) :
        invoked(0),
        mock_time(false),
        ms(0)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        mock_time = false;
        ms = 0;
    }
    size_t invoked;
    bool mock_time;
    uint32_t ms;
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

struct NoteGetFnI2C_Parameters {
    NoteGetFnI2C_Parameters(
        void
    ) :
        invoked(0),
        notecardAddr(nullptr),
        maxTransmitSize(nullptr),
        resetFn(nullptr),
        transmitFn(nullptr),
        receiveFn(nullptr),
        notecardAddr_result(0),
        maxTransmitSize_result(0),
        resetFn_result(nullptr),
        transmitFn_result(nullptr),
        receiveFn_result(nullptr)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        notecardAddr = nullptr;
        maxTransmitSize = nullptr;
        resetFn = nullptr;
        transmitFn = nullptr;
        receiveFn = nullptr;
        notecardAddr_result = 0;
        maxTransmitSize_result = 0;
        resetFn_result = nullptr;
        transmitFn_result = nullptr;
        receiveFn_result = nullptr;
    }
    size_t invoked;
    uint32_t * notecardAddr;
    uint32_t * maxTransmitSize;
    i2cResetFn * resetFn;
    i2cTransmitFn * transmitFn;
    i2cReceiveFn * receiveFn;
    uint32_t notecardAddr_result;
    uint32_t maxTransmitSize_result;
    i2cResetFn resetFn_result;
    i2cTransmitFn transmitFn_result;
    i2cReceiveFn receiveFn_result;
};

struct NoteGetFnSerial_Parameters {
    NoteGetFnSerial_Parameters(
        void
    ) :
        invoked(0),
        resetFn(nullptr),
        transmitFn(nullptr),
        availFn(nullptr),
        receiveFn(nullptr),
        resetFn_result(nullptr),
        transmitFn_result(nullptr),
        availFn_result(nullptr),
        receiveFn_result(nullptr)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        resetFn = nullptr;
        transmitFn = nullptr;
        availFn = nullptr;
        receiveFn = nullptr;
        resetFn_result = nullptr;
        transmitFn_result = nullptr;
        availFn_result = nullptr;
        receiveFn_result = nullptr;
    }
    size_t invoked;
    serialResetFn * resetFn;
    serialTransmitFn * transmitFn;
    serialAvailableFn * availFn;
    serialReceiveFn * receiveFn;
    serialResetFn resetFn_result;
    serialTransmitFn transmitFn_result;
    serialAvailableFn availFn_result;
    serialReceiveFn receiveFn_result;
};

struct NoteGetMs_Parameters {
    NoteGetMs_Parameters(
        void
    ) :
        invoked(0),
        default_result(0),
        result(0)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        default_result = 0;
        result.clear();
    }
    size_t invoked;
    uint32_t default_result;
    std::vector<uint32_t> result;
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

struct NoteSetFn_Parameters {
    NoteSetFn_Parameters(
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
        i2cmtu(0),
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
        i2cmtu = 0;
        resetfn = nullptr;
        transmitfn = nullptr;
        receivefn = nullptr;
    }
    size_t invoked;
    uint32_t i2caddr;
    uint32_t i2cmtu;
    i2cResetFn resetfn;
    i2cTransmitFn transmitfn;
    i2cReceiveFn receivefn;
};

struct NoteSetFnI2CDefault_Parameters {
    NoteSetFnI2CDefault_Parameters(
        void
    ) :
        invoked(0),
        i2caddr(0),
        i2cmtu(0),
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
        i2cmtu = 0;
        resetfn = nullptr;
        transmitfn = nullptr;
        receivefn = nullptr;
    }
    size_t invoked;
    uint32_t i2caddr;
    uint32_t i2cmtu;
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

struct NoteSetFnSerialDefault_Parameters {
    NoteSetFnSerialDefault_Parameters(
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

struct NoteSetI2CAddress_Parameters {
    NoteSetI2CAddress_Parameters(
        void
    ) :
        invoked(0),
        i2caddr(0)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        i2caddr = 0;
    }
    size_t invoked;
    uint32_t i2caddr;
};

struct NoteSetI2CMtu_Parameters {
    NoteSetI2CMtu_Parameters(
        void
    ) :
        invoked(0),
        i2cmtu(0)
    { }
    void
    reset (
        void
    ) {
        invoked = 0;
        i2cmtu = 0;
    }
    size_t invoked;
    uint32_t i2cmtu;
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
extern NoteDelayMs_Parameters noteDelayMs_Parameters;
extern NoteDeleteResponse_Parameters noteDeleteResponse_Parameters;
extern NoteGetFnI2C_Parameters noteGetFnI2C_Parameters;
extern NoteGetFnSerial_Parameters noteGetFnSerial_Parameters;
extern NoteGetMs_Parameters noteGetMs_Parameters;
extern NoteNewCommand_Parameters noteNewCommand_Parameters;
extern NoteNewRequest_Parameters noteNewRequest_Parameters;
extern NoteRequest_Parameters noteRequest_Parameters;
extern NoteRequestWithRetry_Parameters noteRequestWithRetry_Parameters;
extern NoteRequestResponse_Parameters noteRequestResponse_Parameters;
extern NoteRequestResponseWithRetry_Parameters noteRequestResponseWithRetry_Parameters;
extern NoteResponseError_Parameters noteResponseError_Parameters;
extern NoteSetFnDebugOutput_Parameters noteSetFnDebugOutput_Parameters;
extern NoteSetFn_Parameters noteSetFn_Parameters;
extern NoteSetFnDefault_Parameters noteSetFnDefault_Parameters;
extern NoteSetFnI2C_Parameters noteSetFnI2C_Parameters;
extern NoteSetFnI2CDefault_Parameters noteSetFnI2CDefault_Parameters;
extern NoteSetFnI2CMutex_Parameters noteSetFnI2CMutex_Parameters;
extern NoteSetFnNoteMutex_Parameters noteSetFnNoteMutex_Parameters;
extern NoteSetFnSerial_Parameters noteSetFnSerial_Parameters;
extern NoteSetFnSerialDefault_Parameters noteSetFnSerialDefault_Parameters;
extern NoteSetFnTransaction_Parameters noteSetFnTransaction_Parameters;
extern NoteSetI2CAddress_Parameters noteSetI2CAddress_Parameters;
extern NoteSetI2CMtu_Parameters noteSetI2CMtu_Parameters;
extern NoteSetUserAgent_Parameters noteSetUserAgent_Parameters;

#endif // MOCK_PARAMETERS_HPP
