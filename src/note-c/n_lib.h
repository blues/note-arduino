/*!
 * @file n_lib.h
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

#pragma once

#include <string.h>
#include "note.h"

// C-callable functions
#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************/
/*!
    @brief  How long to wait for the card for any given transaction.
*/
/**************************************************************************/
#define NOTECARD_TRANSACTION_TIMEOUT_SEC     10

// The notecard is a real-time device that has a fixed size interrupt buffer.
// We can push data at it far, far faster than it can process it, therefore we
// push it in segments with a pause between each segment.

/**************************************************************************/
/*!
    @brief  The max length, in bytes, of each request segment when using I2C.
*/
/**************************************************************************/
#define CARD_REQUEST_I2C_SEGMENT_MAX_LEN 250
/**************************************************************************/
/*!
    @brief  The delay, in miliseconds, of each request when using I2C.
*/
/**************************************************************************/
#define CARD_REQUEST_I2C_SEGMENT_DELAY_MS 250
/**************************************************************************/
/*!
    @brief  The delay, in miliseconds, between each request chunk when using I2C.
*/
/**************************************************************************/
#define CARD_REQUEST_I2C_CHUNK_DELAY_MS 20
/**************************************************************************/
/*!
    @brief  The max length, in bytes, of each request segment when using Serial.
*/
/**************************************************************************/
#define CARD_REQUEST_SERIAL_SEGMENT_MAX_LEN 250
/**************************************************************************/
/*!
    @brief  The delay, in miliseconds, of each request when using Serial.
*/
/**************************************************************************/
#define CARD_REQUEST_SERIAL_SEGMENT_DELAY_MS 250

/**************************************************************************/
/*!
    @brief  Memory allocation chunk size.
*/
/**************************************************************************/
#ifdef NOTE_LOWMEM
#define ALLOC_CHUNK 64
#else
#define ALLOC_CHUNK 128
#endif

// Transactions
const char *i2cNoteTransaction(char *json, char **jsonResponse);
bool i2cNoteReset(void);
const char *serialNoteTransaction(char *json, char **jsonResponse);
bool serialNoteReset(void);

// Hooks
void NoteLockNote(void);
void NoteUnlockNote(void);
bool NoteTransactionStart(uint32_t timeoutMs);
void NoteTransactionStop(void);
const char *NoteActiveInterface(void);
bool NoteSerialReset(void);
void NoteSerialTransmit(uint8_t *, size_t, bool);
bool NoteSerialAvailable(void);
char NoteSerialReceive(void);
bool NoteI2CReset(uint16_t DevAddress);
const char *NoteI2CTransmit(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size);
const char *NoteI2CReceive(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size, uint32_t *avail);
bool NoteHardReset(void);
const char *NoteJSONTransaction(char *json, char **jsonResponse);
bool NoteIsDebugOutputActive(void);

// Constants, a global optimization to save static string memory
extern const char c_agent[];
#define c_agent_len 5

extern const char c_align[];
#define c_align_len 5

extern const char c_arch_arc32[];
#define c_arch_arc32_len 5

extern const char c_arch_avr[];
#define c_arch_avr_len 3

extern const char c_arch_esp32[];
#define c_arch_esp32_len 5

extern const char c_arch_esp8266[];
#define c_arch_esp8266_len 7

extern const char c_arch_megaavr[];
#define c_arch_megaavr_len 7

extern const char c_arch_nrf52840[];
#define c_arch_nrf52840_len 8

extern const char c_arch_nrf52[];
#define c_arch_nrf52_len 5

extern const char c_arch_nrf51[];
#define c_arch_nrf51_len 5

extern const char c_arch_pic32[];
#define c_arch_pic32_len 5

extern const char c_arch_sam[];
#define c_arch_sam_len 3

extern const char c_arch_samd[];
#define c_arch_samd_len 4

extern const char c_arch_spresence[];
#define c_arch_spresence_len 9

extern const char c_arch_stm32[];
#define c_arch_stm32_len 5

extern const char c_arch_stm32f0[];
#define c_arch_stm32f0_len 7

extern const char c_arch_stm32f1[];
#define c_arch_stm32f1_len 7

extern const char c_arch_stm32f4[];
#define c_arch_stm32f4_len 7

extern const char c_arch_stm32g0[];
#define c_arch_stm32g0_len 7

extern const char c_arch_stm32l4[];
#define c_arch_stm32l4_len 7

extern const char c_arch_stm32u5[];
#define c_arch_stm32u5_len 7

extern const char c_arch_swan_r5[];
#define c_arch_swan_r5_len 7

extern const char c_area[];
#define c_area_len 4

extern const char c_array[];
#define c_array_len 5

extern const char c_asterisk[];
#define c_asterisk_len 1

extern const char c_bad[];
#define c_bad_len 3

