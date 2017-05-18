#include "tic_internal.h"

const tic_name tic_bool_names[] =
{
  { "true", 1 },
  { "false", 0 },
  { NULL, 0 },
};

const tic_name tic_product_names[] =
{
  { "T825", TIC_PRODUCT_T825 },
  { NULL, 0 },
};

const tic_name tic_control_mode_names[] =
{
  { "serial", TIC_CONTROL_MODE_SERIAL },
  { "step_dir", TIC_CONTROL_MODE_STEP_DIR },
  { "rc_position", TIC_CONTROL_MODE_RC_POSITION },
  { "rc_speed", TIC_CONTROL_MODE_RC_SPEED },
  { "analog_position", TIC_CONTROL_MODE_ANALOG_POSITION },
  { "analog_speed", TIC_CONTROL_MODE_ANALOG_SPEED },
  { "encoder_position", TIC_CONTROL_MODE_ENCODER_POSITION },
  { "encoder_speed", TIC_CONTROL_MODE_ENCODER_SPEED },
  { NULL, 0 },
};

const tic_name tic_scaling_degree_names[] =
{
  { "linear", TIC_SCALING_DEGREE_LINEAR },
  { "quadratic", TIC_SCALING_DEGREE_QUADRATIC },
  { "cubic", TIC_SCALING_DEGREE_CUBIC },
  { NULL, 0 },
};

const tic_name tic_microstepping_mode_names[] = // TODO: rename table to tic_step_mode_names
{
  { "1", TIC_STEP_MODE_MICROSTEP1 },
  { "2", TIC_STEP_MODE_MICROSTEP2 },
  { "4", TIC_STEP_MODE_MICROSTEP4 },
  { "8", TIC_STEP_MODE_MICROSTEP8 },
  { "16", TIC_STEP_MODE_MICROSTEP16 },
  { "32", TIC_STEP_MODE_MICROSTEP32 },
  { "full", TIC_STEP_MODE_FULL },
  { "half", TIC_STEP_MODE_HALF },
  { NULL, 0 },
};

const tic_name tic_decay_mode_names[] =
{
  { "mixed", TIC_DECAY_MODE_MIXED },
  { "slow", TIC_DECAY_MODE_SLOW },
  { "fast", TIC_DECAY_MODE_FAST },
  { NULL, 0 },
};

const tic_name tic_pin_config_names[] =
{
  { "pullup", 1 << TIC_PIN_PULLUP },
  { "analog", 1 << TIC_PIN_ANALOG },
  { "active_high", 1 << TIC_PIN_ACTIVE_HIGH },
  { "default", TIC_PIN_FUNC_DEFAULT << TIC_PIN_FUNC_POSN },
  { "general", TIC_PIN_FUNC_GENERAL << TIC_PIN_FUNC_POSN },
  { "serial", TIC_PIN_FUNC_SERIAL << TIC_PIN_FUNC_POSN },
  { "rc", TIC_PIN_FUNC_RC << TIC_PIN_FUNC_POSN },
  { "encoder", TIC_PIN_FUNC_ENCODER << TIC_PIN_FUNC_POSN },
  { "limit_forward", TIC_PIN_FUNC_LIMIT_FORWARD << TIC_PIN_FUNC_POSN },
  { "limit_reverse", TIC_PIN_FUNC_LIMIT_REVERSE << TIC_PIN_FUNC_POSN },
  { "home", TIC_PIN_FUNC_HOME << TIC_PIN_FUNC_POSN },
  { "kill", TIC_PIN_FUNC_KILL << TIC_PIN_FUNC_POSN },
  { NULL, 0 },
};

const char * tic_convert_decay_mode_to_string(uint8_t decay_mode)
{
  const char * str;
  tic_code_to_name(tic_decay_mode_names, decay_mode, &r);
  return str;
}

const char * tic_convert_device_reset_to_string(uint8_t device_reset)
{
  const char * str;
  tic_code_to_name(tic_device_reset_names, device_reset, &str);
  return str;
}

bool tic_name_to_code(const tic_name * table, const char * name, uint32_t * code)
{
  assert(table != NULL);
  assert(name != NULL);
  assert(code != NULL);

  *code = 0;

  for (const tic_name * p = table; p->name; p++)
  {
    if (!strcmp(p->name, name))
    {
      *code = p->code;
      return true;
    }
  }

  return false;
}

bool tic_code_to_name(const tic_name * table, uint32_t code, const char ** name)
{
  assert(table != NULL);
  assert(name != NULL);

  *name = "";

  for (const tic_name * p = table; p->name; p++)
  {
    if (p->code == code)
    {
      *name = p->name;
      return true;
    }
  }

  return false;
}
