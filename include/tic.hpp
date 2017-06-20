// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

/// \file tic.hpp
///
/// This file provides the C++ API for libpololu-tic, a library supports
/// communciating with the Pololu Tic USB Stepper Motor Controller.  The classes
/// and functions here are just thin wrappers around the C API defined in tic.h.
///
/// Note: There are some C functions in libpololu-tic that are not wrapped here
/// because they can easily be called from C++.

#pragma once

#include "tic.h"
#include <cstddef>
#include <utility>
#include <memory>
#include <string>
#include <vector>

// Display a nice error if C++11 is not enabled (e.g. --std=gnu++11).
#if (!defined(__cplusplus) || (__cplusplus < 201103L)) && \
    !defined(__GXX_EXPERIMENTAL_CXX0X__) && !defined(_MSC_VER)
#error This header requires features from C++11.
#endif

namespace tic
{
  /// \cond
  inline void throw_if_needed(tic_error * err);
  /// \endcond

  /// Wrapper for tic_error_free().
  inline void pointer_free(tic_error * p) noexcept
  {
    tic_error_free(p);
  }

  /// Wrapper for tic_error_copy().
  inline tic_error * pointer_copy(const tic_error * p) noexcept
  {
    return tic_error_copy(p);
  }

  /// Wrapper for tic_variables_free().
  inline void pointer_free(tic_variables * p) noexcept
  {
    tic_variables_free(p);
  }

  /// Wrapper for tic_variables_copy().
  inline tic_variables * pointer_copy(const tic_variables * p)
  {
    tic_variables * copy;
    throw_if_needed(tic_variables_copy(p, &copy));
    return copy;
  }

  /// Wrapper for tic_settings_free().
  inline void pointer_free(tic_settings * p) noexcept
  {
    tic_settings_free(p);
  }

  /// Wrapper for tic_settings_copy().
  inline tic_settings * pointer_copy(const tic_settings * p)
  {
    tic_settings * copy;
    throw_if_needed(tic_settings_copy(p, &copy));
    return copy;
  }

  /// Wrapper for tic_device_free().
  inline void pointer_free(tic_device * p) noexcept
  {
    tic_device_free(p);
  }

  /// Wrapper for tic_device_copy().
  inline tic_device * pointer_copy(const tic_device * p)
  {
    tic_device * copy;
    throw_if_needed(tic_device_copy(p, &copy));
    return copy;
  }

  /// Wrapper for tic_handle_close().
  inline void pointer_free(tic_handle * p) noexcept
  {
    tic_handle_close(p);
  }

  /// This class is not part of the public API of the library and you should
  /// not use it directly, but you can use the public methods it provides to
  /// the classes that inherit from it.
  template<class T>
  class unique_pointer_wrapper
  {
  public:
    /// Constructor that takes a pointer.
    explicit unique_pointer_wrapper(T * p = NULL) noexcept : pointer(p)
    {
    }

    /// Move constructor.
    unique_pointer_wrapper(unique_pointer_wrapper && other) noexcept
    {
      pointer = other.pointer_release();
    }

    /// Move assignment operator.
    unique_pointer_wrapper & operator=(unique_pointer_wrapper && other) noexcept
    {
      pointer_reset(other.pointer_release());
      return *this;
    }

    /// Destructor
    ~unique_pointer_wrapper() noexcept
    {
      pointer_reset();
    }

    /// Implicit conversion to bool.  Returns true if the underlying pointer is
    /// not NULL.
    operator bool() const noexcept
    {
      return pointer != NULL;
    }

    /// Returns the underlying pointer.
    T * pointer_get() const noexcept
    {
      return pointer;
    }

    /// Sets the underlying pointer to the specified value, freeing the
    /// previous pointer and taking ownership of the specified one.
    void pointer_reset(T * p = NULL) noexcept
    {
      pointer_free(pointer);
      pointer = p;
    }

    /// Releases the pointer, transferring ownership of it to the caller and
    /// resetting the underlying pointer of this object to NULL.  The caller
    /// is responsible for freeing the returned pointer if it is not NULL.
    T * pointer_release() noexcept
    {
      T * p = pointer;
      pointer = NULL;
      return p;
    }

    /// Returns a pointer to the underlying pointer.
    T ** pointer_to_pointer_get() noexcept
    {
      return &pointer;
    }

