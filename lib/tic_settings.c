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
  uint8_t input_averaging_enabled;
  uint16_t input_hysteresis;
  uint16_t input_error_min;
  uint16_t input_error_max;
  uint8_t input_scaling_degree;
  bool input_invert;
  uint16_t input_min;
  uint16_t input_neutral_min;
  uint16_t input_neutral_max;
  uint16_t input_max;
  int32_t output_min;
  int32_t output_max;
  uint32_t encoder_prescaler;
  uint32_t encoder_postscaler;
  bool encoder_unlimited;
  uint8_t scl_config;
  uint8_t sda_config;
  uint8_t tx_config;
  uint8_t rx_config;
  uint8_t rc_config;
  uint32_t current_limit;
  uint8_t step_mode;
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
  tic_settings_serial_device_number_set(settings, 14);
  tic_settings_low_vin_timeout_set(settings, 250);
  tic_settings_low_vin_shutoff_voltage_set(settings, 6000);
  tic_settings_low_vin_startup_voltage_set(settings, 6500);
  tic_settings_high_vin_shutoff_voltage_set(settings, 35000);
  tic_settings_rc_max_pulse_period_set(settings, 100);
  tic_settings_rc_bad_signal_timeout_set(settings, 500);
  tic_settings_rc_consecutive_good_pulses_set(settings, 2);
  tic_settings_input_averaging_enabled_set(settings, true);
  tic_settings_input_error_max_set(settings, 0xFFFF);
  tic_settings_input_neutral_min_set(settings, 0x0800);
  tic_settings_input_neutral_max_set(settings, 0x0800);
  tic_settings_input_max_set(settings, 0x0FFF);
  tic_settings_output_min_set(settings, -200);
  tic_settings_output_max_set(settings, 200);
  tic_settings_encoder_prescaler_set(settings, 1);
  tic_settings_encoder_postscaler_set(settings, 1);
  tic_settings_current_limit_set(settings, 192);
  tic_settings_speed_max_set(settings, 2000000);
  tic_settings_accel_max_set(settings, 40000);
}

