{ stdenv
, cmake
, gd
, nix-gitignore
} :
stdenv.mkDerivation {
  name = "tiletel";
  version = "20220404";
  src = nix-gitignore.gitignoreSourcePure [ ./.gitignore ] ./.;
  nativeBuildInputs = [ cmake gd ];
  cmakeFlags = "-DTOOLKIT=web";
}
