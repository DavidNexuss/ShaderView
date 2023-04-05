#!/bin/sh
cmake -S . -B build
cd build
make -j$(nproc)
cd ..
cp build/shaderview .
