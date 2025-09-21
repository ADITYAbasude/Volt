#!/bin/bash

cd "$(dirname "$0")"

mkdir -p build
cd build

echo "== Configuring CMake (if needed) =="

# Use CMAKE_PREFIX_PATH if set, otherwise let CMake find Qt automatically
if [ -n "$CMAKE_PREFIX_PATH" ]; then
    echo "Using CMAKE_PREFIX_PATH: $CMAKE_PREFIX_PATH"
else
    echo "CMAKE_PREFIX_PATH not set, relying on system Qt installation"
fi

# Use compilers from CMAKE_C_COMPILER/CMAKE_CXX_COMPILER if set
CMAKE_ARGS=""
if [ -n "$CMAKE_C_COMPILER" ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_C_COMPILER=$CMAKE_C_COMPILER"
fi
if [ -n "$CMAKE_CXX_COMPILER" ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_CXX_COMPILER=$CMAKE_CXX_COMPILER"
fi

cmake -G "MinGW Makefiles" $CMAKE_ARGS ..
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

echo "== Copying Qt DLLs =="
# Copy required Qt DLLs to the executable directory (only if QT_DIR is set)
if [ -n "$QT_DIR" ] && [ -d "$QT_DIR" ]; then
    echo "Copying DLLs from QT_DIR: $QT_DIR"
    DLL_DIR="dll"  # Store DLLs in a central location
    TARGET_DIR="."  # Root of build directory where Volt.exe is
    
    # First, ensure we have the DLLs in our dll directory
    mkdir -p "../$DLL_DIR"
    cp -f "$QT_DIR/bin/Qt6Core.dll" "../$DLL_DIR/" 2>/dev/null || echo "Warning: Could not copy Qt6Core.dll"
    cp -f "$QT_DIR/bin/Qt6Gui.dll" "../$DLL_DIR/" 2>/dev/null || echo "Warning: Could not copy Qt6Gui.dll"
    cp -f "$QT_DIR/bin/Qt6Widgets.dll" "../$DLL_DIR/" 2>/dev/null || echo "Warning: Could not copy Qt6Widgets.dll"
    cp -f "$QT_DIR/plugins/platforms/qwindows.dll" "../$DLL_DIR/" 2>/dev/null || echo "Warning: Could not copy qwindows.dll"

    # Then copy from our dll directory to the build directory
    echo "Copying DLLs to build directory..."
    cp -f "../$DLL_DIR/Qt6Core.dll" "$TARGET_DIR/" 2>/dev/null || true
    cp -f "../$DLL_DIR/Qt6Gui.dll" "$TARGET_DIR/" 2>/dev/null || true
    cp -f "../$DLL_DIR/Qt6Widgets.dll" "$TARGET_DIR/" 2>/dev/null || true
    cp -f "../$DLL_DIR/Qt6Svg.dll" "$TARGET_DIR/" 2>/dev/null || true
    
    # Copy qscintilla DLLs from dll directory of project root to build directory
    cp -f "../$DLL_DIR/qscintilla2_qt6.dll" "$TARGET_DIR/" 2>/dev/null || true
    
    # Create platforms directory and copy platform plugin
    mkdir -p "$TARGET_DIR/platforms"
    cp -f "../$DLL_DIR/qwindows.dll" "$TARGET_DIR/platforms/" 2>/dev/null || true
    
    # Copy install_context_menu.reg, install.bat, and uninstall.bat
    cp -f "../install_context_menu.reg" "$TARGET_DIR/" 2>/dev/null || true
    cp -f "../install.bat" "$TARGET_DIR/" 2>/dev/null || true
    cp -f "../uninstall.bat" "$TARGET_DIR/" 2>/dev/null || true
else
    echo "QT_DIR not set, skipping DLL copy. Set QT_DIR environment variable if you need Qt DLLs copied."
fi

echo "== Running Volt =="
./Volt.exe
