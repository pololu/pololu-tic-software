require 'fileutils'
require 'pathname'
include FileUtils

ENV['PATH'] = ENV.fetch('_PATH')

EnvName = ENV.fetch('env_name')
OutDir = Pathname(ENV.fetch('out'))
PayloadDir = Pathname(ENV.fetch('payload'))
SrcDir = Pathname(ENV.fetch('src'))
Version = File.read(PayloadDir + 'version.txt')

StagingDir = OutDir + 'pololu-tic'

mkdir_p StagingDir
cp_r Dir.glob(PayloadDir + 'bin' + '*'), StagingDir
cp_r SrcDir + 'udev-rules' + '99-pololu.rules', StagingDir
cp ENV.fetch('license'), StagingDir + 'LICENSE.html'

File.open(StagingDir + 'README.txt', 'w') do |f|
  f.puts <<EOF
Pololu Tic Stepper Motor Controller Software #{Version} for #{EnvName}

To install this software, we recommend starting a terminal, navigating to this
directory using the "cd" command, and then running:

  sudo ./install.sh

The install.sh script will install files and directories to the following
locations on your system:

  /usr/local/bin/ticcmd
  /usr/local/bin/ticgui
  /usr/local/bin/pololu-tic/
  /etc/udev/rules.d/99-pololu.rules

For more information, see the Tic Stepper Motor Controller User's Guide:

  https://www.pololu.com/docs/0J71
EOF
end

File.open(StagingDir + 'install.sh', 'w') do |f|
  f.puts <<EOF
#!/usr/bin/env sh
set -ue
cd "$(dirname "$0")"
rm -vrf /usr/local/share/pololu-tic
mkdir -v /usr/local/share/pololu-tic
cp ticcmd ticgui *.ttf LICENSE.html /usr/local/share/pololu-tic
ln -vsf /usr/local/share/pololu-tic/ticcmd /usr/local/bin/
ln -vsf /usr/local/share/pololu-tic/ticgui /usr/local/bin/
cp -v 99-pololu.rules /etc/udev/rules.d/
EOF
end

chmod_R 'u+w', StagingDir
chmod 'u+x', StagingDir + 'install.sh'

cd OutDir
success = system("tar cJfv pololu-tic-#{Version}-#{EnvName}.tar.xz pololu-tic")
raise "tar failed: error #{$?.exitstatus}" if !success
