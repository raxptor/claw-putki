#/bin/bash
echo "Making ship64 build and copying files from out."

set -e 1
cd build
make config=ship64 -f Claw.make -j2
make config=ship64 -f Tools.make -j2
cd ..
rm -rf out/x64-default
./build/claw-databuilder --platform x64
rm -rf ship
mkdir ship
cp build/claw ship
strip build/claw
cp -r out/x64-default/* ship
find . -name "*.manifest" -exec rm {} \;
find . -name "*.ptr" -exec rm {} \;

echo "Done."
