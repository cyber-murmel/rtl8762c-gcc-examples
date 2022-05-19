{
  pkgs ?
  # nixpkgs 21.11, deterministic. Last updated: 2022-01-01.
  import (fetchTarball("https://github.com/NixOS/nixpkgs/archive/8a053bc.tar.gz")) {}
}:

with pkgs;
let
  my-python-packages = python-packages: with python-packages; [
    pyserial crccheck coloredlogs
  ];
  python-with-my-packages = python3.withPackages my-python-packages;

  openocd_0_10_0 = (import (fetchTarball("https://github.com/NixOS/nixpkgs/archive/20.09.tar.gz")) {}).openocd;
in
mkShell {
  buildInputs = [
    gnumake
    gcc-arm-embedded-9
    clang-tools
    python-with-my-packages
    openocd_0_10_0
  ];
}
