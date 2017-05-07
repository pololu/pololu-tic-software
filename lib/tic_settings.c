#include "tic_internal.h"

struct tic_settings
{
  uint32_t product;

  uint8_t control_mode;
  bool never_sleep;
  bool disable_safe_start;
  bool ignore_err_line_high;
  uint32_t serial_baud_rate;
  uint8_t serial_device_number;
  uint8_t i2c_device_address;
  uint16_t command_timeout;
  bool serial_crc_enabled;
  uint16_t low_vin_timeout;
  uint16_t low_vin_shutoff_voltage;
  uint16_t low_vin_startup_voltage;
  uint16_t high_vin_shutoff_voltage;
  int16_t vin_multiplier_offset;
  uint16_t rc_max_pulse_period;
  uint16_t rc_bad_signal_timeout;
  uint8_t rc_consecutive_good_pulses;
  uint8_t input_play;
  uint16_t input_error_min;
  uint16_t input_error_max;
  uint8_t input_scaling_degree;
  bool input_invert;
  uint16_t input_min;
  uint16_t input_neutral_min;
  uint16_t input_neutral_max;
  uint16_t input_max;
  int32_t output_min;
  int32_t output_neutral;
  int32_t output_max;
  uint8_t encoder_prescaler;
  uint32_t encoder_postscaler;
  uint8_t scl_config;
  uint8_t sda_config;
  uint8_t tx_config;
  uint8_t rx_config;
  uint8_t rc_config;
  uint32_t current_limit;
  uint8_t microstepping_mode;
  uint8_t decay_mode;
  uint32_t speed_min;
  uint32_t speed_max;
  uint32_t decel_max;
  uint32_t accel_max;
  uint32_t decel_max_during_error;
};

void tic_settings_fill_with_defaults(tic_settings * settings)
{
  assert(settings != NULL);

  uint32_t product = settings->product;

  // The product should be set beforehand, and if it is not then we should do
  // nothing.
  if (product != TIC_PRODUCT_T825)
  {
    return;
  }

  // Reset all fields to zero.
  memset(settings, 0, sizeof(tic_settings));

  tic_settings_product_set(settings, product);

  tic_settings_serial_baud_rate_set(settings, 9600);
  tic_settings_low_vin_timeout_set(settings, 250);
  tic_settings_low_vin_shutoff_voltage_set(settings, 6000);
  tic_settings_low_vin_startup_voltage_set(settings, 6500);
  tic_settings_high_vin_shutoff_voltage_set(settings, 35000);
  tic_settings_rc_max_pulse_period_set(settings, 100);
  tic_settings_rc_bad_signal_timeout_set(settings, 500);
  tic_settings_rc_consecutive_good_pulses_set(settings, 2);
  tic_settings_input_error_max_set(settings, 0xFFFF);
  tic_settings_input_neutral_min_set(settings, 0x8000);
  tic_settings_input_neutral_max_set(settings, 0x8000);
  tic_settings_input_max_set(settings, 0xFFFF);
  tic_settings_output_min_set(settings, -200);
  tic_settings_output_max_set(settings, 200);
  tic_settings_encoder_prescaler_set(settings, 2);
  tic_settings_encoder_postscaler_set(settings, 1);
  tic_settings_scl_config_set(settings,
    (1 << TIC_PIN_SERIAL) | (1 << TIC_PIN_ACTIVE_HIGH));
  tic_settings_sda_config_set(settings,
    (1 << TIC_PIN_SERIAL) | (1 << TIC_PIN_ACTIVE_HIGH));
  tic_settings_tx_config_set(settings,
    (1 << TIC_PIN_SERIAL) | (1 << TIC_PIN_ACTIVE_HIGH));
  tic_settings_rx_config_set(settings,
    (1 << TIC_PIN_SERIAL) | (1 << TIC_PIN_ACTIVE_HIGH));
  tic_settings_rc_config_set(settings,
    1 <<TIC_PIN_ACTIVE_HIGH);
  tic_settings_current_limit_set(settings, 192);
  tic_settings_speed_max_set(settings, 2000000);
  tic_settings_accel_max_set(settings, 40000);
}

tic_error * tic_settings_create(tic_settings ** settings)
{
  if (settings == NULL)
  {
    return tic_error_create("Settings output pointer is NULL.");
  }

  *settings = NULL;

  tic_error * error = NULL;

  tic_settings * new_settings = NULL;
  if (error == NULL)
  {
    new_settings = (tic_settings *)calloc(1, sizeof(tic_settings));
    if (new_settings == NULL) { error = &tic_error_no_memory; }
  }

  if (error == NULL)
  {
    *settings = new_settings;
    new_settings = NULL;
  }

  tic_settings_free(new_settings);

  return error;
}

