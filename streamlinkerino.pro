QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/submodules.cpp \
    src/windark.cpp \
    src/windows_interface.cpp

HEADERS += \
    src/debugging.h \
    src/mainwindow.h \
    src/submodules.h \
    src/windark.h \
    src/windows_interface.h

FORMS += \
    src/Forms/mainwindow.ui \
    src/Forms/submodulesDialog.ui

LIBS += -luser32
