#include "tic_internal.h"

#define PIN_COUNT 5

struct tic_variables
{
  uint8_t operation_state;
  uint16_t error_status;
  uint32_t error_occurred;
  uint8_t planning_mode;
  int32_t target_position;
  int32_t target_velocity;
  uint32_t speed_min;
  uint32_t speed_max;
  uint32_t decel_max;
  uint32_t accel_max;
  int32_t current_position;
  int32_t current_velocity;
  int32_t acting_target_position;
  uint32_t time_since_last_step;
  uint8_t device_reset;
  uint16_t vin_voltage;
  uint32_t up_time;
  int32_t encoder_position;
  uint16_t rc_pulse_width;
  struct {
    bool switch_status;
    uint16_t analog_reading;
    bool digital_reading;
    uint8_t pin_state;
  } pin_info[PIN_COUNT];
  uint8_t step_mode;
  uint8_t decay_mode;
};

tic_error * tic_variables_create(tic_variables ** variables)
{
  if (variables == NULL)
  {
    return tic_error_create("Variables output pointer is null.");
  }

  *variables = NULL;

  tic_error * error = NULL;

  tic_variables * new_variables = NULL;
  if (error == NULL)
  {
    new_variables = (tic_variables *)calloc(1, sizeof(tic_variables));
    if (new_variables == NULL) { error = &tic_error_no_memory; }
  }

  if (error == NULL)
  {
    *variables = new_variables;
    new_variables = NULL;
  }

  tic_variables_free(new_variables);

  return error;
}

tic_error * tic_variables_copy(const tic_variables * source, tic_variables ** dest)
{
  if (dest == NULL)
  {
    return tic_error_create("Variables output pointer is null.");
  }

  *dest = NULL;

  if (source == NULL)
  {
    return NULL;
  }

  tic_error * error = NULL;

  tic_variables * new_variables = NULL;
  if (error == NULL)
  {
    new_variables = (tic_variables *)calloc(1, sizeof(tic_variables));
    if (new_variables == NULL) { error = &tic_error_no_memory; }
  }

  if (error == NULL)
  {
    memcpy(new_variables, source, sizeof(tic_variables));
  }

  if (error == NULL)
  {
    *dest = new_variables;
    new_variables = NULL;
  }

  tic_variables_free(new_variables);

  return error;
}

void tic_variables_free(tic_variables * variables)
{
  free(variables);
}

static void write_buffer_to_variables(const uint8_t * buf, tic_variables * variables)
{
  assert(variables != NULL);
  assert(buf != NULL);

  // TODO:
}

tic_error * tic_get_variables(tic_handle * handle, tic_variables ** variables,
  bool clear_errors_occurred)
{
  if (variables == NULL)
  {
    return tic_error_create("Variables output pointer is null.");
  }

  *variables = NULL;

  if (handle == NULL)
  {
    return tic_error_create("Handle is null.");
  }

  tic_error * error = NULL;

  // Create a variables object.
  tic_variables * new_variables = NULL;
  if (error == NULL)
  {
    error = tic_variables_create(&new_variables);
  }

  // Read all the variables from the device.
  uint8_t buf[TIC_VARIABLES_SIZE];
  if (error == NULL)
  {
    size_t index = 0;
    error = tic_get_variable_segment(handle, clear_errors_occurred, index, sizeof(buf), buf);
  }

  // Store the variables in the new variables object.
  if (error == NULL)
  {
    write_buffer_to_variables(buf, new_variables);
  }

  // Pass the new variables to the caller.
  if (error == NULL)
  {
    *variables = new_variables;
    new_variables = NULL;
  }

  tic_variables_free(new_variables);

  if (error != NULL)
  {
    error = tic_error_add(error,
      "There was an error reading variables from the device.");
  }

  return error;
}


uint8_t tic_variables_get_operation_state(const tic_variables * variables)
{
  if (variables == NULL) { return 0; }
  return variables->operation_state;
}

