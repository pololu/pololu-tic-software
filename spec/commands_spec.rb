require_relative 'spec_helper'

describe 'commands for controlling the motor', usb: true do
  before(:all) do
    tic_change_settings do |s|
      s['control_mode'] = 'serial'
      s['speed_max'] = '500000000'
    end
  end

  describe 'Stop command' do
    it 'lets you clear a target position or velocity' do
      stdout, stderr, result = run_ticcmd('-p 230000')
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0

      expect(tic_get_status['Target position']).to eq 230000

      stdout, stderr, result = run_ticcmd('--stop')
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0

      # Ruby converts 'off' to false
      expect(tic_get_status['Planning mode']).to eq false
    end
  end

  describe 'Set Target Position command' do
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

  describe 'Set Target Velocity command' do
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

  describe 'Enable/Disable driver' do
    it 'works' do
      stdout, stderr, result = run_ticcmd('--disable-driver')
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0

      errors = tic_get_status['Errors currently stopping the motor']
      expect(errors).to be_include 'Intentionally disabled'

      stdout, stderr, result = run_ticcmd('--enable-driver')
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0

      errors = tic_get_status['Errors currently stopping the motor']
      expect(errors).to_not be_include 'Intentionally disabled'
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


