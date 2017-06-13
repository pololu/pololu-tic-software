// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

/// \file tic.h
///
/// This file provides the C API for libpololu-tic, a library for configuring
/// and controlling the Pololu Tic USB Stepper Motor Controller over USB.
///
/// If you want to communicate with the Tic over USB, you will need these
/// functions:
///
/// - tic_list_connected_devices()
/// - tic_handle_open()
/// - tic_list_free()
/// - tic_handle_close()
/// - tic_device_free()
///
/// If you want to control the position or speed of a stepper motor over USB,
/// see these functions:
///
/// - tic_set_target_speed()
/// - tic_get_target_position()
///
/// If you want to get the current status of the Tic, include analog and digital
/// readings from its pins, the current position of the stepper motor, and any
/// errors, see these functions:
///
/// - tic_get_variables()
/// - tic_variables_free()
///
/// If you want to change the settings of the Tic, see these functions:
///
/// - tic_get_settings()
/// - tic_set_settings()
/// - tic_restore_defaults()
/// - tic_reinitialize()
/// - tic_settings_free()
///
/// Most library functions return a tic_error pointer if an error happens.  To
/// avoid memory leaks, you must free the error when that happens.  You can use
/// these functions to get detailed error information and free the error:
///
/// - tic_error_get_message()
/// - tic_error_has_code()
/// - tic_error_free()
///
/// Many of the functions in this file refer to numeric constant macros defined
/// in tic_protocol.h.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "tic_protocol.h"

#define TIC_PRODUCT_T825 1

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

// TODO: uint32_t tic_ez_enable_driver(); ?
// TODO: uint32_t tic_ez_set_target_speed(int32_t);
// TODO: uint32_t tic_ez_set_target_position(int32_t);

/// Certain functions in the library return a newly-created string and require
/// the caller to call this function to free the string.  Passing a NULL pointer
/// to this function is OK.  Do not free the same non-NULL string twice.
void tic_string_free(char *);


// Name lookup //////////////////////////////////////////////////////////////////

/// Looks up a user-friendly string corresponding to the specified error bit,
/// e.g. "Safe start violation".  The error argument should be of the form (1 <<
/// x) where x is one of the TIC_ERROR_* macros, but if it is not, this function
/// returns an empty string.  The returned string will be valid indefinitely and
/// should not be freed.
TIC_API
const char * tic_look_up_nice_error_string(uint32_t error);

/// Looks up the string corresponding to the specified decay mode, e.g. "mixed".
/// The decay_mode argument should be one of the TIC_DECAY_MODE_* macros, but if
/// it is not, this function returns an empty string.  The returned string will
/// be valid indefinitely and should not be freed.
TIC_API
const char * tic_look_up_decay_mode_string(uint8_t decay_mode);

/// Looks up the string corresponding to the specified device reset,
/// e.g. "stack_underflow".  The device_reset argument should be one of the
/// TIC_RESET_* macros, but if it is not, this function returns an empty string.
/// The returned string will be valid indefinitely and should not be freed.
TIC_API
const char * tic_look_up_device_reset_string(uint8_t device_reset);

/// Looks up the string corresponding to the specified operation state,
/// e.g. "holding".  The operation_state argument should be one of the
/// TIC_OPERATION_STATE_* macros, but if it is not, this function returns an
/// empty string.  The returned string will be valid indefinitely and should not
/// be freed.
TIC_API
const char * tic_look_up_operation_state_string(uint8_t operation_state);

/// Looks up the string corresponding to the specified step mode,
/// e.g. "microstep32".  The step_mode argument should be one of the
/// TIC_STEP_MODE_* macros, but if it is not, this functions returns an empty
/// string.  The returned string will be valid indefinitely and should not be
/// freed.
TIC_API
const char * tic_look_up_step_mode_string(uint8_t step_mode);

/// Looks up the string corresponding to the specified pin state,
/// e.g. "output_low".  The pin_state argument should be one of the
/// TIC_PIN_STATE_* macros, but if it is not, this functions returns an empty
/// string.  The returned string will be valid indefinitely and should not be
/// freed.
TIC_API
const char * tic_look_up_pin_state_string(uint8_t pin_state);

/// Looks up the string corresponding to the specified planning mode,
/// e.g. "target_position".  The pin_state argument should be one of the
/// TIC_PLANNING_MODE_* macros, but if it is not, this functions returns an empty
/// string.  The returned string will be valid indefinitely and should not be
/// freed.
TIC_API
const char * tic_look_up_planning_mode_string(uint8_t planning_mode);


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


// tic_variables ///////////////////////////////////////////////////////////////

typedef struct tic_variables tic_variables;

/// Copies a tic_variables object.  If this function is successful, the caller must
/// free the settings later by calling tic_settings_free().
TIC_API TIC_WARN_UNUSED
tic_error * tic_variables_copy(
  const tic_variables * source,
  tic_variables ** dest);

/// Frees a tic_variables object.  It is OK to pass a NULL pointer to this
/// function.  Do not free the same non-NULL settings object twice.
TIC_API
void tic_variables_free(tic_variables *);

/// Returns the current operation state of the Tic, which will be one of the
/// TIC_OPERATION_STATE_* macros.
TIC_API
uint8_t tic_variables_get_operation_state(const tic_variables *);

