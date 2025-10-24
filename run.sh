#!/usr/bin/env bash

set -e
cd "$(dirname "$0")"

mkdir -p build
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
make
./termi-timer
