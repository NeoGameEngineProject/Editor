#!/bin/sh -e

mkdir -p build-appimage
cd build-appimage

rm -rf AppDir

cmake .. -DCMAKE_BUILD_TYPE=Release -DNO_TESTS=TRUE -DCMAKE_INSTALL_PREFIX=/usr
make -j$(nproc)
make install DESTDIR=$PWD/AppDir

export LD_LIBRARY_PATH=$PWD/AppDir/usr/lib/
../linuxdeploy.AppImage --appdir AppDir --output appimage -d ../scripts/NeoEditor.desktop -i ../scripts/neo-icon.png --plugin qt

cd ..
