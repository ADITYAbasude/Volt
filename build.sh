#!/bin/bash

cd "$(dirname "$0")"

mkdir -p build
cd build

echo "== Configuring CMake (if needed) =="

# Set CMAKE_PREFIX_PATH to help find Qt and QScintilla
QT_DIR="${QT_DIR:-C:/Qt/6.9.0/mingw_64}"
export CMAKE_PREFIX_PATH="$QT_DIR"

# Use Qt's MinGW compilers to avoid ABI mismatch
QT_MINGW="${QT_MINGW:-C:/Qt/Tools/mingw1310_64/bin}"
if [ -d "$QT_MINGW" ]; then
    export PATH="$QT_MINGW:$PATH"
    CMAKE_COMPILER_ARGS="-DCMAKE_C_COMPILER=$QT_MINGW/gcc.exe -DCMAKE_CXX_COMPILER=$QT_MINGW/g++.exe"
else
    CMAKE_COMPILER_ARGS=""
fi

cmake -G "MinGW Makefiles" $CMAKE_COMPILER_ARGS ..
if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

# Build the project
echo "== Building Project =="
cmake --build . --parallel 4 
if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi


QT_DIR="C:/Qt/6.9.0/mingw_64"
DLL_DIR="dll"  
TARGET_DIR="."

# First, ensure we have the DLLs in our dll directory
mkdir -p "../$DLL_DIR"
cp -f "$QT_DIR/bin/Qt6Core.dll" "../$DLL_DIR/"
cp -f "$QT_DIR/bin/Qt6Gui.dll" "../$DLL_DIR/"
cp -f "$QT_DIR/bin/Qt6Widgets.dll" "../$DLL_DIR/"
cp -f "$QT_DIR/plugins/platforms/qwindows.dll" "../$DLL_DIR/"

# Then copy from our dll directory to the build directory
echo "Copying DLLs to build directory..."
cp -f "../$DLL_DIR/Qt6Core.dll" "$TARGET_DIR/"
cp -f "../$DLL_DIR/Qt6Gui.dll" "$TARGET_DIR/"
cp -f "../$DLL_DIR/Qt6Widgets.dll" "$TARGET_DIR/"
cp -f "../$DLL_DIR/Qt6Svg.dll" "$TARGET_DIR/"

# Copy qscintilla DLLs from dll directory of project root to build directory
cp -f "../$DLL_DIR/qscintilla2_qt6.dll" "$TARGET_DIR/"

# Create platforms directory and copy platform plugin
mkdir -p "$TARGET_DIR/platforms"
cp -f "../$DLL_DIR/qwindows.dll" "$TARGET_DIR/platforms/"

# Copy install_context_menu.reg, install.bat, and uninstall.bat
cp -f "../install_context_menu.reg" "$TARGET_DIR/"
cp -f "../install.bat" "$TARGET_DIR/"
cp -f "../uninstall.bat" "$TARGET_DIR/" 

echo "== Running Volt =="
./Volt.exe
