cmake_minimum_required(VERSION 3.16)
project(Volt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.9.0/mingw_64")

find_package(Qt6 REQUIRED COMPONENTS 
    Core
    Gui
    Widgets
)

add_subdirectory(src)
