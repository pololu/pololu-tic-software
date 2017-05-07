require 'rspec'
require 'open3'
require 'yaml'

Valgrind = false

def run_ticcmd(args, opts = {})
  env = {}
  cmd = 'ticcmd ' + args.to_s
  cmd = 'valgrind ' + cmd if Valgrind
  open3_opts = {}
  open3_opts[:stdin_data] = opts[:input] if opts.has_key?(:input)
  stdout, stderr, status = Open3.capture3(env, cmd, open3_opts)

  stdout.gsub!("\r\n", "\n")
  stderr.gsub!("\r\n", "\n")

  [stdout, stderr, status.exitstatus]
end