tic_error * tic_settings_copy(const tic_settings * source, tic_settings ** dest)
{
  if (dest == NULL)
  {
    return tic_error_create("Settings output pointer is NULL.");
  }

  *dest = NULL;

  if (source == NULL)
  {
    return NULL;
  }

  tic_error * error = NULL;

  tic_settings * new_settings = NULL;
  if (error == NULL)
  {
    new_settings = (tic_settings *)calloc(1, sizeof(tic_settings));
    if (new_settings == NULL) { error = &tic_error_no_memory; }
  }

  if (error == NULL)
  {
    memcpy(new_settings, source, sizeof(tic_settings));
  }

  if (error == NULL)
  {
    *dest = new_settings;
    new_settings = NULL;
  }

  tic_settings_free(new_settings);

  return error;
}

void tic_settings_free(tic_settings * settings)
{
  free(settings);
}

static void tic_settings_fix_core(tic_settings * settings, tic_string * warnings)
{
  //tic_string_printf(warnings, "Warning: TODO: implement tic_settings_fix fully.\n");
  //tic_string_printf(warnings, "Warning: TODO: implement tic_settings_fix fully!!\n");

  (void)settings; (void)warnings;  // tmphax
}

tic_error * tic_settings_fix(tic_settings * settings, char ** warnings)
{
  if (warnings) { *warnings = NULL; }

  if (settings == NULL)
  {
    return tic_error_create("Tic settings pointer is NULL.");
  }

  // Make a string to store the warnings we accumulate in this function.
  tic_string str;
  if (warnings)
  {
    tic_string_setup(&str);
  }
  else
  {
    tic_string_setup_dummy(&str);
  }

  tic_settings_fix_core(settings, &str);

  if (warnings && str.data == NULL)
  {
    // Memory allocation for the warning string failed at some point.
    return &tic_error_no_memory;
  }

  if (warnings)
  {
    *warnings = str.data;
    str.data = NULL;
  }

  return NULL;
}

void tic_settings_product_set(tic_settings * settings, uint8_t product)
{
  if (!settings) { return; }
  settings->product = product;
}

uint8_t tic_settings_product_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->product;
}

void tic_settings_control_mode_set(tic_settings * settings,
  uint8_t control_mode)
{
  if (!settings) { return; }
  settings->control_mode = control_mode;
}

uint8_t tic_settings_control_mode_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->control_mode;
}

void tic_settings_never_sleep_set(tic_settings * settings, bool never_sleep)
{
  if (!settings) { return; }
  settings->never_sleep = never_sleep;
}

bool tic_settings_never_sleep_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->never_sleep;
}

void tic_settings_disable_safe_start_set(tic_settings * settings,
  bool disable_safe_start)
{
  if (!settings) { return; }
  settings->disable_safe_start = disable_safe_start;
}

bool tic_settings_disable_safe_start_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->disable_safe_start;
}

void tic_settings_ignore_err_line_high_set(tic_settings * settings,
  bool ignore_err_line_high)
{
  if (!settings) { return; }
  settings->ignore_err_line_high = ignore_err_line_high;
}

bool tic_settings_ignore_err_line_high_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->ignore_err_line_high;
}

void tic_settings_serial_baud_rate_set(tic_settings * settings, uint32_t serial_baud_rate)
{
  if (!settings) { return; }
  settings->serial_baud_rate = serial_baud_rate;
}

uint32_t tic_settings_serial_baud_rate_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->serial_baud_rate;
}

void tic_settings_serial_device_number_set(tic_settings * settings,
  uint8_t serial_device_number)
{
  if (!settings) { return; }
  settings->serial_device_number = serial_device_number;
}

uint8_t tic_settings_serial_device_number_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->serial_device_number;
}

void tic_settings_i2c_device_address_set(tic_settings * settings,
  uint8_t i2c_device_address)
{
  if (!settings) { return; }
  settings->i2c_device_address = i2c_device_address;
}

uint8_t tic_settings_i2c_device_address_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->i2c_device_address;
}

void tic_settings_command_timeout_set(tic_settings * settings,
  uint16_t command_timeout)
{
  if (!settings) { return; }
  settings->command_timeout = command_timeout;
}

uint16_t tic_settings_command_timeout_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->command_timeout;
}

void tic_settings_serial_crc_enabled_set(tic_settings * settings,
  bool serial_crc_enabled)
{
  if (!settings) { return; }
  settings->serial_crc_enabled = serial_crc_enabled;
}

bool tic_settings_serial_crc_enabled_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->serial_crc_enabled;
}

