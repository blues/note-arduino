// Copyright 2018 Inca Roads LLC.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include <stdlib.h>
#include <stdio.h>
#include "n_lib.h"

// When interfacing with the Notecard, it is generally encouraged that the JSON object manipulation and
// calls to the note-arduino library are done directly at point of need.  However, there are cases in which
// it's convenient to have a wrapper.  The most common reason is when it's best to have a suppression timer
// on the actual call to the Notecard so as not to assault the I2C bus or UART on a continuing basis - the most
// common examples of this being "card.time" and any other Notecard state that needs to be polled for an app,
// such as the device location, its voltage level, and so on.  This file contains this kind of wrapper,
// just implemented here as a convenience to all developers.

// Time-related suppression timer and cache
static long timeBaseSetAtMs = 0;
static JTIME timeBase = 0;
static uint32_t timeTimer = 0;
static bool zoneStillUnavailable = true;
static char curZone[48] = {0};
static char curArea[64] = {0};
static char curCountry[8] = "";
static int curZoneOffsetMins = 0;

// Location-related suppression timer and cache
static uint32_t locationTimer = 0;
static char locationLastErr[64] = {0};
static bool locationValid = false;

// Connection-related suppression timer and cache
static uint32_t connectivityTimer = 0;
static bool cardConnected = false;

// Service config-related suppression timer and cache
static uint32_t serviceConfigTimer = 0;
static char scDevice[128] = {0};
static char scSN[128] = {0};
static char scProduct[128] = {0};
static char scService[128] = {0};

// Forwards
static bool timerExpiredSecs(uint32_t *timer, uint32_t periodSecs);

// See if the card time is "real" calendar/clock time, or if it is simply the millisecond clock since boot
bool NoteTimeValid() {
    timeTimer = 0;
    return NoteTimeValidST();
}

// See if the suppression-timer card time is valid
bool NoteTimeValidST() {
    NoteTimeST();
    return (timeBase != 0);
}

// Get the current epoch time, unsuppressed
JTIME NoteTime() {
    timeTimer = 0;
    return NoteTimeST();
}

// Set the time
static void setTime(JTIME seconds) {
    timeBase = seconds;
    timeBaseSetAtMs = _GetMs();
    _Debug("setting time\n");
}

// Get the current epoch time as known by the module.  If it isn't known by the module, just
// return the time since boot as indicated by the millisecond clock.
JTIME NoteTimeST() {

    // If we haven't yet fetched the time, or if we still need the timezone, do so with a suppression
    // timer so that we don't hammer the module before it's had a chance to connect to the network to fetch time.
    if (timeBase == 0 || zoneStillUnavailable) {
        if (timerExpiredSecs(&timeTimer, 10)) {

            // Request time and zone info from the card
            J *rsp = NoteRequestResponse(NoteNewRequest("card.time"));
            if (rsp != NULL) {
                if (!NoteResponseError(rsp)) {
                    JTIME seconds = JGetInt(rsp, "time");
                    if (seconds != 0) {

                        // Set the time if it hasn't yet been set
						if (timeBase == 0)
	                        setTime(seconds);

                        // Get the zone
                        char *z = JGetString(rsp, "zone");
                        if (z[0] != '\0') {
                            char zone[64];
                            strlcpy(zone, z, sizeof(zone));
                            // Only use the 3-letter abbrev
                            char *sep = strchr(zone, ',');
                            if (sep == NULL)
                                zone[0] = '\0';
                            else
                                *sep = '\0';
                            zoneStillUnavailable = (memcmp(zone, "UTC", 3) == 0);
                            strlcpy(curZone, zone, sizeof(curZone));
                            curZoneOffsetMins = JGetInt(rsp, "minutes");
                            strlcpy(curCountry, JGetString(rsp, "country"), sizeof(curCountry));
                            strlcpy(curArea, JGetString(rsp, "area"), sizeof(curArea));
                        }

                    }
                }
                NoteDeleteResponse(rsp);
            }
        }
    }

    // Adjust the base time by the number of seconds that have elapsed since the base.
    JTIME adjustedTime = timeBase + ((_GetMs() - timeBaseSetAtMs) / 1000);

    // Done
    return adjustedTime;

}

