#include "main_controller.h"
#include "main_window.h"

#include <cassert>

/** This is how often we fetch the variables from the device. */
static uint32_t const UPDATE_INTERVAL_MS = 100;

void main_controller::set_window(main_window * window)
{
  this->window = window;
}

void main_controller::start()
{
  assert(!connected());
  
  // Start the update timer so that update() will be called regularly.
  // todo: use longer update period when disconnected
  window->start_update_timer(UPDATE_INTERVAL_MS);

  // The program has just started, so try to connect to a device.

  bool successfully_updated_list = update_device_list();
  if (successfully_updated_list)
  {
    window->set_device_list_contents(device_list);
    
    // Automatically connect if there is only one device.
    if (device_list.size() == 1)
    {
      connect_device(device_list.at(0));
      return;
    }
  }

  handle_model_changed();
}

/** Returns the device that matches the specified OS ID from the list, or a null
 * device if none match. */
static tic::device device_with_os_id(
  std::vector<tic::device> const & device_list,
  std::string const & id)
{
  for (tic::device const & candidate : device_list)
  {
    if (candidate.get_os_id() == id)
    {
      return candidate;
    }
  }
  return tic::device(); // null device
}

void main_controller::connect_device_with_os_id(std::string const & id)
{
  // TODO: deal with id being invalid
  connect_device(device_with_os_id(device_list, id));
}

bool main_controller::disconnect_device()
{
  if (!connected()) { return true; }
  
  if (settings_modified)
  {
    std::string question =
      "The settings you changed have not been applied to the device.  "
      "If you disconnect from the device now, those changes will be lost.  "
      "Are you sure you want to disconnect?";
    if (!window->confirm(question))
    {
      return false;
    }
  }
  
  device_handle.close();
  settings_modified = false;
  disconnected_by_user = true;
  connection_error = false;
  handle_model_changed();
  return true;
}

void main_controller::connect_device(tic::device const & device)
{
  assert(device);

  try
  {
    // Close the old handle in case one is already open.
    device_handle.close();

    connection_error = false;
    disconnected_by_user = false;

    // Open a handle to the specified device.
    device_handle = tic::handle(device);
  }
  catch (std::exception const & e)
  {
    set_connection_error("Failed to connect to device.");
    show_exception(e, "There was an error connecting to the device.");
    handle_model_changed();
    return;
  }

  try
  {
    settings = device_handle.get_settings();
    handle_settings_applied(true);
  }
  catch (std::exception const & e)
  {
    show_exception(e, "There was an error loading settings from the device.");
  }

  try
  {
    reload_variables();
  }
  catch (std::exception const & e)
  {
    show_exception(e, "There was an error getting the status of the device.");
  }
  
  handle_model_changed();
}

void main_controller::disconnect_device_by_error(std::string const & error_message)
{
  device_handle.close();
  settings_modified = false;
  disconnected_by_user = false;
  set_connection_error(error_message);
}

void main_controller::set_connection_error(std::string const & error_message)
{
  connection_error = true;
  connection_error_message = error_message;
}

void main_controller::reload_settings()
{
  if (!connected()) { return; }
  try
  {
    settings = device_handle.get_settings();
    handle_settings_applied();
    settings_modified = false;
  }
  catch (std::exception const & e)
  {
    settings_modified = true;
    show_exception(e, "There was an error loading the settings from the device.");
  }
  handle_settings_changed();
}

void main_controller::restore_default_settings()
{
  if (!connected()) { return; }

  std::string question = "This will reset all of your device's settings "
    "back to their default values.  "
    "You will lose your custom settings.  "
    "Are you sure you want to continue?";
  if (!window->confirm(question))
  {
    return;
  }

  bool restore_success = false;
  try
  {
      device_handle.restore_defaults();
      restore_success = true;
  }
  catch (std::exception const & e)
  {
    show_exception(e, "There was an error resetting to the default settings.");
  }

  // This takes care of reloading the settings and telling the view to update.
  reload_settings();

  if (restore_success)
  {
    window->show_info_message(
      "Your device's settings have been reset to their default values.");
  }
}

