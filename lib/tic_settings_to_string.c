#include "tic_internal.h"

static void print_pin_config_to_yaml(tic_string * str, const char * config_name, uint8_t value)
{
  assert(str != NULL);
  assert(config_name != NULL);

  const char * serial_str = "";
  if (value >> TIC_PIN_SERIAL_OR_I2C & 1) { serial_str = " serial"; }

  const char * pullup_str = "";
  if (value >> TIC_PIN_PULLUP & 1) { pullup_str = " pullup"; }

  const char * analog_str = "";
  if (value >> TIC_PIN_ANALOG & 1) { analog_str = " analog"; }

  const char * polarity_str = "";
  if (value >> TIC_PIN_ACTIVE_HIGH & 1) { analog_str = " active_high"; }

  const char * switch_str = "";
  switch(value >> TIC_PIN_SWITCH_POSN & TIC_PIN_SWITCH_MASK)
  {
  case TIC_PIN_SWITCH_LIMIT_FORWARD: switch_str = " limit_forward"; break;
  case TIC_PIN_SWITCH_LIMIT_REVERSE: switch_str = " limit_reverse"; break;
  case TIC_PIN_SWITCH_HOME: switch_str = " home"; break;
  case TIC_PIN_SWITCH_KILL: switch_str = " kill"; break;
  }

  tic_string_printf(str, "%s:%s%s%s%s%s\n", config_name,
    serial_str, pullup_str, analog_str, polarity_str, switch_str);
}