void tic_settings_low_vin_timeout_set(tic_settings * settings,
  uint16_t low_vin_timeout)
{
  if (!settings) { return; }
  settings->low_vin_timeout = low_vin_timeout;
}

uint16_t tic_settings_low_vin_timeout_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->low_vin_timeout;
}

void tic_settings_low_vin_shutoff_voltage_set(tic_settings * settings,
  uint16_t low_vin_shutoff_voltage)
{
  if (!settings) { return; }
  settings->low_vin_shutoff_voltage = low_vin_shutoff_voltage;
}

uint16_t tic_settings_low_vin_shutoff_voltage_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->low_vin_shutoff_voltage;
}

void tic_settings_low_vin_startup_voltage_set(tic_settings * settings,
  uint16_t low_vin_startup_voltage)
{
  if (!settings) { return; }
  settings->low_vin_startup_voltage = low_vin_startup_voltage;
}

uint16_t tic_settings_low_vin_startup_voltage_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->low_vin_startup_voltage;
}

void tic_settings_high_vin_shutoff_voltage_set(tic_settings * settings,
  uint16_t high_vin_shutoff_voltage)
{
  if (!settings) { return; }
  settings->high_vin_shutoff_voltage = high_vin_shutoff_voltage;
}

uint16_t tic_settings_high_vin_shutoff_voltage_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->high_vin_shutoff_voltage;
}

void tic_settings_vin_multiplier_offset_set(tic_settings * settings,
  uint16_t vin_multiplier_offset)
{
  if (!settings) { return; }
  settings->vin_multiplier_offset = vin_multiplier_offset;
}

uint16_t tic_settings_vin_multiplier_offset_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->vin_multiplier_offset;
}

void tic_settings_rc_max_pulse_period_set(tic_settings * settings,
  uint16_t rc_max_pulse_period)
{
  if (!settings) { return; }
  settings->rc_max_pulse_period = rc_max_pulse_period;
}

uint16_t tic_settings_rc_max_pulse_period_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->rc_max_pulse_period;
}

void tic_settings_rc_bad_signal_timeout_set(tic_settings * settings,
  uint16_t rc_bad_signal_timeout)
{
  if (!settings) { return; }
  settings->rc_bad_signal_timeout = rc_bad_signal_timeout;
}

uint16_t tic_settings_rc_bad_signal_timeout_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->rc_bad_signal_timeout;
}

void tic_settings_rc_consecutive_good_pulses_set(tic_settings * settings,
  uint8_t rc_consecutive_good_pulses)
{
  if (!settings) { return; }
  settings->rc_consecutive_good_pulses = rc_consecutive_good_pulses;
}

uint8_t tic_settings_rc_consecutive_good_pulses_get(
  const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->rc_consecutive_good_pulses;
}

void tic_settings_input_error_min_set(tic_settings * settings,
  uint16_t input_error_min)
{
  if (!settings) { return; }
  settings->input_error_min = input_error_min;
}

uint16_t tic_settings_input_error_min_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->input_error_min;
}

void tic_settings_input_error_max_set(tic_settings * settings,
  uint16_t input_error_max)
{
  if (!settings) { return; }
  settings->input_error_max = input_error_max;
}

uint16_t tic_settings_input_error_max_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->input_error_max;
}

void tic_settings_input_play_set(tic_settings * settings, uint8_t input_play)
{
  if (!settings) { return; }
  settings->input_play = input_play;
}

uint8_t tic_settings_input_play_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->input_play;
}

void tic_settings_input_scaling_degree_set(tic_settings * settings,
  uint8_t input_scaling_degree)
{
  if (!settings) { return; }
  settings->input_scaling_degree = input_scaling_degree;
}

uint8_t tic_settings_input_scaling_degree_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->input_scaling_degree;
}

void tic_settings_input_invert_set(tic_settings * settings, bool invert)
{
  if (!settings) { return; }
  settings->input_invert = invert;
}

bool tic_settings_input_invert_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->input_invert;
}

void tic_settings_input_min_set(tic_settings * settings, uint16_t input_min)
{
  if (!settings) { return; }
  settings->input_min = input_min;
}

uint16_t tic_settings_input_min_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->input_min;
}

void tic_settings_input_neutral_min_set(tic_settings * settings,
  uint16_t input_neutral_min)
{
  if (!settings) { return; }
  settings->input_neutral_min = input_neutral_min;
}

uint16_t tic_settings_input_neutral_min_get(const tic_settings * settings)
{
  if (!settings){ return 0; }
  return settings->input_neutral_min;
}

void tic_settings_input_neutral_max_set(tic_settings * settings,
  uint16_t input_neutral_max)
{
  if (!settings) { return; }
  settings->input_neutral_max = input_neutral_max;
}

uint16_t tic_settings_input_neutral_max_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->input_neutral_max;
}

