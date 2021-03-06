#!/bin/bash
set -e

WIN_MKARGS="CC=x86_64-w64-mingw32-gcc CXX=x86_64-w64-mingw32-g++ AR=x86_64-w64-mingw32-ar RANLIB=x86_64-wi64-mingw32-ranlib CXXFLAGS=-std=c++11"

pushd .
cd ext/putki/putked/interopdll
premake5 --os=windows gmake
make -f interopjava.make clean
make -f interopjava.make $WIN_MKARGS
premake5 gmake
sed -i .bak 's/\-Wl\,\-x//g' *.make
make -f interopjava.make clean
make -f interopjava.make
popd

premake5 --os=windows gmake
cd build
sed -i .bak 's/lws2_32\.lib/lws2_32/g' *.make
make -f Tools.make $WIN_MKARGS clean
make -f Tools.make $WIN_MKARGS
cd ..
premake4 gmake
cd build
make -f Tools.make clean
make -f Tools.make
cd ..
cd java-putked
ant
