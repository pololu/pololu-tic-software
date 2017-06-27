require_relative 'spec_helper'

describe 'commands for controlling the motor', usb: true do
  before(:all) do
    tic_change_settings do |s|
      s['control_mode'] = 'serial'
      s['speed_max'] = '500000000'
      s['disable_safe_start'] = false
    end

    # Get out of safe-start.
    stdout, stderr, result = run_ticcmd('--energize --exit-safe-start')
    expect(stderr).to eq ''
    expect(stdout).to eq ''
    expect(result).to eq 0
  end

  describe 'Set Target Position' do
    it 'lets you set the position' do
      stdout, stderr, result = run_ticcmd('-p 230000')
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0

      expect(tic_get_status['Target position']).to eq 230000

      stdout, stderr, result = run_ticcmd('--position -44')
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0

      expect(tic_get_status['Target position']).to eq -44
    end
  end

  describe 'Set Target Velocity' do
    it 'lets you set the velocity' do
      stdout, stderr, result = run_ticcmd('-y 100000')
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0

      expect(tic_get_status['Target velocity']).to eq 100000

      stderr, stdout, result = run_ticcmd('--velocity -1')
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0

      expect(tic_get_status['Target velocity']).to eq -1
    end
  end

  describe 'Halt and Set Position' do
    it 'lets you set the current position' do
      stdout, stderr, result = run_ticcmd('--halt-and-set-position 2146054486')
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0

      expect(tic_get_status['Current position']).to eq 2146054486

      stderr, stdout, result = run_ticcmd('--halt-and-set-position 10')
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0

      expect(tic_get_status['Current position']).to eq 10
    end
  end

  describe 'Halt and Hold' do
    it 'lets you clear a target position or velocity' do
      stdout, stderr, result = run_ticcmd('-p 230000')
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0

      expect(tic_get_status['Target position']).to eq 230000

      stdout, stderr, result = run_ticcmd('--halt-and-hold')
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0

      # Ruby converts 'off' to false
      expect(tic_get_status['Planning mode']).to eq false
    end
  end

  describe 'Reset Command Timeout' do
    it 'runs' do
      stdout, stderr, result = run_ticcmd('--reset-command-timeout')
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0
    end
  end

  describe 'Deenergize/energize' do
    it 'works' do
      stdout, stderr, result = run_ticcmd('--deenergize')
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0

      errors = tic_get_status['Errors currently stopping the motor']
      expect(errors).to be_include 'Intentionally de-energized'

      stdout, stderr, result = run_ticcmd('--energize')
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0

      errors = tic_get_status['Errors currently stopping the motor']
      expect(errors).to_not be_include 'Intentionally de-energized'
    end
  end

  describe 'Exit Safe Start' do
    it 'runs' do
      stdout, stderr, result = run_ticcmd('--exit-safe-start')
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0
    end
  end

  describe 'Enter Safe Start' do
    it 'runs' do
      stdout, stderr, result = run_ticcmd('--enter-safe-start')
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0
    end
  end

  describe 'Reset' do
    it 'runs' do
      stdout, stderr, result = run_ticcmd('--reset')
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0
    end
  end

  describe 'Clear Driver Error' do
    it 'runs' do
      stdout, stderr, result = run_ticcmd('--clear-driver-error')
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0
    end
  end
end

describe 'Set Speed Max' do
  it 'works', usb: true do
    ['10000', '2000000']. each do |mode|
      stdout, stderr, result = run_ticcmd("--speed-max #{mode}")
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0
      expect(tic_get_status['Speed max'].to_s).to eq mode
    end
  end
end

describe 'Set Speed Min' do
  it 'works', usb: true do
    ['512', '0']. each do |mode|
      stdout, stderr, result = run_ticcmd("--speed-min #{mode}")
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0
      expect(tic_get_status['Speed min'].to_s).to eq mode
    end
  end
end

describe 'Set Accel Max' do
  it 'works', usb: true do
    ['10000', '2000000']. each do |mode|
      stdout, stderr, result = run_ticcmd("--accel-max #{mode}")
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0
      expect(tic_get_status['Accel max'].to_s).to eq mode
    end
  end
end

describe 'Set Decel Max' do
  it 'works', usb: true do
    ['10000', '2000000']. each do |mode|
      stdout, stderr, result = run_ticcmd("--decel-max #{mode}")
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0
      expect(tic_get_status['Decel max'].to_s).to eq mode
    end
  end
end

describe 'Set Step Mode' do
  it 'it complains if the step mode is invalid' do
    stdout, stderr, result = run_ticcmd('-d x --step-mode foobar')
    expect(stderr).to eq "Error: The step mode specified is invalid.\n"
    expect(stdout).to eq ''
    expect(result).to eq EXIT_BAD_ARGS
  end

  it 'it works', usb: true do
    ['1', '2', '4', '8', '16', '32']. each do |mode|
      stdout, stderr, result = run_ticcmd("--step-mode #{mode}")
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0
      expect(tic_get_status['Step mode'].to_s).to eq mode
    end
  end
end


describe 'Set Current Limit command' do
  it 'it prevents you from setting a current too high' do
    stdout, stderr, result = run_ticcmd('-d x --current 10000')
    expect(stderr).to eq \
      "Warning: The current limit was too high so it will be lowered to 3968 mA.\n" \
      "Error: No device was found with serial number 'x'.\n"
    expect(stdout).to eq ''
    expect(result).to eq EXIT_DEVICE_NOT_FOUND
  end

  it 'it works', usb: true do
    [32, 64]. each do |limit|
      stdout, stderr, result = run_ticcmd("--current #{limit}")
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0
      expect(tic_get_status['Current limit']).to eq "#{limit} mA"
    end
  end
end

describe 'Set Decay Mode' do
  it 'it complains if the decay mode is invalid' do
    stdout, stderr, result = run_ticcmd('-d x --decay foobar')
    expect(stderr).to eq "Error: The decay mode specified is invalid.\n"
    expect(stdout).to eq ''
    expect(result).to eq EXIT_BAD_ARGS
  end

  it 'it works', usb: true do
    ['fast', 'slow', 'mixed']. each do |mode|
      stdout, stderr, result = run_ticcmd("--decay #{mode}")
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0
      expect(tic_get_status['Decay mode']).to eq mode
    end
  end
end


