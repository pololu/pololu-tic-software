source $setup

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
This document contains the license information for the Pololu Tic software and drivers and the dependencies that are distributed with the software.
</p>
EOF
  cat $license_fragments
} > $out/LICENSE.html
