{ pkgs ? import <nixpkgs> {} }:
let
  mdpsan = pkgs.callPackage ./nix/mdspan/package.nix { };
in
pkgs.stdenv.mkDerivation rec {
  pname = "emu";
  version = "14e7bb4ae24616ec54ff6b2f6ef4e8659434ea44";

  src = ./.;

  buildInputs = with pkgs; [
    cmake
    ninja
    fmt
    boost
    microsoft-gsl
    mdpsan
  ];


  # preBuild = ''
  #   export HOME=$(mktemp -d)
  # '';

  # configurePhase = ''
  #   export HOME=$(mktemp -d)
  #   conan remote update conancenter --url="https://center2.conan.io"
  #   conan profile detect --force
  #   conan install . --build=missing
  # '';

  # buildPhase = ''
  #   cmake --build --preset conan-release
  # '';

  # installPhase = ''
  #   cmake --install --preset conan-release
  # '';

  enableParallelBuilding = true;
}
