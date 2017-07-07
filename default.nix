rec {
  nixcrpkgs = import <nixcrpkgs>;
  build = env: env.make_derivation rec {
    builder = ./nix/builder.sh;
    src = nixcrpkgs.filter ./.;
    cross_inputs = [ env.libusbp env.qt.base ];
    CMAKE_PREFIX_PATH = "${env.qt.base}";
  };
  win32 = build nixcrpkgs.win32;
}
