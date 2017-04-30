// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

/// \file tic.h
///
/// This file provides the C API for libpololu-tic, a library supports
/// communciating with the Pololu Tic USB Stepper Motor Controller.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// The maximum firmware major version supported by this library.
#define TIC_FIRMWARE_VERSION_MAJOR_MAX 1

#ifdef _WIN32
#  define TIC_DLL_EXPORT __declspec(dllexport)
#  define TIC_DLL_IMPORT __declspec(dllimport)
#else
#  define TIC_DLL_IMPORT __attribute__((visibility ("default")))
#  define TIC_DLL_EXPORT __attribute__((visibility ("default")))
#endif

#ifdef _MSC_VER
#define TIC_WARN_UNUSED _Check_return_
#else
#define TIC_WARN_UNUSED __attribute__((warn_unused_result))
#endif

#ifdef TIC_STATIC
#  define TIC_API
#else
#  ifdef TIC_EXPORTS
#    define TIC_API TIC_DLL_EXPORT
#  else
#    define TIC_API TIC_DLL_IMPORT
#  endif
#endif


/// Certain functions in the library return a string and require the caller to
/// call this function to free the string.
void tic_string_free(char *);


// tic_error ////////////////////////////////////////////////////////////////////

/// A ::tic_error object represents an error that occurred in the library.  Many
/// functions return a tic_error pointer.  The convention is that a NULL pointer
/// indicates success.  If the pointer is not NULL, the caller can use the
/// pointer to get information about the error, and then must free the error at
/// some point by calling tic_error_free.
typedef struct tic_error tic_error;

/// Each ::tic_error can have 0 or more error codes that give additional
/// information about the error that might help the caller take the right action
/// when the error occurs.  This enum defines which error codes are possible.
enum tic_error_code
{
  /// There were problems allocating memory.  A memory shortage might be the
  /// root cause of the error, or there might be another error that is masked by
  /// the memory problems.
  TIC_ERROR_MEMORY = 1,

  /// Access was denied.  A common cause of this error on Windows is that
  /// another application has a handle open to the same device.
  TIC_ERROR_ACCESS_DENIED = 2,

  /// The device took too long to respond to a request or transfer data.
  TIC_ERROR_TIMEOUT = 3,

  /// The error might have been caused by the device being disconnected, but it
  /// is possible it was caused by something else.
  TIC_ERROR_DEVICE_DISCONNECTED = 4,
};

/// Attempts to copy an error.  If you copy a NULL ::tic_error pointer, the
/// result will also be NULL.  If you copy a non-NULL ::tic_error pointer, the
/// result will be non-NULL, but if there are issues allocating memory, then the
/// copied error might have different properties than the original error, and it
/// will have the ::TIC_ERROR_MEMORY code.
///
/// It is the caller's responsibility to free the copied error.
TIC_API TIC_WARN_UNUSED
tic_error * tic_error_copy(const tic_error *);

/// Frees a returned error object.  Passing the NULL pointer to this function is
/// OK.  Do not free the same non-NULL error twice.
TIC_API
void tic_error_free(tic_error *);

/// Returns true if the error has specified error code.  The error codes are
/// listed in the ::tic_error_code enum.
TIC_API TIC_WARN_UNUSED
bool tic_error_has_code(const tic_error *, uint32_t code);

/// Returns an English-language ASCII-encoded string describing the error.  The
/// string consists of one or more complete sentences.
///
/// The returned pointer will be valid at least until the error is freed.
TIC_API TIC_WARN_UNUSED
const char * tic_error_get_message(const tic_error *);


// tic_settings ////////////////////////////////////////////////////////////////

/// Represents the non-volatile settings for a Tic.
typedef struct tic_handle tic_handle;

/// Creates a new settings object with the default settings.  The "model"
/// parameter should be set to 0 for now, and in the future will be used to
/// specify what type of Tic you are making settings for.
/// If this function is successful, the caller must free the settings later by
/// calling tic_settings_free().
TIC_API TIC_WARN_UNUSED
tic_error * tic_settings_create(tic_settings **, uint32_t model);

