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

const tic_name tic_nice_error_names[] =
{
  { "Intentionally disabled", 1 << TIC_ERROR_INTENTIONALLY_DISABLED },
  { "Motor driver error", 1 << TIC_ERROR_MOTOR_DRIVER_ERROR },
  { "Low VIN", 1 << TIC_ERROR_LOW_VIN },
  { "Kill switch", 1 << TIC_ERROR_KILL_SWITCH },
  { "Required input invalid", 1 << TIC_ERROR_REQUIRED_INPUT_INVALID },
  { "Serial error", 1 << TIC_ERROR_SERIAL_ERROR },
  { "Command timeout", 1 << TIC_ERROR_COMMAND_TIMEOUT },
  { "Safe start violation", 1 << TIC_ERROR_SAFE_START_VIOLATION },
  { "ERR line high", 1 << TIC_ERROR_ERR_LINE_HIGH },
  { "Serial framing", 1 << TIC_ERROR_SERIAL_FRAMING },
  { "Serial RX overrun", 1 << TIC_ERROR_SERIAL_RX_OVERRUN },
  { "Serial format", 1 << TIC_ERROR_SERIAL_FORMAT },
  { "Serial CRC", 1 << TIC_ERROR_SERIAL_CRC },
  { "Encoder skip", 1 << TIC_ERROR_ENCODER_SKIP },
  { NULL, 0 },
};

const tic_name tic_decay_mode_names[] =
{
  { "mixed", TIC_DECAY_MODE_MIXED },
  { "slow", TIC_DECAY_MODE_SLOW },
  { "fast", TIC_DECAY_MODE_FAST },
  { NULL, 0 },
};

const tic_name tic_device_reset_names[] =
{
  { "power_up", TIC_RESET_POWER_UP },
  { "brownout", TIC_RESET_BROWNOUT },
  { "reset_line", TIC_RESET_RESET_LINE },
  { "watchdog", TIC_RESET_WATCHDOG },
  { "software", TIC_RESET_SOFTWARE },
  { "stack_overflow", TIC_RESET_STACK_OVERFLOW },
  { "stack_underflow", TIC_RESET_STACK_UNDERFLOW },
  { NULL, 0 },
};

const tic_name tic_operation_state_names[] =
{
  { "disabled", TIC_OPERATION_STATE_DISABLED },
  { "error", TIC_OPERATION_STATE_ERROR },
  { "waiting_for_err_line", TIC_OPERATION_STATE_WAITING_FOR_ERR_LINE },
  { "waiting_for_input", TIC_OPERATION_STATE_WAITING_FOR_INPUT },
  { "input_invalid", TIC_OPERATION_STATE_INPUT_INVALID },
  { "active", TIC_OPERATION_STATE_ACTIVE },
  { NULL, 0 },
};

const tic_name tic_step_mode_names[] =
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

const tic_name tic_pin_state_names[] =
{
  { "high_impedance", TIC_PIN_STATE_HIGH_IMPEDANCE },
  { "pulled_up", TIC_PIN_STATE_PULLED_UP },
  { "output_low", TIC_PIN_STATE_OUTPUT_LOW },
  { "output_high", TIC_PIN_STATE_OUTPUT_HIGH },
  { NULL, 0},
};

const tic_name tic_planning_mode_names[] =
{
  { "off", TIC_PLANNING_MODE_OFF },
  { "target_position", TIC_PLANNING_MODE_TARGET_POSITION },
  { "target_velocity", TIC_PLANNING_MODE_TARGET_VELOCITY },
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

const tic_name tic_response_names[] =
{
  { "disable_driver", TIC_RESPONSE_DISABLE_DRIVER },
  { "stop", TIC_RESPONSE_STOP },
  { "coast_to_stop", TIC_RESPONSE_COAST_TO_STOP },
  { "go_to_position", TIC_RESPONSE_GO_TO_POSITION },
  { NULL, 0},
};

const tic_name tic_scaling_degree_names[] =
{
  { "linear", TIC_SCALING_DEGREE_LINEAR },
  { "quadratic", TIC_SCALING_DEGREE_QUADRATIC },
  { "cubic", TIC_SCALING_DEGREE_CUBIC },
  { NULL, 0 },
};

const tic_name tic_pin_config_names[] =
{
  { "pullup", 1 << TIC_PIN_PULLUP },
  { "analog", 1 << TIC_PIN_ANALOG },
  { "active_high", 1 << TIC_PIN_ACTIVE_HIGH },
  { "default", TIC_PIN_FUNC_DEFAULT << TIC_PIN_FUNC_POSN },
  { "general", TIC_PIN_FUNC_GENERAL << TIC_PIN_FUNC_POSN },
  { "pot_power", TIC_PIN_FUNC_POT_POWER << TIC_PIN_FUNC_POSN },
  { "serial", TIC_PIN_FUNC_SERIAL << TIC_PIN_FUNC_POSN },
  { "rc", TIC_PIN_FUNC_RC << TIC_PIN_FUNC_POSN },
  { "encoder", TIC_PIN_FUNC_ENCODER << TIC_PIN_FUNC_POSN },
  { "limit_forward", TIC_PIN_FUNC_LIMIT_FORWARD << TIC_PIN_FUNC_POSN },
  { "limit_reverse", TIC_PIN_FUNC_LIMIT_REVERSE << TIC_PIN_FUNC_POSN },
  { "home", TIC_PIN_FUNC_HOME << TIC_PIN_FUNC_POSN },
  { "kill", TIC_PIN_FUNC_KILL << TIC_PIN_FUNC_POSN },
  { NULL, 0 },
};

const char * tic_look_up_nice_error_string(uint32_t error)
{
  const char * str;
  tic_code_to_name(tic_nice_error_names, error, &str);
  return str;
}

const char * tic_look_up_decay_mode_string(uint8_t decay_mode)
{
  const char * str;
  tic_code_to_name(tic_decay_mode_names, decay_mode, &str);
  return str;
}

const char * tic_look_up_device_reset_string(uint8_t device_reset)
{
  const char * str;
  tic_code_to_name(tic_device_reset_names, device_reset, &str);
  return str;
}

const char * tic_look_up_operation_state_string(uint8_t operation_state)
{
  const char * str;
  tic_code_to_name(tic_operation_state_names, operation_state, &str);
  return str;
}

const char * tic_look_up_step_mode_string(uint8_t step_mode)
{
  const char * str;
  tic_code_to_name(tic_step_mode_names, step_mode, &str);
  return str;
}

const char * tic_look_up_pin_state_string(uint8_t pin_state)
{
  const char * str;
  tic_code_to_name(tic_pin_state_names, pin_state, &str);
  return str;
}

const char * tic_look_up_planning_mode_string(uint8_t planning_mode)
{
  const char * str;
  tic_code_to_name(tic_planning_mode_names, planning_mode, &str);
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
