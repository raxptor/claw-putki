#!/bin/bash
set -e

pushd .
cd ext/putki/putked/interopdll
premake5 --os=windows gmake
make -f interopjava.make CC=x86_64-w64-mingw32-gcc CXX=x86_64-w64-mingw32-g++ AR=x86_64-w64-mingw32-ar RANLIB=x86_64-wi64-mingw32-ranlib CXXFLAGS=-std=c++11 
premake5 gmake
make -f interopjava.make
sed -i .bak 's/\-Wl\,\-x//g' *.make
popd

premake5 --os=windows gmake
cd build
sed -i .bak 's/lws2_32\.lib/lws2_32/g' *.make
make -f Tools.make CC=x86_64-w64-mingw32-gcc CXX=x86_64-w64-mingw32-g++ AR=x86_64-w64-mingw32-ar RANLIB=x86_64-wi64-mingw32-ranlib CXXFLAGS=-std=c++11 clean
make -f Tools.make CC=x86_64-w64-mingw32-gcc CXX=x86_64-w64-mingw32-g++ AR=x86_64-w64-mingw32-ar RANLIB=x86_64-wi64-mingw32-ranlib CXXFLAGS=-std=c++11 -j3 
cd ..
premake4 gmake
cd build
make -f Tools.make clean
make
cd java-putked
ant