/// Copies a settings object. If this function is successful, the caller must
/// free the settings later by calling tic_settings_free().
TIC_API TIC_WARN_UNUSED
tic_error * tic_settings_copy(
  const tic_settings * source ,
  tic_settings ** dest);

// Frees the settings object.
void tic_settings_free(tic_settings *);

/// Fixes the settings to valid and consistent.
TIC_API TIC_WARN_UNUSED
void tic_settings_fix(tic_setting *, char **);

/// Sets the Control mode, which should be one of the TIC_CONTROL_MODE_* macros.
/// Silently obeys if the input is invalid so that you can see a warning later
/// when calling tic_settings_fix.
TIC_API
void tic_settings_control_mode_set(tic_settings *, uint8_t control_mode);

/// Returns the Control mode described in tic_settings_control_mode_set().
TIC_API
uint8_t tic_settings_control_mode_get(const tic_settings *);

/// Sets the Never sleep (ignore USB suspend) setting.  If true, prevents the
/// device from going into deep sleep mode in order to comply with suspend
/// current requirements of the USB specifications.
TIC_API
void tic_settings_never_sleep_set(tic_settings *);

/// Gets the Never sleep (ignore USB suspend) setting described in
/// tic_settings_never_sleep_set().
TIC_API
bool tic_settings_never_sleep_get(const tic_settings *);

/// Sets the Disable safe start setting.  If true, it disables some extra
/// conditions that are required to let the device start moving the motor.
TIC_API
void tic_settings_disable_safe_start_set(tic_settings *, bool);

/// Gets the Disable safe start setting described in
/// tic_settings_disable_safe_start_get().
TIC_API
bool tic_settings_disable_safe_start_get(const tic_settings *);

/// Sets the Ignore ERR line high setting.  If true, allows the device to
/// operate even if the ERR line is being driven high by something.
TIC_API
void tic_settings_ignore_err_line_high_set(tic_settings *, bool);

/// Gets the Ignore ERR line high setting described in
/// tic_settings_ignore_err_line_high_set().
TIC_API
bool tic_settings_ignore_err_line_high_get(const tic_settings *);

/// Sets the baud rate to be an approximation of the specified baud rate in bits
/// per second.
TIC_API
void tic_settings_baud_rate_set(tic_settings *);

/// Returns an estimate of the baud rate the controller will use for its
/// asynchronous serial port, in bits per second.
TIC_API
uint32_t tic_settings_baud_rate_get(const tic_settings *);

/// Sets the Serial device number, a number between 0 and 0x7F that is used to
/// identify the device when using the Pololu protocol.
TIC_API
void tic_settings_serial_device_number_set(tic_settings *, uint8_t);

/// Gets the Serial device number setting described in
/// tic_settings_serial_device_number_set().
TIC_API
uint8_t tic_settings_serial_device_number_get(const tic_settings *);

/// Sets the I2C device address to use for I2C slave communication.
TIC_API
void tic_settings_i2c_device_address_set(tic_settings *, uint8_t);

/// Gets the I2C device address to use for I2C slave communication.
TIC_API
uint8_t tic_settings_i2c_device_address_get(const tic_settings *);

/// Sets the Command timeout, the time in milliseconds before the device
/// considers it to be an error if we have not received certain commands.  A
/// value of 0 disables the command timeout feature.
TIC_API
void tic_settings_command_timeout_set(const tic_settings *, uint16_t);

/// Gets the Command timeout setting described in
/// tic_settings_command_timeout_set().
TIC_API
uint16_t tic_settings_command_timeout_get(const tic_settings *);

/// Sets the Serial CRC enabled setting.  If true, the device requires 7-bit CRC
/// bytes on all serial commands.
TIC_API
void tic_settings_serial_crc_enabled_set(tic_settings *, bool);

