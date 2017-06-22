#include "tic_internal.h"

static void print_pin_config_to_yaml(tic_string * str,
  const char * config_name, uint8_t value)
{
  assert(str != NULL);
  assert(config_name != NULL);

  const char * pullup_str = "";
  if (value >> TIC_PIN_PULLUP & 1) { pullup_str = " pullup"; }

  const char * analog_str = "";
  if (value >> TIC_PIN_ANALOG & 1) { analog_str = " analog"; }

  const char * polarity_str = "";
  if (value >> TIC_PIN_ACTIVE_HIGH & 1) { polarity_str = " active_high"; }

  const char * func_str;
  tic_code_to_name(tic_pin_config_names, (value & TIC_PIN_FUNC_MASK), &func_str);

  tic_sprintf(str, "%s: %s%s%s%s\n", config_name, func_str,
    pullup_str, analog_str, polarity_str);
}

tic_error * tic_settings_to_string(const tic_settings * settings, char ** string)
{
  if (string == NULL)
  {
    return tic_error_create("String output pointer is null.");
  }

  *string = NULL;

  if (settings == NULL)
  {
    return tic_error_create("Settings pointer is null.");
  }

  tic_error * error = NULL;

  tic_string str;
  tic_string_setup(&str);

  tic_sprintf(&str, "# Pololu Tic USB Stepper Controller settings file.\n");
  tic_sprintf(&str, "# " DOCUMENTATION_URL "\n");

  {
    uint32_t product = tic_settings_product_get(settings);
    const char * product_str;
    tic_code_to_name(tic_product_names, product, &product_str);
    tic_sprintf(&str, "product: %s\n", product_str);
  }

  {
    uint8_t control_mode = tic_settings_control_mode_get(settings);
    const char * mode_str;
    tic_code_to_name(tic_control_mode_names, control_mode, &mode_str);
    tic_sprintf(&str, "control_mode: %s\n", mode_str);
  }

  {
    bool never_sleep = tic_settings_never_sleep_get(settings);
    tic_sprintf(&str, "never_sleep: %s\n",
      never_sleep ? "true" : "false");
  }

  {
    bool disable_safe_start = tic_settings_disable_safe_start_get(settings);
    tic_sprintf(&str, "disable_safe_start: %s\n",
      disable_safe_start ? "true" : "false");
  }

  {
    bool ignore_err_line_high = tic_settings_ignore_err_line_high_get(settings);
    tic_sprintf(&str, "ignore_err_line_high: %s\n",
      ignore_err_line_high ? "true" : "false");
  }

  {
    bool auto_clear = tic_settings_auto_clear_driver_error_get(settings);
    tic_sprintf(&str, "auto_clear_driver_error: %s\n",
      auto_clear ? "true" : "false");
  }

  {
    uint8_t response = tic_settings_input_invalid_response_get(settings);
    const char * response_str;
    tic_code_to_name(tic_response_names, response, &response_str);
    tic_sprintf(&str, "input_invalid_response: %s\n", response_str);
  }

  {
    int32_t position = tic_settings_input_invalid_position_get(settings);
    tic_sprintf(&str, "input_invalid_position: %d\n", position);
  }

  {
    uint32_t baud = tic_settings_serial_baud_rate_get(settings);
    tic_sprintf(&str, "serial_baud_rate: %u\n", baud);
  }

  {
    uint8_t device_number = tic_settings_serial_device_number_get(settings);
    tic_sprintf(&str, "serial_device_number: %u\n", device_number);
  }

  {
    uint16_t command_timeout = tic_settings_command_timeout_get(settings);
    tic_sprintf(&str, "command_timeout: %u\n", command_timeout);
  }

  {
    bool serial_crc_enabled = tic_settings_serial_crc_enabled_get(settings);
    tic_sprintf(&str, "serial_crc_enabled: %s\n",
      serial_crc_enabled ? "true" : "false");
  }

  {
    uint16_t low_vin_timeout = tic_settings_low_vin_timeout_get(settings);
    tic_sprintf(&str, "low_vin_timeout: %u\n", low_vin_timeout);
  }

  {
    uint16_t voltage = tic_settings_low_vin_shutoff_voltage_get(settings);
    tic_sprintf(&str, "low_vin_shutoff_voltage: %u\n", voltage);
  }

  {
    uint16_t voltage = tic_settings_low_vin_startup_voltage_get(settings);
    tic_sprintf(&str, "low_vin_startup_voltage: %u\n", voltage);
  }

  {
    uint16_t voltage = tic_settings_high_vin_shutoff_voltage_get(settings);
    tic_sprintf(&str, "high_vin_shutoff_voltage: %u\n", voltage);
  }

  {
    int16_t offset = tic_settings_vin_multiplier_offset_get(settings);
    tic_sprintf(&str, "vin_multiplier_offset: %d\n", offset);
  }

  {
    uint16_t pulse_period = tic_settings_rc_max_pulse_period_get(settings);
    tic_sprintf(&str, "rc_max_pulse_period: %u\n", pulse_period);
  }

  {
    uint16_t timeout = tic_settings_rc_bad_signal_timeout_get(settings);
    tic_sprintf(&str, "rc_bad_signal_timeout: %u\n", timeout);
  }

  {
    uint16_t pulses = tic_settings_rc_consecutive_good_pulses_get(settings);
    tic_sprintf(&str, "rc_consecutive_good_pulses: %u\n", pulses);
  }

  {
    bool enabled = tic_settings_input_averaging_enabled_get(settings);
    tic_sprintf(&str, "input_averaging_enabled: %s\n",
      enabled ? "true" : "false");
  }

  {
    uint16_t input_hysteresis = tic_settings_input_hysteresis_get(settings);
    tic_sprintf(&str, "input_hysteresis: %u\n", input_hysteresis);
  }

  {
    uint16_t input_error_min = tic_settings_input_error_min_get(settings);
    tic_sprintf(&str, "input_error_min: %u\n", input_error_min);
  }

  {
    uint16_t input_error_max = tic_settings_input_error_max_get(settings);
    tic_sprintf(&str, "input_error_max: %u\n", input_error_max);
  }

  {
    uint8_t degree = tic_settings_input_scaling_degree_get(settings);
    const char * degree_str;
    tic_code_to_name(tic_scaling_degree_names, degree, &degree_str);
    tic_sprintf(&str, "input_scaling_degree: %s\n", degree_str);
  }

  {
    bool input_invert = tic_settings_input_invert_get(settings);
    tic_sprintf(&str, "input_invert: %s\n", input_invert ? "true" : "false");
  }

  {
    uint16_t input_min = tic_settings_input_min_get(settings);
    tic_sprintf(&str, "input_min: %u\n", input_min);
  }

  {
    uint16_t input_neutral_min = tic_settings_input_neutral_min_get(settings);
    tic_sprintf(&str, "input_neutral_min: %u\n", input_neutral_min);
  }

  {
    uint16_t input_neutral_max = tic_settings_input_neutral_max_get(settings);
    tic_sprintf(&str, "input_neutral_max: %u\n", input_neutral_max);
  }

  {
    uint16_t input_max = tic_settings_input_max_get(settings);
    tic_sprintf(&str, "input_max: %u\n", input_max);
  }

  {
    int32_t output = tic_settings_output_min_get(settings);
    tic_sprintf(&str, "output_min: %d\n", output);
  }

  {
    int32_t output = tic_settings_output_max_get(settings);
    tic_sprintf(&str, "output_max: %d\n", output);
  }

  {
    uint32_t encoder_prescaler = tic_settings_encoder_prescaler_get(settings);
    tic_sprintf(&str, "encoder_prescaler: %u\n", encoder_prescaler);
  }

  {
    uint32_t encoder_postscaler = tic_settings_encoder_postscaler_get(settings);
    tic_sprintf(&str, "encoder_postscaler: %u\n", encoder_postscaler);
  }

  {
    bool encoder_unlimited = tic_settings_encoder_unlimited_get(settings);
    tic_sprintf(&str, "encoder_unlimited: %s\n", encoder_unlimited ? "true" : "false");
  }

  {
    uint8_t scl_config = tic_settings_scl_config_get(settings);
    print_pin_config_to_yaml(&str, "scl_config", scl_config);
  }

  {
    uint8_t sda_config = tic_settings_sda_config_get(settings);
    print_pin_config_to_yaml(&str, "sda_config", sda_config);
  }

  {
    uint8_t tx_config = tic_settings_tx_config_get(settings);
    print_pin_config_to_yaml(&str, "tx_config", tx_config);
  }

  {
    uint8_t rx_config = tic_settings_rx_config_get(settings);
    print_pin_config_to_yaml(&str, "rx_config", rx_config);
  }

  {
    uint8_t rc_config = tic_settings_rc_config_get(settings);
    print_pin_config_to_yaml(&str, "rc_config", rc_config);
  }

  {
    uint32_t current_limit = tic_settings_current_limit_get(settings);
    tic_sprintf(&str, "current_limit: %u\n", current_limit);
  }

  {
    uint8_t mode = tic_settings_step_mode_get(settings);
    const char * mode_str = tic_look_up_step_mode_string(mode);
    tic_sprintf(&str, "step_mode: %s\n", mode_str);
  }

  {
    uint8_t decay_mode = tic_settings_decay_mode_get(settings);
    const char * decay_mode_str = tic_look_up_decay_mode_string(decay_mode);
    tic_sprintf(&str, "decay_mode: %s\n", decay_mode_str);
  }

  {
    uint32_t speed_min = tic_settings_speed_min_get(settings);
    tic_sprintf(&str, "speed_min: %u\n", speed_min);
  }

  {
    uint32_t speed_max = tic_settings_speed_max_get(settings);
    tic_sprintf(&str, "speed_max: %u\n", speed_max);
  }

  {
    uint32_t accel = tic_settings_accel_max_get(settings);
    tic_sprintf(&str, "accel_max: %u\n", accel);
  }

  {
    uint32_t decel = tic_settings_decel_max_get(settings);
    tic_sprintf(&str, "decel_max: %u\n", decel);
  }

  if (error == NULL && str.data == NULL)
  {
    error = &tic_error_no_memory;
  }

  if (error == NULL)
  {
    *string = str.data;
    str.data = NULL;
  }

  tic_string_free(str.data);

  return error;
}
