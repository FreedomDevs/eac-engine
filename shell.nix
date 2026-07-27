{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell.override { stdenv = pkgs.clangStdenv; } {
  nativeBuildInputs = with pkgs; [
    cmake
    ninja

    llvmPackages.clang
    llvmPackages.clang-tools

    pkg-config
    gdb

    (python3.withPackages (ps: with ps; [
      nbtlib
    ]))
  ];
}
