#!/bin/bash

install_name_tool -id @executable_path/../Frameworks/libgrt.dylib third-party/grt/build/tmp/libgrt.dylib
cp third-party/grt/build/tmp/libgrt.dylib Xcode/ESP