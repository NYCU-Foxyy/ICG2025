#!/bin/bash

cd "$(dirname "$(realpath $0)")"
cmake -S . -B build -G "Unix Makefiles"
cd build
make install/local # the output file will be in ./build/src/
