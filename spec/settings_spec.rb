require_relative 'spec_helper'

DefaultSettings = {
  T825: <<END
product: T825
control_mode: serial
never_sleep: false
disable_safe_start: false
ignore_err_line_high: false
serial_baud_rate: 9600
serial_device_number: 14
i2c_device_address: 14
command_timeout: 0
serial_crc_enabled: false
low_vin_timeout: 250
low_vin_shutoff_voltage: 6000
low_vin_startup_voltage: 6500
high_vin_shutoff_voltage: 35000
vin_multiplier_offset: 0
rc_max_pulse_period: 100
rc_bad_signal_timeout: 500
rc_consecutive_good_pulses: 2
input_play: 0
input_error_min: 0
input_error_max: 65535
input_scaling_degree: linear
input_invert: false
input_min: 0
input_neutral_min: 32768
input_neutral_max: 32768
input_max: 65535
output_min: -200
output_neutral: 0
output_max: 200
encoder_prescaler: 2
encoder_postscaler: 1
scl_config: default
sda_config: default
tx_config: default
rx_config: default
rc_config: default
current_limit: 192
step_mode: 1
decay_mode: mixed
speed_min: 0
speed_max: 2000000
accel_max: 40000
decel_max: 0
decel_max_during_error: 0
END
}

TestSettings1 = {
  T825: <<END
product: T825
control_mode: rc_position
never_sleep: true
disable_safe_start: true
ignore_err_line_high: true
serial_baud_rate: 115385
serial_device_number: 40
i2c_device_address: 2
command_timeout: 2020
serial_crc_enabled: true
low_vin_timeout: 8000
low_vin_shutoff_voltage: 14000
low_vin_startup_voltage: 15000
high_vin_shutoff_voltage: 32000
vin_multiplier_offset: -345
rc_max_pulse_period: 1234
rc_bad_signal_timeout: 909
rc_consecutive_good_pulses: 9
input_play: 255
input_error_min: 303
input_error_max: 50050
input_scaling_degree: quadratic
input_invert: true
input_min: 404
input_neutral_min: 12312
input_neutral_max: 13412
input_max: 50000
output_min: -999
output_neutral: 12
output_max: 999
encoder_prescaler: 5
encoder_postscaler: 1000000000
scl_config: limit_forward pullup active_high
sda_config: limit_reverse analog
tx_config: kill pullup analog
rx_config: serial
rc_config: rc pullup
current_limit: 1984
step_mode: 32
decay_mode: fast
speed_min: 10000
speed_max: 234567890
accel_max: 934567820
decel_max: 734567890
decel_max_during_error: 234333890
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
    [ { 'i2c_device_address' => 128 },
      { 'i2c_device_address' => 127 },
      "Warning: The I2C device address was too high so it was changed to 127.\n"
    ],
    [ { 'low_vin_shutoff_voltage' => 9001, 'low_vin_startup_voltage' => 9000,
        'high_vin_shutoff_voltage' => 8999 },
      { 'low_vin_shutoff_voltage' => 9001, 'low_vin_startup_voltage' => 9501,
        'high_vin_shutoff_voltage' => 10001 },
        "Warning: The low VIN startup voltage was changed to 9501 mV.\n" \
        "Warning: The high VIN shutoff voltage was changed to 10001 mV.\n"
    ],
    [ { 'input_error_min' => 10, 'input_min' => 9, 'input_neutral_min' => 8,
        'input_neutral_max' => 7, 'input_max' => 6, 'input_error_max' => 5 },
      { 'input_error_min' => 0, 'input_min' => 0, 'input_neutral_min' => 0x8000,
        'input_neutral_max' => 0x8000, 'input_max' => 0xFFFF,
        'input_error_max' => 0xFFFF },
      "Warning: The input scaling values were out of order " \
      "so they will be reset to their default values.\n"
    ],
    [ { 'output_min' => -1, 'output_neutral' => -2, 'output_max' => -3 },
      { 'output_min' => -200, 'output_neutral' => 0, 'output_max' => 200 },
      "Warning: The output scaling values were out of order " \
      "so they will be reset to their default values.\n"
    ],
    [ { 'control_mode' => 'rc_speed', 'output_neutral' => 100 },
      { 'output_neutral' => 0 },
      "Warning: The output neutral value must be 0 in RC speed control mode " \
      "so it will be changed to 0.\n"
    ],
    [ { 'control_mode' => 'analog_speed', 'output_neutral' => 100 },
      { 'output_neutral' => 0 },
      "Warning: The output neutral value must be 0 in analog speed control mode " \
      "so it will be changed to 0.\n"
    ],
    [ { 'encoder_prescaler' => 9 },
      { 'encoder_prescaler' => 8 },
      "Warning: The encoder prescaler was too high " \
      "so it will be lowered to 8.\n"
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
    [ { 'decel_max_during_error' => 0x80000000 },
      { 'decel_max_during_error' => 0x7FFFFFFF },
      "Warning: The maximum deceleration during error was too high " \
      "so it will be lowered to 2147483647.\n"
    ],
    [ { 'decel_max_during_error' => 99 },
      { 'decel_max_during_error' => 100 },
      "Warning: The maximum deceleration during error was too low " \
      "so it will be raised to 100.\n"
    ],
    [ { 'decel_max_during_error' => 0 },  # 0 means same as accel_max
      { }
    ],
    [ { 'control_mode' => 'analog_position', 'sda_config' => 'home active_high' },
      { 'sda_config' => 'default active_high' },
      "Warning: The SDA pin must be used as an analog input " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'control_mode' => 'analog_speed', 'sda_config' => 'home pullup' },
      { 'sda_config' => 'default pullup' },
      "Warning: The SDA pin must be used as an analog input " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'control_mode' => 'analog_speed', 'sda_config' => 'general pullup' },
      { }
    ],
    [ { 'control_mode' => 'rc_position', 'rc_config' => 'home active_high' },
      { 'rc_config' => 'default active_high' },
      "Warning: The RC pin must be used as an RC input " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'control_mode' => 'rc_speed', 'rc_config' => 'general pullup' },
      { 'rc_config' => 'default pullup' },
      "Warning: The RC pin must be used as an RC input " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'control_mode' => 'rc_speed', 'rc_config' => 'rc pullup' },
      { }
    ],
    [ { 'control_mode' => 'encoder_speed', 'tx_config' => 'limit_forward active_high' },
      { 'tx_config' => 'default active_high' },
      "Warning: The TX pin must be used as an encoder input " \
      "so its function will be changed to the default.\n"
    ],
    [ { 'control_mode' => 'encoder_position', 'rx_config' => 'limit_reverse pullup',
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
    [ { 'rc_config' => 'general analog' },
      { 'rc_config' => 'general' },
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
    [ { 'scl_config' => 'general analog' },
      { 'scl_config' => 'default analog' },
      "Warning: The SCL pin must be used for I2C if the SDA pin is " \
      "so the SCL and SDA pin functions will be changed to the default.\n"
    ],
    [ { 'sda_config' => 'general pullup' },
      { 'sda_config' => 'default pullup' },
      "Warning: The SDA pin must be used for I2C if the SCL pin is " \
      "so the SCL and SDA pin functions will be changed to the default.\n"
    ],
    [ { 'control_mode' => 'analog_speed', 'scl_config' => 'general' },
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