/** Returns true if the device list includes the specified device. */
static bool device_list_includes(
  std::vector<tic::device> const & device_list,
  tic::device const & device)
{
  return device_with_os_id(device_list, device.get_os_id());
}

void main_controller::update()
{
  // This is called regularly by the view when it is time to check for updates
  // to the state of the USB devices.  This runs on the same thread as
  // everything else, so we should be careful not to do anything too slow
  // here.  If the user tries to use the UI at all while this function is
  // running, the UI cannot respond until this function returns.

  bool successfully_updated_list = update_device_list();
  if (successfully_updated_list && device_list_changed)
  {
    window->set_device_list_contents(device_list);
  }
  
  if (connected())
  {
    // First, see if the device we are connected to is still available.
    // Note: It would be nice if the libusbp::generic_handle class had a
    // function that tests if the actual handle we are using is still valid.
    // This would be better for tricky cases like if someone unplugs and
    // plugs the same device in very fast.
    bool device_still_present = device_list_includes(
      device_list, device_handle.get_device());

    if (device_still_present)
    {
        // Reload the variables from the device.
        try
        {
          reload_variables();
        }
        catch (std::exception const & e)
        {
          // Ignore the exception.  The model provides other ways to tell that
          // the variable update failed, and the exact message is probably
          // not that useful since it is probably just a generic problem with
          // the USB connection.
        }
        handle_variables_changed();
    }
    else
    {
        // The device is gone.
        disconnect_device_by_error("The connection to the device was lost.");
        handle_model_changed();
    }
  }
  else
  {
    // We are not connected, so consider auto-connecting to a device.

    if (connection_error)
    {
      // There is an error related to a previous connection or connection
      // attempt, so don't automatically reconnect.  That would be
      // confusing, because the user might be looking away and not notice
      // that the connection was lost and then regained, or they could be
      // trying to read the error message.
    }
    else if (disconnected_by_user)
    {
      // The user explicitly disconnected the last connection, so don't
      // automatically reconnect.
    }
    else if (successfully_updated_list && device_list.size() == 1)
    {
      // Automatically connect if there is only one device and we were not
      // recently disconnected from a device.
      connect_device(device_list.at(0));
    }
  }
}

bool main_controller::exit()
{
  if (connected() && settings_modified)
  {
    std::string question =
      "The settings you changed have not been applied to the device.  "
      "If you exit now, those changes will be lost.  "
      "Are you sure you want to exit?";
    return window->confirm(question);
  }
  else
  {
    return true;
  }
}

static bool device_lists_different(std::vector<tic::device> const & list1,
  std::vector<tic::device> const & list2)
{
  if (list1.size() != list2.size())
  {
    return true;
  }
  else
  {
    for (int i = 0; i < list1.size(); i++)
    {
      if (list1.at(i).get_os_id() != list2.at(i).get_os_id())
      {
        return true;
      }
    }
    return false;
  }
}

bool main_controller::update_device_list()
{
  try
  {
    std::vector<tic::device> new_device_list = tic::list_connected_devices();
    if (device_lists_different(device_list, new_device_list))
    {
      device_list_changed = true;
    }
    else
    {
      device_list_changed = false;
    }
    device_list = new_device_list;
    return true;
  }
  catch (std::exception const & e)
  {
    set_connection_error("Failed to get the list of devices.");
    show_exception(e, "There was an error getting the list of devices.");
    return false;
  }
}

void main_controller::show_exception(std::exception const & e,
    std::string const & context)
{
    std::string message;
    if (context.size() > 0)
    {
      message += context;
      message += "  ";
    }
    message += e.what();
    window->show_error_message(message);
}

void main_controller::handle_model_changed()
{
  handle_device_changed();
  handle_variables_changed();
  handle_settings_changed();
}

