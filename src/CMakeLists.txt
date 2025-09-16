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

if(DEFINED QSCINTILLA_ROOT)
    set(_qsc_root "${QSCINTILLA_ROOT}")
else()
    set(_qsc_root "")
endif()

if(WIN32)
    if(NOT _qsc_root AND EXISTS "${CMAKE_SOURCE_DIR}/dev/QScintilla/QScintilla_src-2.14.1/release")
        set(_qsc_root "${CMAKE_SOURCE_DIR}/dev/QScintilla/QScintilla_src-2.14.1/release")
    endif()

    set(_qsc_hints)
    if(_qsc_root)
        list(APPEND _qsc_hints ${_qsc_root})
    endif()
    if(DEFINED Qt6_DIR)
        list(APPEND _qsc_hints ${Qt6_DIR}/../../../)
    endif()
    if(DEFINED CMAKE_PREFIX_PATH)
        list(APPEND _qsc_hints ${CMAKE_PREFIX_PATH})
    endif()

    find_path(QSCINTILLA_INCLUDE_DIR
        NAMES Qsci/qsciscintilla.h
        HINTS ${_qsc_hints}
        PATHS ${_qsc_root}/include ${_qsc_root}/src
        PATH_SUFFIXES qscintilla2 Qsci
    )

    find_library(QSCINTILLA_LIBRARY
        NAMES qscintilla2_qt6 qscintilla2_qt6d libqscintilla2_qt6
        HINTS ${_qsc_hints}
        PATHS ${_qsc_root}/lib ${Qt6_DIR}/../../../lib
        PATH_SUFFIXES lib
    )

else()
    find_path(QSCINTILLA_INCLUDE_DIR
        NAMES Qsci/qsciscintilla.h
        PATHS
            ${_qsc_root}/include
            /usr/include/qt6
            /usr/local/include/qt6
            /usr/include/x86_64-linux-gnu/qt6
            /usr/include/qscintilla2
            /usr/local/include/qscintilla2
            ${Qt6_DIR}/../../../include
        PATH_SUFFIXES
            qscintilla2
            Qsci
    )

    find_library(QSCINTILLA_LIBRARY
        NAMES qscintilla2_qt6 qscintilla2_qt6d
        PATHS
            ${_qsc_root}/lib
            /usr/lib
            /usr/local/lib
            ${Qt6_DIR}/../../../lib
    )
endif()

if(NOT QSCINTILLA_INCLUDE_DIR)
    message(FATAL_ERROR "QScintilla headers not found! Please install libqscintilla2-qt6-dev")
else()
    message(STATUS "Found QScintilla headers: ${QSCINTILLA_INCLUDE_DIR}")
endif()

if(NOT QSCINTILLA_LIBRARY)
    message(WARNING "QScintilla library not foundm build may fail to link.")
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

#link the necessary Qt and QScintilla libraries to the executable
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Svg
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
