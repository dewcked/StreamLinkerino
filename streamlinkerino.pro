QT       += core gui \
    network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

SOURCES += \
    src/chatterino/chatterinojson.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/settings.cpp \
    src/settingsdialog.cpp \
    src/type.cpp

HEADERS += \
    src/chatterino/chatterinojson.h \
    src/debug.h \
    src/mainwindow.h \
    src/os/darkmode.h \
    src/os/path.h \
    src/settings.h \
    src/settingsdialog.h \
    src/type.h

FORMS += \
    src/Forms/mainwindow.ui \
    src/Forms/settingsDialog.ui

LIBS += -luser32

linux-g++ | linux-g++-64 | linux-g++-32 {
    INCLUDEPATH += \
        src/os \
        src/os/linux \
        src/chatterino

    SOURCES += \
    src/os/linux/native.cpp \
    src/os/linux/path.cpp

    HEADERS += \
    src/os/linux/native.h
}

win32 {
    INCLUDEPATH += \
        src/os \
        src/os/windows \
        src/chatterino

    SOURCES += \
    src/os/windows/native.cpp \
    src/os/windows/path.cpp \
        src/os/windows/windark.cpp

    HEADERS += \
    src/os/windows/native.h \
        src/os/windows/windark.h

    RC_ICONS = resource/Streamlinkerinoicon.ico
}

DISTFILES += \
    CMakeLists.txt \

RESOURCES += \
    icon.qrc
