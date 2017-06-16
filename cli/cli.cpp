#include "cli.h"

// [all-settings]
static const char help[] =
  CLI_NAME ": Pololu Tic USB Stepper Motor Controller Command-line Utility\n"
  "Version " SOFTWARE_VERSION_STRING "\n"
  "Usage: " CLI_NAME " OPTIONS\n"
  "\n"
  "General options:\n"
  "  -s, --status                Show device settings and info.\n"
  "  -d SERIALNUMBER             Specifies the serial number of the device.\n"
  "  --list                      List devices connected to computer.\n"
  "  --stop                      Stop the motor.\n"
  "  -p, --position NUM          Set target position in microsteps.\n"
  "  -y, --velocity NUM          Set target velocity in microsteps / 10000 s.\n"
  "  --set-current-position NUM  Set where the controller thinks it currently is.\n"
  // TODO: --stop, --enable-driver, --disable-driver
  "  --speed-max NUM             Set the speed maximum.\n"
  "  --speed-min NUM             Set the speed minimum.\n"
  "  --accel-max NUM             Set the acceleration maximum.\n"
  "  --decel-max NUM             Set the deceleration maximum.\n"
  "  --step-mode NUM             Set step mode: full, half, 1, 2, 4, 8, 16, 32.\n"
  "  --current NUM               Set the current limit in mA.\n"
  "  --decay MODE                Set decay mode: mixed, slow, or fast.\n"
  "  --restore-defaults          Restore device's factory settings\n"
  "  --settings FILE             Load settings file into device.\n"
  "  --get-settings FILE         Read device settings and write to file.\n"
  "  --fix-settings IN OUT       Read settings from a file and fix them.\n"
  "  -h, --help                  Show this help screen.\n"
  "\n"
  "The only option that makes permanent changes to the device is --settings.\n"
  "\n"
  "For more help, see: " DOCUMENTATION_URL "\n"
  "\n";

struct arguments
{
  bool show_status = false;

  bool serial_number_specified = false;
  std::string serial_number;

  bool show_list = false;

  bool set_target_position = false;
  int32_t target_position;

  bool set_target_velocity = false;
  int32_t target_velocity;

  bool set_current_position = false;
  int32_t current_position;

  bool stop = false;

  bool enable_driver = false;

  bool disable_driver = false;

  bool set_speed_max = false;
  uint32_t speed_max;

  bool set_speed_min = false;
  uint32_t speed_min;

  bool set_accel_max = false;
  uint32_t accel_max;

  bool set_decel_max = false;
  uint32_t decel_max;

  bool set_step_mode = false;
  uint8_t step_mode;

  bool set_current_limit = false;
  uint32_t current_limit;

  bool set_decay_mode = false;
  uint8_t decay_mode;

  bool restore_defaults = false;

  bool set_settings = false;
  std::string set_settings_filename;

  bool get_settings = false;
  std::string get_settings_filename;

  bool fix_settings = false;
  std::string fix_settings_input_filename;
  std::string fix_settings_output_filename;

  bool show_help = false;

  bool get_debug_data = false;

  uint32_t test_procedure = 0;

  bool action_specified() const
  {
    return show_status ||
      show_list ||
      set_target_position ||
      set_target_velocity ||
      set_current_position ||
      stop ||
      enable_driver ||
      disable_driver ||
      set_speed_max ||
      set_speed_min ||
      set_accel_max ||
      set_decel_max ||
      set_step_mode ||
      set_current_limit ||
      set_decay_mode ||
      restore_defaults ||
      set_settings ||
      get_settings ||
      fix_settings ||
      show_help ||
      get_debug_data ||
      test_procedure;
  }
};

