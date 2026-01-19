cmake_minimum_required(VERSION 3.16)
project(tikz_editor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 (fallback to Qt5)
find_package(Qt6 COMPONENTS Core Gui Widgets QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Core Gui Widgets)
    set(QT_VERSION 5)
else()
    set(QT_VERSION 6)
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/data/point.cpp
    src/data/line.cpp
    src/data/polygon.cpp
    src/data/curve.cpp
    src/data/ellipse.cpp
    src/data/document.cpp
    src/gui/mainwindow.cpp
    src/gui/canvas.cpp
    src/gui/canvasview.cpp
    src/gui/toolpalette.cpp
    src/gui/colorpalette.cpp
    src/gui/pointitem.cpp
    src/gui/lineitem.cpp
    src/gui/polygonitem.cpp
    src/gui/curveitem.cpp
    src/gui/ellipseitem.cpp
    src/gui/propertiespanel.cpp
    src/gui/contexttoolbar.cpp
)

set(HEADERS
    src/data/point.h
    src/data/line.h
    src/data/polygon.h
    src/data/curve.h
    src/data/ellipse.h
    src/data/document.h
    src/gui/mainwindow.h
    src/gui/canvas.h
    src/gui/canvasview.h
    src/gui/toolpalette.h
    src/gui/colorpalette.h
    src/gui/pointitem.h
    src/gui/lineitem.h
    src/gui/polygonitem.h
    src/gui/curveitem.h
    src/gui/ellipseitem.h
    src/gui/propertiespanel.h
    src/gui/contexttoolbar.h
    src/common.h
)

add_executable(tikz_editor ${SOURCES} ${HEADERS})

target_include_directories(tikz_editor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/data
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui
)

if(QT_VERSION EQUAL 6)
    target_link_libraries(tikz_editor PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
else()
    target_link_libraries(tikz_editor PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets)
endif()

# macOS bundle
if(APPLE)
    set_target_properties(tikz_editor PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.tikzeditor"
        MACOSX_BUNDLE_BUNDLE_NAME "TikZ Editor"
    )
endif()

# Install
install(TARGETS tikz_editor
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
