{
  lib,
  stdenv,
  fetchFromGitHub,
  cmake,
  pkg-config,
}:

stdenv.mkDerivation rec {
  pname = "mdpsan";
  version = "0.6.0";

  src = fetchFromGitHub {
    owner = "kokkos";
    repo = "mdspan";
    rev = "v${version}";
    hash = "";
  };

  nativeBuildInputs = [ cmake pkg-config ];

  meta = with lib; {
    description = "Reference implementation of mdspan targeting C++23 ";
    homepage = "https://github.com/kokkos/mdspan";
    license = licenses.asl20 ;
    platforms = platforms.all;
    maintainers = [ ];
  };
}
