{
  pkgs ? import (fetchTarball "https://github.com/NixOS/nixpkgs/archive/21.11.tar.gz") {}
} :
with pkgs;
let
  tiletel = callPackage ./default.nix { };
in
mkShell { 
  buildInputs = [ glibcLocales tiletel ];
  LOCALE_ARCHIVE_2_27 = "${glibcLocales}/lib/locale/locale-archive";
}
