#!/bin/bash

rm -rf build
cmake -B build
cmake --build build
cd build
ctest
./chat_cli
git tag v02