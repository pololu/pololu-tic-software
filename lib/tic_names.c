#include "tic_internal.h"

tic_name tic_control_mode_names[] =
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

bool tic_name_to_code(const tic_name * table, const char * name, uint32_t * code)
{
  assert (table != NULL);
  assert (name != NULL);
  assert (code != NULL);

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
  assert (table != NULL);
  assert (name != NULL);

  *name = NULL;

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
