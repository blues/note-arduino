// Copyright 2022 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// Notecard "edge mode" type definitions

#pragma once

#include "n_lib.h"

// For standard tracking, the data format of a single point
#define TRACKTYPE_NORMAL c_nullstring
#define TRACKTYPE_HEARTBEAT c_heartbeat
#define TRACKTYPE_USB_CHANGE c_usb
#define TRACKTYPE_NO_SAT c_no_sat

typedef struct {
#define TRACKPOINT_MEASUREMENT_TIME c_mtime
    double mtime;
#define TRACKPOINT_LAT c_lat
    double lat;
#define TRACKPOINT_LON c_lon
    double lon;
#define TRACKPOINT_TIME c_time
    uint32_t time;
#define TRACKPOINT_HDOP c_dop
    double hdop;
#define TRACKPOINT_JOURNEY_TIME c_journey
    uint32_t journeyTime;
#define TRACKPOINT_JOURNEY_COUNT c_jcount
    uint32_t journeyCount;
#define TRACKPOINT_TYPE c_status
    char trackType[32];
#define TRACKPOINT_MOTION_COUNT c_motion
    uint32_t motionCount;
#define TRACKPOINT_SECONDS c_seconds
    int32_t seconds;
#define TRACKPOINT_DISTANCE c_distance
    double distance;
#define TRACKPOINT_BEARING c_bearing
    double bearing;
#define TRACKPOINT_VELOCITY c_velocity
    double velocity;
#define TRACKPOINT_TEMPERATURE c_temperature
    double temperature;
#define TRACKPOINT_HUMIDITY c_humidity
    double humidity;
#define TRACKPOINT_PRESSURE c_pressure
    double pressure;
#define TRACKPOINT_VOLTAGE c_voltage
    double voltage;
#define TRACKPOINT_USB c_usb
    bool usb;
#define TRACKPOINT_CHARGING c_charging
    bool charging;
} TrackPoint;

// MotionPoint is the data structure that we use when tracking motion
typedef struct {
#define MOTIONPOINT_MEASUREMENT_TIME c_mtime
    double mtime;
#define MOTIONPOINT_MOVEMENTS c_movements
    char movements[250];
#define MOTIONPOINT_ORIENTATION c_orientation
    char orientation[20];
#define MOTIONPOINT_MOTION_COUNT c_motion
    uint32_t motionCount;
#define MOTIONPOINT_TILT_COUNT "tilt"
    uint32_t tiltCount;
} MotionPoint;

// LogData is the data structure that we use when capturing hub.log information
typedef struct {
#define LOGDATA_MESSAGE "message"
    char message[256];
#define LOGDATA_METHOD "method"
    char method[64];
#define LOGDATA_ALERT "alert"
    bool alert;
} LogData;

// Format of the edge entry which is the dequeued note body
#define EDGE_NOTEFILE        c_edge_notefile
#define EDGETYPE_SCAN        c_scan
#define EDGETYPE_TRACKPOINT  c_track
#define EDGETYPE_MOTIONPOINT c_motion
#define EDGETYPE_LOG		 c_log

typedef struct {
#define EDGE_TYPE c_type
    char edgeType[32];
#define EDGE_TRACKPOINT c_point
    TrackPoint trackPoint;
#define EDGE_MOTIONPOINT c_motion
    MotionPoint motionPoint;
#define EDGE_SCAN_SECS c_secs
    uint32_t scanSecs;
#define EDGE_SCAN_METERS c_meters
    uint32_t scanMeters;
#define EDGE_SCAN_BEARING c_bearing
    uint32_t scanBearing;
#define EDGE_SCAN_BEGAN_TIME c_began_when
    uint32_t scanBeganTime;
#define EDGE_SCAN_BEGAN_LOC_TIME c_began_loc_when
    uint32_t scanBeganLocTime;
#define EDGE_SCAN_BEGAN_LOC_LAT c_began_loc_lat
    double scanBeganLocLat;
#define EDGE_SCAN_BEGAN_LOC_LON c_began_loc_lon
    double scanBeganLocLon;
#define EDGE_SCAN_BEGAN_LOC_HDOP c_began_loc_dop
    double scanBeganLocHDOP;
#define EDGE_SCAN_BEGAN_MOTION_TIME c_began_motion_when
    uint32_t scanBeganMotionTime;
#define EDGE_SCAN_ENDED_TIME c_ended_when
    uint32_t scanEndedTime;
#define EDGE_SCAN_ENDED_LOC_TIME c_ended_loc_when
    uint32_t scanEndedLocTime;
#define EDGE_SCAN_ENDED_LOC_LAT c_ended_loc_lat
    double scanEndedLocLat;
#define EDGE_SCAN_ENDED_LOC_LON c_ended_loc_lon
    double scanEndedLocLon;
#define EDGE_SCAN_ENDED_LOC_HDOP c_ended_loc_dop
    double scanEndedLocHDOP;
#define EDGE_SCAN_ENDED_MOTION_TIME c_ended_motion_when
    uint32_t scanEndedMotionTime;
#define EDGE_SCAN_LOC_LAT c_loc_lat
    double scanLocLat;
#define EDGE_SCAN_LOC_LON c_loc_lon
    double scanLocLon;
} EdgeData;

// Scan formats
#define SCAN_SEP            '\n'			// inter-reading separator
#define SCAN_TYPE_GSM       'g'             // FORMAT_2G
#define SCAN_TYPE_CDMA      'c'             // FORMAT_2G
#define SCAN_TYPE_UMTS      'u'             // FORMAT_3G
#define SCAN_TYPE_WCDMA     'w'             // FORMAT_3G
#define SCAN_TYPE_LTE       'l'             // FORMAT_4G
#define SCAN_TYPE_EMTC      'e'             // FORMAT_4G
#define SCAN_TYPE_NBIOT     'i'             // FORMAT_4G
#define SCAN_TYPE_NR        'n'             // FORMAT_5G
#define SCAN_TYPE_WIFI      'x'             // FORMAT_WIFI
#define SCAN_TYPE_CELLTIME  't'             // FORMAT_TIME
#define SCAN_TYPE_WIFITIME  's'             // FORMAT_TIME
#define SCAN_TYPE_GPS       'd'             // FORMAT_GPS
#define SCAN_FORMAT_2G      c_scan_format_2g
#define SCAN_FORMAT_3G      c_scan_format_3g
#define SCAN_FORMAT_4G      c_scan_format_4g
#define SCAN_FORMAT_5G      c_scan_format_5g
#define SCAN_FORMAT_WIFI    c_scan_format_wifi
#define SCAN_FORMAT_TIME    c_scan_format_time
#define SCAN_FORMAT_GPS     c_scan_format_gps
