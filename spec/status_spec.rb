
require_relative 'spec_helper'

FakeStatus = <<END
Name:                         Fake name
Serial number:                123
Firmware version:             9.99
Last reset:                   (Unknown)
Up time:                      4294967295 ms

Operation state:              (Unknown)
Energized:                    yes
Planning mode:                (Unknown)
Current position:             -1
Position uncertain:           yes
Current velocity:             -1
Max speed:                    4294967295
Starting speed:               4294967295
Max acceleration:             4294967295
Max deceleration:             4294967295
Acting target position:       -1
Time since last step:         4294967295

VIN:                          65535 mV
Encoder position:             -1
RC pulse width:               N/A
Step mode:                    (Unknown)
Current limit:                4294967295 mA
Decay mode:                   (Unknown)
Input state:                  (Unknown)
Input after averaging:        N/A
Input after hysteresis:       N/A
Input after scaling:          -1

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
  - (Unknown)
  - (Unknown)
  - (Unknown)
  - (Unknown)
  - (Unknown)
  - (Unknown)
  - (Unknown)
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
  - (Unknown)
  - (Unknown)
  - (Unknown)
  - (Unknown)
  - (Unknown)
  - (Unknown)
  - (Unknown)
  - Serial framing
  - Serial RX overrun
  - Serial format
  - Serial CRC
  - Encoder skip
  - (Unknown)
  - (Unknown)
  - (Unknown)
  - (Unknown)
  - (Unknown)
  - (Unknown)
  - (Unknown)
  - (Unknown)
  - (Unknown)
  - (Unknown)
  - (Unknown)

SCL pin:
  State:                      (Unknown)
  Digital reading:            255
SDA pin:
  State:                      (Unknown)
  Digital reading:            255
TX pin:
  State:                      (Unknown)
  Digital reading:            255
RX pin:
  State:                      (Unknown)
  Digital reading:            255
RC pin:
  Digital reading:            255
END

describe '--status' do
  it 'can get the status', usb: true do
    stdout, stderr, result = run_ticcmd('--status')
    expect(stderr).to eq ''
    expect(result).to eq 0
    expect(stdout).to_not include '(Unknown)'
    expect(stdout).to_not include '_'
    status = YAML.load(stdout)

    # These keys are not always in the output
    unreliable_keys = [
      "Planning mode", "Target velocity", "Target position",
      "Acting target position", "Time since last step",
    ]

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