/// Gets the Serial CRC enabled setting described in
/// tic_settings_serial_crc_enabled_set().
TIC_API
bool tic_settings_serial_crc_enabled_get(const tic_settings *);

/// Sets the Low VIN shutoff timeout.  This determines how long, in milliseconds,
/// it takes for a low reading on VIN to be interpreted as an error.
TIC_API
void tic_settings_low_vin_timeout_set(tic_settings *, uint16_t);

/// Gets the Low VIN shutoff timeout setting described in
/// tic_settings_vin_shutoff_timeout_set().
TIC_API
uint16_t tic_settings_low_vin_timeout_get(const tic_settings *);

/// Sets the Low VIN shutoff voltage.  This determines how low, in millivolts,
/// the VIN reading has to be for the controller to change from considering it
/// to be adequate to considering it to be too low.
TIC_API
void tic_settings_low_vin_shutoff_voltage_set(tic_settings *, uint16_t);

/// Gets the Low VIN shutoff voltage setting described in
/// tic_settings_low_vin_shutoff_voltage_set().
TIC_API
uint16_t tic_settings_low_vin_shutoff_voltage_get(const tic_settings *);

/// Sets the Low VIN startup voltage.  This determines how high, in millivolts,
/// the VIN reading has to be for the controller to change from considering it
/// to be too low to considering it to be adequate.
TIC_API
void tic_settings_low_vin_startup_voltage_set(tic_settings *, uint16_t);

/// Gets the Low VIN startup voltage setting described in
/// tic_settings_low_vin_startup_voltage_set().
TIC_API
uint16_t tic_settings_low_vin_startup_voltage_get(const tic_settings *);

/// Sets the High VIN shutoff voltage.  This determines how high, in millivolts,
/// the VIN reading has to be for the controller to change from considering it
/// to be adequate to considering it to be too high.
TIC_API
void tic_settings_high_vin_shutoff_voltage_set(tic_settings *, uint16_t);

/// Gets the High VIN shutoff voltage setting described in
/// tic_settings_high_vin_shutoff_voltage_set().
TIC_API
uint16_t tic_settings_high_vin_shutoff_voltage_get(const tic_settings *);

/// Sets the VIN multiplier offset, a calibration factor used in computing VIN.
TIC_API
void tic_settings_vin_multiplier_offset_set(tic_settings *, uint16_t);

/// Sets the VIN multiplier offset setting described in
/// tic_settings_vin_multiplier_offset_set().
TIC_API
uint16_t void tic_settings_vin_multiplier_offset_get(const tic_settings *);

/// Sets the RC max pulse period.  This is the maximum allowed time between
/// consecutive RC pulse rising edges, in milliseconds.
TIC_API
void tic_settings_rc_max_pulse_period_set(tic_settings *, uint16_t);

/// Gets the RC max pulse period setting described in
/// tic_settings_rc_max_pulse_period_set().
TIC_API
uint16_t tic_settings_rc_max_pulse_period_get(const tic_settings *);

/// Sets the RC bad signal timeout.  This is the maximum time between valid RC
/// pulses that we use to update the motor, in milliseconds.
TIC_API
void tic_settings_rc_bad_signal_timeout_set(tic_settings *, uint16_t);

/// Gets the RC bad signal timeout setting described in
/// tic_settings_rc_bad_signal_timeout_set.
TIC_API
uint16_t tic_settings_rc_bad_signal_timeout_get(const tic_settings *);

// TODO: finish these accessors




// tic_device ///////////////////////////////////////////////////////////////////

/// Represents a Tic that is or was connected to the computer.
///
typedef struct tic_device tic_device;

/// Finds all the Tic devices connected to the computer via USB and returns a
/// list of them.
///
/// The list is terminated by a NULL pointer.  The optional @a device_count
/// parameter is used to return the number of devices in the list.
///
/// If this function is successful (the returned error pointer is NULL), then
/// you must later free each device by calling tic_device_free() and free the
/// list by calling tic_list_free().  The order in which the list and the
/// retrieved objects are freed does not matter.
TIC_API TIC_WARN_UNUSED
tic_error * tic_list_connected_devices(
  tic_device *** device_list,
  size_t * device_count);

