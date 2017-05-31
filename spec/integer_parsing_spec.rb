require_relative 'spec_helper'

describe 'int32_t parsing' do
  def test_reject(num)
    stdout, stderr, result = run_ticcmd("-d x -p #{num}")
    expect(stdout).to eq ''
    expect(stderr).to eq "Error: The number after '-p' is invalid.\n"
    expect(result).to eq 1
  end

  def test_accept(num)
    stdout, stderr, result = run_ticcmd("-d x -p #{num}")
    expect(stdout).to eq ''
    expect(stderr).to eq "Error: No device was found with serial number 'x'.\n"
    expect(result).to eq 3
  end

  it 'accepts 0' do
    test_accept(0)
  end

  it 'accepts the minimum' do
    test_accept(-2147483648)
  end

  it 'rejects the minium minus 1' do
    test_reject(-2147483648 - 1)
  end

  it 'accepts the maximum' do
    test_accept(2147483647)
  end

  it 'rejects the minium plus 1' do
    test_reject(2147483647 + 1)
  end
end
