// Functions for reading and writing settings from the Tic over USB.

#include "tic_internal.h"

static tic_error * read_setting_byte(tic_handle * handle,
  uint8_t address, uint8_t * output)
{
  assert(handle != NULL);
  assert(output != NULL);
  *output = 0;

  libusbp_generic_handle * usb_handle = tic_handle_get_usb_handle(handle);

  size_t transferred;
  tic_error * error = tic_usb_error(libusbp_control_transfer(usb_handle,
      0xC0, TIC_CMD_READ_SETTING, 0, address, output, 1, &transferred));
  if (error != NULL)
  {
    return error;
  }

  if (transferred != 1)
  {
    return tic_error_create(
      "Failed to read a setting.  Expected 1 byte, got %d.\n", transferred);
  }

  return NULL;
}

tic_error * tic_get_settings(tic_handle * handle, tic_settings ** settings)
{
  if (settings == NULL)
  {
    return tic_error_create("Settings output pointer is NULL.");
  }

  *settings = NULL;

  if (handle == NULL)
  {
    return tic_error_create("Handle is NULL.");
  }

  tic_error * error = NULL;

  uint32_t model = TIC_MODEL_T825;  // TODO: get from handle

  // Allocate the new settings object.
  tic_settings * new_settings = NULL;
  if (error == NULL)
  {
    error = tic_settings_create(&new_settings, model);
  }

  if (error == NULL)
  {
    uint8_t control_mode;
    error = read_setting_byte(handle,
      TIC_SETTING_CONTROL_MODE, &control_mode);
    tic_settings_control_mode_set(new_settings, control_mode);
  }

  // TODO: read in the rest of the settings

  // Pass the new settings to the caller.
  if (error == NULL)
  {
    *settings = new_settings;
    new_settings = NULL;
  }

  tic_settings_free(new_settings);

  return error;
}
