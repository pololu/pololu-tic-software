#include "main_controller.h"
#include "main_window.h"

#include <cassert>

void main_controller::set_window(main_window * window)
{
  this->window = window;
}

void main_controller::start()
{
  assert(!connected());

  bool successfully_updated_list = try_update_device_list();
  if (successfully_updated_list && device_list.size() > 0)
  {
    really_connect();
  }
  else
  {
    handle_model_changed();
  }
}

void main_controller::connect_device()
{
  if (connected())
  {
    // todo: figure out what to do with multiple devices
    return;
  }
  
  bool successfully_updated_list = try_update_device_list();
  if (!successfully_updated_list)
  {
    return;
  }

  if (device_list.size() > 0)
  {
    really_connect();
  }
  else
  {
    window->show_error_message(
      "No Tic was found.  "
      "Please verify that the Tic is connected to the computer via USB."
      );
  }
}

void main_controller::really_connect()
{
  assert(device_list.size() > 0);

  try
  {
    // Close the old handle in case one is already open.
    device_handle.close();

    connection_error = false;
    //disconnectedByUser = false;

    // Open a handle to the specified programmer.
    device_handle = tic::handle(device_list.at(0));

  }
  catch (const std::exception & e)
  {
    //setConnectionError("Failed to connect to device.");
    show_exception(e, "There was an error connecting to the device.");
    handle_model_changed();
    return;
  }

  /*try
  {
      model->reloadFirmwareVersionString();
  }
  catch (const std::exception & e)
  {
      showException(e, "There was an error getting the firmware version.");
  }*/

  try
  {
    settings = device_handle.get_settings();
  }
  catch (const std::exception & e)
  {
    show_exception(e, "There was an error loading settings from the device.");
  }

  /*try
  {
      model->reloadVariables();
  }
  catch (const std::exception & e)
  {
      showException(e, "There was an error getting the status of the device.");
  }*/

  handle_model_changed();
}

bool main_controller::try_update_device_list()
{
  try
  {
    device_list = tic::list_connected_devices();
    return true;
  }
  catch (const std::exception & e)
  {
    // model->setConnectionError("Failed to get the list of devices.");
    show_exception(e, "There was an error getting the list of devices.");
    return false;
  }
}

void main_controller::show_exception(const std::exception & e,
    const std::string & context)
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
  //handleVariablesChanged();
  handle_settings_changed();
}

void main_controller::handle_device_changed()
{
  if (connected())
  {
    window->set_connection_status("Connected.", false);
  }
  else if (connection_error)
  {
    window->set_connection_status(connection_error_message, true);
  }
  else if (disconnected_by_user)
  {
    window->set_connection_status("Not connected.", false);
  }
  else
  {
    // This is a subtle way of saying that we are not connected but we will
    // auto-connect when we see a device available.
    window->set_connection_status("Not connected yet...", false);
  }
}

void main_controller::handle_settings_changed()
{
  // [all-settings]
  window->set_control_mode(tic_settings_control_mode_get(settings.pointer_get()));
  window->set_input_min(tic_settings_input_min_get(settings.pointer_get()));
  window->set_input_neutral_min(tic_settings_input_neutral_min_get(settings.pointer_get()));
  window->set_input_neutral_max(tic_settings_input_neutral_max_get(settings.pointer_get()));
  window->set_input_max(tic_settings_input_max_get(settings.pointer_get()));
  window->set_output_min(tic_settings_output_min_get(settings.pointer_get()));
  window->set_output_max(tic_settings_output_max_get(settings.pointer_get()));
  window->set_speed_max(tic_settings_speed_max_get(settings.pointer_get()));
  window->set_speed_min(tic_settings_speed_min_get(settings.pointer_get()));
  window->set_accel_max(tic_settings_accel_max_get(settings.pointer_get()));
  window->set_decel_max(tic_settings_decel_max_get(settings.pointer_get()));
  window->set_step_mode(tic_settings_step_mode_get(settings.pointer_get()));
  window->set_current_limit(tic_settings_current_limit_get(settings.pointer_get()));
  window->set_decay_mode(tic_settings_decay_mode_get(settings.pointer_get()));
}

void main_controller::handle_control_mode_input(uint8_t control_mode)
{
  if (!connected()) { return; }
  tic_settings_control_mode_set(settings.pointer_get(), control_mode);
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

void main_controller::handle_decay_mode_input(uint8_t decay_mode)
{
  if (!connected()) { return; }
  tic_settings_decay_mode_set(settings.pointer_get(), decay_mode);
  settings_modified = true;
  handle_settings_changed();
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
      handle_model_changed();
    
      device_handle.set_settings(settings);
      device_handle.reinitialize();
      settings_modified = false;  // this must be last in case exceptions are thrown
    }
  }
  catch (const std::exception & e)
  {
    show_exception(e, "There was an error applying settings.");
  }

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
  catch (const std::exception & e)
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
  catch (const std::exception & e)
  {
    show_exception(e, "There was an error setting target velocity.");
  }
}