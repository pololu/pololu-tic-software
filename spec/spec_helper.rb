require 'rspec'
require 'open3'
require 'yaml'

Valgrind = false

def run_ticcmd(args)
  env = { }
  cmd = 'ticcmd ' + args.to_s
  cmd = 'validgrind ' + cmd if Valgrind
  opts = { }
  stdout, stderr, status = Open3.capture3(env, cmd, opts)

  stdout.gsub!("\r\n", "\n")

  [stdout, stderr, status.exitstatus]
end

