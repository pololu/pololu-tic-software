#include "tic_internal.h"

const tic_name tic_bool_names[] =
{
  { "true", 1 },
  { "false", 0 },
  { NULL, 0 },
};

const tic_name tic_product_names_short[] =
{
  { "T825", TIC_PRODUCT_T825 },
  { NULL, 0 },
};

const tic_name tic_product_names_ui[] =
{
  { "Tic T825 Stepper Motor Controller", TIC_PRODUCT_T825 },
  { NULL, 0 },
};

const tic_name tic_error_names_ui[] =
{
  { "Intentionally de-energized", 1 << TIC_ERROR_INTENTIONALLY_DEENERGIZED },
  { "Motor driver error", 1 << TIC_ERROR_MOTOR_DRIVER_ERROR },
  { "Low VIN", 1 << TIC_ERROR_LOW_VIN },
  { "Kill switch active", 1 << TIC_ERROR_KILL_SWITCH },
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

const tic_name tic_decay_mode_names_ui[] =
{
  { "Mixed", TIC_DECAY_MODE_MIXED },
  { "Slow", TIC_DECAY_MODE_SLOW },
  { "Fast", TIC_DECAY_MODE_FAST },
  { NULL, 0 },
};

const tic_name tic_input_state_names_ui[] =
{
  { "Not ready", TIC_INPUT_STATE_NOT_READY },
  { "Invalid", TIC_INPUT_STATE_INVALID },
  { "Halt", TIC_INPUT_STATE_HALT },
  { "Position", TIC_INPUT_STATE_POSITION },
  { "Velocity", TIC_INPUT_STATE_VELOCITY },
  { NULL, 0 },
};

const tic_name tic_device_reset_names_ui[] =
{
  { "Power-on reset", TIC_RESET_POWER_UP },
  { "Brown-out reset", TIC_RESET_BROWNOUT },
  { "Reset pin driven low", TIC_RESET_RESET_LINE },
  { "Watchdog reset", TIC_RESET_WATCHDOG },
  { "Software reset (bootloader)", TIC_RESET_SOFTWARE },
  { "Stack overflow", TIC_RESET_STACK_OVERFLOW },
  { "Stack underflow", TIC_RESET_STACK_UNDERFLOW },
  { NULL, 0 },
};

const tic_name tic_operation_state_names_ui[] =
{
  { "Reset", TIC_OPERATION_STATE_RESET },
  { "De-energized", TIC_OPERATION_STATE_DEENERGIZED },
  { "Soft error", TIC_OPERATION_STATE_SOFT_ERROR },
  { "Waiting for ERR line", TIC_OPERATION_STATE_WAITING_FOR_ERR_LINE },
  { "Starting up", TIC_OPERATION_STATE_STARTING_UP },
  { "Normal", TIC_OPERATION_STATE_NORMAL },
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

const tic_name tic_step_mode_names_ui[] =
{
  { "Full step", TIC_STEP_MODE_MICROSTEP1 },
  { "1/2 step", TIC_STEP_MODE_MICROSTEP2 },
  { "1/4 step", TIC_STEP_MODE_MICROSTEP4 },
  { "1/8 step", TIC_STEP_MODE_MICROSTEP8 },
  { "1/16 step", TIC_STEP_MODE_MICROSTEP16 },
  { "1/32 step", TIC_STEP_MODE_MICROSTEP32 },
  { NULL, 0 },
};

const tic_name tic_pin_state_names_ui[] =
{
  { "High impedance", TIC_PIN_STATE_HIGH_IMPEDANCE },
  { "Pulled up", TIC_PIN_STATE_PULLED_UP },
  { "Output low", TIC_PIN_STATE_OUTPUT_LOW },
  { "Output high", TIC_PIN_STATE_OUTPUT_HIGH },
  { NULL, 0},
};

const tic_name tic_planning_mode_names_ui[] =
{
  { "Off", TIC_PLANNING_MODE_OFF },
  { "Target position", TIC_PLANNING_MODE_TARGET_POSITION },
  { "Target velocity", TIC_PLANNING_MODE_TARGET_VELOCITY },
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
  { "deenergize", TIC_RESPONSE_DEENERGIZE },
  { "halt_and_hold", TIC_RESPONSE_HALT_AND_HOLD },
  { "decel_to_hold", TIC_RESPONSE_DECEL_TO_HOLD },
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

const tic_name tic_pin_func_names[] =
{
  { "default", TIC_PIN_FUNC_DEFAULT },
  { "user_io", TIC_PIN_FUNC_USER_IO },
  { "user_input", TIC_PIN_FUNC_USER_INPUT },
  { "pot_power", TIC_PIN_FUNC_POT_POWER },
  { "serial", TIC_PIN_FUNC_SERIAL },
  { "rc", TIC_PIN_FUNC_RC },
  { "encoder", TIC_PIN_FUNC_ENCODER },
  { "kill_switch", TIC_PIN_FUNC_KILL_SWITCH },
  { NULL, 0 },
};

const char * tic_look_up_product_name_short(uint32_t product)
{
  const char * str = "";
  tic_code_to_name(tic_product_names_short, product, &str);
  return str;
}

const char * tic_look_up_product_name_ui(uint32_t product)
{
  const char * str = "(Unknown)";
  tic_code_to_name(tic_product_names_ui, product, &str);
  return str;
}

const char * tic_look_up_error_name_ui(uint32_t error)
{
  const char * str = "(Unknown)";
  tic_code_to_name(tic_error_names_ui, error, &str);
  return str;
}

const char * tic_look_up_decay_mode_name_ui(uint8_t decay_mode)
{
  const char * str = "(Unknown)";
  tic_code_to_name(tic_decay_mode_names_ui, decay_mode, &str);
  return str;
}

const char * tic_look_up_input_state_name_ui(uint8_t input_state)
{
  const char * str = "(Unknown)";
  tic_code_to_name(tic_input_state_names_ui, input_state, &str);
  return str;
}

const char * tic_look_up_device_reset_name_ui(uint8_t device_reset)
{
  const char * str = "(Unknown)";
  tic_code_to_name(tic_device_reset_names_ui, device_reset, &str);
  return str;
}

const char * tic_look_up_operation_state_name_ui(uint8_t operation_state)
{
  const char * str = "(Unknown)";
  tic_code_to_name(tic_operation_state_names_ui, operation_state, &str);
  return str;
}

const char * tic_look_up_step_mode_name_ui(uint8_t step_mode)
{
  const char * str = "(Unknown)";
  tic_code_to_name(tic_step_mode_names_ui, step_mode, &str);
  return str;
}

const char * tic_look_up_pin_state_name_ui(uint8_t pin_state)
{
  const char * str = "(Unknown)";
  tic_code_to_name(tic_pin_state_names_ui, pin_state, &str);
  return str;
}

const char * tic_look_up_planning_mode_name_ui(uint8_t planning_mode)
{
  const char * str = "(Unknown)";
  tic_code_to_name(tic_planning_mode_names_ui, planning_mode, &str);
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
  assert(name != NULL);
  assert(table != NULL);

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
