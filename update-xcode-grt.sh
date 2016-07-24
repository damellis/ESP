#!/bin/bash
set -e

if [ -z "$1" ]
then
    SRC_DYLIB=~/repos/grt/build/tmp/libgrt.dylib
else
    SRC_DYLIB=~/repos/grt/build/$1/libgrt.dylib
fi

cp ${SRC_DYLIB} Xcode/ESP
install_name_tool -id @executable_path/../Frameworks/libgrt.dylib Xcode/ESP/libgrt.dylib
