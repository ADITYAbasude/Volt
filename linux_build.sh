#!/bin/bash

#go to project root directory
cd "$(dirname "$0")"

#create build directory if it doesn't exist
mkdir -p build
cd build

#configure CMake
echo "== Configuring CMake =="
#set CMAKE_PREFIX_PATH to help CMake find QT 6
cmake -DCMAKE_PREFIX_PATH=$(qmake6 -query QT_INSTALL_PREFIX) ..
if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

#build the editor
echo "== Building Project =="
cmake --build . --parallel $(nproc)
if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi
