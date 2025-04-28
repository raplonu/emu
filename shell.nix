{ pkgs ? import <nixpkgs>{} }:
pkgs.mkShell {
  buildInputs = with pkgs; [
    # Add your development dependencies here
    gcc
    gnumake
  ];
  shellHook = ''
    echo "hello world"
  '';
}
