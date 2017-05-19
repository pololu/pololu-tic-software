// Function for reading settings from the Tic over USB.

#include "tic_internal.h"

static void write_buffer_to_settings(const uint8_t * buf, tic_settings * settings)
{
  {
    uint8_t control_mode = buf[TIC_SETTING_CONTROL_MODE];
    tic_settings_control_mode_set(settings, control_mode);
  }

  {
    bool never_sleep = buf[TIC_SETTING_NEVER_SLEEP] & 1;
    tic_settings_never_sleep_set(settings, never_sleep);
  }

  {
    bool disable_safe_start = buf[TIC_SETTING_DISABLE_SAFE_START] & 1;
    tic_settings_disable_safe_start_set(settings, disable_safe_start);
  }

  {
    bool ignore_err_line_high = buf[TIC_SETTING_IGNORE_ERR_LINE_HIGH] & 1;
    tic_settings_ignore_err_line_high_set(settings, ignore_err_line_high);
  }

  {
    uint16_t brg = read_u16(buf + TIC_SETTING_SERIAL_BAUD_RATE_GENERATOR);
    uint32_t baud_rate = tic_baud_rate_from_brg(brg);
    tic_settings_serial_baud_rate_set(settings, baud_rate);
  }

  {
    uint8_t serial_device_number = buf[TIC_SETTING_SERIAL_DEVICE_NUMBER];
    tic_settings_serial_device_number_set(settings, serial_device_number);
  }

  {
    uint8_t i2c_device_address = buf[TIC_SETTING_I2C_DEVICE_ADDRESS];
    tic_settings_i2c_device_address_set(settings, i2c_device_address);
  }

  {
    const uint8_t * p = buf + TIC_SETTING_COMMAND_TIMEOUT;
    uint16_t command_timeout = p[0] + (p[1] << 8);
    tic_settings_command_timeout_set(settings, command_timeout);
  }

  {
    bool serial_crc_enabled = buf[TIC_SETTING_SERIAL_CRC_ENABLED] & 1;
    tic_settings_serial_crc_enabled_set(settings, serial_crc_enabled);
  }

  {
    const uint8_t * p = buf + TIC_SETTING_LOW_VIN_TIMEOUT;
    uint16_t low_vin_timeout = p[0] + (p[1] << 8);
    tic_settings_low_vin_timeout_set(settings, low_vin_timeout);
  }

  {
    const uint8_t * p = buf + TIC_SETTING_LOW_VIN_SHUTOFF_VOLTAGE;
    uint16_t low_vin_shutoff_voltage = p[0] + (p[1] << 8);
    tic_settings_low_vin_shutoff_voltage_set(settings,
      low_vin_shutoff_voltage);
  }

  {
    const uint8_t * p = buf + TIC_SETTING_LOW_VIN_STARTUP_VOLTAGE;
    uint16_t low_vin_startup_voltage = p[0] + (p[1] << 8);
    tic_settings_low_vin_startup_voltage_set(settings,
      low_vin_startup_voltage);
  }

  {
    const uint8_t * p = buf + TIC_SETTING_HIGH_VIN_SHUTOFF_VOLTAGE;
    uint16_t high_vin_shutoff_voltage = p[0] + (p[1] << 8);
    tic_settings_high_vin_shutoff_voltage_set(settings,
      high_vin_shutoff_voltage);
  }

  {
    const uint8_t * p = buf + TIC_SETTING_VIN_MULTIPLIER_OFFSET;
    int16_t vin_multiplier_offset = p[0] + (p[1] << 8);
    tic_settings_vin_multiplier_offset_set(settings,
      vin_multiplier_offset);
  }

  {
    const uint8_t * p = buf + TIC_SETTING_RC_MAX_PULSE_PERIOD;
    uint16_t rc_max_pulse_period = p[0] + (p[1] << 8);
    tic_settings_rc_max_pulse_period_set(settings,
      rc_max_pulse_period);
  }

  {
    const uint8_t * p = buf + TIC_SETTING_RC_BAD_SIGNAL_TIMEOUT;
    uint16_t rc_bad_signal_timeout = p[0] + (p[1] << 8);
    tic_settings_rc_bad_signal_timeout_set(settings,
      rc_bad_signal_timeout);
  }

  {
    const uint8_t * p = buf + TIC_SETTING_RC_CONSECUTIVE_GOOD_PULSES;
    uint16_t rc_consecutive_good_pulses = p[0] + (p[1] << 8);
    tic_settings_rc_consecutive_good_pulses_set(settings,
      rc_consecutive_good_pulses);
  }

  {
    uint8_t input_play = buf[TIC_SETTING_INPUT_PLAY];
    tic_settings_input_play_set(settings, input_play);
  }

  {
    const uint8_t * p = buf + TIC_SETTING_INPUT_ERROR_MIN;
    uint16_t input_error_min = p[0] + (p[1] << 8);
    tic_settings_input_error_min_set(settings, input_error_min);
  }

  {
    const uint8_t * p = buf + TIC_SETTING_INPUT_ERROR_MAX;
    uint16_t input_error_max = p[0] + (p[1] << 8);
    tic_settings_input_error_max_set(settings, input_error_max);
  }

  {
    uint8_t input_scaling_degree = buf[TIC_SETTING_INPUT_SCALING_DEGREE];
    tic_settings_input_scaling_degree_set(settings, input_scaling_degree);
  }

  {
    bool input_invert = buf[TIC_SETTING_INPUT_INVERT] & 1;
    tic_settings_input_invert_set(settings, input_invert);
  }

  {
    const uint8_t * p = buf + TIC_SETTING_INPUT_MIN;
    uint16_t input_min = p[0] + (p[1] << 8);
    tic_settings_input_min_set(settings, input_min);
  }

  {
    const uint8_t * p = buf + TIC_SETTING_INPUT_NEUTRAL_MIN;
    uint16_t input_neutral_min = p[0] + (p[1] << 8);
    tic_settings_input_neutral_min_set(settings, input_neutral_min);
  }

  {
    const uint8_t * p = buf + TIC_SETTING_INPUT_NEUTRAL_MAX;
    uint16_t input_neutral_max = p[0] + (p[1] << 8);
    tic_settings_input_neutral_max_set(settings, input_neutral_max);
  }

  {
    const uint8_t * p = buf + TIC_SETTING_INPUT_MAX;
    uint16_t input_max = p[0] + (p[1] << 8);
    tic_settings_input_max_set(settings, input_max);
  }

  {
    const uint8_t * p = buf + TIC_SETTING_OUTPUT_MIN;
    int32_t output_min = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
    tic_settings_output_min_set(settings, output_min);
  }

  {
    const uint8_t * p = buf + TIC_SETTING_OUTPUT_NEUTRAL;
    int32_t output_neutral = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
    tic_settings_output_neutral_set(settings, output_neutral);
  }

  {
    const uint8_t * p = buf + TIC_SETTING_OUTPUT_MAX;
    int32_t output_max = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
    tic_settings_output_max_set(settings, output_max);
  }

  {
    uint8_t encoder_prescaler = buf[TIC_SETTING_ENCODER_PRESCALER];
    tic_settings_encoder_prescaler_set(settings, encoder_prescaler);
  }

  {
    const uint8_t * p = buf + TIC_SETTING_ENCODER_POSTSCALER;
    uint32_t encoder_postscaler = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
    tic_settings_encoder_postscaler_set(settings, encoder_postscaler);
  }

  {
    uint8_t scl_config = buf[TIC_SETTING_SCL_CONFIG];
    tic_settings_scl_config_set(settings, scl_config);
  }

  {
    uint8_t sda_config = buf[TIC_SETTING_SDA_CONFIG];
    tic_settings_sda_config_set(settings, sda_config);
  }

  {
    uint8_t tx_config = buf[TIC_SETTING_TX_CONFIG];
    tic_settings_tx_config_set(settings, tx_config);
  }

  {
    uint8_t rx_config = buf[TIC_SETTING_RX_CONFIG];
    tic_settings_rx_config_set(settings, rx_config);
  }

  {
    uint8_t rc_config = buf[TIC_SETTING_RC_CONFIG];
    tic_settings_rc_config_set(settings, rc_config);
  }

  {
    uint32_t current_limit = tic_current_limit_from_code(buf[TIC_SETTING_CURRENT_LIMIT]);
    tic_settings_current_limit_set(settings, current_limit);
  }

  {
    uint8_t step_mode = buf[TIC_SETTING_STEP_MODE];
    tic_settings_step_mode_set(settings, step_mode);
  }

  {
    uint8_t decay_mode = buf[TIC_SETTING_DECAY_MODE];
    tic_settings_decay_mode_set(settings, decay_mode);
  }

  {
    const uint8_t * p = buf + TIC_SETTING_SPEED_MIN;
    uint32_t speed_min = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
    tic_settings_speed_min_set(settings, speed_min);
  }

  {
    const uint8_t * p = buf + TIC_SETTING_SPEED_MAX;
    uint32_t speed_max = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
    tic_settings_speed_max_set(settings, speed_max);
  }

  {
    const uint8_t * p = buf + TIC_SETTING_DECEL_MAX;
    uint32_t decel_max = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
    tic_settings_decel_max_set(settings, decel_max);
  }

  {
    const uint8_t * p = buf + TIC_SETTING_ACCEL_MAX;
    uint32_t accel_max = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
    tic_settings_accel_max_set(settings, accel_max);
  }

  {
    const uint8_t * p = buf + TIC_SETTING_DECEL_MAX_DURING_ERROR;
    uint32_t decel_max_during_error = p[0] + (p[1] << 8) +
      (p[2] << 16) + (p[3] << 24);
    tic_settings_decel_max_during_error_set(settings, decel_max_during_error);
  }
}