/// Gets the errors that are currently stopping the motor.
///
/// This is a bitmask, and the bits are defined by the TIC_ERROR_* macros.
TIC_API
uint16_t tic_variables_get_error_status(const tic_variables *);

/// Gets the errors that have occurred since the last time these error occurred
/// bits were cleared.
///
/// This is a bitmask, and the bits are defined by the TIC_ERROR_* macros.  To
/// learn more about what the different errors mean, see the Tic user's guide.
TIC_API
uint32_t tic_variables_get_errors_occurred(const tic_variables *);

/// Gets the current planning mode.
///
/// This will be either TIC_PLANNING_MODE_OFF,
/// TIC_PLANNING_MODE_TARGET_POSITION, or TIC_PLANNING_MODE_TARGET_VELOCITY.
TIC_API
uint8_t tic_variables_get_planning_mode(const tic_variables *);

/// Gets the target position, in microsteps.
///
/// This is only relevant if the planning mode from
/// tic_variables_get_planning_mode() is TIC_PLANNING_MODE_TARGET_POSITION.
TIC_API
int32_t tic_variables_get_target_position(const tic_variables *);

/// Gets the target velocity, in microsteps per 10000 seconds.
///
/// This is only relevant if the planning mode from
/// tic_variables_get_planning_mode() is TIC_PLANNING_MODE_TARGET_VELOCITY.
TIC_API
int32_t tic_variables_get_target_velocity(const tic_variables *);

/// Gets the current speed minimum, or starting speed, in microsteps per 10000 seconds.
///
/// This is the current value.  To get the default value at startup, see
/// tic_settings_get_speed_min().
TIC_API
uint32_t tic_variables_get_speed_min(const tic_variables *);
// TODO: equivalent cross-referencing comments for tic_settings accessors

/// Gets the current speed maximum, in microsteps per 10000 seconds.
///
/// This is the current value.  To get the default value at startup, see
/// tic_settings_get_speed_max().
TIC_API
uint32_t tic_variables_get_speed_max(const tic_variables *);

/// Gets the current maximum deceleration, in microsteps per 100 square seconds.
///
/// This is the current value.  To get the default value at startup, see
/// tic_settings_get_decel_max().
TIC_API
uint32_t tic_variables_get_decel_max(const tic_variables *);

/// Gets the current maximum acceleration, in microsteps per 100 square seconds.
///
/// This is the current value.  To get the default value at startup, see
/// tic_settings_get_accel_max().
TIC_API
uint32_t tic_variables_get_accel_max(const tic_variables *);

/// Gets the current position of the stepper motor, in microsteps.
///
/// Note that this just tracks steps that the Tic has commanded the stepper
/// driver to take, it could be different from the actual position of the motor
/// for various reasons.
TIC_API
int32_t tic_variables_get_current_position(const tic_variables *);

/// Gets the current velocity of the stepper motor, in microsteps for 10000
/// seconds.
///
/// Note that this is just the velocity used in the Tic's step planning
/// algorithms, and it might not correspond to the actual velocity of the motor
/// for various reasons.
TIC_API
int32_t tic_variables_get_current_velocity(const tic_variables *);

/// Gets the acting target position, in microsteps.
///
/// This is a variable used in the Tic's target position step planning
/// algorithm.
TIC_API
int32_t tic_variables_get_acting_target_position(const tic_variables *);

/// Gets the time since the last step, in timer ticks.
///
/// Each timer tick represents one third of a microsecond.  The Tic only updates
/// this variable every 5 milliseconds or so.
TIC_API
uint32_t tic_variables_get_time_since_last_step(const tic_variables *);

/// Gets the cause of the controller's last reset.
///
/// This will be one the TIC_RESET_* macros.
TIC_API
uint8_t tic_variables_get_device_reset(const tic_variables *);

/// Gets the current measurement of the VIN voltage, in millivolts.
TIC_API
uint32_t tic_variables_get_vin_voltage(const tic_variables *);

/// Gets the time since the controller was last reset, in milliseconds.
TIC_API
uint32_t tic_variables_get_up_time(const tic_variables *);

/// Gets the reading from the quadrature encoder input, in ticks.
TIC_API
int32_t tic_variables_get_encoder_position(const tic_variables *);

/// Gets the reading from the RC pulse input, in units of twelfth microseconds.
TIC_API
uint16_t tic_variables_get_rc_pulse_width(const tic_variables *);

/// Gets the current step mode of the Tic.
///
/// Note that this is the current mode.  To get the default mode at startup,
/// see tic_settings_get_step_mode().
///
/// The return value is one of the TIC_STEP_MODE_* macros.
TIC_API
uint8_t tic_variables_get_step_mode(const tic_variables *);

/// Gets the stepper motor coil current limit in milliamps.
///
/// Note that this is the current limit being used at the moment.  To get the
/// default current limit at startup, see tic_settings_get_current_limit().
TIC_API
uint32_t tic_variables_get_current_limit(const tic_variables *);

/// Gets the current decay mode of the Tic.
///
/// Note that this is the current decay mode.  To get the default decay mode at
/// startup, see tic_settings_get_decay_mode().
///
/// The return value is one of the TIC_DECAY_MODE_* macros.
TIC_API
uint8_t tic_variables_get_decay_mode(const tic_variables *);

