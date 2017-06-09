#pragma once

#include "tic.hpp"

class main_window;

class main_controller
{
public:
  /** Stores a pointer to the main_window object so that we can update the window. **/
  void set_window(main_window * window);

  /** This is called when the program starts up. */
  void start();
  
  /** This is called when the user issues a connect command. */
  void connect_device();

  /** This is called when the user issues a disconnect command. */
  //TODO void disconnect_device();
  
  /** This is called whenever something in the model has changed that might
   * require the window to be updated.  It includes no details about what
   * exactly changed. */
  void handle_model_changed();

private:
  void really_connect();

  /** Returns true for success, false for failure. */
  bool try_update_device_list();
  
  void show_exception(const std::exception & e, const std::string & context = "");

public:
  void set_target_position(int32_t position);
  void set_target_velocity(int32_t velocity);

  /** This is called when the user wants to apply the settings. */
  void apply_settings();

  // These are called when the user changes a setting.
  void handle_control_mode_input(uint8_t control_mode);
  void handle_input_min_input(uint16_t input_min);
  void handle_input_neutral_min_input(uint16_t input_neutral_min);
  void handle_input_neutral_max_input(uint16_t input_neutral_max);
  void handle_input_max_input(uint16_t input_max);
  void handle_output_min_input(int32_t output_min);
  void handle_output_max_input(int32_t output_max);
  void handle_speed_max_input(uint32_t speed_max);
  void handle_speed_min_input(uint32_t speed_min);
  void handle_accel_max_input(uint32_t accel_max);
  void handle_decel_max_input(uint32_t decel_max);
  void handle_step_mode_input(uint8_t step_mode);
  void handle_current_limit_input(uint32_t current_limit);
  void handle_decay_mode_input(uint8_t decay_mode);

private:
  /** This is called whenever it is possible that we have connected to a
   * different device. */
  void handle_device_changed();

  //void handle_variables_changed();

  void handle_settings_changed();  
  
  /** Holds a list of the relevant devices that are connected to the computer. */
  std::vector<tic::device> device_list;

  /** Holds an open handle to a device or a null handle if we are not
   * connected. */
  tic::handle device_handle;
  
  /** True if the last connection or connection attempt resulted in an error.
   * If true, connectionErrorMessage provides some information about the
   * error. */
  bool connection_error = false;
  std::string connection_error_message;
  
  /** True if we are disconnected now and the last connection was terminated
   * by the user. */
  bool disconnected_by_user = false;

  /** Holds the settings from the device. */
  tic::settings settings;
  
  /** True if the settings have been modified by user and could be different
   * from what is on the device. */
  bool settings_modified = false;
  
  // todo: variables

  /** Returns true if we are currently connected to a device. */
  bool connected() const { return device_handle; }
  
  main_window * window;
};