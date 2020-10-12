/*!
 * @file n_helpers.c
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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "n_lib.h"

// When interfacing with the Notecard, it is generally encouraged that the JSON object manipulation and
// calls to the note-arduino library are done directly at point of need.  However, there are cases in which
// it's convenient to have a wrapper.  The most common reason is when it's best to have a suppression timer
// on the actual call to the Notecard so as not to assault the I2C bus or UART on a continuing basis - the most
// common examples of this being "card.time" and any other Notecard state that needs to be polled for an app,
// such as the device location, its voltage level, and so on.  This file contains this kind of wrapper,
// just implemented here as a convenience to all developers.

// Time-related suppression timer and cache
static uint32_t timeBaseSetAtMs = 0;
static JTIME timeBaseSec = 0;
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

//**************************************************************************/
/*!
    @brief  Determine if the card time is "real" calendar/clock time, or if
            it is simply the millisecond clock since boot.
    @returns  A boolean indicating whether the current time is valid.
*/
/**************************************************************************/
bool NoteTimeValid() {
    timeTimer = 0;
    return NoteTimeValidST();
}

//**************************************************************************/
/*!
    @brief  Determine if if the suppression-timer card time is valid.
    @returns  A boolean indicating whether the current time is valid.
*/
/**************************************************************************/
bool NoteTimeValidST() {
    NoteTimeST();
    return (timeBaseSec != 0);
}

//**************************************************************************/
/*!
    @brief  Get the current epoch time, unsuppressed.
    @returns  The current time.
*/
/**************************************************************************/
JTIME NoteTime() {
    timeTimer = 0;
    return NoteTimeST();
}

//**************************************************************************/
/*!
    @brief  Set the time.
    @param   seconds The UNIX Epoch time.
*/
/**************************************************************************/
static void setTime(JTIME seconds) {
    timeBaseSec = seconds;
    timeBaseSetAtMs = _GetMs();
    _Debug("setting time\n");
}

//**************************************************************************/
/*!
    @brief  Print a full, newline-terminated line.
    @param   line The c-string line to print.
*/
/**************************************************************************/
void NotePrintln(const char *line) {
	NotePrint(line);
	NotePrint(c_newline);
}

//**************************************************************************/
/*!
    @brief  Log text "raw" to either the active debug console or to the
            Notecard's USB console.
    @param   text  A debug string for output.
*/
/**************************************************************************/
bool NotePrint(const char *text) {
	bool success = false;
	if (NoteIsDebugOutputActive()) {
		NoteDebug(text);
		return true;
	}
    int inLog = 0;
	if (inLog++ != 0) {
		inLog--;
		return false;
	}
    J *req = NoteNewRequest("card.log");
    JAddStringToObject(req, "text", text);
    success = NoteRequest(req);
	inLog--;
	return success;
}

//**************************************************************************/
/*!
    @brief  Write a formatted string to the debug output.
    @param   format  A format string for output.
    @param   ...  One or more values to interpolate into the format string.
*/
/**************************************************************************/
bool NotePrintf(const char *format, ...) {
    char line[256];
    va_list args;
    va_start(args, format);
    vsnprintf(line, sizeof(line), format, args);
    va_end(args);
	return NotePrint(line);
}