extern const char c_bearing[];
#define c_bearing_len 7

extern const char c_began_loc_dop[];
#define c_began_loc_dop_len 13

extern const char c_began_loc_lat[];
#define c_began_loc_lat_len 13

extern const char c_began_loc_lon[];
#define c_began_loc_lon_len 13

extern const char c_began_loc_when[];
#define c_began_loc_when_len 14

extern const char c_began_motion_when[];
#define c_began_motion_when_len 17

extern const char c_began_when[];
#define c_began_when_len 10

extern const char c_body[];
#define c_body_len 4

extern const char c_bool[];
#define c_bool_len 4

extern const char c_card_attn[];
#define c_card_attn_len 9

extern const char c_card_contact[];
#define c_card_contact_len 12

extern const char c_card_location[];
#define c_card_location_len 13

extern const char c_card_location_mode[];
#define c_card_location_mode_len 18

extern const char c_card_log[];
#define c_card_log_len 8

extern const char c_card_restore[];
#define c_card_restore_len 12

extern const char c_card_status[];
#define c_card_status_len 11

extern const char c_card_temp[];
#define c_card_temp_len 9

extern const char c_card_time[];
#define c_card_time_len 9

extern const char c_card_version[];
#define c_card_version_len 12

extern const char c_card_voltage[];
#define c_card_voltage_len 12

extern const char c_charging[];
#define c_charging_len 8

extern const char c_cmd[];
#define c_cmd_len 3

extern const char c_colon[];
#define c_colon_len 1

extern const char c_compiler[];
#define c_compiler_len 8

extern const char c_comma[];
#define c_comma_len 1

extern const char c_connected[];
#define c_connected_len 9

extern const char c_country[];
#define c_country_len 7

extern const char c_cpu_cores[];
#define c_cpu_cores_len 9

extern const char c_cpu_mem[];
#define c_cpu_mem_len 7

extern const char c_cpu_mhz[];
#define c_cpu_mhz_len 7

extern const char c_cpu_name[];
#define c_cpu_name_len 8

extern const char c_cpu_vendor[];
#define c_cpu_vendor_len 10

extern const char c_curly_brace_close[];
#define c_curly_brace_close_len 1

extern const char c_curly_brace_open[];
#define c_curly_brace_open_len 1

extern const char c_dbg_msg_about_to_sleep[];
#define c_dbg_msg_about_to_sleep_len 15

extern const char c_dbg_msg_awakened_successfully[];
#define c_dbg_msg_awakened_successfully_len 22

extern const char c_dbg_msg_cannot_convert_to_json[];
#define c_dbg_msg_cannot_convert_to_json_len 21

extern const char c_dbg_msg_card_restored[];
#define c_dbg_msg_card_restored_len 14

extern const char c_dbg_msg_did_not_sleep[];
#define c_dbg_msg_did_not_sleep_len 13

extern const char c_dbg_msg_i2c_not_active[];
#define c_dbg_msg_i2c_not_active_len 14

extern const char c_dbg_msg_i2c_or_serial_interface_must_be_selected[];
#define c_dbg_msg_i2c_or_serial_interface_must_be_selected_len 40

extern const char c_dbg_msg_i2c_receive_error[];
#define c_dbg_msg_i2c_receive_error_len 18

extern const char c_dbg_msg_i2c_transmit[];
#define c_dbg_msg_i2c_transmit_len 12

extern const char c_dbg_msg_insufficient_memory[];
#define c_dbg_msg_insufficient_memory_len 19

extern const char c_dbg_msg_invalid_data_received_on_serial_port_from_notecard[];
#define c_dbg_msg_invalid_data_received_on_serial_port_from_notecard_len 51

extern const char c_dbg_msg_invalid_json[];
#define c_dbg_msg_invalid_json_len 14

extern const char c_dbg_msg_io_bad[];
#define c_dbg_msg_io_bad_len 8

extern const char c_dbg_msg_io_err[];
#define c_dbg_msg_io_err_len 4

extern const char c_dbg_msg_io_serial_communications_error[];
#define c_dbg_msg_io_serial_communications_error_len 32

extern const char c_dbg_msg_io_timeout[];
#define c_dbg_msg_io_timeout_len 12

extern const char c_dbg_msg_io_transaction_incomplete[];
#define c_dbg_msg_io_transaction_incomplete_len 27

extern const char c_dbg_msg_io_transaction_timeout[];
#define c_dbg_msg_io_transaction_timeout_len 24

extern const char c_dbg_msg_io_unrecognized_response_from_card[];
#define c_dbg_msg_io_unrecognized_response_from_card_len 36

extern const char c_dbg_msg_no_notecard[];
#define c_dbg_msg_no_notecard_len 12

extern const char c_dbg_msg_notecard_not_responding[];
#define c_dbg_msg_notecard_not_responding_len 24

