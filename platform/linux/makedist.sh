# Run this script after building to make a distro-agnostic distributable binary package.

BUILDDIR=$1
DATADIR=$2

mkdir dist
mkdir dist/bin

cp $BUILDDIR/tiletel dist/bin
cp `ldd dist/bin/tiletel | grep -o '\W/[^ ]*'` dist/bin
cp $DATADIR/*.bdf $DATADIR/*.png $DATADIR/*.cfg $DATADIR/LICENSES $DATADIR/README dist

cat << "EOF" > dist/tiletel
#!/bin/bash
SCRIPT_PATH=$(dirname $(readlink -f $0))
$SCRIPT_PATH/bin/ld-*.so.2 --library-path $SCRIPT_PATH/bin $SCRIPT_PATH/bin/tiletel $* 
EOF

strip dist/bin/tiletel

chmod +x dist/tiletel

mv dist tiletel
tar -cvzf tiletel.tar.gz tiletel