// Note: This will not work correctly if T is uint64_t.
template <typename T>
static T parse_arg_int(arg_reader & arg_reader, int base = 10)
{
  const char * value_c = arg_reader.next();
  if (value_c == NULL)
  {
    throw exception_with_exit_code(EXIT_BAD_ARGS,
      "Expected a number after '" + std::string(arg_reader.last()) + "'.");
  }

  char * end;
  int64_t result = strtoll(value_c, &end, base);
  if (errno || *end)
  {
    throw exception_with_exit_code(EXIT_BAD_ARGS,
      "The number after '" + std::string(arg_reader.last()) + "' is invalid.");
  }

  if (result < std::numeric_limits<T>::min())
  {
    throw exception_with_exit_code(EXIT_BAD_ARGS,
      "The number after '" + std::string(arg_reader.last()) + "' is too small.");
  }

  if (result > std::numeric_limits<T>::max())
  {
    throw exception_with_exit_code(EXIT_BAD_ARGS,
      "The number after '" + std::string(arg_reader.last()) + "' is too large.");
  }

  return result;
}

static std::string parse_arg_string(arg_reader & arg_reader)
{
    const char * value_c = arg_reader.next();
    if (value_c == NULL)
    {
      throw exception_with_exit_code(EXIT_BAD_ARGS,
        "Expected an argument after '" +
        std::string(arg_reader.last()) + "'.");
    }
    if (value_c[0] == 0)
    {
      throw exception_with_exit_code(EXIT_BAD_ARGS,
        "Expected a non-empty argument after '" +
        std::string(arg_reader.last()) + "'.");
    }
    return std::string(value_c);
}

static uint8_t parse_arg_step_mode(arg_reader & arg_reader)
{
  std::string mode_str = parse_arg_string(arg_reader);
  if (mode_str == "1" || mode_str == "full")
  {
    return TIC_STEP_MODE_MICROSTEP1;
  }
  else if (mode_str == "2" || mode_str == "half")
  {
    return TIC_STEP_MODE_MICROSTEP2;
  }
  else if (mode_str == "4")
  {
    return TIC_STEP_MODE_MICROSTEP4;
  }
  else if (mode_str == "8")
  {
    return TIC_STEP_MODE_MICROSTEP8;
  }
  else if (mode_str == "16")
  {
    return TIC_STEP_MODE_MICROSTEP16;
  }
  else if (mode_str == "32")
  {
    return TIC_STEP_MODE_MICROSTEP32;
  }
  else
  {
    throw exception_with_exit_code(EXIT_BAD_ARGS,
      "The step mode specified is invalid.");
  }
}

static uint8_t parse_arg_decay_mode(arg_reader & arg_reader)
{
  std::string decay_str = parse_arg_string(arg_reader);
  if (decay_str == "mixed")
  {
    return TIC_DECAY_MODE_MIXED;
  }
  else if (decay_str == "slow")
  {
    return TIC_DECAY_MODE_SLOW;
  }
  else if (decay_str == "fast")
  {
    return TIC_DECAY_MODE_FAST;
  }
  else
  {
    throw exception_with_exit_code(EXIT_BAD_ARGS,
      "The decay mode specified is invalid.");
  }
}