// Return local region info, if known.  Returns true if valid
bool NoteRegion(char **retCountry, char **retArea, char **retZone, int *retZoneOffset) {
    if (zoneStillUnavailable) {
        if (retCountry != NULL)
            *retCountry = "";
        if (retArea != NULL)
            *retArea = "";
        if (retZone != NULL)
            *retZone = curZone;
        if (retZoneOffset != NULL)
            *retZoneOffset = 0;
        return false;
    }
    if (retCountry != NULL)
        *retCountry = curCountry;
    if (retArea != NULL)
        *retArea = curArea;
    if (retZone != NULL)
        *retZone = curZone;
    if (retZoneOffset != NULL)
        *retZoneOffset = curZoneOffsetMins;
    return true;;
}

// See if the card location is valid.  If it is OFF, it is treated as valid.
bool NoteLocationValid(char *errbuf, uint32_t errbuflen) {
    locationTimer = 0;
    locationValid = false;
    locationLastErr[0] = '\0';
    return NoteLocationValidST(errbuf, errbuflen);
}

// See if the card location is valid, time-suppressed.  If it is OFF, it is treated as valid.
bool NoteLocationValidST(char *errbuf, uint32_t errbuflen) {

    // Preset
    if (errbuf != NULL)
        strlcpy(errbuf, locationLastErr, errbuflen);

    // If it was ever valid, return true
    if (locationValid) {
        locationLastErr[0] = '\0';
        if (errbuf != NULL)
            *errbuf = '\0';
        return true;
    }

    // If we haven't yet fetched the location, do so with a suppression
    // timer so that we don't hammer the module before it's had a chance to
    // connect to the gps to fetch location.
    if (!timerExpiredSecs(&locationTimer, 5))
        return false;

    // Request location from the card
    J *rsp = NoteRequestResponse(NoteNewRequest("card.location"));
    if (rsp == NULL)
        return false;

    // If valid, or the location mode is OFF, we're done
    if (!NoteResponseError(rsp) || strcmp(JGetString(rsp, "mode"), "off") == 0) {
        NoteDeleteResponse(rsp);
        locationValid = true;
        locationLastErr[0] = '\0';
        if (errbuf != NULL)
            *errbuf = '\0';
        return true;
    }

    // Remember the error for next iteration
    strlcpy(locationLastErr, JGetString(rsp, "err"), sizeof(locationLastErr));
    if (errbuf != NULL)
        strlcpy(errbuf, locationLastErr, errbuflen);
    NoteDeleteResponse(rsp);
    return false;

}

// Get a service environment variable integer
int NoteGetEnvInt(const char *variable, int defaultVal) {
    char buf[64];
    NoteGetEnv(variable, "0", buf, sizeof(buf));
    return atoi(buf);
}

// Get a service environment variable
void NoteGetEnv(const char *variable, const char *defaultVal, char *buf, uint32_t buflen) {
    if (defaultVal == NULL)
        buf[0] = '\0';
    else
        strlcpy(buf, defaultVal, buflen);
    J *req = NoteNewRequest("service.env");
    if (req != NULL) {
        JAddStringToObject(req, "name", variable);
        J *rsp = NoteRequestResponse(req);
        if (rsp != NULL) {
            if (!NoteResponseError(rsp)) {
                char *val = JGetString(rsp, "text");
                if (val[0] != '\0')
                    strlcpy(buf, val, buflen);
            }
            NoteDeleteResponse(rsp);
        }
    }
}

