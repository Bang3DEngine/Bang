#!/bin/bash

cd "$(dirname "$0")"

rm -rf Bang

mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF ..
if [ $? -ne 0 ] ; then exit 1 ; fi

make VERBOSE=1 -j6
if [ $? -ne 0 ] ; then exit 1 ; fi

cd ..
tar -czvf Bang.tar.gz Bang/
if [ $? -ne 0 ] ; then exit 1 ; fi

rm -rf Bang
