cmake_minimum_required(VERSION 3.5)

project(streamlinkerino VERSION 0.3 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/qt5app.rc")
    add_executable(qt5app main.cpp ${APP_ICON_RESOURCE_WINDOWS})

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

# SOURCES
set(PROJECT_SOURCES
    src/chatterino/chatterinojson.cpp
    src/main.cpp
    src/mainwindow.cpp
    src/submodules.cpp
    src/type.cpp
)
# UI
set(PROJECT_UI
    src/Forms/mainwindow.ui
    src/Forms/settingsDialog.ui
)
# HEADERS
set(PROJECT_HEADERS
    src/debug.h
    src/mainwindow.h
    src/submodules.h
    src/type.h
    src/chatterino/chatterinojson.h
    src/os/darkmode.h
    src/os/path.h
)
# HEADERS
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}

)
# CROSS-PLATFORM HEADERS
if(WIN32)
    {
    find_library(USER32 REQUIRED)
    include_directories(
        src/os/windows
    )
    }
endif (WIN32)
if(UNIX)
    {
    find_package(X11 REQUIRED)
    include_directories(
        src/os/linux
    )
    }
endif (UNIX)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(streamlinkerino
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET streamlinkerino APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(streamlinkerino SHARED
            ${PROJECT_SOURCES}
            ${PROJECT_UI}
            ${PROJECT_HEADERS}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(streamlinkerino
            ${PROJECT_SOURCES}
            ${PROJECT_UI}
            ${PROJECT_HEADERS}
        )
    endif()
endif()

if(WIN32)
    {
    target_link_libraries(streamlinkerino PRIVATE Qt${QT_VERSION_MAJOR}::Widgets USER32)
    }
endif (WIN32)
if(LINUX)
    {
    target_link_libraries(streamlinkerino PRIVATE Qt${QT_VERSION_MAJOR}::Widgets X11)
    }
endif (LINUX)
set_target_properties(streamlinkerino PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(streamlinkerino)
endif()