/// Gets the analog reading from the specified pin, if analog readings are
/// enabled for that pin.
///
/// The pin argument should be one of the TIC_PIN_NUM_* macros.
///
/// The return value will be a left-justified analog reading; a value of 0
/// represents 0 V and a value of 0xFFFF represents approximately the voltage on
/// the controller's 5V pin.
TIC_API
uint16_t tic_variables_get_analog_reading(const tic_variables *, uint8_t pin);

/// Gets the digital reading for the specified pin.
///
/// The pin argument should be one of the TIC_PIN_NUM_* macros.
TIC_API
bool tic_variables_get_digital_reading(const tic_variables *, uint8_t pin);

/// Gets the switch status for the specified pin.
///
/// This will be false for pins not configured as switches.  It will be true for
/// pins that are configured as switches and which are in the active position.
///
/// The pin argument should be one of the TIC_PIN_NUM_* macros.
TIC_API
bool tic_variables_get_switch_status(const tic_variables *, uint8_t pin);

/// Gets the pin state for the specified pin.
///
/// This pin argument should be one of the TIC_PIN_NUM_* macros.
///
/// The return value is one of the TIC_PIN_STATE_* macros.
TIC_API
uint8_t tic_variables_get_pin_state(const tic_variables *, uint8_t pin);


// tic_settings ////////////////////////////////////////////////////////////////

/// Represents the settings for a Tic.  This object is just plain old data; it
/// does not have any pointers or handles for other resources.
typedef struct tic_settings tic_settings;

/// Creates a new settings object.
///
/// The new settings object will have no product specified and all settings set
/// to zero.  After creating the settings object, you would typically call
/// tic_settings_product_set() and then tic_settings_fill_with_defaults().
///
/// Then you would use setter and getter methods to work with the settings.  At
/// some point, you should call tic_settings_fix() to make sure the settings are
/// valid and provide warnings about what settings were invalid.
///
/// The settings parameter should be a non-null pointer to a tic_settings
/// pointer, which will receive a pointer to a new settings object if and only
/// if this function is successful.  The caller must free the settings later by
/// calling tic_settings_free().
TIC_API TIC_WARN_UNUSED
tic_error * tic_settings_create(tic_settings ** settings);

/// Copies a tic_settings object.  If this function is successful, the caller must
/// free the settings later by calling tic_settings_free().
TIC_API TIC_WARN_UNUSED
tic_error * tic_settings_copy(
  const tic_settings * source ,
  tic_settings ** dest);

/// Frees a tic_settings object.  It is OK to pass a NULL pointer to this
/// function.  Do not free the same non-NULL settings object twice.
TIC_API
void tic_settings_free(tic_settings *);

/// Fixes the settings to have defaults.  Before calling this, you should
/// specify what product the settings are for by calling
/// tic_settings_set_product().  If the product is not set to a valid non-zero
/// value, this function will do nothing (and you can later see a warning from
/// tic_settings_fix()).
void tic_settings_fill_with_defaults(tic_settings * settings);

/// Fixes the settings to valid and consistent.
///
/// The warnings parameters is an optional pointer to pointer to a string.  If
/// you supply the warnings parameter, and this function is successful, this
/// function will allocate and return a string using the parameter.  The string
/// will describe what was fixed in the settings or be empty if nothing was
/// fixed.  Each distinct warning in the string will be a series of complete
/// English sentences that ends with a newline character.  The string must be
/// freed by the caller using tic_string_free().
TIC_API TIC_WARN_UNUSED
tic_error * tic_settings_fix(tic_settings *, char ** warnings);

/// Gets the settings as a YAML string, also known as a settings file.  If this
/// function is successful, the string must be freed by the caller using
/// tic_string_free().
TIC_API TIC_WARN_UNUSED
tic_error * tic_settings_to_string(const tic_settings *, char ** string);

/// Parses an YAML settings string, also known as a settings file, and returns
/// the corresponding settings object.  The settings returned might be invalid,
/// so it is recommend to call tic_settings_fix() to fix the settings and warn
/// the user.
///
/// The settings parameter should be a non-null pointer to a tic_settings
/// pointer, which will receive a pointer to a new settings object if and only
/// if this function is successful.  The caller must free the settings later by
/// calling tic_settings_free().
///
/// The warnings parameters is an optional pointer to pointer to a string.  If
/// you supply the warnings parameter, and this function is successful, this
/// function will allocate and return a string using the parameter.  The string
/// will describe what was fixed in the settings or be empty if nothing was
/// fixed.  Each distinct warning in the string will be a series of complete
/// English sentences that ends with a newline character.  The string must be
/// freed by the caller using tic_string_free().
TIC_API TIC_WARN_UNUSED
tic_error * tic_settings_read_from_string(const char * string,
  tic_settings ** settings, char ** warnings);

// TODO: how about we move set and get to the beginning of these
// accessor function names like normal people: tic_settings_get_product.
// Consistent with tic_get_settings.

/// Sets the product, which specifies what Tic product these settings are for.
/// The value should be one of the TIC_PRODUCT_* macros.
TIC_API
void tic_settings_product_set(tic_settings *, uint8_t product);

/// Gets the product described in tic_settings_product_set().
TIC_API
uint8_t tic_settings_product_get(const tic_settings *);

/// Sets the control mode, which should be one of the TIC_CONTROL_MODE_* macros.
/// Silently obeys if the input is invalid so that you can see a warning later
/// when calling tic_settings_fix.
TIC_API
void tic_settings_control_mode_set(tic_settings *, uint8_t control_mode);

