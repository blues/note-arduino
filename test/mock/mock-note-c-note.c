#include "mock-parameters.hpp"

JAddIntToObject_Parameters jAddIntToObject_Parameters;
NoteDebug_Parameters noteDebug_Parameters;
NoteDebugSyncStatus_Parameters noteDebugSyncStatus_Parameters;
NoteDelayMs_Parameters noteDelayMs_Parameters;
NoteDeleteResponse_Parameters noteDeleteResponse_Parameters;
NoteGetFnI2C_Parameters noteGetFnI2C_Parameters;
NoteGetFnSerial_Parameters noteGetFnSerial_Parameters;
NoteGetMs_Parameters noteGetMs_Parameters;
NoteNewCommand_Parameters noteNewCommand_Parameters;
NoteNewRequest_Parameters noteNewRequest_Parameters;
NoteRequest_Parameters noteRequest_Parameters;
NoteRequestWithRetry_Parameters noteRequestWithRetry_Parameters;
NoteRequestResponse_Parameters noteRequestResponse_Parameters;
NoteRequestResponseWithRetry_Parameters noteRequestResponseWithRetry_Parameters;
NoteResponseError_Parameters noteResponseError_Parameters;
NoteSetFnDebugOutput_Parameters noteSetFnDebugOutput_Parameters;
NoteSetFn_Parameters noteSetFn_Parameters;
NoteSetFnDefault_Parameters noteSetFnDefault_Parameters;
NoteSetFnI2C_Parameters noteSetFnI2C_Parameters;
NoteSetFnI2CDefault_Parameters noteSetFnI2CDefault_Parameters;
NoteSetFnI2CMutex_Parameters noteSetFnI2CMutex_Parameters;
NoteSetFnNoteMutex_Parameters noteSetFnNoteMutex_Parameters;
NoteSetFnSerial_Parameters noteSetFnSerial_Parameters;
NoteSetFnSerialDefault_Parameters noteSetFnSerialDefault_Parameters;
NoteSetFnTransaction_Parameters noteSetFnTransaction_Parameters;
NoteSetI2CAddress_Parameters noteSetI2CAddress_Parameters;
NoteSetI2CMtu_Parameters noteSetI2CMtu_Parameters;
NoteSetUserAgent_Parameters noteSetUserAgent_Parameters;

J *
JAddIntToObject (
    J * const object_,
    const char * const name_,
    const JINTEGER number_
) {
    // Record invocation(s)
    ++jAddIntToObject_Parameters.invoked;

    // Stash parameter(s)
    jAddIntToObject_Parameters.object.push_back(object_);
    jAddIntToObject_Parameters.name.push_back(name_);
    jAddIntToObject_Parameters.number.push_back(number_);

    // Return user-supplied result
    if (jAddIntToObject_Parameters.result.size() < jAddIntToObject_Parameters.invoked) {
        return jAddIntToObject_Parameters.default_result;
    } else {
        return jAddIntToObject_Parameters.result[(jAddIntToObject_Parameters.invoked - 1)];
    }
}

void
MockNoteDeleteResponse (
    J * response_
) {
    // Record invocation(s)
    ++noteDeleteResponse_Parameters.invoked;

    // Stash parameter(s)
    noteDeleteResponse_Parameters.response = response_;
}

bool
MockNoteResponseError (
    J * rsp_
) {
    // Record invocation(s)
    ++noteResponseError_Parameters.invoked;

    // Stash parameter(s)
    noteResponseError_Parameters.rsp = rsp_;

    // Return user-supplied result
    return noteResponseError_Parameters.result;
}

void
NoteDebug(
    const char * message_
) {
    // Record invocation(s)
    ++noteDebug_Parameters.invoked;

    // Stash parameter(s)
    noteDebug_Parameters.message = message_;
    if (message_) {
        noteDebug_Parameters.message_cache = message_;
    }
}

