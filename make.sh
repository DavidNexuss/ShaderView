#!/bin/sh
cmake -S . -B build
cd build
make -j4
cd ..
cp build/shaderview .