static arguments parse_args(int argc, char ** argv)
{
  arg_reader arg_reader(argc, argv);
  arguments args;

  while (1)
  {
    const char * arg_c = arg_reader.next();
    if (arg_c == NULL)
    {
      break;  // Done reading arguments.
    }

    std::string arg = arg_c;

    if (arg == "-s" || arg == "--status")
    {
      args.show_status = true;
    }
    else if (arg == "-d" || arg == "--serial")
    {
      args.serial_number_specified = true;
      args.serial_number = parse_arg_string(arg_reader);
    }
    else if (arg == "--list")
    {
      args.show_list = true;
    }
    else if (arg == "-p" || arg == "--position")
    {
      args.set_target_position = true;
      args.target_position = parse_arg_int<int32_t>(arg_reader);
    }
    else if (arg == "-y" || arg == "--velocity")
    {
      args.set_target_velocity = true;
      args.target_velocity = parse_arg_int<int32_t>(arg_reader);
    }
    else if (arg == "--set-current-position")
    {
      args.set_current_position = true;
      args.current_position = parse_arg_int<int32_t>(arg_reader);
    }
    else if (arg == "--stop")
    {
      args.stop = true;
    }
    else if (arg == "--enable-driver")
    {
      args.enable_driver = true;
    }
    else if (arg == "--disable-driver")
    {
      args.disable_driver = true;
    }
    else if (arg == "--speed-max")
    {
      args.set_speed_max = true;
      args.speed_max = parse_arg_int<uint32_t>(arg_reader);
    }
    else if (arg == "--speed-min")
    {
      args.set_speed_min = true;
      args.speed_min = parse_arg_int<uint32_t>(arg_reader);
    }
    else if (arg == "--accel-max")
    {
      args.set_accel_max = true;
      args.accel_max = parse_arg_int<uint32_t>(arg_reader);
    }
    else if (arg == "--decel-max")
    {
      args.set_decel_max = true;
      args.decel_max = parse_arg_int<uint32_t>(arg_reader);
    }
    else if (arg == "--step-mode")
    {
      args.set_step_mode = true;
      args.step_mode = parse_arg_step_mode(arg_reader);
    }
    else if (arg == "--current")
    {
      args.set_current_limit = true;
      args.current_limit = parse_arg_int<uint32_t>(arg_reader);
    }
    else if (arg == "--decay" || arg == "--decay-mode")
    {
      args.set_decay_mode = true;
      args.decay_mode = parse_arg_decay_mode(arg_reader);
    }
    else if (arg == "--restore-defaults" || arg == "--restoredefaults")
    {
      args.restore_defaults = true;
    }
    else if (arg == "--settings" || arg == "--set-settings" || arg == "--configure")
    {
      args.set_settings = true;
      args.set_settings_filename = parse_arg_string(arg_reader);
    }
    else if (arg == "--get-settings" || arg == "--getconf")
    {
      args.get_settings = true;
      args.get_settings_filename = parse_arg_string(arg_reader);
    }
    else if (arg == "--fix-settings")
    {
      args.fix_settings = true;
      args.fix_settings_input_filename = parse_arg_string(arg_reader);
      args.fix_settings_output_filename = parse_arg_string(arg_reader);
    }
    else if (arg == "-h" || arg == "--help" ||
      arg == "--h" || arg == "-help" || arg == "/help" || arg == "/h")
    {
      args.show_help = true;
    }
    else if (arg == "--debug")
    {
      // This is an unadvertized option for helping customers troubleshoot
      // issues with their device.
      args.get_debug_data = true;
    }
    else if (arg == "--test")
    {
      // This option and the options below are unadvertised and helps us test
      // the software.
      args.test_procedure = parse_arg_int<uint32_t>(arg_reader);
    }
    else
    {
      throw exception_with_exit_code(EXIT_BAD_ARGS,
        std::string("Unknown option: '") + arg + "'.");
    }
  }
  return args;
}

static void print_list(device_selector & selector)
{
  for (const tic::device & instance : selector.list_devices())
  {
    std::cout << std::left << std::setfill(' ');
    std::cout << std::setw(17) << instance.get_serial_number() + "," << " ";
    std::cout << std::setw(45) << instance.get_name();
    std::cout << std::endl;
  }
}

static void set_target_position(device_selector & selector, int32_t position)
{
  tic::device device = selector.select_device();
  tic::handle handle(device);
  handle.set_target_position(position);
}

static void set_target_velocity(device_selector & selector, int32_t velocity)
{
  tic::device device = selector.select_device();
  tic::handle handle(device);
  handle.set_target_velocity(velocity);
}

static void set_current_position(device_selector & selector, int32_t current_position)
{
  tic::device device = selector.select_device();
  tic::handle handle(device);
  handle.set_current_position(current_position);
}

static void stop(device_selector & selector)
{
  tic::device device = selector.select_device();
  tic::handle handle(device);
  handle.stop();
}

static void enable_driver(device_selector & selector)
{
  tic::device device = selector.select_device();
  tic::handle handle(device);
  handle.enable_driver();
}

