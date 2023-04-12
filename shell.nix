{
  # nixpkgs 22.11, deterministic. Last updated: 2023-04-12.
  pkgs ? import (fetchTarball ("https://github.com/NixOS/nixpkgs/archive/115a96e2ac1e92937cd47c30e073e16dcaaf6247.tar.gz")) { }
}:

with pkgs;
let
  my-python-packages = python-packages: with python-packages; [
    pyserial
    crccheck
    coloredlogs
    ipython
  ];
  python-with-my-packages = python3.withPackages my-python-packages;
in
mkShell {
  buildInputs = [
    gnumake
    gcc-arm-embedded-10
    clang-tools
    python-with-my-packages
    openocd
  ];
}