// Get the entire set of available environment vars
bool NoteGetEnvAll(char *statusBuf, int statusBufLen) {
    bool success = false;
    statusBuf[0] = '\0';
    J *rsp = NoteRequestResponse(NoteNewRequest("service.env"));
    if (rsp != NULL) {
        success = !NoteResponseError(rsp);
        if (success) {
            char *text = JGetString(rsp, "text");
            if (text[0] != '\0')
                strlcpy(statusBuf, text, statusBufLen);
        }
        NoteDeleteResponse(rsp);
    }
    return success;
}

// See if we're connected to the net
bool NoteIsConnected() {
    connectivityTimer = 0;
    return NoteIsConnectedST();
}

// See if we're connected to the net
bool NoteIsConnectedST() {
    if (timerExpiredSecs(&connectivityTimer, 10)) {
        J *rsp = NoteRequestResponse(NoteNewRequest("service.status"));
        if (rsp != NULL) {
            if (!NoteResponseError(rsp))
                cardConnected = JGetBool(rsp, "connected");
            NoteDeleteResponse(rsp);
        }
    }
    return cardConnected;
}

// Get Network Status
bool NoteGetNetStatus(char *statusBuf, int statusBufLen) {
    bool success = false;
    statusBuf[0] = '\0';
    J *rsp = NoteRequestResponse(NoteNewRequest("service.status"));
    if (rsp != NULL) {
        success = !NoteResponseError(rsp);
        if (success) {
            strlcpy(statusBuf, JGetString(rsp, "status"), statusBufLen);
        }
        NoteDeleteResponse(rsp);
    }
    return success;
}

// Get Version
bool NoteGetVersion(char *versionBuf, int versionBufLen) {
    bool success = false;
    versionBuf[0] = '\0';
    J *rsp = NoteRequestResponse(NoteNewRequest("card.version"));
    if (rsp != NULL) {
        success = !NoteResponseError(rsp);
        if (success) {
            strlcpy(versionBuf, JGetString(rsp, "version"), versionBufLen);
        }
        NoteDeleteResponse(rsp);
    }
    return success;
}

// Get Location
bool NoteGetLocation(JNUMBER *retLat, JNUMBER *retLon, JTIME *time, char *statusBuf, int statusBufLen) {
    bool locValid = false;
    if (statusBuf != NULL)
        *statusBuf = '\0';
    if (retLat != NULL)
        *retLat = 0.0;
    if (retLon != NULL)
        *retLon = 0.0;
    if (time != NULL)
        *time = 0;
    J *rsp = NoteRequestResponse(NoteNewRequest("card.location"));
    if (rsp != NULL) {
        if (statusBuf != NULL)
            strlcpy(statusBuf, JGetString(rsp, "err"), statusBufLen);
        if (JIsPresent(rsp, "lat") && JIsPresent(rsp, "lon")) {
            if (retLat != NULL)
                *retLat = JGetNumber(rsp, "lat");
            if (retLon != NULL)
                *retLon = JGetNumber(rsp, "lon");
            locValid = true;
        }
        JTIME seconds = JGetInt(rsp, "time");
        if (seconds != 0 && time != NULL)
            *time = seconds;
        NoteDeleteResponse(rsp);
    }
    return locValid;
}

// Set Static Location
bool NoteSetLocation(JNUMBER lat, JNUMBER lon) {
    bool success = false;
    J *req = NoteNewRequest("card.location.mode");
    if (req != NULL) {
        JAddStringToObject(req, "mode", "fixed");
        JAddNumberToObject(req, "lat", lat);
        JAddNumberToObject(req, "lon", lon);
        success = NoteRequest(req);
    }
    return success;
}

// Clear last known Location
bool NoteClearLocation() {
    bool success = false;
    J *req = NoteNewRequest("card.location.mode");
    if (req != NULL) {
        JAddBoolToObject(req, "delete", true);
        success = NoteRequest(req);
    }
    return success;
}

