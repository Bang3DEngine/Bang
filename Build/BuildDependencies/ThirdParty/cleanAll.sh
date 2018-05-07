#!/bin/bash

cd "$(dirname "$0")"

cd SDL2-2.0.8
rm -rf build
rm -rf sdl2-install
cd ..

cd SDL2_ttf-2.0.14/
make distclean
rm -rf sdl2-ttf-install
cd ..

cd freetype-2.4.0
make distclean
rm -rf freetype-install
cd ..

cd assimp
rm -rf build
cd ..

cd glew-2.1.0
make distclean
cd ..

cd libjpeg-turbo
rm -rf build jpeg-install
cd ..

cd libpng-1.6.34
rm -rf build png-install
cd ..

cd openal-soft
rm -rf build
cd ..

cd sndio
make clean 
make distclean
rm -rf build
cd ..

cd minizip
rm -rf build
cd ..

cd alsa-lib-1.1.4
make clean
make distclean
rm -rf build
cd ..
