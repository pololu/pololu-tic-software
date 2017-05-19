#include "cli.h"

static const int left_column_width = 30;
static auto left_column = std::setw(left_column_width);

static std::string pretty_enum(const char * c_str)
{
  std::string str = c_str;

  if (str == "")
  {
    return std::string("(unknown)");
  }

  // Replace underscores with spaces.
  std::replace(str.begin(), str.end(), '_', ' ');
  return str;
}

static void print_errors(uint32_t errors, const char * error_set_name)
{
  std::cout << error_set_name << ":" << std::endl;
  for (uint32_t i = 0; i < 32; i++)
  {
    uint32_t error = (1 << i);
    if (errors & error)
    {
      std::cout << "  - " << pretty_enum(tic_look_up_nice_error_string(error)) << std::endl;
    }
  }
}

static void print_pin_info(const tic::variables & vars,
  uint8_t pin, const char * pin_name)
{
  std::cout<< pin_name << " pin:" << std::endl;
  std::cout << left_column << "  Digital reading: "
    << vars.get_digital_reading(pin) << std::endl;
  std::cout << left_column << "  Switch status: "
    << vars.get_switch_status(pin) << std::endl;
  if (pin != TIC_PIN_NUM_RC)
  {
    std::cout << left_column << "  Analog reading: "
      << vars.get_analog_reading(pin) << std::endl;
  }
  std::cout << left_column << "  State: "
    << pretty_enum(tic_look_up_pin_state_string(vars.get_pin_state(pin)))
    << std::endl;
}

void print_status(const tic::variables & vars,
  const std::string & name,
  const std::string & serial_number,
  const std::string & firmware_version)
{
  // The output here is YAML so that people can more easily write scripts that
  // use it.

  // TODO: make it pretty, or make it ugly, just make it consistent

  std::cout << std::left << std::setfill(' ');

  std::cout << left_column << "Name: "
    << name << std::endl;

  std::cout << left_column << "Serial number: "
    << serial_number << std::endl;

  std::cout << left_column << "Firmware version: "
    << firmware_version << std::endl;

  std::cout << left_column << "Last reset: "
    << pretty_enum(tic_look_up_device_reset_string(vars.get_device_reset()))
    << std::endl;

  std::cout << std::endl;

  std::cout << left_column << "Operation state: "
    << pretty_enum(tic_look_up_operation_state_string(vars.get_operation_state()))
    << std::endl;
  std::cout << std::endl;

  print_errors(vars.get_error_status(),
    "Errors currently stopping the motor");
  std::cout << std::endl;

  print_errors(vars.get_errors_occurred(),
    "Errors that occurred since last check");
  std::cout << std::endl;

  std::cout << left_column << "Planning mode: "
    << pretty_enum(tic_look_up_planning_mode_string(vars.get_planning_mode()))
    << std::endl;

  std::cout << left_column << "Target position: "
    << vars.get_target_position() << std::endl;

  std::cout << left_column << "Target velocity: "
    << vars.get_target_velocity() << std::endl;

  std::cout << left_column << "Speed min: "
    << vars.get_speed_min() << std::endl;

  std::cout << left_column << "Speed max: "
    << vars.get_speed_max() << std::endl;

  std::cout << left_column << "Decel max: "
    << vars.get_decel_max() << std::endl;

  std::cout << left_column << "Accel max: "
    << vars.get_accel_max() << std::endl;

  std::cout << left_column << "Current position: "
    << vars.get_current_position() << std::endl;

  std::cout << left_column << "Current velocity: "
    << vars.get_current_velocity() << std::endl;

  std::cout << left_column << "Acting target position: "
    << vars.get_acting_target_position() << std::endl;

  std::cout << left_column << "Time since last step: "
    << vars.get_time_since_last_step() << std::endl;

  std::cout << std::endl;

  std::cout << left_column << "VIN: "
    << vars.get_vin_voltage() << " mV"
    << std::endl;

  std::cout << left_column << "Up time: "
    << vars.get_up_time() << " ms"  // TODO: HH:MM:SS:mmm format
    << std::endl;

  std::cout << left_column << "Encoder position: "
    << vars.get_encoder_position()
    << std::endl;

  std::cout << left_column << "RC pulse width: "
    << vars.get_rc_pulse_width()
    << std::endl;

  std::cout << left_column << "Step mode: "
    << pretty_enum(tic_look_up_step_mode_string(vars.get_step_mode()))
    << std::endl;

  std::cout << left_column << "Decay mode: "
    << pretty_enum(tic_look_up_decay_mode_string(vars.get_decay_mode()))
    << std::endl;
  std::cout << std::endl;

  print_pin_info(vars, TIC_PIN_NUM_SCL, "SCL");
  print_pin_info(vars, TIC_PIN_NUM_SDA, "SDA");
  print_pin_info(vars, TIC_PIN_NUM_TX, "TX");
  print_pin_info(vars, TIC_PIN_NUM_RX, "RX");
  print_pin_info(vars, TIC_PIN_NUM_RC, "RC");
}
