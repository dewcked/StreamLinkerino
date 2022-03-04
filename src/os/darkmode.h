#pragma once
#include <QApplication>

#ifdef Q_OS_WIN
#include "windows/windark.h"
#else
// #include "linux/linuxdark.h"?
#endif
