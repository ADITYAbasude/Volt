cmake_minimum_required(VERSION 3.16)

project(Volt LANGUAGES CXX)

#sets C++ standard to 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#enable QTs automatic build tools
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

#disable stack protector for MinGW
if(MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-stack-protector")
endif()

#find Qt packages
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Svg)

qt_standard_project_setup()

#add the src directory to the build
add_subdirectory(src)
