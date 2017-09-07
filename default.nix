rec {
  nixcrpkgs = import <nixcrpkgs>;

  build = env: env.make_derivation rec {
    builder = ./nix/builder.sh;
    src = nixcrpkgs.filter ./.;
    cross_inputs = [ env.libusbp env.qt ];
    dejavu = env.dejavu-fonts;
  };

  win32 = build nixcrpkgs.win32;
  linux32 = build nixcrpkgs.linux32;
  rpi = build nixcrpkgs.rpi;

  license_data = env: {
    commit = builtins.getEnv "commit";
    nixcrpkgs_commit = builtins.getEnv "nixcrpkgs_commit";
    nixpkgs_commit = builtins.getEnv "nixpkgs_commit";
    qt_version = env.qt.version;
    libusbp_version = env.libusbp.version;
    license_fragments = [
      env.global_license_fragment
      env.libusbp.license_fragment
      env.qt.license_fragment
    ];
  };

  windows_installer = nixcrpkgs.win32.make_derivation(rec {
    builder.ruby = ./nix/windows_installer_builder.rb;
    src = win32.src;
    payload = win32;
    libusbp = nixcrpkgs.win32.libusbp;
  } // license_data nixcrpkgs.win32);
}
