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

#include "tic_protocol.h"

#define TIC_MODEL_T825 0

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
/// call this function to free the string.  Passing a NULL pointer to this
/// function is OK.  Do not free the same non-NULL string twice.
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

/// Represents the settings for a Tic.  This object is just plain old data; it
/// does not have any pointers or handles for other resources.
typedef struct tic_settings tic_settings;

/// Creates a new settings object with the default settings.  The "model"
/// parameter specifies what type of Tic you are making settings for, and must
/// be TIC_MODEL_T825.
///
/// The settings parameter should be a non-null pointer to a tic_settings
/// pointer, which will receive a pointer to a new settings object if and only
/// if this function is successful.  The caller must free the settings later by
/// calling tic_settings_free().
TIC_API TIC_WARN_UNUSED
tic_error * tic_settings_create(tic_settings ** settings, uint32_t model);

/// Copies a settings object. If this function is successful, the caller must
/// free the settings later by calling tic_settings_free().
TIC_API TIC_WARN_UNUSED
tic_error * tic_settings_copy(
  const tic_settings * source ,
  tic_settings ** dest);

// Frees a settings object.  It is OK to pass a NULL pointer to this function.
// Do not free the same non-NULL settings object twice.
TIC_API
void tic_settings_free(tic_settings *);

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

/// Gets the settings as an XML string, also known as a settings file.  If this
/// function is successful, the string must be freed by the caller using
/// tic_string_free().
TIC_API TIC_WARN_UNUSED
tic_error * tic_settings_to_string(const tic_settings *, char ** string);

/// Parses an XML settings string, also known as a settings file, and returns
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
/// tic_settings_baud_rate_get() to see what baud rate was used.
TIC_API
void tic_settings_baud_rate_set(tic_settings *, uint32_t);

/// Returns an estimate of the baud rate the controller will use for its
/// asynchronous serial port, in bits per second.
TIC_API
uint32_t tic_settings_baud_rate_get(const tic_settings *);

/// Sets the serial device number, a number between 0 and 0x7F that is used to
/// identify the device when using the Pololu protocol.
TIC_API
void tic_settings_serial_device_number_set(tic_settings *, uint8_t);

/// Gets the serial device number setting described in
/// tic_settings_serial_device_number_set().
TIC_API
uint8_t tic_settings_serial_device_number_get(const tic_settings *);

/// Sets the I2C device address to use for I2C slave communication.
TIC_API
void tic_settings_i2c_device_address_set(tic_settings *, uint8_t);

/// Gets the I2C device address to use for I2C slave communication.
TIC_API
uint8_t tic_settings_i2c_device_address_get(const tic_settings *);

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
uint16_t tic_settings_input_max_get(const tic_settings *);

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
void tic_settings_output_min_set(tic_settings *, uint16_t);

/// Gets the output minimum scaling parameter.  See
/// tic_settings_output_max_set().
TIC_API
uint16_t tic_settings_output_min_get(const tic_settings *);

/// Sets the output neutral scaling parameter.  See
/// tic_settings_output_max_set().
TIC_API
void tic_settings_output_neutral_set(tic_settings *, uint16_t);

/// Gets the output neutral scaling parameter.  See
/// tic_settings_output_max_set().
TIC_API
uint16_t tic_settings_output_neutral_get(const tic_settings *);

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
void tic_settings_output_max_set(tic_settings *, uint16_t);

/// Gets the output maximum scaling parameter.  See
/// tic_settings_output_max_set().
TIC_API
uint16_t tic_settings_output_max_get(const tic_settings *);

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

/// Sets the stepper motor current limit in milliamps.
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

/// Sets the microstepping mode, which is the number of microsteps that
/// corresponds to one step.  Valid values for the Tic T825 are 1, 2, 4, 8, and
/// 32.
TIC_API
void tic_settings_microstepping_mode_set(tic_settings *, uint32_t);

/// Gets the microstepping mode described in
/// tic_settings_microstepping_mode_set().
TIC_API
uint32_t tic_settings_microstepping_mode_get(const tic_settings *);

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
///
/// The settings parameter should be a non-null pointer to a tic_settings
/// pointer, which will receive a pointer to a new settings object if and only
/// if this function is successful.  The caller must free the settings later by
/// calling tic_settings_free().
TIC_API TIC_WARN_UNUSED
tic_error * tic_get_settings(tic_handle *, tic_settings ** settings);

/// Writes all of the Tic's non-volatile settings.  Internally, this function
/// copies the settings and calls tic_settings_fix() to make sure that the
/// settings will be valid when they are written to the device.  If you want to
/// get warnings about what was changed, you should call tic_settings_fix()
/// yourself beforehand.
TIC_API TIC_WARN_UNUSED
tic_error * tic_set_settings(tic_handle *, const tic_settings *);

/// \cond
TIC_API TIC_WARN_UNUSED
tic_error * tic_get_debug_data(tic_handle *, uint8_t * data, size_t * size);
/// \endcond

#ifdef __cplusplus
}
#endif