/// Frees a device list returned by ::tic_list_connected_devices.
TIC_API
void tic_list_free(tic_device ** list);

/// Makes a copy of a device object.  If this function is successful, you will
/// need to free the copy by calling tic_device_free() at some point.
TIC_API TIC_WARN_UNUSED
tic_error * tic_device_copy(
  const tic_device * source,
  tic_device ** dest);

/// Frees a device object.  Passing a NULL pointer to this function is OK.  Do
/// not free the same non-NULL pointer twice.
TIC_API
void tic_device_free(tic_device *);

/// Gets the name of the device as an ASCII-encoded string.
/// The string will be valid at least as long as the device object.
/// If the device is NULL, returns an empty string.
TIC_API TIC_WARN_UNUSED
const char * tic_device_get_name(const tic_device *);

/// Gets the serial number of the device as an ASCII-encoded string.
/// The string will be valid at least as long as the device object.
/// If the device is NULL, returns an empty string.
TIC_API TIC_WARN_UNUSED
const char * tic_device_get_serial_number(const tic_device *);

/// Get an operating system-specific identifier for this device as an
/// ASCII-encoded string.  The string will be valid at least as long as the
/// device object.
/// If the device is NULL, returns an empty string.
TIC_API TIC_WARN_UNUSED
const char * tic_device_get_os_id(const tic_device *);

/// Gets the firmware version as a binary-coded decimal number (e.g. 0x0100
/// means "1.00").  We recommend using tic_get_firmware_version_string() instead
/// of this function if possible.
TIC_API TIC_WARN_UNUSED
uint16_t tic_device_get_firmware_version(const tic_device *);


// tic_handle ///////////////////////////////////////////////////////////////////

/// Represents an open handle that can be used to read and write data from a
/// device.
typedef struct tic_handle tic_handle;

/// Opens a handle to the specified device.  The handle must later be closed
/// with tic_handle_close().
///
/// On Windows, if another applications has a handle open already, then this
/// function will fail and the returned error will have code
/// ::TIC_ERROR_ACCESS_DENIED.
TIC_API TIC_WARN_UNUSED
tic_error * tic_handle_open(const tic_device *, tic_handle **);

/// Closes and frees the specified handle.  It is OK to pass NULL to this
/// function.  Do not close the same non-NULL handle twice.
TIC_API
void tic_handle_close(tic_handle *);

/// Gets the device object that corresponds to this handle.
/// The device object will be valid for at least as long as the handle.
TIC_API TIC_WARN_UNUSED
const tic_device * tic_handle_get_device(const tic_handle *);

/// Gets the firmware version string, including any special modification codes
/// (e.g. "1.07nc").  The string will be valid for at least as long as the device.
///
/// This function might need to do I/O, but it will cache the result so it is OK
/// to call it frequently.
TIC_API TIC_WARN_UNUSED
const char * tic_get_firmware_version_string(tic_handle *);

/// Reads all of the Tic's non-volatile settings and returns them as an object.
/// The caller must free the settings later by calling tic_settings_free().
TIC_API TIC_WARN_UNUSED
tic_error * tic_get_settings(tic_handle *, tic_settings **);

/// Writes all of the Tic's non-volatile settings.  This function calls
/// tic_settings_fix() to fix any inconsistencies in the settings, so the
/// settings might be modified by the function.
TIC_API TIC_WARN_UNUSED
tic_error * tic_set_settings(tic_handle *, tic_settings *);

/// \cond
TIC_API TIC_WARN_UNUSED
tic_error * tic_get_debug_data(tic_handle *, uint8_t * data, size_t * size);
/// \endcond

#ifdef __cplusplus
}
#endif
