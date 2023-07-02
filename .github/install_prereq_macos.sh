#!/bin/sh

mkdir tmp
cd tmp

brew install autoconf automake

# SDL2
curl --get https://www.libsdl.org/release/SDL2-2.28.1.dmg --output SDL2.dmg
mkdir sdltmp
hdiutil attach -mountpoint sdltmp -readonly SDL2.dmg
cp -R sdltmp/SDL2.framework ../src/Unix/MacOSX/SDL2.Framework
hdiutil detach sdltmp

cd ..

# we must install the macports version of the dependencies,
# because the brew packages are not universal
mkdir -p src/Unix/MacOSX/external
for i in gmp/gmp-6.1.2_3+universal.darwin_16.i386-x86_64.tbz2 \
    mpfr/mpfr-4.0.2_1.darwin_15.x86_64.tbz2 \
    jpeg/jpeg-9d_0+universal.darwin_16.i386-x86_64.tbz2; do
    f=`basename $i`
    curl --get https://tho-otto.de/download/macports/$f --output - | bzip2 -dc | tar -C src/Unix/MacOSX/external --strip-components=3 -xf -
done
cd src/Unix/MacOSX/external/lib
rm -f libjpeg.dylib
for i in .7 .8 .9; do
    test -f libjpeg${i}.dylib && ln libjpeg${i}.dylib libjpeg.dylib
done
rm -f libgmp.dylib
for i in .10; do
    test -f libgmp{$i}.dylib && ln libgmp{$i}.dylib libgmp.dylib
done
rm -f libgmpxx.dylib
for i in .4; do
    test -f libgmpxx${i}.dylib && ln libgmpxx${i}.dylib libgmpxx.dylib
done
rm -f libmpfr.dylib
for i in .4 .6; do
    test -f libmpfr${i}.dylib && ln libmpfr${i}.dylib libmpfr.dylib
done
cd ..

# cleanup tmp
rm -rf tmp