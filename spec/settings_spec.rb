require_relative 'spec_helper'

DefaultSettings = {
  t825: <<END
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
rc_consecutive_good_pulses_get: 2
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

describe 'settings' do
  it 'settings test 1' do
    # TODO: load some test settings and read them back

    stdout, stderr, result = run_ticcmd('--restore-defaults')
    expect(result).to eq 0
    expect(stdout).to eq ""
    expect(stderr).to eq ""

    stdout, stderr, result = run_ticcmd('--get-settings -')
    expect(result). to eq 0
    expect(stderr).to eq ""
    expect(stdout).to eq DefaultSettings[:t825]
    expect { YAML.load(stdout) }.to_not raise_error
  end
end