static void disable_driver(device_selector & selector)
{
  tic::device device = selector.select_device();
  tic::handle handle(device);
  handle.disable_driver();
}

static void set_speed_max(device_selector & selector, uint32_t speed_max)
{
  tic::device device = selector.select_device();
  tic::handle handle(device);
  handle.set_speed_max(speed_max);
}

static void set_speed_min(device_selector & selector, uint32_t speed_min)
{
  tic::device device = selector.select_device();
  tic::handle handle(device);
  handle.set_speed_min(speed_min);
}

static void set_accel_max(device_selector & selector, uint32_t accel_max)
{
  tic::device device = selector.select_device();
  tic::handle handle(device);
  handle.set_accel_max(accel_max);
}

static void set_decel_max(device_selector & selector, uint32_t decel_max)
{
  tic::device device = selector.select_device();
  tic::handle handle(device);
  handle.set_decel_max(decel_max);
}

static void set_step_mode(device_selector & selector, uint8_t step_mode)
{
  tic::device device = selector.select_device();
  tic::handle handle(device);
  handle.set_step_mode(step_mode);
}

static void set_current_limit(device_selector & selector, uint32_t current_limit)
{
  if (current_limit > TIC_MAX_ALLOWED_CURRENT)
  {
    current_limit = TIC_MAX_ALLOWED_CURRENT;
    std::cerr
      << "Warning: The current limit was too high "
      << "so it will be lowered to " << current_limit << " mA." << std::endl;
  }

  tic::device device = selector.select_device();
  tic::handle handle(device);
  handle.set_current_limit(current_limit);
}

static void set_decay_mode(device_selector & selector, uint8_t decay_mode)
{
  tic::device device = selector.select_device();
  tic::handle handle(device);
  handle.set_decay_mode(decay_mode);
}

static void get_status(device_selector & selector)
{
  tic::device device = selector.select_device();
  tic::handle handle(device);
  tic::variables vars = handle.get_variables(true);
  std::string name = device.get_name();
  std::string serial_number = device.get_serial_number();
  std::string firmware_version = handle.get_firmware_version_string();
  print_status(vars, name, serial_number, firmware_version);
}

static void restore_defaults(device_selector & selector)
{
  tic::device device = selector.select_device();
  tic::handle(device).restore_defaults();

  // Give the Tic time to modify its settings.
  std::this_thread::sleep_for(std::chrono::milliseconds(1500));
}

static void get_settings(device_selector & selector,
  const std::string & filename)
{
  tic::device device = selector.select_device();
  tic::settings settings = tic::handle(device).get_settings();
  std::string settings_string = settings.to_string();

  std::string warnings;
  settings.fix(&warnings);
  std::cerr << warnings;

  write_string_to_file(filename, settings_string);
}

static void set_settings(device_selector & selector,
  const std::string & filename)
{
  std::string settings_string = read_string_from_file(filename);

  std::string warnings;
  tic::settings settings = tic::settings::read_from_string(
    settings_string, &warnings);
  std::cerr << warnings;

  settings.fix(&warnings);
  std::cerr << warnings;

  tic::device device = selector.select_device();
  tic::handle handle(device);
  handle.set_settings(settings);
  handle.reinitialize();
}

static void fix_settings(const std::string & input_filename,
  const std::string & output_filename)
{
  std::string in_str = read_string_from_file(input_filename);

  std::string warnings;
  tic::settings settings = tic::settings::read_from_string(in_str, &warnings);
  std::cerr << warnings;

  settings.fix(&warnings);
  std::cerr << warnings;

  write_string_to_file(output_filename, settings.to_string());
}

static void print_debug_data(device_selector & selector)
{
  tic::device device = selector.select_device();
  tic::handle handle(device);

  std::vector<uint8_t> data(4096, 0);
  handle.get_debug_data(data);

  for (const uint8_t & byte : data)
  {
    std::cout << std::setfill('0') << std::setw(2) << std::hex
              << (unsigned int)byte << ' ';
  }
  std::cout << std::endl;
}