// Get the current Location Mode
bool NoteGetLocationMode(char *modeBuf, int modeBufLen) {
    bool success = false;
    modeBuf[0] = '\0';
    J *rsp = NoteRequestResponse(NoteNewRequest("card.location.mode"));
    if (rsp != NULL) {
        success = !NoteResponseError(rsp);
        if (success)
            strlcpy(modeBuf, JGetString(rsp, "mode"), modeBufLen);
        NoteDeleteResponse(rsp);
    }
    return success;
}

// Set Location Mode
bool NoteSetLocationMode(const char *mode) {
    bool success = false;
    J *req = NoteNewRequest("card.location.mode");
    if (req != NULL) {
        if (mode[0] == '\0')
            mode = "-";
        JAddStringToObject(req, "mode", mode);
        success = NoteRequest(req);
    }
    return success;
}

// Get the current epoch time, unsuppressed
bool NoteGetServiceConfig(char *productBuf, int productBufLen, char *serviceBuf, int serviceBufLen, char *deviceBuf, int deviceBufLen, char *snBuf, int snBufLen) {
    serviceConfigTimer = 0;
    return NoteGetServiceConfigST(productBuf, productBufLen, serviceBuf, serviceBufLen, deviceBuf, deviceBufLen, snBuf, snBufLen);
}

// Get Service Configuration
bool NoteGetServiceConfigST(char *productBuf, int productBufLen, char *serviceBuf, int serviceBufLen, char *deviceBuf, int deviceBufLen, char *snBuf, int snBufLen) {
    bool success = false;

    // Use cache except for a rare refresh
    if (scProduct[0] == '\0' || scDevice[0] == '\0' || timerExpiredSecs(&serviceConfigTimer, 4*60*60)) {
        J *rsp = NoteRequestResponse(NoteNewRequest("service.get"));
        if (rsp != NULL) {
            success = !NoteResponseError(rsp);
            if (success) {
                strlcpy(scProduct, JGetString(rsp, "product"), sizeof(scProduct));
                strlcpy(scService, JGetString(rsp, "host"), sizeof(scService));
                strlcpy(scDevice, JGetString(rsp, "device"), sizeof(scDevice));
                strlcpy(scSN, JGetString(rsp, "sn"), sizeof(scSN));
            }
            NoteDeleteResponse(rsp);
        }
    } else {
        success = true;
    }

    // Done
    if (productBuf != NULL)
        strlcpy(productBuf, scProduct, productBufLen);
    if (serviceBuf != NULL)
        strlcpy(serviceBuf, scService, serviceBufLen);
    if (deviceBuf != NULL)
        strlcpy(deviceBuf, scDevice, deviceBufLen);
    if (snBuf != NULL)
        strlcpy(snBuf, scSN, snBufLen);
    return success;
}

// Get Status of the Notecard
bool NoteGetStatus(char *statusBuf, int statusBufLen, JTIME *bootTime, bool *retUSB, bool *retSignals) {
    bool success = false;
    if (statusBuf != NULL)
        statusBuf[0] = '\0';
    if (bootTime != NULL)
        *bootTime = 0;
    if (retUSB != NULL)
        *retUSB = false;
    if (retSignals != NULL)
        *retSignals = false;
    J *rsp = NoteRequestResponse(NoteNewRequest("card.status"));
    if (rsp != NULL) {
        success = !NoteResponseError(rsp);
        if (success) {
            if (statusBuf != NULL)
                strlcpy(statusBuf, JGetString(rsp, "status"), statusBufLen);
            if (bootTime != NULL)
                *bootTime = JGetInt(rsp, "time");
            if (retUSB != NULL)
                *retUSB = JGetBool(rsp, "usb");
            if (retSignals != NULL && JGetBool(rsp, "connected"))
                *retSignals = (JGetInt(rsp, "signals") > 0);
        }
        NoteDeleteResponse(rsp);
    }
    return success;
}