extern const char c_dbg_msg_received_only_partial_reply_after_timeout[];
#define c_dbg_msg_received_only_partial_reply_after_timeout_len 43

extern const char c_dbg_msg_reply_to_request_did_not_arrive_from_module_in_time[];
#define c_dbg_msg_reply_to_request_did_not_arrive_from_module_in_time_len 51

extern const char c_dbg_msg_request_or_response_was_lost[];
#define c_dbg_msg_request_or_response_was_lost_len 33

extern const char c_dbg_msg_requesting_sleep[];
#define c_dbg_msg_requesting_sleep_len 17

extern const char c_dbg_msg_transaction_jsonbuf_malloc_failed[];
#define c_dbg_msg_transaction_jsonbuf_malloc_failed_len 35

extern const char c_dbg_msg_unrecognized_data_from_notecard[];
#define c_dbg_msg_unrecognized_data_from_notecard_len 32

extern const char c_delete[];
#define c_delete_len 6

extern const char c_device[];
#define c_device_len 6

extern const char c_distance[];
#define c_distance_len 8

extern const char c_dop[];
#define c_dop_len 3

extern const char c_edge_notefile[];
#define c_edge_notefile_len 8

extern const char c_email[];
#define c_email_len 5

extern const char c_ended_loc_dop[];
#define c_ended_loc_dop_len 13

extern const char c_ended_loc_lat[];
#define c_ended_loc_lat_len 13

extern const char c_ended_loc_lon[];
#define c_ended_loc_lon_len 13

extern const char c_ended_loc_when[];
#define c_ended_loc_when_len 14

extern const char c_ended_motion_when[];
#define c_ended_motion_when_len 17

extern const char c_ended_when[];
#define c_ended_when_len 10

extern const char c_env_default[];
#define c_env_default_len 11

extern const char c_env_get[];
#define c_env_get_len 7

extern const char c_err[];
#define c_err_len 3

extern const char c_FAIL[];
#define c_FAIL_len 4

extern const char c_false[];
#define c_false_len 5

extern const char c_file[];
#define c_file_len 4

extern const char c_fixed[];
#define c_fixed_len 5

extern const char c_fmt_1_15g[];
#define c_fmt_1_15g_len 6

extern const char c_fmt_1_17g[];
#define c_fmt_1_17g_len 6

extern const char c_fmt_lg[];
#define c_fmt_lg_len 3

extern const char c_free[];
#define c_free_len 4

extern const char c_heartbeat[];
#define c_heartbeat_len 9

extern const char c_HEX[];
#define c_HEX_len 16

extern const char c_hex[];
#define c_hex_len 16

extern const char c_host[];
#define c_host_len 4

extern const char c_hub_get[];
#define c_hub_get_len 7

extern const char c_hub_set[];
#define c_hub_set_len 7

extern const char c_hub_status[];
#define c_hub_status_len 10

extern const char c_humidity[];
#define c_humidity_len 8

extern const char c_hyphen[];
#define c_hyphen_len 1

extern const char c_i2c[];
#define c_i2c_len 3

extern const char c_inbound[];
#define c_inbound_len 7

extern const char c_INF[];
#define c_INF_len 3

extern const char c_inf[];
#define c_inf_len 3

extern const char c_invalid[];
#define c_invalid_len 7

extern const char c_jcount[];
#define c_jcount_len 6

extern const char c_journey[];
#define c_journey_len 7

extern const char c_lat[];
#define c_lat_len 3

extern const char c_level[];
#define c_level_len 5

extern const char c_loc_lat[];
#define c_loc_lat_len 7

extern const char c_loc_lon[];
#define c_loc_lon_len 7

extern const char c_log[];
#define c_log_len 3

extern const char c_lon[];
#define c_lon_len 3

extern const char c_malloc[];
#define c_malloc_len 6

extern const char c_mem[];
#define c_mem_len 3

extern const char c_meters[];
#define c_meters_len 6

extern const char c_minutes[];
#define c_minutes_len 7

extern const char c_mode[];
#define c_mode_len 4

extern const char c_motion[];
#define c_motion_len 6

extern const char c_movements[];
#define c_movements_len 9

extern const char c_mtime[];
#define c_mtime_len 5

extern const char c_name[];
#define c_name_len 4

extern const char c_NAN[];
#define c_NAN_len 3

extern const char c_nan[];
#define c_nan_len 3

extern const char c_newline[];
#define c_newline_len 2

extern const char c_no_sat[];
#define c_no_sat_len 6

extern const char c_note_add[];
#define c_note_add_len 8

extern const char c_note_c[];
#define c_note_c_len 6

extern const char c_note_event[];
#define c_note_event_len 10

extern const char c_note_get[];
#define c_note_get_len 8

extern const char c_note_template[];
#define c_note_template_len 13

