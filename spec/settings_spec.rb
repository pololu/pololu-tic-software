require_relative 'spec_helper'

DefaultSettings = {
  T825: <<END
product: T825
control_mode: serial
never_sleep: false
disable_safe_start: false
ignore_err_line_high: false
auto_clear_driver_error: true
soft_error_response: deenergize
soft_error_position: 0
serial_baud_rate: 9600
serial_device_number: 14
command_timeout: 1000
serial_crc_enabled: false
vin_multiplier_offset: 0
input_averaging_enabled: true
input_hysteresis: 0
input_invert: false
input_min: 0
input_neutral_min: 2048
input_neutral_max: 2048
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
current_limit_during_error: 0
step_mode: 1
decay_mode: mixed
speed_min: 0
speed_max: 2000000
accel_max: 40000
decel_max: 0
invert_motor_direction: false
END
}

TestSettings1 = {
  T825: <<END
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
vin_multiplier_offset: -345
input_averaging_enabled: false
input_hysteresis: 4455
input_invert: true
input_min: 404
input_neutral_min: 12312
input_neutral_max: 13412
input_max: 50000
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
speed_min: 10000
speed_max: 234567890
accel_max: 934567820
decel_max: 734567890
invert_motor_direction: true
END
}

def test_cases_for_settings_fix(product)
  defaults = YAML.load(DefaultSettings.fetch(product))

  [
    [ { 'serial_baud_rate' => 115200 },
      { 'serial_baud_rate' => 115385 }
    ],
    [ { 'serial_baud_rate' => 101 },
      { 'serial_baud_rate' => 200 },
      "Warning: The serial baud rate was too low so it was changed to 200.\n"
    ],
    [ { 'serial_baud_rate' => 115386 },
      { 'serial_baud_rate' => 115385 },
      "Warning: The serial baud rate was too high so it was changed to 115385.\n"
    ],
    [ { 'serial_device_number' => 128 },
      { 'serial_device_number' => 127 },
      "Warning: The serial device number was too high so it was changed to 127.\n"
    ],
    #[ { 'low_vin_shutoff_voltage' => 9001, 'low_vin_startup_voltage' => 9000,
    #    'high_vin_shutoff_voltage' => 8999 },
    #  { 'low_vin_shutoff_voltage' => 9001, 'low_vin_startup_voltage' => 9501,
    #    'high_vin_shutoff_voltage' => 10001 },
    #    "Warning: The low VIN startup voltage was changed to 9501 mV.\n" \
    #    "Warning: The high VIN shutoff voltage was changed to 10001 mV.\n"
    #],
    [ { 'input_min' => 9, 'input_neutral_min' => 8,
        'input_neutral_max' => 7, 'input_max' => 6, },
      { 'input_min' => 0, 'input_neutral_min' => 2048,
        'input_neutral_max' => 2048, 'input_max' => 4095, },
      "Warning: The input scaling values were out of order " \
      "so they will be reset to their default values.\n"
    ],
    [ { 'encoder_prescaler' => 0 },
      { 'encoder_prescaler' => 1 },
      "Warning: The encoder prescaler was zero so it will be changed to 1.\n"
    ],
    [ { 'encoder_prescaler' => 2147483648 },
      { 'encoder_prescaler' => 2147483647 },
      "Warning: The encoder prescaler was too high " \
      "so it will be lowered to 2147483647.\n"
    ],
    [ { 'encoder_postscaler' => 0 },
      { 'encoder_postscaler' => 1 },
      "Warning: The encoder postscaler was zero so it will be changed to 1.\n"
    ],
    [ { 'encoder_postscaler' => 2147483648 },
      { 'encoder_postscaler' => 2147483647 },
      "Warning: The encoder postscaler was too high " \
      "so it will be lowered to 2147483647.\n"
    ],
    [ { 'current_limit' => 0 },
      { }
    ],
    [ { 'current_limit' => 209 },
      { 'current_limit' => 192 },
    ],
    [ { 'current_limit' => 3969 },
      { 'current_limit' => 3968 },
      "Warning: The current limit was too high " \
      "so it will be lowered to 3968 mA.\n"
    ],
    [ { 'current_limit' => 320, 'current_limit_during_error' => 64 },
      { }
    ],
    [ { 'current_limit' => 320, 'current_limit_during_error' => 640 },
      { 'current_limit_during_error' => 0 },
      "Warning: The current limit during error was higher than the default " \
      "current limit so it will be changed to be the same.\n"
    ],
    [ { 'speed_max' => 70000_0000 },
      { 'speed_max' => 50000_0000 },
      "Warning: The maximum speed was too high " \
      "so it will be lowered to 500000000 (50 kHz).\n"
    ],
    [ { 'speed_min' => 500_0000 },
      { 'speed_min' => 200_0000 },
      "Warning: The minimum speed was greater than the maximum speed " \
      "so it will be lowered to 2000000.\n"
    ],
    [ { 'decel_max' => 0x80000000 },
      { 'decel_max' => 0x7FFFFFFF },
      "Warning: The maximum deceleration was too high " \
      "so it will be lowered to 2147483647.\n"
    ],
    [ { 'decel_max' => 99 },
      { 'decel_max' => 100 },
      "Warning: The maximum deceleration was too low " \
      "so it will be raised to 100.\n"
    ],
    [ { 'decel_max' => 0 },  # 0 means same as accel_max
      { }
    ],
    [ { 'accel_max' => 0x80000000 },
      { 'accel_max' => 0x7FFFFFFF },
      "Warning: The maximum acceleration was too high " \
      "so it will be lowered to 2147483647.\n"
    ],
    [ { 'accel_max' => 99 },
      { 'accel_max' => 100 },
      "Warning: The maximum acceleration was too low " \
      "so it will be raised to 100.\n"
    ],
    [ { 'accel_max' => 0 },
      { 'accel_max' => 100 },
      "Warning: The maximum acceleration was too low " \
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
    [ { 'rc_config' => 'serial pullup active_high' },
      { 'rc_config' => 'default pullup active_high' },
      "Warning: The RC pin cannot be a serial pin " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'rc_config' => 'user_io analog' },
      { 'rc_config' => 'user_io' },
      "Warning: The RC pin cannot be an analog input " \
      "so that feature will be disabled.\n"
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
    [ { 'scl_config' => 'user_io analog' },
      { 'scl_config' => 'default analog' },
      "Warning: The SCL pin must be used for I2C if the SDA pin is " \
      "so the SCL and SDA pin functions will be changed to the default.\n"
    ],
    [ { 'sda_config' => 'user_io pullup' },
      { 'sda_config' => 'default pullup' },
      "Warning: The SDA pin must be used for I2C if the SCL pin is " \
      "so the SCL and SDA pin functions will be changed to the default.\n"
    ],
    [ { 'control_mode' => 'analog_speed', 'scl_config' => 'user_io' },
      { }
    ],
  ]
end

describe 'settings' do
  let (:product) { :T825 }

  specify 'settings test 1', usb: true do
    # Set the settings to something kind of random.
    stdout, stderr, result = run_ticcmd('--set-settings -',
      input: TestSettings1[product])
    expect(stderr).to eq ""
    expect(stdout).to eq ""
    expect(result).to eq 0

    # Read the settings back and make sure they match.
    stdout, stderr, result = run_ticcmd('--get-settings -')
    expect(stderr).to eq ""
    expect(YAML.load(stdout)).to eq YAML.load(TestSettings1[product])
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
    expect(YAML.load(stdout)).to eq YAML.load(DefaultSettings[product])
    expect(result).to eq 0
  end

  specify 'tic_settings_fill_with_defaults is correct' do
    stdin = "product: #{product}"
    stdout, stderr, result = run_ticcmd('--fix-settings - -', input: stdin)
    expect(stderr).to eq ""
    expect(YAML.load(stdout)).to eq YAML.load(DefaultSettings[product])
    expect(result).to eq 0
  end

  specify 'tic_settings_fix is correct' do
    defaults = YAML.load(DefaultSettings[product])
    test_cases_for_settings_fix(product).each do |input_part, output_part, warnings|
      input = defaults.merge(input_part)
      output = input.merge(output_part)

      stdout, stderr, result = run_ticcmd('--fix-settings - -',
        input: YAML.dump(input))
      expect(stderr).to eq (warnings || "")
      expect(YAML.load(stdout)).to eq output
      expect(result).to eq 0
    end
  end
end
