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

const tic_name tic_microstepping_mode_names[] =
{
  { "1", TIC_MICROSTEPPING_MODE_1 },
  { "2", TIC_MICROSTEPPING_MODE_2 },
  { "4", TIC_MICROSTEPPING_MODE_4 },
  { "8", TIC_MICROSTEPPING_MODE_8 },
  { "16", TIC_MICROSTEPPING_MODE_16 },
  { "32", TIC_MICROSTEPPING_MODE_32 },
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
  { "serial", 1 << TIC_PIN_SERIAL_OR_I2C },
  { "pullup", 1 << TIC_PIN_PULLUP },
  { "analog", 1 << TIC_PIN_ANALOG },
  { "active_high", 1 << TIC_PIN_ACTIVE_HIGH },
  { "limit_forward", TIC_PIN_SWITCH_LIMIT_FORWARD << TIC_PIN_SWITCH_POSN },
  { "limit_reverse", TIC_PIN_SWITCH_LIMIT_REVERSE << TIC_PIN_SWITCH_POSN },
  { "home", TIC_PIN_SWITCH_HOME << TIC_PIN_SWITCH_POSN },
  { "kill", TIC_PIN_SWITCH_KILL << TIC_PIN_SWITCH_POSN },
  { NULL, 0 },
};


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