bool
NoteDebugSyncStatus(
    int poll_frequency_ms_,
    int max_level_
) {
    // Record invocation(s)
    ++noteDebugSyncStatus_Parameters.invoked;

    // Stash parameter(s)
    noteDebugSyncStatus_Parameters.pollFrequencyMs = poll_frequency_ms_;
    noteDebugSyncStatus_Parameters.maxLevel = max_level_;

    // Return user-supplied result
    return noteDebugSyncStatus_Parameters.result;
}

void
NoteDelayMs (
    uint32_t ms_
) {
    // Record invocation(s)
    ++noteDelayMs_Parameters.invoked;

    // Stash parameter(s)
    noteDelayMs_Parameters.ms = ms_;

    // Emulate the passing time for timeout loops
    if (noteDelayMs_Parameters.mock_time) {
        noteGetMs_Parameters.default_result += ms_;
    }
}

void NoteGetFnI2C(uint32_t *notecardAddr, uint32_t *maxTransmitSize,
                  i2cResetFn *resetFn, i2cTransmitFn *transmitFn,
                  i2cReceiveFn *receiveFn
) {
    // Record invocation(s)
    ++noteGetFnI2C_Parameters.invoked;

    // Stash parameter(s)
    noteGetFnI2C_Parameters.notecardAddr = notecardAddr;
    noteGetFnI2C_Parameters.maxTransmitSize = maxTransmitSize;
    noteGetFnI2C_Parameters.resetFn = resetFn;
    noteGetFnI2C_Parameters.transmitFn = transmitFn;
    noteGetFnI2C_Parameters.receiveFn = receiveFn;

    // Return user-supplied result
    if (notecardAddr) {
        *notecardAddr = noteGetFnI2C_Parameters.notecardAddr_result;
    }
    if (maxTransmitSize) {
        *maxTransmitSize = noteGetFnI2C_Parameters.maxTransmitSize_result;
    }
    if (resetFn) {
        *resetFn = noteGetFnI2C_Parameters.resetFn_result;
    }
    if (transmitFn) {
        *transmitFn = noteGetFnI2C_Parameters.transmitFn_result;
    }
    if (receiveFn) {
        *receiveFn = noteGetFnI2C_Parameters.receiveFn_result;
    }
}

void NoteGetFnSerial(serialResetFn *resetFn, serialTransmitFn *transmitFn,
                     serialAvailableFn *availFn, serialReceiveFn *receiveFn
) {
    // Record invocation(s)
    ++noteGetFnSerial_Parameters.invoked;

    // Stash parameter(s)
    noteGetFnSerial_Parameters.resetFn = resetFn;
    noteGetFnSerial_Parameters.transmitFn = transmitFn;
    noteGetFnSerial_Parameters.availFn = availFn;
    noteGetFnSerial_Parameters.receiveFn = receiveFn;

    // Return user-supplied result
    if (resetFn) {
        *resetFn = noteGetFnSerial_Parameters.resetFn_result;
    }
    if (transmitFn) {
        *transmitFn = noteGetFnSerial_Parameters.transmitFn_result;
    }
    if (availFn) {
        *availFn = noteGetFnSerial_Parameters.availFn_result;
    }
    if (receiveFn) {
        *receiveFn = noteGetFnSerial_Parameters.receiveFn_result;
    }
}

uint32_t
NoteGetMs(
    void
) {
    // Record invocation(s)
    ++noteGetMs_Parameters.invoked;

    // Return user-supplied result
    if (noteGetMs_Parameters.result.size() < noteGetMs_Parameters.invoked) {
        return noteGetMs_Parameters.default_result;
    } else {
        return noteGetMs_Parameters.result[(noteGetMs_Parameters.invoked - 1)];
    }
}

J *
NoteNewCommand(
    const char * request_
) {
    // Record invocation(s)
    ++noteNewCommand_Parameters.invoked;

    // Stash parameter(s)
    noteNewCommand_Parameters.request = request_;
    if (request_) {
        noteNewCommand_Parameters.request_cache = request_;
    }

    // Return user-supplied result
    return noteNewCommand_Parameters.result;
}

J *
NoteNewRequest(
    const char * request_
) {
    // Record invocation(s)
    ++noteNewRequest_Parameters.invoked;

    // Stash parameter(s)
    noteNewRequest_Parameters.request = request_;
    if (request_) {
        noteNewRequest_Parameters.request_cache = request_;
    }

    // Return user-supplied result
    return noteNewRequest_Parameters.result;
}

