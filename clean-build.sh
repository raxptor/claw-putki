#!/bin/sh
set -e
echo "Erasing _gen folders..."
rm -rf _gen/
rm -rf ext/ccg-ui/_gen
rm -rf ext/kosmos/_gen
pushd .
pushd .
cd ext/putki/compiler
premake4 gmake
cd build
make
popd
ext/putki/compiler/build/compiler
premake4 gmake
./make.sh -e -j2
popd
rm -rf out
./build/claw-databuilder
echo "Done!"
