#!/bin/bash
set -e

install_name_tool -id @executable_path/../Frameworks/libep.so.3.0  third-party/gdp/libs/libep.so.3.0 
install_name_tool -id @executable_path/../Frameworks/libgdp.so.0.7 third-party/gdp/libs/libgdp.so.0.7
