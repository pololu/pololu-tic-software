require_relative 'spec_helper'

DefaultSettings = {
  T825: <<END
product: T825
control_mode: serial
never_sleep: false
disable_safe_start: false
ignore_err_line_high: false
serial_baud_rate: 9600
serial_device_number: 0
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
scl_config: serial active_high
sda_config: serial active_high
tx_config: serial active_high
rx_config: serial active_high
rc_config: active_high
current_limit: 192
microstepping_mode: 1
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
control_mode: rc_speed
never_sleep: true
disable_safe_start: true
ignore_err_line_high: true
serial_baud_rate: 115200
serial_device_number: 40
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
input_error_max: 5050
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
scl_config: pullup analog active_high limit_forward
sda_config: analog limit_reverse
tx_config: pullup kill
rx_config: serial
rc_config: pullup home
current_limit: 2000
microstepping_mode: 1
decay_mode: fast
speed_min: 10000
speed_max: 234567890
accel_max: 934567820
decel_max: 734567890
decel_max_during_error: 234333890
END
}

describe 'settings' do
  let (:model) { :T825 }

  it 'settings test 1' do
    stdout, stderr, result = run_ticcmd('--set-settings -',
      input: TestSettings1[model])
    expect(stderr).to eq ""
    expect(stdout).to eq ""
    expect(result).to eq 0

    stdout, stderr, result = run_ticcmd('--restore-defaults')
    expect(stdout).to eq ""
    expect(stderr).to eq ""
    expect(result).to eq 0

    stdout, stderr, result = run_ticcmd('--get-settings -')
    expect(stderr).to eq ""
    expect(YAML.load(stdout)).to eq YAML.load(DefaultSettings[model])
    expect(result). to eq 0
  end
end