    /// Copy constructor: forbid.
    unique_pointer_wrapper(const unique_pointer_wrapper & other) = delete;

    /// Copy assignment operator: forbid.
    unique_pointer_wrapper & operator=(const unique_pointer_wrapper & other) = delete;

  protected:
    T * pointer;
  };

  /// This class is not part of the public API of the library and you should not
  /// use it directly, but you can use the public methods it provides to the
  /// classes that inherit from it.
  template <class T>
  class unique_pointer_wrapper_with_copy : public unique_pointer_wrapper<T>
  {
  public:
    /// Constructor that takes a pointer.
    explicit unique_pointer_wrapper_with_copy(T * p = NULL) noexcept
      : unique_pointer_wrapper<T>(p)
    {
    }

    /// Move constructor.
    unique_pointer_wrapper_with_copy(
      unique_pointer_wrapper_with_copy && other) = default;

    /// Copy constructor.
    unique_pointer_wrapper_with_copy(
      const unique_pointer_wrapper_with_copy & other)
      : unique_pointer_wrapper<T>()
    {
      this->pointer = pointer_copy(other.pointer);
    }

    /// Copy assignment operator.
    unique_pointer_wrapper_with_copy & operator=(
      const unique_pointer_wrapper_with_copy & other)
    {
      this->pointer_reset(pointer_copy(other.pointer));
      return *this;
    }

    /// Move assignment operator.
    unique_pointer_wrapper_with_copy & operator=(
      unique_pointer_wrapper_with_copy && other) = default;
  };

  /// Represents an error from a library call.
  class error : public unique_pointer_wrapper_with_copy<tic_error>, public std::exception
  {
  public:
    /// Constructor that takes a pointer from the C API.
    explicit error(tic_error * p = NULL) noexcept :
      unique_pointer_wrapper_with_copy(p)
    {
    }

    /// Similar to message(), but returns a C string.  The returned string will
    /// be valid at least until this object is destroyed or modified.
    virtual const char * what() const noexcept
    {
      return tic_error_get_message(pointer);
    }

    /// Returns an English-language ASCII-encoded string describing the error.
    /// The string consists of one or more complete sentences.
    std::string message() const
    {
      return tic_error_get_message(pointer);
    }

    /// Returns true if the error has specified error code.  The error codes are
    /// listed in the ::tic_error_code enum.
    bool has_code(uint32_t error_code) const noexcept
    {
      return tic_error_has_code(pointer, error_code);
    }
  };

  /*! \cond */
  inline void throw_if_needed(tic_error * err)
  {
    if (err != NULL) { throw error(err); }
  }
  /*! \endcond */

  /// Represents the variables read from a Tic.  This object just stores plain
  /// old data; it does not have any pointer or handles for other resources.
  class variables : public unique_pointer_wrapper_with_copy<tic_variables>
  {
  public:
    /// Constructor that takes a pointer from the C API.
    explicit variables(tic_variables * p = NULL) noexcept :
      unique_pointer_wrapper_with_copy(p)
    {
    }

    /// Wrapper for tic_variables_get_operation_state().
    uint8_t get_operation_state() const noexcept
    {
      return tic_variables_get_operation_state(pointer);
    }

    /// Wrapper for tic_variables_get_error_status().
    uint16_t get_error_status() const noexcept
    {
      return tic_variables_get_error_status(pointer);
    }

    /// Wrapper for tic_variables_get_errors_occurred().
    uint32_t get_errors_occurred() const noexcept
    {
      return tic_variables_get_errors_occurred(pointer);
    }

    /// Wrapper for tic_variables_get_planning_mode().
    uint8_t get_planning_mode() const noexcept
    {
      return tic_variables_get_planning_mode(pointer);
    }

    /// Wrapper for tic_variables_get_target_position().
    int32_t get_target_position() const noexcept
    {
      return tic_variables_get_target_position(pointer);
    }

    /// Wrapper for tic_variables_get_target_velocity().
    int32_t get_target_velocity() const noexcept
    {
      return tic_variables_get_target_velocity(pointer);
    }

    /// Wrapper for tic_variables_get_speed_min().
    uint32_t get_speed_min() const noexcept
    {
      return tic_variables_get_speed_min(pointer);
    }

