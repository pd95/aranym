#!/bin/sh
set -e

SDL2_VERSION=
DEPENDENCIES_DATE=2023-07-02

mkdir tmp
cd tmp

brew install autoconf automake

# Download additional dependencies (libjpeg, libgmp, libgmpxx, libmpfr) as universal binaries (not available through homebrew)
curl -s --get https://xn--donz-epa.ch/atari/software/macaranym/Dependencies-$DEPENDENCIES_DATE.zip --output dependencies.zip
unzip -o dependencies.zip -d ../src/Unix/MacOSX/
cd ..

# Optionally add specific SDL2 version
if [ "x$SDL2_VERSION" != "x" ]; then
    curl -s --get https://www.libsdl.org/release/SDL2-$SDL2_VERSION.dmg --output SDL2.dmg
    mkdir sdltmp
    hdiutil attach -mountpoint sdltmp -readonly SDL2.dmg
    cp -R sdltmp/SDL2.framework ../src/Unix/MacOSX/SDL2.Framework
    hdiutil detach sdltmp
fi

# cleanup tmp
rm -rf tmp