bool
NoteRequest(
    J * req_
) {
    // Record invocation(s)
    ++noteRequest_Parameters.invoked;

    // Stash parameter(s)
    noteRequest_Parameters.req = req_;

    // Return user-supplied result
    return noteRequest_Parameters.result;
}

bool
NoteRequestWithRetry(
    J * req_,
    uint32_t timeoutSeconds_
) {
    // Record invocation(s)
    ++noteRequestWithRetry_Parameters.invoked;

    // Stash parameter(s)
    noteRequestWithRetry_Parameters.req = req_;
    noteRequestWithRetry_Parameters.timeoutSeconds = timeoutSeconds_;

    // Return user-supplied result
    return noteRequestWithRetry_Parameters.result;
}

J *
NoteRequestResponse(
    J * req_
) {
    // Record invocation(s)
    ++noteRequestResponse_Parameters.invoked;

    // Stash parameter(s)
    noteRequestResponse_Parameters.req = req_;

    // Return user-supplied result
    return noteRequestResponse_Parameters.result;
}

J *
NoteRequestResponseWithRetry(
    J * req_,
    uint32_t timeoutSeconds_
) {
    // Record invocation(s)
    ++noteRequestResponseWithRetry_Parameters.invoked;

    // Stash parameter(s)
    noteRequestResponseWithRetry_Parameters.req = req_;
    noteRequestResponseWithRetry_Parameters.timeoutSeconds = timeoutSeconds_;

    // Return user-supplied result
    return noteRequestResponseWithRetry_Parameters.result;
}

void
NoteSetFnDebugOutput(
    debugOutputFn fn_
) {
    // Record invocation(s)
    ++noteSetFnDebugOutput_Parameters.invoked;

    // Stash parameter(s)
    noteSetFnDebugOutput_Parameters.fn = fn_;
}

void
NoteSetFn(
    mallocFn malloc_fn_,
    freeFn free_fn_,
    delayMsFn delay_fn_,
    getMsFn millis_fn_
) {
    // Record invocation(s)
    ++noteSetFn_Parameters.invoked;

    // Stash parameter(s)
    noteSetFn_Parameters.mallocfn = malloc_fn_;
    noteSetFn_Parameters.freefn = free_fn_;
    noteSetFn_Parameters.delayfn = delay_fn_;
    noteSetFn_Parameters.millisfn = millis_fn_;
}

void
NoteSetFnDefault(
    mallocFn malloc_fn_,
    freeFn free_fn_,
    delayMsFn delay_fn_,
    getMsFn millis_fn_
) {
    // Record invocation(s)
    ++noteSetFnDefault_Parameters.invoked;

    // Stash parameter(s)
    noteSetFnDefault_Parameters.mallocfn = malloc_fn_;
    noteSetFnDefault_Parameters.freefn = free_fn_;
    noteSetFnDefault_Parameters.delayfn = delay_fn_;
    noteSetFnDefault_Parameters.millisfn = millis_fn_;
}

void
NoteSetFnI2C(
    uint32_t i2c_addr_,
    uint32_t i2c_mtu_,
    i2cResetFn reset_fn_,
    i2cTransmitFn transmit_fn_,
    i2cReceiveFn receive_fn_
) {
    // Record invocation(s)
    ++noteSetFnI2C_Parameters.invoked;

    // Stash parameter(s)
    noteSetFnI2C_Parameters.i2caddr = i2c_addr_;
    noteSetFnI2C_Parameters.i2cmtu = i2c_mtu_;
    noteSetFnI2C_Parameters.resetfn = reset_fn_;
    noteSetFnI2C_Parameters.transmitfn = transmit_fn_;
    noteSetFnI2C_Parameters.receivefn = receive_fn_;
}