    /// Wrapper for tic_variables_get_speed_max().
    uint32_t get_speed_max() const noexcept
    {
      return tic_variables_get_speed_max(pointer);
    }

    /// Wrapper for tic_variables_get_decel_max().
    uint32_t get_decel_max() const noexcept
    {
      return tic_variables_get_decel_max(pointer);
    }

    /// Wrapper for tic_variables_get_accel_max().
    uint32_t get_accel_max() const noexcept
    {
      return tic_variables_get_accel_max(pointer);
    }

    /// Wrapper for tic_variables_get_current_position().
    int32_t get_current_position() const noexcept
    {
      return tic_variables_get_current_position(pointer);
    }

    /// Wrapper for tic_variables_get_current_velocity().
    int32_t get_current_velocity() const noexcept
    {
      return tic_variables_get_current_velocity(pointer);
    }

    /// Wrapper for tic_variables_get_acting_target_position().
    int32_t get_acting_target_position() const noexcept
    {
      return tic_variables_get_acting_target_position(pointer);
    }

    /// Wrapper for tic_variables_get_time_since_last_step().
    uint32_t get_time_since_last_step() const noexcept
    {
      return tic_variables_get_time_since_last_step(pointer);
    }

    /// Wrapper for tic_variables_get_device_reset().
    uint8_t get_device_reset() const noexcept
    {
      return tic_variables_get_device_reset(pointer);
    }

    /// Wrapper for tic_variables_get_vin_voltage().
    uint32_t get_vin_voltage() const noexcept
    {
      return tic_variables_get_vin_voltage(pointer);
    }

    /// Wrapper for tic_variables_get_up_time().
    uint32_t get_up_time() const noexcept
    {
      return tic_variables_get_up_time(pointer);
    }

    /// Wrapper for tic_variables_get_encoder_position().
    int32_t get_encoder_position() const noexcept
    {
      return tic_variables_get_encoder_position(pointer);
    }

    /// Wrapper for tic_variables_get_rc_pulse_width().
    uint16_t get_rc_pulse_width() const noexcept
    {
      return tic_variables_get_rc_pulse_width(pointer);
    }

    /// Wrapper for tic_variables_get_step_mode().
    uint8_t get_step_mode() const noexcept
    {
      return tic_variables_get_step_mode(pointer);
    }

    /// Wrapper for tic_variables_get_current_limit().
    uint32_t get_current_limit() const noexcept
    {
      return tic_variables_get_current_limit(pointer);
    }

    /// Wrapper for tic_variables_get_decay_mode().
    uint8_t get_decay_mode() const noexcept
    {
      return tic_variables_get_decay_mode(pointer);
    }

    /// Wrapper for tic_variables_get_input_after_averaging().
    uint16_t get_input_after_averaging() const noexcept
    {
      return tic_variables_get_input_after_averaging(pointer);
    }

    /// Wrapper for tic_variables_get_input_after_hysteresis().
    uint16_t get_input_after_hysteresis() const noexcept
    {
      return tic_variables_get_input_after_hysteresis(pointer);
    }

    /// Wrapper for tic_variables_get_input_after_scaling().
    int32_t get_input_after_scaling() const noexcept
    {
      return tic_variables_get_input_after_scaling(pointer);
    }

    /// Wrapper for tic_variables_get_analog_reading().
    uint16_t get_analog_reading(uint8_t pin) const noexcept
    {
      return tic_variables_get_analog_reading(pointer, pin);
    }

    /// Wrapper for tic_variables_get_digital_reading().
    bool get_digital_reading(uint8_t pin) const noexcept
    {
      return tic_variables_get_digital_reading(pointer, pin);
    }

    /// Wrapper for tic_variables_get_switch_status().
    bool get_switch_status(uint8_t pin) const noexcept
    {
      return tic_variables_get_switch_status(pointer, pin);
    }

    /// Wrapper for tic_variables_get_pin_state().
    uint16_t get_pin_state(uint8_t pin) const noexcept
    {
      return tic_variables_get_pin_state(pointer, pin);
    }
  };

  /// Represets the settings for a Tic.  This object just stores plain old data;
  /// it does not have any pointers or handles for other resources.
  class settings : public unique_pointer_wrapper_with_copy<tic_settings>
  {
  public:
    /// Constructor that takes a pointer from the C API.
    explicit settings(tic_settings * p = NULL) noexcept :
      unique_pointer_wrapper_with_copy(p)
    {
    }

