#define source files and header files
set(SOURCES
    main.cpp
    ui/menubar/FileMenu.cpp
    ui/MainWindow.cpp
    ui/statusbar/StatusBar.cpp
    ui/sidebar/Sidebar.cpp
    ui/sidebar/CustomTreeView.cpp
    ui/components/IconButton.cpp
    ui/components/FilledColorButton.cpp
    ui/utils/IconUtils.cpp
    editor/CodeEditor.cpp
    editor/Minimap.cpp
    themes/Theme.cpp
    styles/StyleManager.cpp
    logging/VoltLogger.cpp
)

set(HEADERS
    ui/menubar/FileMenu.h
    ui/MainWindow.h
    ui/statusbar/StatusBar.h
    ui/sidebar/Sidebar.h
    ui/sidebar/CustomTreeView.h
    ui/components/IconButton.h
    ui/components/FilledColorButton.h
    ui/utils/IconUtils.h
    editor/CodeEditor.h
    editor/Minimap.h
    themes/Theme.h
    styles/StyleManager.h
    logging/VoltLogger.h
)

qt_add_resources(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources/app.qrc)

# Cross-platform QScintilla detection using standard CMake mechanisms
# Relies on CMAKE_PREFIX_PATH and system package managers
# Use -DQSCINTILLA_ROOT=<path> to override if needed

# Allow overriding with -DQSCINTILLA_ROOT=path for local builds
if(DEFINED QSCINTILLA_ROOT)
    message(STATUS "Using QSCINTILLA_ROOT override: ${QSCINTILLA_ROOT}")
    set(_qsc_hints ${QSCINTILLA_ROOT})
else()
    set(_qsc_hints "")
endif()

# Find QScintilla headers
find_path(QSCINTILLA_INCLUDE_DIR
    NAMES Qsci/qsciscintilla.h
    HINTS ${_qsc_hints}
    PATH_SUFFIXES include qscintilla2 Qsci
    DOC "QScintilla include directory"
)

# Find QScintilla library
find_library(QSCINTILLA_LIBRARY
    NAMES qscintilla2_qt6 qscintilla2_qt6d libqscintilla2_qt6
    HINTS ${_qsc_hints}
    PATH_SUFFIXES lib
    DOC "QScintilla library"
)

if(NOT QSCINTILLA_INCLUDE_DIR)
    message(FATAL_ERROR 
        "QScintilla headers not found!\n"
        "Please install QScintilla development package:\n"
        "  Linux: sudo apt install libqscintilla2-qt6-dev\n"
        "  Windows: Install via package manager or set CMAKE_PREFIX_PATH\n"
        "  Or use: -DQSCINTILLA_ROOT=<path-to-qscintilla-install>"
    )
else()
    message(STATUS "Found QScintilla headers: ${QSCINTILLA_INCLUDE_DIR}")
endif()

if(NOT QSCINTILLA_LIBRARY)
    message(FATAL_ERROR 
        "QScintilla library not found!\n"
        "Please install QScintilla:\n"
        "  Linux: sudo apt install libqscintilla2-qt6-dev\n"
        "  Windows: Install via package manager or set CMAKE_PREFIX_PATH\n"
        "  Or use: -DQSCINTILLA_LIBRARY=<path-to-library>"
    )
else()
    message(STATUS "Found QScintilla library: ${QSCINTILLA_LIBRARY}")
endif()

#create the executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

#copy theme files to the build directory after the project is built
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/themes/themes"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/themes/themes"
        "${CMAKE_BINARY_DIR}/themes/themes"
)

#link the necessary Qt and QScintila libraries to the executable
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Svg
    ${QSCINTILLA_LIBRARY}
)

#set include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${QSCINTILLA_INCLUDE_DIR}
)

#set properties for the executable handling different OSes
set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
