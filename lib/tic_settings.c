#include "tic_internal.h"

struct tic_settings
{
  uint32_t product;

  uint8_t control_mode;
  bool never_sleep;
  bool disable_safe_start;
  bool ignore_err_line_high;
  bool auto_clear_driver_error;
  uint8_t soft_error_response;
  int32_t soft_error_position;
  uint32_t serial_baud_rate;
  uint8_t serial_device_number;
  uint16_t command_timeout;
  bool serial_crc_enabled;
  uint16_t low_vin_timeout;
  uint16_t low_vin_shutoff_voltage;
  uint16_t low_vin_startup_voltage;
  uint16_t high_vin_shutoff_voltage;
  int16_t vin_calibration;
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
  struct {
    uint8_t func;
    bool pullup;
    bool analog;
    bool polarity;
  } pin_settings[TIC_CONTROL_PIN_COUNT];
  uint32_t current_limit;
  int32_t current_limit_during_error;
  uint8_t step_mode;
  uint8_t decay_mode;
  uint32_t starting_speed;
  uint32_t speed_max;
  uint32_t decel_max;
  uint32_t accel_max;
  bool invert_motor_direction;
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

  tic_settings_set_product(settings, product);

  tic_settings_set_auto_clear_driver_error(settings, true);
  tic_settings_set_soft_error_response(settings, TIC_RESPONSE_DECEL_TO_HOLD);
  tic_settings_set_serial_baud_rate(settings, 9600);
  tic_settings_set_serial_device_number(settings, 14);
  tic_settings_set_command_timeout(settings, 1000);
  tic_settings_set_low_vin_timeout(settings, 250);
  tic_settings_set_low_vin_shutoff_voltage(settings, 6000);
  tic_settings_set_low_vin_startup_voltage(settings, 6500);
  tic_settings_set_high_vin_shutoff_voltage(settings, 35000);
  tic_settings_set_rc_max_pulse_period(settings, 100);
  tic_settings_set_rc_bad_signal_timeout(settings, 500);
  tic_settings_set_rc_consecutive_good_pulses(settings, 2);
  tic_settings_set_input_averaging_enabled(settings, true);
  tic_settings_set_input_error_max(settings, 4095);
  tic_settings_set_input_neutral_min(settings, 2015);
  tic_settings_set_input_neutral_max(settings, 2080);
  tic_settings_set_input_max(settings, 4095);
  tic_settings_set_output_min(settings, -200);
  tic_settings_set_output_max(settings, 200);
  tic_settings_set_encoder_prescaler(settings, 1);
  tic_settings_set_encoder_postscaler(settings, 1);
  tic_settings_set_current_limit(settings, 192);
  tic_settings_set_current_limit_during_error(settings, -1);
  tic_settings_set_speed_max(settings, 2000000);
  tic_settings_set_accel_max(settings, 40000);
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

uint32_t tic_settings_achievable_serial_baud_rate(const tic_settings * settings,
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

static void tic_settings_fix_core(tic_settings * settings, tic_string * warnings)
{
  // TODO: fix enum values to be valid
  // TODO: enforce -500 to 500 range for vin_calibration

  uint8_t control_mode = tic_settings_get_control_mode(settings);

  bool speed_control_mode = false;

  switch (control_mode)
  {
  case TIC_CONTROL_MODE_RC_SPEED:
  case TIC_CONTROL_MODE_ANALOG_SPEED:
  case TIC_CONTROL_MODE_ENCODER_SPEED:
    speed_control_mode = true;
    break;
  }

  {
    uint8_t response = tic_settings_get_soft_error_response(settings);
    switch (response)
    {
    case TIC_RESPONSE_DEENERGIZE:
    case TIC_RESPONSE_HALT_AND_HOLD:
    case TIC_RESPONSE_DECEL_TO_HOLD:
      break;

    case TIC_RESPONSE_GO_TO_POSITION:
      if (speed_control_mode)
      {
        response = TIC_RESPONSE_DEENERGIZE;
        tic_sprintf(warnings,
          "Warning: The soft error response cannot be \"Go to position\" in a "
          "speed control mode, so it will be changed to \"De-energize\".\n");
      }
      break;

    default:
      break;
    }
    tic_settings_set_soft_error_response(settings, response);
  }

  {
    uint32_t baud = tic_settings_get_serial_baud_rate(settings);
    if (baud < TIC_MIN_ALLOWED_BAUD_RATE)
    {
      baud = TIC_MIN_ALLOWED_BAUD_RATE;
      tic_sprintf(warnings,
        "Warning: The serial baud rate is too low "
        "so it will be changed to %u.\n", baud);
    }
    if (baud > TIC_MAX_ALLOWED_BAUD_RATE)
    {
      baud = TIC_MAX_ALLOWED_BAUD_RATE;
      tic_sprintf(warnings,
        "Warning: The serial baud rate is too high "
        "so it will be changed to %u.\n",
        baud);
    }

    baud = tic_settings_achievable_serial_baud_rate(settings, baud);
    tic_settings_set_serial_baud_rate(settings, baud);
  }

  {
    uint8_t device_number = tic_settings_get_serial_device_number(settings);
    if (device_number > 127)
    {
      device_number = 127;
      tic_sprintf(warnings,
        "Warning: The serial device number is too high "
        "so it will be changed to 127.\n");
    }
    tic_settings_set_serial_device_number(settings, device_number);
  }

  {
    uint16_t command_timeout = tic_settings_get_command_timeout(settings);
    if (command_timeout > TIC_MAX_ALLOWED_COMMAND_TIMEOUT)
    {
      command_timeout = TIC_MAX_ALLOWED_COMMAND_TIMEOUT;
      tic_sprintf(warnings,
        "Warning: The command timeout is too high "
        "so it will be changed to %u ms.\n", command_timeout);
    }
    tic_settings_set_command_timeout(settings, command_timeout);
  }

  {
    uint16_t low_shutoff = tic_settings_get_low_vin_shutoff_voltage(settings);
    uint16_t low_startup = tic_settings_get_low_vin_startup_voltage(settings);
    uint16_t high_shutoff = tic_settings_get_high_vin_shutoff_voltage(settings);

    // Move low_shutoff down a little bit to prevent overflows below.
    if (low_shutoff > 64000)
    {
      low_shutoff = 64000;
      tic_sprintf(warnings,
        "Warning: The low VIN shutoff voltage will be changed to %u mV.\n",
        low_shutoff);
    }

    if (low_startup < low_shutoff)
    {
      low_startup = low_shutoff + 500;
      tic_sprintf(warnings,
        "Warning: The low VIN startup voltage will be changed to %u mV.\n",
        low_startup);
    }

    if (high_shutoff < low_startup)
    {
      high_shutoff = low_startup + 500;
      tic_sprintf(warnings,
        "Warning: The high VIN shutoff voltage will be changed to %u mV.\n",
        high_shutoff);
    }

    tic_settings_set_low_vin_shutoff_voltage(settings, low_shutoff);
    tic_settings_set_low_vin_startup_voltage(settings, low_startup);
    tic_settings_set_high_vin_shutoff_voltage(settings, high_shutoff);
  }

  {
    uint16_t min = tic_settings_get_input_min(settings);
    uint16_t neutral_min = tic_settings_get_input_neutral_min(settings);
    uint16_t neutral_max = tic_settings_get_input_neutral_max(settings);
    uint16_t max = tic_settings_get_input_max(settings);

    if (min > neutral_min || neutral_min > neutral_max || neutral_max > max)
    {
      min = 0;
      neutral_min = 2015;
      neutral_max = 2080;
      max = 4095;

      tic_sprintf(warnings,
        "Warning: The input scaling values are out of order "
        "so they will be reset to their default values.\n");
    }

    if (min > 4095)
    {
      tic_sprintf(warnings,
        "Warning: The input minimum is too high "
        "so it will be lowered to 4095.\n");
      min = 4095;
    }
    if (neutral_min > 4095)
    {
      tic_sprintf(warnings,
        "Warning: The input neutral min is too high "
        "so it will be lowered to 4095.\n");
      neutral_min = 4095;
    }
    if (neutral_max > 4095)
    {
      tic_sprintf(warnings,
        "Warning: The input neutral max is too high "
        "so it will be lowered to 4095.\n");
      neutral_max = 4095;
    }
    if (max > 4095)
    {
      tic_sprintf(warnings,
        "Warning: The input maximum is too high "
        "so it will be lowered to 4095.\n");
      max = 4095;
    }

    tic_settings_set_input_min(settings, min);
    tic_settings_set_input_neutral_min(settings, neutral_min);
    tic_settings_set_input_neutral_max(settings, neutral_max);
    tic_settings_set_input_max(settings, max);
  }

  {
    int32_t output_min = tic_settings_get_output_min(settings);
    if (output_min > 0)
    {
      output_min = 0;
      tic_sprintf(warnings,
        "Warning: The scaling output minimum is above 0 "
        "so it will be lowered to 0.\n");
    }
    tic_settings_set_output_min(settings, output_min);
  }

  {
    int32_t output_max = tic_settings_get_output_max(settings);
    if (output_max < 0)
    {
      output_max = 0;
      tic_sprintf(warnings,
        "Warning: The scaling output maximum is below 0 "
        "so it will be raised to 0.\n");
    }
    tic_settings_set_output_max(settings, output_max);
  }

  {
    uint32_t prescaler = tic_settings_get_encoder_prescaler(settings);

    if (prescaler > TIC_MAX_ALLOWED_ENCODER_PRESCALER)
    {
      prescaler = TIC_MAX_ALLOWED_ENCODER_PRESCALER;
      tic_sprintf(warnings,
        "Warning: The encoder prescaler is too high "
        "so it will be lowered to %u.\n", prescaler);
    }

    if (prescaler < 1)
    {
      prescaler = 1;
      tic_sprintf(warnings,
        "Warning: The encoder prescaler is zero "
        "so it will be changed to 1.\n");
    }

    tic_settings_set_encoder_prescaler(settings, prescaler);
  }

  {
    uint32_t postscaler = tic_settings_get_encoder_postscaler(settings);

    if (postscaler > TIC_MAX_ALLOWED_ENCODER_POSTSCALER)
    {
      postscaler = TIC_MAX_ALLOWED_ENCODER_POSTSCALER;
      tic_sprintf(warnings,
        "Warning: The encoder postscaler is too high "
        "so it will be lowered to %u.\n", postscaler);
    }

    if (postscaler < 1)
    {
      postscaler = 1;
      tic_sprintf(warnings,
        "Warning: The encoder postscaler is zero "
        "so it will be changed to 1.\n");
    }

    tic_settings_set_encoder_postscaler(settings, postscaler);
  }

  {
    uint32_t current = tic_settings_get_current_limit(settings);

    if (current > TIC_MAX_ALLOWED_CURRENT)
    {
      current = TIC_MAX_ALLOWED_CURRENT;
      tic_sprintf(warnings,
        "Warning: The current limit is too high "
        "so it will be lowered to %u mA.\n", current);
    }

    current = tic_settings_achievable_current_limit(settings, current);
    tic_settings_set_current_limit(settings, current);

    int32_t current_during_error =
      tic_settings_get_current_limit_during_error(settings);

    if (current_during_error > (int32_t)current)
    {
      current_during_error = -1;
      tic_sprintf(warnings,
        "Warning: The current limit during error is higher than "
        "the default current limit so it will be changed to be the same.\n");
    }

    if (current_during_error < -1)
    {
      current_during_error = -1;
      tic_sprintf(warnings,
        "Warning: The current limit during error is an invalid negative number "
        "so it will be changed to be the same as the default current limit.\n");
    }

    if (current_during_error >= 0)
    {
      current_during_error = tic_settings_achievable_current_limit(
        settings, current_during_error);
    }
    tic_settings_set_current_limit_during_error(settings, current_during_error);
  }

  {
    uint8_t decay_mode = tic_settings_get_decay_mode(settings);
    if (decay_mode != TIC_DECAY_MODE_MIXED &&
      decay_mode != TIC_DECAY_MODE_SLOW &&
      decay_mode != TIC_DECAY_MODE_FAST)
    {
      decay_mode = TIC_DECAY_MODE_MIXED;
      tic_sprintf(warnings,
        "Warning: The decay mode is invalid "
        "so it will be changed to mixed.\n");
    }
  }

  {
    uint8_t mode = tic_settings_get_step_mode(settings);
    if (mode != TIC_STEP_MODE_MICROSTEP1 &&
      mode != TIC_STEP_MODE_MICROSTEP2 &&
      mode != TIC_STEP_MODE_MICROSTEP4 &&
      mode != TIC_STEP_MODE_MICROSTEP8 &&
      mode != TIC_STEP_MODE_MICROSTEP16 &&
      mode != TIC_STEP_MODE_MICROSTEP32)
    {
      mode = TIC_STEP_MODE_MICROSTEP1;
      tic_sprintf(warnings,
        "Warning: The step mode is invalid "
        "so it will be changed to 1 (full step).\n");
    }

    tic_settings_set_step_mode(settings, mode);
  }

  {
    uint32_t speed_max = tic_settings_get_speed_max(settings);
    uint32_t starting_speed = tic_settings_get_starting_speed(settings);

    if (speed_max > TIC_MAX_ALLOWED_SPEED)
    {
      speed_max = TIC_MAX_ALLOWED_SPEED;
      uint32_t speed_max_khz = speed_max / TIC_SPEED_UNITS_PER_HZ / 1000;
      tic_sprintf(warnings,
        "Warning: The maximum speed is too high "
        "so it will be lowered to %u (%u kHz).\n",
        speed_max, speed_max_khz);
    }

    if (starting_speed > speed_max)
    {
      starting_speed = speed_max;
      tic_sprintf(warnings,
        "Warning: The starting speed is greater than the maximum speed "
        "so it will be lowered to %u.\n", starting_speed);
    }

    tic_settings_set_speed_max(settings, speed_max);
    tic_settings_set_starting_speed(settings, starting_speed);
  }

  {
    uint32_t decel_max = tic_settings_get_decel_max(settings);

    if (decel_max > TIC_MAX_ALLOWED_ACCEL)
    {
      decel_max = TIC_MAX_ALLOWED_ACCEL;
      tic_sprintf(warnings,
        "Warning: The maximum deceleration is too high "
        "so it will be lowered to %u.\n", decel_max);
    }

    if (decel_max != 0 && decel_max < TIC_MIN_ALLOWED_ACCEL)
    {
      decel_max = TIC_MIN_ALLOWED_ACCEL;
      tic_sprintf(warnings,
        "Warning: The maximum deceleration is too low "
        "so it will be raised to %u.\n", decel_max);
    }

    tic_settings_set_decel_max(settings, decel_max);
  }

  {
    uint32_t accel_max = tic_settings_get_accel_max(settings);

    if (accel_max > TIC_MAX_ALLOWED_ACCEL)
    {
      accel_max = TIC_MAX_ALLOWED_ACCEL;
      tic_sprintf(warnings,
        "Warning: The maximum acceleration is too high "
        "so it will be lowered to %u.\n", accel_max);
    }

    if (accel_max < TIC_MIN_ALLOWED_ACCEL)
    {
      accel_max = TIC_MIN_ALLOWED_ACCEL;
      tic_sprintf(warnings,
        "Warning: The maximum acceleration is too low "
        "so it will be raised to %u.\n", accel_max);
    }

    tic_settings_set_accel_max(settings, accel_max);
  }

  {
    uint8_t scl_func = tic_settings_get_pin_func(settings, TIC_PIN_NUM_SCL);
    uint8_t sda_func = tic_settings_get_pin_func(settings, TIC_PIN_NUM_SDA);
    uint8_t tx_func = tic_settings_get_pin_func(settings, TIC_PIN_NUM_TX);
    uint8_t rx_func = tic_settings_get_pin_func(settings, TIC_PIN_NUM_RX);
    uint8_t rc_func = tic_settings_get_pin_func(settings, TIC_PIN_NUM_RC);
    bool rc_analog = tic_settings_get_pin_analog(settings, TIC_PIN_NUM_RC);

    // First, we make sure the pins are configured to provide the primary
    // input that will be used to control the motor.

    switch (control_mode)
    {
    case TIC_CONTROL_MODE_ANALOG_POSITION:
    case TIC_CONTROL_MODE_ANALOG_SPEED:
      if (sda_func != TIC_PIN_FUNC_DEFAULT &&
        sda_func != TIC_PIN_FUNC_USER_INPUT)
      {
        sda_func = TIC_PIN_FUNC_DEFAULT;
        tic_sprintf(warnings,
          "Warning: The SDA pin must be used as an analog input "
          "so its function will be changed to the default.\n");
      }
      break;
    case TIC_CONTROL_MODE_RC_POSITION:
    case TIC_CONTROL_MODE_RC_SPEED:
      if (rc_func != TIC_PIN_FUNC_DEFAULT &&
        rc_func != TIC_PIN_FUNC_RC)
      {
        rc_func = TIC_PIN_FUNC_DEFAULT;
        tic_sprintf(warnings,
          "Warning: The RC pin must be used as an RC input "
          "so its function will be changed to the default.\n");
      }
      break;
    case TIC_CONTROL_MODE_ENCODER_POSITION:
    case TIC_CONTROL_MODE_ENCODER_SPEED:
      if (tx_func != TIC_PIN_FUNC_DEFAULT &&
        tx_func != TIC_PIN_FUNC_ENCODER)
      {
        tx_func = TIC_PIN_FUNC_DEFAULT;
        tic_sprintf(warnings,
          "Warning: The TX pin must be used as an encoder input "
          "so its function will be changed to the default.\n");
      }
      if (rx_func != TIC_PIN_FUNC_DEFAULT &&
        rx_func != TIC_PIN_FUNC_ENCODER)
      {
        rx_func = TIC_PIN_FUNC_DEFAULT;
        tic_sprintf(warnings,
          "Warning: The RX pin must be used as an encoder input "
          "so its function will be changed to the default.\n");
      }
      break;
    }

    // In this section, we make sure no pin is configured to do something that
    // it cannot do.

    // TODO: enforce that only SCL can be a potentiometer power pin

    if (rc_func == TIC_PIN_FUNC_USER_IO)
    {
      rc_func = TIC_PIN_FUNC_DEFAULT;
      tic_sprintf(warnings,
        "Warning: The RC pin cannot be a user I/O pin "
        "so its function will be changed to the default.\n");
    }

    if (rc_func == TIC_PIN_FUNC_SERIAL)
    {
      rc_func = TIC_PIN_FUNC_DEFAULT;
      tic_sprintf(warnings,
        "Warning: The RC pin cannot be a serial pin "
        "so its function will be changed to the default.\n");
    }

    if (rc_analog)
    {
      rc_analog = false;
      tic_sprintf(warnings,
        "Warning: The RC pin cannot be an analog input "
        "so that feature will be disabled.\n");
    }

    if (sda_func == TIC_PIN_FUNC_RC)
    {
      sda_func = TIC_PIN_FUNC_DEFAULT;
      tic_sprintf(warnings,
        "Warning: The SDA pin cannot be used as an RC input "
        "so its function will be changed to the default.\n");
    }

    if (scl_func == TIC_PIN_FUNC_RC)
    {
      scl_func = TIC_PIN_FUNC_DEFAULT;
      tic_sprintf(warnings,
        "Warning: The SCL pin cannot be used as an RC input "
        "so its function will be changed to the default.\n");
    }

    if (tx_func == TIC_PIN_FUNC_RC)
    {
      tx_func = TIC_PIN_FUNC_DEFAULT;
      tic_sprintf(warnings,
        "Warning: The TX pin cannot be used as an RC input "
        "so its function will be changed to the default.\n");
    }

    if (rx_func == TIC_PIN_FUNC_RC)
    {
      rx_func = TIC_PIN_FUNC_DEFAULT;
      tic_sprintf(warnings,
        "Warning: The RX pin cannot be used as an RC input "
        "so its function will be changed to the default.\n");
    }

    if (scl_func == TIC_PIN_FUNC_ENCODER)
    {
      scl_func = TIC_PIN_FUNC_DEFAULT;
      tic_sprintf(warnings,
        "Warning: The SCL pin cannot be used as an encoder input "
        "so its function will be changed to the default.\n");
    }

    if (sda_func == TIC_PIN_FUNC_ENCODER)
    {
      sda_func = TIC_PIN_FUNC_DEFAULT;
      tic_sprintf(warnings,
        "Warning: The SDA pin cannot be used as an encoder input "
        "so its function will be changed to the default.\n");
    }

    if (rc_func == TIC_PIN_FUNC_ENCODER)
    {
      rc_func = TIC_PIN_FUNC_DEFAULT;
      tic_sprintf(warnings,
        "Warning: The RC pin cannot be used as an encoder input "
        "so its function will be changed to the default.\n");
    }

    // If one of the SCL/SDA pins is configured for I2C, make sure the other one
    // is configured that way too.
    bool analog_control = control_mode == TIC_CONTROL_MODE_ANALOG_POSITION ||
      control_mode == TIC_CONTROL_MODE_ANALOG_SPEED;
    bool scl_is_i2c = (scl_func == TIC_PIN_FUNC_DEFAULT && !analog_control) ||
      (scl_func == TIC_PIN_FUNC_SERIAL);
    bool sda_is_i2c = (sda_func == TIC_PIN_FUNC_DEFAULT && !analog_control) ||
      (sda_func == TIC_PIN_FUNC_SERIAL);
    if (sda_is_i2c != scl_is_i2c)
    {
      scl_func = TIC_PIN_FUNC_DEFAULT;
      sda_func = TIC_PIN_FUNC_DEFAULT;
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

    // TODO: only SCL can be POT_POWER and it can't be any other non-default function
    // if the control mode is analog.

    // TODO: if the control mode is analog, SDA must be *input* (general is not allowed)

    tic_settings_set_pin_func(settings, TIC_PIN_NUM_SCL, scl_func);
    tic_settings_set_pin_func(settings, TIC_PIN_NUM_SDA, sda_func);
    tic_settings_set_pin_func(settings, TIC_PIN_NUM_TX, tx_func);
    tic_settings_set_pin_func(settings, TIC_PIN_NUM_RX, rx_func);
    tic_settings_set_pin_func(settings, TIC_PIN_NUM_RC, rc_func);
    tic_settings_set_pin_analog(settings, TIC_PIN_NUM_RC, rc_analog);
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

void tic_settings_set_product(tic_settings * settings, uint8_t product)
{
  if (!settings) { return; }
  settings->product = product;
}

uint8_t tic_settings_get_product(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->product;
}

void tic_settings_set_control_mode(tic_settings * settings,
  uint8_t control_mode)
{
  if (!settings) { return; }
  settings->control_mode = control_mode;
}

uint8_t tic_settings_get_control_mode(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->control_mode;
}

void tic_settings_set_never_sleep(tic_settings * settings, bool never_sleep)
{
  if (!settings) { return; }
  settings->never_sleep = never_sleep;
}

bool tic_settings_get_never_sleep(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->never_sleep;
}

void tic_settings_set_disable_safe_start(tic_settings * settings,
  bool disable_safe_start)
{
  if (!settings) { return; }
  settings->disable_safe_start = disable_safe_start;
}

bool tic_settings_get_disable_safe_start(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->disable_safe_start;
}

void tic_settings_set_ignore_err_line_high(tic_settings * settings,
  bool ignore_err_line_high)
{
  if (!settings) { return; }
  settings->ignore_err_line_high = ignore_err_line_high;
}

bool tic_settings_get_ignore_err_line_high(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->ignore_err_line_high;
}

void tic_settings_set_auto_clear_driver_error(tic_settings * settings,
  bool auto_clear_driver_error)
{
  if (!settings) { return; }
  settings->auto_clear_driver_error = auto_clear_driver_error;
}

bool tic_settings_get_auto_clear_driver_error(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->auto_clear_driver_error;
}

void tic_settings_set_soft_error_response(tic_settings * settings,
  uint8_t soft_error_response)
{
  if (!settings) { return; }
  settings->soft_error_response = soft_error_response;
}

uint8_t tic_settings_get_soft_error_response(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->soft_error_response;
}

void tic_settings_set_soft_error_position(tic_settings * settings,
  int32_t soft_error_position)
{
  if (!settings) { return; }
  settings->soft_error_position = soft_error_position;
}

int32_t tic_settings_get_soft_error_position(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->soft_error_position;
}

void tic_settings_set_serial_baud_rate(tic_settings * settings, uint32_t serial_baud_rate)
{
  if (!settings) { return; }
  settings->serial_baud_rate = serial_baud_rate;
}

uint32_t tic_settings_get_serial_baud_rate(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->serial_baud_rate;
}

void tic_settings_set_serial_device_number(tic_settings * settings,
  uint8_t serial_device_number)
{
  if (!settings) { return; }
  settings->serial_device_number = serial_device_number;
}

uint8_t tic_settings_get_serial_device_number(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->serial_device_number;
}

void tic_settings_set_command_timeout(tic_settings * settings,
  uint16_t command_timeout)
{
  if (!settings) { return; }
  settings->command_timeout = command_timeout;
}

uint16_t tic_settings_get_command_timeout(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->command_timeout;
}

void tic_settings_set_serial_crc_enabled(tic_settings * settings,
  bool serial_crc_enabled)
{
  if (!settings) { return; }
  settings->serial_crc_enabled = serial_crc_enabled;
}

bool tic_settings_get_serial_crc_enabled(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->serial_crc_enabled;
}

void tic_settings_set_low_vin_timeout(tic_settings * settings,
  uint16_t low_vin_timeout)
{
  if (!settings) { return; }
  settings->low_vin_timeout = low_vin_timeout;
}

uint16_t tic_settings_get_low_vin_timeout(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->low_vin_timeout;
}

void tic_settings_set_low_vin_shutoff_voltage(tic_settings * settings,
  uint16_t low_vin_shutoff_voltage)
{
  if (!settings) { return; }
  settings->low_vin_shutoff_voltage = low_vin_shutoff_voltage;
}

uint16_t tic_settings_get_low_vin_shutoff_voltage(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->low_vin_shutoff_voltage;
}

void tic_settings_set_low_vin_startup_voltage(tic_settings * settings,
  uint16_t low_vin_startup_voltage)
{
  if (!settings) { return; }
  settings->low_vin_startup_voltage = low_vin_startup_voltage;
}

uint16_t tic_settings_get_low_vin_startup_voltage(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->low_vin_startup_voltage;
}

void tic_settings_set_high_vin_shutoff_voltage(tic_settings * settings,
  uint16_t high_vin_shutoff_voltage)
{
  if (!settings) { return; }
  settings->high_vin_shutoff_voltage = high_vin_shutoff_voltage;
}

uint16_t tic_settings_get_high_vin_shutoff_voltage(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->high_vin_shutoff_voltage;
}

void tic_settings_set_vin_calibration(tic_settings * settings,
  uint16_t vin_calibration)
{
  if (!settings) { return; }
  settings->vin_calibration = vin_calibration;
}

uint16_t tic_settings_get_vin_calibration(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->vin_calibration;
}

void tic_settings_set_rc_max_pulse_period(tic_settings * settings,
  uint16_t rc_max_pulse_period)
{
  if (!settings) { return; }
  settings->rc_max_pulse_period = rc_max_pulse_period;
}

uint16_t tic_settings_get_rc_max_pulse_period(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->rc_max_pulse_period;
}

void tic_settings_set_rc_bad_signal_timeout(tic_settings * settings,
  uint16_t rc_bad_signal_timeout)
{
  if (!settings) { return; }
  settings->rc_bad_signal_timeout = rc_bad_signal_timeout;
}

uint16_t tic_settings_get_rc_bad_signal_timeout(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->rc_bad_signal_timeout;
}

void tic_settings_set_rc_consecutive_good_pulses(tic_settings * settings,
  uint8_t rc_consecutive_good_pulses)
{
  if (!settings) { return; }
  settings->rc_consecutive_good_pulses = rc_consecutive_good_pulses;
}

uint8_t tic_settings_get_rc_consecutive_good_pulses(
  const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->rc_consecutive_good_pulses;
}

void tic_settings_set_input_error_min(tic_settings * settings,
  uint16_t input_error_min)
{
  if (!settings) { return; }
  settings->input_error_min = input_error_min;
}

uint16_t tic_settings_get_input_error_min(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->input_error_min;
}

void tic_settings_set_input_error_max(tic_settings * settings,
  uint16_t input_error_max)
{
  if (!settings) { return; }
  settings->input_error_max = input_error_max;
}

uint16_t tic_settings_get_input_error_max(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->input_error_max;
}

void tic_settings_set_input_averaging_enabled(tic_settings * settings,
  bool input_averaging_enabled)
{
  if (!settings) { return; }
  settings->input_averaging_enabled = input_averaging_enabled;
}

bool tic_settings_get_input_averaging_enabled(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->input_averaging_enabled;
}

void tic_settings_set_input_hysteresis(tic_settings * settings, uint16_t input_hysteresis)
{
  if (!settings) { return; }
  settings->input_hysteresis = input_hysteresis;
}

uint16_t tic_settings_get_input_hysteresis(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->input_hysteresis;
}

void tic_settings_set_input_scaling_degree(tic_settings * settings,
  uint8_t input_scaling_degree)
{
  if (!settings) { return; }
  settings->input_scaling_degree = input_scaling_degree;
}

uint8_t tic_settings_get_input_scaling_degree(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->input_scaling_degree;
}

void tic_settings_set_input_invert(tic_settings * settings, bool invert)
{
  if (!settings) { return; }
  settings->input_invert = invert;
}

bool tic_settings_get_input_invert(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->input_invert;
}

void tic_settings_set_input_min(tic_settings * settings, uint16_t input_min)
{
  if (!settings) { return; }
  settings->input_min = input_min;
}

uint16_t tic_settings_get_input_min(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->input_min;
}

void tic_settings_set_input_neutral_min(tic_settings * settings,
  uint16_t input_neutral_min)
{
  if (!settings) { return; }
  settings->input_neutral_min = input_neutral_min;
}

uint16_t tic_settings_get_input_neutral_min(const tic_settings * settings)
{
  if (!settings){ return 0; }
  return settings->input_neutral_min;
}

void tic_settings_set_input_neutral_max(tic_settings * settings,
  uint16_t input_neutral_max)
{
  if (!settings) { return; }
  settings->input_neutral_max = input_neutral_max;
}

uint16_t tic_settings_get_input_neutral_max(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->input_neutral_max;
}

void tic_settings_set_input_max(tic_settings * settings, uint16_t input_max)
{
  if (!settings) { return; }
  settings->input_max = input_max;
}

uint16_t tic_settings_get_input_max(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->input_max;
}

void tic_settings_set_output_min(tic_settings * settings, int32_t output_min)
{
  if (!settings) { return; }
  settings->output_min = output_min;
}

int32_t tic_settings_get_output_min(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->output_min;
}

void tic_settings_set_output_max(tic_settings * settings, int32_t output_max)
{
  if (!settings) { return; }
  settings->output_max = output_max;
}

int32_t tic_settings_get_output_max(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->output_max;
}

void tic_settings_set_encoder_prescaler(tic_settings * settings,
  uint32_t encoder_prescaler)
{
  if (!settings) { return; }
  settings->encoder_prescaler = encoder_prescaler;
}

uint32_t tic_settings_get_encoder_prescaler(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->encoder_prescaler;
}

void tic_settings_set_encoder_postscaler(tic_settings * settings,
  uint32_t encoder_postscaler)
{
  if (!settings) { return; }
  settings->encoder_postscaler = encoder_postscaler;
}

uint32_t tic_settings_get_encoder_postscaler(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->encoder_postscaler;
}

void tic_settings_set_encoder_unlimited(tic_settings * settings,
  bool encoder_unlimited)
{
  if (!settings) { return; }
  settings->encoder_unlimited = encoder_unlimited;
}

bool tic_settings_get_encoder_unlimited(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->encoder_unlimited;
}

void tic_settings_set_pin_func(tic_settings * settings, uint8_t pin, uint8_t func)
{
  if (!settings || pin >= TIC_CONTROL_PIN_COUNT) { return; }
  settings->pin_settings[pin].func = func;
}

uint8_t tic_settings_get_pin_func(const tic_settings * settings, uint8_t pin)
{
  if (!settings || pin >= TIC_CONTROL_PIN_COUNT) { return 0; }
  return settings->pin_settings[pin].func;
}

void tic_settings_set_pin_pullup(tic_settings * settings, uint8_t pin,
  bool pullup)
{
  if (!settings || pin >= TIC_CONTROL_PIN_COUNT) { return; }
  settings->pin_settings[pin].pullup = pullup;
}

bool tic_settings_get_pin_pullup(const tic_settings * settings, uint8_t pin)
{
  if (!settings || pin >= TIC_CONTROL_PIN_COUNT) { return 0; }
  return settings->pin_settings[pin].pullup;
}

void tic_settings_set_pin_analog(tic_settings * settings, uint8_t pin,
  bool analog)
{
  if (!settings || pin >= TIC_CONTROL_PIN_COUNT) { return; }
  settings->pin_settings[pin].analog = analog;
}

bool tic_settings_get_pin_analog(const tic_settings * settings, uint8_t pin)
{
  if (!settings || pin >= TIC_CONTROL_PIN_COUNT) { return 0; }
  return settings->pin_settings[pin].analog;
}

void tic_settings_set_pin_polarity(tic_settings * settings, uint8_t pin,
  bool polarity)
{
  if (!settings || pin >= TIC_CONTROL_PIN_COUNT) { return; }
  settings->pin_settings[pin].polarity = polarity;
}

bool tic_settings_get_pin_polarity(const tic_settings * settings, uint8_t pin)
{
  if (!settings || pin >= TIC_CONTROL_PIN_COUNT) { return 0; }
  return settings->pin_settings[pin].polarity;
}

void tic_settings_set_current_limit(tic_settings * settings,
  uint32_t current_limit)
{
  if (!settings) { return; }
  settings->current_limit = current_limit;
}

uint32_t tic_settings_get_current_limit(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->current_limit;
}

void tic_settings_set_current_limit_during_error(tic_settings * settings,
  int32_t current_limit)
{
  if (!settings) { return; }
  settings->current_limit_during_error = current_limit;
}

int32_t tic_settings_get_current_limit_during_error(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->current_limit_during_error;
}

void tic_settings_set_step_mode(tic_settings * settings, uint8_t step_mode)
{
  if (!settings) { return; }
  settings->step_mode = step_mode;
}

uint8_t tic_settings_get_step_mode(const tic_settings * settings)
{
  if (!settings) { return 1; }
  return settings->step_mode;
}

void tic_settings_set_decay_mode(tic_settings * settings, uint8_t decay_mode)
{
  if (!settings) { return; }
  settings->decay_mode = decay_mode;
}

uint8_t tic_settings_get_decay_mode(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->decay_mode;
}

void tic_settings_set_starting_speed(tic_settings * settings, uint32_t starting_speed)
{
  if (!settings) { return; }
  settings->starting_speed = starting_speed;
}

uint32_t tic_settings_get_starting_speed(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->starting_speed;
}

void tic_settings_set_speed_max(tic_settings * settings, uint32_t speed_max)
{
  if (!settings) { return; }
  settings->speed_max = speed_max;
}

uint32_t tic_settings_get_speed_max(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->speed_max;
}

void tic_settings_set_decel_max(tic_settings * settings, uint32_t decel_max)
{
  if (!settings) { return; }
  settings->decel_max = decel_max;
}

uint32_t tic_settings_get_decel_max(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->decel_max;
}

void tic_settings_set_accel_max(tic_settings * settings, uint32_t accel_max)
{
  if (!settings) { return; }
  settings->accel_max = accel_max;
}

uint32_t tic_settings_get_accel_max(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->accel_max;
}

void tic_settings_set_invert_motor_direction(tic_settings * settings, bool invert)
{
  if (!settings) { return; }
  settings->invert_motor_direction = invert;
}

bool tic_settings_get_invert_motor_direction(const tic_settings * settings)
{
  if (!settings) { return 0; }
  return settings->invert_motor_direction;
}
