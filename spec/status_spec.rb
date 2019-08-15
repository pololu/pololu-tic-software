
require_relative 'spec_helper'

FakeStatus = <<END
Name:                         Fake name
Serial number:                123
Firmware version:             9.99
Last reset:                   (Unknown)
Up time:                      1193:02:47

Encoder position:             -1
RC pulse width:               N/A
Input state:                  (Unknown)
Input after averaging:        N/A
Input after hysteresis:       N/A
Input after scaling:          -1
Forward limit active:         Yes
Reverse limit active:         Yes

VIN voltage:                  65.535 V
Operation state:              (Unknown)
Energized:                    Yes
Homing active:                Yes

Target:                       No target
Current position:             -1
Position uncertain:           Yes
Current velocity:             -1
Max speed:                    4294967295
Starting speed:               4294967295
Max acceleration:             4294967295
Max deceleration:             4294967295
Acting target position:       -1
Time since last step:         4294967295
Step mode:                    (Unknown)
Current limit:                0 mA

Errors currently stopping the motor:
  - Intentionally de-energized
  - Motor driver error
  - Low VIN
  - Kill switch active
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
  - Kill switch active
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
  Analog reading:             N/A
  Digital reading:            1
SDA pin:
  State:                      (Unknown)
  Analog reading:             N/A
  Digital reading:            1
TX pin:
  State:                      (Unknown)
  Analog reading:             N/A
  Digital reading:            1
RX pin:
  State:                      (Unknown)
  Analog reading:             N/A
  Digital reading:            1
RC pin:
  Digital reading:            1

END

describe '--status' do
  it 'can get the status', usb: true do
    stdout, stderr, result = run_ticcmd('--status')
    expect(stderr).to eq ''
    expect(result).to eq 0
    expect(stdout).to_not include '(Unknown)'
    expect(stdout).to_not include '_'
    status = YAML.load(stdout)
  end

  it 'can get the full status', usb: true do
    stdout, stderr, result = run_ticcmd('--status --full')
    expect(stderr).to eq ''
    expect(result).to eq 0
    expect(stdout).to_not include '(Unknown)'
    expect(stdout).to_not include '_'
    status = YAML.load(stdout)

    # These keys are not always in the output
    unreliable_keys = [
      "Target", "Target velocity", "Target position",
      "Input before scaling",
    ]

    actual_keys = status.keys.sort
    expected_keys = YAML.load(FakeStatus).keys.sort
    unless %i(T825 N825 T834).include?(tic_product)
      expected_keys.delete("Decay mode")
    end
    if tic_product == :T249
      expected_keys << 'AGC bottom current limit'
      expected_keys << 'AGC current boost steps'
      expected_keys << 'AGC frequency limit'
      expected_keys << 'AGC mode'
      expected_keys << 'Last motor driver error'
    end
    if tic_product == :'36v4'
      expected_keys << 'Last motor driver errors'
    end

    unexpected_keys = actual_keys - expected_keys - unreliable_keys
    expect(unexpected_keys).to eq []

    missing_keys = expected_keys - actual_keys - unreliable_keys
    expect(missing_keys).to eq []
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
