#!/bin/sh

SDL2_VERSION=2.28.1
DEPENDENCIES_DATE=2023-07-02

mkdir tmp
cd tmp

brew install autoconf automake

# SDL2
curl -s --get https://www.libsdl.org/release/SDL2-$SDL2_VERSION.dmg --output SDL2.dmg
mkdir sdltmp
hdiutil attach -mountpoint sdltmp -readonly SDL2.dmg
cp -R sdltmp/SDL2.framework ../src/Unix/MacOSX/SDL2.Framework
hdiutil detach sdltmp

curl -s --get https://xn--donz-epa.ch/atari/software/macaranym/Dependencies-$DEPENDENCIES_DATE.zip --output dependencies.zip
unzip -o dependencies.zip -d ../src/Unix/MacOSX/
mv external ../src/Unix/MacOSX/external
cd ..

# cleanup tmp
rm -rf tmp