/// Returns the control mode described in tic_settings_control_mode_set().
TIC_API
uint8_t tic_settings_control_mode_get(const tic_settings *);

/// Sets the never sleep (ignore USB suspend) setting.  If true, prevents the
/// device from going into deep sleep mode in order to comply with suspend
/// current requirements of the USB specifications.
TIC_API
void tic_settings_never_sleep_set(tic_settings *, bool never_sleep);

/// Gets the Never sleep (ignore USB suspend) setting described in
/// tic_settings_never_sleep_set().
TIC_API
bool tic_settings_never_sleep_get(const tic_settings *);

/// Sets the disable safe start setting.  If true, it disables some extra
/// conditions that are required to let the device start moving the motor.
TIC_API
void tic_settings_disable_safe_start_set(tic_settings *, bool);

/// Gets the disable safe start setting described in
/// tic_settings_disable_safe_start_get().
TIC_API
bool tic_settings_disable_safe_start_get(const tic_settings *);

/// Sets the ignore ERR line high setting.  If true, allows the device to
/// operate even if the ERR line is being driven high by something.
TIC_API
void tic_settings_ignore_err_line_high_set(tic_settings *, bool);

/// Gets the ignore ERR line high setting described in
/// tic_settings_ignore_err_line_high_set().
TIC_API
bool tic_settings_ignore_err_line_high_get(const tic_settings *);

/// Sets the baud rate n bits per second.
///
/// Only certain baud rates are actually achievable.  This function will change
/// the supplied baud rate to the nearest achievable baud rate.  You can call
/// tic_settings_serial_baud_rate_get() to see what baud rate was used.
TIC_API
void tic_settings_serial_baud_rate_set(tic_settings *, uint32_t);

/// Returns an estimate of the baud rate the controller will use for its
/// asynchronous serial port, in bits per second.
TIC_API
uint32_t tic_settings_serial_baud_rate_get(const tic_settings *);

/// Sets the serial device number, a number between 0 and 0x7F that is used to
/// identify the device when using the Pololu protocol.
TIC_API
void tic_settings_serial_device_number_set(tic_settings *, uint8_t);

/// Gets the serial device number setting described in
/// tic_settings_serial_device_number_set().
TIC_API
uint8_t tic_settings_serial_device_number_get(const tic_settings *);

/// Sets the I2C address to use for I2C slave communication.
TIC_API
void tic_settings_i2c_address_set(tic_settings *, uint8_t);

/// Gets the I2C address to use for I2C slave communication.
TIC_API
uint8_t tic_settings_i2c_address_get(const tic_settings *);

/// Sets the command timeout, the time in milliseconds before the device
/// considers it to be an error if we have not received certain commands.  A
/// value of 0 disables the command timeout feature.
TIC_API
void tic_settings_command_timeout_set(tic_settings *, uint16_t);

/// Gets the command timeout setting described in
/// tic_settings_command_timeout_set().
TIC_API
uint16_t tic_settings_command_timeout_get(const tic_settings *);

/// Sets the serial CRC enabled setting.  If true, the device requires 7-bit CRC
/// bytes on all serial commands.
TIC_API
void tic_settings_serial_crc_enabled_set(tic_settings *, bool);

/// Gets the serial CRC enabled setting described in
/// tic_settings_serial_crc_enabled_set().
TIC_API
bool tic_settings_serial_crc_enabled_get(const tic_settings *);

/// Sets the low VIN shutoff timeout.  This determines how long, in milliseconds,
/// it takes for a low reading on VIN to be interpreted as an error.
TIC_API
void tic_settings_low_vin_timeout_set(tic_settings *, uint16_t);

/// Gets the low VIN shutoff timeout setting described in
/// tic_settings_vin_shutoff_timeout_set().
TIC_API
uint16_t tic_settings_low_vin_timeout_get(const tic_settings *);

/// Sets the low VIN shutoff voltage.  This determines how low, in millivolts,
/// the VIN reading has to be for the controller to change from considering it
/// to be adequate to considering it to be too low.
TIC_API
void tic_settings_low_vin_shutoff_voltage_set(tic_settings *, uint16_t);

/// Gets the low VIN shutoff voltage setting described in
/// tic_settings_low_vin_shutoff_voltage_set().
TIC_API
uint16_t tic_settings_low_vin_shutoff_voltage_get(const tic_settings *);

/// Sets the Low VIN startup voltage.  This determines how high, in millivolts,
/// the VIN reading has to be for the controller to change from considering it
/// to be too low to considering it to be adequate.
TIC_API
void tic_settings_low_vin_startup_voltage_set(tic_settings *, uint16_t);

/// Gets the low VIN startup voltage setting described in
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
uint16_t tic_settings_vin_multiplier_offset_get(const tic_settings *);

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

/// Sets the RC consecutive good pulses setting.  This is the number of
/// consecutive good pulses that must be received before the controller starts
/// using good pulses to control the motor.  A value of 2 means that after 2
/// good pulses in a row are received, the third one will be used to control the
/// motor.  A value of 0 means that every good pulse results in an update of the
/// motor.
TIC_API
void tic_settings_rc_consecutive_good_pulses_set(tic_settings *, uint8_t);