tic_error * tic_settings_create(tic_settings ** settings)
{
  if (settings == NULL)
  {
    return tic_error_create("Settings output pointer is null.");
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

void tic_settings_free(tic_settings * settings)
{
  free(settings);
}

tic_error * tic_settings_copy(const tic_settings * source, tic_settings ** dest)
{
  if (dest == NULL)
  {
    return tic_error_create("Settings output pointer is null.");
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

uint32_t tic_settings_achievable_baud_rate(const tic_settings * settings,
  uint32_t baud)
{
  if (settings == NULL) { return 0; }
  uint16_t brg = tic_baud_rate_to_brg(baud);
  return tic_baud_rate_from_brg(brg);
}

uint32_t tic_settings_achievable_current_limit(const tic_settings * settings,
  uint32_t current_limit)
{
  if (settings == NULL) { return 0; }
  uint8_t code = tic_current_limit_to_code(current_limit);
  return tic_current_limit_from_code(code);
}

// TODO: use present and future tense for these messages, not past tense

static void tic_settings_fix_core(tic_settings * settings, tic_string * warnings)
{
  uint8_t control_mode = tic_settings_control_mode_get(settings);

  {
    uint32_t baud = tic_settings_serial_baud_rate_get(settings);
    if (baud < TIC_MIN_ALLOWED_BAUD_RATE)
    {
      baud = TIC_MIN_ALLOWED_BAUD_RATE;
      tic_sprintf(warnings,
        "Warning: The serial baud rate was too low so it was changed to %u.\n", baud);
    }
    if (baud > TIC_MAX_ALLOWED_BAUD_RATE)
    {
      baud = TIC_MAX_ALLOWED_BAUD_RATE;
      tic_sprintf(warnings,
        "Warning: The serial baud rate was too high so it was changed to %u.\n", baud);
    }

    baud = tic_settings_achievable_baud_rate(settings, baud);
    tic_settings_serial_baud_rate_set(settings, baud);
  }

  {
    uint8_t serial_device_number = tic_settings_serial_device_number_get(settings);
    if (serial_device_number > 127)
    {
      serial_device_number = 127;
      tic_sprintf(warnings,
        "Warning: The serial device number was too high so it was changed to 127.\n");
    }
    tic_settings_serial_device_number_set(settings, serial_device_number);
  }

  {
    uint16_t low_shutoff = tic_settings_low_vin_shutoff_voltage_get(settings);
    uint16_t low_startup = tic_settings_low_vin_startup_voltage_get(settings);
    uint16_t high_shutoff = tic_settings_high_vin_shutoff_voltage_get(settings);

    // Move low_shutoff down a little bit to prevent overflows below.
    if (low_shutoff > 64000)
    {
      low_shutoff = 64000;
      tic_sprintf(warnings,
        "Warning: The low VIN shutoff voltage was changed to %u mV.\n",
        low_shutoff);
    }

    if (low_startup < low_shutoff)
    {
      low_startup = low_shutoff + 500;
      tic_sprintf(warnings,
        "Warning: The low VIN startup voltage was changed to %u mV.\n",
        low_startup);
    }

    if (high_shutoff < low_startup)
    {
      high_shutoff = low_startup + 500;
      tic_sprintf(warnings,
        "Warning: The high VIN shutoff voltage was changed to %u mV.\n",
        high_shutoff);
    }

    tic_settings_low_vin_shutoff_voltage_set(settings, low_shutoff);
    tic_settings_low_vin_startup_voltage_set(settings, low_startup);
    tic_settings_high_vin_shutoff_voltage_set(settings, high_shutoff);
  }

  {
    uint16_t error_min = tic_settings_input_error_min_get(settings);
    uint16_t min = tic_settings_input_min_get(settings);
    uint16_t neutral_min = tic_settings_input_neutral_min_get(settings);
    uint16_t neutral_max = tic_settings_input_neutral_max_get(settings);
    uint16_t max = tic_settings_input_max_get(settings);
    uint16_t error_max = tic_settings_input_error_max_get(settings);

    if (error_min > min || min > neutral_min || neutral_min > neutral_max ||
      neutral_max > max || max > error_max)
    {
      error_min = 0;
      min = 0;
      neutral_min = 0x8000;
      neutral_max = 0x8000;
      max = 0xFFFF;
      error_max = 0xFFFF;

      tic_sprintf(warnings,
        "Warning: The input scaling values were out of order "
        "so they will be reset to their default values.\n");
    }

    tic_settings_input_error_min_set(settings, error_min);
    tic_settings_input_min_set(settings, min);
    tic_settings_input_neutral_min_set(settings, neutral_min);
    tic_settings_input_neutral_max_set(settings, neutral_max);
    tic_settings_input_max_set(settings, max);
    tic_settings_input_error_max_set(settings, error_max);
  }

  {
    int32_t output_min = tic_settings_output_min_get(settings);
    int32_t output_max = tic_settings_output_max_get(settings);

    // TODO: enforce allowed range of output_min, output_max

    tic_settings_output_min_set(settings, output_min);
    tic_settings_output_max_set(settings, output_max);
  }

  {
    uint32_t prescaler = tic_settings_encoder_prescaler_get(settings);

    if (prescaler > TIC_MAX_ALLOWED_ENCODER_PRESCALER)
    {
      prescaler = TIC_MAX_ALLOWED_ENCODER_PRESCALER;
      tic_sprintf(warnings,
        "Warning: The encoder prescaler was too high "
        "so it will be lowered to %u.\n", prescaler);
    }

    if (prescaler < 1)
    {
      prescaler = 1;
      tic_sprintf(warnings,
        "Warning: The encoder prescaler was zero "
        "so it will be changed to 1.\n");
    }

    tic_settings_encoder_prescaler_set(settings, prescaler);
  }

  {
    uint32_t postscaler = tic_settings_encoder_postscaler_get(settings);

    if (postscaler > TIC_MAX_ALLOWED_ENCODER_POSTSCALER)
    {
      postscaler = TIC_MAX_ALLOWED_ENCODER_POSTSCALER;
      tic_sprintf(warnings,
        "Warning: The encoder postscaler was too high "
        "so it will be lowered to %u.\n", postscaler);
    }

    if (postscaler < 1)
    {
      postscaler = 1;
      tic_sprintf(warnings,
        "Warning: The encoder postscaler was zero "
        "so it will be changed to 1.\n");
    }

    tic_settings_encoder_postscaler_set(settings, postscaler);
  }

  {
    uint32_t current = tic_settings_current_limit_get(settings);

    if (current > TIC_MAX_ALLOWED_CURRENT)
    {
      current = TIC_MAX_ALLOWED_CURRENT;
      tic_sprintf(warnings,
        "Warning: The current limit was too high "
        "so it will be lowered to %u mA.\n", current);
    }

    current = tic_settings_achievable_current_limit(settings, current);
    tic_settings_current_limit_set(settings, current);
  }

  {
    uint8_t decay_mode = tic_settings_decay_mode_get(settings);
    if (decay_mode != TIC_DECAY_MODE_MIXED &&
      decay_mode != TIC_DECAY_MODE_SLOW &&
      decay_mode != TIC_DECAY_MODE_FAST)
    {
      decay_mode = TIC_DECAY_MODE_MIXED;
      tic_sprintf(warnings,
        "Warning: The decay mode was invalid "
        "so it will be changed to mixed.\n");
    }
  }

  {
    uint8_t mode = tic_settings_step_mode_get(settings);
    if (mode != TIC_STEP_MODE_MICROSTEP1 &&
      mode != TIC_STEP_MODE_MICROSTEP2 &&
      mode != TIC_STEP_MODE_MICROSTEP4 &&
      mode != TIC_STEP_MODE_MICROSTEP8 &&
      mode != TIC_STEP_MODE_MICROSTEP16 &&
      mode != TIC_STEP_MODE_MICROSTEP32)
    {
      mode = TIC_STEP_MODE_MICROSTEP1;
      tic_sprintf(warnings,
        "Warning: The step mode was invalid "
        "so it will be changed to 1 (full step).\n");
    }

    tic_settings_step_mode_set(settings, mode);
  }

  {
    uint32_t speed_max = tic_settings_speed_max_get(settings);
    uint32_t speed_min = tic_settings_speed_min_get(settings);

    if (speed_max > TIC_MAX_ALLOWED_SPEED)
    {
      speed_max = TIC_MAX_ALLOWED_SPEED;
      uint32_t speed_max_khz = speed_max / TIC_SPEED_UNITS_PER_HZ / 1000;
      tic_sprintf(warnings,
        "Warning: The maximum speed was too high so it will be lowered to %u (%u kHz).\n",
        speed_max, speed_max_khz);
    }

    if (speed_min > speed_max)
    {
      speed_min = speed_max;
      tic_sprintf(warnings,
        "Warning: The minimum speed was greater than the maximum speed "
        "so it will be lowered to %u.\n", speed_min);
    }

    tic_settings_speed_max_set(settings, speed_max);
    tic_settings_speed_min_set(settings, speed_min);
  }

  {
    uint32_t decel_max = tic_settings_decel_max_get(settings);

    if (decel_max > TIC_MAX_ALLOWED_ACCEL)
    {
      decel_max = TIC_MAX_ALLOWED_ACCEL;
      tic_sprintf(warnings,
        "Warning: The maximum deceleration was too high "
        "so it will be lowered to %u.\n", decel_max);
    }

    if (decel_max != 0 && decel_max < TIC_MIN_ALLOWED_ACCEL)
    {
      decel_max = TIC_MIN_ALLOWED_ACCEL;
      tic_sprintf(warnings,
        "Warning: The maximum deceleration was too low "
        "so it will be raised to %u.\n", decel_max);
    }

    tic_settings_decel_max_set(settings, decel_max);
  }

  {
    uint32_t accel_max = tic_settings_accel_max_get(settings);

    if (accel_max > TIC_MAX_ALLOWED_ACCEL)
    {
      accel_max = TIC_MAX_ALLOWED_ACCEL;
      tic_sprintf(warnings,
        "Warning: The maximum acceleration was too high "
        "so it will be lowered to %u.\n", accel_max);
    }

    if (accel_max < TIC_MIN_ALLOWED_ACCEL)
    {
      accel_max = TIC_MIN_ALLOWED_ACCEL;
      tic_sprintf(warnings,
        "Warning: The maximum acceleration was too low "
        "so it will be raised to %u.\n", accel_max);
    }

    tic_settings_accel_max_set(settings, accel_max);
  }

  {
    uint32_t decel_max_during_error = tic_settings_decel_max_during_error_get(settings);

    if (decel_max_during_error > TIC_MAX_ALLOWED_ACCEL)
    {
      decel_max_during_error = TIC_MAX_ALLOWED_ACCEL;
      tic_sprintf(warnings,
        "Warning: The maximum deceleration during error was too high "
        "so it will be lowered to %u.\n", decel_max_during_error);
    }

    if (decel_max_during_error != 0 &&
      decel_max_during_error < TIC_MIN_ALLOWED_ACCEL)
    {
      decel_max_during_error = TIC_MIN_ALLOWED_ACCEL;
      tic_sprintf(warnings,
        "Warning: The maximum deceleration during error was too low "
        "so it will be raised to %u.\n", decel_max_during_error);
    }

    tic_settings_decel_max_during_error_set(settings, decel_max_during_error);
  }

  {
    #if TIC_PIN_FUNC_POSN != 0
    #error This code needs to be fixed.
    #endif
    uint8_t scl_config = tic_settings_scl_config_get(settings);
    uint8_t sda_config = tic_settings_sda_config_get(settings);
    uint8_t tx_config = tic_settings_tx_config_get(settings);
    uint8_t rx_config = tic_settings_rx_config_get(settings);
    uint8_t rc_config = tic_settings_rc_config_get(settings);

    // First, we make sure the pins are configured to provide the primary
    // input that will be used to control the motor.

    switch (control_mode)
    {
    case TIC_CONTROL_MODE_ANALOG_POSITION:
    case TIC_CONTROL_MODE_ANALOG_SPEED:
      if ((sda_config & TIC_PIN_FUNC_MASK) != TIC_PIN_FUNC_DEFAULT &&
        (sda_config & TIC_PIN_FUNC_MASK) != TIC_PIN_FUNC_GENERAL)
      {
        sda_config = (sda_config & ~TIC_PIN_FUNC_MASK) | TIC_PIN_FUNC_DEFAULT;
        tic_sprintf(warnings,
          "Warning: The SDA pin must be used as an analog input "
          "so its function will be changed to the default.\n");
      }
      break;
    case TIC_CONTROL_MODE_RC_POSITION:
    case TIC_CONTROL_MODE_RC_SPEED:
      if ((rc_config & TIC_PIN_FUNC_MASK) != TIC_PIN_FUNC_DEFAULT &&
        (rc_config & TIC_PIN_FUNC_MASK) != TIC_PIN_FUNC_RC)
      {
        rc_config = (rc_config & ~TIC_PIN_FUNC_MASK) | TIC_PIN_FUNC_DEFAULT;
        tic_sprintf(warnings,
          "Warning: The RC pin must be used as an RC input "
          "so its function will be changed to the default.\n");
      }
      break;
    case TIC_CONTROL_MODE_ENCODER_POSITION:
    case TIC_CONTROL_MODE_ENCODER_SPEED:
      if ((tx_config & TIC_PIN_FUNC_MASK) != TIC_PIN_FUNC_DEFAULT &&
        (tx_config & TIC_PIN_FUNC_MASK) != TIC_PIN_FUNC_ENCODER)
      {
        tx_config = (tx_config & ~TIC_PIN_FUNC_MASK) | TIC_PIN_FUNC_DEFAULT;
        tic_sprintf(warnings,
          "Warning: The TX pin must be used as an encoder input "
          "so its function will be changed to the default.\n");
      }
      if ((rx_config & TIC_PIN_FUNC_MASK) != TIC_PIN_FUNC_DEFAULT &&
        (rx_config & TIC_PIN_FUNC_MASK) != TIC_PIN_FUNC_ENCODER)
      {
        rx_config = (rx_config & ~TIC_PIN_FUNC_MASK) | TIC_PIN_FUNC_DEFAULT;
        tic_sprintf(warnings,
          "Warning: The RX pin must be used as an encoder input "
          "so its function will be changed to the default.\n");
      }
      break;
    }

    // In this section, we make sure no pin is configured to do something that
    // it cannot do.

    if ((rc_config & TIC_PIN_FUNC_MASK) == TIC_PIN_FUNC_SERIAL)
    {
      rc_config = (rc_config & ~TIC_PIN_FUNC_MASK) | TIC_PIN_FUNC_DEFAULT;
      tic_sprintf(warnings,
        "Warning: The RC pin cannot be a serial pin "
        "so its function will be changed to the default.\n");
    }

    if (rc_config >> TIC_PIN_ANALOG & 1)
    {
      rc_config &= ~(1 << TIC_PIN_ANALOG);
      tic_sprintf(warnings,
        "Warning: The RC pin cannot be an analog input "
        "so that feature will be disabled.\n");
    }

    if ((sda_config & TIC_PIN_FUNC_MASK) == TIC_PIN_FUNC_RC)
    {
      sda_config = (sda_config & ~TIC_PIN_FUNC_MASK) | TIC_PIN_FUNC_DEFAULT;
      tic_sprintf(warnings,
        "Warning: The SDA pin cannot be used as an RC input "
        "so its function will be changed to the default.\n");
    }

    if ((scl_config & TIC_PIN_FUNC_MASK) == TIC_PIN_FUNC_RC)
    {
      scl_config = (scl_config & ~TIC_PIN_FUNC_MASK) | TIC_PIN_FUNC_DEFAULT;
      tic_sprintf(warnings,
        "Warning: The SCL pin cannot be used as an RC input "
        "so its function will be changed to the default.\n");
    }

    if ((tx_config & TIC_PIN_FUNC_MASK) == TIC_PIN_FUNC_RC)
    {
      tx_config = (tx_config & ~TIC_PIN_FUNC_MASK) | TIC_PIN_FUNC_DEFAULT;
      tic_sprintf(warnings,
        "Warning: The TX pin cannot be used as an RC input "
        "so its function will be changed to the default.\n");
    }

    if ((rx_config & TIC_PIN_FUNC_MASK) == TIC_PIN_FUNC_RC)
    {
      rx_config = (rx_config & ~TIC_PIN_FUNC_MASK) | TIC_PIN_FUNC_DEFAULT;
      tic_sprintf(warnings,
        "Warning: The RX pin cannot be used as an RC input "
        "so its function will be changed to the default.\n");
    }

    if ((scl_config & TIC_PIN_FUNC_MASK) == TIC_PIN_FUNC_ENCODER)
    {
      scl_config = (scl_config & ~TIC_PIN_FUNC_MASK) | TIC_PIN_FUNC_DEFAULT;
      tic_sprintf(warnings,
        "Warning: The SCL pin cannot be used as an encoder input "
        "so its function will be changed to the default.\n");
    }

    if ((sda_config & TIC_PIN_FUNC_MASK) == TIC_PIN_FUNC_ENCODER)
    {
      sda_config = (sda_config & ~TIC_PIN_FUNC_MASK) | TIC_PIN_FUNC_DEFAULT;
      tic_sprintf(warnings,
        "Warning: The SDA pin cannot be used as an encoder input "
        "so its function will be changed to the default.\n");
    }

    if ((rc_config & TIC_PIN_FUNC_MASK) == TIC_PIN_FUNC_ENCODER)
    {
      rc_config = (rc_config & ~TIC_PIN_FUNC_MASK) | TIC_PIN_FUNC_DEFAULT;
      tic_sprintf(warnings,
        "Warning: The RC pin cannot be used as an encoder input "
        "so its function will be changed to the default.\n");
    }

    // If one of the SCL/SDA pins is configured for I2C, make sure the other one
    // is configured that way too.
    bool analog_control = control_mode == TIC_CONTROL_MODE_ANALOG_POSITION ||
      control_mode == TIC_CONTROL_MODE_ANALOG_SPEED;
    bool scl_is_i2c = ((scl_config & TIC_PIN_FUNC_MASK) == TIC_PIN_FUNC_DEFAULT
      && !analog_control) ||
      (scl_config & TIC_PIN_FUNC_MASK) == TIC_PIN_FUNC_SERIAL;
    bool sda_is_i2c = ((sda_config & TIC_PIN_FUNC_MASK) == TIC_PIN_FUNC_DEFAULT
      && !analog_control) ||
      (sda_config & TIC_PIN_FUNC_MASK) == TIC_PIN_FUNC_SERIAL;
    if (sda_is_i2c != scl_is_i2c)
    {
      scl_config = (scl_config & ~TIC_PIN_FUNC_MASK) | TIC_PIN_FUNC_DEFAULT;
      sda_config = (sda_config & ~TIC_PIN_FUNC_MASK) | TIC_PIN_FUNC_DEFAULT;
      if (sda_is_i2c)
      {
        tic_sprintf(warnings,
          "Warning: The SCL pin must be used for I2C if the SDA pin is "
          "so the SCL and SDA pin functions will be changed to the default.\n");
      }
      else
      {
        tic_sprintf(warnings,
          "Warning: The SDA pin must be used for I2C if the SCL pin is "
          "so the SCL and SDA pin functions will be changed to the default.\n");
      }
    }

    // TODO: force the pull-up bool to be true for RX, TX, RC pins?

    // TODO: only SCL can be POT_POWER and it can't be any other non-default function
    // if the control mode is analog.

    // TODO: if the control mode is analog, SDA must be *input* (general is not allowed)

    tic_settings_scl_config_set(settings, scl_config);
    tic_settings_sda_config_set(settings, sda_config);
    tic_settings_tx_config_set(settings, tx_config);
    tic_settings_rx_config_set(settings, rx_config);
    tic_settings_rc_config_set(settings, rc_config);
  }
}

tic_error * tic_settings_fix(tic_settings * settings, char ** warnings)
{
  if (warnings) { *warnings = NULL; }

  if (settings == NULL)
  {
    return tic_error_create("Tic settings pointer is null.");
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

  #ifndef NDEBUG
  {
    // In debug builds, assert that this function is idempotent.
    tic_string str2;
    tic_string_setup(&str2);
    tic_settings_fix_core(settings, &str2);
    assert(str2.data != NULL && str2.data[0] == 0);
    tic_string_free(str2.data);
  }
  #endif

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

void tic_settings_input_averaging_enabled_set(tic_settings * settings,
  bool input_averaging_enabled)
{
  if (!settings) { return; }
  settings->input_averaging_enabled = input_averaging_enabled;
}

bool tic_settings_input_averaging_enabled_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->input_averaging_enabled;
}

void tic_settings_input_hysteresis_set(tic_settings * settings, uint16_t input_hysteresis)
{
  if (!settings) { return; }
  settings->input_hysteresis = input_hysteresis;
}

uint16_t tic_settings_input_hysteresis_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->input_hysteresis;
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
  uint32_t encoder_prescaler)
{
  if (!settings) { return; }
  settings->encoder_prescaler = encoder_prescaler;
}

uint32_t tic_settings_encoder_prescaler_get(const tic_settings * settings)
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

void tic_settings_encoder_unlimited_set(tic_settings * settings,
  bool encoder_unlimited)
{
  if (!settings) { return; }
  settings->encoder_unlimited = encoder_unlimited;
}

bool tic_settings_encoder_unlimited_get(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->encoder_unlimited;
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

void tic_settings_step_mode_set(tic_settings * settings, uint8_t step_mode)
{
  if (!settings) { return; }
  settings->step_mode = step_mode;
}

uint8_t tic_settings_step_mode_get(const tic_settings * settings)
{
  if (!settings) { return 1; }
  return settings->step_mode;
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