void
NoteSetFnI2CDefault(
    uint32_t i2c_addr_,
    uint32_t i2c_mtu_,
    i2cResetFn reset_fn_,
    i2cTransmitFn transmit_fn_,
    i2cReceiveFn receive_fn_
) {
    // Record invocation(s)
    ++noteSetFnI2CDefault_Parameters.invoked;

    // Stash parameter(s)
    noteSetFnI2CDefault_Parameters.i2caddr = i2c_addr_;
    noteSetFnI2CDefault_Parameters.i2cmtu = i2c_mtu_;
    noteSetFnI2CDefault_Parameters.resetfn = reset_fn_;
    noteSetFnI2CDefault_Parameters.transmitfn = transmit_fn_;
    noteSetFnI2CDefault_Parameters.receivefn = receive_fn_;
}

void
NoteSetFnI2CMutex(
    mutexFn lockI2cFn_,
    mutexFn unlockI2cFn_
) {
    // Record invocation(s)
    ++noteSetFnI2CMutex_Parameters.invoked;

    // Stash parameter(s)
    noteSetFnI2CMutex_Parameters.lockI2cFn = lockI2cFn_;
    noteSetFnI2CMutex_Parameters.unlockI2cFn = unlockI2cFn_;
}

void
NoteSetFnNoteMutex(
    mutexFn lockNoteFn_,
    mutexFn unlockNoteFn_
) {
    // Record invocation(s)
    ++noteSetFnNoteMutex_Parameters.invoked;

    // Stash parameter(s)
    noteSetFnNoteMutex_Parameters.lockNoteFn = lockNoteFn_;
    noteSetFnNoteMutex_Parameters.unlockNoteFn = unlockNoteFn_;
}

void
NoteSetFnSerial(
    serialResetFn reset_fn_,
    serialTransmitFn write_fn_,
    serialAvailableFn avail_fn_,
    serialReceiveFn read_fn_
) {
    // Record invocation(s)
    ++noteSetFnSerial_Parameters.invoked;

    // Stash parameter(s)
    noteSetFnSerial_Parameters.resetfn = reset_fn_;
    noteSetFnSerial_Parameters.writefn = write_fn_;
    noteSetFnSerial_Parameters.availfn = avail_fn_;
    noteSetFnSerial_Parameters.readfn = read_fn_;
}

void
NoteSetFnSerialDefault(
    serialResetFn reset_fn_,
    serialTransmitFn write_fn_,
    serialAvailableFn avail_fn_,
    serialReceiveFn read_fn_
) {
    // Record invocation(s)
    ++noteSetFnSerialDefault_Parameters.invoked;

    // Stash parameter(s)
    noteSetFnSerialDefault_Parameters.resetfn = reset_fn_;
    noteSetFnSerialDefault_Parameters.writefn = write_fn_;
    noteSetFnSerialDefault_Parameters.availfn = avail_fn_;
    noteSetFnSerialDefault_Parameters.readfn = read_fn_;
}

void
NoteSetFnTransaction(
    txnStartFn start_fn_,
    txnStopFn stop_fn_
) {
    // Record invocation(s)
    ++noteSetFnTransaction_Parameters.invoked;

    // Stash parameter(s)
    noteSetFnTransaction_Parameters.startfn = start_fn_;
    noteSetFnTransaction_Parameters.stopfn = stop_fn_;
}

void
NoteSetI2CAddress(
    uint32_t i2c_addr_
) {
    // Record invocation(s)
    ++noteSetI2CAddress_Parameters.invoked;

    // Stash parameter(s)
    noteSetI2CAddress_Parameters.i2caddr = i2c_addr_;
}

void
NoteSetI2CMtu(
    uint32_t i2c_mtu_
) {
    // Record invocation(s)
    ++noteSetI2CMtu_Parameters.invoked;

    // Stash parameter(s)
    noteSetI2CMtu_Parameters.i2cmtu = i2c_mtu_;
}

void
NoteSetUserAgent(
    char * agent_
) {
    // Record invocation(s)
    ++noteSetUserAgent_Parameters.invoked;

    // Stash parameter(s)
    noteSetUserAgent_Parameters.agent = agent_;
    if (agent_) {
        noteSetUserAgent_Parameters.agent_cache = agent_;
    }
}
