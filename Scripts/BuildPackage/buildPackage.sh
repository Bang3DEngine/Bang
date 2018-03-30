#!/bin/bash

cd "$(dirname "$0")"

mkdir -p build
cd build
NEWPATH="$PATH:/usr/lib/x86_64-linux-gnu/"
PATH=$NEWPATH cmake -DCMAKE_BUILD_TYPE=Release ..
PATH=$NEWPATH make -j6
