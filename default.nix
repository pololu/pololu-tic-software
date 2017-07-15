rec {
  nixcrpkgs = import <nixcrpkgs>;
  build = env: env.make_derivation rec {
    builder = ./nix/builder.sh;
    src = nixcrpkgs.filter ./.;
    cross_inputs = [ env.libusbp env.qt ];
  };

  win32 = build nixcrpkgs.win32;

  windows_installer = nixcrpkgs.win32.make_derivation rec {
    builder = ./nix/windows_installer_builder.sh;
    payload = win32;
    drivers = nixcrpkgs.filter ./drivers;
    license_fragments = [
      nixcrpkgs.win32.global_license_fragment
      nixcrpkgs.win32.libusbp.license_fragment
      nixcrpkgs.win32.qt.license_fragment
    ];
  };
}