extern const char c_null[];
#define c_null_len 4

extern const char c_null_paren[];
#define c_null_paren_len 6

extern const char c_nullstring[];
#define c_nullstring_len 0

extern const char c_number[];
#define c_number_len 6

extern const char c_object[];
#define c_object_len 6

extern const char c_off[];
#define c_off_len 3

extern const char c_org[];
#define c_org_len 3

extern const char c_orientation[];
#define c_orientation_len 11

extern const char c_os_family[];
#define c_os_family_len 9

extern const char c_os_name[];
#define c_os_name_len 7

extern const char c_os_platform[];
#define c_os_platform_len 11

extern const char c_os_version[];
#define c_os_version_len 10

extern const char c_outbound[];
#define c_outbound_len 8

extern const char c_payload[];
#define c_payload_len 7

extern const char c_point[];
#define c_point_len 5

extern const char c_pressure[];
#define c_pressure_len 8

extern const char c_product[];
#define c_product_len 7

extern const char c_quotation_mark[];
#define c_quotation_mark_len 1

extern const char c_req[];
#define c_req_len 3

extern const char c_req_interface[];
#define c_req_interface_len 13

extern const char c_role[];
#define c_role_len 4

extern const char c_route[];
#define c_route_len 5

extern const char c_scan[];
#define c_scan_len 4

extern const char c_scan_format_2g[];
#define c_scan_format_2g_len 25

extern const char c_scan_format_3g[];
#define c_scan_format_3g_len 30

extern const char c_scan_format_4g[];
#define c_scan_format_4g_len 51

extern const char c_scan_format_5g[];
#define c_scan_format_5g_len 51

extern const char c_scan_format_gps[];
#define c_scan_format_gps_len 18

extern const char c_scan_format_time[];
#define c_scan_format_time_len 9

extern const char c_scan_format_wifi[];
#define c_scan_format_wifi_len 37

extern const char c_seconds[];
#define c_seconds_len 7

extern const char c_secs[];
#define c_secs_len 4

extern const char c_serial[];
#define c_serial_len 6

extern const char c_signals[];
#define c_signals_len 7

extern const char c_sleep[];
#define c_sleep_len 5

extern const char c_sn[];
#define c_sn_len 2

extern const char c_space[];
#define c_space_len 1

extern const char c_start[];
#define c_start_len 5

extern const char c_status[];
#define c_status_len 6

extern const char c_string[];
#define c_string_len 6

extern const char c_subsystem[];
#define c_subsystem_len 9

extern const char c_sync[];
#define c_sync_len 4

extern const char c_synclog_notefile[];
#define c_synclog_notefile_len 11

extern const char c_temperature[];
#define c_temperature_len 11

extern const char c_text[];
#define c_text_len 4

extern const char c_time[];
#define c_time_len 4

extern const char c_track[];
#define c_track_len 5

extern const char c_true[];
#define c_true_len 4

extern const char c_type[];
#define c_type_len 4

extern const char c_unknown[];
#define c_unknown_len 7

extern const char c_usb[];
#define c_usb_len 3

extern const char c_UTC[];
#define c_UTC_len 3

extern const char c_utf8_bom[];
#define c_utf8_bom_len 3

extern const char c_value[];
#define c_value_len 5

extern const char c_velocity[];
#define c_velocity_len 8

extern const char c_version[];
#define c_version_len 7

extern const char c_voltage[];
#define c_voltage_len 7

extern const char c_web_dot[];
#define c_web_dot_len 4

extern const char c_zone[];
#define c_zone_len 4

// Readability wrappers.  Anything starting with _ is simply calling the wrapper
// function.
#define _LockNote NoteLockNote
#define _UnlockNote NoteUnlockNote
#define _TransactionStart NoteTransactionStart
#define _TransactionStop NoteTransactionStop
#define _SerialReset NoteSerialReset
#define _SerialTransmit NoteSerialTransmit
#define _SerialAvailable NoteSerialAvailable
#define _SerialReceive NoteSerialReceive
#define _I2CReset NoteI2CReset
#define _I2CTransmit NoteI2CTransmit
#define _I2CReceive NoteI2CReceive
#define _Reset NoteHardReset
#define _Transaction NoteJSONTransaction
#define _Malloc NoteMalloc
#define _Free NoteFree
#define _GetMs NoteGetMs
#define _DelayMs NoteDelayMs
#define _LockI2C NoteLockI2C
#define _UnlockI2C NoteUnlockI2C
#define _I2CAddress NoteI2CAddress
#define _I2CMax NoteI2CMax
#ifdef NOTE_NODEBUG
#define _Debug(x)
#define _Debugln(x)
#else
#define _Debug(x) NoteDebug(x)
#define _Debugln(x) NoteDebugln(x)
#endif

// End of C-callable functions
#ifdef __cplusplus
}
#endif
