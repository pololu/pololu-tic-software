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

  // Read all bytes from EEPROM that contain settings.
  uint8_t buf[TIC_HIGHEST_SETTING_ADDRESS + 1];
  for (uint8_t i = 1; i < sizeof(buf) && error == NULL; i++)
  {
    error = read_setting_byte(handle, i, buf + i);
  }

  {
    bool control_mode = buf[TIC_SETTING_CONTROL_MODE] & 1;
    tic_settings_control_mode_set(new_settings, control_mode);
  }

  {
    bool never_sleep = buf[TIC_SETTING_NEVER_SLEEP] & 1;
    tic_settings_never_sleep_set(new_settings, never_sleep);
  }

  {
    bool disable_safe_start = buf[TIC_SETTING_DISABLE_SAFE_START] & 1;
    tic_settings_disable_safe_start_set(new_settings, disable_safe_start);
  }

  {
    bool ignore_err_line_high = buf[TIC_SETTING_IGNORE_ERR_LINE_HIGH] & 1;
    tic_settings_ignore_err_line_high_set(new_settings, ignore_err_line_high);
  }

  {
    uint8_t * p = buf + TIC_SETTING_SERIAL_BAUD_RATE_GENERATOR;
    uint16_t brg = p[0] + (p[1] << 8);
    tic_settings_brg_set(new_settings, brg);
  }

  {
    uint8_t serial_device_number = buf[TIC_SETTING_SERIAL_DEVICE_NUMBER];
    tic_settings_serial_device_number_set(new_settings, serial_device_number);
  }

  {
    uint8_t i2c_device_address = buf[TIC_SETTING_I2C_DEVICE_ADDRESS];
    tic_settings_i2c_device_address_set(new_settings, i2c_device_address);
  }

  {
    uint8_t * p = buf + TIC_SETTING_COMMAND_TIMEOUT;
    uint16_t command_timeout = p[0] + (p[1] << 8);
    tic_settings_command_timeout_set(new_settings, command_timeout);
  }

  {
    bool serial_crc_enabled = buf[TIC_SETTING_SERIAL_CRC_ENABLED] & 1;
    tic_settings_serial_crc_enabled_set(new_settings, serial_crc_enabled);
  }

  {
    uint8_t * p = buf + TIC_SETTING_LOW_VIN_TIMEOUT;
    uint16_t low_vin_timeout = p[0] + (p[1] << 8);
    tic_settings_low_vin_timeout_set(new_settings, low_vin_timeout);
  }

  {
    uint8_t * p = buf + TIC_SETTING_LOW_VIN_SHUTOFF_VOLTAGE;
    uint16_t low_vin_shutoff_voltage = p[0] + (p[1] << 8);
    tic_settings_low_vin_shutoff_voltage_set(new_settings,
      low_vin_shutoff_voltage);
  }

  {
    uint8_t * p = buf + TIC_SETTING_LOW_VIN_STARTUP_VOLTAGE;
    uint16_t low_vin_startup_voltage = p[0] + (p[1] << 8);
    tic_settings_low_vin_startup_voltage_set(new_settings,
      low_vin_startup_voltage);
  }

  {
    uint8_t * p = buf + TIC_SETTING_HIGH_VIN_SHUTOFF_VOLTAGE;
    uint16_t high_vin_shutoff_voltage = p[0] + (p[1] << 8);
    tic_settings_high_vin_shutoff_voltage_set(new_settings,
      high_vin_shutoff_voltage);
  }

  {
    uint8_t * p = buf + TIC_SETTING_VIN_MULTIPLIER_OFFSET;
    int16_t vin_multiplier_offset = p[0] + (p[1] << 8);
    tic_settings_vin_multiplier_offset_set(new_settings,
      vin_multiplier_offset);
  }

  {
    uint8_t * p = buf + TIC_SETTING_RC_MAX_PULSE_PERIOD;
    uint16_t rc_max_pulse_period = p[0] + (p[1] << 8);
    tic_settings_rc_max_pulse_period_set(new_settings,
      rc_max_pulse_period);
  }

  {
    uint8_t * p = buf + TIC_SETTING_RC_BAD_SIGNAL_TIMEOUT;
    uint16_t rc_bad_signal_timeout = p[0] + (p[1] << 8);
    tic_settings_rc_bad_signal_timeout_set(new_settings,
      rc_bad_signal_timeout);
  }

  {
    uint8_t * p = buf + TIC_SETTING_RC_CONSECUTIVE_GOOD_PULSES;
    uint16_t rc_consecutive_good_pulses = p[0] + (p[1] << 8);
    tic_settings_rc_consecutive_good_pulses_set(new_settings,
      rc_consecutive_good_pulses);
  }

  {
    uint8_t input_play = buf[TIC_SETTING_INPUT_PLAY];
    tic_settings_input_play_set(new_settings, input_play);
  }

  {
    uint8_t * p = buf + TIC_SETTING_INPUT_ERROR_MIN;
    uint16_t input_error_min = p[0] + (p[1] << 8);
    tic_settings_input_error_min_set(new_settings, input_error_min);
  }

  {
    uint8_t * p = buf + TIC_SETTING_INPUT_ERROR_MAX;
    uint16_t input_error_max = p[0] + (p[1] << 8);
    tic_settings_input_error_max_set(new_settings, input_error_max);
  }

  {
    uint8_t input_scaling_degree = buf[TIC_SETTING_INPUT_SCALING_DEGREE];
    tic_settings_input_play_set(new_settings, input_scaling_degree);
  }

  {
    bool input_invert = buf[TIC_SETTING_INPUT_INVERT] & 1;
    tic_settings_input_play_set(new_settings, input_invert);
  }

  {
    uint8_t * p = buf + TIC_SETTING_INPUT_MIN;
    uint16_t input_min = p[0] + (p[1] << 8);
    tic_settings_input_min_set(new_settings, input_min);
  }

  {
    uint8_t * p = buf + TIC_SETTING_INPUT_NEUTRAL_MIN;
    uint16_t input_neutral_min = p[0] + (p[1] << 8);
    tic_settings_input_neutral_min_set(new_settings, input_neutral_min);
  }

  {
    uint8_t * p = buf + TIC_SETTING_INPUT_NEUTRAL_MAX;
    uint16_t input_neutral_max = p[0] + (p[1] << 8);
    tic_settings_input_neutral_max_set(new_settings, input_neutral_max);
  }

  {
    uint8_t * p = buf + TIC_SETTING_INPUT_MAX;
    uint16_t input_max = p[0] + (p[1] << 8);
    tic_settings_input_max_set(new_settings, input_max);
  }

  {
    uint8_t * p = buf + TIC_SETTING_OUTPUT_MIN;
    int32_t output_min = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
    tic_settings_output_min_set(new_settings, output_min);
  }

  {
    uint8_t * p = buf + TIC_SETTING_OUTPUT_NEUTRAL;
    int32_t output_neutral = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
    tic_settings_output_neutral_set(new_settings, output_neutral);
  }

  {
    uint8_t * p = buf + TIC_SETTING_OUTPUT_MAX;
    int32_t output_max = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
    tic_settings_output_max_set(new_settings, output_max);
  }

  {
    uint8_t encoder_prescaler = buf[TIC_SETTING_ENCODER_PRESCALER];
    tic_settings_output_max_set(new_settings, encoder_prescaler);
  }

  {
    uint8_t * p = buf + TIC_SETTING_ENCODER_POSTSCALER;
    uint32_t encoder_postscaler = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
    tic_settings_output_max_set(new_settings, encoder_postscaler);
  }

  {
    uint8_t scl_config = buf[TIC_SETTING_SCL_CONFIG];
    tic_settings_scl_config_set(new_settings, scl_config);
  }

  {
    uint8_t sda_config = buf[TIC_SETTING_SDA_CONFIG];
    tic_settings_sda_config_set(new_settings, sda_config);
  }

  {
    uint8_t tx_config = buf[TIC_SETTING_TX_CONFIG];
    tic_settings_tx_config_set(new_settings, tx_config);
  }

  {
    uint8_t rx_config = buf[TIC_SETTING_RX_CONFIG];
    tic_settings_rx_config_set(new_settings, rx_config);
  }

  {
    uint8_t rc_config = buf[TIC_SETTING_RC_CONFIG];
    tic_settings_rc_config_set(new_settings, rc_config);
  }

  {
    uint32_t current_limit = buf[TIC_SETTING_CURRENT_LIMIT] *
      TIC_CURRENT_LIMIT_UNITS_MA;
    tic_settings_current_limit_set(new_settings, current_limit);
  }

  {
    uint8_t microstepping_mode = buf[TIC_SETTING_MICROSTEPPING_MODE];
    tic_settings_microstepping_mode_set(new_settings, microstepping_mode);
  }

  {
    uint8_t decay_mode = buf[TIC_SETTING_DECAY_MODE];
    tic_settings_decay_mode_set(new_settings, decay_mode);
  }

  {
    uint8_t * p = buf + TIC_SETTING_SPEED_MIN;
    uint32_t speed_min = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
    tic_settings_speed_min_set(new_settings, speed_min);
  }

  {
    uint8_t * p = buf + TIC_SETTING_SPEED_MAX;
    uint32_t speed_max = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
    tic_settings_speed_max_set(new_settings, speed_max);
  }

  {
    uint8_t * p = buf + TIC_SETTING_DECEL_MAX;
    uint32_t decel_max = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
    tic_settings_decel_max_set(new_settings, decel_max);
  }

  {
    uint8_t * p = buf + TIC_SETTING_ACCEL_MAX;
    uint32_t accel_max = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
    tic_settings_accel_max_set(new_settings, accel_max);
  }

  {
    uint8_t * p = buf + TIC_SETTING_DECEL_MAX_DURING_ERROR;
    uint32_t decel_max_during_error = p[0] + (p[1] << 8) +
      (p[2] << 16) + (p[3] << 24);
    tic_settings_decel_max_set(new_settings, decel_max_during_error);
  }

  // Pass the new settings to the caller.
  if (error == NULL)
  {
    *settings = new_settings;
    new_settings = NULL;
  }

  tic_settings_free(new_settings);

  return error;
}