/// Gets the RC consecutive good pulses setting described in
/// tic_settings_rc_consecutive_good_pulses_set().
TIC_API
uint8_t tic_settings_rc_consecutive_good_pulses_get(const tic_settings *);

/// Sets the input error minimum parameter.  In analog or RC control
/// mode, values below this will cause an error.
TIC_API
void tic_settings_input_error_min_set(tic_settings *, uint16_t);

/// Gets the input error minimum parameter.  See
/// tic_settings_error_min_set().
TIC_API
uint16_t tic_settings_input_error_min_get(const tic_settings *);

/// Sets the input error maximum parameter.  In analog or RC control
/// mode, values above this will cause an error.
TIC_API
void tic_settings_input_error_max_set(tic_settings *, uint16_t);

/// Gets the input error maximum parameter.  See
/// tic_settings_error_max_set().
TIC_API
uint16_t tic_settings_input_error_max_get(const tic_settings *);

/// Sets the input play.  The controller implements a filter on the raw
/// input value (from RC pulses, encoders, or analog) that allows you to make
/// noisy inputs have a constant value when they are not moving.  When the raw
/// input value changes, the filtered input value will only change by the
/// minimum amount necessary to ensure that the magnitude of the difference
/// between the filtered input value and the raw input value is no more than the
/// play value.
TIC_API
void tic_settings_input_play_set(tic_settings *, uint8_t);

/// Gets the input play setting described in tic_settings_input_play_set().
TIC_API
uint8_t tic_settings_input_play_get(const tic_settings *);

/// Sets the input scaling degree.  0 is linear, 1 is quadratic, 2 is cubic.
TIC_API
void tic_settings_input_scaling_degree_set(tic_settings *, uint8_t);

/// Gets the input scaling degree setting described in
/// tic_settings_input_scaling_degree_set().
TIC_API
uint8_t tic_settings_input_scaling_degree_get(const tic_settings *);

/// Sets the input invert setting.  See tic_settings_output_max_set().
TIC_API
void tic_settings_input_invert_set(tic_settings *, bool);

/// Gets the input invert setting described in
/// tic_settings_input_invert_set().
TIC_API
bool tic_settings_input_invert_get(const tic_settings *);

/// Sets the input minimum scaling parameter.  See
/// tic_settings_output_max_set().
TIC_API
void tic_settings_input_min_set(tic_settings *, uint16_t);

/// Gets the input minimum scaling parameter.  See
/// tic_settings_output_max_set().
TIC_API
uint16_t tic_settings_input_min_get(const tic_settings *);

/// Sets the input neutral minimum scaling parameter.  See
/// tic_settings_output_max_set().
TIC_API
void tic_settings_input_neutral_min_set(tic_settings *, uint16_t);

/// Gets the input neutral minimum scaling parameter.  See
/// tic_settings_output_max_set().
TIC_API
uint16_t tic_settings_input_neutral_min_get(const tic_settings *);

/// Sets the input neutral maximum scaling parameter.  See
/// tic_settings_output_max_set().
TIC_API
void tic_settings_input_neutral_max_set(tic_settings *, uint16_t);

/// Gets the input neutral maximum scaling parameter.  See
/// tic_settings_output_max_set().
TIC_API
uint16_t tic_settings_input_neutral_max_get(const tic_settings *);

/// Sets the input maximum scaling parameter.  See
/// tic_settings_output_max_set().
TIC_API
void tic_settings_input_max_set(tic_settings *, uint16_t);

/// Gets the input maximum scaling parameter.  See
/// tic_settings_output_max_set().
TIC_API
uint16_t tic_settings_input_max_get(const tic_settings *);

/// Sets the output minimum scaling parameter.  See
/// tic_settings_output_max_set().
TIC_API
void tic_settings_output_min_set(tic_settings *, int32_t);

/// Gets the output minimum scaling parameter.  See
/// tic_settings_output_max_set().
TIC_API
int32_t tic_settings_output_min_get(const tic_settings *);

/// Sets the output neutral scaling parameter.  See
/// tic_settings_output_max_set().
TIC_API
void tic_settings_output_neutral_set(tic_settings *, int32_t);

/// Gets the output neutral scaling parameter.  See
/// tic_settings_output_max_set().
TIC_API
int32_t tic_settings_output_neutral_get(const tic_settings *);

/// Sets the output maximum scaling parameter.
///
/// The following functions control the settings that define how an RC or analog
/// input gets mapped to a stepper motor position or speed:
///
/// - tic_settings_input_invert_set()
/// - tic_settings_input_min_set()
/// - tic_settings_input_neutral_min_set()
/// - tic_settings_input_neutral_max_set()
/// - tic_settings_input_max_set()
/// - tic_settings_output_min_set()
/// - tic_settings_output_neutral_set()
/// - tic_settings_output_max_set()
///
/// Input values between less than or equal to the input minimum are mapped to
/// the output minimum (or output maximum if the input is inverted).
///
/// Input values between the input minimum and input neutral minimum map to an
/// output value between the output minimum (or the output maximum if the input
/// is inverted) and the output neutral.
///
/// Input values between the input neutral minimum and input netural maximum map
/// to output_neutral.
///
/// Input values between the input neutral maximum and input maximum map to an
/// output value between the output neutral and output maximum (or output
/// minimum if the input is inverted).
///
/// Input values greater than or equal to the input maximum map to an output
/// value of output_max.
TIC_API
void tic_settings_output_max_set(tic_settings *, int32_t);

