#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QMainWindow>
#include <QOperatingSystemVersion>
#include <QSettings>
#include <QWidget>

#include "debugging.h"
#include "submodules.h"
#include "windows_interface.h"
#include <QDebug>
#include <QFileSystemWatcher>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QProcess>
#include <QResizeEvent>
#include <QTimer>
#include <QWindow>
#include <thread>
// test
#include <cstdio>
#include <tlhelp32.h>
#include <windows.h>

QT_BEGIN_NAMESPACE
namespace Ui
{
  class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  // Darktheme
  void setupDarktheme();
  void setupUI();
  void setupCustomContextMenu();
  bool isWindowsDarkthemeAvailable();
  bool isWindowsDarktheme();
  // Context menu
  void showContextMenu(const QPoint &pos);
  // Main logic
  void setupChatterino();
  void setupStreamlink();
  void setupMVP();
  QString loadCurrentChannel();
  void initialize();
  void refreshStream();
  void loadStream();
  void quit();

protected:
  void resizeEvent(QResizeEvent *event) override;
  bool eventFilter(QObject *obj, QEvent *event);

private:
  Ui::MainWindow *ui;
  // Main logic
  void resizeEmbeds();
  QString generateStatusHTML(bool bPrerollAds = false);
  void quitChatterino();
  void quitChatterino_();
  void initChatterinoFocus();
  void simulateMouse(unsigned int pos, INPUT *inputs);

  WindowsInterface _WMP;
  Submodules::SubmodulesDialog *_Submodules;
  QWidget *_chatContainer;
  QWidget *_mpvContainer;
  QMenu _contextMenu;
  QList<QAction *> _contextMenuActions;
  QList<QProcess *> _pStreamlinkProcess;
  QProcess *_pChatterinoProcess;
  QWindow *_wChatterinowindow;
  QTimer *monitorChatterinoWidth = new QTimer;
  QTimer *monitorChatterinoHeight = new QTimer;
  int _chatterinoWidth;
  int _chatterinoHeight;
  bool _lChatterinoLock = false;
  bool _bChatterinoEmbedded = false;
  QString _cChatChannel;
  unsigned long _mpvContainerWID;
  bool _bStreamlinkProcessSelector = false;
  bool _bStreamlinkAllowSwitching = false;
  bool _bDebug = true;
};
#endif // MAINWINDOW_H