void main_controller::handle_device_changed()
{
  if (connected())
  {
    tic::device const & device = device_handle.get_device();
    window->set_device_name(device.get_name(), true);
    window->set_serial_number(device.get_serial_number());
    window->set_firmware_version(device_handle.get_firmware_version_string());
    
    window->set_device_list_selected(device);
    window->set_connection_status("", false);
  }
  else 
  {
    std::string value = "N/A";
    window->set_device_name(value, false);
    window->set_serial_number(value);
    window->set_firmware_version(value);
    
    window->set_device_list_selected(tic::device()); // show "Not connected"
    
    if (connection_error)
    {
      window->set_connection_status(connection_error_message, true);
    }
    else
    {
      window->set_connection_status("", false);
    }
  }
  
  window->set_disconnect_enabled(connected());
  window->set_reload_settings_enabled(connected());
  window->set_restore_defaults_enabled(connected());
  window->set_main_boxes_enabled(connected());
}

void main_controller::handle_variables_changed()
{
  //todo get_device_reset()
  
  window->set_vin_voltage(std::to_string(variables.get_vin_voltage()) + " mV");
  
  if (variables.get_planning_mode() == TIC_PLANNING_MODE_TARGET_POSITION)
  {
    window->set_target_position(std::to_string(variables.get_target_position()));
  }
  else if (variables.get_planning_mode() == TIC_PLANNING_MODE_TARGET_VELOCITY)
  {
    window->set_target_velocity(std::to_string(variables.get_target_velocity()));
  }
  
  window->set_current_position(std::to_string(variables.get_current_position()));
  window->set_current_velocity(std::to_string(variables.get_current_velocity()));
}

void main_controller::handle_settings_changed()
{
  // [all-settings]
  window->set_control_mode(tic_settings_control_mode_get(settings.pointer_get()));
  window->set_serial_baud_rate(tic_settings_serial_baud_rate_get(settings.pointer_get()));
  window->set_serial_device_number(tic_settings_serial_device_number_get(settings.pointer_get()));
  window->set_serial_crc_enabled(tic_settings_serial_crc_enabled_get(settings.pointer_get()));
  window->set_input_min(tic_settings_input_min_get(settings.pointer_get()));
  window->set_input_neutral_min(tic_settings_input_neutral_min_get(settings.pointer_get()));
  window->set_input_neutral_max(tic_settings_input_neutral_max_get(settings.pointer_get()));
  window->set_input_max(tic_settings_input_max_get(settings.pointer_get()));
  window->set_output_min(tic_settings_output_min_get(settings.pointer_get()));
  window->set_output_max(tic_settings_output_max_get(settings.pointer_get()));
  window->set_input_averaging_enabled(tic_settings_input_averaging_enabled_get(settings.pointer_get()));
  window->set_input_hysteresis(tic_settings_input_hysteresis_get(settings.pointer_get()));
  window->set_encoder_prescaler(tic_settings_encoder_prescaler_get(settings.pointer_get()));
  window->set_encoder_postscaler(tic_settings_encoder_postscaler_get(settings.pointer_get()));
  window->set_encoder_unlimited(tic_settings_encoder_unlimited_get(settings.pointer_get()));
  window->set_speed_max(tic_settings_speed_max_get(settings.pointer_get()));
  window->set_speed_min(tic_settings_speed_min_get(settings.pointer_get()));
  window->set_accel_max(tic_settings_accel_max_get(settings.pointer_get()));
  window->set_decel_max(tic_settings_decel_max_get(settings.pointer_get()));
  window->set_step_mode(tic_settings_step_mode_get(settings.pointer_get()));
  window->set_current_limit(tic_settings_current_limit_get(settings.pointer_get()));
  window->set_decay_mode(tic_settings_decay_mode_get(settings.pointer_get()));
  
  window->set_apply_settings_enabled(connected() && settings_modified);
}

void main_controller::handle_settings_applied(bool force_reset_manual_target)
{  
  window->set_manual_target_range(
    tic_settings_output_min_get(settings.pointer_get()),
    tic_settings_output_max_get(settings.pointer_get()));
    
  window->set_manual_target_box_enabled(control_mode_is_serial(settings));
    
  if (!control_mode_is_serial(settings) || force_reset_manual_target)
  {
    window->set_manual_target(0);
  }
  
  // this must be last so the preceding code can compare old and new settings
  cached_settings = settings;
}

void main_controller::handle_control_mode_input(uint8_t control_mode)
{
  if (!connected()) { return; }
  tic_settings_control_mode_set(settings.pointer_get(), control_mode);
  settings_modified = true;
  handle_settings_changed();
}