/// Gets the output maximum scaling parameter.  See
/// tic_settings_output_max_set().
TIC_API
int32_t tic_settings_output_max_get(const tic_settings *);

/// Sets the encoder prescaler.  For encoder control mode, this determines the
/// number of counts from the encoder per unit change of the stepper motor
/// position or speed.  The number of counts is 2 raised to encoder_prescaler
/// value.  The allowed values are 0 through 8.
TIC_API
void tic_settings_encoder_prescaler_set(tic_settings *, uint8_t);

/// Gets the encoder prescaler described in tic_settings_encoder_prescaler_set().
TIC_API
uint8_t tic_settings_encoder_prescaler_get(const tic_settings *);

/// Sets the encoder postscaler.  For encoder control mode, this determines the
/// size of a unit change in the stepper motor position or speed.  This cannot
/// be negative, but the input_invert setting can be used.
TIC_API
void tic_settings_encoder_postscaler_set(tic_settings *, uint32_t);

/// Gets the encoder postscaler described in tic_settings_encoder_postscaler_set().
TIC_API
uint32_t tic_settings_encoder_postscaler_get(const tic_settings *);

/// Sets the pin configuration for the SCL pin.  See
/// tic_settings_rc_config_set().
TIC_API
void tic_settings_scl_config_set(tic_settings *, uint8_t);

/// Gets the pin configuration for the SCL pin.  See
/// tic_settings_rc_config_set().
TIC_API
uint8_t tic_settings_scl_config_get(const tic_settings *);

/// Sets the pin configuration for the SDA/AN pin.  See
/// tic_settings_rc_config_set().
TIC_API
void tic_settings_sda_config_set(tic_settings *, uint8_t);

/// Gets the pin configuration for the SDA/AN pin.  See
/// tic_settings_rc_config_set().
TIC_API
uint8_t tic_settings_sda_config_get(const tic_settings *);

/// Sets the pin configuration for the TX pin.  See
/// tic_settings_tx_config_set().
TIC_API
void tic_settings_tx_config_set(tic_settings *, uint8_t);

/// Gets the pin configuration for the TX pin.  See
/// tic_settings_rc_config_set().
TIC_API
uint8_t tic_settings_tx_config_get(const tic_settings *);

/// Sets the pin configuration for the RX pin.  See
/// tic_settings_rx_config_set().
TIC_API
void tic_settings_rx_config_set(tic_settings *, uint8_t);

/// Gets the pin configuration for the RX pin.  See
/// tic_settings_rc_config_set().
TIC_API
uint8_t tic_settings_rx_config_get(const tic_settings *);

/// Sets the pin configuration for the RC pin.
///
/// There are several settings packed into the 8-bit configuration register.
///
/// bit 7 (TIC_PIN_SERIAL_OR_I2C) - use pin for serial or I2C
/// bit 6 (TIC_PIN_PULLUP) - enable internal pull-up
/// bit 5 (TIC_PIN_ANALOG) - enable analog readings
/// bit 4 - reserved, set to 0
/// bit 3 (TIC_PIN_ACTIVE_HIGH) - if the pin is used as a switch, this bit
///   specifies the switch polarity: 0 = active low, 1 = active high.
/// bits 2,1,0 (TIC_PIN_SWITCH_POSN)-
///   000 (TIC_PIN_SWITCH_NONE) = nothing
///   001 (TIC_PIN_SWITCH_LIMIT_FORWARD) = limit forward
///   010 (TIC_PIN_SWITCH_LIMIT_REVERSE) = limit reverse
///   011 (TIC_PIN_SWITCH_LIMIT_HOME) = home switch
///   100 (TIC_PIN_SWITCH_LIMIT_KILL) = kill switch
///
/// Some bits are not valid on some pins, and certain combinations of bits are
/// not allowed.  See the source for of tic_settings_fix() for details.
TIC_API
void tic_settings_rc_config_set(tic_settings *, uint8_t);

/// Gets the pin configuration for the RC pin.  See
/// tic_settings_rc_config_set().
TIC_API
uint8_t tic_settings_rc_config_get(const tic_settings *);

/// Sets the default stepper motor coil current limit in milliamps.
///
/// Like the baud rate setting, only certain current limit values are actually
/// achievable.  This function will use the highest achievable current level
/// that is below the given the argument.  You can call
/// tic_settings_current_limit_get() after calling this function to see what
/// current level was used.
TIC_API
void tic_settings_current_limit_set(tic_settings *, uint32_t);

/// Gets the current limit settings as described in
/// tic_settings_current_limit_set().
TIC_API
uint32_t tic_settings_current_limit_get(const tic_settings *);

/// Sets the default step mode, also known as the microstepping mode.  This
/// should be one of the TIC_STEP_MODE_* macros, but not all step modes are
/// supported on all products.  If you specify an invalid or unsupported mode to
/// this function, you can fix it with tic_settings_fix().
TIC_API
void tic_settings_step_mode_set(tic_settings *, uint8_t);

/// Gets the step mode described in
/// tic_settings_step_mode_set().
TIC_API
uint8_t tic_settings_step_mode_get(const tic_settings *);

/// Sets the decay mode.  The argument should be TIC_DECAY_MIXED,
/// TIC_DELAY_SLOW, or TIC_DECAY_FAST.
TIC_API
void tic_settings_decay_mode_set(tic_settings *, uint8_t);

