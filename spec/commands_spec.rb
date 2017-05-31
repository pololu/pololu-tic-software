require_relative 'spec_helper'

describe 'commands for controlling the motor', usb: true do
  before(:all) do
    tic_change_settings do |s|
      s['control_mode'] = 'serial'
      s['speed_max'] = '500000000'
    end
  end

  describe 'Set Target Position command' do
    it 'lets you set the position' do
      stdout, stderr, result = run_ticcmd('-p 230000')
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0

      expect(tic_get_status['Target position']).to eq 230000

      run_ticcmd('--position -44')
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

      run_ticcmd('--velocity -1')
      expect(stderr).to eq ''
      expect(stdout).to eq ''
      expect(result).to eq 0

      expect(tic_get_status['Target velocity']).to eq -1
    end
  end
end
