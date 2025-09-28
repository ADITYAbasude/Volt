# Volt Qt Application

A simple Qt-based application demonstrating basic window creation and styling.

## Prerequisites

Before you can build and run this application, you need to have the following installed:

1. **Qt 6.9.0** or later
   - Download from [Qt's official website](https://www.qt.io/download)
   - Make sure to install the MinGW 64-bit compiler during Qt installation

2. **CMake** (3.16 or higher)
   - Can be installed with Qt or separately from [CMake's website](https://cmake.org/download/)

3. **MinGW-w64**
   - This should be installed as part of your Qt installation

## Building the Application

### Using the Build Script

1. Open a terminal in the project root directory
2. Run the build script:
   ```bash
   ./build.sh
   ```
   This will:
   - Create a build directory
   - Configure CMake
   - Build the project
   - Copy necessary Qt DLLs
   - Run the application

### Manual Build

If you prefer to build manually:

1. Create and enter the build directory:
   ```bash
   mkdir build
   cd build
   ```

2. Configure CMake:
   ```bash
   cmake -G "MinGW Makefiles" ..
   ```

3. Build the project:
   ```bash
   cmake --build .
   ```

4. Copy required Qt DLLs to the executable directory:
   ```bash
   cp /c/Qt/6.9.0/mingw_64/bin/Qt6Core.dll src/
   cp /c/Qt/6.9.0/mingw_64/bin/Qt6Gui.dll src/
   cp /c/Qt/6.9.0/mingw_64/bin/Qt6Widgets.dll src/
   mkdir -p src/platforms
   cp /c/Qt/6.9.0/mingw_64/plugins/platforms/qwindows.dll src/platforms/
   ```

5. Run the application:
   ```bash
   cd src
   ./Volt.exe
   ```

## Project Structure

- `src/main.cpp` - Main application code
- `src/CMakeLists.txt` - CMake configuration for source files
- `CMakeLists.txt` - Root CMake configuration
- `build.sh` - Build automation script

## Troubleshooting

1. **Missing DLL Errors**
   - If you see errors about missing DLLs, make sure all required Qt DLLs are copied to the same directory as the executable
   - Required DLLs:
     - Qt6Core.dll
     - Qt6Gui.dll
     - Qt6Widgets.dll
     - platforms/qwindows.dll

2. **Build Errors**
   - Ensure Qt is properly installed and the PATH environment variable includes the Qt and MinGW binary directories
   - Make sure CMake can find Qt by setting CMAKE_PREFIX_PATH correctly in CMakeLists.txt

