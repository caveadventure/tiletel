{
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";
  outputs = { self, nixpkgs }:
    let
      packages = system:
        let
          pkgs = import nixpkgs { inherit system; };
        in {
          default = with pkgs; stdenv.mkDerivation {
            name = "tiletel";
            version = "20220404";
            src = nix-gitignore.gitignoreSourcePure [ ./.gitignore ] ./.;
            nativeBuildInputs = [ cmake gd ];
            cmakeFlags = [ "-DTOOLKIT=web" ];
          };
        };
    in {
      packages.x86_64-linux = packages "x86_64-linux";
    };
}
