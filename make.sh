#!/bin/bash
set -e
cd build
make -f Claw.make config=release64 $@
make -f Tools.make config=release32  $@
