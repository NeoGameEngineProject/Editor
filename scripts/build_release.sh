#!/bin/sh -e

scripts/build_linux_kits.sh
mkdir -p build-linux64-clang
cd build-linux64-clang

CC=clang CXX=clang++ cmake .. -DCMAKE_BUILD_TYPE=Release -DNO_TESTS=TRUE -DINCLUDE_KITS=TRUE
make -j$(nproc) package
