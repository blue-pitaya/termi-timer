#!/usr/bin/env bash

set -e
cd "$(dirname "$0")"

cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
make
./stopwatch
