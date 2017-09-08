rec {
  nixcrpkgs = import <nixcrpkgs>;

  src = nixcrpkgs.filter ./.;

  build = env: env.make_derivation {
    builder = ./nix/builder.sh;
    inherit src;
    cross_inputs = [ env.libusbp env.qt ];
    dejavu = (if env.os == "linux" then env.dejavu-fonts else null);
  };

  build_license = env:
    let
      license_set =
        (if env.os == "linux" then env.dejavu-fonts.license_set else {}) //
        env.libusbp.license_set //
        env.qt.license_set //
        env.global_license_set;
    in
      env.native.make_derivation {
        name = "license";
        builder.ruby = ./nix/license_builder.rb;
        src = win32.src;
        commit = builtins.getEnv "commit";
        nixcrpkgs_commit = builtins.getEnv "nixcrpkgs_commit";
        nixpkgs_commit = builtins.getEnv "nixpkgs_commit";
        license_names = builtins.attrNames license_set;
        licenses = builtins.attrValues license_set;
      };

  win32 = build nixcrpkgs.win32;
  linux32 = build nixcrpkgs.linux32;
  rpi = build nixcrpkgs.rpi;

  win32_installer = nixcrpkgs.win32.make_derivation {
    name = "win32-installer";
    builder.ruby = ./nix/windows_installer_builder.rb;
    payload = build nixcrpkgs.win32;
    license = build_license nixcrpkgs.win32;
    inherit src;
    libusbp = nixcrpkgs.win32.libusbp;
  };

  build_linux_installer = env: env_name:
    env.make_derivation {
      name = "${env_name}-installer";
      builder.ruby = ./nix/linux_installer_builder.rb;
      payload = build env;
      license = build_license env;
      inherit src env_name;
    };

  linux32_installer = build_linux_installer nixcrpkgs.linux32 "linux32";
  rpi_installer = build_linux_installer nixcrpkgs.linux32 "rpi";
}