tic_error * tic_settings_to_string(const tic_settings * settings, char ** string)
{
  if (string == NULL)
  {
    return tic_error_create("String output pointer is NULL.");
  }

  *string = NULL;

  if (settings == NULL)
  {
    return tic_error_create("Settings pointer is NULL.");
  }

  tic_error * error = NULL;

  tic_string str;
  tic_string_setup(&str);

  {
    char * product_str = "";  // TODO: use name table
    switch (tic_settings_product_get(settings))
    {
    case TIC_PRODUCT_T825: product_str = "T825"; break;
    }
    tic_string_printf(&str, "product: %s\n", product_str);
  }

  {
    char * mode_str = "";  // TODO: use name table
    switch (tic_settings_control_mode_get(settings))
    {
    case TIC_CONTROL_MODE_SERIAL: mode_str = "serial"; break;
    case TIC_CONTROL_MODE_STEP_DIR: mode_str = "step_dir"; break;
    case TIC_CONTROL_MODE_RC_POSITION: mode_str = "rc_position"; break;
    case TIC_CONTROL_MODE_RC_SPEED: mode_str = "rc_speed"; break;
    case TIC_CONTROL_MODE_ANALOG_POSITION: mode_str = "analog_position"; break;
    case TIC_CONTROL_MODE_ANALOG_SPEED: mode_str = "analog_speed"; break;
    case TIC_CONTROL_MODE_ENCODER_POSITION: mode_str = "encoder_position"; break;
    case TIC_CONTROL_MODE_ENCODER_SPEED: mode_str = "encoder_speed"; break;
    }
    tic_string_printf(&str, "control_mode: %s\n", mode_str);
  }

  {
    bool never_sleep = tic_settings_never_sleep_get(settings);
    tic_string_printf(&str, "never_sleep: %s\n",
      never_sleep ? "true" : "false");
  }

  {
    bool disable_safe_start = tic_settings_disable_safe_start_get(settings);
    tic_string_printf(&str, "disable_safe_start: %s\n",
      disable_safe_start ? "true" : "false");
  }

  {
    bool ignore_err_line_high = tic_settings_ignore_err_line_high_get(settings);
    tic_string_printf(&str, "ignore_err_line_high: %s\n",
      ignore_err_line_high ? "true" : "false");
  }

  {
    uint32_t baud = tic_settings_serial_baud_rate_get(settings);
    tic_string_printf(&str, "serial_baud_rate: %u\n", baud);
  }

  {
    uint8_t device_number = tic_settings_serial_device_number_get(settings);
    tic_string_printf(&str, "serial_device_number: %u\n", device_number);
  }

  {
    uint16_t command_timeout = tic_settings_command_timeout_get(settings);
    tic_string_printf(&str, "command_timeout: %u\n", command_timeout);
  }

  {
    bool serial_crc_enabled = tic_settings_serial_crc_enabled_get(settings);
    tic_string_printf(&str, "serial_crc_enabled: %s\n",
      serial_crc_enabled ? "true" : "false");
  }

  {
    uint16_t low_vin_timeout = tic_settings_low_vin_timeout_get(settings);
    tic_string_printf(&str, "low_vin_timeout: %u\n", low_vin_timeout);
  }

  {
    uint16_t voltage = tic_settings_low_vin_shutoff_voltage_get(settings);
    tic_string_printf(&str, "low_vin_shutoff_voltage: %u\n", voltage);
  }

  {
    uint16_t voltage = tic_settings_low_vin_startup_voltage_get(settings);
    tic_string_printf(&str, "low_vin_startup_voltage: %u\n", voltage);
  }

  {
    uint16_t voltage = tic_settings_high_vin_shutoff_voltage_get(settings);
    tic_string_printf(&str, "high_vin_shutoff_voltage: %u\n", voltage);
  }

  {
    int16_t offset = tic_settings_vin_multiplier_offset_get(settings);
    tic_string_printf(&str, "vin_multiplier_offset: %d\n", offset);
  }

  {
    uint16_t pulse_period = tic_settings_rc_max_pulse_period_get(settings);
    tic_string_printf(&str, "rc_max_pulse_period: %u\n", pulse_period);
  }

  {
    uint16_t timeout = tic_settings_rc_bad_signal_timeout_get(settings);
    tic_string_printf(&str, "rc_bad_signal_timeout: %u\n", timeout);
  }

  {
    uint16_t pulses = tic_settings_rc_consecutive_good_pulses_get(settings);
    tic_string_printf(&str, "rc_consecutive_good_pulses_get: %u\n", pulses);
  }

  {
    uint8_t input_play = tic_settings_input_play_get(settings);
    tic_string_printf(&str, "input_play: %u\n", input_play);
  }

  {
    uint16_t input_error_min = tic_settings_input_error_min_get(settings);
    tic_string_printf(&str, "input_error_min: %u\n", input_error_min);
  }

  {
    uint16_t input_error_max = tic_settings_input_error_max_get(settings);
    tic_string_printf(&str, "input_error_max: %u\n", input_error_max);
  }

  {
    const char * degree_str = ""; // TODO: use name table
    switch(tic_settings_input_scaling_degree_get(settings))
    {
    case TIC_SCALING_DEGREE_LINEAR: degree_str = "linear"; break;
    case TIC_SCALING_DEGREE_QUADRATIC: degree_str = "quadratic"; break;
    case TIC_SCALING_DEGREE_CUBIC: degree_str = "cubic"; break;
    }
    tic_string_printf(&str, "input_scaling_degree: %s\n", degree_str);
  }

  {
    bool input_invert = tic_settings_input_invert_get(settings);
    tic_string_printf(&str, "input_invert: %s\n",
      input_invert ? "true" : "false");
  }

  {
    uint16_t input_min = tic_settings_input_min_get(settings);
    tic_string_printf(&str, "input_min: %u\n", input_min);
  }

  {
    uint16_t input_neutral_min = tic_settings_input_neutral_min_get(settings);
    tic_string_printf(&str, "input_neutral_min: %u\n", input_neutral_min);
  }

  {
    uint16_t input_neutral_max = tic_settings_input_neutral_max_get(settings);
    tic_string_printf(&str, "input_neutral_max: %u\n", input_neutral_max);
  }

  {
    uint16_t input_max = tic_settings_input_max_get(settings);
    tic_string_printf(&str, "input_max: %u\n", input_max);
  }

  {
    int32_t output = tic_settings_output_min_get(settings);
    tic_string_printf(&str, "output_min: %d\n", output);
  }

  {
    int32_t output = tic_settings_output_neutral_get(settings);
    tic_string_printf(&str, "output_neutral: %d\n", output);
  }

  {
    int32_t output = tic_settings_output_max_get(settings);
    tic_string_printf(&str, "output_max: %d\n", output);
  }

  {
    uint8_t encoder_prescaler = tic_settings_encoder_prescaler_get(settings);
    tic_string_printf(&str, "encoder_prescaler: %u\n", encoder_prescaler);
  }

  {
    uint32_t encoder_postscaler = tic_settings_encoder_postscaler_get(settings);
    tic_string_printf(&str, "encoder_postscaler: %u\n", encoder_postscaler);
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
    tic_string_printf(&str, "current_limit: %u\n", current_limit);
  }

  {
    const char * mode_str = "";  // TODO: use name table
    switch (tic_settings_microstepping_mode_get(settings))
    {
    case TIC_MICROSTEPPING_MODE_1: mode_str = "1"; break;
    case TIC_MICROSTEPPING_MODE_2: mode_str = "2"; break;
    case TIC_MICROSTEPPING_MODE_4: mode_str = "4"; break;
    case TIC_MICROSTEPPING_MODE_8: mode_str = "8"; break;
    case TIC_MICROSTEPPING_MODE_16: mode_str = "16"; break;
    case TIC_MICROSTEPPING_MODE_32: mode_str = "32"; break;
    }
    tic_string_printf(&str, "microstepping_mode: %s\n", mode_str);
  }

  {
    const char * decay_mode_str = "";  // TODO: use name table
    switch (tic_settings_decay_mode_get(settings))
    {
    case TIC_DECAY_MODE_MIXED: decay_mode_str = "mixed"; break;
    case TIC_DECAY_MODE_SLOW: decay_mode_str = "slow"; break;
    case TIC_DECAY_MODE_FAST: decay_mode_str = "fast"; break;
    }
    tic_string_printf(&str, "decay_mode: %s\n", decay_mode_str);
  }

  {
    uint32_t speed_min = tic_settings_speed_min_get(settings);
    tic_string_printf(&str, "speed_min: %u\n", speed_min);
  }

  {
    uint32_t speed_max = tic_settings_speed_max_get(settings);
    tic_string_printf(&str, "speed_max: %u\n", speed_max);
  }

  {
    uint32_t accel = tic_settings_accel_max_get(settings);
    tic_string_printf(&str, "accel_max: %u\n", accel);
  }

  {
    uint32_t decel = tic_settings_decel_max_get(settings);
    tic_string_printf(&str, "decel_max: %u\n", decel);
  }

  {
    uint32_t decel = tic_settings_decel_max_during_error_get(settings);
    tic_string_printf(&str, "decel_max_during_error: %u\n", decel);
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
