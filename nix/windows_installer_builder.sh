source $setup

if [ -z "$commit" -o -z "$nixcrpkgs_commit" -o -z "$nixpkgs_commit" ]; then
  echo "commit info is missing (run \`nix/build_installer.rb windows\`)"
  exit 1
fi

mkdir $out
cp -r $payload/bin $out/
cp -r $drivers $out/drivers

{
  cat <<EOF
<!DOCTYPE html>
<html>
<head>
<style>
pre {
  padding-left: 5em;
  padding-bottom: 1em;
  border-bottom: 1em solid #DDD;
}
</style>
<meta charset="utf-8">
<title>License Information</title>
<body>
<p>
  This document contains the license information for this software release.
</p>
<h2>Source code</h2>
<p>
  Source code and instructions for building this software can be found in the
  <a href="https://github.com/pololu/pololu-tic-software">pololu-tic-software</a> repository.
</p>
<p>
  This release was built from
  <a href="https://github.com/pololu/pololu-tic-software/tree/$commit">
  pololu-tic-software commit $commit</a>,
  <a href="https://github.com/pololu/nixcrpkgs/tree/$nixcrpkgs_commit">
  nixcrpkgs commit $nixcrpkgs_commit</a>, and
  <a href="https://github.com/pololu/nixcrpkgs/tree/$nixpkgs_commit">
  nixpkgs commit $nixpkgs_commit</a>.
</p>
<p>
  Notable library versions:
</p>
<ul>
  <li>libusbp - $libusbp_version
  <li>qt - $qt_version
</ul>
EOF
  cat $license_fragments
} > $out/LICENSE.html
