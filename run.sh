#!/bin/bash

set -e

echo "Staring Cmake build..."

cmake --build build

echo "Running Executable...."
cd build
bin/quarks sample/test.qs