//**************************************************************************/
/*!
    @brief  Get the current epoch time as known by the module. If it isn't known
            by the module, just return the time since boot as indicated by the
            millisecond clock.
    @returns  The current time, or the time since boot.
*/
/**************************************************************************/
JTIME NoteTimeST() {

    // If we haven't yet fetched the time, or if we still need the timezone, do so with a suppression
    // timer so that we don't hammer the module before it's had a chance to connect to the network to fetch time.
    if (timeBaseSec == 0 || zoneStillUnavailable) {
        if (timerExpiredSecs(&timeTimer, 10)) {

            // Request time and zone info from the card
            J *rsp = NoteRequestResponse(NoteNewRequest("card.time"));
            if (rsp != NULL) {
                if (!NoteResponseError(rsp)) {
                    JTIME seconds = JGetInt(rsp, "time");
                    if (seconds != 0) {

                        // Set the time if it hasn't yet been set
						if (timeBaseSec == 0)
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
    JTIME adjustedTime = timeBaseSec + ((_GetMs() - timeBaseSetAtMs) / 1000);

    // Done
    return adjustedTime;

}

//**************************************************************************/
/*!
    @brief  Return local region info, if known. Returns true if valid.
    @param   retCountry (out) The country.
    @param   retArea (out) The area value.
    @param   retZone (out) The timezone value.
    @param   retZoneOffset (out) The timezone offset.
    @returns boolean indicating if the region information is valid.
*/
/**************************************************************************/
bool NoteRegion(char **retCountry, char **retArea, char **retZone, int *retZoneOffset) {
    if (zoneStillUnavailable) {
        if (retCountry != NULL)
            *retCountry = (char *) "";
        if (retArea != NULL)
            *retArea = (char *) "";
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

//**************************************************************************/
/*!
    @brief  See if the card location is valid.  If it is OFF, it is
            treated as valid.
    @param   errbuf (out) A buffer with any error information to return.
    @param   errbuflen The length of the error buffer.
    @returns boolean indicating if the location information is valid.
*/
/**************************************************************************/
bool NoteLocationValid(char *errbuf, uint32_t errbuflen) {
    locationTimer = 0;
    locationValid = false;
    locationLastErr[0] = '\0';
    return NoteLocationValidST(errbuf, errbuflen);
}

//**************************************************************************/
/*!
    @brief  See if the card location is valid, time-suppressed.  If it is OFF,
            it is treated as valid.
    @param   errbuf (out) A buffer with any error information to return.
    @param   errbuflen The length of the error buffer.
    @returns boolean indicating if the location information is valid.
*/
/**************************************************************************/
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

//**************************************************************************/
/*!
    @brief  Set a service environment variable default string.
    @param   variable The variable key.
    @param   buf The variable value.
    @returns boolean indicating if variable was set.
*/
/**************************************************************************/
bool NoteSetEnvDefault(const char *variable, char *buf) {
	bool success = false;
    J *req = NoteNewRequest("env.default");
    if (req != NULL) {
        JAddStringToObject(req, "name", variable);
		JAddStringToObject(req, "text", buf);
        success = NoteRequest(req);
    }
	return success;
}

//**************************************************************************/
/*!
    @brief  Set a service environment variable default integer.
    @param   variable The variable key.
    @param   defaultVal The variable value.
    @returns boolean indicating if variable was set.
*/
/**************************************************************************/
bool NoteSetEnvDefaultInt(const char *variable, int defaultVal) {
    char buf[32];
	snprintf(buf, sizeof(buf), "%d", defaultVal);
    return NoteSetEnvDefault(variable, buf);
}

//**************************************************************************/
/*!
    @brief  Set a service environment variable default number.
    @param   variable The variable key.
    @param   defaultVal The variable value.
    @returns boolean indicating if variable was set.
*/
/**************************************************************************/
bool NoteSetEnvDefaultNumber(const char *variable, JNUMBER defaultVal) {
    char buf[32];
	JNtoA(defaultVal, buf, -1);
    return NoteSetEnvDefault(variable, buf);
}

//**************************************************************************/
/*!
    @brief  Get a service environment variable number.
    @param   variable The variable key.
    @param   defaultVal The default variable value.
    @returns environment variable value.
*/
/**************************************************************************/
JNUMBER NoteGetEnvNumber(const char *variable, JNUMBER defaultVal) {
    char buf[32], buf2[32];;
	snprintf(buf2, sizeof(buf2), "%f", defaultVal);
    NoteGetEnv(variable, buf2, buf, sizeof(buf));
    return JAtoN(buf, NULL);
}

//**************************************************************************/
/*!
    @brief  Get a service environment variable integer.
    @param   variable The variable key.
    @param   defaultVal The default variable value.
    @returns environment variable value.
*/
/**************************************************************************/
int NoteGetEnvInt(const char *variable, int defaultVal) {
    char buf[32], buf2[32];;
	snprintf(buf2, sizeof(buf2), "%d", defaultVal);
    NoteGetEnv(variable, buf2, buf, sizeof(buf));
    return atoi(buf);
}

//**************************************************************************/
/*!
    @brief  Get a service environment variable.
    @param   variable The variable key.
    @param   defaultVal The variable value.
    @param   buf (out) The buffer in which to place the variable value.
    @param   buflen The length of the output buffer.
*/
/**************************************************************************/
void NoteGetEnv(const char *variable, const char *defaultVal, char *buf, uint32_t buflen) {
    if (defaultVal == NULL)
        buf[0] = '\0';
    else
        strlcpy(buf, defaultVal, buflen);
    J *req = NoteNewRequest("env.get");
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

//**************************************************************************/
/*!
    @brief  Determine if the Notecard is connected to the network.
    @returns boolean. `true` if connected.
*/
/**************************************************************************/
bool NoteIsConnected() {
    connectivityTimer = 0;
    return NoteIsConnectedST();
}

//**************************************************************************/
/*!
    @brief  Determine if the Notecard is connected to the network.
    @returns boolean. `true` if connected.
*/
/**************************************************************************/
bool NoteIsConnectedST() {
    if (timerExpiredSecs(&connectivityTimer, 10)) {
        J *rsp = NoteRequestResponse(NoteNewRequest("hub.status"));
        if (rsp != NULL) {
            if (!NoteResponseError(rsp))
                cardConnected = JGetBool(rsp, "connected");
            NoteDeleteResponse(rsp);
        }
    }
    return cardConnected;
}

//**************************************************************************/
/*!
    @brief  Get Full Network Status via `hub.status`.
    @param  statusBuf (out) a buffer to populate with the status response.
    @param  statusBufLen The length of the status buffer
    @returns boolean. `true` if the status was retrieved.
*/
/**************************************************************************/
bool NoteGetNetStatus(char *statusBuf, int statusBufLen) {
    bool success = false;
    statusBuf[0] = '\0';
    J *rsp = NoteRequestResponse(NoteNewRequest("hub.status"));
    if (rsp != NULL) {
        success = !NoteResponseError(rsp);
        if (success) {
            strlcpy(statusBuf, JGetString(rsp, "status"), statusBufLen);
        }
        NoteDeleteResponse(rsp);
    }
    return success;
}

//**************************************************************************/
/*!
    @brief  Make a `card.version` request
    @param  versionBuf (out) a buffer to populate with the version response.
    @param  versionBufLen The length of the version buffer
    @returns boolean. `true` if the version information was retrieved.
*/
/**************************************************************************/
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

//**************************************************************************/
/*!
    @brief  Make a `card.location` request
    @param  retLat (out) The retrieved latitude value.
    @param  retLon (out) The retrieved longitude value.
    @param  time (out) The retrieved time.
    @param  statusBuf (out) a buffer to populate with the location response.
    @param  statusBufLen The length of the location buffer
    @returns boolean. `true` if the location information was retrieved.
*/
/**************************************************************************/
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

//**************************************************************************/
/*!
    @brief  Set the card location to a static value.
    @param  lat The latitude value to set on the Notecard.
    @param  lon The longitude value to set on the Notecard.
    @returns boolean. `true` if the location information was set.
*/
/**************************************************************************/
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

//**************************************************************************/
/*!
    @brief  Clear last known Location.
    @returns boolean. `true` if the location information was cleared.
*/
/**************************************************************************/
bool NoteClearLocation() {
    bool success = false;
    J *req = NoteNewRequest("card.location.mode");
    if (req != NULL) {
        JAddBoolToObject(req, "delete", true);
        success = NoteRequest(req);
    }
    return success;
}

//**************************************************************************/
/*!
    @brief  Get the current location Mode.
    @param  modeBuf (out) a buffer to populate with the location mode response.
    @param  modeBufLen The length of the mode buffer
    @returns boolean. `true` if the mode information was retrieved.
*/
/**************************************************************************/
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

//**************************************************************************/
/*!
    @brief  Make a `card.location.mode` request.
    @param  mode the Notecard location mode to set.
    @param  seconds The value of the `seconds` field for the request.
    @returns boolean. `true` if the mode was set.
*/
/**************************************************************************/
bool NoteSetLocationMode(const char *mode, uint32_t seconds) {
    bool success = false;
    J *req = NoteNewRequest("card.location.mode");
    if (req != NULL) {
        if (mode[0] == '\0')
            mode = "-";
        JAddStringToObject(req, "mode", mode);
        JAddNumberToObject(req, "seconds", seconds);
        success = NoteRequest(req);
    }
    return success;
}

//**************************************************************************/
/*!
    @brief  Get the current service configuration information, unsuppressed.
    @param  productBuf (out) a buffer to populate with the ProductUID from the
                       response.
    @param  productBufLen The length of the ProductUID buffer.
    @param  serviceBuf (out) a buffer to populate with the host url from the
                       response.
    @param  serviceBufLen The length of the host buffer.
    @param  deviceBuf (out) a buffer to populate with the DeviceUID from the
                      response.
    @param  deviceBufLen The length of the DeviceUID buffer.
    @param  snBuf (out) a buffer to populate with the Product Serial Number
                      from the response.
    @param  snBufLen The length of the Serial Number buffer.
    @returns boolean. `true` if the service configuration was obtained.
*/
/**************************************************************************/
bool NoteGetServiceConfig(char *productBuf, int productBufLen, char *serviceBuf, int serviceBufLen, char *deviceBuf, int deviceBufLen, char *snBuf, int snBufLen) {
    serviceConfigTimer = 0;
    return NoteGetServiceConfigST(productBuf, productBufLen, serviceBuf, serviceBufLen, deviceBuf, deviceBufLen, snBuf, snBufLen);
}

//**************************************************************************/
/*!
    @brief  Get the current service configuration information, with
            a supression timer.
    @param  productBuf (out) a buffer to populate with the ProductUID from the
                       response.
    @param  productBufLen The length of the ProductUID buffer.
    @param  serviceBuf (out) a buffer to populate with the host url from the
                       response.
    @param  serviceBufLen The length of the host buffer.
    @param  deviceBuf (out) a buffer to populate with the DeviceUID from the
                      response.
    @param  deviceBufLen The length of the DeviceUID buffer.
    @param  snBuf (out) a buffer to populate with the Product Serial Number
                      from the response.
    @param  snBufLen The length of the Serial Number buffer.
    @returns boolean. `true` if the service configuration was obtained.
*/
/**************************************************************************/
bool NoteGetServiceConfigST(char *productBuf, int productBufLen, char *serviceBuf, int serviceBufLen, char *deviceBuf, int deviceBufLen, char *snBuf, int snBufLen) {
    bool success = false;

    // Use cache except for a rare refresh
    if (scProduct[0] == '\0' || scDevice[0] == '\0' || timerExpiredSecs(&serviceConfigTimer, 4*60*60)) {
        J *rsp = NoteRequestResponse(NoteNewRequest("hub.get"));
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

//**************************************************************************/
/*!
    @brief  Get Status of the Notecard, unsuppressed.
    @param  statusBuf (out) a buffer to populate with the Notecard status
                      from the response.
    @param  statusBufLen The length of the status buffer.
    @param  bootTime (out) The Notecard boot time.
    @param  retUSB (out) Whether the Notecard is powered over USB.
    @param  retSignals (out) Whether the Notecard has a network signal.
    @returns boolean. `true` if the card status was obtained.
*/
/**************************************************************************/
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

//**************************************************************************/
/*!
    @brief  Get Status of the Notecard, with a supression timer.
    @param  statusBuf (out) a buffer to populate with the Notecard status
                      from the response.
    @param  statusBufLen The length of the status buffer.
    @param  bootTime (out) The Notecard boot time.
    @param  retUSB (out) Whether the Notecard is powered over USB.
    @param  retSignals (out) Whether the Notecard has a network signal.
    @returns boolean. `true` if the card status was obtained.
*/
/**************************************************************************/
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

//**************************************************************************/
/*!
    @brief  Save the current state and use `card.attn` to set a host
            sleep interval.
    @param  stateb64 A base64 payload to keep in memory while the host sleeps.
    @param  seconds The duration to sleep.
    @param  modes A list of `card.attn` modes.
    @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
bool NoteSleep(char *stateb64, uint32_t seconds, const char *modes) {
    bool success = false;

    // Put ourselves to sleep
    _Debug("requesting sleep\n");
    J *req = NoteNewRequest("card.attn");
    if (req != NULL) {
        // Add the base64 item in a wonderful way that doesn't strdup the huge string
        J *stringReferenceItem = JCreateStringReference(stateb64);
        if (stringReferenceItem != NULL)
            JAddItemToObject(req, "payload", stringReferenceItem);
		char modestr[64];
		strlcpy(modestr, "sleep", sizeof(modestr));
		if (modes != NULL) {
			strlcat(modestr, ",", sizeof(modestr));
			strlcat(modestr, modes, sizeof(modestr));
		}
        JAddStringToObject(req, "mode", modestr);
        JAddNumberToObject(req, "seconds", seconds);
        success = NoteRequest(req);
    }

    // Done
    return success;
}

//**************************************************************************/
/*!
    @brief  Wake the module by restoring state into a state buffer of a
            specified length, and fail if it isn't available or isn't that
            length.
    @param  stateLen A length of the state payload buffer to return to the host.
    @param  state (out) The in-memory payload to return to the host.
    @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
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

//**************************************************************************/
/*!
    @brief  Restore the module to factory settings.
    @param  deleteConfigSettings Whether to delete current configuration
            settings on the Notecard during the reset.
    @returns boolean. `true` if reset was successful.
*/
/**************************************************************************/
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

//**************************************************************************/
/*!
    @brief  Set the Notecard Product UID.
    @param  productID The ProductUID to set on the Notecard.
    @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
bool NoteSetProductID(const char *productID) {
    bool success = false;
    J *req = NoteNewRequest("hub.set");
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

//**************************************************************************/
/*!
    @brief  Set the Notecard Serial Number.
    @param  sn The Serial Number to set on the Notecard.
    @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
bool NoteSetSerialNumber(const char *sn) {
    bool success = false;
    J *req = NoteNewRequest("hub.set");
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

//**************************************************************************/
/*!
    @brief  Set the Notecard upload mode and interval.
    @param   uploadMode The upload mode (for instance, `continuous`,
             or `periodic`).
    @param   uploadMinutes The max number of minutes to wait between Notehub
             uploads.
    @param   align Flag to specify that uploads should be grouped within the
             specified period, rather than counting the number of minutes
             from first modified.
    @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
bool NoteSetUploadMode(const char *uploadMode, int uploadMinutes, bool align) {
    bool success = false;
    J *req = NoteNewRequest("hub.set");
    if (req != NULL) {
        JAddStringToObject(req, "mode", uploadMode);
        if (uploadMinutes != 0) {
            JAddNumberToObject(req, "outbound", uploadMinutes);
            // Setting this flag aligns uploads to be grouped within the period,
            // rather than counting the number of minutes from "first modified".
            JAddBoolToObject(req, "align", align);
        }
        success = NoteRequest(req);
    }
    return success;

}

//**************************************************************************/
/*!
    @brief  Set the Notecard upload mode, download mode and interval.
    @param   uploadMode The upload mode (for instance, `continuous`,
             or `periodic`).
    @param   uploadMinutes The max number of minutes to wait between Notehub
             uploads.
    @param   downloadMinutes The max number of minutes to wait between Notehub
             downloads.
    @param   align Flag to specify that uploads should be grouped within the
             specified period, rather than counting the number of minutes
             from first modified.
    @param   sync Setting this flag when mode is `continuous` causes an
             immediate sync when a file is modified on the service side.
    @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
bool NoteSetSyncMode(const char *uploadMode, int uploadMinutes, int downloadMinutes, bool align, bool sync) {
    bool success = false;
    J *req = NoteNewRequest("hub.set");
    if (req != NULL) {
        JAddStringToObject(req, "mode", uploadMode);
        if (uploadMinutes != 0) {
            JAddNumberToObject(req, "outbound", uploadMinutes);
            // Setting this flag aligns uploads to be grouped within the period,
            // rather than counting the number of minutes from "first modified".
            JAddBoolToObject(req, "align", align);
        }
        if (downloadMinutes != 0)
            JAddNumberToObject(req, "inbound", downloadMinutes);
        // Setting this flag when mode is "continuous" causes an immediate sync
        // when a file is modified on the service side via HTTP
        JAddBoolToObject(req, "sync", sync);
        success = NoteRequest(req);
    }
    return success;

}

//**************************************************************************/
/*!
    @brief  Set a template for a Notefile.
    @param   target The Notefile on which to set a template.
    @param   body The template body.
    @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
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

//**************************************************************************/
/*!
    @brief  Add a Note to a Notefile with `note.add`.
    @param   target The Notefile on which to set a template.
    @param   body The template body.
    @param   urgent Whether to perform an immediate sync after the Note
             is added.
    @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
bool NoteAdd(const char *target, J *body, bool urgent) {

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

//**************************************************************************/
/*!
    @brief  Send a body to a route using an HTTP request.
            Body is freed, regardless of success.
    @param   method HTTP method to use, `get`, `post` or `put`.
    @param   routeAlias The Notehub Route alias.
    @param   notefile The Notefile name.
    @param   body The request JSON body.
    @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
bool NoteSendToRoute(const char *method, const char *routeAlias, char *notefile, J *body) {

    // Create the new event
    J *req = NoteNewRequest("note.event");
    if (req == NULL) {
        JDelete(body);
        return false;
    }

    // Add the body item and the Notefile name
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

//**************************************************************************/
/*!
    @brief  Get the voltage of the Notecard.
    @param   voltage (out) The Notecard voltage.
    @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
bool NoteGetVoltage(JNUMBER *voltage) {
    bool success = false;
    *voltage = 0.0;
    J *rsp = NoteRequestResponse(NoteNewRequest("card.voltage"));
    if (rsp != NULL) {
        if (!NoteResponseError(rsp)) {
            *voltage = JGetNumber(rsp, "value");
			success = true;
		}
        NoteDeleteResponse(rsp);
    }
    return success;
}

//**************************************************************************/
/*!
    @brief  Get the temperature of the Notecard.
    @param   temp (out) The Notecard temperature.
    @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
bool NoteGetTemperature(JNUMBER *temp) {
    bool success = false;
    *temp = 0.0;
    J *rsp = NoteRequestResponse(NoteNewRequest("card.temp"));
    if (rsp != NULL) {
        if (!NoteResponseError(rsp)) {
            *temp = JGetNumber(rsp, "value");
			success = true;
		}
        NoteDeleteResponse(rsp);
    }
    return success;
}

//**************************************************************************/
/*!
    @brief  Get the Notecard contact info.
    @param   nameBuf (out) The contact name buffer.
    @param   nameBufLen The length of the contact name buffer.
    @param   orgBuf (out) The contact organization buffer.
    @param   orgBufLen The length of the contact organization buffer.
    @param   roleBuf (out) The contact role buffer.
    @param   roleBufLen The length of the contact role buffer.
    @param   emailBuf (out) The contact email buffer.
    @param   emailBufLen The length of the contact email buffer.
    @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
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

//**************************************************************************/
/*!
    @brief  Set the Notecard contact info.
    @param   nameBuf (out) The contact name buffer.
    @param   orgBuf (out) The contact organization buffer.
    @param   roleBuf (out) The contact role buffer.
    @param   emailBuf (out) The contact email buffer.
    @returns boolean. `true` if request was successful.
*/
/**************************************************************************/
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

//**************************************************************************/
/*!
    @brief  Periodically show Notecard sync status, returning `true` if
            something was displayed
    @param   pollFrequencyMs The time to wait, in milliseconds, between
             polling for sync status.
    @param   maxLevel The Synclog level to monitor.
    @returns boolean. `true` if a sync status message was displayed.
*/
/**************************************************************************/
bool NoteDebugSyncStatus(int pollFrequencyMs, int maxLevel) {

	// Suppress polls so as to not overwhelm the notecard
	static uint32_t lastCommStatusPollMs = 0;
	if (lastCommStatusPollMs != 0 && _GetMs() < (lastCommStatusPollMs + pollFrequencyMs))
		return false;

	// Get the next queued status note
	J *req = NoteNewRequest("note.get");
	if (req == NULL)
		return false;
	JAddStringToObject(req, "file", "_synclog.qi");
	JAddBoolToObject(req, "delete", true);
	NoteSuspendTransactionDebug();
	J *rsp = NoteRequestResponse(req);
	NoteResumeTransactionDebug();
	if (rsp != NULL) {

		// If an error is returned, this means that no response is pending.	 Note
		// that it's expected that this might return either a "note does not exist"
		// error if there are no pending inbound notes, or a "file does not exist" error
		// if the inbound queue hasn't yet been created on the service.
		if (NoteResponseError(rsp)) {
			// Only stop polling quickly if we don't receive anything
			lastCommStatusPollMs = _GetMs();
			NoteDeleteResponse(rsp);
			return false;
		}

		// Get the note's body
		J *body = JGetObject(rsp, "body");
		if (body != NULL) {
			if (maxLevel < 0 || JGetInt(body, "level") <= maxLevel) {
				_Debug("sync: ");
				_Debug(JGetString(body, "subsystem"));
				_Debug(" ");
				_Debug(JGetString(body, "text"));
				_Debug("\n");
			}
		}

		// Done with this response
		NoteDeleteResponse(rsp);
		return true;
	}

	return false;

}

// A general purpose, super nonperformant, but accurate way of figuring out how much memory
// is available, while exercising the allocator to ensure that it competently deals with
// adjacent block coalescing on free.
typedef struct objHeader_s {
    struct objHeader_s *prev;
    int length;
} objHeader;

//**************************************************************************/
/*!
    @brief  Obtain the amount of free memory available on the Notecard.
    @returns The number of bytes of memory available on the Notecard.
*/
/**************************************************************************/
uint32_t NoteMemAvailable() {

    // Allocate progressively smaller and smaller chunks
    objHeader *lastObj = NULL;
    static int maxsize = 35000;
    for (int i=maxsize; i>=sizeof(objHeader); i=i-sizeof(objHeader)) {
        for (int j=0;;j++) {
            objHeader *thisObj;
            thisObj = (objHeader *) malloc(i);
            if (thisObj == NULL)
                break;
            thisObj->prev = lastObj;
            thisObj->length = i;
            lastObj = thisObj;
        }
    }

    // Free the objects backwards
    int lastLength = 0;
    int lastLengthCount = 0;
    uint32_t total = 0;
    while (lastObj != NULL) {
        if (lastObj->length != lastLength) {
            lastLength = lastObj->length;
            lastLengthCount = 1;
        } else {
            lastLengthCount++;
        }
        objHeader *thisObj = lastObj;
        lastObj = lastObj->prev;
        total += thisObj->length;
        free(thisObj);
    }

	return total;
  
}
