set(SOURCES
    main.cpp
    ui/menubar/FileMenu.cpp
    ui/MainWindow.cpp
    ui/statusbar/StatusBar.cpp
    editor/CodeEditor.cpp
    themes/Theme.cpp
    styles/StyleManager.cpp
    )
    
set(HEADERS
    ui/menubar/FileMenu.h
    ui/MainWindow.h
    ui/statusbar/StatusBar.h
    editor/CodeEditor.h
    themes/Theme.h
    styles/StyleManager.h
    )

# Resource files
qt_add_resources(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources/app.qrc)

# Find Qt installation
set(Qt6_DIR "C:/Qt/6.9.0/mingw_64/lib/cmake/Qt6")

# Find QScintilla
set(QSCINTILLA_ROOT "C:/Qt/6.9.0/mingw_64")  # Adjust this path to where QScintilla was installed
find_library(QSCINTILLA_LIBRARY
    NAMES qscintilla2_qt6 qscintilla2_qt6d
    PATHS ${QSCINTILLA_ROOT}/lib
)
find_path(QSCINTILLA_INCLUDE_DIR
    NAMES Qsci/qsciscintilla.h
    PATHS ${QSCINTILLA_ROOT}/include
)

if(NOT QSCINTILLA_LIBRARY OR NOT QSCINTILLA_INCLUDE_DIR)
    message(FATAL_ERROR "QScintilla library not found!")
endif()

# Add Qt MOC and UIC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Create executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# Copy theme files to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/themes/themes"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/themes/themes"
        "${CMAKE_BINARY_DIR}/themes/themes"
)

# Add include directories for the main executable
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${QSCINTILLA_INCLUDE_DIR}
)

# Link Qt libraries and QScintilla
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    ${QSCINTILLA_LIBRARY}
)

# Set executable properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