uint16_t tic_variables_get_error_status(const tic_variables * variables)
{
  if (variables == NULL) { return 0; }
  return variables->error_status;
}

uint32_t tic_variables_get_error_occurred(const tic_variables * variables)
{
  if (variables == NULL) { return 0; }
  return variables->error_occurred;
}

uint8_t tic_variables_get_planning_mode(const tic_variables * variables)
{
  if (variables == NULL) { return 0; }
  return variables->planning_mode;
}

int32_t tic_variables_get_target_position(const tic_variables * variables)
{
  if (variables == NULL) { return 0; }
  return variables->target_position;
}

int32_t tic_variables_get_target_velocity(const tic_variables * variables)
{
  if (variables == NULL) { return 0; }
  return variables->target_velocity;
}

uint32_t tic_variables_get_speed_min(const tic_variables * variables)
{
  if (variables == NULL) { return 0; }
  return variables->speed_min;
}

uint32_t tic_variables_get_speed_max(const tic_variables * variables)
{
  if (variables == NULL) { return 0; }
  return variables->speed_max;
}

uint32_t tic_variables_get_decel_max(const tic_variables * variables)
{
  if (variables == NULL) { return 0; }
  return variables->decel_max;
}

uint32_t tic_variables_get_accel_max(const tic_variables * variables)
{
  if (variables == NULL) { return 0; }
  return variables->accel_max;
}

int32_t tic_variables_get_current_position(const tic_variables * variables)
{
  if (variables == NULL) { return 0; }
  return variables->current_position;
}

int32_t tic_variables_get_current_velocity(const tic_variables * variables)
{
  if (variables == NULL) { return 0; }
  return variables->current_velocity;
}

int32_t tic_variables_get_acting_target_position(const tic_variables * variables)
{
  if (variables == NULL) { return 0; }
  return variables->acting_target_position;
}

uint32_t tic_variables_get_time_since_last_step(const tic_variables * variables)
{
  if (variables == NULL) { return 0; }
  return variables->time_since_last_step;
}

uint8_t tic_variables_get_device_reset(const tic_variables * variables)
{
  if (variables == NULL) { return 0; }
  return variables->device_reset;
}

uint32_t tic_variables_get_vin_voltage(const tic_variables * variables)
{
  if (variables == NULL) { return 0; }
  return variables->vin_voltage;
}

uint32_t tic_variables_get_up_time(const tic_variables * variables)
{
  if (variables == NULL) { return 0; }
  return variables->up_time;
}

int32_t tic_variables_get_encoder_position(const tic_variables * variables)
{
  if (variables == NULL) { return 0; }
  return variables->encoder_position;
}

uint16_t tic_variables_get_rc_pulse_width(const tic_variables * variables)
{
  if (variables == NULL) { return 0; }
  return variables->rc_pulse_width;
}

uint8_t tic_variables_get_step_mode(const tic_variables * variables)
{
  if (variables == NULL) { return 0; }
  return variables->step_mode;
}

uint8_t tic_variables_get_decay_mode(const tic_variables * variables)
{
  if (variables == NULL) { return 0; }
  return variables->decay_mode;
}

uint16_t tic_variables_get_analog_reading(const tic_variables * variables,
  uint8_t pin)
{
  if (variables == NULL || pin >= PIN_COUNT) { return 0; }
  return variables->pin_info[pin].analog_reading;
}

bool tic_variables_get_digital_reading(const tic_variables * variables,
  uint8_t pin)
{
  if (variables == NULL || pin >= PIN_COUNT) { return 0; }
  return variables->pin_info[pin].digital_reading;
}

bool tic_variables_get_switch_status(const tic_variables * variables,
  uint8_t pin)
{
  if (variables == NULL || pin >= PIN_COUNT) { return 0; }
  return variables->pin_info[pin].switch_status;
}

uint8_t tic_variables_get_pin_state(const tic_variables * variables,
  uint8_t pin)
{
  if (variables == NULL || pin >= PIN_COUNT) { return 0; }
  return variables->pin_info[pin].pin_state;
}