tic_error * tic_get_settings(tic_handle * handle, tic_settings ** settings)
{
  if (settings == NULL)
  {
    return tic_error_create("Settings output pointer is null.");
  }

  *settings = NULL;

  if (handle == NULL)
  {
    return tic_error_create("Handle is null.");
  }

  tic_error * error = NULL;

  uint32_t product = TIC_PRODUCT_T825;  // TODO: get from handle

  // Allocate the new settings object.
  tic_settings * new_settings = NULL;
  if (error == NULL)
  {
    error = tic_settings_create(&new_settings);
  }

  // Specify what product these settings are for.
  if (error == NULL)
  {
    tic_settings_product_set(new_settings, product);
  }

  // Read all the settings from the device into a buffer.
  uint8_t buf[TIC_SETTINGS_SIZE];
  {
    memset(buf, 0, sizeof(buf));
    size_t index = 1;
    while (index < sizeof(buf) && error == NULL)
    {
      size_t length = TIC_MAX_USB_RESPONSE_SIZE;
      if (index + length > sizeof(buf))
      {
        length = sizeof(buf) - index;
      }
      error = tic_get_setting_segment(handle, index, length, buf + index);
      index += length;
    }
  }

  // Store the settings in the new settings object.
  if (error == NULL)
  {
    write_buffer_to_settings(buf, new_settings);
  }

  // Pass the new settings to the caller.
  if (error == NULL)
  {
    *settings = new_settings;
    new_settings = NULL;
  }

  tic_settings_free(new_settings);

  if (error != NULL)
  {
    error = tic_error_add(error,
      "There was an error reading settings from the device.");
  }

  return error;
}
