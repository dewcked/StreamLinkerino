cmake_minimum_required(VERSION 3.5)

project(streamlinkerino VERSION 1.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# icon..?
set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/icon.qrc")

if(WIN32)
    set(CMAKE_PREFIX_PATH "C:/Qt/6.3.0/msvc2019_64/lib/cmake/")
endif (WIN32)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network REQUIRED)

# UI
set(CMAKE_AUTOUIC_SEARCH_PATHS src/Forms)

# SOURCES
set(PROJECT_SOURCES
    src/chatterino/chatterinojson.cpp
    src/main.cpp
    src/mainwindow.cpp
    src/settings.cpp
    src/settingsdialog.cpp
    src/type.cpp
)
# HEADERS
set(PROJECT_HEADERS
    src/chatterino/chatterinojson.h
    src/debug.h
    src/mainwindow.h
    src/os/darkmode.h
    src/os/path.h
    src/settings.h
    src/settingsdialog.h
    src/type.h
)
# HEADERS
include_directories(
    src/os
    src/chatterino
)
#     find_library(USER32 REQUIRED)
# CROSS-PLATFORM HEADERS
if(WIN32)
    set(OS_PROJECT_SOURCES
        src/os/windows/native.cpp
        src/os/windows/path.cpp
        src/os/windows/windark.cpp)
    set(OS_PROJECT_HEADERS
        src/os/windows/native.h
        src/os/windows/windark.h)
    find_library(USER32 REQUIRED)
    include_directories(
        src/os/windows
    )
endif (WIN32)
if(UNIX)
    set(OS_PROJECT_SOURCES
        src/os/linux/native.cpp
        src/os/linux/path.cpp)
    set(OS_PROJECT_HEADERS
        src/os/linux/native.h)
    find_package(X11 REQUIRED)
    include_directories(
        src/os/linux
    )
endif (UNIX)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(streamlinkerino
        MANUAL_FINALIZATION
        ${OS_PROJECT_SOURCES}
        ${OS_PROJECT_HEADERS}
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET streamlinkerino APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(streamlinkerino SHARED
            ${OS_PROJECT_SOURCES}
            ${OS_PROJECT_HEADERS}
            ${PROJECT_SOURCES}
            ${PROJECT_HEADERS}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(streamlinkerino
            ${OS_PROJECT_SOURCES}
            ${OS_PROJECT_HEADERS}
            ${PROJECT_SOURCES}
            ${PROJECT_HEADERS}
        )
    endif()
endif()

if(WIN32)
    target_link_libraries(streamlinkerino PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network USER32)
endif (WIN32)
if(UNIX)
    target_link_libraries(streamlinkerino PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network X11)
endif (UNIX)
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
