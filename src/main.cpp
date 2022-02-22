#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  if (WinDark::isDarkTheme())
    WinDark::setDark_qApp();

  MainWindow w;
  w.show();
  return a.exec();
}
