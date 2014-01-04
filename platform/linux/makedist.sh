# Run this script after building to make a distro-agnostic distributable binary package.

BUILDDIR=$1
DATADIR=$2

mkdir dist
mkdir dist/bin

cp $BUILDDIR/tiletel dist/bin
cp `ldd dist/bin/tiletel | grep -o '\W/[^ ]*'` dist/bin
cp $DATADIR/*.bdf $DATADIR/*.png $DATADIR/*.cfg $DATADIR/LICENSES dist

cat << "EOF" > dist/tiletel
#!/bin/bash
SCRIPT_PATH=$(dirname $(readlink -f $0))
$SCRIPT_PATH/bin/ld-*.so.2 --library-path $SCRIPT_PATH/bin $SCRIPT_PATH/bin/tiletel $* 
EOF

strip dist/bin/tiletel

chmod +x dist/tiletel

cat << "EOF" > dist/README

This is a portable, OS-agnostic telnet client.
It is based on SDL2 and includes support for all 65535 Unicode characters, including the Asian ones.
It is meant for use as a game client, but will work as a generic telnet client too.

Edit 'default.cfg' to change options.

EOF

mv dist tiletel
tar -cvzf tiletel.tar.gz tiletel

