#!/bin/bash

# Navigate to project root
cd "$(dirname "$0")"

# Remove old build
rm -rf build

# Create build directory
mkdir build
cd build

# Configure CMake
echo "== Configuring CMake =="
cmake -G "MinGW Makefiles" ..
if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

# Build the project
echo "== Building Project =="
cmake --build .
if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "== Copying Qt DLLs =="
# Copy required Qt DLLs to the executable directory
QT_DIR="C:/Qt/6.9.0/mingw_64"
TARGET_DIR="src"

# Copy essential Qt DLLs
cp "$QT_DIR/bin/Qt6Core.dll" "$TARGET_DIR/"
cp "$QT_DIR/bin/Qt6Gui.dll" "$TARGET_DIR/"
cp "$QT_DIR/bin/Qt6Widgets.dll" "$TARGET_DIR/"

# Create platforms directory and copy platform plugin
mkdir -p "$TARGET_DIR/platforms"
cp "$QT_DIR/plugins/platforms/qwindows.dll" "$TARGET_DIR/platforms/"

echo "== Running Volt =="
cd src
./Volt.exe
