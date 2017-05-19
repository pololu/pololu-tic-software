require_relative 'spec_helper'

FakeStatus = <<END
Name:                         Fake name
Serial number:                123
Firmware version:             9.99
Last reset:                   (unknown)

Operation state:              (unknown)

Errors currently stopping the motor:
  - Safe start violation
  - Required input invalid
  - Serial error
  - Command timeout
  - Motor driver error
  - Low VIN
  - High VIN
  - ERR line high
  - Kill switch
  - Intentionally disabled
  - Intentionally disabled from USB
  - (unknown)
  - (unknown)
  - (unknown)
  - (unknown)
  - (unknown)

Errors that occurred since last check:
  - Safe start violation
  - Required input invalid
  - Serial error
  - Command timeout
  - Motor driver error
  - Low VIN
  - High VIN
  - ERR line high
  - Kill switch
  - Intentionally disabled
  - Intentionally disabled from USB
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
Target position:              -1
Target velocity:              -1
Speed min:                    4294967295
Speed max:                    4294967295
Decel max:                    4294967295
Accel max:                    4294967295
Current position:             -1
Current velocity:             -1
Acting target position:       -1
Time since last step:         -1

VIN:                          65535 mV
Up time:                      4294967295 ms
Encoder position:             -1
RC pulse width:               65535
Step mode:                    (unknown)
Decay mode:                   (unknown)

SCL pin:
  Digital reading:            255
  Switch status:              255
  Analog reading:             65535
  State:                      (unknown)
SDA pin:
  Digital reading:            255
  Switch status:              255
  Analog reading:             65535
  State:                      (unknown)
TX pin:
  Digital reading:            255
  Switch status:              255
  Analog reading:             65535
  State:                      (unknown)
RX pin:
  Digital reading:            255
  Switch status:              255
  Analog reading:             65535
  State:                      (unknown)
RC pin:
  Digital reading:            255
  Switch status:              255
  State:                      (unknown)
END

describe '--status' do
  it 'can get the status', usb: true do
    stdout, stderr, result = run_ticcmd('--status')
    expect(stderr).to eq ''
    expect(result).to eq 0
    expect(stdout).to_not include '(unknown)'
    expect(stdout).to_not include '_'
    status = YAML.load(stdout)
    expect(status.keys.sort).to eq YAML.load(FakeStatus).keys.sort
  end
end

describe 'print_status' do
  it 'can print a fake status for testing' do
    stdout, stderr, result = run_ticcmd('--test 1')
    expect(stderr).to eq ''
    expect(result).to eq 0
    YAML.load(stdout)
    expect(stdout).to eq FakeStatus
  end
end
