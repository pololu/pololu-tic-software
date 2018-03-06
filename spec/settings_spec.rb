require_relative 'spec_helper'

DefaultSettings = {
  T825: <<END,
product: T825
control_mode: serial
never_sleep: false
disable_safe_start: false
ignore_err_line_high: false
auto_clear_driver_error: true
soft_error_response: decel_to_hold
soft_error_position: 0
serial_baud_rate: 9600
serial_device_number: 14
command_timeout: 1000
serial_crc_enabled: false
serial_response_delay: 0
vin_calibration: 0
input_averaging_enabled: true
input_hysteresis: 0
input_scaling_degree: linear
input_invert: false
input_min: 0
input_neutral_min: 2015
input_neutral_max: 2080
input_max: 4095
output_min: -200
output_max: 200
encoder_prescaler: 1
encoder_postscaler: 1
encoder_unlimited: false
scl_config: default
sda_config: default
tx_config: default
rx_config: default
rc_config: default
current_limit: 192
current_limit_during_error: -1
step_mode: 1
decay_mode: mixed
max_speed: 2000000
starting_speed: 0
max_accel: 40000
max_decel: 0
invert_motor_direction: false
END

  T834: <<END,
product: T834
control_mode: serial
never_sleep: false
disable_safe_start: false
ignore_err_line_high: false
auto_clear_driver_error: true
soft_error_response: decel_to_hold
soft_error_position: 0
serial_baud_rate: 9600
serial_device_number: 14
command_timeout: 1000
serial_crc_enabled: false
serial_response_delay: 0
vin_calibration: 0
input_averaging_enabled: true
input_hysteresis: 0
input_scaling_degree: linear
input_invert: false
input_min: 0
input_neutral_min: 2015
input_neutral_max: 2080
input_max: 4095
output_min: -200
output_max: 200
encoder_prescaler: 1
encoder_postscaler: 1
encoder_unlimited: false
scl_config: default
sda_config: default
tx_config: default
rx_config: default
rc_config: default
current_limit: 192
current_limit_during_error: -1
step_mode: 1
decay_mode: mixed50
max_speed: 2000000
starting_speed: 0
max_accel: 40000
max_decel: 0
invert_motor_direction: false
END

  T500: <<END,
product: T500
control_mode: serial
never_sleep: false
disable_safe_start: false
ignore_err_line_high: false
auto_clear_driver_error: true
soft_error_response: decel_to_hold
soft_error_position: 0
serial_baud_rate: 9600
serial_device_number: 14
command_timeout: 1000
serial_crc_enabled: false
serial_response_delay: 0
vin_calibration: 0
input_averaging_enabled: true
input_hysteresis: 0
input_scaling_degree: linear
input_invert: false
input_min: 0
input_neutral_min: 2015
input_neutral_max: 2080
input_max: 4095
output_min: -200
output_max: 200
encoder_prescaler: 1
encoder_postscaler: 1
encoder_unlimited: false
scl_config: default
sda_config: default
tx_config: default
rx_config: default
rc_config: default
current_limit: 275
current_limit_during_error: -1
step_mode: 1
max_speed: 2000000
starting_speed: 0
max_accel: 40000
max_decel: 0
invert_motor_direction: false
END
}

