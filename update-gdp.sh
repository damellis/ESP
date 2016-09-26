#!/bin/bash
set -e

install_name_tool -id @executable_path/../Frameworks/libep.3.0.dylib  third-party/gdp/libs/libep.3.0.dylib
install_name_tool -id @executable_path/../Frameworks/libgdp.0.7.dylib third-party/gdp/libs/libgdp.0.7.dylib
