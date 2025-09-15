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

#find QScintilla
find_library(QSCINTILLA_LIBRARY
    NAMES qscintilla2_qt6 libqscintilla2_qt6
    PATHS /usr/lib /usr/local/lib
)
find_path(QSCINTILLA_INCLUDE_DIR
    NAMES Qsci/qsciscintilla.h
    PATHS /usr/include/qt6 /usr/local/include/qt6
)

if(NOT QSCINTILLA_LIBRARY OR NOT QSCINTILLA_INCLUDE_DIR)
    message(FATAL_ERROR "QScintilla library not found! Please ensure 'qscintilla-qt6' is installed.")
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