/// Gets the decay mode described in tic_settings_decay_mode_get().
TIC_API
uint8_t tic_settings_decay_mode_get(const tic_settings *);

/// Sets the speed minimum, or starting speed, in steps per 10000 seconds.
/// Valid values are from -500000000 to 500000000.
TIC_API
void tic_settings_speed_min_set(tic_settings *, uint32_t);

/// Gets the speed minimum described in tic_settings_speed_min_set().
TIC_API
uint32_t tic_settings_speed_min_get(const tic_settings *);

/// Sets the speed maximum, or speed limit, in steps per 10000 seconds.  Valid
/// values are from 0 to 500000000 (50 kHz).  Values between 0 and 6 behave the
/// same as 0.
TIC_API
void tic_settings_speed_max_set(tic_settings *, uint32_t);

/// Gets the speed maximum described in tic_settings_speed_max_set().
TIC_API
uint32_t tic_settings_speed_max_get(const tic_settings *);

/// Sets the deceleration maximum, or deceleration limit, in steps per 100
/// square seconds.  Valid values are 0 and 59 to 2147483647.  A value of 0
/// causes the controller's deceleration limit to be equal to the acceleration
/// limit.
TIC_API
void tic_settings_decel_max_set(tic_settings *, uint32_t);

/// Gets the deceleration maximum described in tic_settings_decel_max_set().
TIC_API
uint32_t tic_settings_decel_max_get(const tic_settings *);

/// Sets the acceleration maximum, or acceleration limit, in steps per 100
/// square seconds.  Valid values are 59 to 2147483647.
TIC_API
void tic_settings_accel_max_set(tic_settings *, uint32_t);

/// Gets the acceleration maximum described in tic_settings_accel_max_set().
TIC_API
uint32_t tic_settings_accel_max_get(const tic_settings *);

/// Sets the deceleration maximum, or deceleration limit, that is used when the
/// controller is stopping the motor due to an error, in steps per 100 square
/// seconds.  Valid values are 0 and 59 to 2147483647.  A value of 0 causes the
/// controller's deceleration limit to be equal to the acceleration limit.
TIC_API
void tic_settings_decel_max_during_error_set(tic_settings *, uint32_t);

/// Gets the deceleration maximum described in tic_settings_decel_max_set().
TIC_API
uint32_t tic_settings_decel_max_during_error_get(const tic_settings *);


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

/// Frees a device list returned by ::tic_list_connected_devices.  It is OK to
/// pass NULL to this function.
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

/// Gets the full name of the device (e.g. "Tic USB Stepper Motor Controller
/// T825") as an ASCII-encoded string.  The string will be valid at least as
/// long as the device object.
/// If the device is NULL, returns an empty string.
TIC_API TIC_WARN_UNUSED
const char * tic_device_get_name(const tic_device *);

/// Gets the short name of the device (e.g. "T825") as an ASCII-encoded string.
/// The string will be valid at least as long as the device object.
/// If the device is NULL, returns an empty string.
TIC_API TIC_WARN_UNUSED
const char * tic_device_get_short_name(const tic_device *);

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

/// Sets the target position of the Tic, in microsteps.
///
/// This function sends a Set Target Position to the Tic.  If the Control mode
/// is set to Serial, the Tic will start moving the motor to reach the target
/// position.  If the control mode is something other than Serial, this command
/// will be silently ignored.
TIC_API TIC_WARN_UNUSED
tic_error * tic_set_target_position(tic_handle *, int32_t position);

/// Sets the target velocity of the Tic, in microsteps per 10000 seconds.
///
/// This function sends a Set Target Velocity command to the Tic.  If the
/// Control mode is set to Serial, the Tic will start accelerating or
/// decelerating to reach the target velocity.  If the control mode is something
/// other than Serial, this command will be silently ignored.
TIC_API TIC_WARN_UNUSED
tic_error * tic_set_target_velocity(tic_handle *, int32_t velocity);

/// Sets the current position, e.g. the position where the Tic currently thinks
/// it is.
///
/// This functions sends a Set Current Position command to the Tic.
///
/// Do not call this if the stepper motor is moving, or else you could get
/// unexpected behavior from the controller.
TIC_API TIC_WARN_UNUSED
tic_error * tic_set_current_position(tic_handle *, int32_t position);

/// Stops the Tic abruptly without respecting the deceleration limits.
///
/// This function sends a Stop command to the Tic.  If the Control mode is set
/// to Serial, the Tic will stop abruptly.  If the control mode is something
/// other than Serial, ths command will be silently ignored.
TIC_API TIC_WARN_UNUSED
tic_error * tic_stop(tic_handle *);

/// Enables the stepper motor driver.
///
/// This function sends an Enable Driver command to the Tic, causing it to
/// enable its stepper driver if it was previously disabled.  This command only
/// works if the control mode setting is set to Serial or STEP/DIR.  The effect
/// only lasts until the Tic is powered off, reset, or reinitialized, or a
/// Disable Driver command is issued.
TIC_API TIC_WARN_UNUSED
tic_error * tic_enable_driver(tic_handle *);