    /// Wrapper for tic_settings_fix().
    ///
    /// If a non-NULL warnings pointer is provided, and this function does not
    /// throw an exception, the string it points to will be overridden with an
    /// empty string or a string with English warnings.
    void fix(std::string * warnings = NULL)
    {
      char * cstr = NULL;
      char ** cstr_pointer = warnings ? &cstr : NULL;
      throw_if_needed(tic_settings_fix(pointer, cstr_pointer));
      if (warnings) { *warnings = std::string(cstr); }
    }

    /// Wrapper for tic_settings_to_string().
    std::string to_string() const
    {
      char * str;
      throw_if_needed(tic_settings_to_string(pointer, &str));
      return std::string(str);
    }

    /// Wrapper for tic_settings_read_from_string.
    ///
    /// If a non-NULL warnings pointer is provided, and this function does not
    /// throw an exception, the string it points to will be overridden with an
    /// empty string or a string with English warnings.
    static settings read_from_string(
      const std::string & settings_string, std::string * warnings = NULL)
    {
      char * cstr = NULL;
      char ** cstr_pointer = warnings ? &cstr : NULL;

      settings r;
      throw_if_needed(tic_settings_read_from_string(
          settings_string.c_str(), r.pointer_to_pointer_get(), cstr_pointer));

      if (warnings) { *warnings = std::string(cstr); }

      return r;
    }

    // TODO: wrappers for all the other accessors
  };

  /// Represents a Tic that is or was connected to the computer.  Can also be in
  /// a null state where it does not represent a device.
  class device : public unique_pointer_wrapper_with_copy<tic_device>
  {
  public:
    /// Constructor that takes a pointer from the C API.
    explicit device(tic_device * p = NULL) noexcept :
      unique_pointer_wrapper_with_copy(p)
    {
    }

    /// Gets the full name of the device (e.g. "Tic USB Stepper Motor Controller
    /// T825") as an ASCII-encoded string.
    /// If the device is null, returns an empty string.
    std::string get_name() const noexcept
    {
      return tic_device_get_name(pointer);
    }

    /// Gets the short name of the device (e.g. "T825") as an ASCII-encoded
    /// string.
    /// If the device is null, returns an empty string.
    std::string get_short_name() const noexcept
    {
      return tic_device_get_short_name(pointer);
    }

    /// Gets the serial number of the device as an ASCII-encoded string.
    /// If the device is null, returns an empty string.
    std::string get_serial_number() const noexcept
    {
      return tic_device_get_serial_number(pointer);
    }

    /// Get an operating system-specific identifier for this device as an
    /// ASCII-encoded string.  The string will be valid at least as long as the
    /// device object.  If the device is NULL, returns an empty string.
    std::string get_os_id() const noexcept
    {
      return tic_device_get_os_id(pointer);
    }

    /// Gets the firmware version as a binary-coded decimal number (e.g. 0x0100
    /// means "1.00").  We recommend using
    /// tic::handle::get_firmware_version_string() instead of this function if
    /// possible.
    uint16_t get_firmware_version() const noexcept
    {
      return tic_device_get_firmware_version(pointer);
    }
  };

  /// Finds all the Tic devices connected to the computer via USB and returns a
  /// list of them.
  inline std::vector<tic::device> list_connected_devices()
  {
    tic_device ** device_list;
    size_t size;
    throw_if_needed(tic_list_connected_devices(&device_list, &size));
    std::vector<device> vector;
    for (size_t i = 0; i < size; i++)
    {
      vector.push_back(device(device_list[i]));
    }
    tic_list_free(device_list);
    return vector;
  }

  /// Represents an open handle that can be used to read and write data from a
  /// device.  Can also be in a null state where it does not represent a device.
  class handle : public unique_pointer_wrapper<tic_handle>
  {
  public:
    /// Constructor that takes a pointer from the C API.  This object will free
    /// the pointer when it is destroyed.
    explicit handle(tic_handle * p = NULL) noexcept
      : unique_pointer_wrapper(p)
    {
    }