// Get Status with a Suppression Timer
bool NoteGetStatusST(char *statusBuf, int statusBufLen, JTIME *bootTime, bool *retUSB, bool *retSignals) {
    bool success = false;
    static char lastStatus[128] = {0};
    static JTIME lastBootTime = 0;
    static bool lastUSB = false;
    static bool lastSignals = false;
    static uint32_t statusTimer = 0;

    // Refresh if it's time to do so
    if (timerExpiredSecs(&statusTimer, 10)) {
        J *rsp = NoteRequestResponse(NoteNewRequest("card.status"));
        if (rsp != NULL) {
            success = !NoteResponseError(rsp);
            if (success) {
                strlcpy(lastStatus, JGetString(rsp, "status"), sizeof(lastStatus));
                lastBootTime = JGetInt(rsp, "time");
                lastUSB = JGetBool(rsp, "usb");
                if (JGetBool(rsp, "connected"))
                    lastSignals = (JGetInt(rsp, "signals") > 0);
                else
                    lastSignals = false;
            }
            NoteDeleteResponse(rsp);
        }
    } else {
        success = true;
    }

    // Done
    if (statusBuf != NULL)
        strlcpy(statusBuf, lastStatus, statusBufLen);
    if (bootTime != NULL)
        *bootTime = lastBootTime;
    if (retUSB != NULL)
        *retUSB = lastUSB;
    if (retSignals != NULL)
        *retSignals = lastSignals;

    return success;
}

// Save state and sleep, returning FALSE if failure to proceed
bool NoteSleep(char *stateb64, uint32_t seconds, const char *modes) {
    bool success = false;

    // If optional wakeup modes are specified, set them
    if (modes != NULL) {
        J *req = NoteNewRequest("card.attn");
        if (req != NULL) {
            JAddStringToObject(req, "mode", modes);
            if (!NoteRequest(req))
                return false;
        }
    }

    // Put ourselves to sleep
    _Debug("requesting sleep\n");
    J *req = NoteNewRequest("card.attn");
    if (req != NULL) {
        // Add the base64 item in a wonderful way that doesn't strdup the huge string
        J *stringReferenceItem = JCreateStringReference(stateb64);
        if (stringReferenceItem != NULL)
            JAddItemToObject(req, "payload", stringReferenceItem);
        JAddStringToObject(req, "mode", "reset");
        JAddNumberToObject(req, "seconds", seconds);
        success = NoteRequest(req);
    }

    // Done
    return success;
}

// Wake the module by restoring state into a state buffer of exactly this length,
// and fail if it isn't available or isn't that length
bool NoteWake(int stateLen, void *state) {

    J *req = NoteNewRequest("card.attn");
    if (req == NULL)
        return false;

    // Send it a command to request the saved state
    JAddBoolToObject(req, "start", true);
    J *rsp = NoteRequestResponse(req);
    if (rsp == NULL)
        return false;
    if (NoteResponseError(rsp)) {
        NoteDeleteResponse(rsp);
        return false;
    }

    // Note the current time, if the field is present
    JTIME seconds = JGetInt(rsp, "time");
    if (seconds != 0)
        setTime(seconds);

    // Exit if no payload
    char *payload = JGetString(rsp, "payload");
    if (payload[0] == '\0') {
        NoteDeleteResponse(rsp);
        return false;
    }

    // Exit if the purpose of the call was to intentionally discard saved state
    if (state == NULL) {
        NoteDeleteResponse(rsp);
        return true;
    }

    // Allocate a buffer for the payload.  (We can't decode in-place because we can't
    // risk overwriting memory if the actual payload is even slightly different.)
    char *p = (char *) _Malloc(JB64DecodeLen(payload));
    if (p == NULL) {
        NoteDeleteResponse(rsp);
        return false;
    }
    int actualLen = JB64Decode(p, payload);
    if (actualLen != stateLen) {
        _Free(p);
        _Debug("*** discarding saved state\n");
        NoteDeleteResponse(rsp);
        return false;
    }
    memcpy(state, p, stateLen);
    _Free(p);

    // State restored
    _Debug("STATE RESTORED\n");

    NoteDeleteResponse(rsp);
    return true;
}