TestSettings1 = {
  T825: <<END,
product: T825
control_mode: rc_position
never_sleep: true
disable_safe_start: true
ignore_err_line_high: true
auto_clear_driver_error: true
soft_error_response: decel_to_hold
soft_error_position: -234333890
serial_baud_rate: 115385
serial_device_number: 40
command_timeout: 2020
serial_crc_enabled: true
serial_response_delay: 123
vin_calibration: -345
input_averaging_enabled: false
input_hysteresis: 4455
input_scaling_degree: cubic
input_invert: true
input_min: 404
input_neutral_min: 505
input_neutral_max: 606
input_max: 3000
output_min: -999
output_max: 999
encoder_prescaler: 5
encoder_postscaler: 1000000000
encoder_unlimited: true
scl_config: user_input pullup active_high
sda_config: kill_switch analog
tx_config: kill_switch pullup analog
rx_config: serial
rc_config: rc pullup
current_limit: 384
current_limit_during_error: 96
step_mode: 32
decay_mode: fast
max_speed: 234567890
starting_speed: 10000
max_accel: 934567820
max_decel: 734567890
invert_motor_direction: true
END

  T834: <<END,
product: T834
control_mode: rc_position
never_sleep: true
disable_safe_start: true
ignore_err_line_high: true
auto_clear_driver_error: true
soft_error_response: decel_to_hold
soft_error_position: -234333890
serial_baud_rate: 115385
serial_device_number: 40
command_timeout: 2020
serial_crc_enabled: true
serial_response_delay: 123
vin_calibration: -345
input_averaging_enabled: false
input_hysteresis: 4455
input_scaling_degree: cubic
input_invert: true
input_min: 404
input_neutral_min: 505
input_neutral_max: 606
input_max: 3000
output_min: -999
output_max: 999
encoder_prescaler: 5
encoder_postscaler: 1000000000
encoder_unlimited: true
scl_config: user_input pullup active_high
sda_config: kill_switch analog
tx_config: kill_switch pullup analog
rx_config: serial
rc_config: rc pullup
current_limit: 384
current_limit_during_error: 96
step_mode: 32
decay_mode: mixed75
max_speed: 234567890
starting_speed: 10000
max_accel: 934567820
max_decel: 734567890
invert_motor_direction: true
END

  T500: <<END,
product: T500
control_mode: rc_position
never_sleep: true
disable_safe_start: true
ignore_err_line_high: true
auto_clear_driver_error: true
soft_error_response: decel_to_hold
soft_error_position: -234333890
serial_baud_rate: 115385
serial_device_number: 40
command_timeout: 2020
serial_crc_enabled: true
serial_response_delay: 123
vin_calibration: -345
input_averaging_enabled: false
input_hysteresis: 4455
input_scaling_degree: cubic
input_invert: true
input_min: 404
input_neutral_min: 505
input_neutral_max: 606
input_max: 3000
output_min: -999
output_max: 999
encoder_prescaler: 5
encoder_postscaler: 1000000000
encoder_unlimited: true
scl_config: user_input pullup active_high
sda_config: kill_switch analog
tx_config: kill_switch pullup analog
rx_config: serial
rc_config: rc pullup
current_limit: 1455
current_limit_during_error: 847
step_mode: 8
max_speed: 234567890
starting_speed: 10000
max_accel: 934567820
max_decel: 734567890
invert_motor_direction: true
END
}

