#!/bin/bash

rm -rf build
cmake -B build
cmake --build build
cd build
ctest
./chat_cli
