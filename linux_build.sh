#!/bin/sh

#go to project root directory
cd "$(dirname "$0")"

#makes the script exit on error
set -e

#configure and build
echo "== Configuring and Building Project =="
cmake -S . -B build -G Ninja
cmake --build build --parallel

echo "== Build completed successfully =="
