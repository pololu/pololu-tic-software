#!/usr/bin/env ruby

# This is a Ruby script that uses Git and NIX_PATH to figure out the commit
# numbers of the important repositories used to build this code.  It then sets
# the appropriate environment variables and calls nix-build to build an
# installer for the specified operating system.

require 'pathname'

if ARGV.empty?
  $stderr.puts "usage: #{$PROGRAM_NAME} ATTRPATH"
  $stderr.puts "where ATTRPATH is one of installers, win32.installer, linux-x86.installer, etc."
  exit 1
end

attr_name = ARGV.shift

# Read in the NIX_PATH as a hash.
$nix_path = {}
ENV.fetch('NIX_PATH', '').split(":").each do |entry|
  if md = entry.match(/(\w+)=(.+)/)
    $nix_path[md[1]] = md[2]
  end
end

def get_git_commit(name)
  if name.start_with?('.')
    path = name
  else
    path = $nix_path.fetch(name, File.expand_path("~/.nix-defexpr/channels/#{name}"))
  end
  path = Pathname(path)
  if File.exist?(path + '.git-revision')
    return File.read(path + '.git-revision')
  end
  if File.exist?(path + '.git')
    return %x(git -C #{path} rev-parse HEAD).strip
  end
  raise "unable to get git commit for #{path}"
end

env = {}
env['commit'] = get_git_commit('.')
env['nixcrpkgs_commit'] = get_git_commit('nixcrpkgs')
env['nixpkgs_commit'] = get_git_commit('nixpkgs')
cmd = "nix-build -A #{attr_name}"
exec(env, cmd)
