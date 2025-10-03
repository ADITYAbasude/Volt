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

## Windows first-time build note

If you're building on Windows for the first time (or using a Qt installation that CMake can't auto-detect), set the `CMAKE_PREFIX_PATH` environment variable to your Qt installation root before running the build script. Example (Git Bash / MSYS):

```bash
export CMAKE_PREFIX_PATH="/c/Qt/6.9.0/mingw_64"
./build.sh
```

This ensures CMake and the build script find Qt and QScintilla headers/libraries. If you still see a CMake error like:

```
QScintilla headers not found!
```

then either QScintilla isn't installed for your Qt toolchain or CMake is pointed at a different Qt than QScintilla was built for.

## Project Structure

- `src/main.cpp` - Main application code
- `src/CMakeLists.txt` - CMake configuration for source files
- `CMakeLists.txt` - Root CMake configuration
- `build.sh` - Build automation script for windows
- `linux_build.sh` - Build automation script for Linux