// Restore the module to factory settings
bool NoteFactoryReset(bool deleteConfigSettings) {
    bool success = false;

    // Perform the restore-to-factor-settings transaction
    J *req = NoteNewRequest("card.restore");
    if (req != NULL) {
        JAddBoolToObject(req, "delete", deleteConfigSettings);
        success = NoteRequest(req);
    }

    // Exit if it didn't work
    if (!success)
        return false;

    // Wait for serial to stabilize after it reboots
    _DelayMs(5000);
    _Debug("CARD RESTORED\n");

    // Reset the Notecard
    while (!NoteReset())
        _DelayMs(5000);

    // Success
    return true;

}

// Set the card product ID
bool NoteSetProductID(const char *productID) {
    bool success = false;
    J *req = NoteNewRequest("service.set");
    if (req != NULL) {
        if (productID[0] == '\0')
            JAddStringToObject(req, "product", "-");
        else
            JAddStringToObject(req, "product", productID);
        success = NoteRequest(req);
    }
    // Flush cache so that service config is re-fetched
    serviceConfigTimer = 0;
    return success;
}

// Set the card serial number
bool NoteSetSerialNumber(const char *sn) {
    bool success = false;
    J *req = NoteNewRequest("service.set");
    if (req != NULL) {
        if (sn[0] == '\0')
            JAddStringToObject(req, "sn", "-");
        else
            JAddStringToObject(req, "sn", sn);
        success = NoteRequest(req);
    }
    // Flush cache so that service config is re-fetched
    serviceConfigTimer = 0;
    return success;
}

// Set the upload mode and interval
bool NoteSetUploadMode(const char *uploadMode, int uploadMinutes, bool align) {
    bool success = false;
    J *req = NoteNewRequest("service.set");
    if (req != NULL) {
        JAddStringToObject(req, "mode", uploadMode);
        if (uploadMinutes != 0) {
            JAddNumberToObject(req, "minutes", uploadMinutes);
            // Setting this flag aligns uploads to be grouped within the period,
            // rather than counting the number of minutes from "first modified".
            JAddBoolToObject(req, "align", align);
        }
        success = NoteRequest(req);
    }
    return success;

}

// Set the JSON template for a target
bool NoteTemplate(const char *target, J *body) {
    J *req = NoteNewRequest("note.template");
    if (req == NULL) {
        JDelete(body);
        return false;
    }
    JAddStringToObject(req, "file", target);
    JAddItemToObject(req, "body", body);
    return NoteRequest(req);
}

// Append a new value to be sent to the card, with an "urgent" flag indicating that it must be sent NOW.
bool NoteSend(const char *target, J *body, bool urgent) {

    // Initiate the request
    J *req = NoteNewRequest("note.add");
    if (req == NULL) {
        JDelete(body);
        return false;
    }

    // Add the target notefile and body to the request.  Note that
    // JAddItemToObject passes ownership of the object to req
    JAddStringToObject(req, "file", target);
    JAddItemToObject(req, "body", body);

    // Initiate sync NOW if it's urgent
    if (urgent)
        JAddBoolToObject(req, "start", true);

    // Perform the transaction
    return NoteRequest(req);

}

// Send a body to a route, using either "post" or "put" method.  Body is freed, regardless of success
bool NoteSendToRoute(const char *method, const char *routeAlias, char *notefile, J *body) {

    // Create the new event
    J *req = NoteNewRequest("note.event");
    if (req == NULL) {
        JDelete(body);
        return false;
    }

    // Add the body item and the
    JAddItemToObject(req, "body", body);
    JAddStringToObject(req, "file", notefile);

    // Perform the transaction to convert it to an event
    J *rsp = NoteRequestResponse(req);
    if (rsp == NULL) {
        return false;
    }

    // Exit if error
    if (NoteResponseError(rsp)) {
        NoteDeleteResponse(rsp);
        return false;
    }

    // Extract the event, which we'll use as the body for the next transaction
    body = JDetachItemFromObject(rsp, "body");
    NoteDeleteResponse(rsp);

    // Create the post transaction
    char request[32];
    strlcpy(request, "web.", sizeof(request));
    strlcat(request, method, sizeof(request));
    req = NoteNewRequest(request);

    // Add the body, and the alias of the route on the notehub, hard-wired here
    JAddItemToObject(req, "body", body);
    JAddStringToObject(req, "route", routeAlias);

    // Perform the transaction
    return NoteRequest(req);

}