void main_controller::handle_serial_baud_rate_input(uint32_t serial_baud_rate)
{
  if (!connected()) { return; }
  tic_settings_serial_baud_rate_set(settings.pointer_get(), serial_baud_rate);
  settings_modified = true;
  handle_settings_changed();
}

void main_controller::handle_serial_baud_rate_input_finished()
{
  if (!connected()) { return; }
  uint32_t serial_baud_rate = tic_settings_serial_baud_rate_get(settings.pointer_get());
  serial_baud_rate = tic_settings_achievable_baud_rate(settings.pointer_get(), serial_baud_rate);
  tic_settings_serial_baud_rate_set(settings.pointer_get(), serial_baud_rate);
  settings_modified = true;
  handle_settings_changed();
}

void main_controller::handle_serial_device_number_input(uint8_t serial_device_number)
{
  if (!connected()) { return; }
  tic_settings_serial_device_number_set(settings.pointer_get(), serial_device_number);
  settings_modified = true;
  handle_settings_changed();
}

void main_controller::handle_serial_crc_enabled_input(bool serial_crc_enabled)
{
  if (!connected()) { return; }
  tic_settings_serial_crc_enabled_set(settings.pointer_get(), serial_crc_enabled);
  settings_modified = true;
  handle_settings_changed();
}

void main_controller::handle_input_min_input(uint16_t input_min)
{
  if (!connected()) { return; }
  tic_settings_input_min_set(settings.pointer_get(), input_min);
  settings_modified = true;
  handle_settings_changed();
}

void main_controller::handle_input_neutral_min_input(uint16_t input_neutral_min)
{
  if (!connected()) { return; }
  tic_settings_input_neutral_min_set(settings.pointer_get(), input_neutral_min);
  settings_modified = true;
  handle_settings_changed();
}

void main_controller::handle_input_neutral_max_input(uint16_t input_neutral_max)
{
  if (!connected()) { return; }
  tic_settings_input_neutral_max_set(settings.pointer_get(), input_neutral_max);
  settings_modified = true;
  handle_settings_changed();
}

void main_controller::handle_input_max_input(uint16_t input_max)
{
  if (!connected()) { return; }
  tic_settings_input_max_set(settings.pointer_get(), input_max);
  settings_modified = true;
  handle_settings_changed();
}

void main_controller::handle_output_min_input(int32_t output_min)
{
  if (!connected()) { return; }
  tic_settings_output_min_set(settings.pointer_get(), output_min);
  settings_modified = true;
  handle_settings_changed();
}

void main_controller::handle_output_max_input(int32_t output_max)
{
  if (!connected()) { return; }
  tic_settings_output_max_set(settings.pointer_get(), output_max);
  settings_modified = true;
  handle_settings_changed();
}

void main_controller::handle_input_averaging_enabled_input(bool input_averaging_enabled)
{
  if (!connected()) { return; }
  tic_settings_input_averaging_enabled_set(settings.pointer_get(), input_averaging_enabled);
  settings_modified = true;
  handle_settings_changed();
}

void main_controller::handle_input_hysteresis_input(uint16_t input_hysteresis)
{
  if (!connected()) { return; }
  tic_settings_input_hysteresis_set(settings.pointer_get(), input_hysteresis);
  settings_modified = true;
  handle_settings_changed();
}

void main_controller::handle_encoder_prescaler_input(uint32_t encoder_prescaler)
{
  if (!connected()) { return; }
  tic_settings_encoder_prescaler_set(settings.pointer_get(), encoder_prescaler);
  settings_modified = true;
  handle_settings_changed();
}

void main_controller::handle_encoder_postscaler_input(uint32_t encoder_postscaler)
{
  if (!connected()) { return; }
  tic_settings_encoder_postscaler_set(settings.pointer_get(), encoder_postscaler);
  settings_modified = true;
  handle_settings_changed();
}

void main_controller::handle_encoder_unlimited_input(bool encoder_unlimited)
{
  if (!connected()) { return; }
  tic_settings_encoder_unlimited_set(settings.pointer_get(), encoder_unlimited);
  settings_modified = true;
  handle_settings_changed();
}