void tic_settings_input_max_set(tic_settings * settings, uint16_t input_max)
{
  if (!settings) { return; }
  settings->input_max = input_max;
}

uint16_t tic_settings_input_max_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->input_max;
}

void tic_settings_output_min_set(tic_settings * settings, int32_t output_min)
{
  if (!settings) { return; }
  settings->output_min = output_min;
}

int32_t tic_settings_output_min_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->output_min;
}

void tic_settings_output_neutral_set(tic_settings * settings,
  int32_t output_neutral)
{
  if (!settings) { return; }
  settings->output_neutral = output_neutral;
}

int32_t tic_settings_output_neutral_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->output_neutral;
}

void tic_settings_output_max_set(tic_settings * settings, int32_t output_max)
{
  if (!settings) { return; }
  settings->output_max = output_max;
}

int32_t tic_settings_output_max_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->output_max;
}

void tic_settings_encoder_prescaler_set(tic_settings * settings,
  uint8_t encoder_prescaler)
{
  if (!settings) { return; }
  settings->encoder_prescaler = encoder_prescaler;
}

uint8_t tic_settings_encoder_prescaler_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->encoder_prescaler;
}

void tic_settings_encoder_postscaler_set(tic_settings * settings,
  uint32_t encoder_postscaler)
{
  if (!settings) { return; }
  settings->encoder_postscaler = encoder_postscaler;
}

uint32_t tic_settings_encoder_postscaler_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->encoder_postscaler;
}

void tic_settings_scl_config_set(tic_settings * settings, uint8_t scl_config)
{
  if (!settings) { return; }
  settings->scl_config = scl_config;
}

uint8_t tic_settings_scl_config_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->scl_config;
}


void tic_settings_sda_config_set(tic_settings * settings, uint8_t sda_config)
{
  if (!settings) { return; }
  settings->sda_config = sda_config;
}

uint8_t tic_settings_sda_config_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->sda_config;
}

void tic_settings_tx_config_set(tic_settings * settings, uint8_t tx_config)
{
  if (!settings) { return; }
  settings->tx_config = tx_config;
}

uint8_t tic_settings_tx_config_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->tx_config;
}

void tic_settings_rx_config_set(tic_settings * settings, uint8_t rx_config)
{
  if (!settings) { return; }
  settings->rx_config = rx_config;
}

uint8_t tic_settings_rx_config_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->rx_config;
}

void tic_settings_rc_config_set(tic_settings * settings, uint8_t rc_config)
{
  if (!settings) { return; }
  settings->rc_config = rc_config;
}

uint8_t tic_settings_rc_config_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->rc_config;
}

void tic_settings_current_limit_set(tic_settings * settings,
  uint32_t current_limit)
{
  if (!settings) { return; }
  settings->current_limit = current_limit;
}

uint32_t tic_settings_current_limit_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->current_limit;
}

void tic_settings_microstepping_mode_set(tic_settings * settings,
  uint8_t microstepping_mode)
{
  if (!settings) { return; }
  settings->microstepping_mode = microstepping_mode;
}

uint8_t tic_settings_microstepping_mode_get(const tic_settings * settings)
{
  if (!settings) { return 1; }
  return settings->microstepping_mode;
}

void tic_settings_decay_mode_set(tic_settings * settings, uint8_t decay_mode)
{
  if (!settings) { return; }
  settings->decay_mode = decay_mode;
}

uint8_t tic_settings_decay_mode_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->decay_mode;
}

void tic_settings_speed_min_set(tic_settings * settings, uint32_t speed_min)
{
  if (!settings) { return; }
  settings->speed_min = speed_min;
}

uint32_t tic_settings_speed_min_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->speed_min;
}

void tic_settings_speed_max_set(tic_settings * settings, uint32_t speed_max)
{
  if (!settings) { return; }
  settings->speed_max = speed_max;
}

uint32_t tic_settings_speed_max_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->speed_max;
}

void tic_settings_decel_max_set(tic_settings * settings, uint32_t decel_max)
{
  if (!settings) { return; }
  settings->decel_max = decel_max;
}

uint32_t tic_settings_decel_max_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->decel_max;
}

void tic_settings_accel_max_set(tic_settings * settings, uint32_t accel_max)
{
  if (!settings) { return; }
  settings->accel_max = accel_max;
}

uint32_t tic_settings_accel_max_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->accel_max;
}

void tic_settings_decel_max_during_error_set(tic_settings * settings,
  uint32_t decel_max_during_error)
{
  if (!settings) { return; }
  settings->decel_max_during_error = decel_max_during_error;
}

uint32_t tic_settings_decel_max_during_error_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->decel_max_during_error;
}
