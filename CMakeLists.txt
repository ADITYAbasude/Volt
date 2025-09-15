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

#find QScintilla library
find_library(QSCINTILLA_LIBRARY
    NAMES qscintilla2_qt6 libqscintilla2_qt6
)

if(NOT QSCINTILLA_LIBRARY)
    message(FATAL_ERROR "QScintilla library not found. Please install libqscintilla2-qt6-dev or equivalent package")
endif()

message(STATUS "Found QScintilla: ${QSCINTILLA_LIBRARY}")

#add the src directory to the build
add_subdirectory(src)