void main_controller::handle_speed_max_input(uint32_t speed_max)
{
  if (!connected()) { return; }
  tic_settings_speed_max_set(settings.pointer_get(), speed_max);
  settings_modified = true;
  handle_settings_changed();
}

void main_controller::handle_speed_min_input(uint32_t speed_min)
{
  if (!connected()) { return; }
  tic_settings_speed_min_set(settings.pointer_get(), speed_min);
  settings_modified = true;
  handle_settings_changed();
}

void main_controller::handle_accel_max_input(uint32_t accel_max)
{
  if (!connected()) { return; }
  tic_settings_accel_max_set(settings.pointer_get(), accel_max);
  settings_modified = true;
  handle_settings_changed();
}

void main_controller::handle_decel_max_input(uint32_t decel_max)
{
  if (!connected()) { return; }
  tic_settings_decel_max_set(settings.pointer_get(), decel_max);
  settings_modified = true;
  handle_settings_changed();
}

void main_controller::handle_step_mode_input(uint8_t step_mode)
{
  if (!connected()) { return; }
  tic_settings_step_mode_set(settings.pointer_get(), step_mode);
  settings_modified = true;
  handle_settings_changed();
}

void main_controller::handle_current_limit_input(uint32_t current_limit)
{
  if (!connected()) { return; }
  tic_settings_current_limit_set(settings.pointer_get(), current_limit);
  settings_modified = true;
  handle_settings_changed();
}

void main_controller::handle_current_limit_input_finished()
{
  if (!connected()) { return; }
  uint32_t current_limit = tic_settings_current_limit_get(settings.pointer_get());
  current_limit = tic_settings_achievable_current_limit(settings.pointer_get(), current_limit);
  tic_settings_current_limit_set(settings.pointer_get(), current_limit);
  settings_modified = true;
  handle_settings_changed();
}

void main_controller::handle_decay_mode_input(uint8_t decay_mode)
{
  if (!connected()) { return; }
  tic_settings_decay_mode_set(settings.pointer_get(), decay_mode);
  settings_modified = true;
  handle_settings_changed();
}

void main_controller::set_target_position(int32_t position)
{
  if (!connected()) { return; }

  try
  {
    assert(connected());
    
    device_handle.set_target_position(position);
  }
  catch (std::exception const & e)
  {
    show_exception(e, "There was an error setting target position.");
  }
}

void main_controller::set_target_velocity(int32_t velocity)
{
  if (!connected()) { return; }

  try
  {
    assert(connected());
    
    device_handle.set_target_velocity(velocity);
  }
  catch (std::exception const & e)
  {
    show_exception(e, "There was an error setting target velocity.");
  }
}

void main_controller::set_current_position(int32_t position)
{
  if (!connected()) { return; }

  try
  {
    assert(connected());
    
    device_handle.set_current_position(position);
  }
  catch (std::exception const & e)
  {
    show_exception(e, "There was an error setting current position.");
  }
}

void main_controller::apply_settings()
{
  if (!connected()) { return; }

  try
  {
    assert(connected());
    
    tic::settings fixed_settings = settings;
    std::string warnings;
    fixed_settings.fix(&warnings); 
    if (warnings.empty() ||
      window->confirm(warnings.append("\nAccept these changes and apply settings?")))
    {
      settings = fixed_settings;
      device_handle.set_settings(settings);
      device_handle.reinitialize();
      handle_settings_applied();
      settings_modified = false;  // this must be last in case exceptions are thrown
    }
  }
  catch (std::exception const & e)
  {
    show_exception(e, "There was an error applying settings.");
  }
  
  handle_settings_changed();
}

void main_controller::reload_variables()
{
  assert(connected());

  try
  {
    variables = device_handle.get_variables();
    variables_update_failed = false;
  }
  catch (...)
  {
    variables_update_failed = true;
    throw;
  }
}

bool main_controller::control_mode_is_serial(tic::settings const & s) const
{
  uint8_t control_mode = tic_settings_control_mode_get(s.pointer_get());
  return (control_mode == TIC_CONTROL_MODE_SERIAL); 
}