static void test_procedure(device_selector & selector, uint32_t procedure)
{
  if (procedure == 1)
  {
    // Let's print some fake variable data to test our print_status().  This
    // test invokes all sorts of undefined behavior but it's the easiest way to
    // put fake data into a tic::variables object without modifying
    // libpololu-tic.
    uint8_t fake_data[4096];
    memset(fake_data, 0xFF, sizeof(fake_data));
    tic::variables fake_vars((tic_variables *)fake_data);
    print_status(fake_vars, "Fake name", "123", "9.99");
    fake_vars.pointer_release();
  }
  else if (procedure == 2)
  {
    tic::device device = selector.select_device();
    tic::handle handle(device);
    while (1)
    {
      tic::variables vars = handle.get_variables();
      std::cout << vars.get_analog_reading(TIC_PIN_NUM_SDA) << ','
                << vars.get_target_position() << ','
                << vars.get_acting_target_position() << ','
                << vars.get_current_position() << ','
                << vars.get_current_velocity() << ','
                << std::endl;
    }
  }
  else
  {
    throw std::runtime_error("Unknown test procedure.");
  }
}

// A note about ordering: We want to do all the setting stuff first because it
// could affect subsequent options.  We want to shoe the status last, because it
// could be affected by options before it.
static void run(int argc, char ** argv)
{
  arguments args = parse_args(argc, argv);

  if (args.show_help || !args.action_specified())
  {
    std::cout << help;
    return;
  }

  // TODO: friendly error if they try to set target position and velocity

  device_selector selector;
  if (args.serial_number_specified)
  {
    selector.specify_serial_number(args.serial_number);
  }

  if (args.show_list)
  {
    print_list(selector);
    return;
  }

  if (args.fix_settings)
  {
    fix_settings(args.fix_settings_input_filename,
      args.fix_settings_output_filename);
  }

  if (args.get_settings)
  {
    get_settings(selector, args.get_settings_filename);
  }

  if (args.restore_defaults)
  {
    restore_defaults(selector);
  }

  if (args.set_settings)
  {
    set_settings(selector, args.set_settings_filename);
  }

  if (args.set_speed_max)
  {
    set_speed_max(selector, args.speed_max);
  }

  if (args.set_speed_min)
  {
    set_speed_min(selector, args.speed_min);
  }

  if (args.set_accel_max)
  {
    set_accel_max(selector, args.accel_max);
  }

  if (args.set_decel_max)
  {
    set_decel_max(selector, args.decel_max);
  }

  if (args.stop)
  {
    stop(selector);
  }

  if (args.set_target_position)
  {
    set_target_position(selector, args.target_position);
  }

  if (args.set_target_velocity)
  {
    set_target_velocity(selector, args.target_velocity);
  }

  if (args.set_current_position)
  {
    set_current_position(selector, args.current_position);
  }

  if (args.set_step_mode)
  {
    set_step_mode(selector, args.step_mode);
  }

  if (args.set_current_limit)
  {
    set_current_limit(selector, args.current_limit);
  }

  if (args.set_decay_mode)
  {
    set_decay_mode(selector, args.decay_mode);
  }

  if (args.enable_driver)
  {
    enable_driver(selector);
  }

  if (args.disable_driver)
  {
    disable_driver(selector);
  }

  if (args.get_debug_data)
  {
    print_debug_data(selector);
  }

  if (args.test_procedure)
  {
    test_procedure(selector, args.test_procedure);
  }

  if (args.show_status)
  {
    get_status(selector);
  }
}

int main(int argc, char ** argv)
{
  int exit_code = 0;

  try
  {
    run(argc, argv);
  }
  catch (const exception_with_exit_code & error)
  {
    std::cerr << "Error: " << error.what() << std::endl;
    exit_code = error.get_code();
  }
  catch (const std::exception & error)
  {
    std::cerr << "Error: " << error.what() << std::endl;
    exit_code = EXIT_OPERATION_FAILED;
  }

  return exit_code;
}
