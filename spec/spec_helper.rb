require 'rspec'
require 'open3'
require 'yaml'

def run_ticcmd(args, opts = {})
  env = {}
  cmd = 'ticcmd ' + args.to_s
  cmd = 'valgrind ' + cmd if ENV['TIC_SPEC_VALGRIND'] == 'Y'
  open3_opts = {}
  open3_opts[:stdin_data] = opts[:input] if opts.has_key?(:input)
  stdout, stderr, status = Open3.capture3(env, cmd, open3_opts)
  stdout.gsub!("\r\n", "\n")
  stderr.gsub!("\r\n", "\n")
  [stdout, stderr, status.exitstatus]
end

def tic_change_settings
  stdout, stderr, result = run_ticcmd("--get-settings -")
  if result != 0 || !stderr.empty?
    raise "Failed to get settings.  stderr=#{stderr.inspect}, result=#{result}"
  end
  settings = YAML.load(stdout)
  yield settings
  stdout, stderr, result = run_ticcmd("--set-settings -", input: YAML.dump(settings))
  if result != 0 || !stderr.empty?
    raise "Failed to set settings.  stderr=#{stderr.inspect}, result=#{result}"
  end
end

def tic_get_status
  stdout, stderr, result = run_ticcmd("-s")
  if result != 0 || !stderr.empty?
    raise "Failed to get status.  stderr=#{stderr.inspect}, result=#{result}"
  end
  YAML.load(stdout)
end
