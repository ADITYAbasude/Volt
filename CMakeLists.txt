cmake_minimum_required(VERSION 3.16)
project(Volt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set Qt's MinGW compiler and tools
set(CMAKE_C_COMPILER "C:/Qt/Tools/mingw1310_64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/Qt/Tools/mingw1310_64/bin/g++.exe")

# Set Qt path
set(CMAKE_PREFIX_PATH "C:/Qt/6.9.0/mingw_64")

# Disable stack protector for MinGW
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-stack-protector")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-stack-protector")

# Find Qt packages
find_package(Qt6 REQUIRED COMPONENTS 
    Core
    Gui
    Widgets
)
qt_standard_project_setup()

add_subdirectory(src)
