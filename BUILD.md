# Build Instructions for Volt

Volt is a Qt6 C++ desktop application that requires Qt6 and QScintilla to build.

## Prerequisites

### Linux (Ubuntu/Debian)
```bash
# Install Qt6 and QScintilla development packages
sudo apt update
sudo apt install qt6-base-dev libqscintilla2-qt6-dev cmake build-essential

# Optional: Install additional Qt6 modules
sudo apt install qt6-svg-dev
```

### Linux (Fedora/CentOS/RHEL)
```bash
# Install Qt6 and QScintilla development packages
sudo dnf install qt6-qtbase-devel qscintilla-qt6-devel cmake gcc-c++

# Optional: Install additional Qt6 modules  
sudo dnf install qt6-qtsvg-devel
```

### Windows

#### Option 1: Using vcpkg (Recommended)
```bash
# Install vcpkg package manager
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.bat

# Install Qt6 and QScintilla
./vcpkg install qt6[core,gui,widgets,svg] qscintilla[qt6]

# Set CMAKE_PREFIX_PATH when building
set CMAKE_PREFIX_PATH=C:\path\to\vcpkg\installed\x64-windows
```

#### Option 2: Qt Installer
1. Download and install Qt from [qt.io](https://www.qt.io/download)
2. Install Qt 6.5+ with MinGW compiler
3. Install QScintilla:
   ```bash
   # Download QScintilla source from: https://www.riverbankcomputing.com/software/qscintilla/download
   # Extract and build with Qt's qmake:
   cd QScintilla_src-X.X.X/src
   qmake qscintilla.pro
   mingw32-make
   mingw32-make install
   ```

### macOS
```bash
# Using Homebrew
brew install qt6 qscintilla2

# Using MacPorts
sudo port install qt6-qtbase +universal qscintilla-qt6
```

## Building

### Method 1: Using build.sh (Windows/Linux)
```bash
# Set CMAKE_PREFIX_PATH if Qt/QScintilla are not in standard locations
export CMAKE_PREFIX_PATH="/path/to/qt6:/path/to/qscintilla"

# For Windows, optionally set compilers
export CMAKE_C_COMPILER="C:/Qt/Tools/mingw1310_64/bin/gcc.exe"
export CMAKE_CXX_COMPILER="C:/Qt/Tools/mingw1310_64/bin/g++.exe"

# Build and run
./build.sh
```

### Method 2: Manual CMake
```bash
# Configure
cmake -S . -B build -G "Ninja" \
  -DCMAKE_PREFIX_PATH="/path/to/qt6;/path/to/qscintilla" \
  -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --parallel

# Run
./build/Volt
```

### Method 3: Using custom QScintilla location
```bash
# If QScintilla is installed in a custom location
cmake -S . -B build \
  -DQSCINTILLA_ROOT="/path/to/qscintilla/install" \
  -DCMAKE_PREFIX_PATH="/path/to/qt6"

cmake --build build --parallel
```

## Environment Variables

- `CMAKE_PREFIX_PATH`: Paths to Qt6 and QScintilla installations (colon/semicolon separated)
- `CMAKE_C_COMPILER`: C compiler to use (optional)
- `CMAKE_CXX_COMPILER`: C++ compiler to use (optional)
- `QT_DIR`: Qt installation directory (for DLL copying on Windows)
- `QSCINTILLA_ROOT`: Custom QScintilla installation path (override)

## Examples

### Linux with system packages
```bash
# No additional setup needed if packages are installed
./build.sh
```

### Windows with Qt Installer
```bash
# Set paths to your Qt installation
export CMAKE_PREFIX_PATH="C:/Qt/6.9.0/mingw_64"
export CMAKE_CXX_COMPILER="C:/Qt/Tools/mingw1310_64/bin/g++.exe"
export QT_DIR="C:/Qt/6.9.0/mingw_64"
./build.sh
```

### macOS with Homebrew
```bash
export CMAKE_PREFIX_PATH="/opt/homebrew/opt/qt6:/opt/homebrew/opt/qscintilla2"
./build.sh
```

## Troubleshooting

### QScintilla not found
If you get "QScintilla headers/library not found":

1. **Install system package** (preferred):
   ```bash
   # Linux
   sudo apt install libqscintilla2-qt6-dev
   
   # Windows with vcpkg
   vcpkg install qscintilla[qt6]
   ```

2. **Set CMAKE_PREFIX_PATH**:
   ```bash
   export CMAKE_PREFIX_PATH="/path/to/qscintilla/install:$CMAKE_PREFIX_PATH"
   ```

3. **Use QSCINTILLA_ROOT override**:
   ```bash
   cmake -DQSCINTILLA_ROOT="/path/to/qscintilla" ...
   ```

### Compiler/Linker errors
- Ensure your compiler matches the Qt build (e.g., use Qt's MinGW on Windows)
- Set `CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER` explicitly
- Check that all dependencies are built with the same toolchain

### Missing DLLs (Windows)
- Set `QT_DIR` environment variable for automatic DLL copying
- Or manually copy required DLLs to the build directory:
  - Qt6Core.dll, Qt6Gui.dll, Qt6Widgets.dll, Qt6Svg.dll
  - qscintilla2_qt6.dll
  - platforms/qwindows.dll

## Contributing

When submitting pull requests:
- Ensure builds work on multiple platforms
- Don't hardcode paths in CMakeLists.txt or build scripts
- Use `CMAKE_PREFIX_PATH` and system packages when possible
- Test with clean environment (no custom paths set)