// Get the voltage of the Notecard
bool NoteGetVoltage(JNUMBER *voltage) {
    bool success = false;
    *voltage = 0.0;
    J *rsp = NoteRequestResponse(NoteNewRequest("card.voltage"));
    if (rsp != NULL) {
        if (!NoteResponseError(rsp))
            *voltage = JGetNumber(rsp, "value");
        NoteDeleteResponse(rsp);
    }
    return success;
}

// Get the temperature of the Notecard
bool NoteGetTemperature(JNUMBER *temp) {
    bool success = false;
    *temp = 0.0;
    J *rsp = NoteRequestResponse(NoteNewRequest("card.temp"));
    if (rsp != NULL) {
        if (!NoteResponseError(rsp))
            *temp = JGetNumber(rsp, "value");
        NoteDeleteResponse(rsp);
    }
    return success;
}

// Get Contact Info
bool NoteGetContact(char *nameBuf, int nameBufLen, char *orgBuf, int orgBufLen, char *roleBuf, int roleBufLen, char *emailBuf, int emailBufLen) {
    bool success = false;

    if (nameBuf != NULL)
        *nameBuf = '\0';
    if (orgBuf != NULL)
        *orgBuf = '\0';
    if (roleBuf != NULL)
        *roleBuf = '\0';
    if (emailBuf != NULL)
        *emailBuf = '\0';

    J *rsp = NoteRequestResponse(NoteNewRequest("card.contact"));
    if (rsp != NULL) {
        success = !NoteResponseError(rsp);
        if (success) {
            if (nameBuf != NULL)
                strlcpy(nameBuf, JGetString(rsp, "name"), nameBufLen);
            if (orgBuf != NULL)
                strlcpy(orgBuf, JGetString(rsp, "org"), orgBufLen);
            if (roleBuf != NULL)
                strlcpy(roleBuf, JGetString(rsp, "role"), roleBufLen);
            if (emailBuf != NULL)
                strlcpy(emailBuf, JGetString(rsp, "email"), emailBufLen);
            NoteDeleteResponse(rsp);
        }
    }

    return success;
}

// Set Contact Info
bool NoteSetContact(char *nameBuf, char *orgBuf, char *roleBuf, char *emailBuf) {
    J *req = NoteNewRequest("card.contact");
    if (req == NULL)
        return false;
    if (nameBuf != NULL)
        JAddStringToObject(req, "name", nameBuf);
    if (orgBuf != NULL)
        JAddStringToObject(req, "org", orgBuf);
    if (roleBuf != NULL)
        JAddStringToObject(req, "role", roleBuf);
    if (emailBuf != NULL)
        JAddStringToObject(req, "email", emailBuf);
    return NoteRequest(req);
}

// A simple suppression timer based on a millisecond system clock.  This clock is reset to 0
// after boot and every wake.  This returns true if the specified interval has elapsed, in seconds,
// and it updates the timer if it expires so that we will go another period.
static bool timerExpiredSecs(uint32_t *timer, uint32_t periodSecs) {
    bool expired = false;

    // If the timer went backward, we've expired regardless of the interval
    uint32_t prev = *timer;
    uint32_t now = _GetMs();
    if (now < prev)
        prev = 0;

    // If never initialized, it's expired
    if (prev == 0 || now >= prev+(periodSecs*1000)) {
        expired = true;
        *timer = now;
    }

    return expired;

}