/// Disables the stepper motor driver.
///
/// This function sends a Disable Driver command to the Tic, causing it to
/// disable its stepper driver.  This command only works if the control mode
/// setting is set to Serial or STEP/DIR.  The effect only lasts until the Tic
/// is powered off, reset, or reinitialized, or an Enable Driver command is
/// issued.
TIC_API TIC_WARN_UNUSED
tic_error * tic_disable_driver(tic_handle *);

/// Temporarily sets the speed maximum.
///
/// This function sends a Set Speed Max command to the Tic.  For more
/// information on what the Speed Max is, see tic_settings_get_speed_max().
TIC_API TIC_WARN_UNUSED
tic_error * tic_set_speed_max(tic_handle *, uint32_t speed_max);

/// Temporarily sets the speed minimum.
///
/// This function sends a Set Speed Min command to the Tic.  For more
/// information on what the Speed Min is, see tic_settings_get_speed_min().
TIC_API TIC_WARN_UNUSED
tic_error * tic_set_speed_min(tic_handle *, uint32_t speed_min);

/// Temporarily sets the acceleration maximum.
///
/// This function sends a Set Acceleration Max command to the Tic.  For more
/// information on what the Acceleration Max is, see
/// tic_settings_get_accel_max().
TIC_API TIC_WARN_UNUSED
tic_error * tic_set_accel_max(tic_handle *, uint32_t accel_max);

/// Temporarily sets the deceleration maximum.
///
/// This function sends a Set Deceleration Max command to the Tic.  For more
/// information on what the Deceleration Max is, see
/// tic_settings_get_decel_max().
TIC_API TIC_WARN_UNUSED
tic_error * tic_set_decel_max(tic_handle *, uint32_t decel_max);

/// Temporarily sets the step mode.
///
/// The step_mode argument should be one of the TIC_STEP_MODE_* macros.
///
/// This function sends a Set Step Mode commands to the Tic.  The step mode will
/// stay in effect until the Tic is powered off, reset, or reinitialized, or
/// another Set Step Mode command is issued.  To set the step mode permanently,
/// see tic_settings_set_step_mode().
TIC_API TIC_WARN_UNUSED
tic_error * tic_set_step_mode(tic_handle *, uint8_t step_mode);

/// Temporarily sets the stepper motor coil current limit in milliamps.
///
/// This function sends a Set Current Limit command to the Tic.  This current
/// limit will stay in effect until the Tic is powered off, reset, or
/// reinitialized, or another Set Current Limit command is issued.  To set the
/// current limit permanently, see tic_settings_set_current_limit().
TIC_API TIC_WARN_UNUSED
tic_error * tic_set_current_limit(tic_handle *, uint32_t current_limit);

/// Temporarily sets the stepper motor decay mode.
///
/// The decay_mode argument should be TIC_DECAY_MODE_MIXED, TIC_DECAY_MODE_SLOW,
/// or TIC_DECAY_MODE_FAST.
///
/// This function sends a Set Decay Mode command to the Tic.  This decay mode
/// will stay in effect until the Tic is powered off, reset, or reinitialized,
/// or another Set Decay Mode command is issued.  To set the decay mode
/// permanently, see tic_settings_set_decay_mode().
TIC_API TIC_WARN_UNUSED
tic_error * tic_set_decay_mode(tic_handle *, uint8_t decay_mode);

/// Reads all of the Tic's status variables and returns them as an object.
///
/// The variables parameter should be a non-null pointer to a tic_variables
/// pointer, which will receive a pointer to a new variables object if and only
/// if this function is successful.  The caller must free the variables later by
/// calling tic_variables_free().
///
/// To read information from the variables object, see the tic_variables_get_*
/// functions.
TIC_API TIC_WARN_UNUSED
tic_error * tic_get_variables(tic_handle *, tic_variables ** variables);

/// Reads all of the Tic's non-volatile settings and returns them as an object.
///
/// The settings parameter should be a non-null pointer to a tic_settings
/// pointer, which will receive a pointer to a new settings object if and only
/// if this function is successful.  The caller must free the settings later by
/// calling tic_settings_free().
///
/// To access fields in the variables, see the tic_settings_* functions.
TIC_API TIC_WARN_UNUSED
tic_error * tic_get_settings(tic_handle *, tic_settings ** settings);

/// Writes all of the Tic's non-volatile settings.
///
/// Internally, this function copies the settings and calls tic_settings_fix()
/// to make sure that the settings will be valid when they are written to the
/// device.  If you want to get warnings about what was changed, you should call
/// tic_settings_fix() yourself beforehand.
///
/// After calling this function, to make the settings actually take effect, you
/// should call tic_reinitialize().
TIC_API TIC_WARN_UNUSED
tic_error * tic_set_settings(tic_handle *, const tic_settings *);

/// Resets the Tic's settings to their factory default values.
///
/// Since many bytes of EEPROM might need to change, the Tic might be unable to
/// respond to USB requests for a while.  We recommend sleeping for 1500
/// milliseconds after calling this function.
TIC_API TIC_WARN_UNUSED
tic_error * tic_restore_defaults(tic_handle * handle);

/// Causes the Tic to reload all of its settings from EEPROM and make them take
/// effect.
TIC_API TIC_WARN_UNUSED
tic_error * tic_reinitialize(tic_handle * handle);

/// \cond
TIC_API TIC_WARN_UNUSED
tic_error * tic_get_debug_data(tic_handle *, uint8_t * data, size_t * size);
/// \endcond

#ifdef __cplusplus
}
#endif
