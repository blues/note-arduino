/*!
 * @file NotecardPing.cpp
 *
 * Implementation of `Notecard::ping()` — ping the Notecard to verify
 * reachability and, on a serial link, negotiate the AUX port's baud rate
 * to match the rate passed to `Notecard::begin()` if necessary.
 *
 * Written by Ray Ozzie and Blues Inc. team.
 *
 * Copyright (c) 2019 Blues Inc. MIT License. Use of this source code is
 * governed by licenses granted by the copyright holder including that found in
 * the
 * <a href="https://github.com/blues/note-arduino/blob/master/LICENSE">LICENSE</a>
 * file.
 */

#include "Notecard.h"

#include <stdio.h>
#include <string.h>

#ifndef NOTE_MOCK
#include <note-c/note.h>
#else
#include "mock/mock-arduino.hpp"
#include "mock/mock-parameters.hpp"
#endif

// Local formatting helpers that route through note-c's level-filtered log
// macros. The standard NOTE_C_LOG_* macros accept only literal/string-pointer
// arguments, so we format into a stack buffer first and then pass it through.
// This keeps ping() output consistent with logging elsewhere in note-c and
// still honors runtime log-level filtering via NoteSetLogLevel().
#define PING_LOG_BUF_LEN 96

#define PING_LOG_INFO(...) do {                                     \
    char _pingBuf[PING_LOG_BUF_LEN];                                \
    snprintf(_pingBuf, sizeof(_pingBuf), __VA_ARGS__);              \
    NOTE_C_LOG_INFO(_pingBuf);                                      \
} while (0)

#define PING_LOG_WARN(...) do {                                     \
    char _pingBuf[PING_LOG_BUF_LEN];                                \
    snprintf(_pingBuf, sizeof(_pingBuf), __VA_ARGS__);              \
    NOTE_C_LOG_WARN(_pingBuf);                                      \
} while (0)

#define PING_LOG_DEBUG(...) do {                                    \
    char _pingBuf[PING_LOG_BUF_LEN];                                \
    snprintf(_pingBuf, sizeof(_pingBuf), __VA_ARGS__);              \
    NOTE_C_LOG_DEBUG(_pingBuf);                                     \
} while (0)

namespace
{

// Baud rates to scan when the initial ping fails on a serial link. Ordered
// by prevalence on modern Notecard deployments: higher rates first (most
// common for aux-port configurations), then 9600 as the legacy default.
//
// The caller's desired rate is skipped during the scan because it was
// already tried by the initial ping at the top of ping().
const uint32_t kScanBaudRates[] = {
    115200,
    230400,
    460800,
    921600,
    9600,
};
const size_t kScanBaudRateCount = sizeof(kScanBaudRates) / sizeof(kScanBaudRates[0]);

// Small settling delay after receiving the response to `card.aux.serial`
// before switching the host UART to the new rate. The Notecard sends the
// response at the old rate and then commits the new rate; this pause lets
// the final bytes clear the wire before we switch.
const uint32_t kAuxSerialSettleMs = 20;

// After switching the host UART to the new rate, the Notecard may need a
// brief period to fully initialize its port at the new rate before it can
// service a fresh request. We retry the verification ping up to
// kPostReconfigPingAttempts times with kPostReconfigPingDelayMs between
// attempts to accommodate this.
const uint8_t  kPostReconfigPingAttempts = 5;
const uint32_t kPostReconfigPingDelayMs  = 250;

} // namespace

