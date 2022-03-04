#pragma once
#include <QApplication>

#include "mainwindow.h"
#include "os/darkmode.h"

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
#ifdef Q_OS_WIN
  if (WinDark::isDarkTheme())
    WinDark::setDark_qApp();
#endif
  MainWindow w;
  w.show();
  return a.exec();
}