def test_cases_for_settings_fix(product)
  defaults = YAML.load(DefaultSettings.fetch(product))

  if product == :T500
    lowest_current = 19
    low_current = 152
    default_current = 275
    medium_current = 589
    high_current = 2036
  else
    lowest_current = 0
    low_current = 64
    default_current = 192
    medium_current = 320
    high_current = 2048
  end

  max_current = tic_max_allowed_current(product)

  cases = [
    [ { 'control_mode' => 'rc_speed', 'soft_error_response' => 'go_to_position' },
      { 'soft_error_response' => 'decel_to_hold' },
      "Warning: The soft error response cannot be \"Go to position\" in a " \
      "speed control mode, so it will be changed to \"Decelerate to hold\".\n"
    ],
    [ { 'control_mode' => 'rc_position', 'soft_error_response' => 'go_to_position' },
      { },
    ],
    [ { 'serial_baud_rate' => 115200 },
      { 'serial_baud_rate' => 115385 }
    ],
    [ { 'serial_baud_rate' => 101 },
      { 'serial_baud_rate' => 200 },
      "Warning: The serial baud rate is too low so it will be changed to 200.\n"
    ],
    [ { 'serial_baud_rate' => 115386 },
      { 'serial_baud_rate' => 115385 },
      "Warning: The serial baud rate is too high so it will be changed to 115385.\n"
    ],
    [ { 'serial_device_number' => 128 },
      { 'serial_device_number' => 127 },
      "Warning: The serial device number is too high so it will be changed to 127.\n"
    ],
    [ { 'command_timeout' => 60001 },
      { 'command_timeout' => 60000 },
      "Warning: The command timeout is too high so it will be changed to 60000 ms.\n"
    ],
    [ { 'vin_calibration' => -501 },
      { 'vin_calibration' => -500 },
      "Warning: The VIN calibration was too low so it will be raised to -500.\n"
    ],
    [ { 'vin_calibration' => 501 },
      { 'vin_calibration' => 500 },
      "Warning: The VIN calibration was too high so it will be lowered to 500.\n"
    ],
    #[ { 'low_vin_shutoff_voltage' => 9001, 'low_vin_startup_voltage' => 9000,
    #    'high_vin_shutoff_voltage' => 8999 },
    #  { 'low_vin_shutoff_voltage' => 9001, 'low_vin_startup_voltage' => 9501,
    #    'high_vin_shutoff_voltage' => 10001 },
    #    "Warning: The low VIN startup voltage will be changed to 9501 mV.\n" \
    #    "Warning: The high VIN shutoff voltage will be changed to 10001 mV.\n"
    #],
    [ { 'input_min' => 9, 'input_neutral_min' => 8,
        'input_neutral_max' => 7, 'input_max' => 6, },
      { 'input_min' => 0, 'input_neutral_min' => 2015,
        'input_neutral_max' => 2080, 'input_max' => 4095, },
      "Warning: The input scaling values are out of order " \
      "so they will be reset to their default values.\n"
    ],
    [ { 'input_min' => 4096, 'input_neutral_min' => 4097,
        'input_neutral_max' => 4098, 'input_max' => 4099, },
      { 'input_min' => 4095, 'input_neutral_min' => 4095,
        'input_neutral_max' => 4095, 'input_max' => 4095, },
      "Warning: The input minimum is too high so it will be lowered to 4095.\n" \
      "Warning: The input neutral min is too high so it will be lowered to 4095.\n" \
      "Warning: The input neutral max is too high so it will be lowered to 4095.\n" \
      "Warning: The input maximum is too high so it will be lowered to 4095.\n"
    ],
    [ { 'output_min' => 1 },
      { 'output_min' => 0 },
      "Warning: The scaling output minimum is above 0 " \
      "so it will be lowered to 0.\n"
    ],
    [ { 'output_max' => -1 },
      { 'output_max' => 0 },
      "Warning: The scaling output maximum is below 0 " \
      "so it will be raised to 0.\n"
    ],
    [ { 'encoder_prescaler' => 0 },
      { 'encoder_prescaler' => 1 },
      "Warning: The encoder prescaler is zero so it will be changed to 1.\n"
    ],
    [ { 'encoder_prescaler' => 2147483648 },
      { 'encoder_prescaler' => 2147483647 },
      "Warning: The encoder prescaler is too high " \
      "so it will be lowered to 2147483647.\n"
    ],
    [ { 'encoder_postscaler' => 0 },
      { 'encoder_postscaler' => 1 },
      "Warning: The encoder postscaler is zero so it will be changed to 1.\n"
    ],
    [ { 'encoder_postscaler' => 2147483648 },
      { 'encoder_postscaler' => 2147483647 },
      "Warning: The encoder postscaler is too high " \
      "so it will be lowered to 2147483647.\n"
    ],
    [ { 'current_limit' => lowest_current },
      { }
    ],
    [ { 'current_limit' => default_current + 16 },
      { 'current_limit' => default_current },
    ],
    # This warning is not possible any more now that the tic_settings object
    # just stores codes and not milliamps.
    #[ { 'current_limit' => max_current + 1 },
    #  { 'current_limit' => max_current },
    #  "Warning: The current limit is too high " \
    #  "so it will be lowered to #{max_current} mA.\n"
    #],
    [ { 'current_limit' => medium_current, 'current_limit_during_error' => low_current },
      { }
    ],
    [ { 'current_limit' => medium_current, 'current_limit_during_error' => high_current },
      { 'current_limit_during_error' => -1 },
      "Warning: The current limit during error is higher than the default " \
      "current limit so it will be changed to be the same.\n"
    ],
    [ { 'current_limit' => medium_current, 'current_limit_during_error' => -2 },
      { 'current_limit_during_error' => -1 },
    ],
    [ { 'max_speed' => 70000_0000 },
      { 'max_speed' => 50000_0000 },
      "Warning: The maximum speed is too high " \
      "so it will be lowered to 500000000 (50 kHz).\n"
    ],
    [ { 'starting_speed' => 500_0000 },
      { 'starting_speed' => 200_0000 },
      "Warning: The starting speed is greater than the maximum speed " \
      "so it will be lowered to 2000000.\n"
    ],
    [ { 'max_decel' => 0x80000000 },
      { 'max_decel' => 0x7FFFFFFF },
      "Warning: The maximum deceleration is too high " \
      "so it will be lowered to 2147483647.\n"
    ],
    [ { 'max_decel' => 99 },
      { 'max_decel' => 100 },
      "Warning: The maximum deceleration is too low " \
      "so it will be raised to 100.\n"
    ],
    [ { 'max_decel' => 0 },  # 0 means same as max_accel
      { }
    ],
    [ { 'max_accel' => 0x80000000 },
      { 'max_accel' => 0x7FFFFFFF },
      "Warning: The maximum acceleration is too high " \
      "so it will be lowered to 2147483647.\n"
    ],
    [ { 'max_accel' => 99 },
      { 'max_accel' => 100 },
      "Warning: The maximum acceleration is too low " \
      "so it will be raised to 100.\n"
    ],
    [ { 'max_accel' => 0 },
      { 'max_accel' => 100 },
      "Warning: The maximum acceleration is too low " \
      "so it will be raised to 100.\n"
    ],
    [ { 'control_mode' => 'analog_position', 'sda_config' => 'user_io active_high' },
      { 'sda_config' => 'default active_high' },
      "Warning: The SDA pin must be used as an analog input " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'control_mode' => 'analog_speed', 'sda_config' => 'user_io pullup' },
      { 'sda_config' => 'default pullup' },
      "Warning: The SDA pin must be used as an analog input " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'control_mode' => 'analog_speed', 'sda_config' => 'user_input pullup' },
      { }
    ],
    [ { 'control_mode' => 'rc_position', 'rc_config' => 'kill_switch active_high' },
      { 'rc_config' => 'default active_high' },
      "Warning: The RC pin must be used as an RC input " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'control_mode' => 'rc_speed', 'rc_config' => 'user_io pullup' },
      { 'rc_config' => 'default pullup' },
      "Warning: The RC pin must be used as an RC input " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'control_mode' => 'rc_speed', 'rc_config' => 'rc pullup' },
      { }
    ],
    [ { 'control_mode' => 'encoder_speed', 'tx_config' => 'kill_switch active_high' },
      { 'tx_config' => 'default active_high' },
      "Warning: The TX pin must be used as an encoder input " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'control_mode' => 'encoder_position', 'rx_config' => 'kill_switch pullup',
        'tx_config' => 'encoder analog' },
      { 'rx_config' => 'default pullup' },
      "Warning: The RX pin must be used as an encoder input " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'rc_config' => 'user_io active_high' },
      { 'rc_config' => 'default active_high' },
      "Warning: The RC pin cannot be a user I/O pin " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'sda_config' => 'pot_power' },
      { 'sda_config' => 'default' },
      "Warning: The SDA pin cannot be used as a potentiometer power pin " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'tx_config' => 'pot_power active_high' },
      { 'tx_config' => 'default active_high' },
      "Warning: The TX pin cannot be used as a potentiometer power pin " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'rx_config' => 'pot_power active_high' },
      { 'rx_config' => 'default active_high' },
      "Warning: The RX pin cannot be used as a potentiometer power pin " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'rc_config' => 'pot_power' },
      { 'rc_config' => 'default' },
      "Warning: The RC pin cannot be used as a potentiometer power pin " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'rc_config' => 'serial pullup active_high' },
      { 'rc_config' => 'default pullup active_high' },
      "Warning: The RC pin cannot be a serial pin " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'sda_config' => 'rc analog' },
      { 'sda_config' => 'default analog' },
      "Warning: The SDA pin cannot be used as an RC input " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'scl_config' => 'rc pullup' },
      { 'scl_config' => 'default pullup' },
      "Warning: The SCL pin cannot be used as an RC input " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'tx_config' => 'rc active_high' },
      { 'tx_config' => 'default active_high' },
      "Warning: The TX pin cannot be used as an RC input " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'rx_config' => 'rc pullup active_high' },
      { 'rx_config' => 'default pullup active_high' },
      "Warning: The RX pin cannot be used as an RC input " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'scl_config' => 'encoder analog' },
      { 'scl_config' => 'default analog' },
      "Warning: The SCL pin cannot be used as an encoder input " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'sda_config' => 'encoder active_high' },
      { 'sda_config' => 'default active_high' },
      "Warning: The SDA pin cannot be used as an encoder input " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'rc_config' => 'encoder pullup' },
      { 'rc_config' => 'default pullup' },
      "Warning: The RC pin cannot be used as an encoder input " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'rc_config' => 'user_input analog' },
      { 'rc_config' => 'user_input' },
      "Warning: The RC pin cannot be an analog input " \
      "so that feature will be disabled.\n"
    ],
    [ { 'scl_config' => 'user_io analog' },
      { 'scl_config' => 'default analog' },
      "Warning: The SCL pin must be used for I2C if the SDA pin is, " \
      "so the SCL and SDA pin functions will be changed to the default.\n"
    ],
    [ { 'sda_config' => 'user_io pullup' },
      { 'sda_config' => 'default pullup' },
      "Warning: The SDA pin must be used for I2C if the SCL pin is, " \
      "so the SCL and SDA pin functions will be changed to the default.\n"
    ],
    [ { 'control_mode' => 'analog_speed', 'scl_config' => 'user_io' },
      { }
    ],
  ]

  if product == :T500
    cases << [ { 'step_mode' => 16 },
               { 'step_mode' => 1 },
               "Warning: The step mode is invalid " \
               "so it will be changed to 1 (full step).\n"
             ]
  else
    cases << [ { 'decay_mode' => 'mode4' },
               { 'decay_mode' => case product
                                 when :T825 then 'mixed'
                                 when :T834 then 'mixed75'
                                 end
               },
             ]
  end

  cases
