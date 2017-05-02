#include "tic_internal.h"

struct tic_settings
{
  uint32_t tic_model;

  uint8_t control_mode;
  bool never_sleep;
  bool disable_safe_start;
  bool ignore_err_line_high;
  uint16_t serial_baud_rate_generator;
  uint8_t serial_device_number;
  uint8_t i2c_device_address;
  uint16_t command_timeout;
  bool serial_crc_enabled;
  uint16_t low_vin_timeout;
  uint16_t low_vin_shutoff;
  uint16_t high_vin_shutoff;
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
  uint8_t current_limit;
  uint32_t microstepping_mode;
  uint8_t decay_mode;
  uint32_t speed_min;
  uint32_t speed_max;
  uint32_t decel_max;
  uint32_t accel_max;
  int32_t decel_max_during_error;
};

static tic_error * tic_settings_fill_with_defaults(
  tic_settings * settings, uint32_t model)
{
  assert(settings != NULL);

  if (model != TIC_MODEL_T825)
  {
    return tic_error_create("Invalid model specified: %d.\n", model);
  }

  // TODO: finish

  // TODO: make automated test to ensure these defaults match the firmware;
  // probably need to make a hidden feature of the CLI
  return NULL;
}

tic_error * tic_settings_create(tic_settings ** settings, uint32_t model)
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
    error = tic_settings_fill_with_defaults(new_settings, model);
  }

  if (error == NULL)
  {
    *settings = new_settings;
    new_settings = NULL;
  }

  tic_settings_free(new_settings);

  return error;
}

// TODO: tic_settings_copy here

void tic_settings_free(tic_settings * settings)
{
  free(settings);
}

// Attempts to add a string.
TIC_PRINTF(3, 4)
static void warning_add(char ** w, size_t * wlen, const char * format, ...)
{
  assert(format != NULL);

  if (*w == NULL)
  {
    // Either warnings are not needed or there was a memory allocation error
    // earlier.
    return;
  }

  assert(*wlen == strlen(*w));

  va_list ap;
  va_start(ap, format);

  // Determine the total length needed.
  size_t additional_length = 0;
  {
    char x[1];
    va_list ap2;
    va_copy(ap2, ap);
    int result = vsnprintf(x, 0, format, ap2);
    va_end(ap2);
    if (result > 0)
    {
      additional_length = result;
    }
    else
    {
      // This error seems really unlikely to happen.  If it does, we can add a
      // better way to report it.  For now, just set w to NULL so it is treated
      // like a memory allocation problem.
      *w = NULL;
      va_end(ap);
      return;
    }
  }
  size_t newline_length = 1;
  size_t total_length = *wlen + additional_length + newline_length;

  *w = realloc(*w, total_length + 1);
  if (*w == NULL)
  {
    // Failed to allocate memory.
    return;
  }

  // Add the new warning and a newline and null terminator.
  int result = vsnprintf(*w + *wlen, additional_length + 1, format, ap);
  va_end(ap);
  assert((size_t)result == additional_length);
  (*w)[total_length - 1] = '\n';
  (*w)[total_length] = 0;
  *wlen = total_length;

  assert(*w != NULL);
  assert(*wlen == strlen(*w));
}

tic_error * tic_settings_fix(tic_settings * settings, char ** warnings)
{
  if (warnings) { *warnings = NULL; }

  if (settings == NULL)
  {
    return tic_error_create("Tic settings pointer is NULL.");
  }

  // Make a string to store the warnings we accumulate in this function.
  char * w = NULL;
  size_t wlen = 0;

  // We must return an empty string even if there are no warnings, so make an
  // empty string now.  Note: Memory allocation errors are checked at the end.
  if (warnings)
  {
    w = malloc(1);
    if (w != NULL) { *w = 0; }
  }

  warning_add(&w, &wlen, "Warning: TODO: implement tic_settings_fix fully.");
  warning_add(&w, &wlen, "Warning: TODO: implement tic_settings_fix fully!");

  if (warnings && w == NULL)
  {
    // Memory allocation failed at some point and the warning string was freed.
    return &tic_error_no_memory;
  }

  if (warnings)
  {
    *warnings = w;
  }

  return NULL;
}
