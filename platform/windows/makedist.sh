# Run this script after cross-platform building to bundle a Windows package.

BUILDDIR=$1
DATADIR=$2
MINGW=/usr/i686-w64-mingw32/


mkdir dist
cp $BUILDDIR/tiletel.exe dist/

$MINGW/bin/objdump -x dist/tiletel.exe | grep 'DLL Name: ' | grep -o ': .*' | grep -o '[^: ]*' | xargs -IX find $MINGW -name X | xargs -IX cp X dist/

$MINGW/bin/strip dist/tiletel.exe

cp $DATADIR/*.bdf $DATADIR/*.png $DATADIR/*.cfg $DATADIR/LICENSES dist


cat << "EOF" > dist/README.txt

Run 'tiletel' to launch the telnet client.
By default it will connect to host 'dungeon.name' port 20028.
You can give a host and port on the command line to connect to another server, or simply edit 'default.cfg'.

EOF

mv dist tiletel
zip -r tiletel tiletel

