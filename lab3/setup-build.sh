#!/bin/sh -ex

# Create and enter a directory for build products
mkdir -p build
cd build

# Use CMake to configure the build
cmake -G Ninja ..

#echo "OK, the build is ready. cd build and run ninja next."
ninja
echo "ok"