bool Notecard::ping(void)
{
    // I2C: the baud rate is meaningless on I2C, so just ping for
    // connectivity to catch wiring/address problems and return the result.
    if (NoteGetActiveInterface() == NOTE_C_INTERFACE_I2C) {
        NOTE_C_LOG_DEBUG("ping: starting (i2c)");
        const bool ok = NotePing();
        if (ok) {
            NOTE_C_LOG_INFO("ping: Notecard reachable over i2c");
        } else {
            NOTE_C_LOG_WARN("ping: Notecard unreachable over i2c");
        }
        return ok;
    }

    // Serial: must have a NoteSerial singleton to proceed.
    NoteSerial *ns = getNoteSerial();
    if (ns == nullptr) {
        NOTE_C_LOG_WARN("ping: no NoteSerial singleton (did you call Notecard::begin?)");
        return false;
    }

    const size_t desiredRate = ns->getBaudRate();
    PING_LOG_DEBUG("ping: starting (serial, %lu baud)", (unsigned long)desiredRate);

    // Step 1: ping at the currently-configured host UART rate. If the
    // Notecard is already reachable, we are done and nothing else changes.
    if (NotePing()) {
        PING_LOG_INFO("ping: Notecard reachable at %lu baud", (unsigned long)desiredRate);
        return true;
    }

    PING_LOG_INFO("ping: initial probe at %lu baud failed; scanning baud rates",
                  (unsigned long)desiredRate);

    // Step 2: scan through the known aux-port rates looking for one at
    // which the Notecard will talk to us. The desired rate is skipped
    // because it was already tried by the initial ping above. On
    // success, the host UART is left at the discovered rate (i.e.,
    // whatever `ns->setBaudRate` was last called with) and we fall
    // through to step 3.
    bool found = false;
    size_t discoveredRate = 0;
    for (size_t i = 0; i < kScanBaudRateCount; ++i) {
        const size_t rate = kScanBaudRates[i];
        if (rate == desiredRate) {
            continue;
        }
        PING_LOG_DEBUG("ping: trying %lu baud", (unsigned long)rate);
        if (!ns->setBaudRate(rate)) {
            // Implementation can't change baud rate at runtime — nothing we
            // can do but give up.
            NOTE_C_LOG_WARN("ping: host Serial does not support runtime baud rate change");
            return false;
        }
        if (NotePing()) {
            found = true;
            discoveredRate = rate;
            break;
        }
    }

    if (!found) {
        // Couldn't reach the Notecard at any scanned rate. Restore the
        // host UART to the caller's originally-requested rate and fail.
        PING_LOG_WARN("ping: no response at any scanned rate; restoring host UART to %lu baud",
                      (unsigned long)desiredRate);
        ns->setBaudRate(desiredRate);
        return false;
    }

    PING_LOG_INFO("ping: found Notecard at %lu baud", (unsigned long)discoveredRate);

    // Step 3: comms is working at the discovered rate. Query `card.io` with
    // mode:"port" to determine whether we are on the aux port or on a
    // non-configurable port (e.g., usb, notecard).
    J *req = NoteNewRequest("card.io");
    if (req == nullptr) {
        NOTE_C_LOG_WARN("ping: failed to allocate card.io request");
        return false;
    }
    JAddStringToObject(req, "mode", "port");
    J *rsp = NoteRequestResponse(req);
    if (rsp == nullptr) {
        NOTE_C_LOG_WARN("ping: card.io request failed");
        return false;
    }
    const bool ioHasErr = !JIsNullString(rsp, "err");
    const char *portStatus = JGetString(rsp, "status");
    const bool isAux = (strcmp(portStatus, "aux") == 0);
    PING_LOG_INFO("ping: card.io reports port '%s'", portStatus);
    JDelete(rsp);

    if (ioHasErr) {
        // Unexpected: NotePing proved connectivity at the discovered rate,
        // but card.io returned an error. We cannot determine which port
        // we are on, so we cannot safely reconfigure. Return false and
        // leave the host UART at the discovered rate.
        NOTE_C_LOG_WARN("ping: card.io returned an error; aborting");
        return false;
    }

    if (!isAux) {
        // Connected to a non-aux port (e.g., usb or notecard). Its rate
        // cannot be changed, so the host UART is correctly set to the
        // discovered rate and comms is working. Return true even though
        // the host UART may differ from what was passed to begin().
        PING_LOG_INFO("ping: non-aux port; host UART stays at %lu baud",
                      (unsigned long)discoveredRate);
        return true;
    }

    // Step 4: aux port confirmed. Reconfigure it to the desired rate.
    PING_LOG_INFO("ping: reconfiguring aux port from %lu to %lu baud",
                  (unsigned long)discoveredRate, (unsigned long)desiredRate);
    req = NoteNewRequest("card.aux.serial");
    if (req == nullptr) {
        NOTE_C_LOG_WARN("ping: failed to allocate card.aux.serial request");
        return false;
    }
    JAddStringToObject(req, "mode", "req");
    JAddIntToObject(req, "rate", (JINTEGER)desiredRate);
    rsp = NoteRequestResponse(req);
    if (rsp == nullptr) {
        // Transport failure. Host UART stays at the discovered rate so
        // the caller can still reach the Notecard.
        PING_LOG_WARN("ping: card.aux.serial request failed; host UART remains at %lu baud",
                      (unsigned long)discoveredRate);
        return false;
    }
    const bool auxErr = !JIsNullString(rsp, "err");
    JDelete(rsp);
    if (auxErr) {
        // Notecard rejected the rate change. Host UART stays at the
        // discovered rate (per the design decision for option (a)) so
        // the caller can still reach the Notecard.
        PING_LOG_WARN("ping: aux rate change rejected; host UART remains at %lu baud",
                      (unsigned long)discoveredRate);
        return false;
    }

    // Response was received at the old (discovered) rate; the Notecard
    // has now committed to desiredRate. Let residual bytes clear the
    // wire, then switch the host UART to match.
    NoteDelayMs(kAuxSerialSettleMs);
    if (!ns->setBaudRate(desiredRate)) {
        NOTE_C_LOG_WARN("ping: failed to switch host UART after aux reconfig");
        return false;
    }

    // Verify with a retry loop. The Notecard may need a short initialization
    // period after committing to the new aux-port rate before it can service
    // a fresh request; a single probe is not reliable here.
    for (uint8_t attempt = 1; attempt <= kPostReconfigPingAttempts; ++attempt) {
        if (NotePing()) {
            PING_LOG_INFO("ping: aux port now at %lu baud; verified (attempt %u)",
                          (unsigned long)desiredRate, (unsigned)attempt);
            return true;
        }
        if (attempt < kPostReconfigPingAttempts) {
            PING_LOG_DEBUG("ping: post-reconfig probe %u/%u failed; retrying in %lu ms",
                           (unsigned)attempt,
                           (unsigned)kPostReconfigPingAttempts,
                           (unsigned long)kPostReconfigPingDelayMs);
            NoteDelayMs(kPostReconfigPingDelayMs);
        }
    }

    PING_LOG_WARN("ping: verification at %lu baud failed after %u attempts",
                  (unsigned long)desiredRate,
                  (unsigned)kPostReconfigPingAttempts);
    return false;
}
