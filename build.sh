#!/bin/bash

cd "$(dirname "$0")"

mkdir -p build
cd build

echo "== Configuring CMake (if needed) =="
cmake -G "MinGW Makefiles" ..
if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

# Build the project
echo "== Building Project =="
cmake --build . --parallel
if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "== Copying Qt DLLs =="
QT_DIR="C:/Qt/6.9.0/mingw_64"
DLL_DIR="dll" 
TARGET_DIR="."  

mkdir -p "../$DLL_DIR"
cp -f "$QT_DIR/bin/Qt6Core.dll" "../$DLL_DIR/"
cp -f "$QT_DIR/bin/Qt6Gui.dll" "../$DLL_DIR/"
cp -f "$QT_DIR/bin/Qt6Widgets.dll" "../$DLL_DIR/"
cp -f "$QT_DIR/plugins/platforms/qwindows.dll" "../$DLL_DIR/"

# copy from our dll directory to the build directory
echo "Copying DLLs to build directory..."
cp -f "../$DLL_DIR/Qt6Core.dll" "$TARGET_DIR/"
cp -f "../$DLL_DIR/Qt6Gui.dll" "$TARGET_DIR/"
cp -f "../$DLL_DIR/Qt6Widgets.dll" "$TARGET_DIR/"
cp -f "../$DLL_DIR/Qt6Svg.dll" "$TARGET_DIR/"

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