end

describe 'settings' do
  specify 'settings test 1', usb: true do
    # Set the settings to something kind of random.
    stdout, stderr, result = run_ticcmd('--set-settings -',
      input: TestSettings1.fetch(tic_product))
    expect(stderr).to eq ""
    expect(stdout).to eq ""
    expect(result).to eq 0

    # Read the settings back and make sure they match.
    stdout, stderr, result = run_ticcmd('--get-settings -')
    expect(stderr).to eq ""
    expect(YAML.load(stdout)).to eq YAML.load(TestSettings1.fetch(tic_product))
    expect(result). to eq 0

    # Restore the device to its default settings.
    stdout, stderr, result = run_ticcmd('--restore-defaults')
    expect(stdout).to eq ""
    expect(stderr).to eq ""
    expect(result).to eq 0

    # Check that the settings on the device match what we expect the default
    # settings to be.
    stdout, stderr, result = run_ticcmd('--get-settings -')
    expect(stderr).to eq ""
    expect(YAML.load(stdout)).to eq YAML.load(DefaultSettings.fetch(tic_product))
    expect(result).to eq 0
  end

  TicProductSymbols.each do |product|
    specify "tic_settings_fill_with_defaults is correct for #{product}" do
      stdin = "product: #{product}"
      stdout, stderr, result = run_ticcmd('--fix-settings - -', input: stdin)
      expect(stderr).to eq ""
      expect(YAML.load(stdout)).to eq YAML.load(DefaultSettings.fetch(product))
      expect(result).to eq 0
    end

    specify "tic_settings_fix is correct for #{product}" do
      defaults = YAML.load(DefaultSettings.fetch(product))
      test_cases_for_settings_fix(product).each do |input_part, output_part, warnings|
        warnings ||= ""
        if !warnings.empty? && !warnings.end_with?("\n")
          raise "Warnings should end with newline: #{warnings.inspect}"
        end
        input = defaults.merge(input_part)
        output = input.merge(output_part)

        stdout, stderr, result = run_ticcmd('--fix-settings - -',
          input: YAML.dump(input))
        expect(stderr).to eq warnings
        expect(YAML.load(stdout)).to eq output
        expect(result).to eq 0
      end
    end
  end

  describe 'integer processor' do
    let (:head) { "product: T825\n" }

    it "rejects numbers that are too big" do
      stdout, stderr, result = run_ticcmd('--fix-settings - -',
        input: "#{head}serial_baud_rate: 1111111111111111111")
      expect(stderr).to eq \
        "Error: There was an error reading the settings file.  " \
        "The serial_baud_rate value is out of range.\n"
      expect(result).to eq 2
    end

    it "rejects numbers that are too small" do
      stdout, stderr, result = run_ticcmd('--fix-settings - -',
        input: "#{head}serial_baud_rate: -1111111111111111111")
      expect(stderr).to eq \
        "Error: There was an error reading the settings file.  " \
        "The serial_baud_rate value is out of range.\n"
      expect(result).to eq 2
    end

    it "rejects numbers that are too small" do
      stdout, stderr, result = run_ticcmd('--fix-settings - -',
        input: "#{head}serial_baud_rate: -1111111111111111111")
      expect(stderr).to eq \
        "Error: There was an error reading the settings file.  " \
        "The serial_baud_rate value is out of range.\n"
      expect(result).to eq 2
    end

    it "rejects numbers with junk at the end" do
      stdout, stderr, result = run_ticcmd('--fix-settings - -',
        input: "#{head}serial_baud_rate: -11a")
      expect(stderr).to eq \
        "Error: There was an error reading the settings file.  " \
        "Invalid serial_baud_rate value.\n"
      expect(result).to eq 2
    end
  end
end
