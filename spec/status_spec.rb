
require_relative 'spec_helper'

FakeStatus = <<END
Name:                         Fake name
Serial number:                123
Firmware version:             9.99
Last reset:                   (unknown)

Operation state:              (unknown)

Errors currently stopping the motor:
  - Intentionally de-energized
  - Motor driver error
  - Low VIN
  - Kill switch
  - Required input invalid
  - Serial error
  - Command timeout
  - Safe start violation
  - ERR line high
  - (unknown)
  - (unknown)
  - (unknown)
  - (unknown)
  - (unknown)
  - (unknown)
  - (unknown)

Errors that occurred since last check:
  - Intentionally de-energized
  - Motor driver error
  - Low VIN
  - Kill switch
  - Required input invalid
  - Serial error
  - Command timeout
  - Safe start violation
  - ERR line high
  - (unknown)
  - (unknown)
  - (unknown)
  - (unknown)
  - (unknown)
  - (unknown)
  - (unknown)
  - Serial framing
  - Serial RX overrun
  - Serial format
  - Serial CRC
  - Encoder skip
  - (unknown)
  - (unknown)
  - (unknown)
  - (unknown)
  - (unknown)
  - (unknown)
  - (unknown)
  - (unknown)
  - (unknown)
  - (unknown)
  - (unknown)

Planning mode:                (unknown)
Speed max:                    4294967295
Starting speed:               4294967295
Accel max:                    4294967295
Decel max:                    4294967295
Current position:             -1
Current velocity:             -1
Acting target position:       -1
Time since last step:         4294967295

VIN:                          65535 mV
Up time:                      4294967295 ms
Encoder position:             -1
RC pulse width:               N/A
Step mode:                    (unknown)
Current limit:                4294967295 mA
Decay mode:                   (unknown)
Input state:                  (unknown)
Input after averaging:        N/A
Input after hysteresis:       N/A
Input after scaling:          -1

SCL pin:
  State:                      (unknown)
  Digital reading:            255
SDA pin:
  State:                      (unknown)
  Digital reading:            255
TX pin:
  State:                      (unknown)
  Digital reading:            255
RX pin:
  State:                      (unknown)
  Digital reading:            255
RC pin:
  Digital reading:            255
END

describe '--status' do
  it 'can get the status', usb: true do
    stdout, stderr, result = run_ticcmd('--status')
    expect(stderr).to eq ''
    expect(result).to eq 0
    expect(stdout).to_not include '(unknown)'
    expect(stdout).to_not include '_'
    status = YAML.load(stdout)

    # These keys are not always in the output
    unreliable_keys = ["Planning mode", "Target velocity", "Target position"]

    actual_keys = status.keys.sort - unreliable_keys
    expected_keys = YAML.load(FakeStatus).keys.sort - unreliable_keys

    expect(actual_keys).to eq expected_keys
  end
end

describe 'print_status' do
  it 'can print a fake status for testing' do
    stdout, stderr, result = run_ticcmd('-d x --test 1')
    expect(stderr).to eq ''
    expect(result).to eq 0
    YAML.load(stdout)
    expect(stdout).to eq FakeStatus
  end
end