    /// Constructor that opens a handle to the specified device.
    explicit handle(const device & device)
    {
      throw_if_needed(tic_handle_open(device.pointer_get(), &pointer));
    }

    /// Closes the handle and puts this object into the null state.
    void close() noexcept
    {
      pointer_reset();
    }

    /// Wrapper for tic_handle_get_device();
    device get_device() const
    {
      return device(pointer_copy(tic_handle_get_device(pointer)));
    }

    /// Wrapper for tic_get_firmware_version_string().
    std::string get_firmware_version_string()
    {
      return tic_get_firmware_version_string(pointer);
    }

    /// Wrapper for tic_set_target_position().
    void set_target_position(int32_t position)
    {
      throw_if_needed(tic_set_target_position(pointer, position));
    }

    /// Wrapper for tic_set_target_velocity().
    void set_target_velocity(int32_t velocity)
    {
      throw_if_needed(tic_set_target_velocity(pointer, velocity));
    }

    /// Wrapper for tic_set_current_position().
    void set_current_position(int32_t position)
    {
      throw_if_needed(tic_set_current_position(pointer, position));
    }

    /// Wrapper for tic_stop().
    void stop()
    {
      throw_if_needed(tic_stop(pointer));
    }

    /// Wrapper for tic_reset_command_timeout().
    void reset_command_timeout()
    {
      throw_if_needed(tic_reset_command_timeout(pointer));
    }

    /// Wrapper for tic_disable_driver().
    void disable_driver()
    {
      throw_if_needed(tic_disable_driver(pointer));
    }

    /// Wrapper for tic_enable_driver().
    void enable_driver()
    {
      throw_if_needed(tic_enable_driver(pointer));
    }

    void clear_driver_error()
    {
      throw_if_needed(tic_clear_driver_error(pointer));
    }

    /// Wrapper for tic_set_speed_max().
    void set_speed_max(uint32_t speed_max)
    {
      throw_if_needed(tic_set_speed_max(pointer, speed_max));
    }

    /// Wrapper for tic_set_speed_min().
    void set_speed_min(uint32_t speed_min)
    {
      throw_if_needed(tic_set_speed_min(pointer, speed_min));
    }

    /// Wrapper for tic_set_speed_min().
    void set_accel_max(uint32_t accel_max)
    {
      throw_if_needed(tic_set_accel_max(pointer, accel_max));
    }

    /// Wrapper for tic_set_speed_min().
    void set_decel_max(uint32_t decel_max)
    {
      throw_if_needed(tic_set_decel_max(pointer, decel_max));
    }

    /// Wrapper for tic_set_step_mode().
    void set_step_mode(uint8_t step_mode)
    {
      throw_if_needed(tic_set_step_mode(pointer, step_mode));
    }

    /// Wrapper for tic_set_current_limit().
    void set_current_limit(uint32_t current_limit)
    {
      throw_if_needed(tic_set_current_limit(pointer, current_limit));
    }

    /// Wrapper for tic_set_decay_mode().
    void set_decay_mode(uint8_t decay_mode)
    {
      throw_if_needed(tic_set_decay_mode(pointer, decay_mode));
    }

    /// Wrapper for tic_get_variables().
    variables get_variables(bool clear_errors_occurred = false)
    {
      tic_variables * v;
      throw_if_needed(tic_get_variables(pointer, &v, clear_errors_occurred));
      return variables(v);
    }

    /// Wrapper for tic_get_settings().
    settings get_settings()
    {
      tic_settings * s;
      throw_if_needed(tic_get_settings(pointer, &s));
      return settings(s);
    }

    /// Wrapper for tic_set_settings().
    void set_settings(const settings & settings)
    {
      throw_if_needed(tic_set_settings(pointer, settings.pointer_get()));
    }

    /// Wrapper for tic_restore_defaults().
    void restore_defaults()
    {
      throw_if_needed(tic_restore_defaults(pointer));
    }

    /// Wrapper for tic_reinitialize().
    void reinitialize()
    {
      throw_if_needed(tic_reinitialize(pointer));
    }

    /// \cond
    void get_debug_data(std::vector<uint8_t> & data)
    {
      size_t size = data.size();
      throw_if_needed(tic_get_debug_data(pointer, data.data(), &size));
      data.resize(size);
    }
    /// \endcond

  };
#ifdef __cplusplus
}
#endif
