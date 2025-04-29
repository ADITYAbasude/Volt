set(SOURCES
    main.cpp
    ui/menubar/FileMenu.cpp
    ui/MainWindow.cpp
    ui/statusbar/StatusBar.cpp
    )
    
set(HEADERS
    ui/menubar/FileMenu.h
    ui/MainWindow.h
    ui/statusbar/StatusBar.h
    )

# Resource files
qt_add_resources(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources/app.qrc)

# Create executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Set executable properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
